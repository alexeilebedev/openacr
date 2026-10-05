// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Header: include/atf_amc.h
//

#include "include/algo.h"
#include "include/gen/atf_amc_gen.h"
#include "include/gen/atf_amc_gen.inl.h"

namespace atf_amc { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_amc/atree.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_atree_test1(); // atfdb.amctest:atree_test1
    // void amctest_atree_test2(); // atfdb.amctest:atree_test2
    // void amctest_atree_RangeSearch(); // atfdb.amctest:atree_RangeSearch

    // -------------------------------------------------------------------
    // cpp/atf_amc/bheap.cpp
    //

    //
    // Insert 100 ascending values in bheap
    // - ascending
    // - descending
    // - mixed
    // Check they are inserted and read in the same order
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BheapInsert100(); // atfdb.amctest:BheapInsert100
    // void amctest_BheapCursor(); // atfdb.amctest:BheapCursor
    // void bh_typec_FirstChanged();
    // void amctest_BhFirstChanged1(); // atfdb.amctest:BhFirstChanged1
    // void amctest_BhFirstChanged2(); // atfdb.amctest:BhFirstChanged2
    // void amctest_BhFirstChanged3(); // atfdb.amctest:BhFirstChanged3

    // -------------------------------------------------------------------
    // cpp/atf_amc/bheaphead.cpp
    //

    // OnXref hook for FHeadGroup.bh_heap_elem: record the firing on the row
    //     (user-implemented function, prototype is in amc-generated header)
    // void bh_heap_elem_OnXref(atf_amc::FHeadGroup &parent, atf_amc::FHeapElem &row); // dmmeta.ffunc:atf_amc.FHeadGroup.bh_heap_elem.OnXref

    // OnUnref hook for FHeadGroup.bh_heap_elem: record the firing on the row
    // void bh_heap_elem_OnUnref(atf_amc::FHeadGroup &parent, atf_amc::FHeapElem &row); // dmmeta.ffunc:atf_amc.FHeadGroup.bh_heap_elem.OnUnref

    // Step of FDb.bh_time_entry_head: delete the first entry
    // void bh_time_entry_head_Step(); // dmmeta.fstep:atf_amc.FDb.bh_time_entry_head

    // Step of FDb.bh_time_entry_recur: delete the first entry
    // void bh_time_entry_recur_Step(); // dmmeta.fstep:atf_amc.FDb.bh_time_entry_recur

    // Step of FDb.bh_time_entry_once: delete the first entry
    // void bh_time_entry_once_Step(); // dmmeta.fstep:atf_amc.FDb.bh_time_entry_once

    // Fill the Bheap with a head and its twin with N rows whose keys are drawn below
    // N/2+1, so that keys repeat.  Check the cursors, then drain both and check
    // that they agree at every step.  The sizes cross the boundary of the run, of
    // the root's children and of the first heap levels.
    // void amctest_BheapHeadOrder(); // atfdb.amctest:BheapHeadOrder

    // The Bheap with a head agrees with its twin under random operations, once over a
    // large heap, once over 20 rows, where most rows live in the run, and once
    // over 60 rows, where the run and the heap below it trade rows constantly.
    // void amctest_BheapHeadRandomOp(); // atfdb.amctest:BheapHeadRandomOp

    // Drive a row into the gap between the two largest rows of a full run while the
    // array is exactly full.  Making room demotes the root into the heap, so the
    // row has to go into the run, below the old root; a row sent to the heap as
    // well would take a second new slot that the array does not have.  The insert
    // path places a new row, 1937 between 1936 and 1938 of 63 rows.  The Reheap
    // path moves a heap row of 64, the 2000, to 1935 between 1934 and 1936.
    // void amctest_BheapHeadFullGap(); // atfdb.amctest:BheapHeadFullGap

    // Insert 100 rows in ascending key order, so each lands at or above the root and
    // the run holds the root alone.  A walk of the best 10 with curs reads them in
    // order and leaves the run as it was, and a full walk nests inside it.  A walk
    // with fillcurs reads the same rows and leaves them in the run, so the pops that
    // follow take them in order.
    // void amctest_BheapHeadFillcurs(); // atfdb.amctest:BheapHeadFillcurs

    // Exercise a Bheap with a head owned by a record.  300 rows join it through their xref,
    // and each joining fires OnXref once.  A key change through the generated Set
    // moves a row without firing either callback.  Removing rows fires OnUnref
    // and compacts the pointer array once the rows fill less than a quarter of it.
    // Deleting the group deletes the remaining rows through the cascade.
    // void amctest_BheapHeadGroup(); // atfdb.amctest:BheapHeadGroup

    // Drive a TimeHookOnce step from a Bheap with a head: the step's time hook follows the
    // first row of the heap through inserts, key changes and deletes, as it does
    // for the Bheap of amctest fstep_TimeHookOnce.
    // void amctest_BheapHeadFstep(); // atfdb.amctest:BheapHeadFstep

    // Drive a TimeHookRecur step from a Bheap with a head: the step's time hook is scheduled
    // while the heap holds a row and descheduled when it empties, whichever way
    // the rows leave it.
    // void amctest_BheapHeadTimeHookRecur(); // atfdb.amctest:BheapHeadTimeHookRecur

    // Exercise the memory of a Bheap with a head: Reserve makes room without changing the
    // rows, RemoveAll empties the heap and keeps the array, Dealloc frees it, and
    // the heap works again after both.  The unordered cursor visits every row
    // once, in the run and below it.
    // void amctest_BheapHeadMemory(); // atfdb.amctest:BheapHeadMemory

    // Drive an InlineOnce step from a Bheap with a head: each pass of the main loop steps
    // every entry whose time has passed, first to last, and leaves the entries
    // whose time is still ahead.
    // void amctest_BheapHeadInlineOnce(); // atfdb.amctest:BheapHeadInlineOnce

    // Mirror a bool field on a Bheap with a head through dmmeta.fcond: a record is in the
    // heap exactly while its heaped field is true.  Setting the field inserts or
    // removes the record, setting it to the value it holds does nothing, and
    // deleting a member record unlinks it.  The heap orders its members by prio.
    // void amctest_BheapHeadFcond(); // atfdb.amctest:BheapHeadFcond

    // Update a row of a table read with update:Y when a Bheap with a head orders
    // the table by the value the update changes.  The update takes the row out of
    // the heap before the copy and puts it back after it, so the heap orders the
    // row by its new value: lowering the value brings the row to the front,
    // raising it sends the row back.
    // void amctest_BheapHeadUpdateMaybe(); // atfdb.amctest:BheapHeadUpdateMaybe

    // Benchmark the Bheap, the Bheap with a head and the Atree as priority queues, at sizes
    // from 16 rows to 4M rows.  Each structure runs the same scenario: build the
    // heap, then 500K operations of each workload (hold, top, book, depth, reheap,
    // remove), then drain it.  Print one line per size and workload with the
    // nanoseconds per operation of each structure.
    // void amctest_PerfBheapHead(); // atfdb.amctest:PerfBheapHead

    // -------------------------------------------------------------------
    // cpp/atf_amc/bigend.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BigEndian(); // atfdb.amctest:BigEndian
    // void amctest_BigendFconst(); // atfdb.amctest:BigendFconst

    // -------------------------------------------------------------------
    // cpp/atf_amc/bitfld.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestBitfld(); // atfdb.amctest:TestBitfld
    // void amctest_TestBitfld2(); // atfdb.amctest:TestBitfld2

    // Big-endian bitfield test.
    // Set bits 0..4
    // Set bits 8..12
    // Set bits 0..4 again
    // At each step, check that total field has the expected value
    // void amctest_BitfldNet(); // atfdb.amctest:BitfldNet
    // void amctest_BitfldTuple(); // atfdb.amctest:BitfldTuple

    // Fconst on a bitfld field: the field has no direct member, so the numeric
    // fallback of ReadStrptrMaybe stores through the generated Set
    // void amctest_BitfldFconst(); // atfdb.amctest:BitfldFconst
    // void amctest_BitfldBitset(); // atfdb.amctest:BitfldBitset

    // Bitfld on a global (FDb) ctype: the default value is applied at init,
    // and Get/Set take no parent argument.
    // void amctest_BitfldGlobal(); // atfdb.amctest:BitfldGlobal

