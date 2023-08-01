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
// Source: cpp/amc/bheap.cpp -- Binary heaps
//
// Created By: alexei.lebedev alexey.polovinkin
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//
// Binary heap implemenation
// The heap is implemented as a pointer array (Tary)
// Each element gets an extra field representing its position in the heap.
// -- thus, elements can be quickly removed from the heap by pointer,
// and membership can also be determined.
// TODO:
// Allow instantiating a bheap over any array type --
// currently a Ptrary implementation is essentially force.
// And what about "Upptrary"?? Is that even expressible in AMC??
// TODO: prevent modification of heap while there is a live cursor over it

#include "include/amc.h"

void amc::tclass_Bheap() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // implementation helper
    // I starting index
    // RETURN new I
    vrfy(field.c_sortfld, tempstr() << "sortfld must be specified " << field.field);
    amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;
    Set(R, "$sortfld"     , name_Get(sortfld));
    Set(R, "$Sortfldstore", sortfld.cpp_type);
    Set(R, "$inscond"  , field.c_xref ? algo::strptr(field.c_xref->inscond.value) : algo::strptr("true"));

    // helper fields
    InsVar(R, field.p_ctype     , "$Cpptype**", "$name_elems", "", "binary heap by $sortfld");
    InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the heap");
    InsVar(R, field.p_ctype     , "i32", "$name_max", "", "max elements in $name_elems");

    vrfy(!field.p_arg->c_varlenfld || !field.p_arg->c_varlenfld->processed
         , "no field can appear after varlen field.");

    InsVar(R, field.p_arg       , "i32", "$name_idx", "", "index in heap; -1 means not-in-heap");
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));

    Ins(&R, child_init->body  , "$fname.$name_idx = -1; // ($field) not-in-heap");
}

void amc::tfunc_Bheap_Upheap() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& up = amc::CreateCurFunc();
    up.priv=true;
    Ins(&R, up.ret  , "int", false);
    Ins(&R, up.proto, "$name_Upheap($Parent, $Cpptype& row, int idx)", false);
    Ins(&R, up.comment, "Find and return index of new location for element ROW in the heap, starting at index IDX.");
    Ins(&R, up.comment, "Move any elements along the way but do not modify ROW.");
    Ins(&R, up.body, "$Cpptype* *elems = $parname.$name_elems;");
    Ins(&R, up.body, "while (idx>0) {");
    Ins(&R, up.body, "    int j = (idx-1)/2;");
    Ins(&R, up.body, "    $Cpptype* p = elems[j];");
    Ins(&R, up.body, "    if (!$name_ElemLt($pararg, row, *p)) {");
    Ins(&R, up.body, "        break;");
    Ins(&R, up.body, "    }");
    Ins(&R, up.body, "    p->$name_idx = idx;");
    Ins(&R, up.body, "    elems[idx] = p;");
    Ins(&R, up.body, "    idx = j;");
    Ins(&R, up.body, "}");
    Ins(&R, up.body, "return idx;");
}

void amc::tfunc_Bheap_Downheap() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& down = amc::CreateCurFunc();
    down.priv=true;
    Ins(&R, down.comment, "Find new location for ROW starting at IDX");
    Ins(&R, down.comment, "NOTE: Rest of heap is rearranged, but pointer to ROW is NOT stored in array.");
    Ins(&R, down.ret  , "int", false);
    Ins(&R, down.proto, "$name_Downheap($Parent, $Cpptype& row, int idx)", false);
    Ins(&R, down.body, "$Cpptype* *elems = $parname.$name_elems;");
    Ins(&R, down.body, "int n = $parname.$name_n;");
    Ins(&R, down.body, "int child = idx*2+1;");
    Ins(&R, down.body, "while (child < n) {");
    Ins(&R, down.body, "    $Cpptype* p = elems[child]; // left child");
    Ins(&R, down.body, "    int rchild = child+1;");
    Ins(&R, down.body, "    if (rchild < n) {");
    Ins(&R, down.body, "        $Cpptype* q = elems[rchild]; // right child");
    Ins(&R, down.body, "        if ($name_ElemLt($pararg, *q,*p)) {");
    Ins(&R, down.body, "            child = rchild;");
    Ins(&R, down.body, "            p     = q;");
    Ins(&R, down.body, "        }");
    Ins(&R, down.body, "    }");
    Ins(&R, down.body, "    if (!$name_ElemLt($pararg, *p,row)) {");
    Ins(&R, down.body, "        break;");
    Ins(&R, down.body, "    }");
    Ins(&R, down.body, "    p->$name_idx   = idx;");
    Ins(&R, down.body, "    elems[idx]     = p;");
    Ins(&R, down.body, "    idx            = child;");
    Ins(&R, down.body, "    child          = idx*2+1;");
    Ins(&R, down.body, "}");
    Ins(&R, down.body, "return idx;");
}

