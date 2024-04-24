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
// Source: cpp/atf_amc/bheap.cpp
//

#include "include/atf_amc.h"

// =============================================================================
// BHEAP

//
// Insert 100 ascending values in bheap
//    - ascending
//    - descending
//    - mixed
// Check they are inserted and read in the same order
//
void atf_amc::amctest_BheapInsert100() {
    frep_(testno, 3) {
        vrfyeq_(0,atf_amc::bh_typec_N());
        // setup
        atf_amc::FTypeC  item[100];
        switch(testno) {
        case 0: {
            frep_(i,100) item[i].typec = i;// ascending
        }break;

        case 1: {
            frep_(i,100) item[i].typec = 99-i;//descending
        }break;

        case 2: {
            frep_(i,100) { item[i].typec = (i*73) % 100; }// mixed
        }break;
        }

        // exercise
        frep_(i,100) { atf_amc::bh_typec_Insert(item[i]); }

        // verify
        vrfy_(!atf_amc::bh_typec_EmptyQ()); // not empty
        vrfy_(100==atf_amc::bh_typec_N());  // exactly 100 items
        frep_(i,100) { // check each item
            vrfy_(atf_amc::bh_typec_InBheapQ(item[i])); // check if item is in heap
        }

        // check that all values are there
        int i=0;
        ind_beg(atf_amc::_db_bh_typec_curs,item_access,atf_amc::_db) {
            vrfy_(i == item_access.typec); // the same value in the same order
            ++i;
        }ind_end;
        vrfy_(i==100);

        atf_amc::bh_typec_RemoveAll();
        // teardown
    }

    // check reserve
    {
        // insert...
        atf_amc::_db.bh_typec_n = atf_amc::_db.bh_typec_max;
        u64 c = algo::get_cycles();
        atf_amc::bh_typec_Reserve(2500);
        c = algo::get_cycles() - c;
        verblog("reserve 2500 elems: "<<c<<" cycles");

        atf_amc::_db.bh_typec_n = atf_amc::_db.bh_typec_max;
        c = algo::get_cycles();
        atf_amc::bh_typec_Reserve(5000);
        c = algo::get_cycles() - c;
        verblog("reserve 5000 elems: "<<c<<" cycles");

        atf_amc::_db.bh_typec_n = atf_amc::_db.bh_typec_max;
        c = algo::get_cycles();
        atf_amc::bh_typec_Reserve(10000);
        c = algo::get_cycles() - c;
        verblog("reserve 10000 elems: "<<c<<" cycles");

        atf_amc::_db.bh_typec_n = 0;
    }
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_BheapCursor() {
    atf_amc::FTypeA &typea = atf_amc::typea_Alloc();
    typea.typea = 1;
    vrfy(typea_XrefMaybe(typea), algo_lib::_db.errtext);
    frep_(i,100) {
        atf_amc::FTypeB &typeb = atf_amc::typeb_Alloc();
        typeb.typea = typea.typea;
        typeb.j = -i;
        vrfy(typeb_XrefMaybe(typeb), algo_lib::_db.errtext);
    }

    int j = 0;
    ind_beg(atf_amc::typea_bh_typeb_curs,typeb,typea) {
        vrfy_(typeb.j == j);
        j--;
    }ind_end;

    // check again
    j = 0;
    ind_beg(atf_amc::typea_bh_typeb_curs,typeb,typea) {
        vrfy_(typeb.j == j);
        j--;
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_amc::bh_typec_FirstChanged() {
    ++atf_amc::_db.bh_count;
}

void atf_amc::amctest_BhFirstChanged1() {
    // Ascending order
    // setup
    vrfy_(true == atf_amc::bh_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[3];
    item[0].typec = 0;
    item[1].typec = 1;
    item[2].typec = 2;
    atf_amc::_db.bh_count = 0;

    // Insert 3 items, check that trigger fires for the first one
    atf_amc::bh_typec_Insert(item[0]);
    vrfy_(1 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_Insert(item[1]);
    vrfy_(1 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_Insert(item[2]);
    vrfy_(1 == atf_amc::_db.bh_count);

    // remove all -- FirstChanged should be triggered
    atf_amc::bh_typec_RemoveAll();
    vrfy_(2 == atf_amc::_db.bh_count);

    // Try it again -- nothing should happen
    atf_amc::bh_typec_RemoveAll();
    vrfy_(2 == atf_amc::_db.bh_count);

    // Insert 3 items again
    atf_amc::bh_typec_Insert(item[0]);
    vrfy_(3 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_Insert(item[1]);
    vrfy_(3 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_Insert(item[2]);
    vrfy_(3 == atf_amc::_db.bh_count);

    // Remove first
    atf_amc::bh_typec_RemoveFirst();
    vrfy_(4 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_RemoveFirst();
    vrfy_(5 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_RemoveFirst();
    vrfy_(6 == atf_amc::_db.bh_count);
    atf_amc::bh_typec_RemoveFirst(); // remove from empty list
    vrfy_(6 == atf_amc::_db.bh_count);
}

void atf_amc::amctest_BhFirstChanged2() {
    // Insert 100 items in ascending order -- check that trigger fires once
    vrfy_(true == atf_amc::bh_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[100];
    atf_amc::_db.bh_count = 0;
    frep_(i,100) {
        item[i].typec = i;
        atf_amc::bh_typec_Insert(item[i]);
        vrfy_(1 == atf_amc::_db.bh_count);
    }
}

void atf_amc::amctest_BhFirstChanged3() {
    vrfy_(true == atf_amc::bh_typec_EmptyQ()); // check precondition
    atf_amc::FTypeC item[100];
    atf_amc::_db.bh_count = 0;
    frep_(i,100) {
        item[i].typec = -i;
        atf_amc::bh_typec_Insert(item[i]);
        vrfy_(i+1 == atf_amc::_db.bh_count);
    }
    // Remove first item
    vrfy_(item[99].bh_typec_idx == 0);// check that it's the first item
    atf_amc::bh_typec_Remove(item[99]);
    vrfy_(101 == atf_amc::_db.bh_count);

    // Remove non-first item
    vrfy_(item[97].bh_typec_idx != 0);// check that it's the first item
    atf_amc::bh_typec_Remove(item[97]);
    vrfy_(101 == atf_amc::_db.bh_count);

    // Remove first item
    vrfy_(item[98].bh_typec_idx == 0);// check that it's the first item
    atf_amc::bh_typec_Remove(item[98]);
    vrfy_(102 == atf_amc::_db.bh_count);

    // Remove first item
    vrfy_(item[96].bh_typec_idx == 0);// check that it's the first item
    atf_amc::bh_typec_Remove(item[96]);
    vrfy_(103 == atf_amc::_db.bh_count);

    // Remove first item explicitly
    vrfy_(item[95].bh_typec_idx == 0);// check that it's the first item
    atf_amc::bh_typec_RemoveFirst();
    vrfy_(104 == atf_amc::_db.bh_count);

    // Move first item away from the top -- trigger should fire
    vrfy_(item[94].bh_typec_idx == 0);// check that it's the first item
    item[94].typec = 100;
    atf_amc::bh_typec_ReheapFirst();
    vrfy_(105 == atf_amc::_db.bh_count);

    // Reheap first item -- nothing should happen
    atf_amc::bh_typec_ReheapFirst();
    vrfy_(105 == atf_amc::_db.bh_count);

    // Reheap first item (not explicitly) -- trigger should fire
    vrfy_(item[93].bh_typec_idx == 0);// check that it's the first item
    item[93].typec = 100;
    atf_amc::bh_typec_Reheap(item[93]);
    vrfy_(106 == atf_amc::_db.bh_count);

    // inscond is not defined on this item
#if 0
    // Call Set function to change key on first item (not explicitly)
    // trigger should fire
    vrfy_(item[92].bh_typec_idx == 0);// check that it's the first item
    atf_amc::typec_Set(item[92], 100);
    vrfy_(107 == atf_amc::_db.bh_count);

    // _Set on random item -- nothing should happen
    vrfy_(item[0].bh_typec_idx != 0);// check that it's the first item
    atf_amc::typec_Set(item[0], 100);
    vrfy_(107 == atf_amc::_db.bh_count);
#endif
}
