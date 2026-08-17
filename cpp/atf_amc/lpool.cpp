// Copyright (C) 2023-2024,2026 AlgoRND
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
// Exceptions: yes
// Source: cpp/atf_amc/lpool.cpp
//

#include "include/atf_amc.h"
#ifndef WIN32
#include <sys/resource.h>
#include <sys/mman.h>
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
            *((u8*)mem+i) = (u8)random();
        } else {
            vrfy_(*((u8*)mem+i) == (u8)random());
        }
    }
}

// -----------------------------------------------------------------------------

// Lower the soft limit for both AS (mmaps) and Data (Stack + Heap) to SIZE
// bytes. The hard limits stay untouched: lowering rlim_max is irreversible
// for an unprivileged process, and the caller restores the saved soft
// limits when the test completes.
static void LimitMem(i64 size) {
    struct rlimit rlim;
    getrlimit(RLIMIT_DATA, &rlim);
    rlim.rlim_cur = size;
    vrfy_(setrlimit(RLIMIT_DATA, &rlim)==0);

    getrlimit(RLIMIT_AS, &rlim);
    rlim.rlim_cur = size;
    vrfy_(setrlimit(RLIMIT_AS, &rlim)==0);
}

// -----------------------------------------------------------------------------

// Total bytes of address space currently mapped by this process:
// the first field of /proc/self/statm (VmSize), in pages.
// The callers size rlimits and a must-fail alloc from this value, so a
// silent zero (statm unreadable) would surface as a bogus pool-allocator
// failure; refuse it here with a message naming the real cause.
static i64 VmBytes() {
    tempstr statm(algo::FileToString("/proc/self/statm"));
    i64 pages = algo::ParseI64(algo::Pathcomp(statm, " LL"), 0);
    vrfy(pages > 0, "cannot read VmSize from /proc/self/statm");
    return pages * sysconf(_SC_PAGESIZE);
}

// -----------------------------------------------------------------------------

// Count blocks on the free list of raw level CELL
static int FreelistN(int cell) {
    int n = 0;
    for (atf_amc::optalloc_Lpblock *block = atf_amc::_db.optalloc_free[cell]; block; block = block->next) {
        n++;
    }
    return n;
}

// -----------------------------------------------------------------------------

// Count blks on the class list of blk class CELL (full blks leave the list)
static int BlkListN(int cell) {
    int n = 0;
    for (atf_amc::optalloc_Lpblk *blk = atf_amc::_db.optalloc_blk[cell]; blk; blk = blk->next) {
        n++;
    }
    return n;
}

// -----------------------------------------------------------------------------

// Base address of the 64K blk containing the record MEM
static u64 BlkBase(void *mem) {
    return (u64)mem & ~(u64)0xFFFF;
}

// -----------------------------------------------------------------------------