void amc::tfunc_Bheap_Reheap() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& reheap = amc::CreateCurFunc();
    Ins(&R, reheap.comment, "If row is in heap, update its position. If row is not in heap, insert it.");
    Ins(&R, reheap.comment, "Return new position of item in the heap (0=top)");
    Ins(&R, reheap.ret  , "i32", false);
    Ins(&R, reheap.proto, "$name_Reheap($Parent, $Cpptype& row)", false);
    Ins(&R, reheap.body    , "int old_idx = row.$name_idx;");
    Ins(&R, reheap.body    , "bool isnew = old_idx == -1;");
    Ins(&R, reheap.body    , "if (isnew) {");
    Ins(&R, reheap.body    , "    $name_Reserve($pararg, 1);");
    Ins(&R, reheap.body    , "    old_idx = $parname.$name_n++;");
    Ins(&R, reheap.body    , "}");
    Ins(&R, reheap.body    , "int new_idx = $name_Upheap($pararg, row, old_idx);");
    Ins(&R, reheap.body    , "if (!isnew && new_idx == old_idx) {");
    Ins(&R, reheap.body    , "    new_idx = $name_Downheap($pararg, row, old_idx);");
    Ins(&R, reheap.body    , "}");
    Ins(&R, reheap.body    , "row.$name_idx = new_idx;");
    Ins(&R, reheap.body    , "$parname.$name_elems[new_idx] = &row;");
    if (field.need_firstchanged) {
        Ins(&R, reheap.comment, "If first item of the is changed, update fstep:$field");
        Ins(&R, reheap.body, "bool changed = new_idx==0 || old_idx==0;");
        Ins(&R, reheap.body, "if (changed) {");
        Ins(&R, reheap.body, "    $name_FirstChanged($pararg);");
        Ins(&R, reheap.body, "}");
    }
    Ins(&R, reheap.body    , "return new_idx;");
}

void amc::tfunc_Bheap_ReheapFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& reheapfirst = amc::CreateCurFunc();
    Ins(&R, reheapfirst.comment, "Key of first element in the heap changed. Move it.");
    Ins(&R, reheapfirst.comment, "This function does not check the insert condition.");
    Ins(&R, reheapfirst.comment, "Return new position of item in the heap (0=top).");
    Ins(&R, reheapfirst.comment, "Heap must be non-empty or behavior is undefined.");
    Ins(&R, reheapfirst.ret  , "i32", false);
    Ins(&R, reheapfirst.proto, "$name_ReheapFirst($Parent)", false);
    Ins(&R, reheapfirst.body    , "$Cpptype &row = *$parname.$name_elems[0];");
    Ins(&R, reheapfirst.body    , "i32 new_idx = $name_Downheap($pararg, row, 0);");
    Ins(&R, reheapfirst.body    , "row.$name_idx = new_idx;");
    Ins(&R, reheapfirst.body    , "$parname.$name_elems[new_idx] = &row;");
    if (field.need_firstchanged) {
        Ins(&R, reheapfirst.comment, "Update fstep:$field");
        Ins(&R, reheapfirst.body, "if (new_idx != 0) {");
        Ins(&R, reheapfirst.body, "    $name_FirstChanged($pararg);");
        Ins(&R, reheapfirst.body, "}");
    }
    Ins(&R, reheapfirst.body, "return new_idx;");
}

