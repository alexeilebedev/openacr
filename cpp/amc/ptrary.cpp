// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/amc/ptrary.cpp -- Ptrary reftype
//

#include "include/amc.h"

void amc::tclass_Ptrary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    Set(R, "$Rowid", "u64");

    InsVar(R, field.p_ctype, "$Cpptype**", "$name_elems", "", "array of pointers");
    InsVar(R, field.p_ctype, "u64", "$name_n", "", "current size");
    InsVar(R, field.p_ctype, "u64", "$name_max", "", "capacity of allocated array");

    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.heaplike) {
        amccheck(ptrary.unique,"amc.heaplike"
                 <<Keyval("ptrary",ptrary.field)
                 <<Keyval("comment","Heaplike implies unique"));
    }
    if (ptrary.p_field->c_cascdel) {
        amccheck(ptrary.unique,"amc.explicit_multi_delete"
                 <<Keyval("ptrary",ptrary.field)
                 <<Keyval("comment","Ptrary with unique:N cannot be cascdel"));
    }
}

// Generate $name_Cascdel: delete the last row of the array until it is empty
void amc::tfunc_Ptrary_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();
        Ins(&R, cascdel.body, "// Each row's delete removes it from this array (heaplike: O(1) swap;");
        Ins(&R, cascdel.body, "// unique: the backward scan finds the last element first), and a cascade");
        Ins(&R, cascdel.body, "// that deletes other members keeps the array consistent, so re-reading");
        Ins(&R, cascdel.body, "// $name_n each iteration visits every remaining row exactly once.");
        Ins(&R, cascdel.body, "while ($parname.$name_n > 0) {");
        Ins(&R, cascdel.body, tempstr()<<"    "<<DeleteExpr(field,"$pararg","*$parname.$name_elems[$parname.$name_n - 1]")<<";");
        Ins(&R, cascdel.body, "}");
    }
}

void amc::tfunc_Ptrary_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

void amc::tfunc_Ptrary_Find() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& find = amc::CreateCurFunc();
    find.inl = true;
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Rowid t)", false);
    Ins(&R, find.body, "$Cpptype *retval = NULL;");
    Ins(&R, find.body, "u64 idx = t;");
    Ins(&R, find.body, "u64 lim = $parname.$name_n;");
    Ins(&R, find.body, "if (idx < lim) {");
    Ins(&R, find.body, "    retval = $parname.$name_elems[idx];");
    Ins(&R, find.body, "}");
    Ins(&R, find.body, "return retval;");
}

void amc::tfunc_Ptrary_InAryQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField& field = *amc::_db.genctx.p_field;
    amc::FPtrary& ptrary = *field.c_ptrary;
    if (ptrary.unique) {
        amc::FFunc& inary = amc::CreateCurFunc();
        Ins(&R, inary.ret  , "bool", false);
        Ins(&R, inary.proto, "$xfname_InAryQ($Cpptype& row)", false);
        if (ptrary.heaplike) {
            Ins(&R, inary.body, "return row.$xfname_idx != -1;");
        } else {
            Ins(&R, inary.body, "return row.$xfname_in_ary;");
        }
    }
}

void amc::tfunc_Ptrary_qFind() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& find = amc::CreateCurFunc();
    find.inl = true;
    Ins(&R, find.ret  , "$Cpptype&", false);
    Ins(&R, find.proto, "$name_qFind($Parent, $Rowid idx)", false);
    Ins(&R, find.body, "return *$parname.$name_elems[idx];");
}

void amc::tfunc_Ptrary_qLast() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& find = amc::CreateCurFunc();
    find.inl = true;
    Ins(&R, find.ret  , "$Cpptype&", false);
    Ins(&R, find.proto, "$name_qLast($Parent)", false);
    Ins(&R, find.body, "return *$parname.$name_elems[$parname.$name_n-1];");
}