    // -------------------------------------------------------------------
    // cpp/atf_amc/bitset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BitsetTary64(); // atfdb.amctest:BitsetTary64
    // void amctest_BitsetInlary16(); // atfdb.amctest:BitsetInlary16
    // void amctest_BitsetVal8(); // atfdb.amctest:BitsetVal8
    // void amctest_BitsetVal64(); // atfdb.amctest:BitsetVal64
    // void amctest_BitsetVal128(); // atfdb.amctest:BitsetVal128
    // void amctest_BitsetBitcurs(); // atfdb.amctest:BitsetBitcurs

    // -------------------------------------------------------------------
    // cpp/atf_amc/blkhash.cpp
    //

    // Insert/Find semantics: exact lookup, idempotent re-insert, duplicate-key
    // rejection, block accounting (one block per (id, 4096-aligned seq region))
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BlkhashInsertMaybe(); // atfdb.amctest:BlkhashInsertMaybe

    // Remove semantics: no-op on non-member, block freed when its last slot
    // clears, freed region usable again, bucket array growth past 4 blocks
    // void amctest_BlkhashRemove(); // atfdb.amctest:BlkhashRemove

    // Cursor visits every member exactly once
    // void amctest_BlkhashCurs(); // atfdb.amctest:BlkhashCurs

    // XrefMaybe inserts into the Blkhash index and reports duplicate keys
    // void amctest_BlkhashXref(); // atfdb.amctest:BlkhashXref

    // Rolling-window benchmark: 10M appends over a 1M-element window with 3
    // near-tail lookups per append, run once against the Thash twin and once
    // against the Blkhash index.  Per-append cost includes one insert, one
    // head lookup + removal (once the window is full), and the 3 lookups.
    // void amctest_PerfBlkhashRolling(); // atfdb.amctest:PerfBlkhashRolling

    // Growth by bucket splits: 5000 streams, one row each, take the index from 4
    // buckets to 5000, across ten 512-bucket segments and four directory doublings.
    // Every row stays reachable, the cursor visits each one once, and the index
    // holds as many buckets as blocks.
    // void amctest_BlkhashSplit(); // atfdb.amctest:BlkhashSplit

    // Growth benchmark: 2M streams, one row each, so every insert makes a block,
    // and the slowest single insert is reported beside the mean.  The index has
    // 2-slot blocks, so 2M blocks fit in memory.  A bucket array that doubles
    // re-chains every block at each power of two, and the slowest insert is that
    // re-chain.
    // void amctest_PerfBlkhashGrow(); // atfdb.amctest:PerfBlkhashGrow

    // Thash growth benchmark: 2M rows inserted one at a time, each insert that
    // grows the bucket array timed and reported with the row count it grew at.  A
    // Thash grows by rehashing every row in one call, so the cost of one growth is
    // what a rule file quotes for a table of that size.
    // void amctest_PerfThashGrow(); // atfdb.amctest:PerfThashGrow

    // -------------------------------------------------------------------
    // cpp/atf_amc/cascdel.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void key_Cleanup(atf_amc::FCascdel &parent); // dmmeta.ffunc:atf_amc.FCascdel.key.Cleanup

    // ptr
    // void amctest_CascdelPtr(); // atfdb.amctest:CascdelPtr

    // ptrary
    // void amctest_CascdelPtrary(); // atfdb.amctest:CascdelPtrary

    // ptrary - chain
    // void amctest_CascdelPtraryChain(); // atfdb.amctest:CascdelPtraryChain

    // ptrary - heaplike
    // void amctest_CascdelPtraryHeap(); // atfdb.amctest:CascdelPtraryHeap

    // ptrary - heaplike, chain: recursion through the delete-until-empty walk
    // void amctest_CascdelPtraryHeapChain(); // atfdb.amctest:CascdelPtraryHeapChain

    // ptrary - heaplike, with a cascade edge between two members of the same
    // array. Deleting c2 (visited first by Cascdel) cascade-deletes its sibling
    // c1, whose removal from the array must observe the array's true state --
    // not a count zeroed up front, which turns the unlink into an elems[-1]
    // read and leaves the count at (u64)-1.
    // void amctest_CascdelPtraryHeapSibling(); // atfdb.amctest:CascdelPtraryHeapSibling

    // thash
    // void amctest_CascdelThash(); // atfdb.amctest:CascdelThash

    // thash - chain
    // void amctest_CascdelThashChain(); // atfdb.amctest:CascdelThashChain

    // bheap
    // void amctest_CascdelBheap(); // atfdb.amctest:CascdelBheap

    // bheap - chain
    // void amctest_CascdelBheapChain(); // atfdb.amctest:CascdelBheapChain

    // zdlist
    // void amctest_CascdelZdlist(); // atfdb.amctest:CascdelZdlist

    // zdlist - chain
    // void amctest_CascdelZdlistChain(); // atfdb.amctest:CascdelZdlistChain

    // atree
    // void amctest_CascdelAtree(); // atfdb.amctest:CascdelAtree

    // atree - chain
    // void amctest_CascdelAtreeChain(); // atfdb.amctest:CascdelAtreeChain

    // -------------------------------------------------------------------
    // cpp/atf_amc/cdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CdlistItemDfltCtor(); // atfdb.amctest:CdlistItemDfltCtor

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_CdlistDfltCtor(); // atfdb.amctest:CdlistDfltCtor

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsert1(); // atfdb.amctest:CdlistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert2(); // atfdb.amctest:CdlistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert3(); // atfdb.amctest:CdlistInsert3

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CdlistRemoveFirst(); // atfdb.amctest:CdlistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CdlistRemove(); // atfdb.amctest:CdlistRemove

    //
    // Flush empty list
    //
    // void amctest_CdlistFlushEmpty(); // atfdb.amctest:CdlistFlushEmpty

    //
    // Flush 100 elements
    //
    // void amctest_CdlistFlush100(); // atfdb.amctest:CdlistFlush100

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_CdlistInsertMaybe(); // atfdb.amctest:CdlistInsertMaybe

    // CDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsertHead1(); // atfdb.amctest:CdlistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead2(); // atfdb.amctest:CdlistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead3(); // atfdb.amctest:CdlistInsertHead3

    // CDLIST - ROTATE FIRST
    // void amctest_CdlistRotateFirst(); // atfdb.amctest:CdlistRotateFirst

    // -------------------------------------------------------------------
    // cpp/atf_amc/cleanup.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // dmmeta.ffunc:atf_amc.AmcCleanup2.field1.Cleanup
    // void field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // dmmeta.ffunc:atf_amc.AmcCleanup2.field2.Cleanup
    // void amctest_CleanupOrder(); // atfdb.amctest:CleanupOrder

    // -------------------------------------------------------------------
    // cpp/atf_amc/cmp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fcmp(); // atfdb.amctest:Fcmp

    // fcmp on a Smallstr field: the field itself is the char array, so the
    // comparison reads it through the field's Getary/Nextchar
    // void amctest_SmallstrFcmp(); // atfdb.amctest:SmallstrFcmp

    // fcmp on a padded Smallstr field: the comparison reads the value through
    // Getary, which excludes the pad bytes, so neither the length tiebreak nor
    // the versionsort digit walk sees the padding
    // void amctest_SmallstrFcmpPad(); // atfdb.amctest:SmallstrFcmpPad

    // Lt for a ctype whose single field is an Upptr: compares the pointer values
    // void amctest_UpptrLtSingleField(); // atfdb.amctest:UpptrLtSingleField

    // Field-level Lt for an fcmp field whose arg type defines Cmp but no Lt
    // (ccmp order:N): the comparison goes through Cmp
    // void amctest_ErrcodeLtField(); // atfdb.amctest:ErrcodeLtField

    // Lt for an ordered ctype whose single field's type defines Cmp but no Lt
    // (ccmp order:N): the comparison goes through Cmp
    // void amctest_ErrcodeLtSingleField(); // atfdb.amctest:ErrcodeLtSingleField

    // -------------------------------------------------------------------
    // cpp/atf_amc/cslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CslistInsertHead1(); // atfdb.amctest:CslistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead2(); // atfdb.amctest:CslistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead3(); // atfdb.amctest:CslistInsertHead3

    // CSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CslistInsert1(); // atfdb.amctest:CslistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert2(); // atfdb.amctest:CslistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert3(); // atfdb.amctest:CslistInsert3

    // CSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CslistRemoveFirst(); // atfdb.amctest:CslistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CslistRemove(); // atfdb.amctest:CslistRemove

    // CSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void cs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_CslistFirstChangedInsert(); // atfdb.amctest:CslistFirstChangedInsert

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_CslistFirstChangedRemoveFirst(); // atfdb.amctest:CslistFirstChangedRemoveFirst

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_CslistFirstChangedRemove(); // atfdb.amctest:CslistFirstChangedRemove

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_CslistFirstChangedFlush(); // atfdb.amctest:CslistFirstChangedFlush

    // CSLIST - HEAD INSERTION - FIRST CHANGED
    // void csl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_CslistHeadFirstChangedInsert(); // atfdb.amctest:CslistHeadFirstChangedInsert

    // CSLIST - ROTATE FIRST
    // void amctest_CslistRotateFirst(); // atfdb.amctest:CslistRotateFirst

    // -------------------------------------------------------------------
    // cpp/atf_amc/delptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Delptr(); // atfdb.amctest:Delptr

    // -------------------------------------------------------------------
    // cpp/atf_amc/dispatch.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_DispRead1(); // atfdb.amctest:DispRead1
    // void amctest_DispRead2(); // atfdb.amctest:DispRead2
    // void amctest_DispRead3(); // atfdb.amctest:DispRead3
    // void amctest_DispRead4(); // atfdb.amctest:DispRead4

    // Check that dispatch read supports both lowercase and uppercase versions
    // void amctest_DispReadSsimfile(); // atfdb.amctest:DispReadSsimfile
    // void amctest_TestDispFilter(); // atfdb.amctest:TestDispFilter
    // void amctest_TestDispFilter2(); // atfdb.amctest:TestDispFilter2
    // void amctest_TestDispFilter3(); // atfdb.amctest:TestDispFilter3
    // void amctest_TestDispFilter4(); // atfdb.amctest:TestDispFilter4

    // -------------------------------------------------------------------
    // cpp/atf_amc/exec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Exec_Status(); // atfdb.amctest:Exec_Status
    // void amctest_ReadProc(); // atfdb.amctest:ReadProc
    // void amctest_ExecSh(); // atfdb.amctest:ExecSh
    // void amctest_ExecVerbose(); // atfdb.amctest:ExecVerbose

    // -------------------------------------------------------------------
    // cpp/atf_amc/fbuf.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_in_msg_Step(); // dmmeta.fstep:atf_amc.FDb.cd_in_msg
    // void amctest_msgbuf_test0(); // atfdb.amctest:msgbuf_test0
    // void amctest_msgbuf_test1(); // atfdb.amctest:msgbuf_test1
    // void amctest_msgbuf_test1_1(); // atfdb.amctest:msgbuf_test1_1
    // void amctest_msgbuf_test1_2(); // atfdb.amctest:msgbuf_test1_2
    // void amctest_msgbuf_test2(); // atfdb.amctest:msgbuf_test2
    // void amctest_msgbuf_test3(); // atfdb.amctest:msgbuf_test3
    // void amctest_msgbuf_test4(); // atfdb.amctest:msgbuf_test4
    // void amctest_msgbuf_test5(); // atfdb.amctest:msgbuf_test5
    // void amctest_msgbuf_test6(); // atfdb.amctest:msgbuf_test6
    // void amctest_msgbuf_test7(); // atfdb.amctest:msgbuf_test7
    // void amctest_msgbuf_test8(); // atfdb.amctest:msgbuf_test8
    // void amctest_msgbuf_test9(); // atfdb.amctest:msgbuf_test9
    // void amctest_msgbuf_test10(); // atfdb.amctest:msgbuf_test10
    // void amctest_msgbuf_extra_test(); // atfdb.amctest:msgbuf_extra_test
    // void amctest_linebuf_test1(); // atfdb.amctest:linebuf_test1
    // void amctest_linebuf_test2(); // atfdb.amctest:linebuf_test2
    // void amctest_linebuf_test3(); // atfdb.amctest:linebuf_test3
    // void amctest_linebuf_test4(); // atfdb.amctest:linebuf_test4
    // void amctest_linebuf_test5(); // atfdb.amctest:linebuf_test5
    // void amctest_bytebuf_test1(); // atfdb.amctest:bytebuf_test1
    // void amctest_bytebuf_test2(); // atfdb.amctest:bytebuf_test2
    // void amctest_bytebuf_dyn_test1(); // atfdb.amctest:bytebuf_dyn_test1

    // custom framer -- 4 bytes at a time
    // void in_custom_ScanMsg(atf_amc::Msgbuf &msgbuf); // dmmeta.ffunc:atf_amc.Msgbuf.in_custom.ScanMsg
    // void amctest_msgbuf_custom(); // atfdb.amctest:msgbuf_custom

    // Read-direction fbuf on a global (FDb) ctype:
    // buffer state and trace counters live on _db, and every
    // generated member access must go through it.
    // void amctest_FbufGlobalRead(); // atfdb.amctest:FbufGlobalRead

    // Write-direction fbuf on a global (FDb) ctype:
    // WriteAll counts written bytes and messages on _db.
    // void amctest_FbufGlobalWrite(); // atfdb.amctest:FbufGlobalWrite

    // Out-direction flow control: the space condition.
    // A producer that keeps writing into a full out buffer has no event to wait
    // for -- it can only re-try and be refused, and a loop that re-judges a
    // condition it cannot bring about is what keeps a process awake for the whole
    // of a congestion episode.  The insight is that the room a producer waits for
    // can appear in exactly one place, the drain, so the drain is what announces
    // it.  An out fbuf therefore latches a congested flag on the write side and
    // arms the space condition once the buffer falls back to its low-water mark.
    // The marks are fractions of the buffer's own capacity: congestion latches at
    // three quarters full, and the wake comes at one quarter.
    // void amctest_FbufSpaceDrain(); // atfdb.amctest:FbufSpaceDrain

    // Discarding a congested out buffer is a drain like any other: the room
    // appears all at once, so the producer parked on the space condition is woken
    // rather than left waiting for a byte-by-byte drain that will never come.
    // void amctest_FbufSpaceRemoveAll(); // atfdb.amctest:FbufSpaceRemoveAll

    // Write-direction fbuf with iotype:openssl:
    // a TLS hard error (here: SSL_write on an SSL object with no connect/accept
    // role, which fails with SSL_ERROR_SSL) must record the error code and
    // unschedule the buffer from the outflow ready list, exactly like a plain
    // write() hard error does; a buffer left on the list would be re-run by the
    // scheduler forever.
    // void amctest_sslbuf_outflow_error(); // atfdb.amctest:sslbuf_outflow_error

    // Write-direction fbuf with iotype:openssl and nothing buffered:
    // a TLS connection is scheduled for outflow as soon as its file descriptor is
    // writable, which on a fresh connection happens before anything has been
    // buffered, so Outflow runs with a byte count of zero. OpenSSL documents
    // SSL_write with num=0 as an error, and its return of zero as a failed write.
    // Outflow must therefore not reach SSL_write at all with an empty buffer: the
    // call would report a failure that did not happen, and an empty buffer's
    // Outflow has nothing to report. The empty buffer is unscheduled, exactly as a
    // fully drained one is.
    // void amctest_sslbuf_outflow_zero(); // atfdb.amctest:sslbuf_outflow_zero

    // Fbuf backed by a private lpool:
    // Realloc allocates the buffer as plain bytes, and Uninit must return it to
    // the pool with that same byte size. The lpool files a freed block on a
    // freelist keyed by the free size, so an inflated size (sizeof(arg)*max)
    // would park the 8K record on the 32K freelist and a later 32K request
    // would be served only 8K of memory.
    // void amctest_fbuf_lpool_free(); // atfdb.amctest:fbuf_lpool_free

    // A datagram buffer walks the whole messages one datagram packed, in order.
    // void amctest_dgrambuf_walk(); // atfdb.amctest:dgrambuf_walk

    // A length the datagram cannot satisfy ends the datagram and raises no eof.
    //
    // This is the property the whole datagram framing exists for.  A stream framer
    // answers an unframeable length by setting eof, which for a datagram socket is
    // a lie -- there is no end of input -- and it strands the bad header at the
    // buffer's start, where it is re-scanned forever.  A datagram buffer reports no
    // message and leaves eof alone, so the next refill drops the bad bytes with the
    // datagram they came in and the interface keeps serving.
    // void amctest_dgrambuf_badlen(); // atfdb.amctest:dgrambuf_badlen

    // A message whose declared length runs past the datagram is not returned, and
    // it too raises no eof: the bytes are a truncated tail, not an error.
    // void amctest_dgrambuf_overrun(); // atfdb.amctest:dgrambuf_overrun

    // -------------------------------------------------------------------
    // cpp/atf_amc/fcond.cpp
    //

