// Copyright (C) 2026 AlgoRND
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/bheaphead.cpp
//
// Every row of atf_amc.FHeapElem is indexed by three priority structures on
// the same key: a Bheap, a Bheap whose head is a sorted run of 32 rows, and an
// Atree.  The order tests drive the two Bheaps through the same operations
// and compare them at every step.  The feature tests cover a Bheap with a head
// owned by a record, its
// callbacks, compaction and cascade, its steps, and its memory functions.
// The benchmark drives each structure alone through the same scenario.  Rows
// of c_heap_elem are never deleted: each amctest runs in its own process, and
// deleting a row would remove it from c_heap_elem with a scan.

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

// OnXref hook for FHeadGroup.bh_heap_elem: record the firing on the row
void atf_amc::bh_heap_elem_OnXref(atf_amc::FHeadGroup &parent, atf_amc::FHeapElem &row) {
    (void)parent;
    row.n_xref++;
}

// -----------------------------------------------------------------------------

// OnUnref hook for FHeadGroup.bh_heap_elem: record the firing on the row
void atf_amc::bh_heap_elem_OnUnref(atf_amc::FHeadGroup &parent, atf_amc::FHeapElem &row) {
    (void)parent;
    row.n_unref++;
}

// -----------------------------------------------------------------------------

// Step of FDb.bh_time_entry_head: delete the first entry
void atf_amc::bh_time_entry_head_Step() {
    time_entry_Delete(*bh_time_entry_head_RemoveFirst());
}

// -----------------------------------------------------------------------------

// Step of FDb.bh_time_entry_recur: delete the first entry
void atf_amc::bh_time_entry_recur_Step() {
    time_entry_Delete(*bh_time_entry_recur_RemoveFirst());
}

// -----------------------------------------------------------------------------

// Step of FDb.bh_time_entry_once: delete the first entry
void atf_amc::bh_time_entry_once_Step() {
    time_entry_Delete(*bh_time_entry_once_RemoveFirst());
}

// -----------------------------------------------------------------------------

// Advance the 64-bit LCG SEED and return 31 random bits from it.
static u64 NextRand(u64 &seed) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return seed >> 33;
}

// -----------------------------------------------------------------------------

// Return a random number below RANGE, drawn from SEED.  RANGE may exceed 2^31.
static u64 RandBelow(u64 &seed, u64 range) {
    u64 hi = NextRand(seed);
    u64 lo = NextRand(seed);
    return ((hi << 31) | lo) % range;
}

// -----------------------------------------------------------------------------

// Allocate test rows until c_heap_elem holds at least N of them.
static void ReserveElem(i32 n) {
    while (atf_amc::c_heap_elem_N() < n) {
        atf_amc::FHeapElem &elem = atf_amc::heap_elem_Alloc();
        atf_amc::c_heap_elem_Insert(elem);
    }
}

// -----------------------------------------------------------------------------

// Return a random row among the first N rows of c_heap_elem, drawn from SEED.
static atf_amc::FHeapElem &RandElem(u64 &seed, i32 n) {
    return atf_amc::c_heap_elem_qFind(RandBelow(seed, u64(n)));
}

// -----------------------------------------------------------------------------

// The structures are numbered by KIND: 0 is the Bheap, 1 the Bheap with a head and 2 the
// Atree.  The functions below dispatch one operation on KIND.

// Insert ROW into the structure KIND.
static void HeapInsert(i32 kind, atf_amc::FHeapElem &row) {
    if (kind == 0) {
        atf_amc::bh_heap_elem_bheap_Insert(row);
    } else if (kind == 1) {
        atf_amc::bh_heap_elem_head_Insert(row);
    } else {
        atf_amc::tr_heap_elem_Insert(row);
    }
}

// -----------------------------------------------------------------------------

// Remove ROW from the structure KIND.
static void HeapRemove(i32 kind, atf_amc::FHeapElem &row) {
    if (kind == 0) {
        atf_amc::bh_heap_elem_bheap_Remove(row);
    } else if (kind == 1) {
        atf_amc::bh_heap_elem_head_Remove(row);
    } else {
        atf_amc::tr_heap_elem_Remove(row);
    }
}

// -----------------------------------------------------------------------------

// Move ROW within the structure KIND after its key changed.
static void HeapReheap(i32 kind, atf_amc::FHeapElem &row) {
    if (kind == 0) {
        atf_amc::bh_heap_elem_bheap_Reheap(row);
    } else if (kind == 1) {
        atf_amc::bh_heap_elem_head_Reheap(row);
    } else {
        atf_amc::tr_heap_elem_Reinsert(row);
    }
}

// -----------------------------------------------------------------------------

// Return the row with the smallest key in the structure KIND, or NULL.
static atf_amc::FHeapElem *HeapFirst(i32 kind) {
    atf_amc::FHeapElem *row = NULL;
    if (kind == 0) {
        row = atf_amc::bh_heap_elem_bheap_First();
    } else if (kind == 1) {
        row = atf_amc::bh_heap_elem_head_First();
    } else {
        row = atf_amc::tr_heap_elem_First();
    }
    return row;
}

