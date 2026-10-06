// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/bheap.cpp -- Binary heaps
//
// A Bheap is a binary heap of pointers to rows, and each row stores its slot
// in the heap, so a row can be removed or moved by pointer.
// Think of an order book kept as a heap of price levels: most inserts and
// removes land within a few levels of the best one.  A plain binary heap pays
// a full sift for each of them, since a pop takes the last element into the
// root and walks it down, and an insert near the best level walks up from the
// bottom.
// So the head of a Bheap is a sorted run of up to NHEAD rows, where NHEAD comes
// from the field's dmmeta.bheap row and is 1 without one.  The run is
// right-aligned at the start of the pointer array, so that it ends at slot
// R = NHEAD-1.  The run's last row, its largest, is also the root of an
// ordinary binary heap: slot J >= R has children 2J-R+1 and 2J-R+2, and the
// heap's rows follow the root from slot R+1.  Every row of the run is at most
// the root, and the root is at most every row of the heap, so the run and the
// heap together are one heap order.
// A pop while the run holds two or more rows takes its leftmost slot and moves
// nothing.  An insert below the root shifts the smaller run rows one slot left.
// An insert at or above the root is a heap insert, whose sift stops below the
// root.  Inserting into a full run demotes the root into the heap, and only
// then, or when the run is down to the root alone, does a head operation pay
// a full sift.  A run helps when it can hold every row that the traffic keeps
// touching, so an order book wants an NHEAD larger than its active band.
// NHEAD is known when amc runs, so a heap with NHEAD 1 gets the textbook binary
// heap, with the children of slot J at 2J+1 and 2J+2, one count of rows, and
// none of the run's code.  Each generator below emits that form for NHEAD 1,
// and the run helpers emit nothing for it.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Return the capacity of the sorted run at the head of the Bheap FIELD.
static int HeadMax(amc::FField &field) {
    return field.c_bheap ? int(field.c_bheap->nhead) : 1;
}

// Return true when the Bheap FIELD has a sorted run of two or more rows at its
// head, and so needs the run's code.
static bool RunQ(amc::FField &field) {
    return HeadMax(field) > 1;
}

// Return true when a dmmeta.fcurs row asks for the fillcurs of the Bheap FIELD.
static bool FillcursQ(amc::FField &field) {
    return amc::ind_fcurs_Find(dmmeta::Fcurs_Concat_field_curstype(field.field, "fillcurs")) != NULL;
}

// -----------------------------------------------------------------------------

// Check the Bheap field being generated and declare its fields in the parent
// and in the row.
void amc::tclass_Bheap() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    vrfy(field.c_sortfld, tempstr() << "sortfld must be specified " << field.field);
    vrfy(HeadMax(field) >= 1, tempstr() << "amc.bad_nhead" << Keyval("field", field.field) << Keyval("comment", "dmmeta.bheap nhead must be at least 1"));
    amc::FField &sortfld = *field.c_sortfld->p_sortfld;
    Set(R, "$sortfld"     , name_Get(sortfld));
    Set(R, "$Sortfldstore", sortfld.cpp_type);
    Set(R, "$inscond"     , field.c_xref ? algo::strptr(field.c_xref->inscond.value) : algo::strptr("true"));
    Set(R, "$rmax"        , tempstr() << HeadMax(field));
    Set(R, "$rlast"       , tempstr() << HeadMax(field) - 1);

    if (RunQ(field)) {
        InsVar(R, field.p_ctype, "$Cpptype**", "$name_elems", "", "sorted run ending at slot $rlast, then a heap rooted there");
        InsVar(R, field.p_ctype, "i32", "$name_nrun", "", "rows in the sorted run, the root included");
        InsVar(R, field.p_ctype, "i32", "$name_nheap", "", "rows of the heap below the root, in the slots after it");
        InsVar(R, field.p_ctype, "i32", "$name_max", "", "slots allocated in $name_elems");
        InsVar(R, field.p_arg  , "i32", "$xfname_idx", "", "slot in heap; -1 means not-in-heap");
    } else {
        InsVar(R, field.p_ctype     , "$Cpptype**", "$name_elems", "", "binary heap by $sortfld");
        InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the heap");
        InsVar(R, field.p_ctype     , "i32", "$name_max", "", "max elements in $name_elems");
        InsVar(R, field.p_arg       , "i32", "$xfname_idx", "", "index in heap; -1 means not-in-heap");
    }
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname", Refname(*field.p_arg));
    Ins(&R, child_init->body, "$fname.$xfname_idx = -1; // ($field) not-in-heap");
}

// -----------------------------------------------------------------------------

// Generate Upheap, which moves a hole at heap slot IDX toward the root until
// ROW fits there.  With a run, it never moves the root: every caller passes a
// row that is at least the root.
void amc::tfunc_Bheap_Upheap() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& up = amc::CreateCurFunc();
    up.priv = true;
    Ins(&R, up.ret  , "int", false);
    Ins(&R, up.proto, "$name_Upheap($Parent, $Cpptype& row, int idx)", false);
    if (RunQ(field)) {
        Ins(&R, up.comment, "Find and return the slot for ROW, starting at the empty heap slot IDX; never moves the root.");
        Ins(&R, up.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, up.body, "while (idx >= $rmax + 2) {");
        Ins(&R, up.body, "    int j = (idx + $rlast - 1) >> 1;");
    } else {
        Ins(&R, up.comment, "Find and return index of new location for element ROW in the heap, starting at index IDX.");
        Ins(&R, up.comment, "Move any elements along the way but do not modify ROW.");
        Ins(&R, up.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, up.body, "while (idx>0) {");
        Ins(&R, up.body, "    int j = (idx-1)/2;");
    }
    Ins(&R, up.body, "    $Cpptype* p = elems[j];");
    Ins(&R, up.body, "    if (!$name_ElemLt($pararg, row, *p)) {");
    Ins(&R, up.body, "        break;");
    Ins(&R, up.body, "    }");
    Ins(&R, up.body, "    p->$xfname_idx = idx;");
    Ins(&R, up.body, "    elems[idx] = p;");
    Ins(&R, up.body, "    idx = j;");
    Ins(&R, up.body, "}");
    Ins(&R, up.body, "return idx;");
}

// -----------------------------------------------------------------------------

