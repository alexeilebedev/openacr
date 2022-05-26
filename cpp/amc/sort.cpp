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
// Source: cpp/amc/sort.cpp -- Comparison & Sorting routines
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//
// TODO: investigate 3-way partitioning, or dual-pivot partitioning.
// http://www.sorting-algorithms.com/static/QuicksortIsOptimal.pdf

#include "include/amc.h"

void amc::tclass_Sort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    amc::FCtype         &tgttype    = *fsort.p_field->p_arg;
    bool                 is_value   = (fsort.p_sortfld->reftype == dmmeta_Reftype_reftype_Val);

    bool is_ary     = (field.reftype == dmmeta_Reftype_reftype_Tary);
    is_ary         |= (field.reftype == dmmeta_Reftype_reftype_Ptrary);
    is_ary         |= (field.reftype == dmmeta_Reftype_reftype_Varlen);
    is_ary         |= (field.reftype == dmmeta_Reftype_reftype_Inlary);

    bool                 is_ptr     = (field.reftype == dmmeta_Reftype_reftype_Ptrary);

    // do sanity checks
    vrfy(is_value, "Can't sort by field of pool or index nature");
    vrfy(is_ary, "Field for sort must be an array contiguous in memory");
    // don't allow generating by-val sort routine for instantiated ctype with xrefs
    vrfy(is_ptr || FirstInst(tgttype) || !tgttype.n_xref, "Cannot generate by-value sort for ctype with xrefs");

    // fill replacement variables
    Set(R, "$Tgttype"  , tgttype.cpp_type); // type of sort field's ctype
    Set(R, "$ctype"    , tgttype.ctype);
    Set(R, "$Sorttype" , fsort.p_sortfld->p_ctype->cpp_type);
    Set(R, "$sortfld"  , name_Get(*fsort.p_sortfld)); // name of field by which array is being sorted

    Set(R, "$Ptr", (is_ptr ? "*" : ""));
    // This is a trick to take care of dereferences.
    Set(R, "$*"  , (is_ptr ? "*"   : ""));
    // empty substitution eats whitespace! so must define $aryelemtype separately
    Set(R, "$aryelemtype" , "$Tgttype$*");
}

void amc::tfunc_Sort_Swap() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    bool                 is_ptr     = (field.reftype == dmmeta_Reftype_reftype_Ptrary);
    bool                 cheap_copy = amc::CheapCopyQ(field);
    // Swap
    amc::FFunc& swap = amc::CreateCurFunc();
    Ins(&R, swap.ret  , "void",false);
    Ins(&R, swap.proto, "$name_Swap($aryelemtype &elem_a, $aryelemtype &elem_b)",false);
    Ins(&R, swap.comment, "Swap values elem_a and elem_b");
    swap.inl = true;
    swap.priv = true;
    //swap.priv = true; // do not be static, there may be different ns
    if (is_ptr || cheap_copy) {
        Ins(&R, swap.body, "$Tgttype $*temp = elem_a;");
        Ins(&R, swap.body, "elem_a = elem_b;");
        Ins(&R, swap.body, "elem_b = temp;");
    } else {
        Ins(&R, swap.body, "u8 temp[sizeof($Tgttype)];");
        Ins(&R, swap.body, "memcpy(&temp  , &elem_a, sizeof($Tgttype));");
        Ins(&R, swap.body, "memcpy(&elem_a, &elem_b, sizeof($Tgttype));");
        Ins(&R, swap.body, "memcpy(&elem_b, &temp  , sizeof($Tgttype));");
    }
}