// -----------------------------------------------------------------------------

// Remove and return the row with the smallest key in the structure KIND.
static atf_amc::FHeapElem *HeapRemoveFirst(i32 kind) {
    atf_amc::FHeapElem *row = NULL;
    if (kind == 0) {
        row = atf_amc::bh_heap_elem_bheap_RemoveFirst();
    } else if (kind == 1) {
        row = atf_amc::bh_heap_elem_head_RemoveFirst();
    } else {
        // the Atree's RemoveFirst returns nothing; it is this same First and Remove
        row = atf_amc::tr_heap_elem_First();
        atf_amc::tr_heap_elem_Remove(*row);
    }
    return row;
}

// -----------------------------------------------------------------------------

// Walk the first K rows of the structure KIND with its sorted cursor and
// return the sum of their keys.  The Bheap with a head walks with fillcurs.
static u64 WalkTop(i32 kind, i32 k) {
    u64 sum = 0;
    i32 i = 0;
    if (kind == 0) {
        atf_amc::_db_bh_heap_elem_bheap_curs curs;
        atf_amc::_db_bh_heap_elem_bheap_curs_Reset(curs, atf_amc::_db);
        while (i < k && atf_amc::_db_bh_heap_elem_bheap_curs_ValidQ(curs)) {
            sum += atf_amc::_db_bh_heap_elem_bheap_curs_Access(curs).key;
            i++;
            atf_amc::_db_bh_heap_elem_bheap_curs_Next(curs);
        }
    } else if (kind == 1) {
        atf_amc::_db_bh_heap_elem_head_fillcurs curs;
        atf_amc::_db_bh_heap_elem_head_fillcurs_Reset(curs, atf_amc::_db);
        while (i < k && atf_amc::_db_bh_heap_elem_head_fillcurs_ValidQ(curs)) {
            sum += atf_amc::_db_bh_heap_elem_head_fillcurs_Access(curs).key;
            i++;
            atf_amc::_db_bh_heap_elem_head_fillcurs_Next(curs);
        }
    } else {
        atf_amc::_db_tr_heap_elem_curs curs;
        atf_amc::_db_tr_heap_elem_curs_Reset(curs, atf_amc::_db);
        while (i < k && atf_amc::_db_tr_heap_elem_curs_ValidQ(curs)) {
            sum += atf_amc::_db_tr_heap_elem_curs_Access(curs).key;
            i++;
            atf_amc::_db_tr_heap_elem_curs_Next(curs);
        }
    }
    return sum;
}

// =============================================================================
// ORDER: the Bheap with a head against its twin without one

// Walk the Bheap with a head with its sorted cursor and check that the keys come out in
// order and that the walk visits every row once.  Walk it again with the
// unordered cursor and check the count and that each row knows it is in the
// heap.
static void CheckCursor() {
    i32 n = 0;
    i32 nunord = 0;
    u64 prev = 0;
    ind_beg(atf_amc::_db_bh_heap_elem_head_curs, elem, atf_amc::_db) {
        vrfy_(n == 0 || prev <= elem.key);
        prev = elem.key;
        n++;
    }ind_end;
    ind_beg(atf_amc::_db_bh_heap_elem_head_unordcurs, elem, atf_amc::_db) {
        vrfy_(atf_amc::bh_heap_elem_head_InBheapQ(elem));
        nunord++;
    }ind_end;
    vrfyeq_(n, atf_amc::bh_heap_elem_head_N());
    vrfyeq_(nunord, n);
}

// -----------------------------------------------------------------------------

// Remove the first row of the Bheap with a head and the same row from its twin,
// after checking that both heaps agree on the smallest key.
static void PopBoth() {
    atf_amc::FHeapElem *first = atf_amc::bh_heap_elem_bheap_First();
    atf_amc::FHeapElem *row = atf_amc::bh_heap_elem_head_RemoveFirst();
    vrfy_(row && first);
    vrfyeq_(row->key, first->key);
    vrfy_(!atf_amc::bh_heap_elem_head_InBheapQ(*row));
    atf_amc::bh_heap_elem_bheap_Remove(*row);
}

// -----------------------------------------------------------------------------