// Generate Downheap, which moves a hole at slot IDX, the root or below it,
// toward the leaves until ROW fits there.
void amc::tfunc_Bheap_Downheap() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& down = amc::CreateCurFunc();
    down.priv = true;
    if (RunQ(field)) {
        Ins(&R, down.ret  , "int", false);
        Ins(&R, down.proto, "$name_Downheap($Parent, $Cpptype& row, int idx)", false);
        Ins(&R, down.comment, "Find and return the slot for ROW, starting at the empty slot IDX of the heap.");
        Ins(&R, down.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, down.body, "int end = $rmax + $parname.$name_nheap;");
        Ins(&R, down.body, "int child = 2 * idx - $rlast + 1;");
        Ins(&R, down.body, "while (child < end) {");
        Ins(&R, down.body, "    $Cpptype* p = elems[child];");
        Ins(&R, down.body, "    if (child + 1 < end) {");
        Ins(&R, down.body, "        $Cpptype* q = elems[child + 1];");
        Ins(&R, down.body, "        if ($name_ElemLt($pararg, *q, *p)) {");
        Ins(&R, down.body, "            child = child + 1;");
        Ins(&R, down.body, "            p = q;");
        Ins(&R, down.body, "        }");
        Ins(&R, down.body, "    }");
        Ins(&R, down.body, "    if (!$name_ElemLt($pararg, *p, row)) {");
        Ins(&R, down.body, "        break;");
        Ins(&R, down.body, "    }");
        Ins(&R, down.body, "    p->$xfname_idx = idx;");
        Ins(&R, down.body, "    elems[idx] = p;");
        Ins(&R, down.body, "    idx = child;");
        Ins(&R, down.body, "    child = 2 * idx - $rlast + 1;");
        Ins(&R, down.body, "}");
        Ins(&R, down.body, "return idx;");
    } else {
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
        Ins(&R, down.body, "    p->$xfname_idx   = idx;");
        Ins(&R, down.body, "    elems[idx]     = p;");
        Ins(&R, down.body, "    idx            = child;");
        Ins(&R, down.body, "    child          = idx*2+1;");
        Ins(&R, down.body, "}");
        Ins(&R, down.body, "return idx;");
    }
}

// -----------------------------------------------------------------------------

// Generate RemoveRoot, which empties the root when the run holds only the
// root: the last heap row fills it and sinks.  With no heap rows left the
// whole Bheap is empty.
void amc::tfunc_Bheap_RemoveRoot() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& rmroot = amc::CreateCurFunc();
        rmroot.priv = true;
        Ins(&R, rmroot.ret  , "void", false);
        Ins(&R, rmroot.proto, "$name_RemoveRoot($Parent)", false);
        Ins(&R, rmroot.body, "if ($parname.$name_nheap > 0) {");
        Ins(&R, rmroot.body, "    $parname.$name_nheap--;");
        Ins(&R, rmroot.body, "    $Cpptype &last = *$parname.$name_elems[$rmax + $parname.$name_nheap];");
        Ins(&R, rmroot.body, "    int idx = $name_Downheap($pararg, last, $rlast);");
        Ins(&R, rmroot.body, "    last.$xfname_idx = idx;");
        Ins(&R, rmroot.body, "    $parname.$name_elems[idx] = &last;");
        Ins(&R, rmroot.body, "} else {");
        Ins(&R, rmroot.body, "    $parname.$name_nrun = 0;");
        Ins(&R, rmroot.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate RunRemove, which takes the row in run slot IDX out.  The run rows
// to its left shift right by one; when IDX is the root, the next largest run
// row becomes the root, which keeps the heap order.
void amc::tfunc_Bheap_RunRemove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& runrm = amc::CreateCurFunc();
        runrm.priv = true;
        Ins(&R, runrm.ret  , "void", false);
        Ins(&R, runrm.proto, "$name_RunRemove($Parent, int idx)", false);
        Ins(&R, runrm.body, "if ($parname.$name_nrun > 1) {");
        Ins(&R, runrm.body, "    $Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, runrm.body, "    int first = $rmax - $parname.$name_nrun;");
        Ins(&R, runrm.body, "    for (int i = idx; i > first; i--) {");
        Ins(&R, runrm.body, "        elems[i] = elems[i - 1];");
        Ins(&R, runrm.body, "        elems[i]->$xfname_idx = i;");
        Ins(&R, runrm.body, "    }");
        Ins(&R, runrm.body, "    $parname.$name_nrun--;");
        Ins(&R, runrm.body, "} else {");
        Ins(&R, runrm.body, "    $name_RemoveRoot($pararg);");
        Ins(&R, runrm.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate Demote, which moves the root of a full run into the heap.  The run's
// rows shift right, so the next largest becomes the root.  The demoted row is
// at most every heap row, so it rises to just below the new root.
void amc::tfunc_Bheap_Demote() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& demote = amc::CreateCurFunc();
        demote.priv = true;
        Ins(&R, demote.ret  , "void", false);
        Ins(&R, demote.proto, "$name_Demote($Parent)", false);
        Ins(&R, demote.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, demote.body, "$Cpptype &old = *elems[$rlast];");
        Ins(&R, demote.body, "for (int i = $rlast; i > 0; i--) {");
        Ins(&R, demote.body, "    elems[i] = elems[i - 1];");
        Ins(&R, demote.body, "    elems[i]->$xfname_idx = i;");
        Ins(&R, demote.body, "}");
        Ins(&R, demote.body, "$parname.$name_nrun = $rlast;");
        Ins(&R, demote.body, "int idx = $name_Upheap($pararg, old, $rmax + $parname.$name_nheap);");
        Ins(&R, demote.body, "$parname.$name_nheap++;");
        Ins(&R, demote.body, "old.$xfname_idx = idx;");
        Ins(&R, demote.body, "elems[idx] = &old;");
    }
}

// -----------------------------------------------------------------------------

// Generate RunInsert, which places ROW into its sorted position in a run that
// is not full.  ROW must be at most every heap row.  The run rows smaller than
// ROW, the root among them, move one slot left, so a ROW at least the root
// becomes the root.
void amc::tfunc_Bheap_RunInsert() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& runins = amc::CreateCurFunc();
        runins.priv = true;
        Ins(&R, runins.ret  , "void", false);
        Ins(&R, runins.proto, "$name_RunInsert($Parent, $Cpptype& row)", false);
        Ins(&R, runins.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, runins.body, "int p = $rmax - $parname.$name_nrun;");
        Ins(&R, runins.body, "while (p < $rmax && $name_ElemLt($pararg, *elems[p], row)) {");
        Ins(&R, runins.body, "    elems[p - 1] = elems[p];");
        Ins(&R, runins.body, "    elems[p - 1]->$xfname_idx = p - 1;");
        Ins(&R, runins.body, "    p++;");
        Ins(&R, runins.body, "}");
        Ins(&R, runins.body, "elems[p - 1] = &row;");
        Ins(&R, runins.body, "row.$xfname_idx = p - 1;");
        Ins(&R, runins.body, "$parname.$name_nrun++;");
    }
}

// -----------------------------------------------------------------------------

