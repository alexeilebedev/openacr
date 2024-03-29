// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
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
// Exceptions: NO
// Source: cpp/atf_amc/zdlist.cpp
//

#include "include/atf_amc.h"

// =============================================================================
// ZDLIST - TAIL INSERT

//
// create list item, check if it is not in list
//
void atf_amc::amctest_ZdlistItemDfltCtor() {
    // setup
    // exercise
    atf_amc::FTypeC item;
    // verify
    vrfy_(false == atf_amc::zd_t_typec_InLlistQ(item));
    vrfy_((atf_amc::FTypeC*)-1 == atf_amc::zd_t_typec_Next(item));
    vrfy_(NULL == atf_amc::zd_t_typec_Prev(item));
    // teardown
}

//
// Create empty list, check if it is really empty
//
void atf_amc::amctest_ZdlistDfltCtor() {
    // setup
    // exercise
    // verify
    vrfy_(NULL == atf_amc::zd_t_typec_First ());
    vrfy_(NULL == atf_amc::zd_t_typec_Last  ());
    vrfy_(0    == atf_amc::zd_t_typec_N     ());
    vrfy_(true == atf_amc::zd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        (void)li;
        vrfy_(false); // should not happen
    }ind_end;
    // teardown
}

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_ZdlistInsert1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::zd_t_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::zd_t_typec_InLlistQ(item));
    vrfy_(NULL  ==  atf_amc::zd_t_typec_Next    (item));
    vrfy_(NULL  ==  atf_amc::zd_t_typec_Prev    (item));
    vrfy_(&item ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(1     ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false ==  atf_amc::zd_t_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // teardown
}

//
// Insert 2 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsert2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::zd_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Next    (item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[1]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_Prev    (item[1]));

    vrfy_(&item[0] ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item[1] == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(2        ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zd_t_typec_EmptyQ  ());

    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[nb_iter]);
        vrfy_(li.typec == 100+nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(2==nb_iter);
    // teardown
}

//
// Insert 3 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsert3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::zd_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Next    (item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[1]));
    vrfy_(&item[2] ==  atf_amc::zd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_Prev    (item[1]));

    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[2]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Next    (item[2]));
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Prev    (item[2]));

    vrfy_(&item[0] ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item[2] ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item[2] == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(3        ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zd_t_typec_EmptyQ  ());

    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[nb_iter]);
        vrfy_(li.typec == 100+nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

//
// Insert 100 items to the list, remove first item 100 times
// Then try on empty list
//
void atf_amc::amctest_ZdlistRemoveFirst() {
    // setup
    atf_amc::FTypeC item[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::zd_t_typec_Insert(item[i]);
    }
    // exercise/verify
    frep_(i,100) {
        // exercise
        atf_amc::FTypeC *first = atf_amc::zd_t_typec_RemoveFirst();
        // verify
        vrfy_(first == &item[i]);
        vrfy_(first->typec == 100+i);
        vrfy_(false == atf_amc::zd_t_typec_InLlistQ(*first));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::zd_t_typec_Next(*first));
        vrfy_(NULL == atf_amc::zd_t_typec_Prev(*first));
        if (i+1<100) {
            vrfy_(&item[i+1] ==  atf_amc::zd_t_typec_First   ());
            vrfy_(&item[99]  ==  atf_amc::zd_t_typec_Last    ());
            vrfy_(&item[99]  == &atf_amc::zd_t_typec_qLast   ());
            vrfy_(100-i-1    ==  atf_amc::zd_t_typec_N       ());
            vrfy_(false      ==  atf_amc::zd_t_typec_EmptyQ  ());
            int nb_iter = 0;
            atf_amc::FTypeC *prev = NULL;
            ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
                vrfy_(&li == &item[nb_iter+i+1]);
                vrfy_(li.typec == 100+nb_iter+i+1);
                vrfy_(true == atf_amc::zd_t_typec_InLlistQ(li));
                vrfy_(prev == atf_amc::zd_t_typec_Prev(li));
                vrfy_((nb_iter+i+2<100?&item[nb_iter+i+2]:NULL) == atf_amc::zd_t_typec_Next(li));
                prev = &item[nb_iter+i+1];
                ++nb_iter;
            }ind_end;
            vrfy_(100-i-1==nb_iter);
        } else {
            vrfy_(NULL       ==  atf_amc::zd_t_typec_First   ());
            vrfy_(NULL       ==  atf_amc::zd_t_typec_Last    ());
            vrfy_(0          ==  atf_amc::zd_t_typec_N       ());
            vrfy_(true       ==  atf_amc::zd_t_typec_EmptyQ  ());
        }
    }
    // on empty list
    vrfy_(NULL       ==  atf_amc::zd_t_typec_RemoveFirst());
    vrfy_(NULL       ==  atf_amc::zd_t_typec_First   ());
    vrfy_(NULL       ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(0          ==  atf_amc::zd_t_typec_N       ());
    vrfy_(true       ==  atf_amc::zd_t_typec_EmptyQ  ());

    // teardown
}

