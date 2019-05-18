// (C) 2019 NYSE | Intercontinental Exchange
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/lpool.cpp
//

#include <sys/resource.h>
#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

static void MarkOrCheckMem(void *mem, int n, bool mark) {
    srandom((int)(i64)mem);
    int i=0;
    for (; i+4 <= n; i += 4) {
        if (mark) {
            *(u32*)((u8*)mem+i) = random();
        } else {
            vrfy_(*(u32*)((u8*)mem+i) == random());
        }
    }
    for (; i+1 <= n; i += 1) {
        if (mark) {
            *((u8*)mem+i) = random();
        } else {
            vrfy_(*((u8*)mem+i) == random());
        }
    }
}

// -----------------------------------------------------------------------------

// Limit both AS (Address Space) size and Data (Stack + Heap) size
// AS is for mmaps
static void LimitMem(i64 size) {
    struct rlimit rlim;
    getrlimit(RLIMIT_DATA, &rlim);
    rlim.rlim_cur = size;
    rlim.rlim_max = size;
    vrfy_(setrlimit(RLIMIT_DATA, &rlim)==0);

    getrlimit(RLIMIT_AS, &rlim);
    rlim.rlim_cur = size;
    rlim.rlim_max = size;
    vrfy_(setrlimit(RLIMIT_AS, &rlim)==0);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_Lpool() {
    // step 1: check that lpool is empty
    for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
        vrfy_(atf_amc::_db.optalloc_free[i]==NULL);
    }
    // step 2: alloc 2MB
    {
        if (void* elem = atf_amc::optalloc_AllocMem(1<<21)) {
            vrfy_(atf_amc::_db.optalloc_n==1);
            atf_amc::optalloc_FreeMem(elem,1<<21);
        }
        vrfy_(atf_amc::_db.optalloc_n==0);
        for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
            vrfy_((atf_amc::_db.optalloc_free[i]!=NULL) == (i == 21));
        }
    }
    // step 3: alloc 1MB -- it should come out of the 2MB block
    {
        if (void* elem = atf_amc::optalloc_AllocMem(1<<20)) {
            vrfy_(atf_amc::_db.optalloc_n==1);
            atf_amc::optalloc_FreeMem(elem,1<<20);
        }
        vrfy_(atf_amc::_db.optalloc_n==0);
        // and only 1MB freelist should be nonempty
        for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
            vrfy_((atf_amc::_db.optalloc_free[i]!=NULL) == (i == 20));
        }
    }
    // step 4: realloc from 0 -- it's an alloc
    // realloc to 0 is NOT a free (since NULL indicates failure of realloc)
    // it's a realloc to 16 bytes
    //
    {
        void* elem = atf_amc::optalloc_ReallocMem(NULL, 0, 1<<20);
        vrfy_(elem != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        elem = atf_amc::optalloc_ReallocMem(elem, 1<<20, 0);
        vrfy_(elem != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        // free from 0 bytes -- same as free from 16
        atf_amc::optalloc_FreeMem(elem, 0);
        vrfy_(atf_amc::_db.optalloc_n==0);
        vrfy_(atf_amc::_db.optalloc_free[4] == elem);// check that it went into the right bin
    }
    // step 5: use setrlimit and alloc a block that's too large
    if (algo::LockAllMemory()) {
        // don't worry, each test executes inside a fork() anyway
        // so after this function exits, these changes will be discarded
        algo_lib::_db.sbrk_zeromem = true;
        LimitMem(10*1024*1024);

        void *elem1 = atf_amc::optalloc_AllocMem(1<<15); // 1MB will fit
        vrfy_(elem1 != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        MarkOrCheckMem(elem1, 1<<15, true);
        void *elem2 = atf_amc::optalloc_ReallocMem(elem1, 1<<15, 1<<30);// 1GB won't fit
        vrfy_(elem2 == NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        MarkOrCheckMem(elem1, 1<<15, false);
        atf_amc::optalloc_FreeMem(elem1,1<<20);// free the original block
        vrfy_(atf_amc::_db.optalloc_n==0);
    } else {
        verblog("# atf_amc: skipping Lpool LimitMem test, mlockall not available");
    }
}
