// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
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
// Source: cpp/amc/tpool.cpp
//
// See tex/amc/amc_alloc.tex

#include "include/amc.h"

static bool MtfreeQ(amc::FField &field) {
    (void)field;
    return false;
}

void amc::tclass_Tpool() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool mtfree = MtfreeQ(field);

    GenTclass(amc_tclass_Pool);

    InsVar(R, field.p_ctype    , "u64", "$name_blocksize", "", "# bytes per block");
    InsVar(R, field.p_ctype    , "$Cpptype*", "$name_free", "", "");
    InsVar(R, field.p_arg      , "$Cpptype*", "$name_next", "", "Pointer to next free element int tpool");
    if (mtfree) {
        InsVar(R, field.p_ctype, "$Cpptype* volatile", "$name_free_mt", "", "");
    }

    if (!GlobalQ(*field.p_ctype)) {
        prerr("amc.nonglobal_tpool"
              <<Keyval("field",field.field)
              <<Keyval("comment","Tpool doesn't return memory, so it must be global"));
        algo_lib::_db.exit_code=1;
    }

    // avoid coverity warning and initialize _next to -1 in child's ctor,
    // as opposed to Tpool's AllocMem
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));
    Ins(&R, child_init->body  , "$fname.$name_next = ($Cpptype*)-1; // ($field) not-in-tpool's freelist");
}

void amc::tfunc_Tpool_ReserveMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& reservemem = amc::CreateCurFunc();
    Ins(&R, reservemem.comment, "Allocate block of given size, break up into small elements and append to free list.");
    Ins(&R, reservemem.comment, "Return number of elements reserved.");
    Ins(&R, reservemem.ret  , "u64", false);
    Ins(&R, reservemem.proto, "$name_ReserveMem($Parent, u64 size)", false);
    Ins(&R, reservemem.body    , "u64 ret = 0;");
    Ins(&R, reservemem.body    , "if (size >= sizeof($Cpptype)) {");
    Ins(&R, reservemem.body    , "    $Cpptype *mem = ($Cpptype*)$basepool_AllocMem(size);");
    Ins(&R, reservemem.body    , "    ret = mem ? size / sizeof($Cpptype) : 0;");
    Ins(&R, reservemem.body    , "    // add newly allocated elements to the free list;");
    Ins(&R, reservemem.body    , "    for (u64 i=0; i < ret; i++) {");
    Ins(&R, reservemem.body    , "       mem[i].$name_next = $parname.$name_free;");
    Ins(&R, reservemem.body    , "       $parname.$name_free = mem+i;");
    Ins(&R, reservemem.body    , "    }");
    Ins(&R, reservemem.body    , "}");
    Ins(&R, reservemem.body    , "return ret;");
}

void amc::tfunc_Tpool_Reserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.comment, "Preallocate memory for N more elements");
    Ins(&R, reserve.comment, "Return number of elements actually reserved.");
    Ins(&R, reserve.ret  , "u64", false);
    Ins(&R, reserve.proto, "$name_Reserve($Parent, u64 n_elems)", false);
    Ins(&R, reserve.body, "u64 ret = 0;");
    Ins(&R, reserve.body, "while (ret < n_elems) {");
    Ins(&R, reserve.body, "    u64 size = $parname.$name_blocksize; // underlying allocator is probably Lpool");
    Ins(&R, reserve.body, "    u64 reserved = $name_ReserveMem($pararg, size);");
    Ins(&R, reserve.body, "    ret += reserved;");
    Ins(&R, reserve.body, "    if (reserved == 0) {");
    Ins(&R, reserve.body, "        break;");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "}");
    Ins(&R, reserve.body, "return ret;");
}

void amc::tfunc_Tpool_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool mtfree = MtfreeQ(field);

    amc::FFunc& allocmem = amc::CreateCurFunc();
    Ins(&R, allocmem.comment, "Allocate space for one element");
    Ins(&R, allocmem.comment, "If no memory available, return NULL.");
    Ins(&R, allocmem.ret  , "void*", false);
    Ins(&R, allocmem.proto, "$name_AllocMem($Parent)", false);
    Ins(&R, allocmem.body    , "$Cpptype *row = $parname.$name_free;");
    if (mtfree) {
        Ins(&R, allocmem.body, "// empty thread-safe free list.");
        Ins(&R, allocmem.body, "if (UNLIKELY(!row)) {");
        Ins(&R, allocmem.body, "    SwapMT($parname.$name_free_mt, row);");
        Ins(&R, allocmem.body, "}");
    }
    Ins(&R, allocmem.body    , "if (UNLIKELY(!row)) {");
    Ins(&R, allocmem.body    , "    $name_Reserve($pararg,1);");
    Ins(&R, allocmem.body    , "    row = $parname.$name_free;");
    Ins(&R, allocmem.body    , "}");
    Ins(&R, allocmem.body    , "if (row) {");
    Ins(&R, allocmem.body    , "    $parname.$name_free = row->$name_next;");
    Ins(&R, allocmem.body    , "}");
    Ins(&R, allocmem.body    , "return row;");
}

void amc::tfunc_Tpool_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool mtfree = MtfreeQ(field);

    amc::FFunc& freemem = amc::CreateCurFunc();
    Ins(&R, freemem.comment, "Remove mem from all global and cross indices, then deallocate mem");
    Ins(&R, freemem.ret  , "void", false);
    Ins(&R, freemem.proto, "$name_FreeMem($Parent, $Cpptype &row)", false);
    Ins(&R, freemem.body        , "if (UNLIKELY(row.$name_next != ($Cpptype*)-1)) {");
    Ins(&R, freemem.body        , "    FatalErrorExit(\"$ns.tpool_double_delete  pool:$field  comment:'double deletion caught'\");");
    Ins(&R, freemem.body        , "}");
    if (mtfree) {
        Ins(&R, freemem.body    , "// OK to free from another thread.");
        Ins(&R, freemem.body    , "$Cpptype* temp = $parname.$name_free_mt; // insert into thread-safe free list");
        Ins(&R, freemem.body    , "do {");
        Ins(&R, freemem.body    , "    row.$name_next =  temp;");
        Ins(&R, freemem.body    , "} while (!CAS($parname.$name_free_mt, temp, &row));");
    } else {
        Ins(&R, freemem.body    , "row.$name_next = $parname.$name_free; // insert into free list");
        Ins(&R, freemem.body    , "$parname.$name_free  = &row;");
    }
}

void amc::tfunc_Tpool_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    // initialization
    amc::FFunc& init = amc::CreateCurFunc();

    init.inl = false;
    Ins(&R, init.body, "// $name: initialize Tpool");
    if (MtfreeQ(field)) {
        Ins(&R, init.body, "$parname.$name_free_mt = NULL; // ($field)");
    }
    Ins(&R, init.body, "$parname.$name_free      = NULL;");
    Ins(&R, init.body, "$parname.$name_blocksize = algo::BumpToPow2(64 * sizeof($Cpptype)); // allocate 64-127 elements at a time");
}