//
// Insert 100 elements, Remove them in "random" order
//
void atf_amc::amctest_ZdlistRemove() {
    // setup
    atf_amc::FTypeC item[100];
    bool removed[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::zd_t_typec_Insert(item[i]);
        removed[i] = false;
    }
    // exercise/verify
    frep_(i,100) {
        i32 ind = (i*73) % 100;
        removed[ind] = true;
        // exercise
        atf_amc::zd_t_typec_Remove(item[ind]);
        // verify
        vrfy_(false == atf_amc::zd_t_typec_InLlistQ(item[ind]));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::zd_t_typec_Next(item[ind]));
        atf_amc::FTypeC *first = NULL;
        atf_amc::FTypeC *last  = NULL;
        atf_amc::FTypeC *prev  = NULL;
        int nb_typec = 0;
        frep_(j,100) {
            vrfy_(!removed[j] == atf_amc::zd_t_typec_InLlistQ(item[j]));
            if (!removed[j]) {
                ++nb_typec;
                if (!first) first = &item[j];
                last = &item[j];
                if (prev) vrfy_(&item[j] ==  atf_amc::zd_t_typec_Next(*prev));
                vrfy_(prev == atf_amc::zd_t_typec_Prev(item[j]));
                prev = &item[j];
            } else {
                vrfy_((atf_amc::FTypeC*)-1 ==  atf_amc::zd_t_typec_Next(item[j]));
            }
        }
        if (first) vrfy_(NULL == atf_amc::zd_t_typec_Prev    (*first));
        if (last)  vrfy_(NULL == atf_amc::zd_t_typec_Next    (*last));
        vrfy_(first           == atf_amc::zd_t_typec_First   ());
        vrfy_(last            == atf_amc::zd_t_typec_Last    ());
        vrfy_(nb_typec         == atf_amc::zd_t_typec_N       ());
        vrfy_(!nb_typec        == atf_amc::zd_t_typec_EmptyQ  ());
    }
    frep_(i,100) {
        vrfy_(item[i].typec == 100+i); // check if nothing has corrupted
    }
    // teardown
}

//
// Flush empty list
//
void atf_amc::amctest_ZdlistFlushEmpty() {
    // exercise
    atf_amc::zd_t_typec_RemoveAll();
    // verify
    vrfy_(NULL == atf_amc::zd_t_typec_First ());
    vrfy_(NULL == atf_amc::zd_t_typec_Last  ());
    vrfy_(0    == atf_amc::zd_t_typec_N     ());
    vrfy_(true == atf_amc::zd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        (void)li;
        vrfy_(false); // should not happen
    }ind_end;
    // teardown
}

//
// Flush 100 elements
//
void atf_amc::amctest_ZdlistFlush100() {
    // setup
    atf_amc::FTypeC item[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::zd_t_typec_Insert(item[i]);
    }
    // exercise
    atf_amc::zd_t_typec_RemoveAll();
    // verify
    frep_(i,100) {
        vrfy_(item[i].typec == 100+i); // check if nothing has corrupted
        vrfy_(false == atf_amc::zd_t_typec_InLlistQ(item[i]));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::zd_t_typec_Next(item[i]));
        vrfy_(NULL == atf_amc::zd_t_typec_Prev(item[i]));
    }
    vrfy_(NULL == atf_amc::zd_t_typec_First ());
    vrfy_(NULL == atf_amc::zd_t_typec_Last  ());
    vrfy_(0    == atf_amc::zd_t_typec_N     ());
    vrfy_(true == atf_amc::zd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        (void)li;
        vrfy_(false); // should not happen
    }ind_end;
    // teardown
}