void amc::tfunc_Sort_Rotleft() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    bool                 is_ptr     = (field.reftype == dmmeta_Reftype_reftype_Ptrary);
    bool                 cheap_copy = amc::CheapCopyQ(field);
    // Rotleft
    amc::FFunc& rotleft = amc::CreateCurFunc();
    Ins(&R, rotleft.ret  , "void",false);
    Ins(&R, rotleft.proto, "$name_Rotleft($aryelemtype &elem_a, $aryelemtype &elem_b, $aryelemtype &elem_c)",false);
    Ins(&R, rotleft.comment, "Left circular shift of three-tuple");
    rotleft.inl = true;
    rotleft.priv = true;
    if (is_ptr || cheap_copy) {
        Ins(&R, rotleft.body, "$Tgttype $*temp = elem_a;");
        Ins(&R, rotleft.body, "elem_a = elem_b;");
        Ins(&R, rotleft.body, "elem_b = elem_c;");
        Ins(&R, rotleft.body, "elem_c = temp;");
    } else {
        Ins(&R, rotleft.body, "u8 temp[sizeof($Tgttype)];");
        Ins(&R, rotleft.body, "memcpy(&temp, &elem_a   , sizeof($Tgttype));");
        Ins(&R, rotleft.body, "memcpy(&elem_a   , &elem_b   , sizeof($Tgttype));");
        Ins(&R, rotleft.body, "memcpy(&elem_b   , &elem_c   , sizeof($Tgttype));");
        Ins(&R, rotleft.body, "memcpy(&elem_c   , &temp, sizeof($Tgttype));");
    }
}

void amc::tfunc_Sort_Lt() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort         &fsort      = *field.c_fsort;
    amc::FField         &sortfld    = *fsort.p_sortfld;
    amc::FCtype         &tgttype    = *fsort.p_field->p_arg;
    bool                 cheap_copy = amc::CheapCopyQ(field);
    Set(R, "$elem_a_val", FieldvalExpr(&tgttype, *fsort.p_sortfld, "elem_a"));
    Set(R, "$elem_b_val", FieldvalExpr(&tgttype, *fsort.p_sortfld, "elem_b"));

    amc::FFunc& gt = amc::CreateCurFunc();
    Ins(&R, gt.comment, "Compare values elem_a and elem_b");
    Ins(&R, gt.comment, "The comparison function must be anti-symmetric: if a>b, then !(b>a).");
    Ins(&R, gt.comment, "If not, mayhem results.");
    gt.priv = true;
    Ins(&R, gt.ret  , "bool",false);
    // dear gcc -- you are very clever... then why is passing "cheap" elements by value
    // faster than by-reference?

    if (cheap_copy) {
        Ins(&R, gt.proto, "$name_Lt($Tgttype elem_a, $Tgttype elem_b)",false);
    } else {
        Ins(&R, gt.proto, "$name_Lt($Tgttype &elem_a, $Tgttype &elem_b)",false);
    }
    Ins(&R, gt.body, "bool ret;");
    if (sortfld.c_fcmp) {
        Ins(&R, gt.body, "ret = $sortfld_Lt(elem_a, elem_b);");// field has custom compare func
    } else {
        Ins(&R, gt.body, "ret = $elem_a_val < $elem_b_val;");
    }
    Ins(&R, gt.body, "return ret;");
}

void amc::tfunc_Sort_SortedQ() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    // SortedQ
    amc::FFunc& sortedq = amc::CreateCurFunc();
    Ins(&R, sortedq.comment, "Verify whether array is sorted");
    Ins(&R, sortedq.ret  , "bool",false);
    Ins(&R, sortedq.proto, "$name_SortedQ($Parent)",false);
    Ins(&R, sortedq.body, "$aryelemtype *elems = $name_Getary($pararg).elems;");
    Ins(&R, sortedq.body, "int n = $name_N($pararg);");
    Ins(&R, sortedq.body, "for (int i = 1; i < n; i++) {");
    Ins(&R, sortedq.body, "    if ($name_Lt($*elems[i], $*elems[i-1])) {");
    Ins(&R, sortedq.body, "        return false;");
    Ins(&R, sortedq.body, "    }");
    Ins(&R, sortedq.body, "}");
    Ins(&R, sortedq.body, "return true;");
}

