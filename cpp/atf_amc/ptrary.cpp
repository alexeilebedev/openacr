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
// Source: cpp/atf_amc/ptrary.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

// A unique Ptrary's Insert and Remove are idempotent: the row carries the
// flag saying whether it is a member, so inserting a row that is already in
// the array and removing one that is not are both no-ops, and neither the
// element count nor the row's position moves.  Callers rely on that -- it is
// why the codebase never guards these calls with a membership test -- so a
// regression would silently double-count members across the whole tree.
void atf_amc::amctest_PtraryInsert() {
    atf_amc::FTypeK a;
    c_typek_RemoveAll();// establish the empty array the asserts below expect

    // insert a.
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a.c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);

    // insert again -- no effect
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a.c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);

    // remove
    c_typek_Remove(a);
    vrfy_(c_typek_Find(0)==NULL);
    vrfy_(a.c_typek_in_ary ==false);
    vrfy_(c_typek_N()==0);

    // remove again -- no effect
    c_typek_Remove(a);
    vrfy_(c_typek_Find(0)==NULL);
    vrfy_(a.c_typek_in_ary ==false);
    vrfy_(c_typek_N()==0);

    // reinsert -- should be there now
    c_typek_Insert(a);
    vrfy_(c_typek_Find(0)==&a);
    vrfy_(a.c_typek_in_ary ==true);
    vrfy_(c_typek_N()==1);

    // leave the shared array empty: a is a stack row, and a pointer to it
    // must not outlive this test
    c_typek_Remove(a);
    vrfy_(c_typek_N()==0);
}


void atf_amc::amctest_PtraryCursor() {
    atf_amc::FTypeK a,b;
    c_typek_RemoveAll();// establish the empty array the asserts below expect

    c_typek_Insert(a);
    c_typek_Insert(b);
    vrfy_(_db.c_typek_n==2);
    vrfy_(a.c_typek_in_ary ==true);
    vrfy_(b.c_typek_in_ary ==true);
    // check that once cursor on unique array removes elements from list
    ind_beg(_db_c_typek_oncecurs,typek,_db) {
        vrfy_(ind_curs(typek).index!=0 || &typek == &a);
        vrfy_(ind_curs(typek).index!=1 || &typek == &b);
    }ind_end;
    vrfy_(c_typek_N()==0);
    vrfy_(a.c_typek_in_ary ==false);
    vrfy_(b.c_typek_in_ary ==false);
}

// -----------------------------------------------------------------------------

// A once-cursor detaches the captured run's membership at Reset: the array
// empties and every captured element's membership flag clears up front, so a
// walk that exits early cannot leave a row claiming membership in the emptied
// array (a stale flag would turn the row's next Insert into a silent no-op).
// The run itself stays in the parent's buffer, which the cursor aliases
// without copying, so inserting into the array during the walk remains
// forbidden: an insert would overwrite the unread tail of the run.
void atf_amc::amctest_PtraryOnceCursEarlyExit() {
    atf_amc::FTypeK a,b;
    c_typek_RemoveAll();// establish the empty array the asserts below expect
    c_typek_Insert(a);
    c_typek_Insert(b);
    ind_beg(_db_c_typek_oncecurs,typek,_db) {
        vrfy_(&typek == &a);
        break; // early exit is the scenario under test
    }ind_end;
    vrfy_(c_typek_N()==0);
    vrfy_(!c_typek_InAryQ(a));
    vrfy_(!c_typek_InAryQ(b));
    // a cleared flag lets the row re-enter the emptied array
    c_typek_Insert(b);
    vrfy_(c_typek_N()==1);
    vrfy_(c_typek_Find(0)==&b);
    c_typek_RemoveAll();
}

// -----------------------------------------------------------------------------

