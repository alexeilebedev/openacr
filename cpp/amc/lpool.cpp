// Copyright (C) 2023-2024 AlgoRND
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
// Exceptions: NO
// Source: cpp/amc/lpool.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Count # of allocated elements
static bool HaveCountQ(amc::FField &field) {
    return field.arg != "u8";
}

// -----------------------------------------------------------------------------

void amc::tclass_Lpool() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    Set(R, "$minlevels", tempstr()<<4);// lowest size levels to skip. i.e. 4 -> min allocsize is 2^4
    Set(R, "$nlevels", tempstr()<<36);// number of levels; 36 with minlevels:4 = (1<<40) = 1TB

    Ins(&R, *ns.hdr, "struct $name_Lpblock {");
    Ins(&R, *ns.hdr, "    $name_Lpblock* next;");
    Ins(&R, *ns.hdr, "};");

    // instance
    if (HaveCountQ(field)) {
        InsVar(R, field.p_ctype, "i64", "$name_n", "", "Number of elements");
    }
    InsVar(R, field.p_ctype, "$name_Lpblock*", "$name_free[$nlevels]", "", "Lpool levels");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void", "", "");
    AddProtoArg(func, "void*", "mem");
    AddProtoArg(func, "u64", "size");
    Ins(&R, func.body, "size = u64_Max(size,1ULL<<$minlevels);");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(size-1) + 1 - $minlevels;");
    Ins(&R, func.body, "if (mem && cell < $nlevels) {");
    Ins(&R, func.body, "    $name_Lpblock *temp = ($name_Lpblock*)mem; // push  singly linked list");
    Ins(&R, func.body, "    temp->next = $parname.$name_free[cell];");
    Ins(&R, func.body, "    $parname.$name_free[cell] = temp;");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "    $parname.$name_n--;");
    }
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void*","retval","NULL");
    AddProtoArg(func, "u64","size");
    Ins(&R, func.comment, "Allocate new piece of memory at least SIZE bytes long.");
    Ins(&R, func.comment, "If not successful, return NULL");
    Ins(&R, func.comment, "The allocated block is at least 1<<$minlevels");
    Ins(&R, func.comment, "The maximum allocation size is at most 1<<($nlevels+$minlevels)");
    Ins(&R, func.body, "size     = u64_Max(size,1<<$minlevels); // enforce alignment");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(size-1) + 1 - $minlevels;");
    Ins(&R, func.body, "if (cell < $nlevels) {");
    Ins(&R, func.body, "    u64 i    = cell;");
    Ins(&R, func.body, "    // try to find a block that's at least as large as required.");
    Ins(&R, func.body, "    // if found, remove from free list");
    Ins(&R, func.body, "    for (; i < $nlevels; i++) {");
    Ins(&R, func.body, "        $name_Lpblock *blk = $parname.$name_free[i];");
    Ins(&R, func.body, "        if (blk) {");
    Ins(&R, func.body, "            $parname.$name_free[i] = blk->next;");
    Ins(&R, func.body, "            retval = blk;");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    // if suitable size block is not found, create a new one");
    Ins(&R, func.body, "    // by requesting a block from the base allocator.");
    Ins(&R, func.body, "    if (UNLIKELY(!retval)) {");
    Ins(&R, func.body, "        i = u64_Max(cell, 21-$minlevels); // 2MB min -- allow huge page to be used");
    Ins(&R, func.body, "        retval = $basepool_AllocMem(1ULL<<(i+$minlevels));");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (LIKELY(retval)) {");
    Ins(&R, func.body, "        // if block is more than 2x as large as needed, return the upper half to the free");
    Ins(&R, func.body, "        // list (repeatedly). meanwhile, retval doesn't change.");
    Ins(&R, func.body, "        while (i > cell) {");
    Ins(&R, func.body, "            i--;");
    Ins(&R, func.body, "            int half = 1ULL<<(i+$minlevels);");
    Ins(&R, func.body, "            $name_Lpblock *blk = ($name_Lpblock*)((u8*)retval + half);");
    Ins(&R, func.body, "            blk->next = $parname.$name_free[i];");
    Ins(&R, func.body, "            $parname.$name_free[i] = blk;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "$parname.$name_n += retval != NULL;");
    }
    if (field.do_trace) {
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, func.body, "    ++$ns::_db.trace.alloc_$partrace_$name;");
    }
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_ReserveBuffers() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "bool", "retval", "true");
    AddProtoArg(func, "u64", "nbuf");
    AddProtoArg(func, "u64", "bufsize");
    Ins(&R, func.comment, "Reserve NBUF buffers of size BUFSIZE from the base pool ($basepool)");
    Ins(&R, func.body, "bufsize = u64_Max(bufsize, 1<<$minlevels);");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(bufsize-1) + 1 - $minlevels;");
    Ins(&R, func.body, "if (cell < $nlevels) {");
    Ins(&R, func.body, "    for (u64 i = 0; i < nbuf; i++) {");
    Ins(&R, func.body, "        u64 size = 1ULL<<(cell+$minlevels);");
    Ins(&R, func.body, "        $name_Lpblock *temp = ($name_Lpblock*)$basepool_AllocMem(size);");
    Ins(&R, func.body, "        if (temp == NULL) {");
    Ins(&R, func.body, "            retval = false;");
    Ins(&R, func.body, "            break;// why continue?");
    Ins(&R, func.body, "        } else {");
    Ins(&R, func.body, "            temp->next = $parname.$name_free[cell];");
    Ins(&R, func.body, "            $parname.$name_free[cell] = temp;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_ReallocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void*", "ret", "oldmem");
    AddProtoArg(func, "void*", "oldmem");
    AddProtoArg(func, "u64", "old_size");
    AddProtoArg(func, "u64", "new_size");
    Ins(&R, func.comment, "Allocate new block, copy old to new, delete old.");
    Ins(&R, func.comment, "If the new size is same as old size, do nothing.");
    Ins(&R, func.comment, "In all other cases, new memory is allocated (i.e. size reduction is not a no-op)");
    Ins(&R, func.comment, "If no memory, return NULL; old memory remains untouched");
    Ins(&R, func.body, "if (new_size != old_size) {");
    Ins(&R, func.body, "    ret = $name_AllocMem($pararg, new_size);");
    Ins(&R, func.body, "    if (ret && oldmem) {"); // check for oldmem if required for gcc -O2
    Ins(&R, func.body, "        memcpy(ret,oldmem,u64_Min(new_size,old_size));");
    Ins(&R, func.body, "        $name_FreeMem($pararg, oldmem, old_size);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    GenTclass(amc_tclass_Pool);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.body, "memset($parname.$name_free, 0, sizeof($parname.$name_free));");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "$parname.$name_n = 0;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "i64", "", "");
        }
        Ins(&R, func.body, "return $parname.$name_n;");
    }
}
