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
// Exceptions: NO
// Source: cpp/amc/inlary.cpp -- Inline array
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// It is possible to run into formatting problems
// with Inlary separators.
// If the child element being printed uses the same chars as the separator,
// round tripping will not be possible anymore.
// We can't check all cases, but we check some.
static void CheckSep(amc::FField &field) {
    char sep = GetSep(field);
    amc::FCfmt *cfmt = FindStringRead(*field.p_ctype);
    if (cfmt && !sep && field.arg != "char" && field.arg != "u8") {
        prerr("amc.inlary_not_printed"
              <<Keyval("inlary",field.field)
              <<Keyval("cfmt",cfmt->cfmt)
              <<Keyval("comment","please define separator character to allow printing inlary"));
        algo_lib::_db.exit_code=1;
    }
    if (sep) {
        amc::FCfmt *childfmt = FindStringRead(*field.p_arg);
        if (childfmt) {
            if ((sep == ' ' || sep == ':') && (childfmt->printfmt == dmmeta_Printfmt_printfmt_Tuple)) {
                prerr("amc.bad_inlary_print"
                      <<Keyval("inlary",field.field)
                      <<Keyval("elem_type",field.arg)
                      <<Keyval("comment","Inlary element separator incompatible with Tuple format of child elements"));
                algo_lib::_db.exit_code=1;
            }
            if ((tempstr()<<sep) == (tempstr()<<childfmt->sep) && (childfmt->printfmt  == dmmeta_Printfmt_printfmt_Sep)) {
                prerr("amc.bad_inlary_print2"
                      <<Keyval("inlary",field.field)
                      <<Keyval("elem_type",field.arg)
                      <<Keyval("sep",sep)
                      <<Keyval("comment","Inlary element separator conflicts with separator used by child elements"));
                algo_lib::_db.exit_code=1;
            }
        }
    }
}

// -----------------------------------------------------------------------------

static bool FixedQ(amc::FField &field) {
    amc::FInlary&        inlary     = *field.c_inlary;
    return  inlary.max == inlary.min;
}

// -----------------------------------------------------------------------------