void amc::tfunc_Sort_IntInsertionSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 is_ptr     = (field.reftype == dmmeta_Reftype_reftype_Ptrary);
    bool                 cheap_copy = amc::CheapCopyQ(field);
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    bool                 need_ins   = (fsort.sorttype == dmmeta_Sorttype_sorttype_InsertionSort) || need_quick;
    // InsertionSort
    // Three types are provided:
    // 1) sorting pointers or by-value with cheap copy)
    // 2) sorting by-memcopy for contiguous array
    if (need_ins) {
        amc::FFunc& iisort = amc::CreateCurFunc();
        Ins(&R, iisort.comment , "Internal insertion sort");
        iisort.priv = true;
        Ins(&R, iisort.ret     , "void",false);
        Ins(&R, iisort.proto   , "$name_IntInsertionSort($aryelemtype *elems, int n)",false);
        Ins(&R, iisort.body    , "for (int i = 1; i < n; ++i) {");
        Ins(&R, iisort.body    , "    int j = i;");
        if (is_ptr || cheap_copy) {
            Ins(&R, iisort.body, "    $Tgttype $*tmp = elems[i];");
            Ins(&R, iisort.body, "    // shift elements up by one");
            Ins(&R, iisort.body, "    if ($name_Lt($*tmp, $*elems[j-1])) {");
            Ins(&R, iisort.body, "        do {");
            Ins(&R, iisort.body, "            elems[j] = elems[j-1];");
            Ins(&R, iisort.body, "            j--;");
            Ins(&R, iisort.body, "        } while (j>0 && $name_Lt($*tmp, $*elems[j-1]));");
            Ins(&R, iisort.body, "        elems[j] = tmp;");
            Ins(&R, iisort.body, "    }");
        } else {
            Ins(&R, iisort.body, "    // find the spot for ith element.");
            Ins(&R, iisort.body, "    while (j>0 && $name_Lt($*elems[i], $*elems[j-1])) {");
            Ins(&R, iisort.body, "        j--;");
            Ins(&R, iisort.body, "    }");
            Ins(&R, iisort.body, "    if (j<i) {");
            Ins(&R, iisort.body, "        u8 tmp[sizeof($Tgttype$*)];");
            Ins(&R, iisort.body, "        memcpy (tmp                       , &elems[i], sizeof($Tgttype$*)      );");
            Ins(&R, iisort.body, "        memmove(&elems[j+1], &elems[j], sizeof($Tgttype$*)*(i-j));");
            Ins(&R, iisort.body, "        memcpy (&elems[j]  , tmp                     , sizeof($Tgttype$*)      );");
            Ins(&R, iisort.body, "    }");
        }
        Ins(&R, iisort.body, "}");
    }
}

void amc::tfunc_Sort_IntHeapSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    bool                 need_heap  = (fsort.sorttype == dmmeta_Sorttype_sorttype_HeapSort) || need_quick;
    // IntHeapSort
    if (need_heap) {
        amc::FFunc& ihsort = amc::CreateCurFunc();
        Ins(&R, ihsort.comment, "Internal heap sort");
        ihsort.priv = true;
        Ins(&R, ihsort.ret  , "void",false);
        Ins(&R, ihsort.proto, "$name_IntHeapSort($aryelemtype *elems, int n)",false);
        Ins(&R, ihsort.body, "// construct max-heap.");
        Ins(&R, ihsort.body, "// k=current element");
        Ins(&R, ihsort.body, "// j=parent element");
        Ins(&R, ihsort.body, "for (int i = 1; i < n; i++) {");
        Ins(&R, ihsort.body, "    int k=i;");
        Ins(&R, ihsort.body, "    int j=(i-1)/2;");
        Ins(&R, ihsort.body, "    while ($name_Lt($*elems[j], $*elems[k])) {");
        Ins(&R, ihsort.body, "        $name_Swap(elems[k],elems[j]);");
        Ins(&R, ihsort.body, "        k=j;");
        Ins(&R, ihsort.body, "        j=(k-1)/2;");
        Ins(&R, ihsort.body, "    }");
        Ins(&R, ihsort.body, "}");
        Ins(&R, ihsort.body, "// remove elements from heap one-by-one,");
        Ins(&R, ihsort.body, "// deposit them in reverse order starting at the end of ARY.");
        Ins(&R, ihsort.body, "for (int i = n - 1; i>=0; i--) {");
        Ins(&R, ihsort.body, "    int k = 0;");
        Ins(&R, ihsort.body, "    int l = 1;");
        Ins(&R, ihsort.body, "    while (l<i) {");
        Ins(&R, ihsort.body, "        l += l<i-1 && $name_Lt($*elems[l], $*elems[l+1]);");
        Ins(&R, ihsort.body, "        if ($name_Lt($*elems[l], $*elems[i])) {");
        Ins(&R, ihsort.body, "            break;");
        Ins(&R, ihsort.body, "        }");
        Ins(&R, ihsort.body, "        $name_Swap(elems[k], elems[l]);");
        Ins(&R, ihsort.body, "        k = l;");
        Ins(&R, ihsort.body, "        l = k*2+1;");
        Ins(&R, ihsort.body, "    }");
        Ins(&R, ihsort.body, "    if (i != k) {");
        Ins(&R, ihsort.body, "        $name_Swap(elems[i],elems[k]);");
        Ins(&R, ihsort.body, "    }");
        Ins(&R, ihsort.body, "}");
    }
}