void amc::tfunc_Bheap_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (NeedSetQ(field)) {
        amc::FFunc& set = amc::CreateCurFunc();
        Ins(&R, set.comment, "Set row key to new value.");
        Ins(&R, set.comment, "Update heap membership based on insert condition [$inscond]");
        Ins(&R, set.ret  , "void", false);
        Ins(&R, set.proto, "$sortfld_Set($Parent, $Cpptype &row, $Sortfldstore new_key)", false);
        Ins(&R, set.body, "row.$sortfld = new_key;");
        if (field.need_firstchanged) {
            Ins(&R, set.body, "int old_idx = row.$name_idx;");
        }
        Ins(&R, set.body, "bool ins = $inscond; // user-defined insert condition (xref)");
        Ins(&R, set.body, "if (ins) {");
        Ins(&R, set.body, "    $name_Reheap($pararg, row);");
        Ins(&R, set.body, "} else {");
        Ins(&R, set.body, "    $name_Remove($pararg, row);");
        Ins(&R, set.body, "}");
        if (field.need_firstchanged) {
            Ins(&R, set.body, "int new_idx = row.$name_idx;");
            Ins(&R, set.body, "bool changed = new_idx==0 || old_idx==0;");
            Ins(&R, set.body, "// detect changes in the heap top.");
            Ins(&R, set.body, "// this is overly loose -- it may be that row is the top element");
            Ins(&R, set.body, "// but the key value hasn't changed.");
            Ins(&R, set.body, "if (changed) {");
            Ins(&R, set.body, "    $name_FirstChanged($pararg);");
            Ins(&R, set.body, "}");
        }
    }
}

void amc::tfunc_Bheap_SetIfBetter() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (NeedSetQ(field)) {
        amc::FFunc& sib = amc::CreateCurFunc();
        Ins(&R, sib.comment, "Set row key to new value. If row not in heap, the key is set to new value");
        Ins(&R, sib.comment, "Otherwise, the key is changed only if the new key is better than the old.");
        Ins(&R, sib.comment, "Update heap membership based on insert condition [$inscond]");
        Ins(&R, sib.ret  , "void", false);
        Ins(&R, sib.proto, "$sortfld_SetIfBetter($Parent, $Cpptype &row, $Sortfldstore new_key)", false);
        Ins(&R, sib.body, "bool better = true;");
        Ins(&R, sib.body, "if ($name_InBheapQ(row)) {");
        Ins(&R, sib.body, "    better = !$name_ElemLtval($pararg, row,new_key); // this is really Not Worse, not Better");
        Ins(&R, sib.body, "}");
        Ins(&R, sib.body, "if (better) {");
        Ins(&R, sib.body, "    $sortfld_Set($pararg, row, new_key);");
        Ins(&R, sib.body, "}");
    }
}

void amc::tfunc_Bheap_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();
        Ins(&R, cascdel.comment, "Delete all elements referenced by the heap.");
        Ins(&R, cascdel.body, "i32 n = $parname.$name_n;");
        Ins(&R, cascdel.body, "while (n > 0) {");
        Ins(&R, cascdel.body, "    n--;");
        Ins(&R, cascdel.body, "    $Cpptype &elem = *$parname.$name_elems[n]; // pick cheapest element to remove");
        Ins(&R, cascdel.body, "    elem.$name_idx = -1; // mark not-in-heap");
        Ins(&R, cascdel.body, "    $parname.$name_n = n;");
        Ins(&R, cascdel.body, DeleteExpr(field,"$pararg","elem")<<";");
        Ins(&R, cascdel.body, "}");
    }
}

void amc::tfunc_Bheap_RemoveFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& remfirst = amc::CreateCurFunc();
    Ins(&R, remfirst.comment, "If index is empty, return NULL. Otherwise remove and return first key in index.\n Call 'head changed' trigger.", false);
    Ins(&R, remfirst.ret  , "$Cpptype*", false);
    Ins(&R, remfirst.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R, remfirst.body, "$Cpptype *row = NULL;");
    Ins(&R, remfirst.body, "if ($parname.$name_n > 0) {");
    Ins(&R, remfirst.body, "    row = $parname.$name_elems[0];");
    Ins(&R, remfirst.body, "    row->$name_idx = -1;           // mark not in heap");
    Ins(&R, remfirst.body, "    i32 n = $parname.$name_n - 1; // index of last element in heap");
    Ins(&R, remfirst.body, "    $parname.$name_n = n;         // decrease count");
    Ins(&R, remfirst.body, "    if (n) {");
    Ins(&R, remfirst.body, "        $Cpptype &elem = *$parname.$name_elems[n];");
    Ins(&R, remfirst.body, "        int new_idx = $name_Downheap($pararg, elem, 0);");
    Ins(&R, remfirst.body, "        elem.$name_idx = new_idx;");
    Ins(&R, remfirst.body, "        $parname.$name_elems[new_idx] = &elem;");
    Ins(&R, remfirst.body, "    }");
    if (field.need_firstchanged) {
        Ins(&R, remfirst.body, "    $name_FirstChanged($pararg);");
    }
    Ins(&R, remfirst.body, "}");
    Ins(&R, remfirst.body, "return row;");
}