void amc::tfunc_Ptrary_Getary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& getary = amc::CreateCurFunc();
    Ins(&R, getary.ret  , "algo::aryptr<$Cpptype*>", false);
    Ins(&R, getary.proto, "$name_Getary($Parent)", false);
    Ins(&R, getary.body, "return algo::aryptr<$Cpptype*>($parname.$name_elems, $parname.$name_n);");
}

void amc::tfunc_Ptrary_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    Ins(&R, init.body, "$parname.$name_elems = NULL; // ($field)");
    Ins(&R, init.body, "$parname.$name_n = 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_max = 0; // ($field)");
}

void amc::tfunc_Ptrary_Insert() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;

    tempstr text;

    // function body
    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R, insert.ret  , "void", false);
    Ins(&R, insert.comment, "Insert pointer to row into array. Row must not already be in array;");
    Ins(&R, insert.comment, "no duplicate check is performed, so a duplicate insert silently appears twice.");
    Ins(&R, insert.proto, "$name_Insert($Parent, $Cpptype& row)", false);
    bool has_onxref = amc::FindFfunc(field, amcdb_cbtype_OnXref, true);
    if (ptrary.heaplike) {
        Ins(&R, insert.body, "if (row.$xfname_idx == -1) {");
        Ins(&R, insert.body, "    $name_Reserve($pararg, 1);");
        Ins(&R, insert.body, "    u64 n  = $parname.$name_n++;");
        Ins(&R, insert.body, "    $parname.$name_elems[n] = &row;");
        Ins(&R, insert.body, "    row.$xfname_idx = n;");
        if (has_onxref) {
            Ins(&R, insert.body, "    $name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
        }
        Ins(&R, insert.body, "}");
    } else if (ptrary.unique) {
        Ins(&R, insert.body, "if (!row.$xfname_in_ary) {");
        Ins(&R, insert.body, "    $name_Reserve($pararg, 1);");
        Ins(&R, insert.body, "    u64 n  = $parname.$name_n++;");
        Ins(&R, insert.body, "    $parname.$name_elems[n] = &row;");
        Ins(&R, insert.body, "    row.$xfname_in_ary = true;");
        if (has_onxref) {
            Ins(&R, insert.body, "    $name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
        }
        Ins(&R, insert.body, "}");
    } else {
        Ins(&R, insert.body, "$name_Reserve($pararg, 1);");
        Ins(&R, insert.body, "u64 n  = $parname.$name_n++;");
        Ins(&R, insert.body, "$parname.$name_elems[n] = &row;");
        if (has_onxref) {
            Ins(&R, insert.body, "$name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ptrary_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.unique) {
        amc::FFunc& insmaybe = amc::CreateCurFunc();
        Ins(&R, insmaybe.ret  , "bool", false);
        Ins(&R, insmaybe.proto, "$name_InsertMaybe($Parent, $Cpptype& row)", false);
        Ins(&R, insmaybe.comment, "Insert pointer to row in array.");
        Ins(&R, insmaybe.comment, "If row is already in the array, do nothing.");
        Ins(&R, insmaybe.comment, "Return value: whether element was inserted into array.");
        Ins(&R, insmaybe.body, "bool retval = !$xfname_InAryQ(row);");
        Ins(&R, insmaybe.body, "$name_Insert($pararg,row); // check is performed in _Insert again");
        Ins(&R, insmaybe.body, "return retval;");
    }
}

// -----------------------------------------------------------------------------

// Generate $name_ScanInsertMaybe of a non-unique Ptrary: a linear scan
// finds a row already present; an absent row is appended by delegating
// to $name_Insert, which also fires any OnXref hook
void amc::tfunc_Ptrary_ScanInsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (!ptrary.unique) {
        amc::FFunc& insmaybe = amc::CreateCurFunc(true);
        //AddProtoArg(insmaybe, "$Parent", "");
        AddProtoArg(insmaybe, Subst(R,"$Cpptype&"), "row");
        AddRetval(insmaybe, "bool", "retval","true");
        Ins(&R, insmaybe.comment, "Insert pointer to row in array.");
        Ins(&R, insmaybe.comment, "If row is already in the array, do nothing.");
        Ins(&R, insmaybe.comment, "Linear search is used to locate the element.");
        Ins(&R, insmaybe.comment, "Return value: whether element was inserted into array.");
        Ins(&R, insmaybe.body, "u64 n  = $parname.$name_n;");
        Ins(&R, insmaybe.body, "for (u64 i = 0; i < n; i++) {");
        Ins(&R, insmaybe.body, "    if ($parname.$name_elems[i] == &row) {");
        Ins(&R, insmaybe.body, "        retval = false;");
        Ins(&R, insmaybe.body, "        break;");
        Ins(&R, insmaybe.body, "    }");
        Ins(&R, insmaybe.body, "}");
        Ins(&R, insmaybe.body, "if (retval) {");
        Ins(&R, insmaybe.body, "    $name_Insert($pararg,row); // row known absent; the append is Insert's");
        Ins(&R, insmaybe.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ptrary_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i64", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

// -----------------------------------------------------------------------------

// Generate $name_Remove: unlink the row -- an O(1) swap with the last
// element for a heaplike array, a backward scan and shift for a unique
// one, a compacting sweep of every occurrence otherwise -- firing any
// OnUnref hook
void amc::tfunc_Ptrary_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;

    amc::FFunc& rem = amc::CreateCurFunc();
    Ins(&R, rem.ret    , "void", false);
    Ins(&R, rem.proto  , "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R, rem.body, "i64 n = $parname.$name_n;");
    bool has_onunref = amc::FindFfunc(field, amcdb_cbtype_OnUnref, true);
    if (ptrary.heaplike) {
        // heap-like removal
        Ins(&R, rem.body, "i64 idx = row.$xfname_idx;");
        Ins(&R, rem.body, "if (idx != -1) {");
        Ins(&R, rem.body, "    $Cpptype *last = $parname.$name_elems[n-1];");
        Ins(&R, rem.body, "    last->$xfname_idx = idx;");
        Ins(&R, rem.body, "    $parname.$name_elems[idx] = last;");
        Ins(&R, rem.body, "    row.$xfname_idx = -1;");
        Ins(&R, rem.body, "    $parname.$name_n = n - 1;");
        if (has_onunref) {
            Ins(&R, rem.body, "    $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
        }
        Ins(&R, rem.body, "}");
    } else if (ptrary.unique) {
        // removal of single element through scanning
        Ins(&R, rem.body, "if (bool_Update(row.$xfname_in_ary,false)) {");
        Ins(&R, rem.body, "    $Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, rem.body, "    // search backward, so that most recently added element is found first.");
        Ins(&R, rem.body, "    // if found, shift array.");
        Ins(&R, rem.body, "    for (i64 i = n-1; i>=0; i--) {");
        Ins(&R, rem.body, "        $Cpptype* elem = elems[i]; // fetch element");
        Ins(&R, rem.body, "        if (elem == &row) {");
        Ins(&R, rem.body, "            i64 j = i + 1;");
        Ins(&R, rem.body, "            size_t nbytes = sizeof($Cpptype*) * (n - j);");
        Ins(&R, rem.body, "            memmove(elems + i, elems + j, nbytes);");
        Ins(&R, rem.body, "            $parname.$name_n = n - 1;");
        Ins(&R, rem.body, "            break;");
        Ins(&R, rem.body, "        }");
        Ins(&R, rem.body, "    }");
        if (has_onunref) {
            Ins(&R, rem.body, "    $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
        }
        Ins(&R, rem.body, "}");
    } else {
        // element may appear multiple times in a non-unique index
        Ins(&R, rem.body, "i64 j=0;");
        Ins(&R, rem.body, "for (i64 i=0; i<n; i++) {");
        Ins(&R, rem.body, "    if ($parname.$name_elems[i] == &row) {");
        Ins(&R, rem.body, "    } else {");
        Ins(&R, rem.body, "        if (j != i) {");
        Ins(&R, rem.body, "            $parname.$name_elems[j] = $parname.$name_elems[i];");
        Ins(&R, rem.body, "        }");
        Ins(&R, rem.body, "        j++;");
        Ins(&R, rem.body, "    }");
        Ins(&R, rem.body, "}");
        Ins(&R, rem.body, "$parname.$name_n = j;");
        if (has_onunref) {
            // compaction removed every occurrence of the row, so the row is
            // no longer referenced by the array; fire once, like the other
            // flavors, and only if something was actually removed
            Ins(&R, rem.body, "if (j != n) {");
            Ins(&R, rem.body, "    $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
            Ins(&R, rem.body, "}");
        }
    }
}

void amc::tfunc_Ptrary_RemoveFirst() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.heaplike) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "If index is empty, return NULL. Otherwise remove and return first element in index.");
        Ins(&R, func.ret  , "$Cpptype*", false);
        Ins(&R, func.proto, "$name_RemoveFirst($Parent)", false);
        Ins(&R, func.body, "$Cpptype *row = NULL;");
        Ins(&R, func.body, "i64 n = $parname.$name_n;");
        Ins(&R, func.body, "if (n > 0) {");
        Ins(&R, func.body, "    row = $parname.$name_elems[0];");
        Ins(&R, func.body, "    $parname.$name_elems[n-1]->$xfname_idx=0;");
        Ins(&R, func.body, "    row->$xfname_idx=-1;");
        Ins(&R, func.body, "    $parname.$name_elems[0]=$parname.$name_elems[n-1];");
        Ins(&R, func.body, "    $parname.$name_n = n-1;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return row;");
    }
}

void amc::tfunc_Ptrary_First() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.heaplike) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func,Subst(R,"$Cpptype*"),"row","NULL");
        Ins(&R, func.body, "row = $parname.$name_n ? $parname.$name_elems[0] : NULL;");
    }
}

void amc::tfunc_Ptrary_RemoveLast() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.heaplike) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "If index is empty, return NULL. Otherwise remove and return last element in index.");
        Ins(&R, func.ret  , "$Cpptype*", false);
        Ins(&R, func.proto, "$name_RemoveLast($Parent)", false);
        Ins(&R, func.body, "$Cpptype *row = NULL;");
        Ins(&R, func.body, "i64 n = $parname.$name_n;");
        Ins(&R, func.body, "if (n > 0) {");
        Ins(&R, func.body, "    row = $parname.$name_elems[n-1];");
        Ins(&R, func.body, "    row->$xfname_idx=-1;");
        Ins(&R, func.body, "    $parname.$name_n = n-1;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return row;");
    }
}

