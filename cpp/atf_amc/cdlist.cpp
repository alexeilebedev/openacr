// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Exceptions: NO
// Source: cpp/atf_amc/cdlist.cpp
//

#include "include/atf_amc.h"

// =============================================================================
// CDLIST - TAIL INSERT

//
// create list item, check if it is not in list
//
void atf_amc::amctest_CdlistItemDfltCtor() {
    // setup
    // exercise
    atf_amc::FTypeC item;
    // verify
    vrfy_(false == atf_amc::cd_t_typec_InLlistQ(item));
    vrfy_((atf_amc::FTypeC*)-1 == atf_amc::cd_t_typec_Next(item));
    vrfy_(NULL == atf_amc::cd_t_typec_Prev(item));
    // teardown
}

//
// Create empty list, check if it is really empty
//
void atf_amc::amctest_CdlistDfltCtor() {
    // setup
    // exercise
    // verify
    vrfy_(NULL == atf_amc::cd_t_typec_First ());
    vrfy_(NULL == atf_amc::cd_t_typec_Last  ());
    vrfy_(0    == atf_amc::cd_t_typec_N     ());
    vrfy_(true == atf_amc::cd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
        (void)li;
        vrfy_(false); // should not happen
    }ind_end;
    // teardown
}

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_CdlistInsert1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::cd_t_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::cd_t_typec_InLlistQ(item));
    vrfy_(&item ==  atf_amc::cd_t_typec_Next    (item));
    vrfy_(&item ==  atf_amc::cd_t_typec_Prev    (item));
    vrfy_(&item ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(1     ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false ==  atf_amc::cd_t_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistInsert2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::cd_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Prev    (item[1]));

    vrfy_(&item[0] ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item[1] == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(2        ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::cd_t_typec_EmptyQ  ());

    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistInsert3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::cd_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Next    (item[0]));
    vrfy_(&item[2] ==  atf_amc::cd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[1]));
    vrfy_(&item[2] ==  atf_amc::cd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Prev    (item[1]));

    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[2]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Next    (item[2]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Prev    (item[2]));

    vrfy_(&item[0] ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item[2] ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item[2] == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(3        ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::cd_t_typec_EmptyQ  ());

    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistRemoveFirst() {
    // setup
    atf_amc::FTypeC item[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::cd_t_typec_Insert(item[i]);
    }
    // exercise/verify
    frep_(i,100) {
        // exercise
        atf_amc::FTypeC *first = atf_amc::cd_t_typec_RemoveFirst();
        // verify
        vrfy_(first == &item[i]);
        vrfy_(first->typec == 100+i);
        vrfy_(false == atf_amc::cd_t_typec_InLlistQ(*first));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::cd_t_typec_Next(*first));
        vrfy_(NULL == atf_amc::cd_t_typec_Prev(*first));
        if (i+1<100) {
            vrfy_(&item[i+1] ==  atf_amc::cd_t_typec_First   ());
            vrfy_(&item[99]  ==  atf_amc::cd_t_typec_Last    ());
            vrfy_(&item[99]  == &atf_amc::cd_t_typec_qLast   ());
            vrfy_(100-i-1    ==  atf_amc::cd_t_typec_N       ());
            vrfy_(false      ==  atf_amc::cd_t_typec_EmptyQ  ());
            int nb_iter = 0;
            atf_amc::FTypeC *prev = &item[99];
            ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
                vrfy_(&li == &item[nb_iter+i+1]);
                vrfy_(li.typec == 100+nb_iter+i+1);
                vrfy_(true == atf_amc::cd_t_typec_InLlistQ(li));
                vrfy_(prev == atf_amc::cd_t_typec_Prev(li));
                vrfy_((nb_iter+i+2<100?&item[nb_iter+i+2]:&item[i+1]) == atf_amc::cd_t_typec_Next(li));
                prev = &item[nb_iter+i+1];
                ++nb_iter;
            }ind_end;
            vrfy_(100-i-1==nb_iter);
        } else {
            vrfy_(NULL       ==  atf_amc::cd_t_typec_First   ());
            vrfy_(NULL       ==  atf_amc::cd_t_typec_Last    ());
            vrfy_(0          ==  atf_amc::cd_t_typec_N       ());
            vrfy_(true       ==  atf_amc::cd_t_typec_EmptyQ  ());
        }
    }
    // on empty list
    vrfy_(NULL       ==  atf_amc::cd_t_typec_RemoveFirst());
    vrfy_(NULL       ==  atf_amc::cd_t_typec_First   ());
    vrfy_(NULL       ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(0          ==  atf_amc::cd_t_typec_N       ());
    vrfy_(true       ==  atf_amc::cd_t_typec_EmptyQ  ());

    // teardown
}