// Total bytes on the raw free lists across all levels (level i holds 2^(i+4)-byte blocks)
static u64 FreelistBytes() {
    u64 total = 0;
    for (int i = 0; i < ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
        total += (u64)FreelistN(i) << (i+4);
    }
    return total;
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_Lpool() {
    int minlevels=4;
    int nlevels=36;
    Phase("check that lpool is empty");
    for (int i=0; i<ssizeof(atf_amc::_db.optalloc_free)/ssizeof(void*); i++) {
        vrfy_(atf_amc::_db.optalloc_free[i]==NULL);
    }
    for (int i=0; i<ssizeof(atf_amc::_db.optalloc_blk)/ssizeof(void*); i++) {
        vrfy_(atf_amc::_db.optalloc_blk[i]==NULL);
    }
    Phase("alloc 2MB");
    {
        // the refill maps exactly the requested 2MB, granule-aligned by the
        // base pool, so no other level gets stocked as a side effect
        void* elem = atf_amc::optalloc_AllocMem(1<<21);
        vrfy_(elem!=NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        vrfy_(((u64)elem & 0xFFFF) == 0);// refill block is blk-aligned
        vrfy_(FreelistBytes()==0);// exact refill: nothing shed
        atf_amc::optalloc_FreeMem(elem,1<<21);
        vrfy_(atf_amc::_db.optalloc_n==0);
        vrfy_(FreelistN(21-minlevels)==1);
    }
    Phase("alloc 1MB -- it should come out of an existing free block");
    {
        // a from-stock alloc pops one block and sheds the split halves back,
        // shrinking the free lists by exactly the class size
        u64 freebytes = FreelistBytes();
        void* elem = atf_amc::optalloc_AllocMem(1<<20);
        vrfy_(elem!=NULL);
        vrfy_(atf_amc::_db.optalloc_n==1);
        vrfy_(FreelistBytes()==freebytes-(1<<20));// served from stock, no refill
        int nfree_1m = FreelistN(20-minlevels);
        atf_amc::optalloc_FreeMem(elem,1<<20);
        vrfy_(atf_amc::_db.optalloc_n==0);
        vrfy_(FreelistN(20-minlevels)==nfree_1m+1);
    }
    Phase("check small size allocations: served linearly out of a blk");
    {
        // a small class draws a 64K blk and carves records from it linearly;
        // a freed record is reallocated from the blk's free list (LIFO)
        void *elem1=atf_amc::optalloc_AllocMem(16);
        vrfy_(elem1!=NULL);
        vrfy_(atf_amc::_db.optalloc_blk[0]!=NULL);
        vrfy_(BlkListN(0)==1);
        void *elem2=atf_amc::optalloc_AllocMem(16);
        vrfy_(elem2==(u8*)elem1+16);// linear carve
        atf_amc::optalloc_FreeMem(elem2,16);
        void *elem3=atf_amc::optalloc_AllocMem(16);
        vrfy_(elem3==elem2);// freed record reallocated from the blk
        atf_amc::optalloc_FreeMem(elem3,16);
        atf_amc::optalloc_FreeMem(elem1,16);
        // fully drained, but the last blk of a class stays dedicated
        vrfy_(BlkListN(0)==1);
        vrfy_(atf_amc::_db.optalloc_n==0);
    }
    Phase("cross-class blk reuse: a drained blk serves another class");
    {
        // fill two blks of the 4K class, then free everything: one blk
        // stays dedicated, the other reverts to the 64K level and is
        // reusable by a different class
        enum { NREC = 2*15+1 };// two blks' worth of 4K records and one more
        void *elem[NREC];
        for (int i=0; i<NREC; i++) {
            elem[i]=atf_amc::optalloc_AllocMem(1<<12);
            vrfy_(elem[i]!=NULL);
        }
        vrfy_(BlkBase(elem[15])!=BlkBase(elem[0]));// 15 records fill a blk; the 16th opens a second
        vrfy_(BlkBase(elem[30])!=BlkBase(elem[15]));// and the 31st a third
        int nfree_64k = FreelistN(16-minlevels);
        for (int i=0; i<NREC; i++) {
            atf_amc::optalloc_FreeMem(elem[i],1<<12);
        }
        vrfy_(BlkListN(12-minlevels)==1);// drained blks reclaimed, sole survivor kept
        vrfy_(FreelistN(16-minlevels)==nfree_64k+2);// reclaimed blks are 64K blocks now
        void *elem8k=atf_amc::optalloc_AllocMem(1<<13);// another class dedicates one of them
        vrfy_(elem8k!=NULL);
        vrfy_(BlkListN(13-minlevels)==1);
        bool reused = false;
        for (int i=0; i<NREC; i++) {
            reused = reused || BlkBase(elem8k)==BlkBase(elem[i]);
        }
        vrfy_(reused);// the 8K blk is one of the reclaimed 4K blks
        atf_amc::optalloc_FreeMem(elem8k,1<<13);
        vrfy_(atf_amc::_db.optalloc_n==0);
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

        // free from 0 bytes -- same as free from 16 (the size a 0-byte alloc got)
        atf_amc::optalloc_FreeMem(elem, 0);
        vrfy_(atf_amc::_db.optalloc_n==0);
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
        MarkOrCheckMem(elem1, (1<<10)-3, true);// odd size covers the byte-tail path
        void *elem2 = atf_amc::optalloc_ReallocMem(elem1, 1ULL << 10, 1ULL << (minlevels+nlevels+1));
        vrfy_(elem2 == NULL);
        MarkOrCheckMem(elem1, (1<<10)-3, false);// check old contents
        atf_amc::optalloc_FreeMem(elem1, 1<<10);
        vrfy_(atf_amc::_db.optalloc_n==0);
    }
    Phase("alloc an 8GB region -- should succeed");
    {
        // The pool never hands memory back to the OS: a block passed to
        // FreeMem stays on its level's free list. An 8GB block parked on
        // level 29 keeps 8GB mapped for the rest of the process, and every
        // later allocation whose own level is empty is then served by
        // splitting that block instead of by a refill -- so a test that runs
        // after this one in the same process (-dofork:N) sees a pool nothing
        // else in the tree produces. The level is empty here, so the refill
        // maps exactly the level size and sheds nothing on the way down: the
        // freed block is one whole base-pool allocation. The phase therefore
        // pops it off its level and returns it to the base pool, and the
        // process-wide zero-fill flag it lowered goes back as well.
        bool save_zeromem = algo_lib::_db.sbrk_zeromem;
        u64 freebytes = FreelistBytes();
        algo_lib::_db.sbrk_zeromem=false;// an 8GB memset would double the cost of the phase
        void *elem1 = atf_amc::optalloc_AllocMem(1ULL << 33);
        // observations are collected first and asserted after the teardown, so
        // the assertion that catches a regression does not also leave the 8GB
        // block parked and the zero-fill flag lowered for every later amctest
        // in the process; each starts at the value its assertion expects, so an
        // observation an earlier check preempts adds no second failure. The
        // block goes back to the base pool only while it is its level's whole
        // head: a refill that shed lower-level blocks put part of the region on
        // other lists, and handing those 8GB back would free memory the pool
        // still owns.
        bool alloc_ok = elem1 != NULL;
        bool all_freed = true;
        optalloc_Lpblock *freed = (optalloc_Lpblock*)elem1;
        if (alloc_ok) {
            atf_amc::optalloc_FreeMem(elem1, 1ULL << 33);
            all_freed = atf_amc::_db.optalloc_n==0;
            freed = atf_amc::_db.optalloc_free[33-minlevels];
            if ((void*)freed==elem1) {
                atf_amc::_db.optalloc_free[33-minlevels] = freed->next;
                algo_lib::sbrk_FreeMem(elem1, 1ULL << 33);
            }
        }
        algo_lib::_db.sbrk_zeromem=save_zeromem;
        vrfy_(alloc_ok);
        vrfy_(all_freed);
        vrfy_((void*)freed==elem1);// exact refill: the block is its level's head, nothing was shed
        vrfy_(FreelistBytes()==freebytes);// free lists as the phase found them
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

// One typed alloc advances the alloc trace counter by exactly one, and one
// delete advances the del counter by exactly one. The counter has a single
// owner, the typed alloc layer; a second increment in the underlying
// AllocMem would double-count every successful alloc.
void atf_amc::amctest_LpoolAllocTrace() {
    u64 nalloc = atf_amc::_db.trace.alloc__db_msghdrltscale;
    u64 ndel = atf_amc::_db.trace.del__db_msghdrltscale;
    atf_amc::MsgHdrLTScale &hdr = atf_amc::msghdrltscale_Alloc();
    vrfy_(atf_amc::_db.trace.alloc__db_msghdrltscale == nalloc+1);
    atf_amc::msghdrltscale_Delete(hdr);
    vrfy_(atf_amc::_db.trace.del__db_msghdrltscale == ndel+1);
}

// -----------------------------------------------------------------------------

// ReserveBuffers stocks the free store: it allocates NBUF buffers of size
// BUFSIZE through the ordinary alloc path and frees them all, leaving no live
// allocation behind, so a subsequent same-size AllocMem is served from the
// reserved capacity without drawing on the base allocator. For a blk-class
// size the reserve leaves the class with a dedicated blk holding free space,
// and the follow-up alloc is served from that blk, touching no raw level; for
// a level-class size the follow-up alloc pops exactly the class size from the
// raw free lists, with no refill. Each pin is a delta against captured state,
// since earlier traffic in the same process may already have populated the
// pool. A bufsize beyond the largest level cannot be reserved: AllocMem for
// it returns NULL, and ReserveBuffers must report false, not true.
void atf_amc::amctest_LpoolReserveBuffers() {
    int minlevels=4;
    Phase("reserve a blk-class buffer -- a same-size alloc is then served from the class blk");
    {
        i64 nlive = atf_amc::_db.optalloc_n;
        vrfy_(atf_amc::optalloc_ReserveBuffers(1, 1<<10));
        vrfy_(atf_amc::_db.optalloc_n == nlive);// everything the reserve allocated came back
        atf_amc::optalloc_Lpblk *blk = atf_amc::_db.optalloc_blk[10-minlevels];
        vrfy_(blk != NULL);// the class holds a blk with free space
        void *elem = atf_amc::optalloc_AllocMem(1<<10);
        vrfy_(elem != NULL);
        vrfy_(BlkBase(elem) == (u64)blk);// carved from that blk, not a fresh raw block or a level pop
        atf_amc::optalloc_FreeMem(elem, 1<<10);
    }
    Phase("reserve a level-class buffer -- its level then serves a same-size alloc from stock");
    {
        i64 nlive = atf_amc::_db.optalloc_n;
        vrfy_(atf_amc::optalloc_ReserveBuffers(1, 1<<20));
        vrfy_(atf_amc::_db.optalloc_n == nlive);// everything the reserve allocated came back
        u64 freebytes = FreelistBytes();
        void *elem = atf_amc::optalloc_AllocMem(1<<20);
        vrfy_(elem != NULL);
        vrfy_(FreelistBytes() == freebytes-(1<<20));// popped from stock, no refill
        atf_amc::optalloc_FreeMem(elem, 1<<20);
    }
    Phase("reserve a buffer larger than the largest level -- must fail");
    {
        vrfy_(!atf_amc::optalloc_ReserveBuffers(1, 1ULL<<41));
    }
}

void atf_amc::amctest_LpoolLockMem() {
    Phase("use setrlimit and alloc a block that's too large");
    if (algo::LockAllMemory()) {
        struct rlimit save_data;
        struct rlimit save_as;
        getrlimit(RLIMIT_DATA, &save_data);
        getrlimit(RLIMIT_AS, &save_as);
        bool save_zeromem = algo_lib::_db.sbrk_zeromem;
        tempstr err;
        bool ok = true;
        // every engage step past the mlockall gate lives inside the try:
        // a throw from any of them (LimitMem fails when the hard limit is
        // finite and near the process footprint) must still reach the
        // restore epilogue below
        try {
            algo_lib::_db.sbrk_zeromem = true;
            // a refill maps exactly its level size (2MB floor), but the base
            // pool's granule-alignment trim transiently over-maps by one granule;
            // the headroom above the process's current mappings covers that.
            LimitMem(VmBytes() + 64*1024*1024);
            void *elem1 = atf_amc::optalloc_AllocMem(1<<20); // 1MB fits under the headroom
            vrfy_(elem1 != NULL);
            vrfy_(atf_amc::_db.optalloc_n==1);
            MarkOrCheckMem(elem1, 1<<20, true);

            // a grow beyond everything the process has mapped cannot be
            // served from pool stock (every free block is mapped memory),
            // so the refill must ask the OS, and the lowered limit refuses
            u64 grow = VmBytes() + (1ULL<<30);
            void *elem2 = atf_amc::optalloc_ReallocMem(elem1, 1<<20, grow);
            vrfy_(elem2 == NULL);
            vrfy_(atf_amc::_db.optalloc_n==1);

            MarkOrCheckMem(elem1, 1<<20, false);
            atf_amc::optalloc_FreeMem(elem1,1<<20);// free the original block
            vrfy_(atf_amc::_db.optalloc_n==0);
        } catch (algo_lib::ErrorX &x) {
            err << x.str;
            ok = false;
        }
        // the memory caps, the mlockall lock, and the zero-fill flag are
        // process-wide; restore them before reporting the verdict so the
        // tests that follow in a single-process run (-dofork:N) allocate
        // under the original environment
        (void)setrlimit(RLIMIT_DATA, &save_data);
        (void)setrlimit(RLIMIT_AS, &save_as);
        munlockall();
        algo_lib::_db.sbrk_zeromem = save_zeromem;
        vrfy(ok, err);
    } else {
        prlog("# atf_amc: skipping Lpool LimitMem test, mlockall not available");
    }
}