// Fill the Bheap with a head and its twin with N rows whose keys are drawn below
// N/2+1, so that keys repeat.  Check the cursors, then drain both and check
// that they agree at every step.  The sizes cross the boundary of the run, of
// the root's children and of the first heap levels.
void atf_amc::amctest_BheapHeadOrder() {
    u64 seed = 1;
    i32 sizes[] = {0, 1, 2, 3, 7, 8, 9, 31, 32, 33, 34, 35, 63, 64, 65, 100, 300, 1000, 4999, 70001};
    frep_(k, i32(sizeof(sizes) / sizeof(sizes[0]))) {
        i32 n = sizes[k];
        ReserveElem(n);
        frep_(i, n) {
            atf_amc::FHeapElem &elem = c_heap_elem_qFind(i);
            elem.key = RandBelow(seed, u64(n / 2 + 1));
            bh_heap_elem_head_Insert(elem);
            bh_heap_elem_bheap_Insert(elem);
        }
        vrfyeq_(bh_heap_elem_head_N(), n);
        vrfyeq_(bh_heap_elem_head_EmptyQ(), n == 0);
        CheckCursor();
        frep_(i, n) {
            PopBoth();
            vrfyeq_(bh_heap_elem_head_N(), n - i - 1);
        }
        vrfy_(bh_heap_elem_head_First() == NULL);
        vrfy_(bh_heap_elem_head_RemoveFirst() == NULL);
    }
}

// -----------------------------------------------------------------------------

// Drive the Bheap with a head and its twin through 300,000 random operations over
// NROW rows: insert, remove, a key change followed by Reheap, a growth of the
// first row's key followed by ReheapFirst, RemoveFirst, and a sorted walk of a
// random number of rows.  Keys are drawn below RANGE, so a small RANGE makes
// them repeat.  After every operation the two heaps hold the same rows and
// agree on the smallest key; after every walk they agree on the sum of the
// keys walked, and every 10,000 operations the cursors are checked in full.
static void CheckRandomOp(i32 nrow, u64 range) {
    u64 seed = 2;
    ReserveElem(nrow);
    frep_(i, nrow) {
        atf_amc::c_heap_elem_qFind(i).key = RandBelow(seed, range);
    }
    frep_(op, 300000) {
        atf_amc::FHeapElem &elem = RandElem(seed, nrow);
        u64 choice = NextRand(seed) % 6;
        if (choice == 0) {
            atf_amc::bh_heap_elem_head_Insert(elem);
            atf_amc::bh_heap_elem_bheap_Insert(elem);
        } else if (choice == 1) {
            atf_amc::bh_heap_elem_head_Remove(elem);
            atf_amc::bh_heap_elem_bheap_Remove(elem);
        } else if (choice == 2) {
            elem.key = RandBelow(seed, range);
            atf_amc::bh_heap_elem_head_Reheap(elem);
            atf_amc::bh_heap_elem_bheap_Reheap(elem);
        } else if (choice == 3 && !atf_amc::bh_heap_elem_head_EmptyQ()) {
            atf_amc::FHeapElem &first = *atf_amc::bh_heap_elem_head_First();
            first.key += RandBelow(seed, range);
            atf_amc::bh_heap_elem_head_ReheapFirst();
            atf_amc::bh_heap_elem_bheap_Reheap(first);
        } else if (choice == 4 && !atf_amc::bh_heap_elem_head_EmptyQ()) {
            PopBoth();
        } else if (choice == 5) {
            i32 k = i32(RandBelow(seed, 80));
            vrfyeq_(WalkTop(1, k), WalkTop(0, k));
        }
        vrfyeq_(atf_amc::bh_heap_elem_head_InBheapQ(elem), atf_amc::bh_heap_elem_bheap_InBheapQ(elem));
        vrfyeq_(atf_amc::bh_heap_elem_head_N(), atf_amc::bh_heap_elem_bheap_N());
        if (!atf_amc::bh_heap_elem_head_EmptyQ()) {
            vrfyeq_(atf_amc::bh_heap_elem_head_First()->key, atf_amc::bh_heap_elem_bheap_First()->key);
        }
        if (op % 10000 == 0) {
            CheckCursor();
        }
    }
    CheckCursor();
    while (!atf_amc::bh_heap_elem_head_EmptyQ()) {
        PopBoth();
    }
    vrfy_(atf_amc::bh_heap_elem_bheap_EmptyQ());
}

// -----------------------------------------------------------------------------

// The Bheap with a head agrees with its twin under random operations, once over a
// large heap, once over 20 rows, where most rows live in the run, and once
// over 60 rows, where the run and the heap below it trade rows constantly.
void atf_amc::amctest_BheapHeadRandomOp() {
    CheckRandomOp(20000, 5000);
    CheckRandomOp(20, 30);
    CheckRandomOp(60, 1000);
}

// -----------------------------------------------------------------------------

// Check that every slot the Bheap with a head uses lies inside its pointer
// array: the run's 32 slots, then the heap rows after them.
static void CheckSlot() {
    vrfy_(32 + atf_amc::_db.bh_heap_elem_head_nheap <= atf_amc::_db.bh_heap_elem_head_max);
}

// -----------------------------------------------------------------------------

