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
// Source: cpp/amc/lary.cpp
//
// Created By: alexei.lebedev jeffrey.wang
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev jeffrey.wang hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Lary() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    int nlevels = 32;
    Set(R, "$prealloc", "4");
    Set(R, "$nlevels", tempstr() << nlevels);
    Set(R, "$Rowid"  , EvalRowid(*field.p_arg));

    GenTclass(amc_tclass_Pool);
    InsVar(R, field.p_ctype, "$Cpptype*", "$name_lary[$nlevels]", "", "level array");
    InsVar(R, field.p_ctype, "i32", "$name_n", "", "number of elements in array");
    vrfy(field.p_ctype->p_ns->c_globfld != NULL
         , tempstr()<<"Lary "<<field.arg<<" requires global FDb._db in namespace "<<ns_Get(*field.p_ctype));
}

void amc::tfunc_Lary_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& alloc = amc::CreateCurFunc();
    Ins(&R, alloc.ret  , "void*", false);
    Ins(&R, alloc.proto, "$name_AllocMem($Parent)", false);
    Ins(&R, alloc.body, "u64 new_nelems     = $parname.$name_n+1;");
    Ins(&R, alloc.body, "// compute level and index on level");
    Ins(&R, alloc.body, "u64 bsr   = algo::u64_BitScanReverse(new_nelems);");
    Ins(&R, alloc.body, "u64 base  = u64(1)<<bsr;");
    Ins(&R, alloc.body, "u64 index = new_nelems-base;");
    Ins(&R, alloc.body, "void *ret = NULL;");
    Ins(&R, alloc.body, "// if level doesn't exist yet, create it");
    Ins(&R, alloc.body, "$Cpptype*  lev   = NULL;");
    Ins(&R, alloc.body, "if (bsr < $nlevels) {");
    Ins(&R, alloc.body, "    lev = $parname.$name_lary[bsr];");
    Ins(&R, alloc.body, "    if (!lev) {");
    Ins(&R, alloc.body, "        lev=($Cpptype*)$basepool_AllocMem(sizeof($Cpptype) * (u64(1)<<bsr));");
    Ins(&R, alloc.body, "        $parname.$name_lary[bsr] = lev;");
    Ins(&R, alloc.body, "    }");
    Ins(&R, alloc.body, "}");
    Ins(&R, alloc.body, "// allocate element from this level");
    Ins(&R, alloc.body, "if (lev) {");
    Ins(&R, alloc.body, "    $parname.$name_n = new_nelems;");
    Ins(&R, alloc.body, "    ret = lev + index;");
    Ins(&R, alloc.body, "}");
    Ins(&R, alloc.body, "return ret;");
}

void amc::tfunc_Lary_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

void amc::tfunc_Lary_Find() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& find = amc::CreateCurFunc();
    find.inl = true;
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Rowid t)", false);
    Ins(&R, find.body, "u64 x = t + 1;");
    Ins(&R, find.body, "u64 bsr   = algo::u64_BitScanReverse(x);");
    Ins(&R, find.body, "u64 base  = u64(1)<<bsr;");
    Ins(&R, find.body, "u64 index = x-base;");
    Ins(&R, find.body, "$Cpptype *retval = NULL;");
    Ins(&R, find.body, "if (LIKELY(u64(t) < u64($parname.$name_n))) {");
    Ins(&R, find.body, "    retval = &$parname.$name_lary[bsr][index];");
    Ins(&R, find.body, "}");
    Ins(&R, find.body, "return retval;");
}

void amc::tfunc_Lary_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    Ins(&R, init.body, "// initialize LAry $name ($field)");
    Ins(&R, init.body, "$parname.$name_n = 0;");
    Ins(&R, init.body, "memset($parname.$name_lary, 0, sizeof($parname.$name_lary)); // zero out all level pointers");
    Ins(&R, init.body, "$Cpptype* $name_first = ($Cpptype*)$basepool_AllocMem(sizeof($Cpptype) * (u64(1)<<$prealloc));");
    Ins(&R, init.body, "if (!$name_first) {");
    Ins(&R, init.body, "    FatalErrorExit(\"out of memory\");");
    Ins(&R, init.body, "}");
    Ins(&R, init.body, "for (int i = 0; i < $prealloc; i++) {");
    Ins(&R, init.body, "    $parname.$name_lary[i]  = $name_first;");
    Ins(&R, init.body, "    $name_first    += 1ULL<<i;");
    Ins(&R, init.body, "}");
}

void amc::tfunc_Lary_Last() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& last = amc::CreateCurFunc();
    last.inl = true;
    Ins(&R, last.ret  , "$Cpptype*", false);
    Ins(&R, last.proto, "$name_Last($Parent)", false);
    Ins(&R, last.body, "return $name_Find($pararg, $Rowid($parname.$name_n-1));");
}

void amc::tfunc_Lary_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

void amc::tfunc_Lary_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    bool dtor = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;

    if (!field.c_fnoremove) {
        amc::FFunc& removelast = amc::CreateCurFunc();
        Ins(&R, removelast.ret     , "void", false);
        Ins(&R, removelast.proto   , "$name_RemoveAll($Parent)", false);
        Set(R, "$partrace", Refname(*field.p_ctype));
        if (field.do_trace) {
            Ins(&R, removelast.body, "$ns::_db.trace.del_$partrace_$name += $parname.$name_n;");
        }
        if (!dtor) {
            Ins(&R, removelast.body, "$parname.$name_n = 0;");
        } else {
            Ins(&R, removelast.body, "for (u64 n = $parname.$name_n; n>0; ) {");
            Ins(&R, removelast.body, "    n--;");
            Ins(&R, removelast.body, "    $name_qFind($pararg, $Rowid(n)).~$Ctype(); // destroy last element");
            Ins(&R, removelast.body, "    $parname.$name_n = n;");
            Ins(&R, removelast.body, "}");
        }
    }
}