void amc::tfunc_Bheap_First() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& first = amc::CreateCurFunc(true); {
        AddRetval(first,Subst(R,"$Cpptype*"),"row","NULL");
    }
    Ins(&R, first.body , "if ($parname.$name_n > 0) {");
    Ins(&R, first.body , "    row = $parname.$name_elems[0];");
    Ins(&R, first.body , "}");
}

void amc::tfunc_Bheap_InBheapQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    // -- NOTE -- function takes reference to CHILD -- not parent.
    amc::FFunc& inheap = amc::CreateCurFunc();
    Ins(&R, inheap.ret  , "bool", false);
    Ins(&R, inheap.proto, "$name_InBheapQ($Cpptype& row)", false);
    Ins(&R, inheap.body, "bool result = false;");
    Ins(&R, inheap.body, "result = row.$name_idx != -1;");
    Ins(&R, inheap.body, "return result;");
}

void amc::tfunc_Bheap_Insert() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R, insert.comment, "Insert row. Row must not already be in index. If row is already in index, do nothing.", false);
    Ins(&R, insert.ret  , "void", false);
    Ins(&R, insert.proto, "$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R, insert.body,     "if (LIKELY(row.$name_idx == -1)) {");
    Ins(&R, insert.body,     "    $name_Reserve($pararg, 1);");
    Ins(&R, insert.body,     "    int n = $parname.$name_n;");
    Ins(&R, insert.body,     "    $parname.$name_n = n + 1;");
    Ins(&R, insert.body,     "    int new_idx = $name_Upheap($pararg, row, n);");
    Ins(&R, insert.body,     "    row.$name_idx = new_idx;");
    Ins(&R, insert.body,     "    $parname.$name_elems[new_idx] = &row;");
    if (field.need_firstchanged) {
        Ins(&R, insert.body, "    if (new_idx==0) {");
        Ins(&R, insert.body, "        $name_FirstChanged($pararg);");
        Ins(&R, insert.body, "    }");
    }
    Ins(&R, insert.body    , "}");
}

void amc::tfunc_Bheap_Compact() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.c_fcompact) {
        amc::FFunc& compact = amc::CreateCurFunc(true);
        AddRetval(compact, "bool", "retval", "false");
        Ins(&R, compact.body, "if (i32_Max($parname.$name_n * 4,8) < $parname.$name_max) {");
        Ins(&R, compact.body, "    u32 old_max  = $parname.$name_max;");
        Ins(&R, compact.body, "    u32 new_max  = $parname.$name_max / 2; // reduce max by 2x");
        Ins(&R, compact.body, "    u32 old_size = old_max * sizeof($Cpptype*);");
        Ins(&R, compact.body, "    u32 new_size = new_max * sizeof($Cpptype*);");
        Ins(&R, compact.body, "    void *new_mem = $basepool_ReallocMem($parname.$name_elems, old_size, new_size);");
        Ins(&R, compact.body, "    if (new_mem) {");
        Ins(&R, compact.body, "        $parname.$name_elems = ($Cpptype**)new_mem;");
        Ins(&R, compact.body, "        $parname.$name_max = new_max;");
        Ins(&R, compact.body, "        retval = true;");
        Ins(&R, compact.body, "    }");
        Ins(&R, compact.body, "}");
    }
}