//
// Insert 100 elements, Remove them in "random" order
//
void atf_amc::amctest_CdlistRemove() {
    // setup
    atf_amc::FTypeC item[100];
    bool removed[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::cd_t_typec_Insert(item[i]);
        removed[i] = false;
    }
    // exercise/verify
    frep_(i,100) {
        i32 ind = (i*73) % 100;
        removed[ind] = true;
        // exercise
        atf_amc::cd_t_typec_Remove(item[ind]);
        // verify
        vrfy_(false == atf_amc::cd_t_typec_InLlistQ(item[ind]));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::cd_t_typec_Next(item[ind]));
        atf_amc::FTypeC *first = NULL;
        atf_amc::FTypeC *last  = NULL;
        int nb_typec = 0;
        frep_(j,100) {
            if (!removed[j]) {
                last = &item[j];
            }
        }
        atf_amc::FTypeC *prev  = last;
        frep_(j,100) {
            vrfy_(!removed[j] == atf_amc::cd_t_typec_InLlistQ(item[j]));
            if (!removed[j]) {
                ++nb_typec;
                if (!first) first = &item[j];
                last = &item[j];
                if (prev) vrfy_(&item[j] ==  atf_amc::cd_t_typec_Next(*prev));
                vrfy_(prev == atf_amc::cd_t_typec_Prev(item[j]));
                prev = &item[j];
            } else {
                vrfy_((atf_amc::FTypeC*)-1 ==  atf_amc::cd_t_typec_Next(item[j]));
            }
        }
        if (first) vrfy_(last  == atf_amc::cd_t_typec_Prev    (*first));
        if (last)  vrfy_(first == atf_amc::cd_t_typec_Next    (*last));
        vrfy_(first            == atf_amc::cd_t_typec_First   ());
        vrfy_(last             == atf_amc::cd_t_typec_Last    ());
        vrfy_(nb_typec         == atf_amc::cd_t_typec_N       ());
        vrfy_(!nb_typec        == atf_amc::cd_t_typec_EmptyQ  ());
    }
    frep_(i,100) {
        vrfy_(item[i].typec == 100+i); // check if nothing has corrupted
    }
    // teardown
}

//
// Flush empty list
//
void atf_amc::amctest_CdlistFlushEmpty() {
    // exercise
    atf_amc::cd_t_typec_RemoveAll();
    // verify
    vrfy_(NULL == atf_amc::cd_t_typec_First ());
    vrfy_(NULL == atf_amc::cd_t_typec_Last  ());
    vrfy_(0    == atf_amc::cd_t_typec_N     ());
    vrfy_(true == atf_amc::cd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
        (void)li;
        vrfy_(false); // should not happen
    }ind_end;
    // teardown
}