void amc::tfunc_Sort_IntQuickSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 is_ptr     = (field.reftype == dmmeta_Reftype_reftype_Ptrary);
    bool                 cheap_copy = amc::CheapCopyQ(field);
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    // IntQuickSort
    if (need_quick) {
        amc::FFunc& iqsort = amc::CreateCurFunc();
        iqsort.priv = true;
        Ins(&R, iqsort.comment, "Quick sort engine");
        Ins(&R, iqsort.ret  , "void",false);
        Ins(&R, iqsort.proto, "$name_IntQuickSort($aryelemtype *elems, int n, int depth)",false);
        Ins(&R, iqsort.body, "while (n>16) {");
        Ins(&R, iqsort.body, "    // detect degenerate case and revert to heap sort");
        Ins(&R, iqsort.body, "    if (depth==0) {");
        Ins(&R, iqsort.body, "        $name_IntHeapSort(elems,n);");
        Ins(&R, iqsort.body, "        return;");
        Ins(&R, iqsort.body, "    }");
        Ins(&R, iqsort.body, "    // elements to sort initially to determine pivot.");
        Ins(&R, iqsort.body, "    // choose pp=n/2 in case the input is already sorted.");
        Ins(&R, iqsort.body, "    int pi = 0;");
        Ins(&R, iqsort.body, "    int pp = n/2;");
        Ins(&R, iqsort.body, "    int pj = n-1;");
        Ins(&R, iqsort.body, "    // insertion sort for 1st, middle and last element");
        Ins(&R, iqsort.body, "    if ($name_Lt($*elems[pp], $*elems[pi])) {");
        Ins(&R, iqsort.body, "        $name_Swap(elems[pi], elems[pp]);");
        Ins(&R, iqsort.body, "    }");
        Ins(&R, iqsort.body, "    if ($name_Lt($*elems[pj], $*elems[pp])) {");
        Ins(&R, iqsort.body, "        if ($name_Lt($*elems[pj], $*elems[pi])) {");
        Ins(&R, iqsort.body, "            $name_Rotleft(elems[pi], elems[pj], elems[pp]);");
        Ins(&R, iqsort.body, "        } else {");
        Ins(&R, iqsort.body, "            $name_Swap(elems[pj], elems[pp]);");
        Ins(&R, iqsort.body, "        }");
        Ins(&R, iqsort.body, "    }");
        Ins(&R, iqsort.body, "    // deposit pivot near the end of the array and skip it.");
        Ins(&R, iqsort.body, "    $name_Swap(elems[--pj], elems[pp]);");
        if (is_ptr || cheap_copy) {
            Ins(&R, iqsort.body, "    // copy pivot into temporary variable");
            Ins(&R, iqsort.body, "    $Tgttype $*pivot = elems[pj];");
        } else {
            Ins(&R, iqsort.body, "    // reference to pivot");
            Ins(&R, iqsort.body, "    $Tgttype $*&pivot = elems[pj];");
        }
        Ins(&R, iqsort.body, "    for(;;){");
        Ins(&R, iqsort.body, "        while ($name_Lt($*elems[++pi], $*pivot)) {");
        Ins(&R, iqsort.body, "        }");
        Ins(&R, iqsort.body, "        while ($name_Lt($*pivot, $*elems[--pj])) {");
        Ins(&R, iqsort.body, "        }");
        Ins(&R, iqsort.body, "        if (pj <= pi) {");
        Ins(&R, iqsort.body, "            break;");
        Ins(&R, iqsort.body, "        }");
        Ins(&R, iqsort.body, "        $name_Swap(elems[pi],elems[pj]);");
        Ins(&R, iqsort.body, "    }");
        Ins(&R, iqsort.body, "    depth -= 1;");
        Ins(&R, iqsort.body, "    $name_IntQuickSort(elems, pi, depth);");
        Ins(&R, iqsort.body, "    elems += pi;");
        Ins(&R, iqsort.body, "    n -= pi;");
        Ins(&R, iqsort.body, "}");
        Ins(&R, iqsort.body, "// sort the remainder of this section");
        Ins(&R, iqsort.body, "$name_IntInsertionSort(elems,n);");
    }
}

