// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
//
// License: GPL
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf_amc/lpool.cpp
//

#include "include/atf_amc.h"
#ifndef WIN32
#include <sys/resource.h>
#endif

// -----------------------------------------------------------------------------

// if MARK is TRUE, fill region MEM with random values
// If MARK is false, check that the contents of the region MEM
// matches random values. The random seed is picked based on the address MEM.
static void MarkOrCheckMem(void *mem, int n, bool mark) {
    srandom((int)(i64)mem);
    int i=0;
    for (; i+4 <= n; i += 4) {
        if (mark) {
            *(u32*)((u8*)mem+i) = random();
        } else {
            vrfy_(*(u32*)((u8*)mem+i) == (u32)random());
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
    int minlevels=4;
    int nlevels=36;
    Phase("check that lpool is empty");
    for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
        vrfy_(atf_amc::_db.optalloc_free[i]==NULL);
    }
    Phase("alloc 2MB");
    {
        void* elem = atf_amc::optalloc_AllocMem(1<<21);
        vrfy_(elem!=NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        atf_amc::optalloc_FreeMem(elem,1<<21);
        vrfy_(atf_amc::_db.optalloc_n==0);
        // and only 2MB freelist should be nonempty
        for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
            vrfy_((atf_amc::_db.optalloc_free[i]!=NULL) == (i == 21-minlevels));
        }
    }
    Phase("alloc 1MB -- it should come out of the 2MB block");
    {
        void* elem = atf_amc::optalloc_AllocMem(1<<20);
        vrfy_(elem!=NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        atf_amc::optalloc_FreeMem(elem,1<<20);
        vrfy_(atf_amc::_db.optalloc_n==0);
        // and only 1MB freelist should be nonempty
        for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
            vrfy_((atf_amc::_db.optalloc_free[i]!=NULL) == (i == 20-minlevels));
        }
    }
    Phase("check small size allocations");
    {
        for (int i=0; i<minlevels; i++) {
            void *elem=atf_amc::optalloc_AllocMem(1<<i);
            vrfy_(elem!=NULL);
            atf_amc::optalloc_FreeMem(elem,1<<i);
            vrfy_(atf_amc::_db.optalloc_free[0]==elem);
        }
    }
    Phase("realloc from 0 -- it's an alloc");
    {
        void* elem = atf_amc::optalloc_ReallocMem(NULL, 0, 1<<20);
        vrfy_(elem != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        Phase("    realloc to 0 size -- it's not a free");
        elem = atf_amc::optalloc_ReallocMem(elem, 1<<20, 0);
        vrfy_(elem != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        // free from 0 bytes -- same as free from 16
        atf_amc::optalloc_FreeMem(elem, 0);
        vrfy_(atf_amc::_db.optalloc_n==0);
        vrfy_(atf_amc::_db.optalloc_free[0] == elem);// check that it went into the right bin
    }
    Phase("alloc a region that's definitely too large");
    {
        void *elem1 = atf_amc::optalloc_AllocMem(1ULL << 41);
        vrfy_(elem1 == NULL);
    }
    Phase("realloc a region to a size that's too large -- should fail");
    {
        void *elem1 = atf_amc::optalloc_AllocMem(1ULL << 10);
        vrfy_(elem1 != NULL);
        MarkOrCheckMem(elem1, 1<<10, true);
        void *elem2 = atf_amc::optalloc_ReallocMem(elem1, 1ULL << 10, 1ULL << (minlevels+nlevels+1));
        vrfy_(elem2 == NULL);
        MarkOrCheckMem(elem1, 1<<10, false);// check old contents
        atf_amc::optalloc_FreeMem(elem1, 1<<10);
        vrfy_(atf_amc::_db.optalloc_n==0);
    }
    Phase("alloc an 8GB region -- should succeed");
    {
        algo_lib::_db.sbrk_zeromem=false;
        void *elem1 = atf_amc::optalloc_AllocMem(1ULL << 33);
        vrfy_(elem1 != NULL);
        atf_amc::optalloc_FreeMem(elem1, 1ULL << 33);
        // print distribution of allocated buckets:
        for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
            tempstr s;
            s<<"level "<<i<<"; size "<<(u64(1)<<(i+minlevels))<<": ";
            for (optalloc_Lpblock *blk = (optalloc_Lpblock *)atf_amc::_db.optalloc_free[i]; blk; blk=blk->next) {
                s<<".";
            }
            prlog(s);
        }
    }
}

void atf_amc::amctest_LpoolLockMem() {
    Phase("use setrlimit and alloc a block that's too large");
    if (algo::LockAllMemory()) {
        // don't worry, each test executes inside a fork() anyway
        // so after this function exits, these changes will be discarded
        algo_lib::_db.sbrk_zeromem = true;
        LimitMem(20*1024*1024);

        void *elem1 = atf_amc::optalloc_AllocMem(1<<20); // 1MB will fit
        vrfy_(elem1 != NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        MarkOrCheckMem(elem1, 1<<20, true);

        void *elem2 = atf_amc::optalloc_ReallocMem(elem1, 1<<20, 1<<30);// 1GB won't fit
        vrfy_(elem2 == NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);

        MarkOrCheckMem(elem1, 1<<20, false);
        atf_amc::optalloc_FreeMem(elem1,1<<20);// free the original block
        vrfy_(atf_amc::_db.optalloc_n==0);
    } else {
        prlog("# atf_amc: skipping Lpool LimitMem test, mlockall not available");
    }
}