void amc::tclass_Inlary() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = FixedQ(field);

    // Check print misconfiguration
    CheckSep(field);

    Set(R, "$Rowid"  , EvalRowid(*field.p_arg));
    inlary.max = i64_Max(inlary.max, c_static_N(*field.p_arg));
    vrfy(inlary.max > 0, "unknown size of inline array: set width or add gstatic");
    Set(R, "$min", tempstr() << inlary.min);
    Set(R, "$max", tempstr() << inlary.max);
    Set(R, "$lenexpr", fixed ? "$max" : "$parname.$name_n");
    Set(R, "$dflt"   , strptr(field.dflt.value));
    Set(R, "$parelems", fixed ? "$parname.$name_elems" : "reinterpret_cast<$Cpptype*>($parname.$name_data)");

    if (fixed) {
        vrfy(!FldfuncQ(field), "computed field not allowed here");
        InsVar(R, field.p_ctype, "$Cpptype", "$name_elems[$lenexpr]", strptr(field.dflt.value), "fixed array");
    } else {
        GenTclass(amc_tclass_Pool);
        if (PackQ(*field.p_arg)) {
            InsVar(R, field.p_ctype, "u8", "$name_data[sizeof($Cpptype) * $max]", "", "place for data");
        } else {
            InsVar(R, field.p_ctype, "u128", "$name_data[sizeu128($Cpptype,$max)]", "", "place for data");
        }
        InsVar(R, field.p_ctype, "i32", "$name_n", "", "number of elems current in existence");
        InsStruct(R, field.p_ctype, "enum { $name_max = $max };");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_AllocMem() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& allocmem = amc::CreateCurFunc();
        allocmem.inl = true;
        Ins(&R, allocmem.ret  , "void*", false);
        Ins(&R, allocmem.proto, "$name_AllocMem($Parent)", false);
        Ins(&R, allocmem.body, "void *row = $parelems + $parname.$name_n;");
        Ins(&R, allocmem.body, "if ($parname.$name_n == $max) row = NULL;");
        Ins(&R, allocmem.body, "if (row) $parname.$name_n++;");
        Ins(&R, allocmem.body, "return row;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_EmptyQ() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& emptyq = amc::CreateCurFunc();
        Ins(&R, emptyq.ret  , "bool", false);
        Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
        Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Fill() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (fixed && !PadQ(field) && field.p_arg->n_xref==0 && !CopyPrivQ(*field.p_arg)) {
        amc::FFunc& fill = amc::CreateCurFunc();
        fill.inl=true;
        Ins(&R, fill.ret  , "void", false);
        Ins(&R, fill.proto, "$name_Fill($Parent, const $Cpptype &rhs)", false);
        Ins(&R, fill.body, "for (int i = 0; i < $lenexpr; i++) {");
        Ins(&R, fill.body, "    $parelems[i] = rhs;");
        Ins(&R, fill.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Find() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;

    if (!PadQ(field)) {
        amc::FFunc& find = amc::CreateCurFunc();
        find.inl = true;
        Ins(&R, find.ret  , "$Cpptype*", false);
        Ins(&R, find.proto, "$name_Find($Parent, $Rowid t)", false);
        Ins(&R, find.body, "u64 idx = t;");
        Ins(&R, find.body, "u64 lim = $lenexpr;");
        Ins(&R, find.body, "return idx < lim ? $parelems + idx : NULL; // unsigned comparison with limit");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Getary() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!PadQ(field)) {
        amc::FFunc& getary = amc::CreateCurFunc();
        Ins(&R, getary.ret  , "algo::aryptr<$Cpptype>", false);
        Ins(&R, getary.proto, "$name_Getary($Parent)", false);
        if (!fixed) {
            Ins(&R, getary.comment, "Return array pointer by value");
        } else {
            Ins(&R, getary.comment, "Access fixed array $name as aryptr.", false);
        }
        Ins(&R, getary.body, "return algo::aryptr<$Cpptype>($parelems, $lenexpr);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Init() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& init = amc::CreateCurFunc();
        init.inl = false;
        Ins(&R, init.body, "$parname.$name_n = 0; // $name: initialize count");
        if (inlary.min > 0) {
            // preallocate N elements
            Set(R, "$min", tempstr() << inlary.min);
            Ins(&R, init.body, "// min size");
            Ins(&R, init.body, "for (int i = 0; i < $min; i++) {");
            Ins(&R, init.body, "    $name_Alloc($pararg);");
            Ins(&R, init.body, "}");
        }
    }
    if (fixed && ch_N(field.dflt.value)) { // initialize fixary
        amc::FFunc& init = amc::CreateCurFunc();
        init.inl = inlary.max < 64;
        Ins(&R, init.body, "for (int i = 0; i < $lenexpr; i++) {");
        Ins(&R, init.body, "    $parelems[i] = $dflt;");
        Ins(&R, init.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Eq() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;

    if (field.p_ctype->c_ccmp && field.arg != "pad_byte") {
        amc::FFunc& opeq = amc::CreateCurFunc();
        opeq.inl = WidthMax(field)<6;
        Ins(&R, opeq.proto, "$name_Eq($Parent, $Partype &rhs)", false);
        Ins(&R, opeq.ret, "bool", false);
        Ins(&R, opeq.body, "int len = $lenexpr;");
        if (amc::FixaryQ(field)) {
        } else {
            Ins(&R, opeq.body, "if (len != $name_N(rhs)) {");// short-circuit
            Ins(&R, opeq.body, "    return false;");
            Ins(&R, opeq.body, "}");
        }
        Ins(&R, opeq.body    , "for (int i = 0; i < len; i++) {");
        Ins(&R, opeq.body    , "    if (!($parelems[i] == $name_qFind(rhs,i))) {");
        Ins(&R, opeq.body    , "        return false;");
        Ins(&R, opeq.body    , "    }");
        Ins(&R, opeq.body    , "}");
        Ins(&R, opeq.body    , "return true;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Cmp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    if (field.p_ctype->c_ccmp && field.arg != "pad_byte") {
        Set(R, "$Fldtype", field.cpp_type);
        amc::FFunc& opeq = amc::CreateCurFunc();
        opeq.inl = WidthMax(field)<6;
        Ins(&R, opeq.proto, "$name_Cmp($Parent, $Partype &rhs)", false);
        Ins(&R, opeq.ret, "int", false);
        if (amc::FixaryQ(field)) {
            Ins(&R, opeq.body, "int len = $max;");
        } else {
            Ins(&R, opeq.body, "int len = i32_Min($name_N($pararg), $name_N(rhs));");
        }
        Ins(&R, opeq.body    , "int retval = 0;");
        Ins(&R, opeq.body    , "for (int i = 0; i < len; i++) {");
        Ins(&R, opeq.body    , "    retval = $Fldtype_Cmp($parelems[i], $name_qFind(rhs,i));");
        Ins(&R, opeq.body    , "    if (retval != 0) {");
        Ins(&R, opeq.body    , "        return retval;");
        Ins(&R, opeq.body    , "    }");
        Ins(&R, opeq.body    , "}");
        if (amc::FixaryQ(field)) {
            Ins(&R, opeq.body, "return 0;");
        } else {
            Ins(&R, opeq.body, "return i32_Cmp($name_N($pararg), $name_N(rhs));");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Max() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& maxitems = amc::CreateCurFunc();
        maxitems.inl = true;
        Ins(&R, maxitems.comment, "Return constant $max -- max. number of items in the pool");
        Ins(&R, maxitems.ret  , "i32", false);
        Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
        if (!GlobalQ(*field.p_ctype)) {
            Ins(&R, maxitems.body, "(void)$pararg;");
        }
        Ins(&R, maxitems.body, "return $max;");
    }
    if (fixed) {
        // max items -- same as N, but semantically different.
        amc::FFunc& max = amc::CreateCurFunc();
        max.inl = true;
        Ins(&R, max.comment, "Return max number of items in the array");
        Ins(&R, max.ret  , "i32", false);
        Ins(&R, max.proto, "$name_Max($Parent)", false);
        if (!GlobalQ(*field.p_ctype)) {
            Ins(&R, max.body, "(void)$pararg;");
        }
        Ins(&R, max.body, "return $max;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_N() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    MaybeUnused(nitems,Subst(R,"$parname"));
    Ins(&R, nitems.body, "return $lenexpr;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_RemoveAll() {
    algo_lib::Replscope &R      = amc::_db.genctx.R;
    amc::FField         &field  = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary = *field.c_inlary;
    bool                 dtor   = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor) ||!field.p_arg->c_cpptype;
    bool                 fixed  = inlary.max == inlary.min;

    if (!fixed && !field.c_fnoremove) {
        amc::FFunc& removeall = amc::CreateCurFunc();
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, removeall.ret  , "void", false);
        Ins(&R, removeall.proto, "$name_RemoveAll($Parent)", false);
        if (field.do_trace) {
            Ins(&R, removeall.body, "++$ns::_db.trace.del_$partrace_$name;");
        }
        if (!dtor) {
            Ins(&R, removeall.body, "$parname.$name_n = 0;");
        } else {
            Ins(&R, removeall.body, "for (u64 n = $parname.$name_n; n>0; ) {");
            Ins(&R, removeall.body, "    n--;");
            Ins(&R, removeall.body, "    $parelems[n].~$Ctype(); // destroy last element");
            Ins(&R, removeall.body, "    $parname.$name_n=n;");
            Ins(&R, removeall.body, "}");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_RemoveLast() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 dtor       = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& remlast = amc::CreateCurFunc();
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, remlast.ret     , "void", false);
        Ins(&R, remlast.proto   , "$name_RemoveLast($Parent)", false);
        Ins(&R, remlast.body    , "u64 n = $parname.$name_n;");
        Ins(&R, remlast.body    , "if (n > 0) {");
        Ins(&R, remlast.body    , "    n -= 1;");
        if (field.do_trace) {
            Ins(&R, remlast.body, "    ++$ns::_db.trace.del_$partrace_$name;");
        }
        if (dtor) {
            Ins(&R, remlast.body, "    $parelems[n].~$Ctype();");
        }
        Ins(&R, remlast.body    , "    $parname.$name_n = n;");
        Ins(&R, remlast.body    , "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_RowidFind() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    if (field.reflect) {
        amc::FFunc& rowid_findx = amc::CreateCurFunc();
        rowid_findx.priv=true;
        Ins(&R, rowid_findx.ret  , "algo::ImrowPtr", false);
        Ins(&R, rowid_findx.proto, "$name_RowidFind(int t)", false);
        Ins(&R, rowid_findx.body, "return algo::ImrowPtr(u64($name_Find($pararg, $Rowid(t))));");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Setary() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    if (!PadQ(field) && field.p_arg->n_xref==0 && !CopyPrivQ(*field.p_arg)) {
        amc::FFunc& setary = amc::CreateCurFunc();
        setary.inl=true;
        Ins(&R, setary.ret  , "void", false);
        Ins(&R, setary.proto, "$name_Setary($Parent, const algo::aryptr<$Cpptype> &rhs)", false);
        Ins(&R, setary.body, "int n = i32_Min($max, rhs.n_elems);");
        if (field.p_arg->plaindata) {
            Ins(&R, setary.body, "memcpy($parelems, rhs.elems, sizeof($Cpptype)*n);");
        } else {
            Ins(&R, setary.body, "for (int i = 0; i < n; i++) {");
            Ins(&R, setary.body, "    $parelems[i] = rhs[i];");
            Ins(&R, setary.body, "}");
        }
        if (inlary.min > 0 && inlary.min < inlary.max) {
            Ins(&R, setary.body, "$parname.$name_n = i32_Max(n,$min);");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_Uninit() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 glob       = GlobalQ(*field.p_ctype);
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& uninit = amc::CreateCurFunc();
        if (!glob) {
            Ins(&R, uninit.body, "$name_RemoveAll($pararg);");
        } else {
            Ins(&R, uninit.body, "// skip destruction in global scope");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_qFind() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    if (!PadQ(field)) {
        amc::FFunc& qfind = amc::CreateCurFunc();
        qfind.inl = true;
        Ins(&R, qfind.ret  , "$Cpptype&", false);
        Ins(&R, qfind.proto, "$name_qFind($Parent, $Rowid t)", false);
        Ins(&R, qfind.body, "return $parelems[u64(t)];");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_rowid_Get() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FInlary&        inlary     = *field.c_inlary;
    bool                 fixed      = inlary.max == inlary.min;

    if (!fixed) {
        amc::FFunc& rowid_get = amc::CreateCurFunc();
        rowid_get.inl = true;
        Ins(&R, rowid_get.ret  , "$Rowid", false);
        Ins(&R, rowid_get.proto, "$name_rowid_Get($Parent, $Cpptype &row)", false);
        Ins(&R, rowid_get.body, "u64 ret = u64(&row - $parelems);");
        Ins(&R, rowid_get.body, "return $Rowid(ret);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Inlary_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    amc::FInlary& inlary = *field.c_inlary;
    bool glob = GlobalQ(*field.p_ctype);
    bool fixed = inlary.max == inlary.min;

    if (!PadQ(field)) {
        Set(R, "$Rowid"  , EvalRowid(*field.p_arg));
        // generate cursor
        Set(R, "$curspar", (glob ? "" : "(*curs.parent)"));

        Ins(&R, ns.curstext, "");
        Ins(&R, ns.curstext, "struct $Parname_$name_curs {// cursor");
        Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext, "    int index;");
        Ins(&R, ns.curstext, "    $Partype *parent;");
        Ins(&R, ns.curstext, "    $Parname_$name_curs() { parent=NULL; index=0; }");
        Ins(&R, ns.curstext, "};");
        Ins(&R, ns.curstext, "");

        {
            amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
            Ins(&R, curs_reset.comment, "cursor points to valid item");
            Ins(&R, curs_reset.ret  , "void", false);
            Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
            Ins(&R, curs_reset.body, "curs.parent = &parent;");
            Ins(&R, curs_reset.body, "curs.index = 0;");
        }

        {
            amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
            Ins(&R, curs_validq.comment, "cursor points to valid item");
            Ins(&R, curs_validq.ret  , "bool", false);
            Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
            if (fixed) {
                Ins(&R, curs_validq.body, "return u64(curs.index) < u64($max);");
            } else {
                Ins(&R, curs_validq.body, "return u64(curs.index) < u64(curs.parent->$name_n);");
            }
        }

        {
            amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
            Ins(&R, curs_next.comment, "proceed to next item");
            Ins(&R, curs_next.ret  , "void", false);
            Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
            Ins(&R, curs_next.body, "curs.index++;");
        }

        {
            amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
            Ins(&R, curs_access.comment, "item access");
            Ins(&R, curs_access.ret  , "$Cpptype&", false);
            Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
            Ins(&R, curs_access.body, "return $name_qFind($curspar, $Rowid(curs.index));");
        }
    }
}

// -----------------------------------------------------------------------------

// Read/Accumulate inline array from string.
// For fixed arrays:
//   multiple reads into fixed array will leave old data behind
//   (e.g. reading 1 element after reading 2 elements will leave the 2nd element from initial read)
//   we don't clear the tail of the array with default values.
//
// arg:char or u8
//    the array is flushed befor reading
//    input is a string that is copied to the array.
//    if the input is too large, it is silently truncated
// any other type, with separator:
//    the array is flushed before reading
//    input string is split on separator character, elements are appended one by one
//    if the input is too large, it is silently truncated
// any other type, without separator:
//    one element is read from input string and appended to the array without flushing.
//    if the element doesn't fit, function returns false.
//    (for fixed array, element #0 is read)
//
void amc::tfunc_Inlary_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    char sep = GetSep(field);
    if (!PadQ(field) && (field.arg=="char" || field.arg=="u8" || HasStringReadQ(*field.p_arg))) {
        Set(R, "$sep", char_ToCppSingleQuote(sep));
        Set(R, "$min", tempstr() << field.c_inlary->min);
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.comment, "Convert string to field. Return success value");
        AddRetval(func, "bool", "retval", "true");
        AddProtoArg(func, "algo::strptr", "in_str");
        if (field.arg == "char" || field.arg == "u8") {
            Ins(&R, func.body , "i32 newlen = i32_Min(in_str.n_elems, $max);");
            Ins(&R, func.body , "memcpy($parelems, in_str.elems, newlen);");
            if (!FixedQ(field)) {
                Ins(&R, func.body , "$pararg.$name_n = newlen;");
            }
            // retval is true
            // always succeed -- even if clipping of input string took place.
        } else if (sep) {
            Set(R, "$Fldcpptype", field.p_arg->cpp_type);
            if (!FixedQ(field)) {
                Ins(&R, func.body, "$name_RemoveAll($pararg);");
            }
            Ins(&R, func.body , "for (int i=0; in_str != \"\" && i < $max; i++) {");
            Ins(&R, func.body , "    algo::strptr token;");
            Ins(&R, func.body , "    algo::NextSep(in_str, $sep, token);");
            if (!FixedQ(field)) {
                Ins(&R, func.body , "    if (i >= $min) { // make room for new element...");
                Ins(&R, func.body , "        $Cpptype &elem = $name_Alloc($pararg);");
                Ins(&R, func.body , "        (void)elem; // will succeed due to a previous check");
                Ins(&R, func.body , "    }");
            }
            Ins(&R, func.body , "    retval = $Cpptype_ReadStrptrMaybe($parelems[i], token);");
            Ins(&R, func.body , "    if (!retval) {");
            if (!FixedQ(field)) {
                Ins(&R, func.body , "        $name_RemoveLast($pararg);");
            }
            Ins(&R, func.body , "        break;");
            Ins(&R, func.body , "    }");
            Ins(&R, func.body , "}");
        } else if (FixedQ(field)) {
            Ins(&R, func.body , "if ($max>0) {");
            Ins(&R, func.body , "    retval = $Cpptype_ReadStrptrMaybe($parelems[0], in_str);");
            Ins(&R, func.body , "}");
        } else {
            Ins(&R, func.body , "retval = $name_N($pararg) < $max;");
            Ins(&R, func.body , "if (retval) {");
            Ins(&R, func.body , "    $Cpptype &elem = $name_Alloc($pararg);");
            Ins(&R, func.body , "    retval = $Cpptype_ReadStrptrMaybe(elem, in_str);");
            Ins(&R, func.body , "    if (!retval) {");
            Ins(&R, func.body , "        $name_RemoveLast($pararg);");
            Ins(&R, func.body , "    }");
            Ins(&R, func.body , "}");
        }
    }
}

// -----------------------------------------------------------------------------

// Print array to string
// char/u8 are printed as-is
// pad_byte is ignored
// all other types are printed separated by SEP
// if none of the above conditions are present, the print function is not generated.
// This function is reused by Tary and Varlen, so it cannot really assume
// we're dealing with an Inlary
void amc::tfunc_Inlary_Print() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    char sep = GetSep(field);
    if (!PadQ(field) && (field.arg=="char" || field.arg=="u8" || sep)) {// ignore pad_byte
        Set(R, "$sep", char_ToCppSingleQuote(sep));
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.comment, "Convert $name to a string.");
        AddRetval(func, "void", "", "");
        AddProtoArg(func, "algo::cstring &", "rhs");
        if (field.arg == "char") {// print array of chars as a string
            Ins(&R, func.comment, "Array is printed as a regular string.");
            Ins(&R, func.body, "rhs << $name_Getary($pararg);");
        } else if (field.arg == "u8") {// contiguous array of u8 as hex chars
            Ins(&R, func.comment, "Array is printed as a regular string.");
            Ins(&R, func.body, "rhs << algo::memptr_ToStrptr($name_Getary($pararg));");
        } else if (sep) {
            Ins(&R, func.comment, "The separator character is $sep.");
            Ins(&R, func.body, "ind_beg($Parname_$name_curs,$name_elem,$parname) {");
            Ins(&R, func.body, "    if (ind_curs($name_elem).index > 0) {");
            Ins(&R, func.body, "        rhs << $sep;");
            Ins(&R, func.body, "    }");
            Ins(&R, func.body, "    $Cpptype_Print($name_elem, rhs);");
            Ins(&R, func.body, "}ind_end;");
        } else {
            Ins(&R, func.body, "// unable to print inlary");
        }
    }
}