//
// InsertMaybe:
// 1) try insert 1 element, check if inserted
// 2) try insert the same element, check if not inserted
// 3) try insert other element, check if inserted
//
void atf_amc::amctest_ZdlistInsertMaybe() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    atf_amc::zd_t_typec_Insert(item[0]);
    // verify
    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Next    (item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Prev    (item[0]));
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(1        ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zd_t_typec_EmptyQ  ());
    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[0]);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // exercise
    atf_amc::zd_t_typec_Insert(item[0]);
    // verify
    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Next    (item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Prev    (item[0]));
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(1        ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zd_t_typec_EmptyQ  ());
    nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[0]);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // exercise
    atf_amc::zd_t_typec_Insert(item[1]);
    // verify
    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Next    (item[0]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zd_t_typec_InLlistQ(item[1]));
    vrfy_(NULL     ==  atf_amc::zd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::zd_t_typec_Prev    (item[1]));

    vrfy_(&item[0] ==  atf_amc::zd_t_typec_First   ());
    vrfy_(&item[1] ==  atf_amc::zd_t_typec_Last    ());
    vrfy_(&item[1] == &atf_amc::zd_t_typec_qLast   ());
    vrfy_(2        ==  atf_amc::zd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zd_t_typec_EmptyQ  ());

    nb_iter = 0;
    ind_beg(atf_amc::_db_zd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[nb_iter]);
        vrfy_(li.typec == 100+nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(2==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// ZDLIST - HEAD INSERT

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_ZdlistInsertHead1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::zdl_h_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::zdl_h_typec_InLlistQ(item));
    vrfy_(NULL  ==  atf_amc::zdl_h_typec_Next    (item));
    vrfy_(NULL  ==  atf_amc::zdl_h_typec_Prev    (item));
    vrfy_(&item ==  atf_amc::zdl_h_typec_First   ());
    vrfy_(&item ==  atf_amc::zdl_h_typec_Last    ());
    vrfy_(&item == &atf_amc::zdl_h_typec_qLast   ());
    vrfy_(1     ==  atf_amc::zdl_h_typec_N       ());
    vrfy_(false ==  atf_amc::zdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_h_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // teardown
}

//
// Insert 2 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsertHead2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::zdl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zdl_h_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zdl_h_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::zdl_h_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zdl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zdl_h_typec_Next    (item[1]));
    vrfy_(NULL     ==  atf_amc::zdl_h_typec_Prev    (item[1]));

    vrfy_(&item[1] ==  atf_amc::zdl_h_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::zdl_h_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::zdl_h_typec_qLast   ());
    vrfy_(2        ==  atf_amc::zdl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::zdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_h_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[1-nb_iter]);
        vrfy_(li.typec == 101-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(2==nb_iter);
    // teardown
}

//
// Insert 3 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsertHead3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::zdl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zdl_h_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zdl_h_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::zdl_h_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zdl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zdl_h_typec_Next    (item[1]));
    vrfy_(&item[2] ==  atf_amc::zdl_h_typec_Prev    (item[1]));

    vrfy_(true     ==  atf_amc::zdl_h_typec_InLlistQ(item[2]));
    vrfy_(&item[1] ==  atf_amc::zdl_h_typec_Next    (item[2]));
    vrfy_(NULL     ==  atf_amc::zdl_h_typec_Prev    (item[2]));

    vrfy_(&item[2] ==  atf_amc::zdl_h_typec_First   ());
    vrfy_(3        ==  atf_amc::zdl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::zdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_h_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[2-nb_iter]);
        vrfy_(li.typec == 102-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// ZDLIST - HEAD INSERT - NO TAIL

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_ZdlistInsertHeadNoTail1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::zdl_hnt_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::zdl_hnt_typec_InLlistQ(item));
    vrfy_(NULL  ==  atf_amc::zdl_hnt_typec_Next    (item));
    vrfy_(NULL  ==  atf_amc::zdl_hnt_typec_Prev    (item));
    vrfy_(&item ==  atf_amc::zdl_hnt_typec_First   ());
    vrfy_(1     ==  atf_amc::zdl_hnt_typec_N       ());
    vrfy_(false ==  atf_amc::zdl_hnt_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_hnt_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // teardown
}

//
// Insert 2 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsertHeadNoTail2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::zdl_hnt_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zdl_hnt_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zdl_hnt_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::zdl_hnt_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zdl_hnt_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zdl_hnt_typec_Next    (item[1]));
    vrfy_(NULL     ==  atf_amc::zdl_hnt_typec_Prev    (item[1]));

    vrfy_(&item[1] ==  atf_amc::zdl_hnt_typec_First   ());
    vrfy_(2        ==  atf_amc::zdl_hnt_typec_N       ());
    vrfy_(false    ==  atf_amc::zdl_hnt_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_hnt_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[1-nb_iter]);
        vrfy_(li.typec == 101-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(2==nb_iter);
    // teardown
}

//
// Insert 3 elements in the list, check if it they are really in the list
//
void atf_amc::amctest_ZdlistInsertHeadNoTail3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::zdl_hnt_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zdl_hnt_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zdl_hnt_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::zdl_hnt_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::zdl_hnt_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zdl_hnt_typec_Next    (item[1]));
    vrfy_(&item[2] ==  atf_amc::zdl_hnt_typec_Prev    (item[1]));

    vrfy_(true     ==  atf_amc::zdl_hnt_typec_InLlistQ(item[2]));
    vrfy_(&item[1] ==  atf_amc::zdl_hnt_typec_Next    (item[2]));
    vrfy_(NULL     ==  atf_amc::zdl_hnt_typec_Prev    (item[2]));

    vrfy_(&item[2] ==  atf_amc::zdl_hnt_typec_First   ());
    vrfy_(3        ==  atf_amc::zdl_hnt_typec_N       ());
    vrfy_(false    ==  atf_amc::zdl_hnt_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zdl_hnt_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[2-nb_iter]);
        vrfy_(li.typec == 102-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ZdlistDelCurs() {
    for (int listtype=0; listtype<3; listtype++) {
        for (int nelem=1; nelem<4; nelem++) {
            for (int delwhich=0; delwhich<nelem; delwhich++) {
                prlog("ZdlistDelCurs"
                      <<Keyval("listtype",listtype)
                      <<Keyval("nelem",nelem)
                      <<Keyval("delwhich",delwhich));
                atf_amc::FTypeD *t[4];
                for (int i=0; i<nelem; i++) {
                    t[i] = &typed_Alloc();
                    typed_XrefMaybe(*t[i]);
                }

                vrfy_(zd_typed_N()==nelem);
                vrfy_(cd_typed_N()==nelem);
                vrfy_(zs_t_typed_N()==nelem);

                int n=0;
                if (listtype == 0) {
                    ind_beg(_db_zd_typed_delcurs,typed,_db) {
                        if (n==delwhich) {
                            typed_Delete(typed);
                        }
                        n++;
                    }ind_end;
                } else if (listtype == 1) {
                    ind_beg(_db_cd_typed_delcurs,typed,_db) {
                        if (n==delwhich) {
                            typed_Delete(typed);
                        }
                        n++;
                    }ind_end;
                } else if (listtype == 2) {
                    ind_beg(_db_zs_t_typed_delcurs,typed,_db) {
                        if (n==delwhich) {
                            typed_Delete(typed);
                        }
                        n++;
                    }ind_end;
                }

                t[delwhich]=NULL;
                vrfy_(zd_typed_N()==nelem-1);
                vrfy_(cd_typed_N()==nelem-1);
                vrfy_(zs_t_typed_N()==nelem-1);

                for (int i=0; i<nelem; i++) {
                    if (i!=delwhich) {
                        vrfy_(zd_typed_InLlistQ(*t[i]));
                        vrfy_(zs_t_typed_InLlistQ(*t[i]));
                        vrfy_(cd_typed_InLlistQ(*t[i]));
                    }
                }
                for (int i=0; i<nelem; i++) {
                    if (i!=delwhich) {
                        typed_Delete(*t[i]);
                    }
                }
            }
        }
    }
}