void amc::tfunc_Ptrary_Last() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;
    if (ptrary.heaplike) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func,Subst(R,"$Cpptype*"),"row","NULL");
        Ins(&R, func.body, "row = $parname.$name_n ? $parname.$name_elems[$parname.$name_n-1] : NULL;");
    }
}

void amc::tfunc_Ptrary_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FPtrary &ptrary = *field.c_ptrary;

    amc::FFunc& removeall = amc::CreateCurFunc();
    removeall.inl = true;
    Ins(&R, removeall.ret  , "void", false);
    Ins(&R, removeall.proto, "$name_RemoveAll($Parent)", false);
    if (ptrary.heaplike) {
        Ins(&R, removeall.body, "for (u64 i = 0; i < $parname.$name_n; i++) {");
        Ins(&R, removeall.body, "    $parname.$name_elems[i]->$xfname_idx = -1;");
        Ins(&R, removeall.body, "}");
    } else if (ptrary.unique) {
        Ins(&R, removeall.body, "for (u64 i = 0; i < $parname.$name_n; i++) {");
        Ins(&R, removeall.body, "    // mark all elements as not-in-array");
        Ins(&R, removeall.body, "    $parname.$name_elems[i]->$xfname_in_ary = false;");
        Ins(&R, removeall.body, "}");
    }
    Ins(&R, removeall.body, "$parname.$name_n = 0;");
}

