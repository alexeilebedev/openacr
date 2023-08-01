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
// Source: cpp/atf/amc/ptrary.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

void atf_amc::amctest_PtraryInsert() {
    atf_amc::FTypeK a,b;

    // insert a.
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a._db_c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);

    // insert again -- no effect
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a._db_c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);

    // remove
    c_typek_Remove(a);
    vrfy_(c_typek_Find(0)==NULL);
    vrfy_(a._db_c_typek_in_ary ==false);
    vrfy_(c_typek_N()==0);

    // remove again -- no effect
    c_typek_Remove(a);
    vrfy_(c_typek_Find(0)==NULL);
    vrfy_(a._db_c_typek_in_ary ==false);
    vrfy_(c_typek_N()==0);

    // reinsert -- should be there now
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a._db_c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);
}


void atf_amc::amctest_PtraryCursor() {
    atf_amc::FTypeK a,b;

    c_typek_Insert(a);
    c_typek_Insert(b);
    vrfy_(a._db_c_typek_in_ary ==true);
    vrfy_(b._db_c_typek_in_ary ==true);
    // check that once cursor on unique array removes elements from list
    ind_beg(_db_c_typek_oncecurs,typek,_db) {
        vrfy_(ind_curs(typek).index!=0 || &typek == &a);
        vrfy_(ind_curs(typek).index!=1 || &typek == &b);
    }ind_end;
    vrfy_(c_typek_N()==0);
    vrfy_(a._db_c_typek_in_ary ==false);
    vrfy_(b._db_c_typek_in_ary ==false);
}