    // XrefMaybe performs the initial-membership dispatch: a record whose
    // field holds a watched value at xref time enters the row's list, so a
    // record born in a registered state needs no manual arm.  A value set
    // before XrefMaybe counts the same way (the setter already dispatched;
    // the xref-time insert is an idempotent no-op).
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_FcondInit(); // atfdb.amctest:FcondInit

    // A mirror row (ins:Y del:Y) keeps membership equal to field==value:
    // entering the value inserts, a repeated Set of the same value is a
    // no-op, leaving the value removes, and deleting a member record
    // unlinks it.
    // void amctest_FcondMirror(); // atfdb.amctest:FcondMirror

    // A queue row (ins:Y del:N) inserts on the rising edge only: the falling
    // edge leaves membership alone (the queue's consumer is the remover),
    // and a Set that does not change the level never re-inserts.
    // void amctest_FcondQueue(); // atfdb.amctest:FcondQueue

    // ReadStrptrMaybe stores through the generated Set for every spelling of
    // the value: the symbolic path ('run') and the numeric fallback ('0')
    // both dispatch fcond membership, so how the input spells the value
    // cannot make the record's list state diverge from its field.
    // void amctest_FcondRead(); // atfdb.amctest:FcondRead

    // A via row operates on the pointed-to record: the parent enters the
    // list when the child's field becomes the value, leaves when it stops
    // being the value, and a NULL pointer skips the operation.
    // void amctest_FcondVia(); // atfdb.amctest:FcondVia

    // -------------------------------------------------------------------
    // cpp/atf_amc/fconst.cpp
    //

    // FCONST tests
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fconst(); // atfdb.amctest:Fconst

    // An fconst with a zero-length name is the enum's sentinel member:
    // the empty string converts to it and it prints back as the empty string
    // void amctest_FconstEmptyName(); // atfdb.amctest:FconstEmptyName

    // -------------------------------------------------------------------
    // cpp/atf_amc/fdec.cpp
    //

    // fdec on a plain FDb field: the getter reads the value through _db
    // and the setter writes it through _db.
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_DecGlobal(); // atfdb.amctest:DecGlobal

    // Two fdec fields on one ctype: each field keeps its own accessors with its
    // own scale; the ctype-named GetScale convenience is not generated.
    // void amctest_DecTwoFields(); // atfdb.amctest:DecTwoFields

    // Printing a signed fdec at the underlying type's minimum: the minimum's
    // magnitude has no representation in the type itself, so Print must widen
    // before negating -- an in-type negation wraps, and the wrapped i32/i16
    // value then sign-extends into a garbage u64 magnitude. Pins the exact
    // digits at the i32 and i64 minimum, plus an ordinary negative value.
    // void amctest_DecPrintMin(); // atfdb.amctest:DecPrintMin

    // -------------------------------------------------------------------
    // cpp/atf_amc/fstep.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_fstep_Inline(); // atfdb.amctest:fstep_Inline
    // void amctest_fstep_InlineOnce(); // atfdb.amctest:fstep_InlineOnce
    // void amctest_fstep_InlineRecur(); // atfdb.amctest:fstep_InlineRecur

    // Check that a TimeHookRecur keeps calling its callback, and never faster than
    // the period it was given.
    //
    // How many times it fires in a window measures the platform's sleep as much as
    // it measures this mechanism.  bh_timehook_Step reschedules the hook from the
    // loop's own clock rather than from the deadline it just met, so the period is a
    // floor: a cycle that returns late pushes the next firing out by however late it
    // was, and the rate that results is whatever nanosleep manages.  A tenth of a
    // second at a hundredth-second period fires ten times on Linux and three times
    // on a macOS runner, and neither of those is the mechanism misbehaving.
    //
    // So the window is a whole second, long enough that even a coarse sleep leaves
    // plenty of firings, and the two bounds state what the mechanism does promise.
    // It keeps firing, which a count of five will not reach if recurrence stops
    // after the first call.  And it never outruns its period, which is a hundred
    // firings in the window plus the immediate one at the start.
    // void amctest_fstep_TimeHookRecur(); // atfdb.amctest:fstep_TimeHookRecur
    // void bh_time_entry_Step(); // dmmeta.fstep:atf_amc.FDb.bh_time_entry
    // void amctest_fstep_TimeHookOnce(); // atfdb.amctest:fstep_TimeHookOnce

    // -------------------------------------------------------------------
    // cpp/atf_amc/gsymbol.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Gsymbol(); // atfdb.amctest:Gsymbol

    // -------------------------------------------------------------------
    // cpp/atf_amc/hook.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Hook1(); // atfdb.amctest:Hook1

    // Static hooks with argument
    // amctest itself uses static hook without argument, so we'll assume it's been
    // tested.
    // void listtype_cd(atf_amc::FListtype &listtype); // dmmeta.listtype:cd
    // void listtype_cdl(atf_amc::FListtype &listtype); // dmmeta.listtype:cdl
    // void listtype_cs(atf_amc::FListtype &listtype); // dmmeta.listtype:cs
    // void listtype_csl(atf_amc::FListtype &listtype); // dmmeta.listtype:csl
    // void listtype_zd(atf_amc::FListtype &listtype); // dmmeta.listtype:zd
    // void listtype_zdl(atf_amc::FListtype &listtype); // dmmeta.listtype:zdl
    // void listtype_zs(atf_amc::FListtype &listtype); // dmmeta.listtype:zs
    // void listtype_zsl(atf_amc::FListtype &listtype); // dmmeta.listtype:zsl
    // void amctest_Hook2(); // atfdb.amctest:Hook2

    // -------------------------------------------------------------------
    // cpp/atf_amc/inlary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Inlary_ReadPrint(); // atfdb.amctest:Inlary_ReadPrint

    // Copy of a variable inlary preserves the element count:
    // copy constructor and operator= both route through inlary_Setary
    // void amctest_InlaryCopyCount(); // atfdb.amctest:InlaryCopyCount

    // A variable inlary with min:2 preallocates two elements at Init.
    // Reading a separated string replaces the contents, never drops the count
    // below min, and value-initializes the slots the input does not cover,
    // so the result is a function of the input alone
    // void amctest_InlaryMinRead(); // atfdb.amctest:InlaryMinRead

    // ReadStrptrMaybe into a variable-length char inlary on a global ctype:
    // the string is copied and the length updated on _db
    // void amctest_InlaryCharReadGlobal(); // atfdb.amctest:InlaryCharReadGlobal

    // -------------------------------------------------------------------
    // cpp/atf_amc/lary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LaryFind(); // atfdb.amctest:LaryFind

    // -------------------------------------------------------------------
    // cpp/atf_amc/lineiter.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LineIter(); // atfdb.amctest:LineIter

    // -------------------------------------------------------------------
    // cpp/atf_amc/lpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Lpool(); // atfdb.amctest:Lpool

    // One typed alloc advances the alloc trace counter by exactly one, and one
    // delete advances the del counter by exactly one. The counter has a single
    // owner, the typed alloc layer; a second increment in the underlying
    // AllocMem would double-count every successful alloc.
    // void amctest_LpoolAllocTrace(); // atfdb.amctest:LpoolAllocTrace

    // ReserveBuffers stocks the free store: it allocates NBUF buffers of size
    // BUFSIZE through the ordinary alloc path and frees them all, leaving no live
    // allocation behind, so a subsequent same-size AllocMem is served from the
    // reserved capacity without drawing on the base allocator. For a blk-class
    // size the reserve leaves the class with a dedicated blk holding free space,
    // and the follow-up alloc is served from that blk, touching no raw level; for
    // a level-class size the follow-up alloc pops exactly the class size from the
    // raw free lists, with no refill. Each pin is a delta against captured state,
    // since earlier traffic in the same process may already have populated the
    // pool. A bufsize beyond the largest level cannot be reserved: AllocMem for
    // it returns NULL, and ReserveBuffers must report false, not true.
    // void amctest_LpoolReserveBuffers(); // atfdb.amctest:LpoolReserveBuffers
    // void amctest_LpoolLockMem(); // atfdb.amctest:LpoolLockMem

    // -------------------------------------------------------------------
    // cpp/atf_amc/main.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PrintBase36(); // atfdb.amctest:PrintBase36
    // void amctest_SetGetnumBase10(); // atfdb.amctest:SetGetnumBase10