// Fill both Bheaps with N rows keyed 2000, 1998, 1996 and so on, so each row is
// a new smallest one and the run of the head ends up full and the array exactly
// full.
static void FillDescending(i32 n) {
    ReserveElem(n);
    frep_(i, n) {
        atf_amc::FHeapElem &elem = atf_amc::c_heap_elem_qFind(i);
        elem.key = 2000 - 2 * i;
        atf_amc::bh_heap_elem_head_Insert(elem);
        atf_amc::bh_heap_elem_bheap_Insert(elem);
        CheckSlot();
    }
}

// Drive a row into the gap between the two largest rows of a full run while the
// array is exactly full.  Making room demotes the root into the heap, so the
// row has to go into the run, below the old root; a row sent to the heap as
// well would take a second new slot that the array does not have.  The insert
// path places a new row, 1937 between 1936 and 1938 of 63 rows.  The Reheap
// path moves a heap row of 64, the 2000, to 1935 between 1934 and 1936.
void atf_amc::amctest_BheapHeadFullGap() {
    FillDescending(63);
    vrfyeq_(atf_amc::_db.bh_heap_elem_head_nrun, 32);
    ReserveElem(64);
    atf_amc::FHeapElem &gap = c_heap_elem_qFind(63);
    gap.key = 1937;
    bh_heap_elem_head_Insert(gap);
    bh_heap_elem_bheap_Insert(gap);
    CheckSlot();
    CheckCursor();
    while (!bh_heap_elem_head_EmptyQ()) {
        PopBoth();
    }

    FillDescending(64);
    vrfyeq_(atf_amc::_db.bh_heap_elem_head_nrun, 32);
    atf_amc::FHeapElem &top = c_heap_elem_qFind(0);
    vrfy_(top.bh_heap_elem_head_idx > 31);
    top.key = 1935;
    bh_heap_elem_head_Reheap(top);
    bh_heap_elem_bheap_Reheap(top);
    CheckSlot();
    CheckCursor();
    while (!bh_heap_elem_head_EmptyQ()) {
        PopBoth();
    }
}

// -----------------------------------------------------------------------------

// Insert 100 rows in ascending key order, so each lands at or above the root and
// the run holds the root alone.  A walk of the best 10 with curs reads them in
// order and leaves the run as it was, and a full walk nests inside it.  A walk
// with fillcurs reads the same rows and leaves them in the run, so the pops that
// follow take them in order.
void atf_amc::amctest_BheapHeadFillcurs() {
    ReserveElem(100);
    frep_(i, 100) {
        atf_amc::FHeapElem &elem = c_heap_elem_qFind(i);
        elem.key = i;
        bh_heap_elem_head_Insert(elem);
        bh_heap_elem_bheap_Insert(elem);
    }
    vrfyeq_(atf_amc::_db.bh_heap_elem_head_nrun, 1);
    i32 n = 0;
    ind_beg(atf_amc::_db_bh_heap_elem_head_curs, elem, atf_amc::_db) {
        if (n < 10) {
            vrfyeq_(elem.key, u64(n));
            i32 ninner = 0;
            ind_beg(atf_amc::_db_bh_heap_elem_head_curs, inner, atf_amc::_db) {
                vrfyeq_(inner.key, u64(ninner));
                ninner++;
            }ind_end;
            vrfyeq_(ninner, 100);
        }
        n++;
    }ind_end;
    vrfyeq_(n, 100);
    vrfyeq_(atf_amc::_db.bh_heap_elem_head_nrun, 1);
    vrfyeq_(WalkTop(1, 10), u64(45));
    vrfy_(atf_amc::_db.bh_heap_elem_head_nrun >= 10);
    CheckCursor();
    while (!bh_heap_elem_head_EmptyQ()) {
        PopBoth();
    }
}

// =============================================================================
// FEATURES

// Walk the group's Bheap with its sorted cursor, check that the keys come out
// in order, and return how many rows the walk visited.
static i32 WalkGroup(atf_amc::FHeadGroup &group) {
    i32 n = 0;
    u64 prev = 0;
    ind_beg(atf_amc::head_group_bh_heap_elem_curs, elem, group) {
        vrfy_(n == 0 || prev <= elem.key);
        prev = elem.key;
        n++;
    }ind_end;
    return n;
}

// -----------------------------------------------------------------------------