void amc::tfunc_Bheap_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R,  remove.ret  , "void", false);
    Ins(&R,  remove.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R,  remove.body    , "if ($name_InBheapQ(row)) {");
    Ins(&R,  remove.body    , "    int old_idx = row.$name_idx;");
    Ins(&R,  remove.body    , "    if ($parname.$name_elems[old_idx] == &row) { // sanity check: heap points back to row");
    Ins(&R,  remove.body    , "        row.$name_idx = -1;           // mark not in heap");
    Ins(&R,  remove.body    , "        i32 n = $parname.$name_n - 1; // index of last element in heap");
    Ins(&R,  remove.body    , "        $parname.$name_n = n;         // decrease count");
    Ins(&R,  remove.body    , "        if (old_idx != n) {");
    Ins(&R,  remove.body    , "            $Cpptype *elem = $parname.$name_elems[n];");
    Ins(&R,  remove.body    , "            int new_idx = $name_Upheap($pararg, *elem, old_idx);");
    Ins(&R,  remove.body    , "            if (new_idx == old_idx) {");
    Ins(&R,  remove.body    , "                new_idx = $name_Downheap($pararg, *elem, old_idx);");
    Ins(&R,  remove.body    , "            }");
    Ins(&R,  remove.body    , "            elem->$name_idx = new_idx;");
    Ins(&R,  remove.body    , "            $parname.$name_elems[new_idx] = elem;");
    Ins(&R,  remove.body    , "        }");
    if (field.c_fcompact) {
        Ins(&R,  remove.body, "        $name_Compact($pararg);");
    }
    if (field.need_firstchanged) {
        Ins(&R,  remove.body, "        if (old_idx == 0) {");
        Ins(&R,  remove.body, "            $name_FirstChanged($pararg);");
        Ins(&R,  remove.body, "        }");
    }
    Ins(&R,  remove.body    , "    }");
    Ins(&R,  remove.body    , "}");
}

void amc::tfunc_Bheap_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.comment, "Return number of items in the heap");
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

void amc::tfunc_Bheap_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.comment, "Return true if index is empty");
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

void amc::tfunc_Bheap_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& flush = amc::CreateCurFunc();
    Ins(&R, flush.comment, "Remove all elements from binary heap");
    Ins(&R, flush.ret  , "void", false);
    Ins(&R, flush.proto, "$name_RemoveAll($Parent)", false);
    Ins(&R, flush.body    , "int n = $parname.$name_n;");
    Ins(&R, flush.body    , "for (int i = n - 1; i>=0; i--) {");
    Ins(&R, flush.body    , "    $parname.$name_elems[i]->$name_idx = -1; // mark not-in-heap");
    Ins(&R, flush.body    , "}");
    Ins(&R, flush.body    , "$parname.$name_n = 0;");
    if (field.need_firstchanged) {
        Ins(&R, flush.body, "if (n > 0) {");
        Ins(&R, flush.body, "    $name_FirstChanged($pararg);");
        Ins(&R, flush.body, "}");
    }
}

void amc::tfunc_Bheap_Dealloc() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& dealloc = amc::CreateCurFunc();
    Ins(&R, dealloc.comment, "Remove all elements from heap and free memory used by the array.");
    Ins(&R, dealloc.ret  , "void", false);
    Ins(&R, dealloc.proto, "$name_Dealloc($Parent)", false);
    Ins(&R, dealloc.body, "$name_RemoveAll($pararg);");
    Ins(&R, dealloc.body, "$basepool_FreeMem($parname.$name_elems, sizeof($Cpptype*)*$parname.$name_max);");
    Ins(&R, dealloc.body, "$parname.$name_max   = 0;");
    Ins(&R, dealloc.body, "$parname.$name_elems = NULL;");
}

void amc::tfunc_Bheap_Reserve() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.ret  , "void", false);
    Ins(&R, reserve.proto, "$name_Reserve($Parent, int n)", false);
    Ins(&R, reserve.body, "i32 old_max = $parname.$name_max;");
    Ins(&R, reserve.body, "if (UNLIKELY($parname.$name_n + n > old_max)) {");
    Ins(&R, reserve.body, "    u32 new_max  = u32_Max(4, old_max * 2);");
    Ins(&R, reserve.body, "    u32 old_size = old_max * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    u32 new_size = new_max * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    void *new_mem = $basepool_ReallocMem($parname.$name_elems, old_size, new_size);");
    Ins(&R, reserve.body, "    if (UNLIKELY(!new_mem)) {");
    Ins(&R, reserve.body, "        FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "    $parname.$name_elems = ($Cpptype**)new_mem;");
    Ins(&R, reserve.body, "    $parname.$name_max = new_max;");
    Ins(&R, reserve.body, "}");
}

