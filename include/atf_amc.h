// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
    // cpp/atf/amc/atree.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_atree_test1();
    // void amctest_atree_test2();
    // void amctest_atree_RangeSearch();

    // -------------------------------------------------------------------
    // cpp/atf/amc/bheap.cpp
    //

    //
    // Insert 100 ascending values in bheap
    // - ascending
    // - descending
    // - mixed
    // Check they are inserted and read in the same order
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BheapInsert100();
    // void amctest_BheapCursor();
    // void bh_typec_FirstChanged();
    // void amctest_BhFirstChanged1();
    // void amctest_BhFirstChanged2();
    // void amctest_BhFirstChanged3();

    // -------------------------------------------------------------------
    // cpp/atf/amc/bigend.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BigEndian();
    // void amctest_BigendFconst();

    // -------------------------------------------------------------------
    // cpp/atf/amc/bitfld.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestBitfld();
    // void amctest_TestBitfld2();

    // Big-endian bitfield test.
    // Set bits 0..4
    // Set bits 8..12
    // Set bits 0..4 again
    // At each step, check that total field has the expected value
    // void amctest_BitfldNet();
    // void amctest_BitfldTuple();
    // void amctest_BitfldBitset();

    // -------------------------------------------------------------------
    // cpp/atf/amc/bitset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_BitsetTary64();
    // void amctest_BitsetInlary16();
    // void amctest_BitsetVal8();
    // void amctest_BitsetVal64();
    // void amctest_BitsetVal128();
    // void amctest_BitsetBitcurs();

    // -------------------------------------------------------------------
    // cpp/atf/amc/cascdel.cpp
    //
    void key_Cleanup(atf_amc::FCascdel &parent);

    // ptr
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CascdelPtr();

    // ptrary
    // void amctest_CascdelPtrary();

    // ptrary - chain
    // void amctest_CascdelPtraryChain();

    // thash
    // void amctest_CascdelThash();

    // thash - chain
    // void amctest_CascdelThashChain();

    // bheap
    // void amctest_CascdelBheap();

    // bheap - chain
    // void amctest_CascdelBheapChain();

    // zslist
    // void amctest_CascdelZslist();

    // zslist - chain
    // void amctest_CascdelZslistChain();

    // atf_amc::atf_amc::zdlist
    // void amctest_CascdelZdlist();

    // zdlist - chain
    // void amctest_CascdelZdlistChain();

    // bheap
    // void amctest_CascdelAtree();

    // -------------------------------------------------------------------
    // cpp/atf/amc/cdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CdlistItemDfltCtor();

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_CdlistDfltCtor();

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsert1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsert3();

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CdlistRemoveFirst();

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CdlistRemove();

    //
    // Flush empty list
    //
    // void amctest_CdlistFlushEmpty();

    //
    // Flush 100 elements
    //
    // void amctest_CdlistFlush100();

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_CdlistInsertMaybe();

    // CDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CdlistInsertHead1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CdlistInsertHead3();

    // CDLIST - ROTATE FIRST
    // void amctest_CdlistRotateFirst();

    // -------------------------------------------------------------------
    // cpp/atf/amc/cleanup.cpp
    //
    void field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2);
    void field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CleanupOrder();

    // -------------------------------------------------------------------
    // cpp/atf/amc/cmp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fcmp();

    // -------------------------------------------------------------------
    // cpp/atf/amc/cslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_CslistInsertHead1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsertHead3();

    // CSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_CslistInsert1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_CslistInsert3();

    // CSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_CslistRemoveFirst();

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_CslistRemove();

    // CSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void cs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_CslistFirstChangedInsert();

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_CslistFirstChangedRemoveFirst();

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_CslistFirstChangedRemove();

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_CslistFirstChangedFlush();

    // CSLIST - HEAD INSERTION - FIRST CHANGED
    // void csl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_CslistHeadFirstChangedInsert();

    // CSLIST - ROTATE FIRST
    // void amctest_CslistRotateFirst();

    // -------------------------------------------------------------------
    // cpp/atf/amc/delptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Delptr();

    // -------------------------------------------------------------------
    // cpp/atf/amc/dispatch.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_DispRead1();
    // void amctest_DispRead2();
    // void amctest_DispRead3();
    // void amctest_DispRead4();

    // Check that dispatch read supports both lowercase and uppercase versions
    // void amctest_DispReadSsimfile();
    // void amctest_TestDispFilter();
    // void amctest_TestDispFilter2();
    // void amctest_TestDispFilter3();
    // void amctest_TestDispFilter4();

    // -------------------------------------------------------------------
    // cpp/atf/amc/exec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Exec_Status();
    // void amctest_ReadProc();
    // void amctest_ExecSh();
    // void amctest_ExecVerbose();

    // -------------------------------------------------------------------
    // cpp/atf/amc/fbuf.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_in_msg_Step();
    // void amctest_msgbuf_test0();
    // void amctest_msgbuf_test1();
    // void amctest_msgbuf_test1_1();
    // void amctest_msgbuf_test1_2();
    // void amctest_msgbuf_test2();
    // void amctest_msgbuf_test3();
    // void amctest_msgbuf_test4();
    // void amctest_msgbuf_test5();
    // void amctest_msgbuf_test6();
    // void amctest_msgbuf_test7();
    // void amctest_msgbuf_test8();
    // void amctest_msgbuf_test9();
    // void amctest_msgbuf_test10();
    // void amctest_msgbuf_extra_test();
    // void amctest_linebuf_test1();
    // void amctest_linebuf_test2();
    // void amctest_linebuf_test3();
    // void amctest_linebuf_test4();
    // void amctest_linebuf_test5();
    // void amctest_bytebuf_test1();
    // void amctest_bytebuf_test2();
    // void amctest_bytebuf_dyn_test1();

    // -------------------------------------------------------------------
    // cpp/atf/amc/fconst.cpp
    //

    // FCONST tests
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Fconst();

    // -------------------------------------------------------------------
    // cpp/atf/amc/fstep.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_fstep_Inline();
    // void amctest_fstep_InlineOnce();
    // void amctest_fstep_InlineRecur();
    // void amctest_fstep_TimeHookRecur();

    // -------------------------------------------------------------------
    // cpp/atf/amc/gsymbol.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Gsymbol();

    // -------------------------------------------------------------------
    // cpp/atf/amc/hook.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Hook1();
    // void amctest_Hook2();

    // -------------------------------------------------------------------
    // cpp/atf/amc/inlary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Inlary_ReadPrint();

    // -------------------------------------------------------------------
    // cpp/atf/amc/lary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LaryFind();

    // -------------------------------------------------------------------
    // cpp/atf/amc/lineiter.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_LineIter();

    // -------------------------------------------------------------------
    // cpp/atf/amc/lpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Lpool();

    // -------------------------------------------------------------------
    // cpp/atf/amc/main.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PrintBase36();
    // void amctest_SetGetnumBase10();

    // not actually a test but scratch area
    // void amctest_Scratch();
    // void amctest_CastUp();
    // void amctest_CastDown();
    // void amctest_CastDownFail();
    // void amctest_CastDownTooShort();
    // void amctest_CopyOut1();
    // void amctest_CopyOut2();
    // void amctest_CopyOut3();
    // void amctest_TestInsertX2();
    // void amctest_TestInsertX3();
    // void amctest_TestCstring1();
    // void amctest_TestCstring2();
    // void amctest_TestSep1();
    // void amctest_TestSep2();
    // void amctest_TestRegx1();
    // void amctest_SubstrDfltval();
    // void amctest_Minmax();
    // void bh_typec_Step();
    // void zsl_h_typec_Step();
    // void zs_t_typec_Step();
    // void csl_h_typec_Step();
    // void cs_t_typec_Step();
    // void amctest_ImdXref();
    // void amctest_Typetag();

    // Check that gconst field within tuple is printed as raw
    // void amctest_PrintRawGconst();
    // void amctest_MsgLength();
    void Main();

    // -------------------------------------------------------------------
    // cpp/atf/amc/msgcurs.cpp
    //

    // Read 2 messages from byteary
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_MsgCurs();

    // Byte array too small for message
    // void amctest_MsgCurs2();

    // Message too big for buffer;
    // void amctest_MsgCurs3();

    // Byte array too small for even message header
    // void amctest_MsgCurs4();

    // -------------------------------------------------------------------
    // cpp/atf/amc/numstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_Numstr();
    // void amctest_NumstrCorruption();

    // -------------------------------------------------------------------
    // cpp/atf/amc/opt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_OptG();
    // void amctest_OptG2();
    // void amctest_OptOptG3();
    // void amctest_OptOptG4();
    // void amctest_OptOptG5();
    // void amctest_OptOptG6();
    // void amctest_OptOptG7();
    // void amctest_OptG8();
    // void amctest_OptOptG8();
    // void amctest_OptG9();
    // void amctest_OptOptG9();
    // void amctest_OptAlloc();

    // -------------------------------------------------------------------
    // cpp/atf/amc/pmask.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestPmask1();
    // void amctest_TestPmask2();
    // void amctest_TestPmask3();
    // void amctest_TestPmask4();
    // void amctest_TestPmask5();
    // void amctest_TestPmask6();
    // void amctest_TestPmask7();
    // void amctest_TestPmask8();

    // -------------------------------------------------------------------
    // cpp/atf/amc/ptrary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_PtraryInsert();
    // void amctest_PtraryCursor();

    // -------------------------------------------------------------------
    // cpp/atf/amc/readstr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ReadTuple1();
    // void amctest_ReadTuple2();
    // void amctest_ReadTuple3();
    // void amctest_ReadTuple4();
    // void amctest_ReadTuple5();

    // -------------------------------------------------------------------
    // cpp/atf/amc/sort.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_AmcSort();
    // void amctest_PerfSortString();

    // -------------------------------------------------------------------
    // cpp/atf/amc/strconv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TestString();

    // -------------------------------------------------------------------
    // cpp/atf/amc/tary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_TaryInit();
    // void amctest_TaryInit2();
    // void amctest_TaryInit3();
    // void amctest_TaryInit4();
    // void amctest_TaryReserve();

    // -------------------------------------------------------------------
    // cpp/atf/amc/thash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ThashEmpty();
    // void amctest_ThashInsertMaybe();
    // void amctest_ThashRemove();
    // void amctest_ThashFindRemove();
    // void amctest_ThashGetOrCreate();
    // void amctest_ThashXref();

    // THASH DLL
    // void amctest_PerfThashRemove();

    // -------------------------------------------------------------------
    // cpp/atf/amc/varlen.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_VarlenExternLength();
    void length_Set(atf_amc::VarlenExtern &vl, u32 n);
    u32 length_Get(atf_amc::VarlenExtern &vl);
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_VarlenAlloc();
    // void amctest_VarlenMsgs();
    // void amctest_VarlenMsgsPnew();

    // -------------------------------------------------------------------
    // cpp/atf/amc/zdlist.cpp
    //

    //
    // create list item, check if it is not in list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZdlistItemDfltCtor();

    //
    // Create empty list, check if it is really empty
    //
    // void amctest_ZdlistDfltCtor();

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsert1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsert3();

    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZdlistRemoveFirst();

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZdlistRemove();

    //
    // Flush empty list
    //
    // void amctest_ZdlistFlushEmpty();

    //
    // Flush 100 elements
    //
    // void amctest_ZdlistFlush100();

    //
    // InsertMaybe:
    // 1) try insert 1 element, check if inserted
    // 2) try insert the same element, check if not inserted
    // 3) try insert other element, check if inserted
    //
    // void amctest_ZdlistInsertMaybe();

    // ZDLIST - HEAD INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHead1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHead3();

    // ZDLIST - HEAD INSERT - NO TAIL
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZdlistInsertHeadNoTail3();
    // void amctest_ZdlistDelCurs();

    // -------------------------------------------------------------------
    // cpp/atf/amc/zslist.cpp
    //

    //
    // Insert 1 element in the list, check if it is really in the list
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void amctest_ZslistInsertHead1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsertHead3();

    // ZSLIST - TAIL INSERT
    //
    // Insert 1 element in the list, check if it is really in the list
    //
    // void amctest_ZslistInsert1();

    //
    // Insert 2 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert2();

    //
    // Insert 3 elements in the list, check if it they are really in the list
    //
    // void amctest_ZslistInsert3();

    // ZSLIST
    //
    // Insert 100 items to the list, remove first item 100 times
    // Then try on empty list
    //
    // void amctest_ZslistRemoveFirst();

    //
    // Insert 100 elements, Remove them in "random" order
    //
    // void amctest_ZslistRemove();

    // ZSLISTMT
    //
    // check the newly created item is not in the list
    //
    // void amctest_ZslistmtItemDfltCtor();

    //
    // check that newly created list is empty
    //
    // void amctest_ZslistmtDfltCtor();

    //
    // add 1 item, and then delete
    //
    // void amctest_Zslistmt1();

    //
    // add 2 items, and then delete
    //
    // void amctest_Zslistmt2();

    //
    // add 3 items, and then delete
    //
    // void amctest_Zslistmt3();

    // ZSLIST - TAIL INSERTION - FIRST CHANGED
    //
    // callback for trigger
    // void zs_t_typec_FirstChanged();

    //
    // Insert 3 items, check trigger fires only for the first
    //
    // void amctest_ZslistFirstChangedInsert();

    //
    // Insert 3 items
    // RemoveFirst 3 items, check trigger fires for each
    // RemoveFirst from empty list, check trigger does not fire
    // void amctest_ZslistFirstChangedRemoveFirst();

    //
    // Insert 4 items
    // Remove in the following order, check trigger:
    // first (first) - fires
    // third (middle) - does not fire
    // fourth (tail) - does not fire
    // second - (the only) - fires
    //
    // void amctest_ZslistFirstChangedRemove();

    //
    // Insert 100 items
    // Flush
    // Trigger fires once
    //
    // void amctest_ZslistFirstChangedFlush();

    // ZSLIST - HEAD INSERTION - FIRST CHANGED
    // void zsl_h_typec_FirstChanged();

    //
    // Insert 3 items, check the trigger fires for each
    //
    // void amctest_ZslistHeadFirstChangedInsert();
}