// Generate InsertImpl, which places a row that is not in the heap.  It fires
// no callback; Insert and Reheap decide which ones apply.
void amc::tfunc_Bheap_InsertImpl() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& insert = amc::CreateCurFunc();
        insert.priv = true;
        Ins(&R, insert.ret  , "void", false);
        Ins(&R, insert.proto, "$name_InsertImpl($Parent, $Cpptype& row)", false);
        Ins(&R, insert.body, "$name_Reserve($pararg, 1);");
        Ins(&R, insert.body, "if ($parname.$name_nrun == 0) {");
        Ins(&R, insert.body, "    $parname.$name_elems[$rlast] = &row;");
        Ins(&R, insert.body, "    row.$xfname_idx = $rlast;");
        Ins(&R, insert.body, "    $parname.$name_nrun = 1;");
        Ins(&R, insert.body, "} else {");
        Ins(&R, insert.body, "    // a row below the root goes to the run, and a full run makes room by");
        Ins(&R, insert.body, "    // demoting the root; the row is below the old root, so it stays in the run");
        Ins(&R, insert.body, "    // and the heap gains one row either way");
        Ins(&R, insert.body, "    if ($name_ElemLt($pararg, row, *$parname.$name_elems[$rlast])) {");
        Ins(&R, insert.body, "        if ($parname.$name_nrun == $rmax) {");
        Ins(&R, insert.body, "            $name_Demote($pararg);");
        Ins(&R, insert.body, "        }");
        Ins(&R, insert.body, "        $name_RunInsert($pararg, row);");
        Ins(&R, insert.body, "    } else {");
        Ins(&R, insert.body, "        int idx = $name_Upheap($pararg, row, $rmax + $parname.$name_nheap);");
        Ins(&R, insert.body, "        $parname.$name_nheap++;");
        Ins(&R, insert.body, "        row.$xfname_idx = idx;");
        Ins(&R, insert.body, "        $parname.$name_elems[idx] = &row;");
        Ins(&R, insert.body, "    }");
        Ins(&R, insert.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate RemoveImpl, which takes a row that is in the heap out of the run or
// out of the heap.  It fires no callback; Remove, RemoveFirst, Reheap and
// Cascdel decide which ones apply.
void amc::tfunc_Bheap_RemoveImpl() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field)) {
        amc::FFunc& remove = amc::CreateCurFunc();
        remove.priv = true;
        Ins(&R, remove.ret  , "void", false);
        Ins(&R, remove.proto, "$name_RemoveImpl($Parent, $Cpptype& row)", false);
        Ins(&R, remove.body, "int idx = row.$xfname_idx;");
        Ins(&R, remove.body, "row.$xfname_idx = -1; // mark not in heap");
        Ins(&R, remove.body, "if (idx <= $rlast) {");
        Ins(&R, remove.body, "    $name_RunRemove($pararg, idx);");
        Ins(&R, remove.body, "} else {");
        Ins(&R, remove.body, "    $parname.$name_nheap--;");
        Ins(&R, remove.body, "    int last = $rmax + $parname.$name_nheap;");
        Ins(&R, remove.body, "    if (idx != last) {");
        Ins(&R, remove.body, "        $Cpptype &elem = *$parname.$name_elems[last];");
        Ins(&R, remove.body, "        int new_idx = $name_Upheap($pararg, elem, idx);");
        Ins(&R, remove.body, "        if (new_idx == idx) {");
        Ins(&R, remove.body, "            new_idx = $name_Downheap($pararg, elem, idx);");
        Ins(&R, remove.body, "        }");
        Ins(&R, remove.body, "        elem.$xfname_idx = new_idx;");
        Ins(&R, remove.body, "        $parname.$name_elems[new_idx] = &elem;");
        Ins(&R, remove.body, "    }");
        Ins(&R, remove.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate Promote, which moves the smallest heap row into the run: the run
// shifts left by one slot, the old root becomes an ordinary run row, and the
// smaller child of the root takes the root's slot.  The last heap row fills
// the vacated slot and sinks.  The run must have room and the heap must hold
// a row.  Only fillcurs calls it, so it exists only for a field that asks for one.
void amc::tfunc_Bheap_Promote() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (RunQ(field) && FillcursQ(field)) {
        amc::FFunc& promote = amc::CreateCurFunc();
        promote.priv = true;
        Ins(&R, promote.ret  , "void", false);
        Ins(&R, promote.proto, "$name_Promote($Parent)", false);
        Ins(&R, promote.body, "$Cpptype* *elems = $parname.$name_elems;");
        Ins(&R, promote.body, "int end = $rmax + $parname.$name_nheap;");
        Ins(&R, promote.body, "int c = $rmax;");
        Ins(&R, promote.body, "if (c + 1 < end && $name_ElemLt($pararg, *elems[c + 1], *elems[c])) {");
        Ins(&R, promote.body, "    c++;");
        Ins(&R, promote.body, "}");
        Ins(&R, promote.body, "$Cpptype &row = *elems[c];");
        Ins(&R, promote.body, "for (int i = $rmax - $parname.$name_nrun; i <= $rlast; i++) {");
        Ins(&R, promote.body, "    elems[i - 1] = elems[i];");
        Ins(&R, promote.body, "    elems[i - 1]->$xfname_idx = i - 1;");
        Ins(&R, promote.body, "}");
        Ins(&R, promote.body, "$parname.$name_nrun++;");
        Ins(&R, promote.body, "elems[$rlast] = &row;");
        Ins(&R, promote.body, "row.$xfname_idx = $rlast;");
        Ins(&R, promote.body, "$parname.$name_nheap--;");
        Ins(&R, promote.body, "int last = $rmax + $parname.$name_nheap;");
        Ins(&R, promote.body, "if (c != last) {");
        Ins(&R, promote.body, "    $Cpptype &elem = *elems[last];");
        Ins(&R, promote.body, "    int idx = $name_Downheap($pararg, elem, c);");
        Ins(&R, promote.body, "    elem.$xfname_idx = idx;");
        Ins(&R, promote.body, "    elems[idx] = &elem;");
        Ins(&R, promote.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate Reheap, which inserts a row or moves it after its key changed.
// Without a run, the row moves up or down from its slot.  With a run, a root
// that is the run's only row sinks in place, a heap row whose key is still at
// least the root moves within the heap, and any other row leaves and enters
// again.  A row already in the heap fires no OnXref or OnUnref, since it never
// leaves the index.
void amc::tfunc_Bheap_Reheap() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& reheap = amc::CreateCurFunc();
    Ins(&R, reheap.comment, "If row is in heap, update its position. If row is not in heap, insert it.");
    Ins(&R, reheap.ret  , "i32", false);
    Ins(&R, reheap.proto, "$name_Reheap($Parent, $Cpptype& row)", false);
    if (RunQ(field)) {
        Ins(&R, reheap.comment, "Return new slot of the row");
        Ins(&R, reheap.body, "int idx = row.$xfname_idx;");
        if (field.need_firstchanged) {
            Ins(&R, reheap.body, "bool wasfirst = idx != -1 && idx == $rmax - $parname.$name_nrun;");
        }
        Ins(&R, reheap.body, "if (idx == $rlast && $parname.$name_nrun == 1) {");
        Ins(&R, reheap.body, "    int new_idx = $name_Downheap($pararg, row, idx);");
        Ins(&R, reheap.body, "    row.$xfname_idx = new_idx;");
        Ins(&R, reheap.body, "    $parname.$name_elems[new_idx] = &row;");
        Ins(&R, reheap.body, "} else if (idx > $rlast && !$name_ElemLt($pararg, row, *$parname.$name_elems[$rlast])) {");
        Ins(&R, reheap.body, "    int new_idx = $name_Upheap($pararg, row, idx);");
        Ins(&R, reheap.body, "    if (new_idx == idx) {");
        Ins(&R, reheap.body, "        new_idx = $name_Downheap($pararg, row, idx);");
        Ins(&R, reheap.body, "    }");
        Ins(&R, reheap.body, "    row.$xfname_idx = new_idx;");
        Ins(&R, reheap.body, "    $parname.$name_elems[new_idx] = &row;");
        Ins(&R, reheap.body, "} else {");
        Ins(&R, reheap.body, "    if (idx != -1) {");
        Ins(&R, reheap.body, "        $name_RemoveImpl($pararg, row);");
        Ins(&R, reheap.body, "    }");
        Ins(&R, reheap.body, "    $name_InsertImpl($pararg, row);");
        Ins(&R, reheap.body, "}");
        if (field.need_firstchanged) {
            Ins(&R, reheap.comment, "If first item of the heap is changed, update fstep:$field");
            Ins(&R, reheap.body, "if (wasfirst || row.$xfname_idx == $rmax - $parname.$name_nrun) {");
            Ins(&R, reheap.body, "    $name_FirstChanged($pararg);");
            Ins(&R, reheap.body, "}");
        }
        Ins(&R, reheap.body, "return row.$xfname_idx;");
    } else {
        Ins(&R, reheap.comment, "Return new position of item in the heap (0=top)");
        Ins(&R, reheap.body    , "int old_idx = row.$xfname_idx;");
        Ins(&R, reheap.body    , "bool isnew = old_idx == -1;");
        Ins(&R, reheap.body    , "if (isnew) {");
        Ins(&R, reheap.body    , "    $name_Reserve($pararg, 1);");
        Ins(&R, reheap.body    , "    old_idx = $parname.$name_n++;");
        Ins(&R, reheap.body    , "}");
        Ins(&R, reheap.body    , "int new_idx = $name_Upheap($pararg, row, old_idx);");
        Ins(&R, reheap.body    , "if (!isnew && new_idx == old_idx) {");
        Ins(&R, reheap.body    , "    new_idx = $name_Downheap($pararg, row, old_idx);");
        Ins(&R, reheap.body    , "}");
        Ins(&R, reheap.body    , "row.$xfname_idx = new_idx;");
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
}

// -----------------------------------------------------------------------------

// Generate ReheapFirst, which moves the first row after its key grew.  The root
// alone in the run sinks in place; the first of several run rows leaves and
// enters again.
void amc::tfunc_Bheap_ReheapFirst() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& reheapfirst = amc::CreateCurFunc();
    Ins(&R, reheapfirst.comment, "Key of first element in the heap changed. Move it.");
    Ins(&R, reheapfirst.comment, "This function does not check the insert condition.");
    if (RunQ(field)) {
        Ins(&R, reheapfirst.comment, "Heap must be non-empty or behavior is undefined.");
        Ins(&R, reheapfirst.ret  , "i32", false);
        Ins(&R, reheapfirst.proto, "$name_ReheapFirst($Parent)", false);
        Ins(&R, reheapfirst.body, "$Cpptype &row = *$parname.$name_elems[$rmax - $parname.$name_nrun];");
        Ins(&R, reheapfirst.body, "if ($parname.$name_nrun == 1) {");
        Ins(&R, reheapfirst.body, "    int new_idx = $name_Downheap($pararg, row, $rlast);");
        Ins(&R, reheapfirst.body, "    row.$xfname_idx = new_idx;");
        Ins(&R, reheapfirst.body, "    $parname.$name_elems[new_idx] = &row;");
        Ins(&R, reheapfirst.body, "} else {");
        Ins(&R, reheapfirst.body, "    $name_RemoveImpl($pararg, row);");
        Ins(&R, reheapfirst.body, "    $name_InsertImpl($pararg, row);");
        Ins(&R, reheapfirst.body, "}");
        if (field.need_firstchanged) {
            Ins(&R, reheapfirst.comment, "Update fstep:$field");
            Ins(&R, reheapfirst.body, "if (row.$xfname_idx != $rmax - $parname.$name_nrun) {");
            Ins(&R, reheapfirst.body, "    $name_FirstChanged($pararg);");
            Ins(&R, reheapfirst.body, "}");
        }
        Ins(&R, reheapfirst.body, "return row.$xfname_idx;");
    } else {
        Ins(&R, reheapfirst.comment, "Return new position of item in the heap (0=top).");
        Ins(&R, reheapfirst.comment, "Heap must be non-empty or behavior is undefined.");
        Ins(&R, reheapfirst.ret  , "i32", false);
        Ins(&R, reheapfirst.proto, "$name_ReheapFirst($Parent)", false);
        Ins(&R, reheapfirst.body    , "$Cpptype &row = *$parname.$name_elems[0];");
        Ins(&R, reheapfirst.body    , "i32 new_idx = $name_Downheap($pararg, row, 0);");
        Ins(&R, reheapfirst.body    , "row.$xfname_idx = new_idx;");
        Ins(&R, reheapfirst.body    , "$parname.$name_elems[new_idx] = &row;");
        if (field.need_firstchanged) {
            Ins(&R, reheapfirst.comment, "Update fstep:$field");
            Ins(&R, reheapfirst.body, "if (new_idx != 0) {");
            Ins(&R, reheapfirst.body, "    $name_FirstChanged($pararg);");
            Ins(&R, reheapfirst.body, "}");
        }
        Ins(&R, reheapfirst.body, "return new_idx;");
    }
}

// -----------------------------------------------------------------------------

// Generate Set, which writes the key of a row and repositions it.  With a run,
// Reheap and Remove fire FirstChanged themselves.
void amc::tfunc_Bheap_Set() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (NeedSetQ(field)) {
        bool firstchanged = field.need_firstchanged && !RunQ(field);
        amc::FFunc& set = amc::CreateCurFunc();
        Ins(&R, set.comment, "Set row key to new value.");
        Ins(&R, set.comment, "Update heap membership based on insert condition [$inscond]");
        Ins(&R, set.ret  , "void", false);
        Ins(&R, set.proto, "$sortfld_Set($Parent, $Cpptype &row, $Sortfldstore new_key)", false);
        Ins(&R, set.body, "row.$sortfld = new_key;");
        if (firstchanged) {
            Ins(&R, set.body, "int old_idx = row.$xfname_idx;");
        }
        Ins(&R, set.body, "bool ins = $inscond; // user-defined insert condition (xref)");
        Ins(&R, set.body, "if (ins) {");
        Ins(&R, set.body, "    $name_Reheap($pararg, row);");
        Ins(&R, set.body, "} else {");
        Ins(&R, set.body, "    $name_Remove($pararg, row);");
        Ins(&R, set.body, "}");
        if (firstchanged) {
            Ins(&R, set.body, "int new_idx = row.$xfname_idx;");
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

// -----------------------------------------------------------------------------

// Generate SetIfBetter, which writes the key of a row unless the row is in
// the heap with a smaller key.
void amc::tfunc_Bheap_SetIfBetter() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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

// -----------------------------------------------------------------------------

// Generate Cascdel, which deletes every row in the heap.  It always takes the
// row whose removal moves no other row: the last one without a run, and with a
// run the last heap row while the heap holds one, then the first row of the run.
void amc::tfunc_Bheap_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();
        Ins(&R, cascdel.comment, "Delete all elements referenced by the heap.");
        if (RunQ(field)) {
            Ins(&R, cascdel.body, "while ($parname.$name_nrun > 0) {");
            Ins(&R, cascdel.body, "    int idx = $parname.$name_nheap > 0 ? $rlast + $parname.$name_nheap : $rmax - $parname.$name_nrun;");
            Ins(&R, cascdel.body, "    $Cpptype &elem = *$parname.$name_elems[idx];");
            Ins(&R, cascdel.body, "    $name_RemoveImpl($pararg, elem);");
        } else {
            Ins(&R, cascdel.body, "i32 n = $parname.$name_n;");
            Ins(&R, cascdel.body, "while (n > 0) {");
            Ins(&R, cascdel.body, "    n--;");
            Ins(&R, cascdel.body, "    $Cpptype &elem = *$parname.$name_elems[n]; // pick cheapest element to remove");
            Ins(&R, cascdel.body, "    elem.$xfname_idx = -1; // mark not-in-heap");
            Ins(&R, cascdel.body, "    $parname.$name_n = n;");
        }
        Ins(&R, cascdel.body, DeleteExpr(field,"$pararg","elem")<<";");
        Ins(&R, cascdel.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate RemoveFirst, which takes the smallest row.  With a run of two or
// more rows, that is the run's leftmost slot, and nothing else moves.
void amc::tfunc_Bheap_RemoveFirst() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& remfirst = amc::CreateCurFunc();
    Ins(&R, remfirst.ret  , "$Cpptype*", false);
    Ins(&R, remfirst.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R, remfirst.body, "$Cpptype *row = NULL;");
    if (RunQ(field)) {
        Ins(&R, remfirst.comment, "If index is empty, return NULL. Otherwise remove and return first key in index.", false);
        Ins(&R, remfirst.body, "if ($parname.$name_nrun > 0) {");
        Ins(&R, remfirst.body, "    row = $parname.$name_elems[$rmax - $parname.$name_nrun];");
        Ins(&R, remfirst.body, "    $name_RemoveImpl($pararg, *row);");
    } else {
        Ins(&R, remfirst.comment, "If index is empty, return NULL. Otherwise remove and return first key in index.\n Call 'head changed' trigger.", false);
        Ins(&R, remfirst.body, "if ($parname.$name_n > 0) {");
        Ins(&R, remfirst.body, "    row = $parname.$name_elems[0];");
        Ins(&R, remfirst.body, "    row->$xfname_idx = -1;           // mark not in heap");
        Ins(&R, remfirst.body, "    i32 n = $parname.$name_n - 1; // index of last element in heap");
        Ins(&R, remfirst.body, "    $parname.$name_n = n;         // decrease count");
        Ins(&R, remfirst.body, "    if (n) {");
        Ins(&R, remfirst.body, "        $Cpptype &elem = *$parname.$name_elems[n];");
        Ins(&R, remfirst.body, "        int new_idx = $name_Downheap($pararg, elem, 0);");
        Ins(&R, remfirst.body, "        elem.$xfname_idx = new_idx;");
        Ins(&R, remfirst.body, "        $parname.$name_elems[new_idx] = &elem;");
        Ins(&R, remfirst.body, "    }");
    }
    if (field.need_firstchanged) {
        Ins(&R, remfirst.body, "    $name_FirstChanged($pararg);");
    }
    Ins(&R, remfirst.body, "}");
    Ins(&R, remfirst.body, "return row;");
}

// -----------------------------------------------------------------------------

// Generate First, which returns the smallest row or NULL.
void amc::tfunc_Bheap_First() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& first = amc::CreateCurFunc(true); {
        AddRetval(first,Subst(R,"$Cpptype*"),"row","NULL");
    }
    if (RunQ(field)) {
        Ins(&R, first.body, "if ($parname.$name_nrun > 0) {");
        Ins(&R, first.body, "    row = $parname.$name_elems[$rmax - $parname.$name_nrun];");
    } else {
        Ins(&R, first.body , "if ($parname.$name_n > 0) {");
        Ins(&R, first.body , "    row = $parname.$name_elems[0];");
    }
    Ins(&R, first.body , "}");
}

// -----------------------------------------------------------------------------

// Generate InBheapQ, which tests a row for membership.  It takes the row alone,
// since the row's slot says whether it is in the heap.
void amc::tfunc_Bheap_InBheapQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& inheap = amc::CreateCurFunc();
    Ins(&R, inheap.ret  , "bool", false);
    Ins(&R, inheap.proto, "$name_InBheapQ($Cpptype& row)", false);
    Ins(&R, inheap.body, "bool result = false;");
    Ins(&R, inheap.body, "result = row.$xfname_idx != -1;");
    Ins(&R, inheap.body, "return result;");
}

// -----------------------------------------------------------------------------

// Generate Insert, which adds a row that is not in the heap.
void amc::tfunc_Bheap_Insert() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R, insert.comment, "Insert row. Row must not already be in index. If row is already in index, do nothing.", false);
    Ins(&R, insert.ret  , "void", false);
    Ins(&R, insert.proto, "$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R, insert.body,     "if (LIKELY(row.$xfname_idx == -1)) {");
    if (RunQ(field)) {
        Ins(&R, insert.body, "    $name_InsertImpl($pararg, row);");
        Set(R, "$isfirst", Subst(R, "row.$xfname_idx == $rmax - $parname.$name_nrun"));
    } else {
        Ins(&R, insert.body, "    $name_Reserve($pararg, 1);");
        Ins(&R, insert.body, "    int n = $parname.$name_n;");
        Ins(&R, insert.body, "    $parname.$name_n = n + 1;");
        Ins(&R, insert.body, "    int new_idx = $name_Upheap($pararg, row, n);");
        Ins(&R, insert.body, "    row.$xfname_idx = new_idx;");
        Ins(&R, insert.body, "    $parname.$name_elems[new_idx] = &row;");
        Set(R, "$isfirst", "new_idx==0");
    }
    if (field.need_firstchanged) {
        Ins(&R, insert.body, "    if ($isfirst) {");
        Ins(&R, insert.body, "        $name_FirstChanged($pararg);");
        Ins(&R, insert.body, "    }");
    }
    if (amc::FindFfunc(field, amcdb_cbtype_OnXref, true)) {
        Ins(&R, insert.body, "    $name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
    }
    Ins(&R, insert.body    , "}");
}

// -----------------------------------------------------------------------------

// Generate Compact, which halves the pointer array when the rows fill less than
// a quarter of it.
void amc::tfunc_Bheap_Compact() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.c_fcompact) {
        amc::FFunc& compact = amc::CreateCurFunc(true);
        AddRetval(compact, "bool", "retval", "false");
        if (RunQ(field)) {
            Ins(&R, compact.body, "if (($rmax + $parname.$name_nheap) * 4 < $parname.$name_max) {");
        } else {
            Ins(&R, compact.body, "if (i32_Max($parname.$name_n * 4,8) < $parname.$name_max) {");
        }
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

// -----------------------------------------------------------------------------

// Generate Remove, which takes a row out of the heap.
void amc::tfunc_Bheap_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R,  remove.ret  , "void", false);
    Ins(&R,  remove.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R,  remove.body    , "if ($name_InBheapQ(row)) {");
    Ins(&R,  remove.body    , "    int old_idx = row.$xfname_idx;");
    Ins(&R,  remove.body    , "    if ($parname.$name_elems[old_idx] == &row) { // sanity check: heap points back to row");
    if (RunQ(field)) {
        if (field.need_firstchanged) {
            Ins(&R, remove.body, "        bool wasfirst = old_idx == $rmax - $parname.$name_nrun;");
        }
        Ins(&R, remove.body, "        $name_RemoveImpl($pararg, row);");
    } else {
        Ins(&R,  remove.body    , "        row.$xfname_idx = -1;           // mark not in heap");
        Ins(&R,  remove.body    , "        i32 n = $parname.$name_n - 1; // index of last element in heap");
        Ins(&R,  remove.body    , "        $parname.$name_n = n;         // decrease count");
        Ins(&R,  remove.body    , "        if (old_idx != n) {");
        Ins(&R,  remove.body    , "            $Cpptype *elem = $parname.$name_elems[n];");
        Ins(&R,  remove.body    , "            int new_idx = $name_Upheap($pararg, *elem, old_idx);");
        Ins(&R,  remove.body    , "            if (new_idx == old_idx) {");
        Ins(&R,  remove.body    , "                new_idx = $name_Downheap($pararg, *elem, old_idx);");
        Ins(&R,  remove.body    , "            }");
        Ins(&R,  remove.body    , "            elem->$xfname_idx = new_idx;");
        Ins(&R,  remove.body    , "            $parname.$name_elems[new_idx] = elem;");
        Ins(&R,  remove.body    , "        }");
    }
    if (field.c_fcompact) {
        Ins(&R,  remove.body, "        $name_Compact($pararg);");
    }
    if (field.need_firstchanged) {
        Ins(&R,  remove.body, RunQ(field) ? "        if (wasfirst) {" : "        if (old_idx == 0) {");
        Ins(&R,  remove.body, "            $name_FirstChanged($pararg);");
        Ins(&R,  remove.body, "        }");
    }
    if (amc::FindFfunc(field, amcdb_cbtype_OnUnref, true)) {
        Ins(&R,  remove.body, "        $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
    }
    Ins(&R,  remove.body    , "    }");
    Ins(&R,  remove.body    , "}");
}

// -----------------------------------------------------------------------------

// Generate N, the number of rows in the heap.
void amc::tfunc_Bheap_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.comment, "Return number of items in the heap");
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, RunQ(field) ? "return $parname.$name_nrun + $parname.$name_nheap;" : "return $parname.$name_n;");
}

