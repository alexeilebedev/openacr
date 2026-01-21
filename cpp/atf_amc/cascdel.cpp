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
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_ptrary);
    frep_(i,100) {
        NewCascdel(&x,atf_amc_FCascdel_type_ptrary);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(c_child_ptrary_N(x) == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// ptrary - chain
void atf_amc::amctest_CascdelPtraryChain() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_ptrary);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,atf_amc_FCascdel_type_ptrary);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(c_child_ptrary_N(x) == 1);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// thash
void atf_amc::amctest_CascdelThash() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_thash);
    frep_(i,100) {
        NewCascdel(&x,atf_amc_FCascdel_type_thash);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(ind_child_thash_N(x) == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// thash - chain
void atf_amc::amctest_CascdelThashChain() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_thash);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,atf_amc_FCascdel_type_thash);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(ind_child_thash_N(x) == 1);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// bheap
void atf_amc::amctest_CascdelBheap() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_bheap);
    frep_(i,100) {
        NewCascdel(&x,atf_amc_FCascdel_type_bheap);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(bh_child_bheap_N(x) == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// bheap - chain
void atf_amc::amctest_CascdelBheapChain() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_bheap);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,atf_amc_FCascdel_type_bheap);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(bh_child_bheap_N(x) == 1);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// zslist
void atf_amc::amctest_CascdelZslist() {
}

// zslist - chain
void atf_amc::amctest_CascdelZslistChain() {
}

// atf_amc::atf_amc::zdlist
void atf_amc::amctest_CascdelZdlist() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_zdlist);
    frep_(i,100) {
        NewCascdel(&x,atf_amc_FCascdel_type_zdlist);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(zd_childlist_N(x) == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}

// zdlist - chain
void atf_amc::amctest_CascdelZdlistChain() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_zdlist);
    atf_amc::FCascdel *y = &x;
    frep_(i,100) {
        y = &NewCascdel(y,atf_amc_FCascdel_type_zdlist);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(zd_childlist_N(x) == 1);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}


// bheap
void atf_amc::amctest_CascdelAtree() {
    // setup
    atf_amc::_db.cascdel_next_key=0;
    ary_RemoveAll(atf_amc::_db.cascdel_check);
    atf_amc::FCascdel &x = NewCascdel(NULL,atf_amc_FCascdel_type_atree);
    frep_(i,100) {
        NewCascdel(&x,atf_amc_FCascdel_type_atree);
    }
    vrfy_(ary_Sum1s(atf_amc::_db.cascdel_check)==atf_amc::_db.cascdel_next_key);
    vrfy_(x.tr_child_atree_n == 100);
    // examine
    cascdel_Delete(x);
    // verify
    vrfy_(!ary_Sum1s(atf_amc::_db.cascdel_check));
}
