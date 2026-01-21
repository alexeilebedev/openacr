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
    // void amctest_BitfldBitset(); // gstatic/atfdb.amctest:BitfldBitset

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
    // cpp/atf_amc/cascdel.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void key_Cleanup(atf_amc::FCascdel &parent); // fcleanup:atf_amc.FCascdel.key

    // ptr
    // void amctest_CascdelPtr(); // gstatic/atfdb.amctest:CascdelPtr

    // ptrary
    // void amctest_CascdelPtrary(); // gstatic/atfdb.amctest:CascdelPtrary

    // ptrary - chain
    // void amctest_CascdelPtraryChain(); // gstatic/atfdb.amctest:CascdelPtraryChain

    // thash
    // void amctest_CascdelThash(); // gstatic/atfdb.amctest:CascdelThash

    // thash - chain
    // void amctest_CascdelThashChain(); // gstatic/atfdb.amctest:CascdelThashChain

    // bheap
    // void amctest_CascdelBheap(); // gstatic/atfdb.amctest:CascdelBheap

    // bheap - chain
    // void amctest_CascdelBheapChain(); // gstatic/atfdb.amctest:CascdelBheapChain

    // zslist
    // void amctest_CascdelZslist(); // gstatic/atfdb.amctest:CascdelZslist

    // zslist - chain
    // void amctest_CascdelZslistChain(); // gstatic/atfdb.amctest:CascdelZslistChain

    // atf_amc::atf_amc::zdlist
    // void amctest_CascdelZdlist(); // gstatic/atfdb.amctest:CascdelZdlist

    // zdlist - chain
    // void amctest_CascdelZdlistChain(); // gstatic/atfdb.amctest:CascdelZdlistChain

    // bheap
    // void amctest_CascdelAtree(); // gstatic/atfdb.amctest:CascdelAtree

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
    // void field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // fcleanup:atf_amc.AmcCleanup2.field1
    // void field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2); // fcleanup:atf_amc.AmcCleanup2.field2
    // void amctest_CleanupOrder(); // gstatic/atfdb.amctest:CleanupOrder

    // -------------------------------------------------------------------
    // cpp/atf_amc/cmp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fcmp(); // gstatic/atfdb.amctest:Fcmp

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
    // void in_custom_ScanMsg(atf_amc::Msgbuf &msgbuf); // fbuf:atf_amc.Msgbuf.in_custom
    // void amctest_msgbuf_custom(); // gstatic/atfdb.amctest:msgbuf_custom

    // -------------------------------------------------------------------
    // cpp/atf_amc/fconst.cpp
    //

    // FCONST tests
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fconst(); // gstatic/atfdb.amctest:Fconst

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
    // void amctest_TestInsertX2(); // gstatic/atfdb.amctest:TestInsertX2
    // void amctest_TestInsertX3(); // gstatic/atfdb.amctest:TestInsertX3
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
    void Phase(algo::strptr phase);
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:atf_amc

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

    // -------------------------------------------------------------------
    // cpp/atf_amc/ptrary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PtraryInsert(); // gstatic/atfdb.amctest:PtraryInsert
    // void amctest_PtraryCursor(); // gstatic/atfdb.amctest:PtraryCursor
    // void amctest_PtraryHeaplike(); // gstatic/atfdb.amctest:PtraryHeaplike
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
    // void amctest_TaryAllocNAt(); // gstatic/atfdb.amctest:TaryAllocNAt

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
