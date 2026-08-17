// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
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
    // void amctest_atree_test1(); // gstatic/atfdb.amctest:atree_test1
    // void amctest_atree_test2(); // gstatic/atfdb.amctest:atree_test2
    // void amctest_atree_RangeSearch(); // gstatic/atfdb.amctest:atree_RangeSearch

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
    // void amctest_BheapInsert100(); // gstatic/atfdb.amctest:BheapInsert100
    // void amctest_BheapCursor(); // gstatic/atfdb.amctest:BheapCursor
    // void bh_typec_FirstChanged();
    // void amctest_BhFirstChanged1(); // gstatic/atfdb.amctest:BhFirstChanged1
    // void amctest_BhFirstChanged2(); // gstatic/atfdb.amctest:BhFirstChanged2
    // void amctest_BhFirstChanged3(); // gstatic/atfdb.amctest:BhFirstChanged3

    // -------------------------------------------------------------------
    // cpp/atf_amc/bigend.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BigEndian(); // gstatic/atfdb.amctest:BigEndian
    // void amctest_BigendFconst(); // gstatic/atfdb.amctest:BigendFconst

    // -------------------------------------------------------------------
    // cpp/atf_amc/bitfld.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestBitfld(); // gstatic/atfdb.amctest:TestBitfld
    // void amctest_TestBitfld2(); // gstatic/atfdb.amctest:TestBitfld2

    // Big-endian bitfield test.
    // Set bits 0..4
    // Set bits 8..12
    // Set bits 0..4 again
    // At each step, check that total field has the expected value
    // void amctest_BitfldNet(); // gstatic/atfdb.amctest:BitfldNet
    // void amctest_BitfldTuple(); // gstatic/atfdb.amctest:BitfldTuple

    // Fconst on a bitfld field: the field has no direct member, so the numeric
    // fallback of ReadStrptrMaybe stores through the generated Set
    // void amctest_BitfldFconst(); // gstatic/atfdb.amctest:BitfldFconst
    // void amctest_BitfldBitset(); // gstatic/atfdb.amctest:BitfldBitset

    // Bitfld on a global (FDb) ctype: the default value is applied at init,
    // and Get/Set take no parent argument.
    // void amctest_BitfldGlobal(); // gstatic/atfdb.amctest:BitfldGlobal

    // -------------------------------------------------------------------
    // cpp/atf_amc/bitset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BitsetTary64(); // gstatic/atfdb.amctest:BitsetTary64
    // void amctest_BitsetInlary16(); // gstatic/atfdb.amctest:BitsetInlary16
    // void amctest_BitsetVal8(); // gstatic/atfdb.amctest:BitsetVal8
    // void amctest_BitsetVal64(); // gstatic/atfdb.amctest:BitsetVal64
    // void amctest_BitsetVal128(); // gstatic/atfdb.amctest:BitsetVal128
    // void amctest_BitsetBitcurs(); // gstatic/atfdb.amctest:BitsetBitcurs

    // -------------------------------------------------------------------
    // cpp/atf_amc/blkhash.cpp
    //

    // Insert/Find semantics: exact lookup, idempotent re-insert, duplicate-key
    // rejection, block accounting (one block per (id, 4096-aligned seq region))
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BlkhashInsertMaybe(); // gstatic/atfdb.amctest:BlkhashInsertMaybe

    // Remove semantics: no-op on non-member, block freed when its last slot
    // clears, freed region usable again, bucket array growth past 4 blocks
    // void amctest_BlkhashRemove(); // gstatic/atfdb.amctest:BlkhashRemove

    // Cursor visits every member exactly once
    // void amctest_BlkhashCurs(); // gstatic/atfdb.amctest:BlkhashCurs

    // XrefMaybe inserts into the Blkhash index and reports duplicate keys
    // void amctest_BlkhashXref(); // gstatic/atfdb.amctest:BlkhashXref

    // Rolling-window benchmark: 10M appends over a 1M-element window with 3
    // near-tail lookups per append, run once against the Thash twin and once
    // against the Blkhash index.  Per-append cost includes one insert, one
    // head lookup + removal (once the window is full), and the 3 lookups.
    // void amctest_PerfBlkhashRolling(); // gstatic/atfdb.amctest:PerfBlkhashRolling

    // -------------------------------------------------------------------
    // cpp/atf_amc/cascdel.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void key_Cleanup(atf_amc::FCascdel &parent); // ffunc:atf_amc.FCascdel.key.Cleanup

    // ptr
    // void amctest_CascdelPtr(); // gstatic/atfdb.amctest:CascdelPtr

    // ptrary
    // void amctest_CascdelPtrary(); // gstatic/atfdb.amctest:CascdelPtrary

    // ptrary - chain
    // void amctest_CascdelPtraryChain(); // gstatic/atfdb.amctest:CascdelPtraryChain

    // ptrary - heaplike
    // void amctest_CascdelPtraryHeap(); // gstatic/atfdb.amctest:CascdelPtraryHeap

    // ptrary - heaplike, chain: recursion through the delete-until-empty walk
    // void amctest_CascdelPtraryHeapChain(); // gstatic/atfdb.amctest:CascdelPtraryHeapChain

    // ptrary - heaplike, with a cascade edge between two members of the same
    // array. Deleting c2 (visited first by Cascdel) cascade-deletes its sibling
    // c1, whose removal from the array must observe the array's true state --
    // not a count zeroed up front, which turns the unlink into an elems[-1]
    // read and leaves the count at (u64)-1.
    // void amctest_CascdelPtraryHeapSibling(); // gstatic/atfdb.amctest:CascdelPtraryHeapSibling

    // thash
    // void amctest_CascdelThash(); // gstatic/atfdb.amctest:CascdelThash

    // thash - chain
    // void amctest_CascdelThashChain(); // gstatic/atfdb.amctest:CascdelThashChain

    // bheap
    // void amctest_CascdelBheap(); // gstatic/atfdb.amctest:CascdelBheap

    // bheap - chain
    // void amctest_CascdelBheapChain(); // gstatic/atfdb.amctest:CascdelBheapChain

    // zdlist
    // void amctest_CascdelZdlist(); // gstatic/atfdb.amctest:CascdelZdlist

    // zdlist - chain
    // void amctest_CascdelZdlistChain(); // gstatic/atfdb.amctest:CascdelZdlistChain

    // atree
    // void amctest_CascdelAtree(); // gstatic/atfdb.amctest:CascdelAtree

    // atree - chain
    // void amctest_CascdelAtreeChain(); // gstatic/atfdb.amctest:CascdelAtreeChain

    // -------------------------------------------------------------------
    // cpp/atf_amc/cdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CdlistItemDfltCtor(); // gstatic/atfdb.amctest:CdlistItemDfltCtor

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_CdlistDfltCtor(); // gstatic/atfdb.amctest:CdlistDfltCtor

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsert1(); // gstatic/atfdb.amctest:CdlistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert2(); // gstatic/atfdb.amctest:CdlistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert3(); // gstatic/atfdb.amctest:CdlistInsert3

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CdlistRemoveFirst(); // gstatic/atfdb.amctest:CdlistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CdlistRemove(); // gstatic/atfdb.amctest:CdlistRemove

    //
    // Flush empty list
    //
    // void amctest_CdlistFlushEmpty(); // gstatic/atfdb.amctest:CdlistFlushEmpty

    //
    // Flush 100 elements
    //
    // void amctest_CdlistFlush100(); // gstatic/atfdb.amctest:CdlistFlush100

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_CdlistInsertMaybe(); // gstatic/atfdb.amctest:CdlistInsertMaybe

    // CDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsertHead1(); // gstatic/atfdb.amctest:CdlistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead2(); // gstatic/atfdb.amctest:CdlistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead3(); // gstatic/atfdb.amctest:CdlistInsertHead3

    // CDLIST - ROTATE FIRST
    // void amctest_CdlistRotateFirst(); // gstatic/atfdb.amctest:CdlistRotateFirst

    // -------------------------------------------------------------------
    // cpp/atf_amc/cleanup.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // ffunc:atf_amc.AmcCleanup2.field1.Cleanup
    // void field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // ffunc:atf_amc.AmcCleanup2.field2.Cleanup
    // void amctest_CleanupOrder(); // gstatic/atfdb.amctest:CleanupOrder

    // -------------------------------------------------------------------
    // cpp/atf_amc/cmp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fcmp(); // gstatic/atfdb.amctest:Fcmp

    // fcmp on a Smallstr field: the field itself is the char array, so the
    // comparison reads it through the field's Getary/Nextchar
    // void amctest_SmallstrFcmp(); // gstatic/atfdb.amctest:SmallstrFcmp

    // fcmp on a padded Smallstr field: the comparison reads the value through
    // Getary, which excludes the pad bytes, so neither the length tiebreak nor
    // the versionsort digit walk sees the padding
    // void amctest_SmallstrFcmpPad(); // gstatic/atfdb.amctest:SmallstrFcmpPad

    // Lt for a ctype whose single field is an Upptr: compares the pointer values
    // void amctest_UpptrLtSingleField(); // gstatic/atfdb.amctest:UpptrLtSingleField

    // Field-level Lt for an fcmp field whose arg type defines Cmp but no Lt
    // (ccmp order:N): the comparison goes through Cmp
    // void amctest_ErrcodeLtField(); // gstatic/atfdb.amctest:ErrcodeLtField

    // Lt for an ordered ctype whose single field's type defines Cmp but no Lt
    // (ccmp order:N): the comparison goes through Cmp
    // void amctest_ErrcodeLtSingleField(); // gstatic/atfdb.amctest:ErrcodeLtSingleField

    // -------------------------------------------------------------------
    // cpp/atf_amc/cslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CslistInsertHead1(); // gstatic/atfdb.amctest:CslistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead2(); // gstatic/atfdb.amctest:CslistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead3(); // gstatic/atfdb.amctest:CslistInsertHead3

    // CSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CslistInsert1(); // gstatic/atfdb.amctest:CslistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert2(); // gstatic/atfdb.amctest:CslistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert3(); // gstatic/atfdb.amctest:CslistInsert3

    // CSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CslistRemoveFirst(); // gstatic/atfdb.amctest:CslistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CslistRemove(); // gstatic/atfdb.amctest:CslistRemove

    // CSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void cs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_CslistFirstChangedInsert(); // gstatic/atfdb.amctest:CslistFirstChangedInsert

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_CslistFirstChangedRemoveFirst(); // gstatic/atfdb.amctest:CslistFirstChangedRemoveFirst

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_CslistFirstChangedRemove(); // gstatic/atfdb.amctest:CslistFirstChangedRemove

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_CslistFirstChangedFlush(); // gstatic/atfdb.amctest:CslistFirstChangedFlush

    // CSLIST - HEAD INSERTION - FIRST CHANGED
    // void csl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_CslistHeadFirstChangedInsert(); // gstatic/atfdb.amctest:CslistHeadFirstChangedInsert

    // CSLIST - ROTATE FIRST
    // void amctest_CslistRotateFirst(); // gstatic/atfdb.amctest:CslistRotateFirst

    // -------------------------------------------------------------------
    // cpp/atf_amc/delptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Delptr(); // gstatic/atfdb.amctest:Delptr

    // -------------------------------------------------------------------
    // cpp/atf_amc/dispatch.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_DispRead1(); // gstatic/atfdb.amctest:DispRead1
    // void amctest_DispRead2(); // gstatic/atfdb.amctest:DispRead2
    // void amctest_DispRead3(); // gstatic/atfdb.amctest:DispRead3
    // void amctest_DispRead4(); // gstatic/atfdb.amctest:DispRead4

    // Check that dispatch read supports both lowercase and uppercase versions
    // void amctest_DispReadSsimfile(); // gstatic/atfdb.amctest:DispReadSsimfile
    // void amctest_TestDispFilter(); // gstatic/atfdb.amctest:TestDispFilter
    // void amctest_TestDispFilter2(); // gstatic/atfdb.amctest:TestDispFilter2
    // void amctest_TestDispFilter3(); // gstatic/atfdb.amctest:TestDispFilter3
    // void amctest_TestDispFilter4(); // gstatic/atfdb.amctest:TestDispFilter4

    // -------------------------------------------------------------------
    // cpp/atf_amc/exec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Exec_Status(); // gstatic/atfdb.amctest:Exec_Status
    // void amctest_ReadProc(); // gstatic/atfdb.amctest:ReadProc
    // void amctest_ExecSh(); // gstatic/atfdb.amctest:ExecSh
    // void amctest_ExecVerbose(); // gstatic/atfdb.amctest:ExecVerbose

    // -------------------------------------------------------------------
    // cpp/atf_amc/fbuf.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_in_msg_Step(); // fstep:atf_amc.FDb.cd_in_msg
    // void amctest_msgbuf_test0(); // gstatic/atfdb.amctest:msgbuf_test0
    // void amctest_msgbuf_test1(); // gstatic/atfdb.amctest:msgbuf_test1
    // void amctest_msgbuf_test1_1(); // gstatic/atfdb.amctest:msgbuf_test1_1
    // void amctest_msgbuf_test1_2(); // gstatic/atfdb.amctest:msgbuf_test1_2
    // void amctest_msgbuf_test2(); // gstatic/atfdb.amctest:msgbuf_test2
    // void amctest_msgbuf_test3(); // gstatic/atfdb.amctest:msgbuf_test3
    // void amctest_msgbuf_test4(); // gstatic/atfdb.amctest:msgbuf_test4
    // void amctest_msgbuf_test5(); // gstatic/atfdb.amctest:msgbuf_test5
    // void amctest_msgbuf_test6(); // gstatic/atfdb.amctest:msgbuf_test6
    // void amctest_msgbuf_test7(); // gstatic/atfdb.amctest:msgbuf_test7
    // void amctest_msgbuf_test8(); // gstatic/atfdb.amctest:msgbuf_test8
    // void amctest_msgbuf_test9(); // gstatic/atfdb.amctest:msgbuf_test9
    // void amctest_msgbuf_test10(); // gstatic/atfdb.amctest:msgbuf_test10
    // void amctest_msgbuf_extra_test(); // gstatic/atfdb.amctest:msgbuf_extra_test
    // void amctest_linebuf_test1(); // gstatic/atfdb.amctest:linebuf_test1
    // void amctest_linebuf_test2(); // gstatic/atfdb.amctest:linebuf_test2
    // void amctest_linebuf_test3(); // gstatic/atfdb.amctest:linebuf_test3
    // void amctest_linebuf_test4(); // gstatic/atfdb.amctest:linebuf_test4
    // void amctest_linebuf_test5(); // gstatic/atfdb.amctest:linebuf_test5
    // void amctest_bytebuf_test1(); // gstatic/atfdb.amctest:bytebuf_test1
    // void amctest_bytebuf_test2(); // gstatic/atfdb.amctest:bytebuf_test2
    // void amctest_bytebuf_dyn_test1(); // gstatic/atfdb.amctest:bytebuf_dyn_test1

    // custom framer -- 4 bytes at a time
    // void in_custom_ScanMsg(atf_amc::Msgbuf &msgbuf); // ffunc:atf_amc.Msgbuf.in_custom.ScanMsg
    // void amctest_msgbuf_custom(); // gstatic/atfdb.amctest:msgbuf_custom

    // Read-direction fbuf on a global (FDb) ctype:
    // buffer state and trace counters live on _db, and every
    // generated member access must go through it.
    // void amctest_FbufGlobalRead(); // gstatic/atfdb.amctest:FbufGlobalRead

    // Write-direction fbuf on a global (FDb) ctype:
    // WriteAll counts written bytes and messages on _db.
    // void amctest_FbufGlobalWrite(); // gstatic/atfdb.amctest:FbufGlobalWrite

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
    // void amctest_FbufSpaceDrain(); // gstatic/atfdb.amctest:FbufSpaceDrain

    // Discarding a congested out buffer is a drain like any other: the room
    // appears all at once, so the producer parked on the space condition is woken
    // rather than left waiting for a byte-by-byte drain that will never come.
    // void amctest_FbufSpaceRemoveAll(); // gstatic/atfdb.amctest:FbufSpaceRemoveAll

    // Write-direction fbuf with iotype:openssl:
    // a TLS hard error (here: SSL_write on an SSL object with no connect/accept
    // role, which fails with SSL_ERROR_SSL) must record the error code and
    // unschedule the buffer from the outflow ready list, exactly like a plain
    // write() hard error does; a buffer left on the list would be re-run by the
    // scheduler forever.
    // void amctest_sslbuf_outflow_error(); // gstatic/atfdb.amctest:sslbuf_outflow_error

    // Write-direction fbuf with iotype:openssl and nothing buffered:
    // a TLS connection is scheduled for outflow as soon as its file descriptor is
    // writable, which on a fresh connection happens before anything has been
    // buffered, so Outflow runs with a byte count of zero. OpenSSL documents
    // SSL_write with num=0 as an error, and its return of zero as a failed write.
    // Outflow must therefore not reach SSL_write at all with an empty buffer: the
    // call would report a failure that did not happen, and an empty buffer's
    // Outflow has nothing to report. The empty buffer is unscheduled, exactly as a
    // fully drained one is.
    // void amctest_sslbuf_outflow_zero(); // gstatic/atfdb.amctest:sslbuf_outflow_zero

    // Fbuf backed by a private lpool:
    // Realloc allocates the buffer as plain bytes, and Uninit must return it to
    // the pool with that same byte size. The lpool files a freed block on a
    // freelist keyed by the free size, so an inflated size (sizeof(arg)*max)
    // would park the 8K record on the 32K freelist and a later 32K request
    // would be served only 8K of memory.
    // void amctest_fbuf_lpool_free(); // gstatic/atfdb.amctest:fbuf_lpool_free

    // -------------------------------------------------------------------
    // cpp/atf_amc/fcond.cpp
    //

    // XrefMaybe performs the initial-membership dispatch: a record whose
    // field holds a watched value at xref time enters the row's list, so a
    // record born in a registered state needs no manual arm.  A value set
    // before XrefMaybe counts the same way (the setter already dispatched;
    // the xref-time insert is an idempotent no-op).
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_FcondInit(); // gstatic/atfdb.amctest:FcondInit

    // A mirror row (ins:Y del:Y) keeps membership equal to field==value:
    // entering the value inserts, a repeated Set of the same value is a
    // no-op, leaving the value removes, and deleting a member record
    // unlinks it.
    // void amctest_FcondMirror(); // gstatic/atfdb.amctest:FcondMirror

    // A queue row (ins:Y del:N) inserts on the rising edge only: the falling
    // edge leaves membership alone (the queue's consumer is the remover),
    // and a Set that does not change the level never re-inserts.
    // void amctest_FcondQueue(); // gstatic/atfdb.amctest:FcondQueue

    // ReadStrptrMaybe stores through the generated Set for every spelling of
    // the value: the symbolic path ('run') and the numeric fallback ('0')
    // both dispatch fcond membership, so how the input spells the value
    // cannot make the record's list state diverge from its field.
    // void amctest_FcondRead(); // gstatic/atfdb.amctest:FcondRead

    // A via row operates on the pointed-to record: the parent enters the
    // list when the child's field becomes the value, leaves when it stops
    // being the value, and a NULL pointer skips the operation.
    // void amctest_FcondVia(); // gstatic/atfdb.amctest:FcondVia

    // -------------------------------------------------------------------
    // cpp/atf_amc/fconst.cpp
    //

    // FCONST tests
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fconst(); // gstatic/atfdb.amctest:Fconst

    // An fconst with a zero-length name is the enum's sentinel member:
    // the empty string converts to it and it prints back as the empty string
    // void amctest_FconstEmptyName(); // gstatic/atfdb.amctest:FconstEmptyName

    // -------------------------------------------------------------------
    // cpp/atf_amc/fdec.cpp
    //

    // fdec on a plain FDb field: the getter reads the value through _db
    // and the setter writes it through _db.
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_DecGlobal(); // gstatic/atfdb.amctest:DecGlobal

    // Two fdec fields on one ctype: each field keeps its own accessors with its
    // own scale; the ctype-named GetScale convenience is not generated.
    // void amctest_DecTwoFields(); // gstatic/atfdb.amctest:DecTwoFields

    // Printing a signed fdec at the underlying type's minimum: the minimum's
    // magnitude has no representation in the type itself, so Print must widen
    // before negating -- an in-type negation wraps, and the wrapped i32/i16
    // value then sign-extends into a garbage u64 magnitude. Pins the exact
    // digits at the i32 and i64 minimum, plus an ordinary negative value.
    // void amctest_DecPrintMin(); // gstatic/atfdb.amctest:DecPrintMin

    // -------------------------------------------------------------------
    // cpp/atf_amc/fstep.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_fstep_Inline(); // gstatic/atfdb.amctest:fstep_Inline
    // void amctest_fstep_InlineOnce(); // gstatic/atfdb.amctest:fstep_InlineOnce
    // void amctest_fstep_InlineRecur(); // gstatic/atfdb.amctest:fstep_InlineRecur
    // void amctest_fstep_TimeHookRecur(); // gstatic/atfdb.amctest:fstep_TimeHookRecur
    // void bh_time_entry_Step(); // fstep:atf_amc.FDb.bh_time_entry
    // void amctest_fstep_TimeHookOnce(); // gstatic/atfdb.amctest:fstep_TimeHookOnce

    // -------------------------------------------------------------------
    // cpp/atf_amc/gsymbol.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Gsymbol(); // gstatic/atfdb.amctest:Gsymbol

    // -------------------------------------------------------------------
    // cpp/atf_amc/hook.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Hook1(); // gstatic/atfdb.amctest:Hook1

    // Static hooks with argument
    // amctest itself uses static hook without argument, so we'll assume it's been
    // tested.
    // void listtype_cd(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:cd
    // void listtype_cdl(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:cdl
    // void listtype_cs(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:cs
    // void listtype_csl(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:csl
    // void listtype_zd(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:zd
    // void listtype_zdl(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:zdl
    // void listtype_zs(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:zs
    // void listtype_zsl(atf_amc::FListtype &listtype); // gstatic/dmmeta.listtype:zsl
    // void amctest_Hook2(); // gstatic/atfdb.amctest:Hook2

    // -------------------------------------------------------------------
    // cpp/atf_amc/inlary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Inlary_ReadPrint(); // gstatic/atfdb.amctest:Inlary_ReadPrint

    // Copy of a variable inlary preserves the element count:
    // copy constructor and operator= both route through inlary_Setary
    // void amctest_InlaryCopyCount(); // gstatic/atfdb.amctest:InlaryCopyCount

    // A variable inlary with min:2 preallocates two elements at Init.
    // Reading a separated string replaces the contents, never drops the count
    // below min, and value-initializes the slots the input does not cover,
    // so the result is a function of the input alone
    // void amctest_InlaryMinRead(); // gstatic/atfdb.amctest:InlaryMinRead

    // ReadStrptrMaybe into a variable-length char inlary on a global ctype:
    // the string is copied and the length updated on _db
    // void amctest_InlaryCharReadGlobal(); // gstatic/atfdb.amctest:InlaryCharReadGlobal

    // -------------------------------------------------------------------
    // cpp/atf_amc/lary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LaryFind(); // gstatic/atfdb.amctest:LaryFind

    // -------------------------------------------------------------------
    // cpp/atf_amc/lineiter.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LineIter(); // gstatic/atfdb.amctest:LineIter

    // -------------------------------------------------------------------
    // cpp/atf_amc/lpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Lpool(); // gstatic/atfdb.amctest:Lpool

    // One typed alloc advances the alloc trace counter by exactly one, and one
    // delete advances the del counter by exactly one. The counter has a single
    // owner, the typed alloc layer; a second increment in the underlying
    // AllocMem would double-count every successful alloc.
    // void amctest_LpoolAllocTrace(); // gstatic/atfdb.amctest:LpoolAllocTrace

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
    // void amctest_LpoolReserveBuffers(); // gstatic/atfdb.amctest:LpoolReserveBuffers
    // void amctest_LpoolLockMem(); // gstatic/atfdb.amctest:LpoolLockMem

    // -------------------------------------------------------------------
    // cpp/atf_amc/main.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PrintBase36(); // gstatic/atfdb.amctest:PrintBase36
    // void amctest_SetGetnumBase10(); // gstatic/atfdb.amctest:SetGetnumBase10

    // not actually a test but scratch area
    // void amctest_Scratch(); // gstatic/atfdb.amctest:Scratch
    // void amctest_CastUp(); // gstatic/atfdb.amctest:CastUp
    // void amctest_CastDown(); // gstatic/atfdb.amctest:CastDown
    // void amctest_CastDownFail(); // gstatic/atfdb.amctest:CastDownFail
    // void amctest_CastDownTooShort(); // gstatic/atfdb.amctest:CastDownTooShort
    // void amctest_CopyOut1(); // gstatic/atfdb.amctest:CopyOut1
    // void amctest_CopyOut2(); // gstatic/atfdb.amctest:CopyOut2
    // void amctest_CopyOut3(); // gstatic/atfdb.amctest:CopyOut3
    // void amctest_TestInsertXref(); // gstatic/atfdb.amctest:TestInsertXref
    // void amctest_TestInsertX3(); // gstatic/atfdb.amctest:TestInsertX3

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
    // void amctest_UpdateMaybe(); // gstatic/atfdb.amctest:UpdateMaybe
    // void amctest_TestCstring1(); // gstatic/atfdb.amctest:TestCstring1
    // void amctest_TestCstring2(); // gstatic/atfdb.amctest:TestCstring2
    // void amctest_TestSep1(); // gstatic/atfdb.amctest:TestSep1
    // void amctest_TestSep2(); // gstatic/atfdb.amctest:TestSep2
    // void amctest_TestRegx1(); // gstatic/atfdb.amctest:TestRegx1
    // void amctest_SubstrDfltval(); // gstatic/atfdb.amctest:SubstrDfltval
    // void amctest_Minmax(); // gstatic/atfdb.amctest:Minmax
    // void bh_typec_Step(); // fstep:atf_amc.FDb.bh_typec
    // void zsl_h_typec_Step(); // fstep:atf_amc.FDb.zsl_h_typec
    // void zs_t_typec_Step(); // fstep:atf_amc.FDb.zs_t_typec
    // void csl_h_typec_Step(); // fstep:atf_amc.FDb.csl_h_typec
    // void cs_t_typec_Step(); // fstep:atf_amc.FDb.cs_t_typec
    // void amctest_ImdXref(); // gstatic/atfdb.amctest:ImdXref
    // void amctest_Typetag(); // gstatic/atfdb.amctest:Typetag

    // Check that gconst field within tuple is printed as raw
    // void amctest_PrintRawGconst(); // gstatic/atfdb.amctest:PrintRawGconst
    // void amctest_MsgLength(); // gstatic/atfdb.amctest:MsgLength

    // Test that lenfld scale attribute works correctly
    // MsgHdrLTScale has: len (u8), scale:4, extra:-2
    // Formula: actual_length = len * scale - extra = len * 4 + 2
    // void amctest_LenfldScale(); // gstatic/atfdb.amctest:LenfldScale
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
    // void Main(); // main:atf_amc

    // A field default that refers to the record itself (*this) resolves to
    // _db when the field's parent is the global FDb.
    // void amctest_ValGlobalDfltThis(); // gstatic/atfdb.amctest:ValGlobalDfltThis

    // -------------------------------------------------------------------
    // cpp/atf_amc/msgcurs.cpp
    //

    // Read 2 messages from byteary
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_MsgCurs(); // gstatic/atfdb.amctest:MsgCurs

    // Byte array too small for message
    // void amctest_MsgCurs2(); // gstatic/atfdb.amctest:MsgCurs2

    // Message too big for buffer;
    // void amctest_MsgCurs3(); // gstatic/atfdb.amctest:MsgCurs3

    // Byte array too small for even message header
    // void amctest_MsgCurs4(); // gstatic/atfdb.amctest:MsgCurs4

    // -------------------------------------------------------------------
    // cpp/atf_amc/numstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Numstr(); // gstatic/atfdb.amctest:Numstr
    // void amctest_NumstrCorruption(); // gstatic/atfdb.amctest:NumstrCorruption

    // Signed numstr with min_len>1: digits are zero-padded to min_len,
    // then '-' is prepended; canonical strings roundtrip exactly
    // void amctest_NumstrSignedMinLen(); // gstatic/atfdb.amctest:NumstrSignedMinLen

    // Letter digits stop at the base's last valid digit: base 16 accepts a-f,
    // base 36 accepts a-z; the next letter ('g', '{', and the uppercase twins)
    // is invalid and GetnumDflt returns the default
    // void amctest_NumstrLetterDigit(); // gstatic/atfdb.amctest:NumstrLetterDigit

    // A numstr longer than 64 bits' worth of digits detects u64 overflow in the
    // digit loop: values above 2^64-1 are rejected, the boundary parses exactly
    // void amctest_NumstrOverflowU64(); // gstatic/atfdb.amctest:NumstrOverflowU64

    // A signed numstr range-checks the parsed magnitude even when the string
    // holds more digits than 64 bits: values beyond the numtype range are
    // rejected instead of wrapping through the i64 cast, and the negative
    // bound is one larger than the positive one
    // void amctest_NumstrOverflowSigned(); // gstatic/atfdb.amctest:NumstrOverflowSigned

    // Geti64 fails when the stored value does not fit in i64: a u64 numstr
    // holding a value above i64max would otherwise wrap through the plain
    // cast and return a silently wrong negative number with out_ok true
    // void amctest_NumstrGeti64Range(); // gstatic/atfdb.amctest:NumstrGeti64Range

    // SetnumMaybe returns false for a value outside the numtype range and
    // leaves the string unchanged, for every numtype width and sign.  The
    // string-length gate cannot stand in for the range gate: the base-10
    // fast path formats through a u32-parameter FmtBuf, whose mod-2^32
    // digit string can be short enough to pass it
    // void amctest_NumstrSetnumRange(); // gstatic/atfdb.amctest:NumstrSetnumRange

    // SetnumMaybe negates a negative value in u64 space: i64min, whose
    // magnitude has no i64 representation, formats and round-trips exactly
    // void amctest_NumstrSetnumI64Min(); // gstatic/atfdb.amctest:NumstrSetnumI64Min

    // min_len padding writes the base's zero digit -- ' ' in base 95, NUL in
    // base 256; the character '0' is a nonzero digit in those bases (16 and 48)
    // and padding with it would change the stored value
    // void amctest_NumstrPadHighBase(); // gstatic/atfdb.amctest:NumstrPadHighBase

    // base-256 digits are unsigned bytes -- a byte >= 0x80 is a large digit,
    // not a negative one; base 95 accepts exactly the printable range ' '..'~'
    // and rejects every other character
    // void amctest_NumstrDigitHighBase(); // gstatic/atfdb.amctest:NumstrDigitHighBase

    // -------------------------------------------------------------------
    // cpp/atf_amc/opt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_OptG(); // gstatic/atfdb.amctest:OptG
    // void amctest_OptG2(); // gstatic/atfdb.amctest:OptG2
    // void amctest_OptOptG3(); // gstatic/atfdb.amctest:OptOptG3
    // void amctest_OptOptG4(); // gstatic/atfdb.amctest:OptOptG4
    // void amctest_OptOptG5(); // gstatic/atfdb.amctest:OptOptG5
    // void amctest_OptOptG6(); // gstatic/atfdb.amctest:OptOptG6
    // void amctest_OptOptG7(); // gstatic/atfdb.amctest:OptOptG7
    // void amctest_OptG8(); // gstatic/atfdb.amctest:OptG8
    // void amctest_OptOptG8(); // gstatic/atfdb.amctest:OptOptG8
    // void amctest_OptG9(); // gstatic/atfdb.amctest:OptG9
    // void amctest_OptOptG9(); // gstatic/atfdb.amctest:OptOptG9
    // void amctest_OptOptG10(); // gstatic/atfdb.amctest:OptOptG10
    // void amctest_OptAlloc(); // gstatic/atfdb.amctest:OptAlloc

    // -------------------------------------------------------------------
    // cpp/atf_amc/pmask.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestPmask1(); // gstatic/atfdb.amctest:TestPmask1
    // void amctest_TestPmask2(); // gstatic/atfdb.amctest:TestPmask2
    // void amctest_TestPmask3(); // gstatic/atfdb.amctest:TestPmask3
    // void amctest_TestPmask4(); // gstatic/atfdb.amctest:TestPmask4
    // void amctest_TestPmask5(); // gstatic/atfdb.amctest:TestPmask5
    // void amctest_TestPmask6(); // gstatic/atfdb.amctest:TestPmask6
    // void amctest_TestPmask7(); // gstatic/atfdb.amctest:TestPmask7
    // void amctest_TestPmask8(); // gstatic/atfdb.amctest:TestPmask8
    // void amctest_PmaskMultiple(); // gstatic/atfdb.amctest:PmaskMultiple

    // Presence tracking on a global (FDb) pmask: the accessors take no parent
    // argument and mark bits directly on _db.
    // void amctest_PmaskGlobal(); // gstatic/atfdb.amctest:PmaskGlobal

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
    // void amctest_PtraryInsert(); // gstatic/atfdb.amctest:PtraryInsert
    // void amctest_PtraryCursor(); // gstatic/atfdb.amctest:PtraryCursor

    // A once-cursor detaches the captured run's membership at Reset: the array
    // empties and every captured element's membership flag clears up front, so a
    // walk that exits early cannot leave a row claiming membership in the emptied
    // array (a stale flag would turn the row's next Insert into a silent no-op).
    // The run itself stays in the parent's buffer, which the cursor aliases
    // without copying, so inserting into the array during the walk remains
    // forbidden: an insert would overwrite the unread tail of the run.
    // void amctest_PtraryOnceCursEarlyExit(); // gstatic/atfdb.amctest:PtraryOnceCursEarlyExit

    // Heaplike flavor of the once-cursor contract: after an early-exited walk
    // every captured element's index reads not-in-array, and a row can re-enter
    // the emptied array.
    // void amctest_PtraryOnceCursHeaplike(); // gstatic/atfdb.amctest:PtraryOnceCursHeaplike
    // void amctest_PtraryHeaplike(); // gstatic/atfdb.amctest:PtraryHeaplike

    // Reserve(n) must guarantee capacity for n more elements even when n
    // exceeds the doubled current capacity. The request is computed relative
    // to whatever capacity earlier in-process tests left behind, so the test
    // needs no pristine global state.
    // void amctest_PtraryReserve(); // gstatic/atfdb.amctest:PtraryReserve

    // OnUnref hook for the non-unique Ptrary c_typem: record the firing on the row
    // void c_typem_OnUnref(atf_amc::FTypeM &row); // ffunc:atf_amc.FDb.c_typem.OnUnref

    // OnXref hook for the non-unique Ptrary c_typem: record the firing on the row
    // void c_typem_OnXref(atf_amc::FTypeM &row); // ffunc:atf_amc.FDb.c_typem.OnXref

    // Both insert paths of a Ptrary fire OnXref when the row enters the array:
    // Insert unconditionally appends and fires; ScanInsertMaybe fires only when
    // the scan found no duplicate and the row was actually inserted.
    // void amctest_PtraryScanInsertOnXref(); // gstatic/atfdb.amctest:PtraryScanInsertOnXref

    // Remove on a non-unique Ptrary compacts away every occurrence of the row;
    // once none remain, the row is no longer referenced by the array, so OnUnref
    // must fire -- exactly once per Remove that removed something, matching the
    // heaplike and unique flavors. A Remove of a row that is not in the array
    // fires nothing.
    // void amctest_PtraryNonUniqueOnUnref(); // gstatic/atfdb.amctest:PtraryNonUniqueOnUnref
    // void amctest_PtraryNonUnique(); // gstatic/atfdb.amctest:PtraryNonUnique

    // -------------------------------------------------------------------
    // cpp/atf_amc/readstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ReadTuple1(); // gstatic/atfdb.amctest:ReadTuple1
    // void amctest_ReadTuple2(); // gstatic/atfdb.amctest:ReadTuple2
    // void amctest_ReadTuple2a(); // gstatic/atfdb.amctest:ReadTuple2a
    // void amctest_ReadTuple3(); // gstatic/atfdb.amctest:ReadTuple3
    // void amctest_ReadTuple4(); // gstatic/atfdb.amctest:ReadTuple4
    // void amctest_ReadTuple5(); // gstatic/atfdb.amctest:ReadTuple5

    // -------------------------------------------------------------------
    // cpp/atf_amc/sbrk.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_SbrkMmapTrace(); // gstatic/atfdb.amctest:SbrkMmapTrace

    // A block too big to fit under the huge-page ceiling leaves the ceiling in place.
    //
    // A runtime arms each process with the ceiling its proctype declares -- four
    // gigabytes for every module -- and a module's largest single request is bigger
    // than that: a receive cache provisioned for twelve gigabytes asks for it in one
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
    // void amctest_SbrkHugeCeiling(); // gstatic/atfdb.amctest:SbrkHugeCeiling

    // -------------------------------------------------------------------
    // cpp/atf_amc/sort.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_AmcSort(); // gstatic/atfdb.amctest:AmcSort
    // void amctest_PerfSortString(); // gstatic/atfdb.amctest:PerfSortString

    // -------------------------------------------------------------------
    // cpp/atf_amc/strconv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestString(); // gstatic/atfdb.amctest:TestString

    // -------------------------------------------------------------------
    // cpp/atf_amc/tary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TaryInit(); // gstatic/atfdb.amctest:TaryInit
    // void amctest_TaryInit2(); // gstatic/atfdb.amctest:TaryInit2
    // void amctest_TaryInit3(); // gstatic/atfdb.amctest:TaryInit3
    // void amctest_TaryInit4(); // gstatic/atfdb.amctest:TaryInit4
    // void amctest_TaryReserve(); // gstatic/atfdb.amctest:TaryReserve
    // void amctest_TaryHash(); // gstatic/atfdb.amctest:TaryHash
    void Insary(algo::StringAry &ary, const char *rhs[], int at);
    bool Cmpary(algo::StringAry &ary, const char **rhs);
    bool Cmpary(algo::aryptr<cstring> ary, const char **rhs);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TaryInsary(); // gstatic/atfdb.amctest:TaryInsary

    // An out-of-range insertion index must abort via FatalErrorExit, not silently
    // corrupt memory. ary_Insary's bounds check is under test; running it in a
    // child lets the parent confirm the child exited with the FatalErrorExit
    // status (1) rather than crashing (signal) or returning normally (status 0).
    // void amctest_TaryInsaryBadIndex(); // gstatic/atfdb.amctest:TaryInsaryBadIndex
    // void amctest_TaryAllocNAt(); // gstatic/atfdb.amctest:TaryAllocNAt
    // void amctest_TaryRemove(); // gstatic/atfdb.amctest:TaryRemove

    // Check that in all cases, Remove(i) == RemRegion(i,1)
    // void amctest_TaryRemove2(); // gstatic/atfdb.amctest:TaryRemove2

    // -------------------------------------------------------------------
    // cpp/atf_amc/thash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ThashEmpty(); // gstatic/atfdb.amctest:ThashEmpty
    // void amctest_ThashInsertMaybe(); // gstatic/atfdb.amctest:ThashInsertMaybe
    // void amctest_ThashRemove(); // gstatic/atfdb.amctest:ThashRemove
    // void amctest_ThashFindRemove(); // gstatic/atfdb.amctest:ThashFindRemove
    // void amctest_ThashGetOrCreate(); // gstatic/atfdb.amctest:ThashGetOrCreate
    // void amctest_ThashXref(); // gstatic/atfdb.amctest:ThashXref

    // THASH DLL
    // void amctest_PerfThashRemove(); // gstatic/atfdb.amctest:PerfThashRemove
    // void amctest_ThashLinear(); // gstatic/atfdb.amctest:ThashLinear

    // Test hash with string keys containing binary chars
    // void amctest_ThashStrkey(); // gstatic/atfdb.amctest:ThashStrkey

    // -------------------------------------------------------------------
    // cpp/atf_amc/varlen.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_VarlenExternLength(); // gstatic/atfdb.amctest:VarlenExternLength
    // void length_Set(atf_amc::VarlenExtern &vl, u32 n); // cppfunc:atf_amc.VarlenExtern.length
    // u32 length_Get(atf_amc::VarlenExtern &vl);
    // void amctest_VarlenAlloc(); // gstatic/atfdb.amctest:VarlenAlloc

    // Pool alloc must store the exact inverse of the lenfld read formula.
    // VarlenAllocScale's lenfld has scale:4 extra:-4, so the reader computes
    // total = length*4 + 4, and the alloc must store length = (total - 4)/4,
    // i.e. the number of 4-byte varlen words.
    // void amctest_PoolLenfldScale(); // gstatic/atfdb.amctest:PoolLenfldScale

    // A negative Opt byte count -- passed by the caller, or read off a corrupt
    // payload's own length word -- underallocates the fixed portion (the header
    // stores already overflow the buffer), and the Opt memcpy converts the count
    // to a huge size_t. Construction must refuse the count (NULL) before any
    // buffer space is taken: the caller-passed arm (OptG) and the payload-derived
    // arm (OptOptG, whose length word 0xFFFFFFFC reads as -4).
    // void amctest_PnewOptNegative(); // gstatic/atfdb.amctest:PnewOptNegative

    // A message constructor over a scaled lenfld: VarlenB counts 4-byte words
    // past the first (scale:4 extra:-4) over a byte-granular payload, so only
    // a total landing on a scale multiple has a representable length word.
    // FmtByteAry with a 4-byte payload round-trips; a 3-byte payload must fail
    // (NULL) before allocation, rather than store a truncated length word the
    // reader would reconstruct short.
    // void amctest_PnewScaleGuard(); // gstatic/atfdb.amctest:PnewScaleGuard

    // A message constructor whose lenfld extra exceeds the ctype's fixed size:
    // VarlenLow's u8 length word carries the total minus 8 (extra:-8) over a
    // 1-byte fixed part, so a total below 8 has no representable length word --
    // the store would go negative and wrap through the unsigned word, and the
    // reader would reconstruct a frame 256 bytes longer than was written. An
    // 8-byte total (7 payload bytes) round-trips; a 1-byte total (empty payload)
    // must fail (NULL) before allocation.
    // void amctest_PnewLowGuard(); // gstatic/atfdb.amctest:PnewLowGuard

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
    // void amctest_PnewWideLen(); // gstatic/atfdb.amctest:PnewWideLen

    // An Opt element with a scaled lenfld: OptBMsg's optional trailing element
    // is a VarlenB, and reading the message from a string stores the element's
    // length through the element's own lenfld formula. A 4-byte payload
    // round-trips; a 3-byte payload has no representable length word -- the
    // read must fail rather than store a truncated word that makes b_Get
    // reconstruct a short element.
    // void amctest_OptScaleGuard(); // gstatic/atfdb.amctest:OptScaleGuard

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
    // void amctest_DispReadLenfldGuard(); // gstatic/atfdb.amctest:DispReadLenfldGuard

    // AllocExtraMaybe takes the varlen byte count as a signed i32, and a caller
    // can arrive at a negative one: InsertMaybe computes the count as the total
    // length minus the fixed size, so a corrupt length word smaller than the
    // fixed size goes negative. An unguarded negative count underallocates the
    // fixed portion, and the extra-bytes memcpy converts it to a huge size_t.
    // The alloc must refuse the count (NULL) like any other alloc failure --
    // on the unscaled path (VarlenAlloc) and on the scaled path
    // (VarlenAllocScale, where -8 passes the multiple-of-scale test).
    // void amctest_PoolAllocExtraNegative(); // gstatic/atfdb.amctest:PoolAllocExtraNegative

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
    // void amctest_PoolInsertMaybeBound(); // gstatic/atfdb.amctest:PoolInsertMaybeBound

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
    // void amctest_PoolInsertMaybeWideWord(); // gstatic/atfdb.amctest:PoolInsertMaybeWideWord
    // void amctest_VarlenMsgs(); // gstatic/atfdb.amctest:VarlenMsgs
    // void amctest_VarlenMsgsPnew(); // gstatic/atfdb.amctest:VarlenMsgsPnew
    template <typename T> strptr Bytes(T &arg);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Varlen2(); // gstatic/atfdb.amctest:Varlen2
    bool Arycmp(algo::aryptr<u32> a, algo::aryptr<u32> b);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Varlen2a(); // gstatic/atfdb.amctest:Varlen2a
    // void amctest_Varlen2m(); // gstatic/atfdb.amctest:Varlen2m
    // void amctest_Varlen2v(); // gstatic/atfdb.amctest:Varlen2v

    // Fixture: VarlenWMsg's header counts total bytes (MsgHeader.length, scale:1),
    // but each VarlenW element counts 4-byte words past the first (scale:4
    // extra:-4). Reading an element from a string must store the element's length
    // through the element's own lenfld formula -- an element of 12 bytes stores
    // length 2, not 12. A raw byte count would make the element cursor stride
    // past the element and misread everything that follows.
    // void amctest_VarlenNestScale(); // gstatic/atfdb.amctest:VarlenNestScale

    // A varlen element with a scaled lenfld over a byte-granular payload: only
    // a byte count that lands on a scale multiple has a representable length
    // word. VarlenB counts 4-byte words past the first (scale:4 extra:-4), and
    // its payload is a char array, so any payload length is expressible in the
    // input. A 4-byte payload round-trips; a 3-byte payload has no length word
    // that reconstructs it -- the read must fail rather than store a truncated
    // word that makes the element cursor stride into the element's middle.
    // void amctest_VarlenNestScaleGuard(); // gstatic/atfdb.amctest:VarlenNestScaleGuard

    // A varlen field with a one-letter name: the field name becomes the element
    // ctype's reference name, so the generated readers take a parameter named w;
    // where the parameter is unused, the (void) suppression must still be
    // emitted even though the body contains w inside another token (new()).
    // void amctest_VarlenShortName(); // gstatic/atfdb.amctest:VarlenShortName

    // Fixture: a wire frame combining fbigend storage, bitfield views (a typefld
    // enum + a lenfld), Base inheritance, a Varlen array of a ctype, and a String.
    // The shape pins three generator obligations:
    // - Base CopyOut/CopyIn reference an fbigend field by its _be member
    // - GetEnum on a bitfield field reads via the _Get accessor, not parent.<f>
    // - the lenfld default (ssizeof) uses the parent arg, not *this, in the
    // free _Init function generated for a bitfield lenfld.
    // void amctest_NetFrameVarlen(); // gstatic/atfdb.amctest:NetFrameVarlen

    // -------------------------------------------------------------------
    // cpp/atf_amc/zdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZdlistItemDfltCtor(); // gstatic/atfdb.amctest:ZdlistItemDfltCtor

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_ZdlistDfltCtor(); // gstatic/atfdb.amctest:ZdlistDfltCtor

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsert1(); // gstatic/atfdb.amctest:ZdlistInsert1

    //
    // InsertBefore: position-addressed insertion; covers every splice shape --
    // empty list, before First (head), before NULL (tail), interior -- plus the
    // two no-op guards (row already in list; row given as its own anchor)
    //
    // void amctest_ZdlistInsertBefore(); // gstatic/atfdb.amctest:ZdlistInsertBefore

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert2(); // gstatic/atfdb.amctest:ZdlistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert3(); // gstatic/atfdb.amctest:ZdlistInsert3

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZdlistRemoveFirst(); // gstatic/atfdb.amctest:ZdlistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZdlistRemove(); // gstatic/atfdb.amctest:ZdlistRemove

    //
    // Flush empty list
    //
    // void amctest_ZdlistFlushEmpty(); // gstatic/atfdb.amctest:ZdlistFlushEmpty

    //
    // Flush 100 elements
    //
    // void amctest_ZdlistFlush100(); // gstatic/atfdb.amctest:ZdlistFlush100

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_ZdlistInsertMaybe(); // gstatic/atfdb.amctest:ZdlistInsertMaybe

    // ZDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHead1(); // gstatic/atfdb.amctest:ZdlistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead2(); // gstatic/atfdb.amctest:ZdlistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead3(); // gstatic/atfdb.amctest:ZdlistInsertHead3

    // ZDLIST - HEAD INSERT - NO TAIL
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail1(); // gstatic/atfdb.amctest:ZdlistInsertHeadNoTail1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail2(); // gstatic/atfdb.amctest:ZdlistInsertHeadNoTail2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail3(); // gstatic/atfdb.amctest:ZdlistInsertHeadNoTail3
    // void amctest_ZdlistDelCurs(); // gstatic/atfdb.amctest:ZdlistDelCurs

    // -------------------------------------------------------------------
    // cpp/atf_amc/zslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZslistInsertHead1(); // gstatic/atfdb.amctest:ZslistInsertHead1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead2(); // gstatic/atfdb.amctest:ZslistInsertHead2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead3(); // gstatic/atfdb.amctest:ZslistInsertHead3

    // ZSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZslistInsert1(); // gstatic/atfdb.amctest:ZslistInsert1

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert2(); // gstatic/atfdb.amctest:ZslistInsert2

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert3(); // gstatic/atfdb.amctest:ZslistInsert3

    // ZSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZslistRemoveFirst(); // gstatic/atfdb.amctest:ZslistRemoveFirst

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZslistRemove(); // gstatic/atfdb.amctest:ZslistRemove

    // ZSLISTMT
    //
    // check the newly created item is not in the list
    //
    // void amctest_ZslistmtItemDfltCtor(); // gstatic/atfdb.amctest:ZslistmtItemDfltCtor

    //
    // check that newly created list is empty
    //
    // void amctest_ZslistmtDfltCtor(); // gstatic/atfdb.amctest:ZslistmtDfltCtor

    //
    // add 1 item, and then delete
    //
    // void amctest_Zslistmt1(); // gstatic/atfdb.amctest:Zslistmt1

    //
    // add 2 items, and then delete
    //
    // void amctest_Zslistmt2(); // gstatic/atfdb.amctest:Zslistmt2

    //
    // add 3 items, and then delete
    //
    // void amctest_Zslistmt3(); // gstatic/atfdb.amctest:Zslistmt3

    // ZSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void zs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_ZslistFirstChangedInsert(); // gstatic/atfdb.amctest:ZslistFirstChangedInsert

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_ZslistFirstChangedRemoveFirst(); // gstatic/atfdb.amctest:ZslistFirstChangedRemoveFirst

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_ZslistFirstChangedRemove(); // gstatic/atfdb.amctest:ZslistFirstChangedRemove

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_ZslistFirstChangedFlush(); // gstatic/atfdb.amctest:ZslistFirstChangedFlush

    // ZSLIST - HEAD INSERTION - FIRST CHANGED
    // void zsl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_ZslistHeadFirstChangedInsert(); // gstatic/atfdb.amctest:ZslistHeadFirstChangedInsert
}