void amc::tfunc_Lary_RemoveLast() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    bool dtor = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;

    amc::FFunc& remlast = amc::CreateCurFunc();
    Set(R, "$partrace", Refname(*field.p_ctype));
    Ins(&R, remlast.ret     , "void", false);
    Ins(&R, remlast.proto   , "$name_RemoveLast($Parent)", false);
    Ins(&R, remlast.body    , "u64 n = $parname.$name_n;");
    Ins(&R, remlast.body    , "if (n > 0) {");
    Ins(&R, remlast.body    , "    n -= 1;");
    if (dtor) {
        Ins(&R, remlast.body, "    $name_qFind($pararg, $Rowid(n)).~$Ctype();");
    }
    if (field.do_trace) {
        Ins(&R, remlast.body, "    ++$ns::_db.trace.del_$partrace_$name;");
    }
    Ins(&R, remlast.body    , "    $parname.$name_n = n;");
    Ins(&R, remlast.body    , "}");
}

void amc::tfunc_Lary_RowidFind() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.reflect) {
        amc::FFunc& rowid_findx = amc::CreateCurFunc();
        rowid_findx.priv=true;
        Ins(&R, rowid_findx.ret  , "algo::ImrowPtr", false);
        Ins(&R, rowid_findx.proto, "$name_RowidFind(int t)", false);
        Ins(&R, rowid_findx.body, "return algo::ImrowPtr(u64($name_Find($pararg, $Rowid(t))));");
    }
}

void amc::tfunc_Lary_Uninit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (field.p_ctype == field.p_ctype->p_ns->c_globfld->p_ctype) {
        Ins(&R, uninit.body, "// skip destruction in global scope");
    } else {
        Ins(&R, uninit.body, "// destroy $field");
        Ins(&R, uninit.body, "// destroy all elements");
        Ins(&R, uninit.body, "$name_RemoveAll($pararg);");
        Ins(&R, uninit.body, "// destroy all levels. stop when NULL level is found -- there is nothing beyond it");
        Ins(&R, uninit.body, "$basepool_FreeMem($parname.$name_lary[0],sizeof($Cpptype) * (u64(1)<<$prealloc));");
        Ins(&R, uninit.body, "for (u64 i = $prealloc; i < $nlevels && $parname.$name_lary[i]; i++) {");
        Ins(&R, uninit.body, "    $basepool_FreeMem($parname.$name_lary[i],sizeof($Cpptype) * (u64(1)<<i));");
        Ins(&R, uninit.body, "}");
    }
}

void amc::tfunc_Lary_qFind() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& qfind = amc::CreateCurFunc();
    qfind.inl = true;
    Ins(&R, qfind.ret  , "$Cpptype&", false);
    Ins(&R, qfind.proto, "$name_qFind($Parent, $Rowid t)", false);
    Ins(&R, qfind.body, "u64 x = t + 1;");
    Ins(&R, qfind.body, "u64 bsr   = algo::u64_BitScanReverse(x);");
    Ins(&R, qfind.body, "u64 base  = u64(1)<<bsr;");
    Ins(&R, qfind.body, "u64 index = x-base;");
    Ins(&R, qfind.body, "return $parname.$name_lary[bsr][index];");
}

void amc::tfunc_Lary_qLast() {
    // algo_lib::Replscope &R = amc::_db.genfield.R;

    // amc::FFunc& qlast = amc::CreateCurFunc();
    // qlast.inl = true;
    // Ins(&R, qlast.ret  , "$Cpptype&", false);
    // Ins(&R, qlast.proto, "$name_qLast($Parent)", false);
    // Ins(&R, qlast.body, "return $name_qFind($pararg, $Rowid($parname.$name_n-1));");
}

void amc::tfunc_Lary_curs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;

    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");

    {
        Ins(&R, ns.curstext    , "");
        Ins(&R, ns.curstext    , "struct $Parname_$name_curs {// cursor");
        Ins(&R, ns.curstext    , "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext    , "    $Partype *parent;");
        Ins(&R, ns.curstext    , "    i64 index;");
        Ins(&R, ns.curstext    , "    $Parname_$name_curs(){ parent=NULL; index=0; }");
        Ins(&R, ns.curstext    , "};");
        Ins(&R, ns.curstext    , "");
    }

    {
        amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reset"));
        curs_reset.inl = true;
        Ins(&R, curs_reset.comment, "cursor points to valid item");
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "curs.parent = &parent;");
        Ins(&R, curs_reset.body, "curs.index = 0;");
    }

    {
        amc::FFunc& curs_validq = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.ValidQ"));
        curs_validq.inl = true;
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.index < $cursparent.$name_n;");
    }

    {
        amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Next"));
        curs_next.inl = true;
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
        Ins(&R, curs_next.body, "curs.index++;");
    }

    {
        amc::FFunc& curs_access = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Access"));
        curs_access.inl = true;
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return $name_qFind($curspararg, $Rowid(curs.index));");
    }
}