// -----------------------------------------------------------------------------

// Generate EmptyQ
void amc::tfunc_Bheap_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.comment, "Return true if index is empty");
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, RunQ(field) ? "return $parname.$name_nrun == 0;" : "return $parname.$name_n == 0;");
}

// -----------------------------------------------------------------------------

// Generate RemoveAll, which empties the heap and keeps its memory.
void amc::tfunc_Bheap_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& flush = amc::CreateCurFunc();
    Ins(&R, flush.ret  , "void", false);
    Ins(&R, flush.proto, "$name_RemoveAll($Parent)", false);
    if (RunQ(field)) {
        Ins(&R, flush.comment, "Remove all elements from the heap");
        Ins(&R, flush.body, "int end = $rmax + $parname.$name_nheap;");
        Ins(&R, flush.body, "int first = $rmax - $parname.$name_nrun;");
        Ins(&R, flush.body, "for (int i = first; i < end; i++) {");
        Ins(&R, flush.body, "    $parname.$name_elems[i]->$xfname_idx = -1; // mark not-in-heap");
        Ins(&R, flush.body, "}");
        Ins(&R, flush.body, "$parname.$name_nrun = 0;");
        Ins(&R, flush.body, "$parname.$name_nheap = 0;");
        Set(R, "$nonempty", "first < end");
    } else {
        Ins(&R, flush.comment, "Remove all elements from binary heap");
        Ins(&R, flush.body    , "int n = $parname.$name_n;");
        Ins(&R, flush.body    , "for (int i = n - 1; i>=0; i--) {");
        Ins(&R, flush.body    , "    $parname.$name_elems[i]->$xfname_idx = -1; // mark not-in-heap");
        Ins(&R, flush.body    , "}");
        Ins(&R, flush.body    , "$parname.$name_n = 0;");
        Set(R, "$nonempty", "n > 0");
    }
    if (field.need_firstchanged) {
        Ins(&R, flush.body, "if ($nonempty) {");
        Ins(&R, flush.body, "    $name_FirstChanged($pararg);");
        Ins(&R, flush.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate Dealloc, which empties the heap and frees its memory.
void amc::tfunc_Bheap_Dealloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& dealloc = amc::CreateCurFunc();
    Ins(&R, dealloc.comment, "Remove all elements from heap and free memory used by the array.");
    Ins(&R, dealloc.ret  , "void", false);
    Ins(&R, dealloc.proto, "$name_Dealloc($Parent)", false);
    Ins(&R, dealloc.body, "$name_RemoveAll($pararg);");
    Ins(&R, dealloc.body, "$basepool_FreeMem($parname.$name_elems, sizeof($Cpptype*)*$parname.$name_max);");
    Ins(&R, dealloc.body, "$parname.$name_max   = 0;");
    Ins(&R, dealloc.body, "$parname.$name_elems = NULL;");
}

// -----------------------------------------------------------------------------

// Generate Reserve, which makes room for N more rows.
void amc::tfunc_Bheap_Reserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.ret  , "void", false);
    Ins(&R, reserve.proto, "$name_Reserve($Parent, int n)", false);
    Ins(&R, reserve.body, "i32 old_max = $parname.$name_max;");
    if (RunQ(field)) {
        Ins(&R, reserve.body, "if (UNLIKELY($rmax + $parname.$name_nheap + n > old_max)) {");
        Ins(&R, reserve.body, "    u32 new_max  = u32_Max(4, old_max * 2);");
        Ins(&R, reserve.body, "    while (new_max < u32($rmax + $parname.$name_nheap + n)) {");
        Ins(&R, reserve.body, "        new_max *= 2;");
        Ins(&R, reserve.body, "    }");
    } else {
        Ins(&R, reserve.body, "if (UNLIKELY($parname.$name_n + n > old_max)) {");
        Ins(&R, reserve.body, "    u32 new_max  = u32_Max(4, old_max * 2);");
    }
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