// Exercise a Bheap with a head owned by a record.  300 rows join it through their xref,
// and each joining fires OnXref once.  A key change through the generated Set
// moves a row without firing either callback.  Removing rows fires OnUnref
// and compacts the pointer array once the rows fill less than a quarter of it.
// Deleting the group deletes the remaining rows through the cascade.
void atf_amc::amctest_BheapHeadGroup() {
    atf_amc::FHeadGroup &group = head_group_Alloc();
    u64 seed = 5;
    atf_amc::FHeapElem *elem[300];
    frep_(i, 300) {
        elem[i] = &heap_elem_Alloc();
        elem[i]->key = NextRand(seed) % 100000;
        elem[i]->p_head_group = &group;
        vrfy_(heap_elem_XrefMaybe(*elem[i]));
        vrfy_(bh_heap_elem_InBheapQ(*elem[i]));
        vrfyeq_(elem[i]->n_xref, 1);
    }
    vrfyeq_(bh_heap_elem_N(group), 300);
    vrfyeq_(group.bh_heap_elem_max, 512);
    // two sorted walks visit every row in order
    vrfyeq_(WalkGroup(group), 300);
    vrfyeq_(WalkGroup(group), 300);
    // a key change moves the row to the front without leaving the index
    key_Set(group, *elem[7], 0);
    vrfy_(bh_heap_elem_First(group) == elem[7]);
    vrfyeq_(elem[7]->n_xref, 1);
    vrfyeq_(elem[7]->n_unref, 0);
    // a worse key does not replace a better one
    key_SetIfBetter(group, *elem[7], 5);
    vrfyeq_(elem[7]->key, u64(0));
    // removing rows fires OnUnref, and the array shrinks as the heap empties
    frep_(i, 280) {
        bh_heap_elem_Remove(group, *elem[i]);
        vrfyeq_(elem[i]->n_unref, 1);
        vrfy_(!bh_heap_elem_InBheapQ(*elem[i]));
    }
    vrfyeq_(bh_heap_elem_N(group), 20);
    vrfyeq_(WalkGroup(group), 20);
    vrfyeq_(group.bh_heap_elem_max, 128);
    // removing a row that is not in the heap fires nothing
    bh_heap_elem_Remove(group, *elem[0]);
    vrfyeq_(elem[0]->n_unref, 1);
    frep_(i, 280) {
        heap_elem_Delete(*elem[i]);
    }
    // the cascade deletes the remaining 20 rows with the group
    head_group_Delete(group);
}

// -----------------------------------------------------------------------------

// Drive a TimeHookOnce step from a Bheap with a head: the step's time hook follows the
// first row of the heap through inserts, key changes and deletes, as it does
// for the Bheap of amctest fstep_TimeHookOnce.
void atf_amc::amctest_BheapHeadFstep() {
    atf_amc::FTimeEntry &a = time_entry_Alloc();
    time_entry_XrefMaybe(a);
    vrfy_(!bh_time_entry_head_InBheapQ(a));
    vrfy_(!algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_head));
    a.time = algo::SchedTime(3);
    bh_time_entry_head_Reheap(a);
    vrfy_(algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_head));
    vrfy_(_db.th_bh_time_entry_head.time == a.time);

    // an entry that preempts a
    atf_amc::FTimeEntry &b = time_entry_Alloc();
    time_entry_XrefMaybe(b);
    b.time = algo::SchedTime(2);
    bh_time_entry_head_Reheap(b);
    vrfy_(_db.th_bh_time_entry_head.time == b.time);

    // an entry that preempts nobody
    atf_amc::FTimeEntry &c = time_entry_Alloc();
    time_entry_XrefMaybe(c);
    c.time = algo::SchedTime(10);
    bh_time_entry_head_Reheap(c);
    vrfy_(_db.th_bh_time_entry_head.time == b.time);

    // the first entry moves behind c, and a becomes first
    b.time = algo::SchedTime(20);
    bh_time_entry_head_ReheapFirst();
    vrfy_(bh_time_entry_head_First() == &a);
    vrfy_(_db.th_bh_time_entry_head.time == a.time);

    // deleting the first entry hands the hook to the next one
    time_entry_Delete(a);
    vrfy_(_db.th_bh_time_entry_head.time == c.time);
    time_entry_Delete(c);
    vrfy_(_db.th_bh_time_entry_head.time == b.time);
    time_entry_Delete(b);
    vrfy_(!algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_head));
}

// -----------------------------------------------------------------------------

