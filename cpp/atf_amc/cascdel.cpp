// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/cascdel.cpp
//

#include "include/atf_amc.h"


// -----------------------------------------------------------------------------

// utility functions for TestCascdel%

static atf_amc::FCascdel &NewCascdel(atf_amc::FCascdel *parent, atf_amc_FCascdel_type_Enum type) {
    atf_amc::FCascdel &x = atf_amc::cascdel_Alloc();
    x.p_parent = parent ? parent : &x;
    x.key = atf_amc::_db.cascdel_next_key;
    x.type = type;
    vrfy(cascdel_XrefMaybe(x), algo_lib::_db.errtext);
    ary_AllocBit(atf_amc::_db.cascdel_check,atf_amc::_db.cascdel_next_key);
    ary_SetBit(atf_amc::_db.cascdel_check,atf_amc::_db.cascdel_next_key++);
    return x;
}

void atf_amc::key_Cleanup(atf_amc::FCascdel &parent) {
    vrfy_(ary_GetBit(atf_amc::_db.cascdel_check,parent.key));
    ary_qClearBit(atf_amc::_db.cascdel_check,parent.key);
    //prlog(parent.key);
}

// Child count of the root's Atree index; Atree generates no _N accessor.
static i32 CascdelAtreeN(const atf_amc::FCascdel &cascdel) {
    return cascdel.tr_child_atree_n;
}

// Fan-out cascdel shape shared by the per-reftype tests: 100 children of
// TYPE under one root, the root's child count read via CHILD_N. Deleting
// the root must cascade to every child, leaving no check bit set.
template <typename CountFn> static void TestCascdelFanout(atf_amc_FCascdel_type_Enum type, CountFn child_n) {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,type);
    frep_(i,100) {
        NewCascdel(&x,type);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(child_n(x) == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// Chain cascdel shape shared by the per-reftype tests: a 100-deep chain of
// TYPE nodes, each the sole child of the previous, the root's child count
// read via CHILD_N. Deleting the root must recurse down the whole chain,
// leaving no check bit set.
template <typename CountFn> static void TestCascdelChain(atf_amc_FCascdel_type_Enum type, CountFn child_n) {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,type);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,type);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(child_n(x) == 1);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// ptr
void atf_amc::amctest_CascdelPtr() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    // Ptr is a special case:
    // for root, root.parent == &root,
    // but Unref code doesn't know that, so it clears child_ptr in parent,
    // thus clearing it in root itself. So,
    // due to this self-join, child objects are not deleted.
    // this is not a bug in amc, because cascdel selfjoin is a design bug,
    // being used here for testing purposes only.
    // we avoid the problem by setting up an extra layer (root,x) and deleting x
    // first manually, then deleting root.
    atf_amc::FCascdel &root = NewCascdel(NULL,atf_amc_FCascdel_type_ptr);

    atf_amc::FCascdel &x = NewCascdel(&root,atf_amc_FCascdel_type_ptr);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,atf_amc_FCascdel_type_ptr);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(NULL != x.child_ptr);
    // examine
    cascdel_Delete(x);
    cascdel_Delete(root);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// ptrary
void atf_amc::amctest_CascdelPtrary() {
    TestCascdelFanout(atf_amc_FCascdel_type_ptrary, c_child_ptrary_N);
}

// ptrary - chain
void atf_amc::amctest_CascdelPtraryChain() {
    TestCascdelChain(atf_amc_FCascdel_type_ptrary, c_child_ptrary_N);
}

// ptrary - heaplike
void atf_amc::amctest_CascdelPtraryHeap() {
    TestCascdelFanout(atf_amc_FCascdel_type_heap, c_child_heap_N);
}

// ptrary - heaplike, chain: recursion through the delete-until-empty walk
void atf_amc::amctest_CascdelPtraryHeapChain() {
    TestCascdelChain(atf_amc_FCascdel_type_heap, c_child_heap_N);
}

// ptrary - heaplike, with a cascade edge between two members of the same
// array. Deleting c2 (visited first by Cascdel) cascade-deletes its sibling
// c1, whose removal from the array must observe the array's true state --
// not a count zeroed up front, which turns the unlink into an elems[-1]
// read and leaves the count at (u64)-1.
void atf_amc::amctest_CascdelPtraryHeapSibling() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_heap);
    atf_amc::FCascdel &c1 = NewCascdel(&x,atf_amc_FCascdel_type_heap);
    atf_amc::FCascdel &c2 = NewCascdel(&x,atf_amc_FCascdel_type_heap);
    c2.child_ptr = &c1; // deleting c2 cascade-deletes c1
    vrfy_(c_child_heap_N(x) == 2);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// thash
void atf_amc::amctest_CascdelThash() {
    TestCascdelFanout(atf_amc_FCascdel_type_thash, ind_child_thash_N);
}

// thash - chain
void atf_amc::amctest_CascdelThashChain() {
    TestCascdelChain(atf_amc_FCascdel_type_thash, ind_child_thash_N);
}

// bheap
void atf_amc::amctest_CascdelBheap() {
    TestCascdelFanout(atf_amc_FCascdel_type_bheap, bh_child_bheap_N);
}

// bheap - chain
void atf_amc::amctest_CascdelBheapChain() {
    TestCascdelChain(atf_amc_FCascdel_type_bheap, bh_child_bheap_N);
}

// zdlist
void atf_amc::amctest_CascdelZdlist() {
    TestCascdelFanout(atf_amc_FCascdel_type_zdlist, zd_childlist_N);
}

// zdlist - chain
void atf_amc::amctest_CascdelZdlistChain() {
    TestCascdelChain(atf_amc_FCascdel_type_zdlist, zd_childlist_N);
}

// atree
void atf_amc::amctest_CascdelAtree() {
    TestCascdelFanout(atf_amc_FCascdel_type_atree, CascdelAtreeN);
}

// atree - chain
void atf_amc::amctest_CascdelAtreeChain() {
    TestCascdelChain(atf_amc_FCascdel_type_atree, CascdelAtreeN);
}