void amc::tfunc_Bheap_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& init = amc::CreateCurFunc();
    Ins(&R, init.body, "$parname.$name_max   \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_n     \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_elems \t= NULL; // ($field)");
}

void amc::tfunc_Bheap_Uninit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (field.p_ctype == field.p_ctype->p_ns->c_globfld->p_ctype) {
        Ins(&R, uninit.body, "// skip destruction in global scope");
    } else {
        Ins(&R, uninit.body, "$basepool_FreeMem((u8*)$parname.$name_elems, sizeof($Cpptype*)*$parname.$name_max); // ($field)");
    }
}

void amc::tfunc_Bheap_ElemLt() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;

    amc::FFunc& lt = amc::CreateCurFunc();
    lt.priv = true;

    Ins(&R, lt.ret  , "bool",false);
    Ins(&R, lt.proto, "$name_ElemLt($Parent, $Cpptype &a, $Cpptype &b)",false);
    Ins(&R, lt.body, "(void)$parname;");
    amc::FCtype *base = GetBaseType(*field.p_arg,NULL);

    bool ownfld = sortfld.c_fcmp && (sortfld.p_ctype == field.p_arg || (base && sortfld.p_ctype == base));
    if (ownfld) {
        Ins(&R, lt.body, "return $sortfld_Lt(a, b);");// direct field of child type
    } else {
        Set(R, "$aval", FieldvalExpr(field.p_arg, sortfld, "a"));
        Set(R, "$bval", FieldvalExpr(field.p_arg, sortfld, "b"));
        Ins(&R, lt.body, "return $aval < $bval;");
    }
}