void amc::tfunc_Sort_InsertionSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    bool                 need_ins   = (fsort.sorttype == dmmeta_Sorttype_sorttype_InsertionSort) || need_quick;
    // Insertion Sort
    if (need_ins) {
        amc::FFunc& isort = amc::CreateCurFunc();
        Ins(&R, isort.comment, "Insertion sort");
        Ins(&R, isort.ret    , "void",false);
        Ins(&R, isort.proto  , "$name_InsertionSort($Parent)",false);
        Ins(&R, isort.body   , "$aryelemtype *elems = $name_Getary($pararg).elems;");
        Ins(&R, isort.body   , "int n = $name_N($pararg);");
        Ins(&R, isort.body   , "$name_IntInsertionSort(elems, n);");
    }
}

void amc::tfunc_Sort_HeapSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    bool                 need_heap  = (fsort.sorttype == dmmeta_Sorttype_sorttype_HeapSort) || need_quick;
    // Heap Sort
    if (need_heap) {
        amc::FFunc& hsort = amc::CreateCurFunc();
        Ins(&R, hsort.comment, "Heap sort");
        Ins(&R, hsort.ret  , "void",false);
        Ins(&R, hsort.proto, "$name_HeapSort($Parent)",false);
        Ins(&R, hsort.body, "$aryelemtype *elems = $name_Getary($pararg).elems;");
        Ins(&R, hsort.body, "int n = $name_N($pararg);");
        Ins(&R, hsort.body, "$name_IntHeapSort(elems, n);");
    }
}

void amc::tfunc_Sort_QuickSort() {
    algo_lib::Replscope &R          = amc::_db.genfield.R;
    amc::FField         &field      = *amc::_db.genfield.p_field;
    amc::FFsort          &fsort       = *field.c_fsort;
    bool                 need_quick = (fsort.sorttype == dmmeta_Sorttype_sorttype_QuickSort);
    // Quick Sort
    if (need_quick) {
        amc::FFunc& qsort = amc::CreateCurFunc();
        Ins(&R, qsort.comment, "Quick sort");
        Ins(&R, qsort.ret  , "void",false);
        Ins(&R, qsort.proto, "$name_QuickSort($Parent)",false);
        Ins(&R, qsort.body, "// compute max recursion depth based on number of elements in the array");
        Ins(&R, qsort.body, "int max_depth = algo::CeilingLog2(u32($name_N($pararg) + 1)) + 3;");
        Ins(&R, qsort.body, "$aryelemtype *elems = $name_Getary($pararg).elems;");
        Ins(&R, qsort.body, "int n = $name_N($pararg);");
        Ins(&R, qsort.body, "$name_IntQuickSort(elems, n, max_depth);");
    }
}