// -----------------------------------------------------------------------------

// Generate the Init statements of the parent
void amc::tfunc_Bheap_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& init = amc::CreateCurFunc();
    if (RunQ(field)) {
        Ins(&R, init.body, "$parname.$name_elems \t= NULL; // ($field)");
        Ins(&R, init.body, "$parname.$name_nrun \t= 0; // ($field)");
        Ins(&R, init.body, "$parname.$name_nheap \t= 0; // ($field)");
        Ins(&R, init.body, "$parname.$name_max \t= 0; // ($field)");
    } else {
        Ins(&R, init.body, "$parname.$name_max   \t= 0; // ($field)");
        Ins(&R, init.body, "$parname.$name_n     \t= 0; // ($field)");
        Ins(&R, init.body, "$parname.$name_elems \t= NULL; // ($field)");
    }
}

// -----------------------------------------------------------------------------

// Generate the Uninit statements of the parent
void amc::tfunc_Bheap_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (field.p_ctype == field.p_ctype->p_ns->c_globfld->p_ctype) {
        Ins(&R, uninit.body, "// skip destruction in global scope");
    } else {
        Ins(&R, uninit.body, "$basepool_FreeMem((u8*)$parname.$name_elems, sizeof($Cpptype*)*$parname.$name_max); // ($field)");
    }
}