void amc::tfunc_Ptrary_Reserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.ret  , "void", false);
    Ins(&R, reserve.proto, "$name_Reserve($Parent, u64 n)", false);
    Ins(&R, reserve.body, "u64 old_max = $parname.$name_max;");
    Ins(&R, reserve.body, "if (UNLIKELY($parname.$name_n + n > old_max)) {");
    Ins(&R, reserve.body, "    u64 new_max  = u64_Max(u64_Max(old_max * 2, $parname.$name_n + n), 4);");
    Ins(&R, reserve.body, "    u64 old_size = old_max * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    u64 new_size = new_max * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    void *new_mem = $basepool_ReallocMem($parname.$name_elems, old_size, new_size);");
    Ins(&R, reserve.body, "    if (UNLIKELY(!new_mem)) {");
    Ins(&R, reserve.body, "        FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "    $parname.$name_elems = ($Cpptype**)new_mem;");
    Ins(&R, reserve.body, "    $parname.$name_max = new_max;");
    Ins(&R, reserve.body, "}");
}

void amc::tfunc_Ptrary_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& uninit = amc::CreateCurFunc();
    Ins(&R, uninit.body, "$basepool_FreeMem($parname.$name_elems, sizeof($Cpptype*)*$parname.$name_max); // ($field)");
}