    // not actually a test but scratch area
    // void amctest_Scratch(); // atfdb.amctest:Scratch
    // void amctest_CastUp(); // atfdb.amctest:CastUp
    // void amctest_CastDown(); // atfdb.amctest:CastDown
    // void amctest_CastDownFail(); // atfdb.amctest:CastDownFail
    // void amctest_CastDownTooShort(); // atfdb.amctest:CastDownTooShort
    // void amctest_CopyOut1(); // atfdb.amctest:CopyOut1
    // void amctest_CopyOut2(); // atfdb.amctest:CopyOut2
    // void amctest_CopyOut3(); // atfdb.amctest:CopyOut3
    // void amctest_TestInsertXref(); // atfdb.amctest:TestInsertXref
    // void amctest_TestInsertX3(); // atfdb.amctest:TestInsertX3

    // A pool whose finput carries update:Y takes an arriving row as a replacement
    // for the one it matches, and the row it replaces has to stay findable by every
    // index that reaches it.
    //
    // The second index is the whole point.  It is keyed by a field the update
    // changes, so the record cannot stay on it across the copy -- it is removed
    // first, and putting it back afterwards is what this pins.  Without that step
    // the record exists, answers to its primary key, and cannot be found by the
    // value it now holds: a keyed row that is updated stops resolving by
    // id, and every permission check that reaches it through the id index misses.
    // void amctest_UpdateMaybe(); // atfdb.amctest:UpdateMaybe
    // void amctest_TestCstring1(); // atfdb.amctest:TestCstring1
    // void amctest_TestCstring2(); // atfdb.amctest:TestCstring2
    // void amctest_TestSep1(); // atfdb.amctest:TestSep1
    // void amctest_TestSep2(); // atfdb.amctest:TestSep2
    // void amctest_TestRegx1(); // atfdb.amctest:TestRegx1
    // void amctest_SubstrDfltval(); // atfdb.amctest:SubstrDfltval
    // void amctest_Minmax(); // atfdb.amctest:Minmax
    // void bh_typec_Step(); // dmmeta.fstep:atf_amc.FDb.bh_typec
    // void zsl_h_typec_Step(); // dmmeta.fstep:atf_amc.FDb.zsl_h_typec
    // void zs_t_typec_Step(); // dmmeta.fstep:atf_amc.FDb.zs_t_typec
    // void csl_h_typec_Step(); // dmmeta.fstep:atf_amc.FDb.csl_h_typec
    // void cs_t_typec_Step(); // dmmeta.fstep:atf_amc.FDb.cs_t_typec
    // void amctest_ImdXref(); // atfdb.amctest:ImdXref
    // void amctest_Typetag(); // atfdb.amctest:Typetag

    // Check that gconst field within tuple is printed as raw
    // void amctest_PrintRawGconst(); // atfdb.amctest:PrintRawGconst
    // void amctest_MsgLength(); // atfdb.amctest:MsgLength

    // Test that lenfld scale attribute works correctly
    // MsgHdrLTScale has: len (u8), scale:4, extra:-2
    // Formula: actual_length = len * scale - extra = len * 4 + 2
    // void amctest_LenfldScale(); // atfdb.amctest:LenfldScale
    void Phase(algo::strptr phase);

    // True when this process runs exactly one of the selected amctest steps, so
    // no earlier test has mutated the db and it still holds the values Init gave
    // it. A test needs this to check a quantity that grows on demand and never
    // shrinks back: a Thash index emptied with RemoveAll keeps the bucket array
    // it grew, so its initial bucket count is only observable in a process where
    // nothing grew it. The forked run (-dofork:Y, the default) gives every test
    // its own process and satisfies this; a single-process run of more than one
    // test does not.
    bool PristineDbQ();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:atf_amc

    // A field default that refers to the record itself (*this) resolves to
    // _db when the field's parent is the global FDb.
    // void amctest_ValGlobalDfltThis(); // atfdb.amctest:ValGlobalDfltThis

    // -------------------------------------------------------------------
    // cpp/atf_amc/msgcurs.cpp
    //

    // Read 2 messages from byteary
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_MsgCurs(); // atfdb.amctest:MsgCurs

    // Byte array too small for message
    // void amctest_MsgCurs2(); // atfdb.amctest:MsgCurs2

    // Message too big for buffer;
    // void amctest_MsgCurs3(); // atfdb.amctest:MsgCurs3

    // Byte array too small for even message header
    // void amctest_MsgCurs4(); // atfdb.amctest:MsgCurs4

    // -------------------------------------------------------------------
    // cpp/atf_amc/numstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Numstr(); // atfdb.amctest:Numstr
    // void amctest_NumstrCorruption(); // atfdb.amctest:NumstrCorruption

    // Signed numstr with min_len>1: digits are zero-padded to min_len,
    // then '-' is prepended; canonical strings roundtrip exactly
    // void amctest_NumstrSignedMinLen(); // atfdb.amctest:NumstrSignedMinLen

    // Letter digits stop at the base's last valid digit: base 16 accepts a-f,
    // base 36 accepts a-z; the next letter ('g', '{', and the uppercase twins)
    // is invalid and GetnumDflt returns the default
    // void amctest_NumstrLetterDigit(); // atfdb.amctest:NumstrLetterDigit

    // A numstr longer than 64 bits' worth of digits detects u64 overflow in the
    // digit loop: values above 2^64-1 are rejected, the boundary parses exactly
    // void amctest_NumstrOverflowU64(); // atfdb.amctest:NumstrOverflowU64

    // A signed numstr range-checks the parsed magnitude even when the string
    // holds more digits than 64 bits: values beyond the numtype range are
    // rejected instead of wrapping through the i64 cast, and the negative
    // bound is one larger than the positive one
    // void amctest_NumstrOverflowSigned(); // atfdb.amctest:NumstrOverflowSigned

    // Geti64 fails when the stored value does not fit in i64: a u64 numstr
    // holding a value above i64max would otherwise wrap through the plain
    // cast and return a silently wrong negative number with out_ok true
    // void amctest_NumstrGeti64Range(); // atfdb.amctest:NumstrGeti64Range

    // SetnumMaybe returns false for a value outside the numtype range and
    // leaves the string unchanged, for every numtype width and sign.  The
    // string-length gate cannot stand in for the range gate: the base-10
    // fast path formats through a u32-parameter FmtBuf, whose mod-2^32
    // digit string can be short enough to pass it
    // void amctest_NumstrSetnumRange(); // atfdb.amctest:NumstrSetnumRange

    // SetnumMaybe negates a negative value in u64 space: i64min, whose
    // magnitude has no i64 representation, formats and round-trips exactly
    // void amctest_NumstrSetnumI64Min(); // atfdb.amctest:NumstrSetnumI64Min

    // min_len padding writes the base's zero digit -- ' ' in base 95, NUL in
    // base 256; the character '0' is a nonzero digit in those bases (16 and 48)
    // and padding with it would change the stored value
    // void amctest_NumstrPadHighBase(); // atfdb.amctest:NumstrPadHighBase

    // base-256 digits are unsigned bytes -- a byte >= 0x80 is a large digit,
    // not a negative one; base 95 accepts exactly the printable range ' '..'~'
    // and rejects every other character
    // void amctest_NumstrDigitHighBase(); // atfdb.amctest:NumstrDigitHighBase

    // -------------------------------------------------------------------
    // cpp/atf_amc/opt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_OptG(); // atfdb.amctest:OptG
    // void amctest_OptG2(); // atfdb.amctest:OptG2
    // void amctest_OptOptG3(); // atfdb.amctest:OptOptG3
    // void amctest_OptOptG4(); // atfdb.amctest:OptOptG4
    // void amctest_OptOptG5(); // atfdb.amctest:OptOptG5
    // void amctest_OptOptG6(); // atfdb.amctest:OptOptG6
    // void amctest_OptOptG7(); // atfdb.amctest:OptOptG7
    // void amctest_OptG8(); // atfdb.amctest:OptG8
    // void amctest_OptOptG8(); // atfdb.amctest:OptOptG8
    // void amctest_OptG9(); // atfdb.amctest:OptG9
    // void amctest_OptOptG9(); // atfdb.amctest:OptOptG9
    // void amctest_OptOptG10(); // atfdb.amctest:OptOptG10
    // void amctest_OptAlloc(); // atfdb.amctest:OptAlloc