// Drive a TimeHookRecur step from a Bheap with a head: the step's time hook is scheduled
// while the heap holds a row and descheduled when it empties, whichever way
// the rows leave it.
void atf_amc::amctest_BheapHeadTimeHookRecur() {
    vrfy_(!algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    atf_amc::FTimeEntry &a = time_entry_Alloc();
    atf_amc::FTimeEntry &b = time_entry_Alloc();
    a.time = algo::SchedTime(5);
    b.time = algo::SchedTime(3);
    bh_time_entry_recur_Insert(a);
    vrfy_(algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    bh_time_entry_recur_Insert(b);
    vrfy_(bh_time_entry_recur_First() == &b);
    vrfy_(algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    // RemoveFirst leaves a, so the hook stays
    vrfy_(bh_time_entry_recur_RemoveFirst() == &b);
    vrfy_(algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    // Remove empties the heap, so the hook goes
    bh_time_entry_recur_Remove(a);
    vrfy_(!algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    // RemoveAll empties it too
    bh_time_entry_recur_Insert(a);
    bh_time_entry_recur_Insert(b);
    vrfy_(algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    bh_time_entry_recur_RemoveAll();
    vrfy_(!algo_lib::bh_timehook_InBheapQ(_db.th_bh_time_entry_recur));
    vrfy_(!bh_time_entry_recur_InBheapQ(a));
    vrfy_(!bh_time_entry_recur_InBheapQ(b));
    time_entry_Delete(a);
    time_entry_Delete(b);
}

// -----------------------------------------------------------------------------

// Exercise the memory of a Bheap with a head: Reserve makes room without changing the
// rows, RemoveAll empties the heap and keeps the array, Dealloc frees it, and
// the heap works again after both.  The unordered cursor visits every row
// once, in the run and below it.
void atf_amc::amctest_BheapHeadMemory() {
    ReserveElem(100);
    bh_heap_elem_head_Reserve(1000);
    i32 max = _db.bh_heap_elem_head_max;
    vrfy_(max >= 32 + 1000);
    frep_(round, 2) {
        frep_(i, 100) {
            atf_amc::FHeapElem &elem = c_heap_elem_qFind(i);
            elem.key = u64((i * 37) % 100);
            bh_heap_elem_head_Insert(elem);
        }
        vrfyeq_(_db.bh_heap_elem_head_max, max);
        i32 n = 0;
        ind_beg(atf_amc::_db_bh_heap_elem_head_unordcurs, elem, atf_amc::_db) {
            vrfy_(bh_heap_elem_head_InBheapQ(elem));
            n++;
        }ind_end;
        vrfyeq_(n, 100);
        vrfyeq_(bh_heap_elem_head_First()->key, u64(0));
        if (round == 0) {
            bh_heap_elem_head_RemoveAll();
            vrfyeq_(_db.bh_heap_elem_head_max, max);
        } else {
            bh_heap_elem_head_Dealloc();
            vrfyeq_(_db.bh_heap_elem_head_max, 0);
            vrfy_(_db.bh_heap_elem_head_elems == NULL);
        }
        vrfy_(bh_heap_elem_head_EmptyQ());
        frep_(i, 100) {
            vrfy_(!bh_heap_elem_head_InBheapQ(c_heap_elem_qFind(i)));
        }
    }
    // the heap grows again from nothing
    frep_(i, 100) {
        bh_heap_elem_head_Insert(c_heap_elem_qFind(i));
    }
    vrfyeq_(bh_heap_elem_head_N(), 100);
    CheckCursor();
}

// -----------------------------------------------------------------------------

// Drive an InlineOnce step from a Bheap with a head: each pass of the main loop steps
// every entry whose time has passed, first to last, and leaves the entries
// whose time is still ahead.
void atf_amc::amctest_BheapHeadInlineOnce() {
    atf_amc::FTimeEntry &a = time_entry_Alloc();
    atf_amc::FTimeEntry &b = time_entry_Alloc();
    atf_amc::FTimeEntry &c = time_entry_Alloc();
    a.time = algo::SchedTime(2);
    b.time = algo::SchedTime(1);
    c.time = algo::CurrSchedTime() + algo::ToSchedTime(100.0);
    bh_time_entry_once_Insert(a);
    bh_time_entry_once_Insert(c);
    bh_time_entry_once_Insert(b);
    vrfyeq_(bh_time_entry_once_N(), 3);
    vrfy_(bh_time_entry_once_First() == &b);
    algo_lib::_db.limit = algo::CurrSchedTime() + algo::ToSchedTime(0.05);
    MainLoop();
    // a and b expired and were stepped, which deleted them; c waits
    vrfyeq_(bh_time_entry_once_N(), 1);
    vrfy_(bh_time_entry_once_First() == &c);
    time_entry_Delete(c);
    vrfy_(bh_time_entry_once_EmptyQ());
}

// -----------------------------------------------------------------------------

// Mirror a bool field on a Bheap with a head through dmmeta.fcond: a record is in the
// heap exactly while its heaped field is true.  Setting the field inserts or
// removes the record, setting it to the value it holds does nothing, and
// deleting a member record unlinks it.  The heap orders its members by prio.
void atf_amc::amctest_BheapHeadFcond() {
    atf_amc::FCondtest *condtest[40];
    frep_(i, 40) {
        condtest[i] = &condtest_Alloc();
        condtest[i]->prio = u32((i * 7) % 40);
        vrfy_(condtest_XrefMaybe(*condtest[i]));
        vrfy_(!bh_condtest_heaped_InBheapQ(*condtest[i]));
    }
    frep_(i, 40) {
        heaped_Set(*condtest[i], true);
        heaped_Set(*condtest[i], true);
    }
    vrfyeq_(bh_condtest_heaped_N(), 40);
    vrfyeq_(bh_condtest_heaped_First()->prio, u32(0));
    // prio is even exactly when i is even, since 7 and 40 are coprime and 40 is even
    frep_(i, 40) {
        if (i % 2 == 0) {
            heaped_Set(*condtest[i], false);
            vrfy_(!bh_condtest_heaped_InBheapQ(*condtest[i]));
        }
    }
    vrfyeq_(bh_condtest_heaped_N(), 20);
    vrfyeq_(bh_condtest_heaped_First()->prio, u32(1));
    // deleting a member unlinks it
    atf_amc::FCondtest *first = bh_condtest_heaped_First();
    condtest_Delete(*first);
    vrfyeq_(bh_condtest_heaped_N(), 19);
    vrfyeq_(bh_condtest_heaped_First()->prio, u32(3));
    i32 n = 0;
    u32 prev = 0;
    ind_beg(atf_amc::_db_bh_condtest_heaped_curs, elem, atf_amc::_db) {
        vrfy_(elem.heaped);
        vrfy_(n == 0 || prev < elem.prio);
        prev = elem.prio;
        n++;
    }ind_end;
    vrfyeq_(n, 19);
    frep_(i, 40) {
        if (condtest[i] != first) {
            condtest_Delete(*condtest[i]);
        }
    }
    vrfy_(bh_condtest_heaped_EmptyQ());
}

// -----------------------------------------------------------------------------

// Update a row of a table read with update:Y when a Bheap with a head orders
// the table by the value the update changes.  The update takes the row out of
// the heap before the copy and puts it back after it, so the heap orders the
// row by its new value: lowering the value brings the row to the front,
// raising it sends the row back.
void atf_amc::amctest_BheapHeadUpdateMaybe() {
    atf_amc::TypeU in;
    frep_(i, 10) {
        in.u = i + 1;
        in.v = 100 - 10 * (i + 1);
        vrfy_(typeu_UpdateMaybe(in));
    }
    vrfyeq_(bh_typeu_N(), 10);
    vrfyeq_(bh_typeu_First()->u, 10);
    // lower the value of row 3 below every other
    in.u = 3;
    in.v = -5;
    atf_amc::FTypeU *row = typeu_UpdateMaybe(in);
    vrfy_(row);
    vrfyeq_(typeu_N(), 10);
    vrfyeq_(bh_typeu_N(), 10);
    vrfy_(bh_typeu_First() == row);
    // raise it above every other
    in.v = 1000;
    vrfy_(typeu_UpdateMaybe(in) == row);
    vrfyeq_(bh_typeu_First()->u, 10);
    i32 n = 0;
    i32 prev = 0;
    atf_amc::FTypeU *last = NULL;
    ind_beg(atf_amc::_db_bh_typeu_curs, elem, atf_amc::_db) {
        vrfy_(n == 0 || prev <= elem.v);
        prev = elem.v;
        last = &elem;
        n++;
    }ind_end;
    vrfyeq_(n, 10);
    vrfy_(last == row);
}

// =============================================================================
// BENCHMARK

// Give ROW the key value VALUE.  The low 23 bits of a key hold the row's
// number, so no two rows tie: every structure pops the same row, and the
// scenario stays the same scenario for all of them.
static void SetKey(atf_amc::FHeapElem &row, u64 value) {
    row.key = (value << 23) | (row.key & ((u64(1) << 23) - 1));
}

// -----------------------------------------------------------------------------

// Run the benchmark scenario on the structure KIND with N rows and NOP
// operations per workload, and store the cycles spent on each workload in
// CYCLES: build, hold, top, book, depth, reheap, remove, drain.  Add the keys
// that the depth walks and the drain return to CHECKSUM.  The scenario depends
// only on N and NOP, so every structure sees the same keys in the same order.
static void RunScenario(i32 kind, i32 n, i32 nop, u64 *cycles, u64 &checksum) {
    u64 seed = 4;
    u64 base = u64(1) << 38; // keeps values clear of zero when the top workload subtracts
    u64 range = u64(1) << 36;
    u64 gap = range / u64(n); // typical distance between neighboring values
    frep_(i, n) {
        atf_amc::FHeapElem &row = atf_amc::c_heap_elem_qFind(i);
        row.key = u64(i);
        SetKey(row, base + RandBelow(seed, range));
    }
    u64 t0 = algo::get_cycles();
    // build: insert every row
    frep_(i, n) {
        HeapInsert(kind, atf_amc::c_heap_elem_qFind(i));
    }
    u64 t1 = algo::get_cycles();
    // hold: pop the smallest row and push it back with a later key
    frep_(i, nop) {
        atf_amc::FHeapElem *row = HeapRemoveFirst(kind);
        SetKey(*row, (row->key >> 23) + RandBelow(seed, range));
        HeapInsert(kind, *row);
    }
    u64 t2 = algo::get_cycles();
    // top: an order book whose traffic is at the best levels.  Pop the best
    // row and insert it again from two positions better than the new best to
    // eight positions worse.
    frep_(i, nop) {
        atf_amc::FHeapElem *row = HeapRemoveFirst(kind);
        atf_amc::FHeapElem *first = HeapFirst(kind);
        u64 best = (first ? first->key : row->key) >> 23;
        SetKey(*row, best - 2 * gap + RandBelow(seed, 10 * gap));
        HeapInsert(kind, *row);
    }
    u64 t3 = algo::get_cycles();
    // book: an order book whose traffic churns a band of 16 levels that sit
    // below every other row.  Half of the operations pop the best level, the
    // other half remove a random level of the band, as a cancel would, and
    // either way the row comes back with a new key inside the band.  Rows 0..15
    // form the band; moving them into it is not timed.
    u64 band = 16 * gap;
    frep_(i, 16) {
        atf_amc::FHeapElem &row = atf_amc::c_heap_elem_qFind(i % n);
        HeapRemove(kind, row);
        SetKey(row, base - band + RandBelow(seed, band));
        HeapInsert(kind, row);
    }
    u64 t3b = algo::get_cycles();
    frep_(i, nop) {
        atf_amc::FHeapElem *row = NULL;
        if (NextRand(seed) & 1) {
            row = HeapRemoveFirst(kind);
        } else {
            row = &atf_amc::c_heap_elem_qFind(RandBelow(seed, u64(i32_Min(16, n))));
            HeapRemove(kind, *row);
        }
        SetKey(*row, base - band + RandBelow(seed, band));
        HeapInsert(kind, *row);
    }
    u64 t3c = algo::get_cycles();
    // depth: the book workload, plus a walk of the best 10 rows after each
    // operation, as a market-data snapshot would
    frep_(i, nop) {
        atf_amc::FHeapElem *row = NULL;
        if (NextRand(seed) & 1) {
            row = HeapRemoveFirst(kind);
        } else {
            row = &atf_amc::c_heap_elem_qFind(RandBelow(seed, u64(i32_Min(16, n))));
            HeapRemove(kind, *row);
        }
        SetKey(*row, base - band + RandBelow(seed, band));
        HeapInsert(kind, *row);
        checksum += WalkTop(kind, 10);
    }
    u64 t3d = algo::get_cycles();
    // reheap: give a random row a random key, smaller or larger
    frep_(i, nop) {
        atf_amc::FHeapElem &row = RandElem(seed, n);
        SetKey(row, base + RandBelow(seed, range * 2));
        HeapReheap(kind, row);
    }
    u64 t4 = algo::get_cycles();
    // remove: take a random row out and put it back with a random key
    frep_(i, nop) {
        atf_amc::FHeapElem &row = RandElem(seed, n);
        HeapRemove(kind, row);
        SetKey(row, base + RandBelow(seed, range * 2));
        HeapInsert(kind, row);
    }
    u64 t5 = algo::get_cycles();
    // drain: pop every row
    frep_(i, n) {
        checksum += HeapRemoveFirst(kind)->key;
    }
    u64 t6 = algo::get_cycles();
    cycles[0] = t1 - t0;
    cycles[1] = t2 - t1;
    cycles[2] = t3 - t2;
    cycles[3] = t3c - t3b;
    cycles[4] = t3d - t3c;
    cycles[5] = t4 - t3d;
    cycles[6] = t5 - t4;
    cycles[7] = t6 - t5;
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Benchmark the Bheap, the Bheap with a head and the Atree as priority queues, at sizes
// from 16 rows to 4M rows.  Each structure runs the same scenario: build the
// heap, then 500K operations of each workload (hold, top, book, depth, reheap,
// remove), then drain it.  Print one line per size and workload with the
// nanoseconds per operation of each structure.
void atf_amc::amctest_PerfBheapHead() {
    i32 sizes[] = {16, 1024, 65536, 1048576, 4194304};
    i32 nop = 500000;
    const char *opname[] = {"build", "hold", "top", "book", "depth", "reheap", "remove", "drain"};
    double hz = algo::get_cpu_hz_int();
    ReserveElem(sizes[sizeof(sizes) / sizeof(sizes[0]) - 1]);
    frep_(k, i32(sizeof(sizes) / sizeof(sizes[0]))) {
        i32 n = sizes[k];
        u64 cycles[3][8];
        u64 checksum[3] = {0, 0, 0};
        frep_(kind, 3) {
            RunScenario(kind, n, nop, cycles[kind], checksum[kind]);
        }
        vrfyeq_(checksum[0], checksum[1]);
        vrfyeq_(checksum[0], checksum[2]);
        frep_(op, 8) {
            double nper = op == 0 || op == 7 ? double(n) : double(nop);
            prlog("atf_amc.PerfBheapHead"
                  << Keyval("n", n)
                  << Keyval("op", opname[op])
                  << Keyval("bheap_ns", u64(double(cycles[0][op]) / nper / hz * 1e9))
                  << Keyval("head_ns", u64(double(cycles[1][op]) / nper / hz * 1e9))
                  << Keyval("atree_ns", u64(double(cycles[2][op]) / nper / hz * 1e9)));
        }
    }
}
