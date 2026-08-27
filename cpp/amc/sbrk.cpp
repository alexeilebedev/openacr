// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/sbrk.cpp -- Sbrk allocator
//

#include "include/amc.h"

void amc::tclass_Sbrk() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    InsVar(R, field.p_ctype, "u64", "$name_huge_limit", "", "Huge page limit in bytes -- set to 0 with fork!");
    InsVar(R, field.p_ctype, "u64", "$name_huge_alloc", "", "Huge page bytes allocated");
    InsVar(R, field.p_ctype, "bool", "$name_zeromem", "", "Zero allocated memory");
}

// Emit the base allocator: it serves a block of a granule or more by mapping it,
// preferring huge pages while the process is under its huge-page ceiling.
//
// The ceiling and the kernel's refusal are two different answers and the
// allocator must not confuse them.  Consider a process whose ceiling is 4GB and
// whose receive cache asks for one 12GB block: the block cannot be served on
// huge pages, and it is served on ordinary ones.  If that outcome also retires
// the huge route, every later block -- a two-megabyte pool block that the
// ceiling has ample room for -- is mapped on ordinary pages too, and the process
// runs its whole heap on 4K pages because of one request the ceiling was
// supposed to merely decline.
//
// So the route is retired only when the huge route was actually taken and the
// kernel refused it: no pages left in the pool, a cgroup allowance already
// spent, or the mlock that verifies the mapping failing.  Those answers do not
// change for the life of the process, which is what makes retiring the route the
// right response to them and the wrong response to a block that was simply too
// big to count against the ceiling.
void amc::tfunc_Sbrk_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& allocmem = amc::CreateCurFunc();
    Ins(&R, allocmem.ret     , "void*", false);
    Ins(&R, allocmem.proto   , "$name_AllocMem($Parent, u64 size)", false);
    Ins(&R, allocmem.comment , "Allocate a new piece of memory at least SIZE bytes long.");
    Ins(&R, allocmem.comment , "If out of memory, return NULL");
    Ins(&R, allocmem.comment , "Newly allocated memory is initialized to zeros");
    Ins(&R, allocmem.comment , "Blocks of the 2MB granule size and larger are granule-aligned;");
    Ins(&R, allocmem.comment , "Lpool relies on this to locate a record's blk by address mask.");
    Ins(&R, allocmem.body    , "void *ret;");
    Ins(&R, allocmem.body    , "#if defined(__MACH__) || __FreeBSD__>0 || __CYGWIN__>0 || defined(WIN32)");
    Ins(&R, allocmem.body    , "    u64 bigsize = 1024*2048;");
    Ins(&R, allocmem.body    , "    if (size >= bigsize) { // big block: granule-aligned");
    Ins(&R, allocmem.body    , "        #if defined(WIN32)");
    Ins(&R, allocmem.body    , "        ret = _aligned_malloc(size, bigsize);");
    Ins(&R, allocmem.body    , "        #else");
    Ins(&R, allocmem.body    , "        ret = NULL;");
    Ins(&R, allocmem.body    , "        if (posix_memalign(&ret, bigsize, size) != 0) {");
    Ins(&R, allocmem.body    , "            ret = NULL;");
    Ins(&R, allocmem.body    , "        }");
    Ins(&R, allocmem.body    , "        #endif");
    Ins(&R, allocmem.body    , "    } else {");
    Ins(&R, allocmem.body    , "        ret = malloc(size);");
    Ins(&R, allocmem.body    , "    }");
    Ins(&R, allocmem.body    , "#else");
    Ins(&R, allocmem.body    , "ret = MAP_FAILED;");
    Ins(&R, allocmem.body    , "u64 bigsize = 1024*2048;");
    Ins(&R, allocmem.body    , "if (size >= bigsize) { // big block -- will be registered");
    Ins(&R, allocmem.body    , "    size = (size + bigsize - 1) / bigsize * bigsize;");
    Ins(&R, allocmem.body    , "    int prot = PROT_READ | PROT_WRITE;");
    Ins(&R, allocmem.body    , "    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE;");
    Ins(&R, allocmem.body    , "    bool use_huge = $parname.$name_huge_alloc + size < $parname.$name_huge_limit;");
    Ins(&R, allocmem.body    , "    if (use_huge) {");
    Ins(&R, allocmem.body    , "        ret = mmap(0x0, size, prot, flags | MAP_HUGETLB, 0, 0); // try huge pages; granule-aligned by the kernel");
    Ins(&R, allocmem.body    , "    }");
    Ins(&R, allocmem.body    , "    // linux huge page handling is buggy -- MAP_POPULATE doesn't guarantee");
    Ins(&R, allocmem.body    , "    // that pages are there. subsequent page fault may cause a SIGBUS.");
    Ins(&R, allocmem.body    , "    // fortunately there is a way to detect this condition using mlock.");
    Ins(&R, allocmem.body    , "    if (use_huge && ret != MAP_FAILED) {");
    Ins(&R, allocmem.body    , "        int rc = mlock(ret, size);");
    Ins(&R, allocmem.body    , "        if (rc!=0) {");
    Ins(&R, allocmem.body    , "            munmap(ret,size);");
    Ins(&R, allocmem.body    , "            ret=MAP_FAILED;");
    Ins(&R, allocmem.body    , "        } else {");
    Ins(&R, allocmem.body    , "            $parname.$name_huge_alloc += size;");
    Ins(&R, allocmem.body    , "        }");
    Ins(&R, allocmem.body    , "    }");
    Ins(&R, allocmem.body    , "    if (ret == MAP_FAILED) { // regular mmap, trimmed to a granule-aligned block:");
    Ins(&R, allocmem.body    , "        // over-map by one granule and unmap the unaligned head and the tail,");
    Ins(&R, allocmem.body    , "        // so the block costs exactly SIZE bytes of address space, and a");
    Ins(&R, allocmem.body    , "        // granule-aligned region remains eligible for transparent huge pages");
    Ins(&R, allocmem.body    , "        if (use_huge) { // the kernel refused the huge route, so stop asking for it");
    Ins(&R, allocmem.body    , "            $parname.$name_huge_limit = 0;");
    Ins(&R, allocmem.body    , "        }");
    Ins(&R, allocmem.body    , "        u8 *base = (u8*)mmap(0x0, size + bigsize, prot, flags, 0, 0);");
    Ins(&R, allocmem.body    , "        if (base != MAP_FAILED) {");
    Ins(&R, allocmem.body    , "            u64 pad = ((u64)base + bigsize - 1) / bigsize * bigsize - (u64)base;");
    Ins(&R, allocmem.body    , "            if (pad) {");
    Ins(&R, allocmem.body    , "                munmap(base, pad);");
    Ins(&R, allocmem.body    , "            }");
    Ins(&R, allocmem.body    , "            munmap(base + pad + size, bigsize - pad);");
    Ins(&R, allocmem.body    , "            ret = base + pad;");
    Ins(&R, allocmem.body    , "        }");
    Ins(&R, allocmem.body    , "    }");
    Ins(&R, allocmem.body    , "    // Count the block by whichever route mapped it.  A process that cannot");
    Ins(&R, allocmem.body    , "    // get huge pages takes the ordinary route for every block it ever asks");
    Ins(&R, allocmem.body    , "    // for, so a counter that sees only the huge route reads zero for it.");
    Ins(&R, allocmem.body    , "    algo_lib::_db.trace.n_mmap += ret != MAP_FAILED;");
    Ins(&R, allocmem.body    , "} else {");
    Ins(&R, allocmem.body    , "    if (ret == MAP_FAILED) { // default method");
    Ins(&R, allocmem.body    , "        ret = sbrk(size);");
    Ins(&R, allocmem.body    , "        ++algo_lib::_db.trace.n_sbrk; // update global counter");
    Ins(&R, allocmem.body    , "    }");
    Ins(&R, allocmem.body    , "}");
    Ins(&R, allocmem.body    , "if (ret == (void*)-1) { // sbrk returns -1 on error");
    Ins(&R, allocmem.body    , "    ret = NULL;");
    Ins(&R, allocmem.body    , "}");
    Ins(&R, allocmem.body    , "#endif");
    Ins(&R, allocmem.body    , "if (ret && $parname.$name_zeromem) {");
    Ins(&R, allocmem.body    , "    memset(ret,0,size); // touch all bytes in the new memory block");
    Ins(&R, allocmem.body    , "}");
    Ins(&R, allocmem.body    , "return ret;");
}

