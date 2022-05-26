// (C) AlgoEngineering LLC 2008-2012
// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/sbrk.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

void amc::tclass_Sbrk() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    InsVar(R, field.p_ctype, "u64", "$name_huge_limit", "", "Huge page limit in bytes -- set to 0 with fork!");
    InsVar(R, field.p_ctype, "u64", "$name_huge_alloc", "", "Huge page bytes allocated");
    InsVar(R, field.p_ctype, "bool", "$name_zeromem", "", "Zero allocated memory");
}

void amc::tfunc_Sbrk_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& allocmem = amc::CreateCurFunc();
    Ins(&R, allocmem.ret     , "void*", false);
    Ins(&R, allocmem.proto   , "$name_AllocMem($Parent, u32 size)", false);
    Ins(&R, allocmem.comment , "Allocate a new piece of memory at least SIZE bytes long.");
    Ins(&R, allocmem.comment , "If out of memory, return NULL");
    Ins(&R, allocmem.comment , "Newly allocated memory is initialized to zeros");
    Ins(&R, allocmem.body    , "void *ret;");
    Ins(&R, allocmem.body    , "#if defined(__MACH__) || __FreeBSD__>0 || __CYGWIN__>0 || defined(WIN32)");
    Ins(&R, allocmem.body    , "    ret = malloc(size);");
    Ins(&R, allocmem.body    , "#else");
    Ins(&R, allocmem.body    , "ret = MAP_FAILED;");
    Ins(&R, allocmem.body    , "u32 bigsize = 1024*2048;");
    Ins(&R, allocmem.body    , "if (size >= bigsize) { // big block -- will be registered");
    Ins(&R, allocmem.body    , "    size = (size + bigsize - 1) / bigsize * bigsize;");
    Ins(&R, allocmem.body    , "    int prot = PROT_READ | PROT_WRITE;");
    Ins(&R, allocmem.body    , "    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE;");
    Ins(&R, allocmem.body    , "    bool use_huge = $parname.$name_huge_alloc + size < $parname.$name_huge_limit;");
    Ins(&R, allocmem.body    , "    if (use_huge) {");
    Ins(&R, allocmem.body    , "        ret = mmap(0x0, size, prot, flags | MAP_HUGETLB, 0, 0); // try huge pages");
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
    Ins(&R, allocmem.body    , "    if (ret == MAP_FAILED) { // try regular mmap");
    Ins(&R, allocmem.body    , "        $parname.$name_huge_limit = 0; // don't try huge pages again");
    Ins(&R, allocmem.body    , "        ret = mmap(0x0, size, prot, flags, 0, 0);");
    Ins(&R, allocmem.body    , "    } else {");
    Ins(&R, allocmem.body    , "        ++algo_lib::_db.trace.n_mmap; // mmap counter");
    Ins(&R, allocmem.body    , "    }");
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
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& freemem = amc::CreateCurFunc();
    Ins(&R, freemem.ret  , "void", false);
    Ins(&R, freemem.proto, "$name_FreeMem($Parent, void *mem, u32 size)", false);
    Ins(&R, freemem.body, "#if defined(__MACH__) || __FreeBSD__>0 || defined(WIN32)");
    Ins(&R, freemem.body, "    free(mem);");
    Ins(&R, freemem.body, "    (void)size;");
    Ins(&R, freemem.body, "#else");
    Ins(&R, freemem.body, "u32 bigsize = 1024*2048;");
    Ins(&R, freemem.body, "if (size >= bigsize) {");
    Ins(&R, freemem.body, "    munmap((void*)mem, size);");
    Ins(&R, freemem.body, "}");
    Ins(&R, freemem.body, "#endif");
}

void amc::tfunc_Sbrk_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    Ins(&R, init.body, "$parname.$name_huge_limit = 0;");
    Ins(&R, init.body, "$parname.$name_huge_alloc = 0;");
    Ins(&R, init.body, "$parname.$name_zeromem = false;");
}