    // -------------------------------------------------------------------
    // cpp/atf_amc/pmask.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestPmask1(); // atfdb.amctest:TestPmask1
    // void amctest_TestPmask2(); // atfdb.amctest:TestPmask2
    // void amctest_TestPmask3(); // atfdb.amctest:TestPmask3
    // void amctest_TestPmask4(); // atfdb.amctest:TestPmask4
    // void amctest_TestPmask5(); // atfdb.amctest:TestPmask5
    // void amctest_TestPmask6(); // atfdb.amctest:TestPmask6
    // void amctest_TestPmask7(); // atfdb.amctest:TestPmask7
    // void amctest_TestPmask8(); // atfdb.amctest:TestPmask8
    // void amctest_PmaskMultiple(); // atfdb.amctest:PmaskMultiple

    // Presence tracking on a global (FDb) pmask: the accessors take no parent
    // argument and mark bits directly on _db.
    // void amctest_PmaskGlobal(); // atfdb.amctest:PmaskGlobal

    // -------------------------------------------------------------------
    // cpp/atf_amc/ptrary.cpp
    //

    // A unique Ptrary's Insert and Remove are idempotent: the row carries the
    // flag saying whether it is a member, so inserting a row that is already in
    // the array and removing one that is not are both no-ops, and neither the
    // element count nor the row's position moves.  Callers rely on that -- it is
    // why the codebase never guards these calls with a membership test -- so a
    // regression would silently double-count members across the whole tree.
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PtraryInsert(); // atfdb.amctest:PtraryInsert
    // void amctest_PtraryCursor(); // atfdb.amctest:PtraryCursor

    // A once-cursor detaches the captured run's membership at Reset: the array
    // empties and every captured element's membership flag clears up front, so a
    // walk that exits early cannot leave a row claiming membership in the emptied
    // array (a stale flag would turn the row's next Insert into a silent no-op).
    // The run itself stays in the parent's buffer, which the cursor aliases
    // without copying, so inserting into the array during the walk remains
    // forbidden: an insert would overwrite the unread tail of the run.
    // void amctest_PtraryOnceCursEarlyExit(); // atfdb.amctest:PtraryOnceCursEarlyExit

    // Heaplike flavor of the once-cursor contract: after an early-exited walk
    // every captured element's index reads not-in-array, and a row can re-enter
    // the emptied array.
    // void amctest_PtraryOnceCursHeaplike(); // atfdb.amctest:PtraryOnceCursHeaplike
    // void amctest_PtraryHeaplike(); // atfdb.amctest:PtraryHeaplike

    // Reserve(n) must guarantee capacity for n more elements even when n
    // exceeds the doubled current capacity. The request is computed relative
    // to whatever capacity earlier in-process tests left behind, so the test
    // needs no pristine global state.
    // void amctest_PtraryReserve(); // atfdb.amctest:PtraryReserve

    // OnUnref hook for the non-unique Ptrary c_typem: record the firing on the row
    // void c_typem_OnUnref(atf_amc::FTypeM &row); // dmmeta.ffunc:atf_amc.FDb.c_typem.OnUnref

    // OnXref hook for the non-unique Ptrary c_typem: record the firing on the row
    // void c_typem_OnXref(atf_amc::FTypeM &row); // dmmeta.ffunc:atf_amc.FDb.c_typem.OnXref

    // Both insert paths of a Ptrary fire OnXref when the row enters the array:
    // Insert unconditionally appends and fires; ScanInsertMaybe fires only when
    // the scan found no duplicate and the row was actually inserted.
    // void amctest_PtraryScanInsertOnXref(); // atfdb.amctest:PtraryScanInsertOnXref

    // Remove on a non-unique Ptrary compacts away every occurrence of the row;
    // once none remain, the row is no longer referenced by the array, so OnUnref
    // must fire -- exactly once per Remove that removed something, matching the
    // heaplike and unique flavors. A Remove of a row that is not in the array
    // fires nothing.
    // void amctest_PtraryNonUniqueOnUnref(); // atfdb.amctest:PtraryNonUniqueOnUnref
    // void amctest_PtraryNonUnique(); // atfdb.amctest:PtraryNonUnique

    // -------------------------------------------------------------------
    // cpp/atf_amc/readstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ReadTuple1(); // atfdb.amctest:ReadTuple1
    // void amctest_ReadTuple2(); // atfdb.amctest:ReadTuple2

    // An attribute naming no field is extra information: the read succeeds and the
    // fields the tuple does name land.  With algo_lib::_db.strict_attr set the
    // attribute is an error naming itself, and acr.rowid is never one.  A positional
    // past the last anonymous field is refused under either setting.
    // void amctest_ReadTupleUnknownAttr(); // atfdb.amctest:ReadTupleUnknownAttr
    // void amctest_ReadTuple2a(); // atfdb.amctest:ReadTuple2a
    // void amctest_ReadTuple3(); // atfdb.amctest:ReadTuple3
    // void amctest_ReadTuple4(); // atfdb.amctest:ReadTuple4
    // void amctest_ReadTuple5(); // atfdb.amctest:ReadTuple5

    // -------------------------------------------------------------------
    // cpp/atf_amc/sbrk.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_SbrkMmapTrace(); // atfdb.amctest:SbrkMmapTrace

    // A block too big to fit under the huge-page ceiling leaves the ceiling in place.
    //
    // A runtime arms each process with the ceiling its proctype declares -- four
    // gigabytes for every module -- and a module's largest single request is bigger
    // than that: a txn cache provisioned for twelve gigabytes asks for it in one
    // block.  That block is served on ordinary pages, which is the ceiling doing its
    // job.  What must not follow is the process losing huge pages for everything
    // else it allocates, because the pool blocks that follow are two megabytes each
    // and the ceiling has room for two thousand of them.
    //
    // The ceiling is set to four granules and a block of eight requested, so the
    // allocator declines the huge route by arithmetic and never asks the kernel for
    // it.  That keeps the test independent of the machine: a container with no
    // hugetlb pages available answers the same way as a tuned node, because neither
    // is consulted.  A budget left at zero afterwards is what production converges
    // to, so nothing here has to be put back.
    // void amctest_SbrkHugeCeiling(); // atfdb.amctest:SbrkHugeCeiling

    // Big-block benchmark: map one 256 MB block on the ordinary route, the one a
    // process past its huge-page ceiling takes, and report how long the map took
    // and how much of the block transparent huge pages back.  The block is
    // populated before the call returns either way; on a host whose THP mode is
    // madvise, only an advised block gets huge pages, and each huge page is one
    // fault where 4K pages are 512.
    // void amctest_PerfSbrkBig(); // atfdb.amctest:PerfSbrkBig

    // -------------------------------------------------------------------
    // cpp/atf_amc/sort.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_AmcSort(); // atfdb.amctest:AmcSort
    // void amctest_PerfSortString(); // atfdb.amctest:PerfSortString

    // -------------------------------------------------------------------
    // cpp/atf_amc/strconv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestString(); // atfdb.amctest:TestString

    // -------------------------------------------------------------------
    // cpp/atf_amc/tary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TaryInit(); // atfdb.amctest:TaryInit
    // void amctest_TaryInit2(); // atfdb.amctest:TaryInit2
    // void amctest_TaryInit3(); // atfdb.amctest:TaryInit3
    // void amctest_TaryInit4(); // atfdb.amctest:TaryInit4
    // void amctest_TaryReserve(); // atfdb.amctest:TaryReserve
    // void amctest_TaryHash(); // atfdb.amctest:TaryHash
    void Insary(algo::StringAry &ary, const char *rhs[], int at);
    bool Cmpary(algo::StringAry &ary, const char **rhs);
    bool Cmpary(algo::aryptr<cstring> ary, const char **rhs);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TaryInsary(); // atfdb.amctest:TaryInsary

    // An out-of-range insertion index must abort via FatalErrorExit, not silently
    // corrupt memory. ary_Insary's bounds check is under test; running it in a
    // child lets the parent confirm the child exited with the FatalErrorExit
    // status (1) rather than crashing (signal) or returning normally (status 0).
    // void amctest_TaryInsaryBadIndex(); // atfdb.amctest:TaryInsaryBadIndex
    // void amctest_TaryAllocNAt(); // atfdb.amctest:TaryAllocNAt
    // void amctest_TaryRemove(); // atfdb.amctest:TaryRemove

    // Check that in all cases, Remove(i) == RemRegion(i,1)
    // void amctest_TaryRemove2(); // atfdb.amctest:TaryRemove2