void amc::tfunc_Sbrk_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& freemem = amc::CreateCurFunc();
    Ins(&R, freemem.ret  , "void", false);
    Ins(&R, freemem.proto, "$name_FreeMem($Parent, void *mem, u64 size)", false);
    Ins(&R, freemem.body, "#if defined(WIN32)");
    Ins(&R, freemem.body, "    if (size >= 1024*2048) { // big blocks come from _aligned_malloc");
    Ins(&R, freemem.body, "        _aligned_free(mem);");
    Ins(&R, freemem.body, "    } else {");
    Ins(&R, freemem.body, "        free(mem);");
    Ins(&R, freemem.body, "    }");
    Ins(&R, freemem.body, "#elif defined(__MACH__) || __FreeBSD__>0");
    Ins(&R, freemem.body, "    free(mem);");
    Ins(&R, freemem.body, "    (void)size;");
    Ins(&R, freemem.body, "#else");
    Ins(&R, freemem.body, "u64 bigsize = 1024*2048;");
    Ins(&R, freemem.body, "if (size >= bigsize) {");
    Ins(&R, freemem.body, "    munmap((void*)mem, size);");
    Ins(&R, freemem.body, "}");
    Ins(&R, freemem.body, "#endif");
}

void amc::tfunc_Sbrk_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    Ins(&R, init.body, "$parname.$name_huge_limit = 0;");
    Ins(&R, init.body, "$parname.$name_huge_alloc = 0;");
    Ins(&R, init.body, "$parname.$name_zeromem = false;");
}
