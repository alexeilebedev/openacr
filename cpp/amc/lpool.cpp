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
// Source: cpp/amc/lpool.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Count # of allocated elements
static bool HaveCountQ(amc::FField &field) {
    return field.arg != "u8";
}

// -----------------------------------------------------------------------------

void amc::tclass_Lpool() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    int nlevels=31;
    Set(R, "$nlevels", tempstr()<<nlevels);

    Ins(&R, *ns.hdr, "struct $name_Lpblock {");
    Ins(&R, *ns.hdr, "    $name_Lpblock* next;");
    Ins(&R, *ns.hdr, "};");

    // instance
    if (HaveCountQ(field)) {
        InsVar(R, field.p_ctype, "i64", "$name_n", "", "Number of elements");
    }
    InsVar(R, field.p_ctype, "$name_Lpblock*", "$name_free[$nlevels]", "", "Lpool levels");
    InsVar(R, field.p_ctype, "u32", "$name_lock", "", "Lpool lock");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_FreeMem($Parent, void *mem, u64 size)", false);
    Ins(&R, func.body, "if (mem) {");
    Ins(&R, func.body, "    size = u64_Max(size,16); // enforce alignment");
    Ins(&R, func.body, "    u64 cell = algo::u64_BitScanReverse(size-1) + 1;");
    Ins(&R, func.body, "    $name_Lpblock *temp = ($name_Lpblock*)mem; // push  singly linked list");
    Ins(&R, func.body, "    temp->next = $parname.$name_free[cell];");
    Ins(&R, func.body, "    $parname.$name_free[cell] = temp;");
    if (HaveCountQ(*amc::_db.genfield.p_field)) {
        Ins(&R, func.body, "    $parname.$name_n--;");
    }
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void*", false);
    Ins(&R, func.proto, "$name_AllocMem($Parent, u64 size)", false);
    Ins(&R, func.comment, "Allocate new piece of memory at least SIZE bytes long.");
    Ins(&R, func.comment, "If not successful, return NULL");
    Ins(&R, func.comment, "The allocated block is 16-byte aligned");
    Ins(&R, func.body, "size     = u64_Max(size,16); // enforce alignment");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(size-1)+1;");
    Ins(&R, func.body, "u64 i    = cell;");
    Ins(&R, func.body, "u8 *retval = NULL;");
    Ins(&R, func.body, "// try to find a block that's at least as large as required.");
    Ins(&R, func.body, "// if found, remove from free list");
    Ins(&R, func.body, "for (; i < $nlevels; i++) {");
    Ins(&R, func.body, "    $name_Lpblock *blk = $parname.$name_free[i];");
    Ins(&R, func.body, "    if (blk) {");
    Ins(&R, func.body, "        $parname.$name_free[i] = blk->next;");
    Ins(&R, func.body, "        retval = (u8*)blk;");
    Ins(&R, func.body, "        break;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "// if suitable size block is not found, create a new one");
    Ins(&R, func.body, "// by requesting a block from the base allocator.");
    Ins(&R, func.body, "if (UNLIKELY(!retval)) {");
    Ins(&R, func.body, "    i = u64_Max(cell, 21); // 2MB min -- allow huge page to be used");
    Ins(&R, func.body, "    retval = (u8*)$basepool_AllocMem(1<<i);");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (LIKELY(retval)) {");
    Ins(&R, func.body, "    // if block is more than 2x as large as needed, return the upper half to the free");
    Ins(&R, func.body, "    // list (repeatedly). meanwhile, retval doesn't change.");
    Ins(&R, func.body, "    while (i > cell) {");
    Ins(&R, func.body, "        i--;");
    Ins(&R, func.body, "        int half = 1<<i;");
    Ins(&R, func.body, "        $name_Lpblock *blk = ($name_Lpblock*)(retval + half);");
    Ins(&R, func.body, "        blk->next = $parname.$name_free[i];");
    Ins(&R, func.body, "        $parname.$name_free[i] = blk;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    if (HaveCountQ(*amc::_db.genfield.p_field)) {
        Ins(&R, func.body, "$parname.$name_n += retval != NULL;");
    }
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.do_trace) {
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, func.body, "    ++$ns::_db.trace.alloc_$partrace_$name;");
    }
    Ins(&R, func.body, "return retval;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_ReserveBuffers() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.proto, "$name_ReserveBuffers($Parent, int nbuf, u64 bufsize)", false);
    Ins(&R, func.body, "bool retval = true;");
    Ins(&R, func.body, "bufsize = u64_Max(bufsize, 16);");
    Ins(&R, func.body, "for (int i = 0; i < nbuf; i++) {");
    Ins(&R, func.body, "    u64     cell = algo::u64_BitScanReverse(bufsize-1)+1;");
    Ins(&R, func.body, "    u64     size = 1ULL<<cell;");
    Ins(&R, func.body, "    $name_Lpblock *temp = ($name_Lpblock*)$basepool_AllocMem(size);");
    Ins(&R, func.body, "    if (temp == NULL) {");
    Ins(&R, func.body, "        retval = false;");
    Ins(&R, func.body, "        break;// why continue?");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        temp->next = $parname.$name_free[cell];");
    Ins(&R, func.body, "        $parname.$name_free[cell] = temp;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return retval;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_ReallocMem() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void*", false);
    Ins(&R, func.proto, "$name_ReallocMem($Parent, void *oldmem, u64 old_size, u64 new_size)", false);
    Ins(&R, func.comment, "Allocate new block, copy old to new, delete old.");
    Ins(&R, func.comment, "New memory is always allocated (i.e. size reduction is not a no-op)");
    Ins(&R, func.comment, "If no memory, return NULL: old memory untouched");
    Ins(&R, func.body, "void* ret = oldmem;");
    Ins(&R, func.body, "if (new_size != old_size) {");
    Ins(&R, func.body, "    ret = $name_AllocMem($pararg, new_size);");
    Ins(&R, func.body, "    if (ret && oldmem) {"); // check for oldmem if required for gcc -O2
    Ins(&R, func.body, "        memcpy(ret,oldmem,u64_Min(new_size,old_size));");
    Ins(&R, func.body, "        $name_FreeMem($pararg, oldmem, old_size);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");

    GenTclass(amc_tclass_Pool);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.body, "$parname.$name_lock = 0;");
    Ins(&R, func.body, "memset($parname.$name_free, 0, sizeof($parname.$name_free));");
    if (HaveCountQ(*amc::_db.genfield.p_field)) {
        Ins(&R, func.body, "$parname.$name_n = 0;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    if (HaveCountQ(*amc::_db.genfield.p_field)) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "i64", "", "");
        }
        Ins(&R, func.body, "return $parname.$name_n;");
    }
}
