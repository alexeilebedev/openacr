// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/zslist.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------
// ZSLIST - HEAD INSERT

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_ZslistInsertHead1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::zsl_h_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::zsl_h_typec_InLlistQ(item));
    vrfy_(NULL  ==  atf_amc::zsl_h_typec_Next    (item));
    vrfy_(&item ==  atf_amc::zsl_h_typec_First   ());
    vrfy_(&item ==  atf_amc::zsl_h_typec_Last    ());
    vrfy_(&item == &atf_amc::zsl_h_typec_qLast   ());
    vrfy_(1     ==  atf_amc::zsl_h_typec_N       ());
    vrfy_(false ==  atf_amc::zsl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zsl_h_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_ZslistInsertHead2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::zsl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zsl_h_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zsl_h_typec_Next    (item[0]));

    vrfy_(true     ==  atf_amc::zsl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zsl_h_typec_Next    (item[1]));

    vrfy_(&item[1] ==  atf_amc::zsl_h_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::zsl_h_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::zsl_h_typec_qLast   ());
    vrfy_(2        ==  atf_amc::zsl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::zsl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zsl_h_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_ZslistInsertHead3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::zsl_h_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zsl_h_typec_InLlistQ(item[0]));
    vrfy_(NULL     ==  atf_amc::zsl_h_typec_Next    (item[0]));

    vrfy_(true     ==  atf_amc::zsl_h_typec_InLlistQ(item[1]));
    vrfy_(&item[0] ==  atf_amc::zsl_h_typec_Next    (item[1]));

    vrfy_(true     ==  atf_amc::zsl_h_typec_InLlistQ(item[2]));
    vrfy_(&item[1] ==  atf_amc::zsl_h_typec_Next    (item[2]));

    vrfy_(&item[2] ==  atf_amc::zsl_h_typec_First   ());
    vrfy_(&item[0] ==  atf_amc::zsl_h_typec_Last    ());
    vrfy_(&item[0] == &atf_amc::zsl_h_typec_qLast   ());
    vrfy_(3        ==  atf_amc::zsl_h_typec_N       ());
    vrfy_(false    ==  atf_amc::zsl_h_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zsl_h_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[2-nb_iter]);
        vrfy_(li.typec == 102-nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// ZSLIST - TAIL INSERT

//
// Insert 1 element in the list, check if it is really in the list
//
void atf_amc::amctest_ZslistInsert1() {
    // setup
    atf_amc::FTypeC item;
    item.typec = 100;
    // exercise
    atf_amc::zs_t_typec_Insert(item);
    // verify
    vrfy_(true  ==  atf_amc::zs_t_typec_InLlistQ(item));
    vrfy_(NULL  ==  atf_amc::zs_t_typec_Next    (item));
    vrfy_(&item ==  atf_amc::zs_t_typec_First   ());
    vrfy_(&item ==  atf_amc::zs_t_typec_Last    ());
    vrfy_(&item == &atf_amc::zs_t_typec_qLast   ());
    vrfy_(1     ==  atf_amc::zs_t_typec_N       ());
    vrfy_(false ==  atf_amc::zs_t_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zs_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_ZslistInsert2() {
    // setup
    atf_amc::FTypeC item[2];
    frep_(i,2) item[i].typec = 100+i;
    // exercise
    frep_(i,2) { atf_amc::zs_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zs_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1] ==  atf_amc::zs_t_typec_Next    (item[0]));

    vrfy_(true     ==  atf_amc::zs_t_typec_InLlistQ(item[1]));
    vrfy_(NULL     ==  atf_amc::zs_t_typec_Next    (item[1]));

    vrfy_(&item[0] ==  atf_amc::zs_t_typec_First   ());
    vrfy_(&item[1] ==  atf_amc::zs_t_typec_Last    ());
    vrfy_(&item[1] == &atf_amc::zs_t_typec_qLast   ());
    vrfy_(2        ==  atf_amc::zs_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zs_t_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zs_t_typec_curs,li,atf_amc::_db) {
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
void atf_amc::amctest_ZslistInsert3() {
    // setup
    atf_amc::FTypeC item[3];
    frep_(i,3) item[i].typec = 100+i;
    // exercise
    frep_(i,3) { atf_amc::zs_t_typec_Insert(item[i]); }
    // verify
    vrfy_(true     ==  atf_amc::zs_t_typec_InLlistQ(item[0]));
    vrfy_(&item[1]  ==  atf_amc::zs_t_typec_Next    (item[0]));

    vrfy_(true     ==  atf_amc::zs_t_typec_InLlistQ(item[1]));
    vrfy_(&item[2] ==  atf_amc::zs_t_typec_Next    (item[1]));

    vrfy_(true     ==  atf_amc::zs_t_typec_InLlistQ(item[2]));
    vrfy_(NULL     ==  atf_amc::zs_t_typec_Next    (item[2]));

    vrfy_(&item[0] ==  atf_amc::zs_t_typec_First   ());
    vrfy_(&item[2] ==  atf_amc::zs_t_typec_Last    ());
    vrfy_(&item[2] == &atf_amc::zs_t_typec_qLast   ());
    vrfy_(3        ==  atf_amc::zs_t_typec_N       ());
    vrfy_(false    ==  atf_amc::zs_t_typec_EmptyQ  ());

    int nb_iter = 0;
    ind_beg(atf_amc::_db_zs_t_typec_curs,li,atf_amc::_db) {
        vrfy_(&li == &item[nb_iter]);
        vrfy_(li.typec == 100+nb_iter);
        ++nb_iter;
    }ind_end;
    vrfy_(3==nb_iter);
    // teardown
}

// -----------------------------------------------------------------------------
// ZSLIST

//
// Insert 100 items to the list, remove first item 100 times
// Then try on empty list
//
void atf_amc::amctest_ZslistRemoveFirst() {
    // setup
    atf_amc::FTypeC item[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::zs_t_typec_Insert(item[i]);
    }
    // exercise/verify
    frep_(i,100) {
        // exercise
        atf_amc::FTypeC *first = atf_amc::zs_t_typec_RemoveFirst();
        // verify
        vrfy_(first == &item[i]);
        vrfy_(first->typec == 100+i);
        vrfy_(false == atf_amc::zs_t_typec_InLlistQ(*first));
        vrfy_((atf_amc::FTypeC*)-1 == atf_amc::zs_t_typec_Next(*first));
        if (i+1<100) {
            vrfy_(&item[i+1] ==  atf_amc::zs_t_typec_First   ());
            vrfy_(&item[99]  ==  atf_amc::zs_t_typec_Last    ());
            vrfy_(&item[99]  == &atf_amc::zs_t_typec_qLast   ());
            vrfy_(100-i-1    ==  atf_amc::zs_t_typec_N       ());
            vrfy_(false      ==  atf_amc::zs_t_typec_EmptyQ  ());
            int nb_iter = 0;
            ind_beg(atf_amc::_db_zs_t_typec_curs,li,atf_amc::_db) {
                vrfy_(&li == &item[nb_iter+i+1]);
                vrfy_(li.typec == 100+nb_iter+i+1);
                ++nb_iter;
            }ind_end;
            vrfy_(100-i-1==nb_iter);
        } else {
            vrfy_(NULL       ==  atf_amc::zs_t_typec_First   ());
            vrfy_(NULL       ==  atf_amc::zs_t_typec_Last    ());
            vrfy_(0          ==  atf_amc::zs_t_typec_N       ());
            vrfy_(true       ==  atf_amc::zs_t_typec_EmptyQ  ());
        }
    }
    // on empty list
    vrfy_(NULL       ==  atf_amc::zs_t_typec_RemoveFirst());
    vrfy_(NULL       ==  atf_amc::zs_t_typec_First   ());
    vrfy_(NULL       ==  atf_amc::zs_t_typec_Last    ());
    vrfy_(0          ==  atf_amc::zs_t_typec_N       ());
    vrfy_(true       ==  atf_amc::zs_t_typec_EmptyQ  ());

    // teardown
}

//
// Insert 100 elements, Remove them in "random" order
//
void atf_amc::amctest_ZslistRemove() {
    // setup
    atf_amc::FTypeC item[100];
    bool removed[100];
    frep_(i,100) {
        item[i].typec = 100+i;
        atf_amc::zs_t_typec_Insert(item[i]);
        removed[i] = false;
    }
    // exercise/verify
    frep_(i,100) {
        i32 ind = (i*73) % 100;
        removed[ind] = true;
        // exercise
        atf_amc::zs_t_typec_Remove(item[ind]);
        // verify
        vrfy_(false == atf_amc::zs_t_typec_InLlistQ(item[ind]));
        atf_amc::FTypeC *first = NULL;
        atf_amc::FTypeC *last  = NULL;
        atf_amc::FTypeC *prev  = NULL;
        int nb_typec = 0;
        frep_(j,100) {
            vrfy_(!removed[j] == atf_amc::zs_t_typec_InLlistQ(item[j]));
            if (!removed[j]) {
                ++nb_typec;
                if (!first) first = &item[j];
                last = &item[j];
                if (prev) vrfy_(&item[j] ==  atf_amc::zs_t_typec_Next(*prev));
                prev = &item[j];
            } else {
                vrfy_((atf_amc::FTypeC*)-1 ==  atf_amc::zs_t_typec_Next(item[j]));
            }
        }
        if (last)  vrfy_(NULL == atf_amc::zs_t_typec_Next    (*last));
        vrfy_(first           == atf_amc::zs_t_typec_First   ());
        vrfy_(last            == atf_amc::zs_t_typec_Last    ());
        vrfy_(nb_typec         == atf_amc::zs_t_typec_N       ());
        vrfy_(!nb_typec        == atf_amc::zs_t_typec_EmptyQ  ());
    }
    frep_(i,100) {
        vrfy_(item[i].typec = 100+i); // check if nothing has corrupted
    }
    // teardown
}

// -----------------------------------------------------------------------------
// ZSLISTMT

//
// check the newly created item is not in the list
//
void atf_amc::amctest_ZslistmtItemDfltCtor() {
    // setup
    atf_amc::FTypeC item;
    // verify
    vrfy_(false == atf_amc::zs_mt_typec_InLlistQ(item));
    // teardown
}

//
// check that newly created list is empty
//
void atf_amc::amctest_ZslistmtDfltCtor() {
    // setup
    // exercise/verify
    vrfy_(NULL == atf_amc::zs_mt_typec_RemoveFirst());
    // teardown
}

//
// add 1 item, and then delete
//
void atf_amc::amctest_Zslistmt1() {
    // setup
    atf_amc::FTypeC  item;
    item.typec = 100;
    // exercise/verify
    zs_mt_typec_Insert(item);
    vrfy_(true == zs_mt_typec_InLlistQ(item));
    vrfy_(&item == atf_amc::zs_mt_typec_RemoveFirst());
    vrfy_(false == zs_mt_typec_InLlistQ(item));
    vrfy_(NULL == atf_amc::zs_mt_typec_RemoveFirst());
    vrfy_(100 == item.typec);
    // teardown
}

//
// add 2 items, and then delete
//
void atf_amc::amctest_Zslistmt2() {
    // setup
    atf_amc::FTypeC  item[2];
    frep_(i,2) { item[i].typec = 100+i; }
    // exercise/verify
    frep_(i,2) {
        atf_amc::zs_mt_typec_Insert(item[i]);
    }
    frep_(i,2) {
        vrfy_(true == atf_amc::zs_mt_typec_InLlistQ(item[i]));
    }
    frep_(i,2) {
        vrfy_(&item[i] == atf_amc::zs_mt_typec_RemoveFirst());
        frep_(j,2) {
            vrfy_((j>i) == atf_amc::zs_mt_typec_InLlistQ(item[j]));
        }
    }
    frep_(i,2) {
        vrfy_(100+i == item[i].typec);
    }
    vrfy_(NULL == atf_amc::zs_mt_typec_RemoveFirst());
    // teardown
}

//
// add 3 items, and then delete
//
void atf_amc::amctest_Zslistmt3() {
    // setup
    atf_amc::FTypeC  item[3];
    frep_(i,3) { item[i].typec = 100+i; }
    // exercise/verify
    frep_(i,3) {
        atf_amc::zs_mt_typec_Insert(item[i]);
    }
    frep_(i,3) {
        vrfy_(true == atf_amc::zs_mt_typec_InLlistQ(item[i]));
    }
    frep_(i,3) {
        vrfy_(&item[i] == atf_amc::zs_mt_typec_RemoveFirst());
        frep_(j,3) {
            vrfy_((j>i) == atf_amc::zs_mt_typec_InLlistQ(item[j]));
        }
    }
    frep_(i,3) {
        vrfy_(100+i == item[i].typec);
    }
    vrfy_(NULL == atf_amc::zs_mt_typec_RemoveFirst());
    // teardown
}

// -----------------------------------------------------------------------------
// ZSLIST - TAIL INSERTION - FIRST CHANGED

//
// callback for trigger

void atf_amc::zs_t_typec_FirstChanged() {
    ++atf_amc::_db.zs_t_count;
}

//
// Insert 3 items, check trigger fires only for the first
//
void atf_amc::amctest_ZslistFirstChangedInsert() {
    // setup
    vrfy_(true == atf_amc::zs_t_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[3];
    atf_amc::_db.zs_t_count = 0;
    // exercise/verify
    atf_amc::zs_t_typec_Insert(item[0]);
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_Insert(item[1]);
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_Insert(item[2]);
    vrfy_(1 == atf_amc::_db.zs_t_count);
    // teardown
    // item destructor should automatically remove item from list
}

//
// Insert 3 items
// RemoveFirst 3 items, check trigger fires for each
// RemoveFirst from empty list, check trigger does not fire
void atf_amc::amctest_ZslistFirstChangedRemoveFirst() {
    // setup
    vrfy_(true == atf_amc::zs_t_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[3];
    frep_(i,3) { atf_amc::zs_t_typec_Insert(item[i]); }
    atf_amc::_db.zs_t_count = 0;
    // exercise/verify
    atf_amc::zs_t_typec_RemoveFirst();
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_RemoveFirst();
    vrfy_(2 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_RemoveFirst();
    vrfy_(3 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_RemoveFirst(); // remove from empty list
    vrfy_(3 == atf_amc::_db.zs_t_count);
    // teardown
    // item destructor should automatically remove item from list
}

//
// Insert 4 items
// Remove in the following order, check trigger:
// first (first) - fires
// third (middle) - does not fire
// fourth (tail) - does not fire
// second - (the only) - fires
//
void atf_amc::amctest_ZslistFirstChangedRemove() {
    // setup
    vrfy_(true == atf_amc::zs_t_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[4];
    frep_(i,4) { atf_amc::zs_t_typec_Insert(item[i]); }
    atf_amc::_db.zs_t_count = 0;
    // exercise/verify
    atf_amc::zs_t_typec_Remove(item[0]); // remove first
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_Remove(item[2]); // remove middle
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_Remove(item[3]); // remove last
    vrfy_(1 == atf_amc::_db.zs_t_count);
    atf_amc::zs_t_typec_Remove(item[1]); // remove only
    vrfy_(2 == atf_amc::_db.zs_t_count);
    // teardown
    // item destructor should automatically remove item from list
}

//
// Insert 100 items
// Flush
// Trigger fires once
//

void atf_amc::amctest_ZslistFirstChangedFlush() {
    // setup
    vrfy_(true == atf_amc::zs_t_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[100];
    frep_(i,100) { atf_amc::zs_t_typec_Insert(item[i]); }
    atf_amc::_db.zs_t_count = 0;
    // exercise/verify
    atf_amc::zs_t_typec_RemoveAll();
    vrfy_(1 == atf_amc::_db.zs_t_count);
    // teardown
    // item destructor should automatically remove item from list
}


// -----------------------------------------------------------------------------
// ZSLIST - HEAD INSERTION - FIRST CHANGED

void atf_amc::zsl_h_typec_FirstChanged() {
    ++atf_amc::_db.zs_h_count;
}

//
// Insert 3 items, check the trigger fires for each
//

void atf_amc::amctest_ZslistHeadFirstChangedInsert() {
    // setup
    vrfy_(true == atf_amc::zs_t_typec_EmptyQ());
    atf_amc::FTypeC item[3];
    atf_amc::_db.zs_h_count = 0;
    // exercise/verify
    atf_amc::zsl_h_typec_Insert(item[0]);
    vrfy_(1 == atf_amc::_db.zs_h_count);
    atf_amc::zsl_h_typec_Insert(item[1]);
    vrfy_(2 == atf_amc::_db.zs_h_count);
    atf_amc::zsl_h_typec_Insert(item[2]);
    vrfy_(3 == atf_amc::_db.zs_h_count);
    // teardown
    // item destructor should automatically remove item from list
}