    // -------------------------------------------------------------------
    // cpp/atf_amc/thash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ThashEmpty(); // atfdb.amctest:ThashEmpty
    // void amctest_ThashInsertMaybe(); // atfdb.amctest:ThashInsertMaybe
    // void amctest_ThashRemove(); // atfdb.amctest:ThashRemove
    // void amctest_ThashFindRemove(); // atfdb.amctest:ThashFindRemove
    // void amctest_ThashGetOrCreate(); // atfdb.amctest:ThashGetOrCreate
    // void amctest_ThashXref(); // atfdb.amctest:ThashXref

    // THASH DLL
    // void amctest_PerfThashRemove(); // atfdb.amctest:PerfThashRemove
    // void amctest_ThashLinear(); // atfdb.amctest:ThashLinear

    // Test hash with string keys containing binary chars
    // void amctest_ThashStrkey(); // atfdb.amctest:ThashStrkey

    // -------------------------------------------------------------------
    // cpp/atf_amc/trie.cpp
    //

    // Allocate, find and re-allocate values at scattered keys: each key finds its
    // own value, a slot never allocated finds nothing, and a second Alloc returns
    // the value already there.
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TrieFind(); // atfdb.amctest:TrieFind

    // Grow a 4-way trie from one key to a key near the top of u64: the root rises a
    // level at a time, and every value placed before survives each rise.
    // void amctest_TrieGrow(); // atfdb.amctest:TrieGrow

    // Remove ranges from a filled trie: whole leaves inside the range are freed,
    // the edge leaves keep what lies outside it, and a trie emptied by removal
    // frees every node and drops to height 0.  Remove takes the largest key too.
    // void amctest_TrieRemoveRange(); // atfdb.amctest:TrieRemoveRange

    // Walk sparse keys with the cursor: every value is visited once, in ascending
    // key order.  Each value is its own key, so the order can be read off them.
    // void amctest_TrieCurs(); // atfdb.amctest:TrieCurs

    // Fill a trie of strings long enough to live on the heap, and take them out by
    // Remove, RemoveRange and RemoveAll.  Each value is destroyed exactly once,
    // which memcheck confirms: a value left behind leaks, and one destroyed twice
    // frees its buffer twice.
    // void amctest_TrieDtor(); // atfdb.amctest:TrieDtor

    // Compare a Trie with the Tary it replaces, on the operations a stream's record
    // index performs, at 2M keys: the worst single append, a lookup a reader makes
    // behind the tip, the trim of the oldest half, and a random seek.  The numbers
    // are reported and not asserted, since a loaded machine moves them.
    // void amctest_TrieBench(); // atfdb.amctest:TrieBench

    // Place keys with holes of every size, in a 4-way trie so the holes span leaves
    // and levels, and check that NextKey and PrevKey land on the nearest held key
    // from every starting point, and report none past either end.
    // void amctest_TrieNextPrev(); // atfdb.amctest:TrieNextPrev

    // -------------------------------------------------------------------
    // cpp/atf_amc/varlen.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_VarlenExternLength(); // atfdb.amctest:VarlenExternLength
    // void length_Set(atf_amc::VarlenExtern &vl, u32 n); // dmmeta.cppfunc:atf_amc.VarlenExtern.length
    // u32 length_Get(atf_amc::VarlenExtern &vl);
    // void amctest_VarlenAlloc(); // atfdb.amctest:VarlenAlloc

    // Pool alloc must store the exact inverse of the lenfld read formula.
    // VarlenAllocScale's lenfld has scale:4 extra:-4, so the reader computes
    // total = length*4 + 4, and the alloc must store length = (total - 4)/4,
    // i.e. the number of 4-byte varlen words.
    // void amctest_PoolLenfldScale(); // atfdb.amctest:PoolLenfldScale

    // A negative Opt byte count -- passed by the caller, or read off a corrupt
    // payload's own length word -- underallocates the fixed portion (the header
    // stores already overflow the buffer), and the Opt memcpy converts the count
    // to a huge size_t. Construction must refuse the count (NULL) before any
    // buffer space is taken: the caller-passed arm (OptG) and the payload-derived
    // arm (OptOptG, whose length word 0xFFFFFFFC reads as -4).
    // void amctest_PnewOptNegative(); // atfdb.amctest:PnewOptNegative

    // A message constructor over a scaled lenfld: VarlenB counts 4-byte words
    // past the first (scale:4 extra:-4) over a byte-granular payload, so only
    // a total landing on a scale multiple has a representable length word.
    // FmtByteAry with a 4-byte payload round-trips; a 3-byte payload must fail
    // (NULL) before allocation, rather than store a truncated length word the
    // reader would reconstruct short.
    // void amctest_PnewScaleGuard(); // atfdb.amctest:PnewScaleGuard

    // A message constructor whose lenfld extra exceeds the ctype's fixed size:
    // VarlenLow's u8 length word carries the total minus 8 (extra:-8) over a
    // 1-byte fixed part, so a total below 8 has no representable length word --
    // the store would go negative and wrap through the unsigned word, and the
    // reader would reconstruct a frame 256 bytes longer than was written. An
    // 8-byte total (7 payload bytes) round-trips; a 1-byte total (empty payload)
    // must fail (NULL) before allocation.
    // void amctest_PnewLowGuard(); // atfdb.amctest:PnewLowGuard

    // Pnew length arithmetic carries the buffer capacity and the varlen byte
    // count in u64, so neither wraps at 4GiB: a capacity beyond 4GiB is compared
    // at its true value rather than mod 2^32, and a varlen portion beyond 4GiB
    // is compared at its true value rather than framing a truncated message.
    // A total the frame length cannot express is refused outright: every buffer
    // takes its size as an i32 (algo::Alloc and lib_ams::BeginWrite narrow len
    // to int, and the reader reconstructs the total as an i32), so a total above
    // i32 max would allocate the low bits while the payload copy moves the whole
    // count. The first case is the accepted control; the accepted edge itself, a
    // total of exactly i32 max, is pinned in the emitted text (comptest
    // amc.LenfldNarrow) because accepting it would move 2GiB of payload.
    // None of these cases needs real gigabytes: capacity and element count only
    // feed comparisons, and bytes move only after the checks pass.
    // void amctest_PnewWideLen(); // atfdb.amctest:PnewWideLen

    // An Opt element with a scaled lenfld: OptBMsg's optional trailing element
    // is a VarlenB, and reading the message from a string stores the element's
    // length through the element's own lenfld formula. A 4-byte payload
    // round-trips; a 3-byte payload has no representable length word -- the
    // read must fail rather than store a truncated word that makes b_Get
    // reconstruct a short element.
    // void amctest_OptScaleGuard(); // atfdb.amctest:OptScaleGuard

    // Reading a message from its ascii form appends the varlen tail to the buffer
    // and stores the resulting total through the message's length field, and that
    // total is a runtime one: nothing about the tuple bounds it to what the length
    // word represents. Text's word is a u16 counting the total, so a total of
    // 65535 is the largest that round-trips and 65536 stores 0 -- a length word
    // that frames the payload as the next message header. MsgLTScaleV's word is a
    // u8 counting (total - 2) / 4, which adds two more ways to miss: a total that
    // is not 2 above a multiple of 4 truncates in the division, and a total past
    // 1022 exceeds the word. Each accepted edge is read back through the length
    // field to confirm the stored word reconstructs the total; each rejected one
    // must fail the read rather than store a word the reader misinterprets.
    // (A total below the word's low end is the one shape this path cannot reach:
    // no in-tree dispatch-read header subtracts more than its own fixed size,
    // and the low-end term comes from the same expression amctest PnewLowGuard
    // pins.)
    // void amctest_DispReadLenfldGuard(); // atfdb.amctest:DispReadLenfldGuard

    // AllocExtraMaybe takes the varlen byte count as a signed i32, and a caller
    // can arrive at a negative one: InsertMaybe computes the count as the total
    // length minus the fixed size, so a corrupt length word smaller than the
    // fixed size goes negative. An unguarded negative count underallocates the
    // fixed portion, and the extra-bytes memcpy converts it to a huge size_t.
    // The alloc must refuse the count (NULL) like any other alloc failure --
    // on the unscaled path (VarlenAlloc) and on the scaled path
    // (VarlenAllocScale, where -8 passes the multiple-of-scale test).
    // void amctest_PoolAllocExtraNegative(); // atfdb.amctest:PoolAllocExtraNegative

