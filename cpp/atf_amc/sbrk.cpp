// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/sbrk.cpp
//
// Tests for the base allocator: what it maps, how it aligns what it maps, and
// what it reports having mapped.

#include "include/algo.h"
#include "include/atf_amc.h"

// The base allocator counts the big blocks it maps, and only those.
//
// A request of two megabytes or more is served by mapping it, and there are two
// ways to map: a huge-page attempt first, an ordinary mapping when huge pages
// are unavailable.  A process that cannot use huge pages -- a container whose
// cgroup exposes no hugetlb controller, which is the ordinary case -- takes the
// second way for every block it ever asks for.  A counter that only counts the
// first way therefore reports nothing at all about such a process while its
// address space grows a granule at a time, and a counter reading zero is
// indistinguishable from a process that allocated nothing.  The field's own
// documentation promises otherwise: every mapping, huge or not.
//
// The huge-page budget starts at zero and only a runtime that wants huge pages
// raises it, so this
// process never had huge pages to lose; zeroing it here states that the ordinary
// path is the one under test and keeps the test independent of the machine it
// runs on.  It is not restored: the allocator itself zeroes the budget for good
// the first time it falls back, so zero is the state production converges to.
//
// The assertions beyond the counter are what keep a wrong fix from passing.  The
// failed request leaving the counter alone is what forbids hoisting the increment
// to an unconditional statement, which would count a mapping that never happened
// -- the same blindness inverted.  Granule alignment is the contract the
// variable-length pool above depends on, and it also makes the size below safe to
// hand back: the allocator rounds the size it maps up to a granule without
// telling the caller, so a size that is already a multiple is the only kind that
// unmaps exactly what was mapped.  The small-allocation counter is read as a
// statement of which branch ran, redundant against the mapping count but free.
//
// Zeroing is turned off because the test never reads the block; false is also
// what the allocator initializes it to, so nothing here has to put it back.
// Only some platforms serve a big block by mapping it; the rest hand one back
// from the aligned allocator, which maps nothing and so counts nothing.  A
// mapping counter reading zero is the truth there rather than the blindness
// under test, and the assertions about it are carried by this one fact so the
// allocator itself is exercised the same way everywhere.
#ifdef __linux__
static const bool mapped = true;
#else
static const bool mapped = false;
#endif

void atf_amc::amctest_SbrkMmapTrace() {
    u64 granule = 2 * 1024 * 1024;
    u64 size = 2 * granule;
    algo_lib::_db.sbrk_huge_limit = 0;
    algo_lib::_db.sbrk_zeromem = false;
    u64 nmmap = algo_lib::_db.trace.n_mmap;
    u64 nsbrk = algo_lib::_db.trace.n_sbrk;
    void *mem = algo_lib::sbrk_AllocMem(size);
    vrfy_(mem != NULL);
    vrfy_((u64(mem) & (granule - 1)) == 0);
    vrfy_(!mapped || algo_lib::_db.trace.n_sbrk == nsbrk);
    vrfy_(!mapped || algo_lib::_db.trace.n_mmap == nmmap + 1);
    algo_lib::sbrk_FreeMem(mem, size);
    // A request no mapping can serve is not a mapping, and must not count as
    // one.  The reading it must leave alone is whatever the served request
    // above counted, which is one mapping where a big block is mapped and none
    // where it comes from the aligned allocator -- so the count is taken rather
    // than assumed, and the assertion keeps its force on either kind of system.
    //
    // The size has to exceed the address space rather than merely exhaust
    // memory: a mapping is requested with the flag that faults every page in up
    // front, so a size the kernel is willing to map would be populated before it
    // failed, and a test that asks for one waits for terabytes of page faults
    // instead of failing.
    u64 nmmap_served = algo_lib::_db.trace.n_mmap;
    void *unservable = algo_lib::sbrk_AllocMem(1ULL << 60);
    vrfy_(unservable == NULL);
    vrfyeq_(algo_lib::_db.trace.n_mmap, nmmap_served);
}

// A block too big to fit under the huge-page ceiling leaves the ceiling in place.
//
// A runtime arms each process with the ceiling its proctype declares -- four
// gigabytes for every module -- and a module's largest single request is bigger
// than that: a txn cache provisioned for twelve gigabytes asks for it in one
// block.  That block is served on ordinary pages, which is the ceiling doing its
// job.  What must not follow is the process losing huge pages for everything
// else it allocates, because the pool blocks that follow are two megabytes each
// and the ceiling has room for two thousand of them.
//
// The ceiling is set to four granules and a block of eight requested, so the
// allocator declines the huge route by arithmetic and never asks the kernel for
// it.  That keeps the test independent of the machine: a container with no
// hugetlb pages available answers the same way as a tuned node, because neither
// is consulted.  A budget left at zero afterwards is what production converges
// to, so nothing here has to be put back.
void atf_amc::amctest_SbrkHugeCeiling() {
    u64 granule = 2 * 1024 * 1024;
    algo_lib::_db.sbrk_zeromem = false;
    algo_lib::_db.sbrk_huge_alloc = 0;
    algo_lib::_db.sbrk_huge_limit = 4 * granule;
    u64 size = 8 * granule;
    void *mem = algo_lib::sbrk_AllocMem(size);
    vrfy_(mem != NULL);
    vrfy_((u64(mem) & (granule - 1)) == 0);
    vrfy_(!mapped || algo_lib::_db.sbrk_huge_limit == 4 * granule);
    vrfy_(algo_lib::_db.sbrk_huge_alloc == 0);
    algo_lib::sbrk_FreeMem(mem, size);
    algo_lib::_db.sbrk_huge_limit = 0;
}

// -----------------------------------------------------------------------------

// Return the kilobytes of this process's anonymous memory held in transparent
// huge pages, from /proc/self/smaps_rollup; zero where the file is absent.
static u64 AnonHugeKb() {
    u64 ret = 0;
    ind_beg(algo::FileLine_curs, line, "/proc/self/smaps_rollup") {
        if (StartsWithQ(line, "AnonHugePages:")) {
            ret = strtoull(Trimmed(Pathcomp(line, ":LR")).elems, NULL, 10);
        }
    }ind_end;
    return ret;
}

// Big-block benchmark: map one 256 MB block on the ordinary route, the one a
// process past its huge-page ceiling takes, and report how long the map took
// and how much of the block transparent huge pages back.  The block is
// populated before the call returns either way; on a host whose THP mode is
// madvise, only an advised block gets huge pages, and each huge page is one
// fault where 4K pages are 512.
void atf_amc::amctest_PerfSbrkBig() {
    u64 size = 256ULL << 20;
    algo_lib::_db.sbrk_zeromem = false;
    algo_lib::_db.sbrk_huge_limit = 0;
    u64 thp0 = AnonHugeKb();
    u64 t0 = algo::get_cycles();
    void *mem = algo_lib::sbrk_AllocMem(size);
    u64 t1 = algo::get_cycles();
    vrfy_(mem != NULL);
    u64 thp1 = AnonHugeKb();
    algo_lib::sbrk_FreeMem(mem, size);
    prlog("atf_amc.PerfSbrkBig"
          << Keyval("mb", size >> 20)
          << Keyval("map_ms", double(t1 - t0) / algo::get_cpu_hz_int() * 1e3)
          << Keyval("thp_mb", (thp1 - u64_Min(thp0, thp1)) >> 10));
}