// -----------------------------------------------------------------------------

// Generate ElemLt, which compares the sort fields of rows A and B.
void amc::tfunc_Bheap_ElemLt() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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

// -----------------------------------------------------------------------------

// Generate ElemLtval, which compares the key of row A with the value B.
void amc::tfunc_Bheap_ElemLtval() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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

// Generate the unordered cursor, which walks the occupied slots in index
// order.
void amc::tfunc_Bheap_unordcurs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");
    // with a run, the occupied slots start at the run's first row
    Set(R, "$first", RunQ(field) ? Subst(R, "$rmax - parent.$name_nrun") : tempstr("0"));
    Set(R, "$end", RunQ(field) ? Subst(R, "$rmax + parent.$name_nheap") : Subst(R, "parent.$name_n"));

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
        amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field.unordcurs_Reset"));
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_unordcurs_Reset($Parname_$name_unordcurs &unordcurs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "unordcurs.elems = parent.$name_elems;");
        Ins(&R, curs_reset.body, "unordcurs.n_elems = $end;");
        Ins(&R, curs_reset.body, "unordcurs.index = $first;");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field.unordcurs_ValidQ"));
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_unordcurs_ValidQ($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_validq.body, "return unordcurs.index < unordcurs.n_elems;");
    }

    {
        amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field.unordcurs_Next"));
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_unordcurs_Next($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_next.body, "unordcurs.index++;");
    }

    {
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field.unordcurs_Access"));
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_unordcurs_Access($Parname_$name_unordcurs &unordcurs)", false);
        Ins(&R, curs_access.body, "return *unordcurs.elems[unordcurs.index];");
    }
}