    // InsertMaybe computes the varlen byte count as the inserted value's length
    // word minus the fixed size, and hands that count to the allocator as an
    // i32. A word smaller than the fixed size (a zeroed struct, a corrupt wire
    // message) makes the count negative, and one past 2^31-1 makes it exceed
    // what the allocator's argument holds; both fail as NULL per the function's
    // contract, rather than dying inside the die-on-fail AllocExtra with a
    // diagnostic blaming memory for an input error.  Out-of-memory keeps dying.
    // A word equal to the fixed size is a record with no trailing element and
    // is accepted, as is any word between that and 2^31-1.
    //
    // The length word is a u32, so the accept/reject table runs: 0 and every
    // word below the fixed size reject, the fixed size itself and larger words
    // accept, and words from 0x80000000 up reject. The accepted upper edge --
    // the word that makes the count exactly 2^31-1 minus the fixed size -- is
    // not exercised here, because reaching it means asking the allocator for
    // two gigabytes.
    // void amctest_PoolInsertMaybeBound(); // atfdb.amctest:PoolInsertMaybeBound

    // A length word whose range runs past u32 cannot be expanded to a byte
    // total first: the scaled multiply, and at scale 1 the extra adjustment
    // after the u64->i64 wrap, would overflow inside the very expression meant
    // to keep the corrupt word visible. InsertMaybe therefore bounds the raw
    // word by a generation-time constant -- the largest value whose expanded
    // total still fits the i32 frame-length domain -- and refuses anything
    // above it. OptWide is the unsigned scaled case: a u64 word, scale 2 and
    // extra -8 put that constant at 1073741819. OptSigned is the signed case,
    // where the bound is what rejects a negative stored length, through the
    // u64 conversion in the emitted test; the addon-count test below it never
    // sees the value. Each ctype's smallest frame -- the word whose total is
    // exactly the fixed size -- is the control that still inserts.
    // void amctest_PoolInsertMaybeWideWord(); // atfdb.amctest:PoolInsertMaybeWideWord
    // void amctest_VarlenMsgs(); // atfdb.amctest:VarlenMsgs
    // void amctest_VarlenMsgsPnew(); // atfdb.amctest:VarlenMsgsPnew
    template <typename T> strptr Bytes(T &arg);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Varlen2(); // atfdb.amctest:Varlen2
    bool Arycmp(algo::aryptr<u32> a, algo::aryptr<u32> b);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Varlen2a(); // atfdb.amctest:Varlen2a
    // void amctest_Varlen2m(); // atfdb.amctest:Varlen2m
    // void amctest_Varlen2v(); // atfdb.amctest:Varlen2v

    // A message with several varlen fields carries the end offset of each but the
    // last, and every accessor of a later field is a subtraction of one end from
    // another or from the length.  The ends come off the wire, so the cast refuses
    // a message whose ends do not fit inside it: one past the varlen area, or one
    // behind the end before it.  A well-formed message casts as before.
    // void amctest_CastDownVarlenEnd(); // atfdb.amctest:CastDownVarlenEnd

    // Fixture: VarlenWMsg's header counts total bytes (MsgHeader.length, scale:1),
    // but each VarlenW element counts 4-byte words past the first (scale:4
    // extra:-4). Reading an element from a string must store the element's length
    // through the element's own lenfld formula -- an element of 12 bytes stores
    // length 2, not 12. A raw byte count would make the element cursor stride
    // past the element and misread everything that follows.
    // void amctest_VarlenNestScale(); // atfdb.amctest:VarlenNestScale

    // A varlen element with a scaled lenfld over a byte-granular payload: only
    // a byte count that lands on a scale multiple has a representable length
    // word. VarlenB counts 4-byte words past the first (scale:4 extra:-4), and
    // its payload is a char array, so any payload length is expressible in the
    // input. A 4-byte payload round-trips; a 3-byte payload has no length word
    // that reconstructs it -- the read must fail rather than store a truncated
    // word that makes the element cursor stride into the element's middle.
    // void amctest_VarlenNestScaleGuard(); // atfdb.amctest:VarlenNestScaleGuard

    // A varlen field with a one-letter name: the field name becomes the element
    // ctype's reference name, so the generated readers take a parameter named w;
    // where the parameter is unused, the (void) suppression must still be
    // emitted even though the body contains w inside another token (new()).
    // void amctest_VarlenShortName(); // atfdb.amctest:VarlenShortName

    // Fixture: a wire frame combining fbigend storage, bitfield views (a typefld
    // enum + a lenfld), Base inheritance, a Varlen array of a ctype, and a String.
    // The shape pins three generator obligations:
    // - Base CopyOut/CopyIn reference an fbigend field by its _be member
    // - GetEnum on a bitfield field reads via the _Get accessor, not parent.<f>
    // - the lenfld default (ssizeof) uses the parent arg, not *this, in the
    // free _Init function generated for a bitfield lenfld.
    // void amctest_NetFrameVarlen(); // atfdb.amctest:NetFrameVarlen

    // -------------------------------------------------------------------
    // cpp/atf_amc/zdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZdlistItemDfltCtor(); // atfdb.amctest:ZdlistItemDfltCtor

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_ZdlistDfltCtor(); // atfdb.amctest:ZdlistDfltCtor

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsert1(); // atfdb.amctest:ZdlistInsert1

    //
    // InsertBefore: position-addressed insertion; covers every splice shape --
    // empty list, before First (head), before NULL (tail), interior -- plus the
    // two no-op guards (row already in list; row given as its own anchor)
    //
    // void amctest_ZdlistInsertBefore(); // atfdb.amctest:ZdlistInsertBefore

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert2(); // atfdb.amctest:ZdlistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert3(); // atfdb.amctest:ZdlistInsert3

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZdlistRemoveFirst(); // atfdb.amctest:ZdlistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZdlistRemove(); // atfdb.amctest:ZdlistRemove

    //
    // Flush empty list
    //
    // void amctest_ZdlistFlushEmpty(); // atfdb.amctest:ZdlistFlushEmpty

    //
    // Flush 100 elements
    //
    // void amctest_ZdlistFlush100(); // atfdb.amctest:ZdlistFlush100

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_ZdlistInsertMaybe(); // atfdb.amctest:ZdlistInsertMaybe

    // ZDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHead1(); // atfdb.amctest:ZdlistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead2(); // atfdb.amctest:ZdlistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead3(); // atfdb.amctest:ZdlistInsertHead3

    // ZDLIST - HEAD INSERT - NO TAIL
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail1(); // atfdb.amctest:ZdlistInsertHeadNoTail1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail2(); // atfdb.amctest:ZdlistInsertHeadNoTail2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail3(); // atfdb.amctest:ZdlistInsertHeadNoTail3
    // void amctest_ZdlistDelCurs(); // atfdb.amctest:ZdlistDelCurs

    // -------------------------------------------------------------------
    // cpp/atf_amc/zslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZslistInsertHead1(); // atfdb.amctest:ZslistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead2(); // atfdb.amctest:ZslistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead3(); // atfdb.amctest:ZslistInsertHead3

    // ZSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZslistInsert1(); // atfdb.amctest:ZslistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert2(); // atfdb.amctest:ZslistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert3(); // atfdb.amctest:ZslistInsert3

    // ZSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZslistRemoveFirst(); // atfdb.amctest:ZslistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZslistRemove(); // atfdb.amctest:ZslistRemove

    // ZSLISTMT
    //
    // check the newly created item is not in the list
    //
    // void amctest_ZslistmtItemDfltCtor(); // atfdb.amctest:ZslistmtItemDfltCtor

    //
    // check that newly created list is empty
    //
    // void amctest_ZslistmtDfltCtor(); // atfdb.amctest:ZslistmtDfltCtor

    //
    // add 1 item, and then delete
    //
    // void amctest_Zslistmt1(); // atfdb.amctest:Zslistmt1

    //
    // add 2 items, and then delete
    //
    // void amctest_Zslistmt2(); // atfdb.amctest:Zslistmt2

    //
    // add 3 items, and then delete
    //
    // void amctest_Zslistmt3(); // atfdb.amctest:Zslistmt3

    // ZSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void zs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_ZslistFirstChangedInsert(); // atfdb.amctest:ZslistFirstChangedInsert

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_ZslistFirstChangedRemoveFirst(); // atfdb.amctest:ZslistFirstChangedRemoveFirst

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_ZslistFirstChangedRemove(); // atfdb.amctest:ZslistFirstChangedRemove

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_ZslistFirstChangedFlush(); // atfdb.amctest:ZslistFirstChangedFlush

    // ZSLIST - HEAD INSERTION - FIRST CHANGED
    // void zsl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_ZslistHeadFirstChangedInsert(); // atfdb.amctest:ZslistHeadFirstChangedInsert
}