// Heaplike flavor of the once-cursor contract: after an early-exited walk
// every captured element's index reads not-in-array, and a row can re-enter
// the emptied array.
void atf_amc::amctest_PtraryOnceCursHeaplike() {
    atf_amc::FTypeL a,b;
    c_typel_RemoveAll();// establish the empty array the asserts below expect
    c_typel_Insert(a);
    c_typel_Insert(b);
    ind_beg(_db_c_typel_oncecurs,typel,_db) {
        vrfy_(&typel == &a);
        break; // early exit is the scenario under test
    }ind_end;
    vrfy_(c_typel_N()==0);
    vrfy_(!c_typel_InAryQ(a));
    vrfy_(!c_typel_InAryQ(b));
    c_typel_Insert(b);
    vrfy_(c_typel_N()==1);
    vrfy_(c_typel_Find(0)==&b);
    c_typel_RemoveAll();
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_PtraryHeaplike() {
    atf_amc::FTypeL a,b,c;
    c_typel_RemoveAll();// establish the empty array the asserts below expect
    vrfy_(c_typel_First()==NULL);
    vrfy_(c_typel_Last()==NULL);

    vrfy_(a.c_typel_idx==-1);
    vrfy_(b.c_typel_idx==-1);
    vrfy_(c.c_typel_idx==-1);

    c_typel_Insert(a);
    vrfy_(c_typel_First()==&a);
    vrfy_(_db.c_typel_n==1);
    c_typel_Insert(b);
    vrfy_(c_typel_Last()==&b);
    vrfy_(_db.c_typel_n==2);
    c_typel_Insert(c);
    vrfy_(c_typel_Last()==&c);

    vrfy_(_db.c_typel_n==3);
    vrfy_(a.c_typel_idx==0);
    vrfy_(b.c_typel_idx==1);
    vrfy_(c.c_typel_idx==2);

    c_typel_Remove(a);
    vrfy_(_db.c_typel_n==2);
    vrfy_(a.c_typel_idx==-1);
    vrfy_(b.c_typel_idx==1);
    vrfy_(c.c_typel_idx==0);

    c_typel_RemoveAll();
    vrfy_(_db.c_typel_n==0);
    vrfy_(a.c_typel_idx==-1);
    vrfy_(b.c_typel_idx==-1);
    vrfy_(c.c_typel_idx==-1);

    c_typel_Insert(a);
    c_typel_Insert(b);
    c_typel_Insert(c);
    vrfy_(c_typel_Last()==&c);
    vrfy_(c.c_typel_idx==2);
    vrfy_(c_typel_RemoveLast()==&c);
    vrfy_(c_typel_Last()==&b);
    vrfy_(b.c_typel_idx==1);
    vrfy_(c_typel_RemoveLast()==&b);
    vrfy_(c_typel_Last()==&a);
    vrfy_(a.c_typel_idx==0);
    vrfy_(c_typel_RemoveLast()==&a);
    vrfy_(c_typel_Last()==NULL);
    vrfy_(c_typel_RemoveLast()==NULL);

    c_typel_Insert(a);
    c_typel_Insert(b);
    c_typel_Insert(c);
    vrfy_(c_typel_First()==&a);
    vrfy_(a.c_typel_idx==0);
    vrfy_(c_typel_RemoveFirst()==&a);
    vrfy_(c_typel_First()==&c);
    vrfy_(c.c_typel_idx==0);
    vrfy_(c_typel_RemoveFirst()==&c);
    vrfy_(c_typel_First()==&b);
    vrfy_(b.c_typel_idx==0);
    vrfy_(c_typel_RemoveFirst()==&b);
    vrfy_(c_typel_Last()==NULL);
    vrfy_(c_typel_RemoveFirst()==NULL);
    // verify removed elements have idx==-1
    vrfy_(a.c_typel_idx==-1);
    vrfy_(b.c_typel_idx==-1);
    vrfy_(c.c_typel_idx==-1);

    // Test RemoveFirst edge case when n==1
    // This catches a bug where elems[n-1]->idx=0 overwrites row->idx=-1
    // when n==1 (because elems[0] == elems[n-1] == row)
    c_typel_Insert(a);
    vrfy_(c_typel_N()==1);
    vrfy_(c_typel_RemoveFirst()==&a);
    vrfy_(a.c_typel_idx==-1);  // must be -1, not 0
    vrfy_(!c_typel_InAryQ(a)); // must report not-in-array
}

// -----------------------------------------------------------------------------

// Reserve(n) must guarantee capacity for n more elements even when n
// exceeds the doubled current capacity. The request is computed relative
// to whatever capacity earlier in-process tests left behind, so the test
// needs no pristine global state.
void atf_amc::amctest_PtraryReserve() {
    u64 want = (_db.c_typem_max - _db.c_typem_n) + 1000;
    c_typem_Reserve(want);
    vrfy_(_db.c_typem_n + want <= _db.c_typem_max);
}

// -----------------------------------------------------------------------------

// OnUnref hook for the non-unique Ptrary c_typem: record the firing on the row
void atf_amc::c_typem_OnUnref(atf_amc::FTypeM &row) {
    row.n_unref++;
}

// OnXref hook for the non-unique Ptrary c_typem: record the firing on the row
void atf_amc::c_typem_OnXref(atf_amc::FTypeM &row) {
    row.n_xref++;
}

// Both insert paths of a Ptrary fire OnXref when the row enters the array:
// Insert unconditionally appends and fires; ScanInsertMaybe fires only when
// the scan found no duplicate and the row was actually inserted.
void atf_amc::amctest_PtraryScanInsertOnXref() {
    atf_amc::FTypeM a;
    c_typem_RemoveAll();

    vrfy_(c_typem_ScanInsertMaybe(a));
    vrfyeq_(a.n_xref, u32(1));

    vrfy_(!c_typem_ScanInsertMaybe(a)); // duplicate: no insert, no callback
    vrfyeq_(a.n_xref, u32(1));

    c_typem_Insert(a); // unconditional append fires as well
    vrfyeq_(a.n_xref, u32(2));

    c_typem_RemoveAll();
}

// Remove on a non-unique Ptrary compacts away every occurrence of the row;
// once none remain, the row is no longer referenced by the array, so OnUnref
// must fire -- exactly once per Remove that removed something, matching the
// heaplike and unique flavors. A Remove of a row that is not in the array
// fires nothing.
void atf_amc::amctest_PtraryNonUniqueOnUnref() {
    atf_amc::FTypeM a,b;
    c_typem_RemoveAll();

    c_typem_Insert(a);
    c_typem_Insert(b);
    c_typem_Insert(a);
    c_typem_Insert(a);
    vrfyeq_(a.n_unref, u32(0));

    c_typem_Remove(a); // removes all 3 occurrences
    vrfyeq_(a.n_unref, u32(1));
    vrfyeq_(_db.c_typem_n, u64(1));

    c_typem_Remove(a); // not in array: no callback
    vrfyeq_(a.n_unref, u32(1));

    c_typem_Remove(b);
    vrfyeq_(b.n_unref, u32(1));
    vrfyeq_(_db.c_typem_n, u64(0));
}

void atf_amc::amctest_PtraryNonUnique() {
    atf_amc::FTypeM a,b,c;
    c_typem_RemoveAll();// establish the empty array the asserts below expect

    c_typem_Insert(a);
    c_typem_Insert(b);
    c_typem_Insert(c);
    c_typem_Insert(a);
    c_typem_Insert(a);
    c_typem_Insert(c);

    vrfy_(_db.c_typem_n == 6);

    // non-unique Ptary - removal must scan for all references
    c_typem_Remove(a);
    vrfy_(_db.c_typem_n == 3);

    c_typem_Remove(c);
    vrfy_(_db.c_typem_n == 1);

    c_typem_Remove(b);
    vrfy_(_db.c_typem_n == 0);
}