// -----------------------------------------------------------------------------

// Generate the cursor's Add, which pushes ROW into the helper heap of the rows
// that may come next, growing it as the walk widens.
static void GenCursAdd() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& curs_add = amc::ind_func_GetOrCreate(Subst(R,"$field_$curstype.Add"));
    curs_add.priv = true;
    Ins(&R, curs_add.ret  , "void", false);
    Ins(&R, curs_add.proto, "$Parname_$name_$curstype_Add($Parname_$name_$curstype &curs, $Cpptype& row)", false);
    Ins(&R, curs_add.body, "// the helper heap grows as the walk widens, so a walk that stops early allocates little");
    Ins(&R, curs_add.body, "if (curs.temp_n == curs.temp_max) {");
    Ins(&R, curs_add.body, "    int max = i32_Max(16, curs.temp_max * 2);");
    Ins(&R, curs_add.body, "    curs.temp_elems = ($Cpptype**)$basepool_ReallocMem(curs.temp_elems, sizeof(void*) * curs.temp_max, sizeof(void*) * max);");
    Ins(&R, curs_add.body, "    if (!curs.temp_elems) {");
    Ins(&R, curs_add.body, "        algo::FatalErrorExit(\"$ns.cursor_out_of_memory  func:$field_$curstype.Add\");");
    Ins(&R, curs_add.body, "    }");
    Ins(&R, curs_add.body, "    curs.temp_max = max;");
    Ins(&R, curs_add.body, "}");
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

// -----------------------------------------------------------------------------

// Generate the sorted cursor without a run.  It keeps a helper heap of the rows
// that may come next, seeded with the root, and never writes to the Bheap.
static void GenCursHeap(amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *field.p_ctype->p_ns;

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

    GenCursAdd();

    {
        amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reset"));
        Ins(&R, curs_reset.comment, "Reset cursor. If HEAP is non-empty, add its top element to CURS.", false);
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "curs.parent       = &parent;");
        Ins(&R, curs_reset.body, "curs.temp_n = 0;");
        Ins(&R, curs_reset.body, "if (parent.$name_n > 0) {");
        Ins(&R, curs_reset.body, "    $Parname_$name_curs_Add(curs, *parent.$name_elems[0]);");
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
        Ins(&R, curs_next.body, "    int index = dead->$xfname_idx;");
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
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
        Ins(&R, curs_access.comment, "Access current element. If not more elements, return NULL");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return *curs.temp_elems[0];");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
        Ins(&R, curs_validq.comment, "Return true if Access() will return non-NULL.");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.temp_n > 0;");
    }
}

// -----------------------------------------------------------------------------