// Generate the Ptrary cursor: the struct plus Reset/ValidQ/Next/Access;
// a ONCE cursor detaches the captured run's membership at Reset -- the
// array empties and every captured element's membership flag clears up
// front, so a walk that exits early leaves no row claiming membership --
// and walks the captured run. The run itself stays in the parent's
// buffer, which the cursor aliases without copying, so inserting into
// the array during the walk remains forbidden: an insert would
// overwrite the unread tail of the run
void amc::Ptrary_curs(bool once) {
    RequireTcurs();
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    amc::FField &field = *amc::_db.genctx.p_field;

    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_$curstype {// fcurs:$fcurs");
    Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext, "    $Cpptype** elems;");
    Ins(&R, ns.curstext, "    u64 n_elems;");
    Ins(&R, ns.curstext, "    u64 index;");
    Ins(&R, ns.curstext, "    $Parname_$name_$curstype() { elems=NULL; n_elems=0; index=0; }");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");

    {
        amc::FFunc& func = amc::CreateInlineFunc(Subst(R,"$field_$curstype.Reset"));
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Reset($Parname_$name_$curstype &curs, $Partype &parent)", false);
        Ins(&R, func.body, "curs.elems = parent.$name_elems;");
        Ins(&R, func.body, "curs.n_elems = parent.$name_n;");
        if (once) {
            Ins(&R, func.body, tempstr()<<"$name_RemoveAll("<<amc::ParentArgExpr(*field.p_ctype,"parent",false)<<"); // detach the captured run: clear membership flags, zero the count");
        }
        Ins(&R, func.body, "curs.index = 0;");
    }

    {
        amc::FFunc& func = amc::CreateInlineFunc(Subst(R,"$field_$curstype.ValidQ"));
        Ins(&R, func.comment, "cursor points to valid item");
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_ValidQ($Parname_$name_$curstype &curs)", false);
        Ins(&R, func.body, "return curs.index < curs.n_elems;");
    }

    {
        amc::FFunc& func = amc::CreateInlineFunc(Subst(R,"$field_$curstype.Next"));
        Ins(&R, func.comment, "proceed to next item");
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Next($Parname_$name_$curstype &curs)", false);
        Ins(&R, func.body, "curs.index++;");
    }

    {
        amc::FFunc& func = amc::CreateInlineFunc(Subst(R,"$field_$curstype.Access"));
        Ins(&R, func.comment, "item access");
        Ins(&R, func.ret  , "$Cpptype&", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Access($Parname_$name_$curstype &curs)", false);
        Ins(&R, func.body, "return *curs.elems[curs.index];");
    }
}

void amc::tfunc_Ptrary_curs() {
    bool once=false;
    Ptrary_curs(once);
}

void amc::tfunc_Ptrary_oncecurs() {
    bool once=true;
    Ptrary_curs(once);
}