//
// Flush 100 elements
//
void atf_amc::amctest_CdlistFlush100() {
    // setup
    atf_amc::FTypeC item[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::cd_t_typec_Insert(item[i]);
    }
    // exercise
    atf_amc::cd_t_typec_RemoveAll();
    // verify
    frep_(i,100) {
        vrfy_(item[i].typec == 100+i); // check if nothing has corrupted
        vrfy_(false == atf_amc::cd_t_typec_InLlistQ(item[i]));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::cd_t_typec_Next(item[i]));
        vrfy_(NULL == atf_amc::cd_t_typec_Prev(item[i]));
    }
    vrfy_(NULL == atf_amc::cd_t_typec_First ());
    vrfy_(NULL == atf_amc::cd_t_typec_Last  ());
    vrfy_(0    == atf_amc::cd_t_typec_N     ());
    vrfy_(true == atf_amc::cd_t_typec_EmptyQ());

    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistInsertMaybe() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    atf_amc::cd_t_typec_Insert(item[0]);
    // verify
    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Next    (item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Prev    (item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(1        ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::cd_t_typec_EmptyQ  ());
    i32 nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[0]);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // exercise
    atf_amc::cd_t_typec_Insert(item[0]);
    // verify
    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Next    (item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Prev    (item[0]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(1        ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::cd_t_typec_EmptyQ  ());
    nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[0]);
        vrfy_(li.typec == 100);
        ++nb_iter;
    }ind_end;
    vrfy_(1==nb_iter);
    // exercise
    atf_amc::cd_t_typec_Insert(item[1]);
    // verify
    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::cd_t_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::cd_t_typec_Prev    (item[1]));

    vrfy_(&item[0] ==  atf_amc::cd_t_typec_First   ());
    vrfy_(&item[1] ==  atf_amc::cd_t_typec_Last    ());
    vrfy_(&item[1] == &atf_amc::cd_t_typec_qLast   ());
    vrfy_(2        ==  atf_amc::cd_t_typec_N       ());
    vrfy_(false    ==  atf_amc::cd_t_typec_EmptyQ  ());

    nb_iter = 0;
    ind_beg(atf_amc::_db_cd_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[nb_iter]);
        vrfy_(li.typec == 100+nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(2==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// CDLIST - HEAD INSERT

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_CdlistInsertHead1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::cdl_h_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::cdl_h_typec_InLlistQ(item));
    vrfy_(&item ==  atf_amc::cdl_h_typec_Next    (item));
    vrfy_(&item ==  atf_amc::cdl_h_typec_Prev    (item));
    vrfy_(&item ==  atf_amc::cdl_h_typec_First   ());
    vrfy_(&item ==  atf_amc::cdl_h_typec_Last    ());
    vrfy_(&item == &atf_amc::cdl_h_typec_qLast   ());
    vrfy_(1     ==  atf_amc::cdl_h_typec_N       ());
    vrfy_(false ==  atf_amc::cdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_cdl_h_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistInsertHead2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::cdl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::cdl_h_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::cdl_h_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::cdl_h_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::cdl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::cdl_h_typec_Next    (item[1]));
    vrfy_(&item[0] ==  atf_amc::cdl_h_typec_Prev    (item[1]));

    vrfy_(&item[1] ==  atf_amc::cdl_h_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::cdl_h_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::cdl_h_typec_qLast   ());
    vrfy_(2        ==  atf_amc::cdl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::cdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_cdl_h_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_CdlistInsertHead3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::cdl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::cdl_h_typec_InLlistQ(item[0]));
    vrfy_(&item[2] ==  atf_amc::cdl_h_typec_Next    (item[0]));
    vrfy_(&item[1] ==  atf_amc::cdl_h_typec_Prev    (item[0]));

    vrfy_(true     ==  atf_amc::cdl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::cdl_h_typec_Next    (item[1]));
    vrfy_(&item[2] ==  atf_amc::cdl_h_typec_Prev    (item[1]));

    vrfy_(true     ==  atf_amc::cdl_h_typec_InLlistQ(item[2]));
    vrfy_(&item[1] ==  atf_amc::cdl_h_typec_Next    (item[2]));
    vrfy_(&item[0] ==  atf_amc::cdl_h_typec_Prev    (item[2]));

    vrfy_(&item[2] ==  atf_amc::cdl_h_typec_First   ());
    vrfy_(3        ==  atf_amc::cdl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::cdl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_cdl_h_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[2-nb_iter]);
        vrfy_(li.typec == 102-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// CDLIST - ROTATE FIRST

void atf_amc::amctest_CdlistRotateFirst() {
    // setup
    vrfy_(true == atf_amc::cd_t_typec_EmptyQ())
        atf_amc::FTypeC item[3];
    // exercise/verify
    vrfy_(NULL == atf_amc::cd_t_typec_RotateFirst());
    // setup
    atf_amc::cd_t_typec_Insert(item[0]);
    // exercise/verify
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    // setup
    atf_amc::cd_t_typec_Insert(item[1]);
    // exercise/verify
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[1] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[1] == atf_amc::cd_t_typec_RotateFirst());
    // setup
    atf_amc::cd_t_typec_Insert(item[2]);
    // exercise/verify
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[1] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[2] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[0] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[1] == atf_amc::cd_t_typec_RotateFirst());
    vrfy_(&item[2] == atf_amc::cd_t_typec_RotateFirst());
}