void amc::tfunc_Bheap_ElemLtval() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (NeedSetQ(field)) {
        amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;

        amc::FFunc& ltval = amc::CreateCurFunc();
        ltval.priv = true;

        Set(R, "$aval", FieldvalExpr(field.p_arg, sortfld, "a"));

        Ins(&R, ltval.ret  , "bool",false);
        Ins(&R, ltval.proto, "$name_ElemLtval($Parent, $Cpptype &a, const $Sortfldstore &b)",false);
        Ins(&R, ltval.body, "(void)$parname;");
        Ins(&R, ltval.body, "return $aval < ($Sortfldstore&)b;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bheap_unordcurs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");

    {
        Ins(&R, ns.curstext, "");
        Ins(&R, ns.curstext, "struct $Parname_$name_unordcurs {// unordered cursor -- iterate over heap in arbitrary order");
        Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext, "    $Cpptype** elems;");
        Ins(&R, ns.curstext, "    u32 n_elems;");
        Ins(&R, ns.curstext, "    u32 index;");
        Ins(&R, ns.curstext, "    $Parname_$name_unordcurs() { elems=NULL; n_elems=0; index=0; }");
        Ins(&R, ns.curstext, "};");
        Ins(&R, ns.curstext, "");
    }

    {
        amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R,"$field.unordcurs_Reset"));
        curs_reset.inl = true;
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_unordcurs_Reset($Parname_$name_unordcurs &unordcurs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "unordcurs.elems = parent.$name_elems;");
        Ins(&R, curs_reset.body, "unordcurs.n_elems = parent.$name_n;");
        Ins(&R, curs_reset.body, "unordcurs.index = 0;");
    }

    {
        amc::FFunc& curs_validq = amc::ind_func_GetOrCreate(Subst(R,"$field.unordcurs_ValidQ"));
        curs_validq.inl = true;
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_unordcurs_ValidQ($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_validq.body, "return unordcurs.index < unordcurs.n_elems;");
    }

    {
        amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field.unordcurs_Next"));
        curs_next.inl = true;
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_unordcurs_Next($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_next.body, "unordcurs.index++;");
    }

    {
        amc::FFunc& curs_access = amc::ind_func_GetOrCreate(Subst(R,"$field.unordcurs_Access"));
        curs_access.inl = true;
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_unordcurs_Access($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_access.body, "return *unordcurs.elems[unordcurs.index];");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bheap_curs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;

    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");

    {
        Ins(&R, ns.curstext, "// Non-destructive heap cursor, returns heap elements in sorted order.");
        Ins(&R, ns.curstext, "// A running front of potential smallest entries is kept in the helper heap (curs.temp_%)");
        Ins(&R, ns.curstext, "struct $Parname_$name_curs {");
        Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext, "    $Partype      *parent;        // parent");
        Ins(&R, ns.curstext, "    $Cpptype*     *temp_elems;    // helper heap");
        Ins(&R, ns.curstext, "    int            temp_n;        // number of elements heaped in the helper heap");
        Ins(&R, ns.curstext, "    int            temp_max;      // max number of elements possible in the helper heap");
        Ins(&R, ns.curstext, "    $Parname_$name_curs() : parent(NULL), temp_elems(NULL), temp_n(0), temp_max(0) {}");
        Ins(&R, ns.curstext, "    ~$Parname_$name_curs();");
        Ins(&R, ns.curstext, "};");
        Ins(&R, ns.curstext, "");
    }

    {
        Ins(&R, *ns.cpp, "$ns::$Parname_$name_curs::~$Parname_$name_curs() {");
        Ins(&R, *ns.cpp, "    $basepool_FreeMem(temp_elems, sizeof(void*) * temp_max);\n");
        Ins(&R, *ns.cpp, "}\n");
    }

    {
        amc::FFunc& curs_add = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Add"));
        curs_add.priv = true;
        Ins(&R, curs_add.ret  , "void", false);
        Ins(&R, curs_add.proto, "$Parname_$name_curs_Add($Parname_$name_curs &curs, $Cpptype& row)", false);
        Ins(&R, curs_add.body, "u32 n = curs.temp_n;");
        Ins(&R, curs_add.body, "int i = n;");
        Ins(&R, curs_add.body, "curs.temp_n = n+1;");
        Ins(&R, curs_add.body, "$Cpptype* *elems = curs.temp_elems;");
        Ins(&R, curs_add.body, "while (i>0) {");
        Ins(&R, curs_add.body, "    int j = (i-1)/2;");
        Ins(&R, curs_add.body, "    $Cpptype* p = elems[j];");
        Ins(&R, curs_add.body, "    if (!$name_ElemLt($curspararg, row,*p)) {");
        Ins(&R, curs_add.body, "        break;");
        Ins(&R, curs_add.body, "    }");
        Ins(&R, curs_add.body, "    elems[i]=p;");
        Ins(&R, curs_add.body, "    i=j;");
        Ins(&R, curs_add.body, "}");
        Ins(&R, curs_add.body, "elems[i]=&row;");
    }

    {
        amc::FFunc& curs_rsrv = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reserve"));
        Ins(&R, curs_rsrv.ret  , "void", false);
        Ins(&R, curs_rsrv.proto, "$Parname_$name_curs_Reserve($Parname_$name_curs &curs, int n)", false);
        Ins(&R, curs_rsrv.body, "if (n > curs.temp_max) {");
        Ins(&R, curs_rsrv.body, "    size_t old_size   = sizeof(void*) * curs.temp_max;");
        Ins(&R, curs_rsrv.body, "    size_t new_size   = sizeof(void*) * $name_N($curspararg);");
        Ins(&R, curs_rsrv.body, "    curs.temp_elems   = ($Cpptype**)$basepool_ReallocMem(curs.temp_elems, old_size, new_size);");
        Ins(&R, curs_rsrv.body, "    if (!curs.temp_elems) {");
        Ins(&R, curs_rsrv.body, "        algo::FatalErrorExit(\"$ns.cursor_out_of_memory  func:$field_curs.Reserve\");");
        Ins(&R, curs_rsrv.body, "    }");
        Ins(&R, curs_rsrv.body, "    curs.temp_max       = $name_N($curspararg);");
        Ins(&R, curs_rsrv.body, "}");
    }

    {
        amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reset"));
        Ins(&R, curs_reset.comment, "Reset cursor. If HEAP is non-empty, add its top element to CURS.", false);
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "curs.parent       = &parent;");
        Ins(&R, curs_reset.body, "$Parname_$name_curs_Reserve(curs, $name_N($curspararg));");
        Ins(&R, curs_reset.body, "curs.temp_n = 0;");
        Ins(&R, curs_reset.body, "if (parent.$name_n > 0) {");
        Ins(&R, curs_reset.body, "    $Cpptype &first = *parent.$name_elems[0];");
        Ins(&R, curs_reset.body, "    curs.temp_elems[0] = &first; // insert first element in heap");
        Ins(&R, curs_reset.body, "    curs.temp_n = 1;");
        Ins(&R, curs_reset.body, "}");
    }

    {
        amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Next"));
        Ins(&R, curs_next.comment, "Advance cursor.", false);
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
        Ins(&R, curs_next.body, "$Cpptype* *elems = curs.temp_elems;");
        Ins(&R, curs_next.body, "int n = curs.temp_n;");
        Ins(&R, curs_next.body, "if (n > 0) {");
        Ins(&R, curs_next.body, "    // remove top element from heap");
        Ins(&R, curs_next.body, "    $Cpptype* dead = elems[0];");
        Ins(&R, curs_next.body, "    int i       = 0;");
        Ins(&R, curs_next.body, "    $Cpptype* last = curs.temp_elems[n-1];");
        Ins(&R, curs_next.body, "    // downheap last elem");
        Ins(&R, curs_next.body, "    do {");
        Ins(&R, curs_next.body, "        $Cpptype* choose = last;");
        Ins(&R, curs_next.body, "        int l         = i*2+1;");
        Ins(&R, curs_next.body, "        if (l<n) {");
        Ins(&R, curs_next.body, "            $Cpptype* el = elems[l];");
        Ins(&R, curs_next.body, "            int r     = l+1;");
        Ins(&R, curs_next.body, "            r        -= r==n;");
        Ins(&R, curs_next.body, "            $Cpptype* er = elems[r];");
        Ins(&R, curs_next.body, "            if ($name_ElemLt($curspararg,*er,*el)) {");
        Ins(&R, curs_next.body, "                el  = er;");
        Ins(&R, curs_next.body, "                l   = r;");
        Ins(&R, curs_next.body, "            }");
        Ins(&R, curs_next.body, "            bool b = $name_ElemLt($curspararg,*el,*last);");
        Ins(&R, curs_next.body, "            if (b) choose = el;");
        Ins(&R, curs_next.body, "            if (!b) l = n;");
        Ins(&R, curs_next.body, "        }");
        Ins(&R, curs_next.body, "        elems[i] = choose;");
        Ins(&R, curs_next.body, "        i = l;");
        Ins(&R, curs_next.body, "    } while (i < n);");
        Ins(&R, curs_next.body, "    curs.temp_n = n-1;");
        Ins(&R, curs_next.body, "    int index = dead->$name_idx;");
        Ins(&R, curs_next.body, "    i = (index*2+1);");
        Ins(&R, curs_next.body, "    if (i < $name_N($curspararg)) {");
        Ins(&R, curs_next.body, "        $Cpptype &elem = *curs.parent->$name_elems[i];");
        Ins(&R, curs_next.body, "        $Parname_$name_curs_Add(curs, elem);");
        Ins(&R, curs_next.body, "    }");
        Ins(&R, curs_next.body, "    if (i+1 < $name_N($curspararg)) {");
        Ins(&R, curs_next.body, "        $Cpptype &elem = *curs.parent->$name_elems[i + 1];");
        Ins(&R, curs_next.body, "        $Parname_$name_curs_Add(curs, elem);");
        Ins(&R, curs_next.body, "    }");
        Ins(&R, curs_next.body, "}");
    }

    {
        amc::FFunc& curs_access = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Access"));
        curs_access.inl = true;
        Ins(&R, curs_access.comment, "Access current element. If not more elements, return NULL");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return *curs.temp_elems[0];");
    }

    {
        amc::FFunc& curs_validq = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.ValidQ"));
        curs_validq.inl = true;
        Ins(&R, curs_validq.comment, "Return true if Access() will return non-NULL.");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.temp_n > 0;");
    }
}
