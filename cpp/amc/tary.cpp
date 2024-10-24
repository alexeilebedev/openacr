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
// Source: cpp/amc/tary.cpp -- Tary (vector) reftype
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Tary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    vrfy(field.c_tary, "tary required");

    Set(R, "$dflt", field.dflt.value);

    Set(R, "$Rowid"  , EvalRowid(*field.p_arg));
    if (field.p_arg->n_xref > 0) {
        prerr("A row allocated using Tary cannot have pointers to it.");
        prerr("    Rows allocated via Tary are movable, so pointers to them are not allowed.");
        prerr("    Consider using Lary");
        prerr("dmmeta.tary"
              <<Keyval("tary",field.field));
        ind_beg(amc::_db_xref_curs, xref,amc::_db) if (xref.p_field->p_arg == field.p_arg) {
            prerr("dmmeta.xref"
                  <<Keyval("field",xref.field));
        }ind_end;
        vrfy(0,"Error");
    }

    InsVar(R, field.p_ctype     , "$Cpptype*", "$name_elems", "", "pointer to elements");
    InsVar(R, field.p_ctype     , "u32", "$name_n", "", "number of elements in array");
    InsVar(R, field.p_ctype     , "u32", "$name_max", "", "max. capacity of array before realloc");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Addary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool can_copy = !CopyPrivQ(*field.p_arg);
    bool can_memcpy = field.p_arg->c_bltin;

    // do not generate Addary for fields which are rows.
    if (field.p_arg->n_xref == 0 && can_copy) {
        amc::FFunc& addary = amc::CreateCurFunc();
        Ins(&R, addary.comment, "Reserve space (this may move memory). Insert N element at the end.");
        Ins(&R, addary.comment, "Return aryptr to newly inserted block.");
        Ins(&R, addary.comment, "If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.");
        Ins(&R, addary.ret  , "algo::aryptr<$Cpptype>", false);
        Ins(&R, addary.proto, "$name_Addary($Parent, algo::aryptr<$Cpptype> rhs)", false);
        Ins(&R, addary.body    , "bool overlaps = rhs.n_elems>0 && rhs.elems >= $parname.$name_elems && rhs.elems < $parname.$name_elems + $parname.$name_max;");
        Ins(&R, addary.body    , "if (UNLIKELY(overlaps)) {");
        Ins(&R, addary.body    , "    FatalErrorExit(\"$ns.tary_alias  field:$field  comment:'alias error: sub-array is being appended to the whole'\");");
        Ins(&R, addary.body    , "}");
        Ins(&R, addary.body    , "int nnew = rhs.n_elems;");
        Ins(&R, addary.body    , "$name_Reserve($pararg, nnew); // reserve space");
        Ins(&R, addary.body    , "int at = $parname.$name_n;");
        if (can_memcpy) {
            Ins(&R, addary.body, "memcpy($parname.$name_elems + at, rhs.elems, nnew * sizeof($Cpptype));");
            Ins(&R, addary.body, "$parname.$name_n += nnew;");
        } else {
            // copy one by one -- if exception thrown during copying, $name_n will contain
            // a valid value.
            Ins(&R, addary.body, "for (int i = 0; i < nnew; i++) {");
            Ins(&R, addary.body, "    new ($parname.$name_elems + at + i) $Cpptype(rhs[i]);");
            Ins(&R, addary.body, "    $parname.$name_n++;");
            Ins(&R, addary.body, "}");
        }
        if (field.do_trace) {
            Set(R, "$partrace", Refname(*field.p_ctype));
            Ins(&R, addary.body, "$ns::_db.trace.alloc_$partrace_$name += nnew;");
        }
        Ins(&R, addary.body    , "return algo::aryptr<$Cpptype>($parname.$name_elems + at, nnew);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Alloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& alloc = amc::CreateCurFunc();
    Ins(&R, alloc.comment, "Reserve space. Insert element at the end");
    Ins(&R, alloc.comment, "The new element is initialized to a default value");
    Ins(&R, alloc.ret  , "$Cpptype&", false);
    Ins(&R, alloc.proto, "$name_Alloc($Parent)", false);
    Ins(&R, alloc.body    , "$name_Reserve($pararg, 1);");
    Ins(&R, alloc.body    , "int n  = $parname.$name_n;");
    Ins(&R, alloc.body    , "int at = n;");
    Ins(&R, alloc.body    , "$Cpptype *elems = $parname.$name_elems;");
    Ins(&R, alloc.body    , "new (elems + at) $Cpptype($dflt); // construct new element, default initializer");
    Ins(&R, alloc.body    , "$parname.$name_n = n+1;");
    if (field.do_trace) {
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, alloc.body, "++$ns::_db.trace.alloc_$partrace_$name;");
    }
    Ins(&R, alloc.body    , "return elems[at];");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_AllocAt() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    {
        amc::FFunc& allocat = amc::CreateCurFunc();
        Ins(&R, allocat.comment, "Reserve space for new element, reallocating the array if necessary");
        Ins(&R, allocat.comment, "Insert new element at specified index. Index must be in range or a fatal error occurs.", false);
        Ins(&R, allocat.ret  , "$Cpptype&", false);
        Ins(&R, allocat.proto, "$name_AllocAt($Parent, int at)", false);
        Ins(&R, allocat.body    , "$name_Reserve($pararg, 1);");
        Ins(&R, allocat.body    , "int n  = $parname.$name_n;");
        Ins(&R, allocat.body    , "if (UNLIKELY(u64(at) >= u64(n+1))) {");// silently cure bad index?
        Ins(&R, allocat.body    , "    FatalErrorExit(\"$ns.bad_alloc_at  field:$field  comment:'index out of range'\");");
        Ins(&R, allocat.body    , "}");
        Ins(&R, allocat.body    , "$Cpptype *elems = $parname.$name_elems;");
        Ins(&R, allocat.body    , "memmove(elems + at + 1, elems + at, (n - at) * sizeof($Cpptype));");
        Ins(&R, allocat.body    , "new (elems + at) $Cpptype($dflt); // construct element, default initializer");
        Ins(&R, allocat.body    , "$parname.$name_n = n+1;");
        if (field.do_trace) {
            Set(R, "$partrace", Refname(*field.p_ctype));
            Ins(&R, allocat.body, "++$ns::_db.trace.alloc_$partrace_$name;");
        }
        Ins(&R, allocat.body    , "return elems[at];");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_AllocN() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool can_memset = field.arg == "char" || field.arg == "u8";

    {
        amc::FFunc& allocn = amc::CreateCurFunc();
        Ins(&R, allocn.ret  , "algo::aryptr<$Cpptype>", false);
        Ins(&R, allocn.proto, "$name_AllocN($Parent, int n_elems)", false);
        Ins(&R, allocn.body    , "$name_Reserve($pararg, n_elems);");
        Ins(&R, allocn.body    , "int old_n  = $parname.$name_n;");
        Ins(&R, allocn.body    , "int new_n = old_n + n_elems;");
        Ins(&R, allocn.body    , "$Cpptype *elems = $parname.$name_elems;");
        if (can_memset) {
            Ins(&R, allocn.body, "memset(elems + old_n, $dflt, new_n - old_n); // initialize new space");
        } else {
            Ins(&R, allocn.body, "for (int i = old_n; i < new_n; i++) {");
            Ins(&R, allocn.body, "    new (elems + i) $Cpptype($dflt); // construct new element, default initialize");
            Ins(&R, allocn.body, "}");
        }
        Ins(&R, allocn.body    , "$parname.$name_n = new_n;");
        if (field.do_trace) {
            Set(R, "$partrace", Refname(*field.p_ctype));
            Ins(&R, allocn.body, "$ns::_db.trace.alloc_$partrace_$name += n_elems;");
        }
        Ins(&R, allocn.body    , "return algo::aryptr<$Cpptype>(elems + old_n, n_elems);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_AllocNVal() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool can_copy = !CopyPrivQ(*field.p_arg);
    if (can_copy) {
        bool can_memset = field.arg == "char" || field.arg == "u8";

        {
            amc::FFunc& allocnval = amc::CreateCurFunc();
            Ins(&R, allocnval.ret     , "algo::aryptr<$Cpptype>", false);
            Ins(&R, allocnval.proto   , "$name_AllocNVal($Parent, int n_elems, const $Cpptype& val)", false);
            Ins(&R, allocnval.body    , "$name_Reserve($pararg, n_elems);");
            Ins(&R, allocnval.body    , "int old_n  = $parname.$name_n;");
            Ins(&R, allocnval.body    , "int new_n = old_n + n_elems;");
            Ins(&R, allocnval.body    , "$Cpptype *elems = $parname.$name_elems;");
            if (can_memset) {
                Ins(&R, allocnval.body, "memset(elems + old_n, val, new_n - old_n); // initialize new space");
            } else {
                Ins(&R, allocnval.body, "for (int i = old_n; i < new_n; i++) {");
                Ins(&R, allocnval.body, "    new (elems + i) $Cpptype(val);");
                Ins(&R, allocnval.body, "}");
            }
            Ins(&R, allocnval.body    , "$parname.$name_n = new_n;");
            if (field.do_trace) {
                Set(R, "$partrace", Refname(*field.p_ctype));
                Ins(&R, allocnval.body, "$ns::_db.trace.alloc_$partrace_$name += n_elems;");
            }
            Ins(&R, allocnval.body    , "return algo::aryptr<$Cpptype>(elems + old_n, n_elems);");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Find() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& find = amc::CreateCurFunc();
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Rowid t)", false);
    Ins(&R, find.body, "u64 idx = t;");
    Ins(&R, find.body, "u64 lim = $parname.$name_n;");
    Ins(&R, find.body, "if (idx >= lim) return NULL;");
    Ins(&R, find.body, "return $parname.$name_elems + idx;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Getary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& getary = amc::CreateCurFunc();
    Ins(&R, getary.ret  , "algo::aryptr<$Cpptype>", false);
    Ins(&R, getary.proto, "$name_Getary($Cparent)", false);
    Ins(&R, getary.body, "return algo::aryptr<$Cpptype>($parname.$name_elems, $parname.$name_n);");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& init = amc::CreateCurFunc();
    Ins(&R, init.body, "$parname.$name_elems \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_n     \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_max   \t= 0; // ($field)");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Last() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& last = amc::CreateCurFunc();
    Ins(&R, last.ret  , "$Cpptype*", false);
    Ins(&R, last.proto, "$name_Last($Parent)", false);
    Ins(&R, last.body, "return $name_Find($pararg, $Rowid($parname.$name_n-1));");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Max() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& maxitems = amc::CreateCurFunc();
    Ins(&R, maxitems.ret  , "i32", false);
    Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
    if (!GlobalQ(*field.p_ctype)) {
        Ins(&R, maxitems.body, "(void)$pararg;");
    }
    Ins(&R, maxitems.body, "return $parname.$name_max;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool dtor = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;

    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R, remove.ret    , "void", false);
    Ins(&R, remove.proto  , "$name_Remove($Parent, u32 i)", false);
    Ins(&R, remove.body    , "u32 lim = $parname.$name_n;");
    Ins(&R, remove.body    , "$Cpptype *elems = $parname.$name_elems;");
    Ins(&R, remove.body    , "if (i < lim) {");
    if (dtor) {
        Ins(&R, remove.body, "    elems[i].~$Ctype(); // destroy element");
    }
    Ins(&R, remove.body    , "    memmove(elems + i, elems + (i + 1), sizeof($Cpptype) * (lim - (i + 1)));");
    Ins(&R, remove.body    , "    $parname.$name_n = lim - 1;");
    if (field.do_trace) {
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, remove.body, "    $ns::_db.trace.del_$partrace_$name += 1;");
    }
    Ins(&R, remove.body    , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool dtor = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;

    if (!field.c_fnoremove) {
        amc::FFunc& removeall = amc::CreateCurFunc();
        Ins(&R, removeall.ret  , "void", false);
        Ins(&R, removeall.proto, "$name_RemoveAll($Parent)", false);
        if (field.do_trace) {
            Set(R, "$partrace", Refname(*field.p_ctype));
            Ins(&R, removeall.body, "$ns::_db.trace.del_$partrace_$name += $parname.$name_n;");
        }
        if (!dtor) {
            removeall.inl = true;
            Ins(&R, removeall.body, "$parname.$name_n = 0;");
        } else {
            Ins(&R, removeall.body, "u32 n = $parname.$name_n;");
            Ins(&R, removeall.body, "while (n > 0) {");
            Ins(&R, removeall.body, "    n -= 1;");
            Ins(&R, removeall.body, "    $parname.$name_elems[n].~$Ctype();");
            Ins(&R, removeall.body, "    $parname.$name_n = n;");
            Ins(&R, removeall.body, "}");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_RemoveLast() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool dtor = (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype;

    amc::FFunc& removelast = amc::CreateCurFunc();
    Ins(&R, removelast.ret  , "void", false);
    Ins(&R, removelast.proto, "$name_RemoveLast($Parent)", false);
    Ins(&R, removelast.body    , "u64 n = $parname.$name_n;");
    Ins(&R, removelast.body    , "if (n > 0) {");
    Ins(&R, removelast.body    , "    n -= 1;");
    if (dtor) {
        Ins(&R, removelast.body, "    $name_qFind($pararg, $Rowid(n)).~$Ctype();");
    }
    if (field.do_trace) {
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, removelast.body, "    $ns::_db.trace.del_$partrace_$name += 1;");
    }
    Ins(&R, removelast.body    , "    $parname.$name_n = n;");
    Ins(&R, removelast.body    , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_AbsReserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, "void", "", "");
        AddProtoArg(func, "int", "n");
        Ins(&R, func.body, "u32 old_max  = $parname.$name_max;");
        Ins(&R, func.body, "if (n > i32(old_max)) {");
        Ins(&R, func.body, "    u32 new_max  = i32_Max(i32_Max(old_max * 2, n), 4);");
        Ins(&R, func.body, "    void *new_mem = $basepool_ReallocMem($parname.$name_elems, old_max * sizeof($Cpptype), new_max * sizeof($Cpptype));");
        Ins(&R, func.body, "    if (UNLIKELY(!new_mem)) {");
        Ins(&R, func.body, "        FatalErrorExit(\"$ns.tary_nomem  field:$field  comment:'out of memory'\");");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    $parname.$name_elems = ($Cpptype*)new_mem;");
        Ins(&R, func.body, "    $parname.$name_max = new_max;");
        Ins(&R, func.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Reserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, "void", "", "");
        AddProtoArg(func, "int", "n");
        Ins(&R, func.body, "u32 new_n = $parname.$name_n + n;");
        Ins(&R, func.body, "if (UNLIKELY(new_n > $parname.$name_max)) {");
        Ins(&R, func.body, "    $name_AbsReserve($pararg, new_n);");
        Ins(&R, func.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_RowidFind() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.reflect) {
        amc::FFunc& rowid_findx = amc::CreateCurFunc();
        rowid_findx.priv=true;
        Ins(&R, rowid_findx.ret  , "algo::ImrowPtr", false);
        Ins(&R, rowid_findx.proto, "$name_RowidFind(int t)", false);
        Ins(&R, rowid_findx.body, "return algo::ImrowPtr(u64($name_Find($pararg, $Rowid(t))));");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Setary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool glob = GlobalQ(*field.p_ctype);
    bool can_copy = !CopyPrivQ(*field.p_arg);
    bool can_memcpy = field.arg == "char" || field.arg == "u8";

    if (!glob && can_copy) {
        amc::FFunc &setary = amc::CreateCurFunc();
        Ins(&R, setary.ret  , "void",false);
        Ins(&R, setary.proto, "$name_Setary($Parent, $Partype &rhs)",false);
        Ins(&R, setary.body    , "$name_RemoveAll($pararg);");
        Ins(&R, setary.body    , "int nnew = rhs.$name_n;");
        Ins(&R, setary.body    , "$name_Reserve($pararg, nnew); // reserve space");
        if (can_memcpy) {
            Ins(&R, setary.body, "memcpy($parname.$name_elems, rhs.$name_elems, nnew * sizeof($Cpptype));");
            Ins(&R, setary.body, "$parname.$name_n = nnew;");
        } else {
            Ins(&R, setary.body, "for (int i = 0; i < nnew; i++) { // copy elements over");
            Ins(&R, setary.body, "    new ($parname.$name_elems + i) $Cpptype($name_qFind(rhs, i));");
            Ins(&R, setary.body, "    $parname.$name_n = i + 1;");
            Ins(&R, setary.body, "}");
        }
        if (field.do_trace) {
            Set(R, "$partrace", Refname(*field.p_ctype));
            Ins(&R, setary.body, "$ns::_db.trace.alloc_$partrace_$name += nnew;");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Setary2() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    bool glob = GlobalQ(*field.p_ctype);
    bool can_copy = !CopyPrivQ(*field.p_arg);

    // This is a potentially aliasing version of the copy function.
    if (!glob && can_copy) {
        amc::FFunc &copy = amc::CreateCurFunc();
        Ins(&R, copy.comment, "Copy specified array into $name, discarding previous contents.");
        Ins(&R, copy.comment, "If the RHS argument aliases the array (refers to the same memory), throw exception.");
        Ins(&R, copy.ret  , "void",false);
        Ins(&R, copy.proto, "$name_Setary($Parent, const algo::aryptr<$Cpptype> &rhs)",false);
        Ins(&R, copy.body, "$name_RemoveAll($pararg);");
        Ins(&R, copy.body, "$name_Addary($pararg, rhs);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& uninit = amc::CreateCurFunc();
    Ins(&R, uninit.body, "// remove all elements from $field");
    Ins(&R, uninit.body, "$name_RemoveAll($pararg);");
    Ins(&R, uninit.body, "// free memory for Tary $field");
    Ins(&R, uninit.body, "$basepool_FreeMem($parname.$name_elems, sizeof($Cpptype)*$parname.$name_max); // ($field)");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_qFind() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& qat = amc::CreateCurFunc();
    Ins(&R, qat.ret  , "$Cpptype&", false);
    Ins(&R, qat.proto, "$name_qFind($Parent, $Rowid t)", false);
    Ins(&R, qat.body, "return $parname.$name_elems[t];");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_qLast() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& qlast = amc::CreateCurFunc();
    Ins(&R, qlast.ret  , "$Cpptype&", false);
    Ins(&R, qlast.proto, "$name_qLast($Parent)", false);
    Ins(&R, qlast.body, "return $name_qFind($pararg, $Rowid($parname.$name_n-1));");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_rowid_Get() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& getrowid = amc::CreateCurFunc();
    Ins(&R, getrowid.ret  , "$Rowid", false);
    Ins(&R, getrowid.proto, "$name_rowid_Get($Parent, $Cpptype &elem)", false);
    Ins(&R, getrowid.body, "u64 id = &elem - $parname.$name_elems;");
    Ins(&R, getrowid.body, "return $Rowid(id);");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Eq() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;

    if (field.p_ctype->c_ccmp) {
        amc::FFunc& opeq = amc::CreateCurFunc();
        Ins(&R, opeq.proto, "$name_Eq(const $Parent,const $Partype &rhs)", false);
        Ins(&R, opeq.ret, "bool", false);
        Ins(&R, opeq.body, "int len = $name_N($pararg);");
        Ins(&R, opeq.body, "if (len != $name_N(rhs)) {");// short-circuit
        Ins(&R, opeq.body, "    return false;");
        Ins(&R, opeq.body, "}");
        Ins(&R, opeq.body, "for (int i = 0; i < len; i++) {");
        Ins(&R, opeq.body, "    if (!($parname.$name_elems[i] == rhs.$name_elems[i])) {");// TODO: USE EQ!
        Ins(&R, opeq.body, "        return false;");
        Ins(&R, opeq.body, "    }");
        Ins(&R, opeq.body, "}");
        Ins(&R, opeq.body, "return true;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Tary_Cmp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    if (field.p_ctype->c_ccmp) {
        Set(R, "$Fldtype", field.cpp_type);
        amc::FFunc& opeq = amc::CreateCurFunc();
        Ins(&R, opeq.proto, "$name_Cmp($Parent, $Partype &rhs)", false);
        Ins(&R, opeq.ret, "int", false);
        Ins(&R, opeq.body, "int len = i32_Min($name_N($pararg), $name_N(rhs));");
        Ins(&R, opeq.body, "int retval = 0;");
        Ins(&R, opeq.body, "for (int i = 0; i < len; i++) {");
        Ins(&R, opeq.body, "    retval = $Fldtype_Cmp($parname.$name_elems[i], rhs.$name_elems[i]);");
        Ins(&R, opeq.body, "    if (retval != 0) {");
        Ins(&R, opeq.body, "        return retval;");
        Ins(&R, opeq.body, "    }");
        Ins(&R, opeq.body, "}");
        Ins(&R, opeq.body, "return i32_Cmp($name_N($pararg), $name_N(rhs));");
    }
}


void amc::tfunc_Tary_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;

    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_curs {// cursor");
    Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext, "    $Cpptype* elems;");
    Ins(&R, ns.curstext, "    int n_elems;");
    Ins(&R, ns.curstext, "    int index;");
    Ins(&R, ns.curstext, "    $Parname_$name_curs() { elems=NULL; n_elems=0; index=0; }");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");

    {
        amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
        Ins(&R, curs_next.body, "curs.index++;");
    }

    {
        amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "curs.elems = parent.$name_elems;");
        Ins(&R, curs_reset.body, "curs.n_elems = parent.$name_n;");
        Ins(&R, curs_reset.body, "curs.index = 0;");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.index < curs.n_elems;");
    }

    {
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return curs.elems[curs.index];");
    }
}

// Read/Accumulate Tary from string.
// arg:char & U8
//    the array is flushed befor reading
//    input is a string that is copied to the array.
// any other type, with separator:
//    the array is flushed before reading
//    input string is split on separator character, elements are appended one by one
//    if any element cannot be read, function returns false
//        (but array retains values read so far)
// any other type, without separator:
//    one element is read from input string and appended to the array without flushing.
//    if the element cannot be read, the array is unchanged
//
void amc::tfunc_Tary_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    char sep = GetSep(field);
    if (!PadQ(field) && HasStringReadQ(*field.p_arg)) {
        Set(R, "$sep", char_ToCppSingleQuote(sep));
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func, "bool", "retval", "true");
        AddProtoArg(func, "algo::strptr", "in_str");
        if (field.arg == "char") {
            Ins(&R, func.comment, "The array is replaced with the input string. Function always succeeds.");
            Ins(&R, func.body, "$name_RemoveAll($pararg);");
            Ins(&R, func.body, "$name_Addary($pararg,in_str);");
        } else if (field.arg == "u8") {
            Ins(&R, func.comment, "The array is replaced with the input string. Function always succeeds.");
            Ins(&R, func.body, "$name_RemoveAll($pararg);");
            Ins(&R, func.body, "$name_Addary($pararg,algo::strptr_ToMemptr(in_str));");
        } else if (sep) {
            Ins(&R, func.comment, "The array is replaced with a set of elements, which are parsed from input string (split on $sep).");
            Ins(&R, func.comment, "Function returns true if the input string contains no error.");
            Ins(&R, func.comment, "Any values already loaded into the array remain in place");
            Ins(&R, func.body, "$name_RemoveAll($pararg);");
            Ins(&R, func.body, "for (int i=0; in_str != \"\"; i++) {");
            Ins(&R, func.body, "    algo::strptr token;");
            Ins(&R, func.body, "    algo::NextSep(in_str, $sep, token);");
            Ins(&R, func.body, "    $Cpptype &elem = $name_Alloc($pararg);");
            Ins(&R, func.body, "    retval = $Cpptype_ReadStrptrMaybe(elem, token);");
            Ins(&R, func.body, "    if (!retval) {");
            Ins(&R, func.body, "        $name_RemoveLast($pararg);");
            Ins(&R, func.body, "        break;");
            Ins(&R, func.body, "    }");
            Ins(&R, func.body, "}");
        } else {
            Ins(&R, func.comment, "A single element is read from input string and appended to the array.");
            Ins(&R, func.comment, "If the string contains an error, the array is untouched.");
            Ins(&R, func.comment, "Function returns success value.");
            Ins(&R, func.body, "$Cpptype &elem = $name_Alloc($pararg);");
            Ins(&R, func.body, "retval = $Cpptype_ReadStrptrMaybe(elem, in_str);");
            Ins(&R, func.body, "if (!retval) {");
            Ins(&R, func.body, "    $name_RemoveLast($pararg);");
            Ins(&R, func.body, "}");
        }
    }
}

void amc::tfunc_Tary_Print() {
    tfunc_Inlary_Print();
}

// -----------------------------------------------------------------------------

// Assignment operator from aryptr
// Generated only if the containing struct has only one field
void amc::tfunc_Tary_AssignAryptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (c_field_N(ctype)==1) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "operator =(const algo::aryptr<$Cpptype> &rhs)", false);
        Ins(&R, func.body, "$name_Setary(*this, rhs);");
    }
}

// -----------------------------------------------------------------------------

// Construct from aryptr
// Generated only if the containing struct has only one field
void amc::tfunc_Tary_CtorAryptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (c_field_N(ctype)==1) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.comment="";
        func.isexplicit=true;
        func.member=true;
        Ins(&R, func.proto, "$Parname(const algo::aryptr<$Cpptype> &rhs)", false);
        Ins(&R, func.body, "$name_elems \t= 0; // ($field)");
        Ins(&R, func.body, "$name_n     \t= 0; // ($field)");
        Ins(&R, func.body, "$name_max   \t= 0; // ($field)");
        Ins(&R, func.body, "$name_Addary(*this, rhs);");
    }
}