// Generate a sorted cursor of a Bheap with a run.  It walks the run in place,
// and past the root it keeps a helper heap of the heap rows that may come next,
// seeded with the root's children; the curs cursor never writes to the Bheap.
// With FILL, it is the fillcurs cursor: when it reaches the root while the run
// has room, it promotes the smallest heap row into the run, so a walk of K rows
// leaves a run of up to K sorted rows behind for the pops and walks that follow.
// A fillcurs walk changes the Bheap's layout, so nothing else may walk the Bheap
// while one is active.
static void GenCursRun(amc::FField &field, bool fill) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *field.p_ctype->p_ns;

    Ins(&R, ns.curstext, fill
        ? "// Sorted heap cursor that promotes heap rows into the run as it walks; nothing else may walk the heap meanwhile."
        : "// Sorted heap cursor: the run in place, then the heap through a helper heap.");
    Ins(&R, ns.curstext, "struct $Parname_$name_$curstype {");
    Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext, "    $Partype      *parent;        // parent");
    Ins(&R, ns.curstext, "    int            k;             // run rows walked so far");
    Ins(&R, ns.curstext, "    $Cpptype*     *temp_elems;    // helper heap of the heap rows that may come next");
    Ins(&R, ns.curstext, "    int            temp_n;        // number of elements in the helper heap");
    Ins(&R, ns.curstext, "    int            temp_max;      // capacity of the helper heap");
    Ins(&R, ns.curstext, "    $Parname_$name_$curstype() : parent(NULL), k(0), temp_elems(NULL), temp_n(0), temp_max(0) {}");
    Ins(&R, ns.curstext, "    ~$Parname_$name_$curstype();");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");

    Ins(&R, *ns.cpp, "$ns::$Parname_$name_$curstype::~$Parname_$name_$curstype() {");
    Ins(&R, *ns.cpp, "    $basepool_FreeMem(temp_elems, sizeof(void*) * temp_max);");
    Ins(&R, *ns.cpp, "}\n");

    GenCursAdd();

    amc::FFunc& curs_addchild = amc::ind_func_GetOrCreate(Subst(R, "$field_$curstype.AddChild"));
    curs_addchild.priv = true;
    Ins(&R, curs_addchild.ret  , "void", false);
    Ins(&R, curs_addchild.proto, "$Parname_$name_$curstype_AddChild($Parname_$name_$curstype &curs, int idx)", false);
    Ins(&R, curs_addchild.body, "int end = $rmax + curs.parent->$name_nheap;");
    Ins(&R, curs_addchild.body, "int child = 2 * idx - $rlast + 1;");
    Ins(&R, curs_addchild.body, "if (child < end) {");
    Ins(&R, curs_addchild.body, "    $Parname_$name_$curstype_Add(curs, *curs.parent->$name_elems[child]);");
    Ins(&R, curs_addchild.body, "}");
    Ins(&R, curs_addchild.body, "if (child + 1 < end) {");
    Ins(&R, curs_addchild.body, "    $Parname_$name_$curstype_Add(curs, *curs.parent->$name_elems[child + 1]);");
    Ins(&R, curs_addchild.body, "}");

    amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R, "$field_$curstype.Reset"));
    Ins(&R, curs_reset.comment, "Reset cursor to the first row of the run.", false);
    Ins(&R, curs_reset.ret  , "void", false);
    Ins(&R, curs_reset.proto, "$Parname_$name_$curstype_Reset($Parname_$name_$curstype &curs, $Partype &parent)", false);
    Ins(&R, curs_reset.body, "curs.parent = &parent;");
    Ins(&R, curs_reset.body, "curs.k = 0;");
    Ins(&R, curs_reset.body, "curs.temp_n = 0;");

    amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R, "$field_$curstype.Next"));
    Ins(&R, curs_next.comment, "Advance cursor.", false);
    Ins(&R, curs_next.ret  , "void", false);
    Ins(&R, curs_next.proto, "$Parname_$name_$curstype_Next($Parname_$name_$curstype &curs)", false);
    Ins(&R, curs_next.body, "$Partype &parent = *curs.parent;");
    Ins(&R, curs_next.body, "if (curs.k < parent.$name_nrun) {");
    Ins(&R, curs_next.body, "    curs.k++;");
    if (fill) {
        Ins(&R, curs_next.body, "    if (curs.k == parent.$name_nrun && parent.$name_nheap > 0) {");
        Ins(&R, curs_next.body, "        if (parent.$name_nrun < $rmax) {");
        Ins(&R, curs_next.body, "            $name_Promote($curspararg);");
        Ins(&R, curs_next.body, "        } else {");
        Ins(&R, curs_next.body, "            $Parname_$name_$curstype_AddChild(curs, $rlast);");
        Ins(&R, curs_next.body, "        }");
        Ins(&R, curs_next.body, "    }");
    } else {
        Ins(&R, curs_next.body, "    if (curs.k == parent.$name_nrun) {");
        Ins(&R, curs_next.body, "        $Parname_$name_$curstype_AddChild(curs, $rlast);");
        Ins(&R, curs_next.body, "    }");
    }
    Ins(&R, curs_next.body, "} else if (curs.temp_n > 0) {");
    Ins(&R, curs_next.body, "    $Cpptype* *elems = curs.temp_elems;");
    Ins(&R, curs_next.body, "    int top = elems[0]->$xfname_idx;");
    Ins(&R, curs_next.body, "    int n = curs.temp_n - 1;");
    Ins(&R, curs_next.body, "    curs.temp_n = n;");
    Ins(&R, curs_next.body, "    $Cpptype* last = elems[n];");
    Ins(&R, curs_next.body, "    int i = 0;");
    Ins(&R, curs_next.body, "    for (;;) {");
    Ins(&R, curs_next.body, "        int l = i * 2 + 1;");
    Ins(&R, curs_next.body, "        if (l >= n) {");
    Ins(&R, curs_next.body, "            break;");
    Ins(&R, curs_next.body, "        }");
    Ins(&R, curs_next.body, "        if (l + 1 < n && $name_ElemLt($curspararg, *elems[l + 1], *elems[l])) {");
    Ins(&R, curs_next.body, "            l++;");
    Ins(&R, curs_next.body, "        }");
    Ins(&R, curs_next.body, "        if (!$name_ElemLt($curspararg, *elems[l], *last)) {");
    Ins(&R, curs_next.body, "            break;");
    Ins(&R, curs_next.body, "        }");
    Ins(&R, curs_next.body, "        elems[i] = elems[l];");
    Ins(&R, curs_next.body, "        i = l;");
    Ins(&R, curs_next.body, "    }");
    Ins(&R, curs_next.body, "    elems[i] = last;");
    Ins(&R, curs_next.body, "    $Parname_$name_$curstype_AddChild(curs, top);");
    Ins(&R, curs_next.body, "}");

    amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R, "$field_$curstype.Access"));
    Ins(&R, curs_access.comment, "Access current element.");
    Ins(&R, curs_access.ret  , "$Cpptype&", false);
    Ins(&R, curs_access.proto, "$Parname_$name_$curstype_Access($Parname_$name_$curstype &curs)", false);
    Ins(&R, curs_access.body, "$Partype &parent = *curs.parent;");
    Ins(&R, curs_access.body, "return curs.k < parent.$name_nrun ? *parent.$name_elems[$rmax - parent.$name_nrun + curs.k] : *curs.temp_elems[0];");

    amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R, "$field_$curstype.ValidQ"));
    Ins(&R, curs_validq.comment, "Return true if Access() will return non-NULL.");
    Ins(&R, curs_validq.ret  , "bool", false);
    Ins(&R, curs_validq.proto, "$Parname_$name_$curstype_ValidQ($Parname_$name_$curstype &curs)", false);
    Ins(&R, curs_validq.body, "return curs.k < curs.parent->$name_nrun || curs.temp_n > 0;");
}

// -----------------------------------------------------------------------------

// Generate the sorted cursor, which walks the rows in key order.
void amc::tfunc_Bheap_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");
    if (RunQ(field)) {
        GenCursRun(field, false);
    } else {
        GenCursHeap(field);
    }
}

// -----------------------------------------------------------------------------

// Generate the fillcurs cursor, a sorted cursor that fills the run as it walks.
// A field asks for it with a dmmeta.fcurs row, and only a Bheap with a head has
// a run to fill, so amc refuses it on a field without one.
void amc::tfunc_Bheap_fillcurs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    bool glob = GlobalQ(*field.p_ctype);
    Set(R, "$cursparent", glob ? "_db" : "(*curs.parent)");
    Set(R, "$curspararg", glob ? "" : "(*curs.parent)");
    if (RunQ(field)) {
        GenCursRun(field, true);
    } else {
        prerr("amc.fillcurs_without_head"
              <<Keyval("field",field.field)
              <<Keyval("comment","fillcurs fills the sorted head; give the field a dmmeta.bheap row with nhead 2 or more"));
        algo_lib::_db.exit_code++;
    }
}
