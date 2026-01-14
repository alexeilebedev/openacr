// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Header: include/amc.h -- Main header
//

#include "include/algo.h"
#include "include/gen/amc_gen.h"
#include "include/gen/amc_gen.inl.h"

#define amccheck(x,y) { if (!(x)) { prerr(y); algo_lib::_db.exit_code=1; } }

namespace amc { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/amc/alias.cpp -- Alias field type
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Alias(); // gstatic/amcdb.tclass:Alias
    // void tfunc_Alias_Get(); // gstatic/amcdb.tfunc:Alias.Get
    // void tfunc_Alias_Set(); // gstatic/amcdb.tfunc:Alias.Set
    // void tfunc_Alias_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Alias.ReadStrptrMaybe

    // -------------------------------------------------------------------
    // cpp/amc/avl.cpp -- AVL tree
    //

    // Initialize the structs etc.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Atree(); // gstatic/amcdb.tclass:Atree

    // Predicate function, can take parent as argument in the case of complicated comparisons.
    // void tfunc_Atree_ElemLt(); // gstatic/amcdb.tfunc:Atree.ElemLt

    // Generate the Prev function for the element.
    // Always symmetrical to Next through substitution of
    // First -> Last and Left -> Right.
    // void tfunc_Atree_Prev(); // gstatic/amcdb.tfunc:Atree.Prev

    // Generate the Next function for the element
    // void tfunc_Atree_Next(); // gstatic/amcdb.tfunc:Atree.Next

    // Initialize Root and Number of elements
    // void tfunc_Atree_Init(); // gstatic/amcdb.tfunc:Atree.Init

    // Returns true if the element is in tree.
    // void tfunc_Atree_InTreeQ(); // gstatic/amcdb.tfunc:Atree.InTreeQ

    // Returns true if the tree is empty.
    // Plant a tree if empty!
    // void tfunc_Atree_EmptyQ(); // gstatic/amcdb.tfunc:Atree.EmptyQ

    // Given an element finds the smallest element in the subtree.
    // void tfunc_Atree_FirstImpl(); // gstatic/amcdb.tfunc:Atree.FirstImpl

    // Given an element finds the largest element in the subtree.
    // void tfunc_Atree_LastImpl(); // gstatic/amcdb.tfunc:Atree.LastImpl

    // Returns the smallest element
    // void tfunc_Atree_First(); // gstatic/amcdb.tfunc:Atree.First

    // Returns the largest element.
    // void tfunc_Atree_Last(); // gstatic/amcdb.tfunc:Atree.Last

    // Finds the child that violates the balance. Left child if no disbalance.
    // void tfunc_Atree_TallerChild(); // gstatic/amcdb.tfunc:Atree.TallerChild

    // Disconnect the node from its parent.
    // void tfunc_Atree_Disconnect(); // gstatic/amcdb.tfunc:Atree.Disconnect

    // Rotates the tree from the direction from->to
    // Assumption is that from is the child of to.
    // void tfunc_Atree_Turn(); // gstatic/amcdb.tfunc:Atree.Turn

    // 1. Find disbalanced child.
    // 2. Find disbalanced grandchild.
    // 3. turn the grandchild toward child to have a-b-c simple case.
    // 4. Turn child onto node.
    // 5. Make sure to NOT turn the grandchild into child if child is balanced(Many hours of debugging).
    // void tfunc_Atree_Rebalance(); // gstatic/amcdb.tfunc:Atree.Rebalance
    // void tfunc_Atree_UpdateDepth(); // gstatic/amcdb.tfunc:Atree.UpdateDepth

    // Keep updating the depth and propagating up.
    // If a node needs rebalancing we rebalance and go up
    // Notice that after rebalance a node can go deeper down the tree.
    // void tfunc_Atree_Propagate(); // gstatic/amcdb.tfunc:Atree.Propagate

    // Iterate down the tree starting from the parent and place
    // the element in the appropriate leaf.
    // Note that balance might be broken after this operation.
    // void tfunc_Atree_InsertImpl(); // gstatic/amcdb.tfunc:Atree.InsertImpl

    // 1.Insert element starting from the root.
    // 2. Rebalance if necessary.
    // 3. Assign new root.
    // void tfunc_Atree_Insert(); // gstatic/amcdb.tfunc:Atree.Insert

    // Leftbalance - Rightbalance
    // void tfunc_Atree_Balance(); // gstatic/amcdb.tfunc:Atree.Balance

    // A recursive function to delete elements.
    // if del flag is set it also destroys the elements.
    // void tfunc_Atree_RemoveAllImpl(); // gstatic/amcdb.tfunc:Atree.RemoveAllImpl
    // void tfunc_Atree_Cascdel(); // gstatic/amcdb.tfunc:Atree.Cascdel

    // Remove all elements without deleting them.
    // void tfunc_Atree_RemoveAll(); // gstatic/amcdb.tfunc:Atree.RemoveAll

    // Remove the first element. Rebalance. Update root.
    // void tfunc_Atree_RemoveFirst(); // gstatic/amcdb.tfunc:Atree.RemoveFirst

    // Reinsert an element
    // void tfunc_Atree_Reinsert(); // gstatic/amcdb.tfunc:Atree.Reinsert

    // Connect 2 elements (either can be NULL).
    // void tfunc_Atree_Connect(); // gstatic/amcdb.tfunc:Atree.Connect

    // 1. Find next/prev element in my subtree (opposite of imbalance direction).
    // 2. If that element has a child, swap it with the child by turning from child.
    // 3. Swap the element to be removed with the next.
    // 4. Remove the element.
    // 5. Propagate up from the next's parent.
    // void tfunc_Atree_Remove(); // gstatic/amcdb.tfunc:Atree.Remove

    // First element that is greater or equal to the given value
    // void tfunc_Atree_FirstGe(); // gstatic/amcdb.tfunc:Atree.FirstGe

    // First element that is smaller than given value
    // void tfunc_Atree_LastLt(); // gstatic/amcdb.tfunc:Atree.LastLt

    // Generate cursor related funcs and struct.
    // Pretty straightforward since Next is done without extra state.
    // void tfunc_Atree_curs(); // gstatic/amcdb.tfunc:Atree.curs

    // -------------------------------------------------------------------
    // cpp/amc/base.cpp -- Base reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Base(); // gstatic/amcdb.tclass:Base

    // Generate a function to copy fields from a ctype to its base type
    // void tfunc_Base_CopyOut(); // gstatic/amcdb.tfunc:Base.CopyOut
    // void tfunc_Base_CopyIn(); // gstatic/amcdb.tfunc:Base.CopyIn
    // void tfunc_Base_Castbase(); // gstatic/amcdb.tfunc:Base.Castbase
    // void tfunc_Base_Castdown(); // gstatic/amcdb.tfunc:Base.Castdown
    double ChildRowid(double rowid);
    void CloneFields(amc::FCtype &from, amc::FCtype &to, double next_rowid, amc::FField &basefield);

    // -------------------------------------------------------------------
    // cpp/amc/bheap.cpp -- Binary heaps
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bheap(); // gstatic/amcdb.tclass:Bheap
    // void tfunc_Bheap_Upheap(); // gstatic/amcdb.tfunc:Bheap.Upheap
    // void tfunc_Bheap_Downheap(); // gstatic/amcdb.tfunc:Bheap.Downheap
    // void tfunc_Bheap_Reheap(); // gstatic/amcdb.tfunc:Bheap.Reheap
    // void tfunc_Bheap_ReheapFirst(); // gstatic/amcdb.tfunc:Bheap.ReheapFirst
    // void tfunc_Bheap_Set(); // gstatic/amcdb.tfunc:Bheap.Set
    // void tfunc_Bheap_SetIfBetter(); // gstatic/amcdb.tfunc:Bheap.SetIfBetter
    // void tfunc_Bheap_Cascdel(); // gstatic/amcdb.tfunc:Bheap.Cascdel
    // void tfunc_Bheap_RemoveFirst(); // gstatic/amcdb.tfunc:Bheap.RemoveFirst
    // void tfunc_Bheap_First(); // gstatic/amcdb.tfunc:Bheap.First
    // void tfunc_Bheap_InBheapQ(); // gstatic/amcdb.tfunc:Bheap.InBheapQ
    // void tfunc_Bheap_Insert(); // gstatic/amcdb.tfunc:Bheap.Insert
    // void tfunc_Bheap_Compact(); // gstatic/amcdb.tfunc:Bheap.Compact
    // void tfunc_Bheap_Remove(); // gstatic/amcdb.tfunc:Bheap.Remove
    // void tfunc_Bheap_N(); // gstatic/amcdb.tfunc:Bheap.N
    // void tfunc_Bheap_EmptyQ(); // gstatic/amcdb.tfunc:Bheap.EmptyQ
    // void tfunc_Bheap_RemoveAll(); // gstatic/amcdb.tfunc:Bheap.RemoveAll
    // void tfunc_Bheap_Dealloc(); // gstatic/amcdb.tfunc:Bheap.Dealloc
    // void tfunc_Bheap_Reserve(); // gstatic/amcdb.tfunc:Bheap.Reserve
    // void tfunc_Bheap_Init(); // gstatic/amcdb.tfunc:Bheap.Init
    // void tfunc_Bheap_Uninit(); // gstatic/amcdb.tfunc:Bheap.Uninit
    // void tfunc_Bheap_ElemLt(); // gstatic/amcdb.tfunc:Bheap.ElemLt
    // void tfunc_Bheap_ElemLtval(); // gstatic/amcdb.tfunc:Bheap.ElemLtval
    // void tfunc_Bheap_unordcurs(); // gstatic/amcdb.tfunc:Bheap.unordcurs
    // void tfunc_Bheap_curs(); // gstatic/amcdb.tfunc:Bheap.curs

    // -------------------------------------------------------------------
    // cpp/amc/bitfld.cpp -- Bit fields
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitfld(); // gstatic/amcdb.tclass:Bitfld
    // void tfunc_Bitfld_Init(); // gstatic/amcdb.tfunc:Bitfld.Init
    // void tfunc_Bitfld_Get(); // gstatic/amcdb.tfunc:Bitfld.Get
    // void tfunc_Bitfld_Set(); // gstatic/amcdb.tfunc:Bitfld.Set

    // -------------------------------------------------------------------
    // cpp/amc/bitset.cpp -- Bit sets
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitset(); // gstatic/amcdb.tclass:Bitset
    // void tfunc_Bitset_N(); // gstatic/amcdb.tfunc:Bitset.N
    // void tfunc_Bitset_qFind(); // gstatic/amcdb.tfunc:Bitset.qFind
    // void tfunc_Bitset_NBits(); // gstatic/amcdb.tfunc:Bitset.NBits
    // void tfunc_Bitset_qGetBit(); // gstatic/amcdb.tfunc:Bitset.qGetBit
    // void tfunc_Bitset_GetBit(); // gstatic/amcdb.tfunc:Bitset.GetBit
    // void tfunc_Bitset_Sum1s(); // gstatic/amcdb.tfunc:Bitset.Sum1s
    // void tfunc_Bitset_BitsEmptyQ(); // gstatic/amcdb.tfunc:Bitset.BitsEmptyQ
    // void tfunc_Bitset_qClearBit(); // gstatic/amcdb.tfunc:Bitset.qClearBit
    // void tfunc_Bitset_ClearBit(); // gstatic/amcdb.tfunc:Bitset.ClearBit
    // void tfunc_Bitset_qSetBit(); // gstatic/amcdb.tfunc:Bitset.qSetBit
    // void tfunc_Bitset_SetBit(); // gstatic/amcdb.tfunc:Bitset.SetBit
    // void tfunc_Bitset_qSetBitVal(); // gstatic/amcdb.tfunc:Bitset.qSetBitVal
    // void tfunc_Bitset_qOrBitVal(); // gstatic/amcdb.tfunc:Bitset.qOrBitVal
    // void tfunc_Bitset_ClearBitsAll(); // gstatic/amcdb.tfunc:Bitset.ClearBitsAll
    // void tfunc_Bitset_ClearBits(); // gstatic/amcdb.tfunc:Bitset.ClearBits
    // void tfunc_Bitset_OrBits(); // gstatic/amcdb.tfunc:Bitset.OrBits
    // void tfunc_Bitset_ExpandBits(); // gstatic/amcdb.tfunc:Bitset.ExpandBits
    // void tfunc_Bitset_AllocBit(); // gstatic/amcdb.tfunc:Bitset.AllocBit
    // void tfunc_Bitset_Sup(); // gstatic/amcdb.tfunc:Bitset.Sup
    // void tfunc_Bitset_bitcurs(); // gstatic/amcdb.tfunc:Bitset.bitcurs

    // -------------------------------------------------------------------
    // cpp/amc/blkpool.cpp -- Block pool
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Blkpool(); // gstatic/amcdb.tclass:Blkpool
    // void tfunc_Blkpool_AllocMem(); // gstatic/amcdb.tfunc:Blkpool.AllocMem
    // void tfunc_Blkpool_SetBufferSize(); // gstatic/amcdb.tfunc:Blkpool.SetBufferSize
    // void tfunc_Blkpool_ReserveBuffers(); // gstatic/amcdb.tfunc:Blkpool.ReserveBuffers
    // void tfunc_Blkpool_FreeMem(); // gstatic/amcdb.tfunc:Blkpool.FreeMem
    // void tfunc_Blkpool_Init(); // gstatic/amcdb.tfunc:Blkpool.Init

    // -------------------------------------------------------------------
    // cpp/amc/cget.cpp -- Getters / Setters
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_cget(); // gstatic/amcdb.gen:cget

    // -------------------------------------------------------------------
    // cpp/amc/charset.cpp -- Charset functions
    //

    // Preprocess charsets
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_newfield_charset(); // gstatic/amcdb.gen:newfield_charset
    // void tclass_Charset(); // gstatic/amcdb.tclass:Charset
    // void tfunc_Charset_Match(); // gstatic/amcdb.tfunc:Charset.Match

    // -------------------------------------------------------------------
    // cpp/amc/checkxref.cpp -- X-reference checker
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_xref(); // gstatic/amcdb.gen:check_xref

    // -------------------------------------------------------------------
    // cpp/amc/cmp.cpp -- Comparison functions
    //

    // Field comparison functions
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cmp(); // gstatic/amcdb.tclass:Cmp

    // Next char function for version sort
    // void tfunc_Cmp_Nextchar(); // gstatic/amcdb.tfunc:Cmp.Nextchar

    // Comparison function for field
    // void tfunc_Cmp_Cmp(); // gstatic/amcdb.tfunc:Cmp.Cmp

    // Lt function for field
    // void tfunc_Cmp_Lt(); // gstatic/amcdb.tfunc:Cmp.Lt

    // -------------------------------------------------------------------
    // cpp/amc/concat.cpp -- Pkey constructor
    //
    void GenerateSetForCoveredTrees(amc::FField& sourceField, u64 filledRange, int lastIndex);

    // -------------------------------------------------------------------
    // cpp/amc/count.cpp -- Count reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Count(); // gstatic/amcdb.tclass:Count
    // void tfunc_Count_Insert(); // gstatic/amcdb.tfunc:Count.Insert
    // void tfunc_Count_Remove(); // gstatic/amcdb.tfunc:Count.Remove
    // void tfunc_Count_N(); // gstatic/amcdb.tfunc:Count.N

    // -------------------------------------------------------------------
    // cpp/amc/cppfunc.cpp -- Cppfunc reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cppfunc(); // gstatic/amcdb.tclass:Cppfunc
    // void tfunc_Cppfunc_Get(); // gstatic/amcdb.tfunc:Cppfunc.Get
    // void tfunc_Cppfunc_Init(); // gstatic/amcdb.tfunc:Cppfunc.Init
    // void tfunc_Cppfunc_Set(); // gstatic/amcdb.tfunc:Cppfunc.Set

    // -------------------------------------------------------------------
    // cpp/amc/ctype.cpp -- Ctype code generators
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ctype(); // gstatic/amcdb.tclass:Ctype

    // Generate de-initialization function:
    // - cascdel fields
    // - remove record from all indexes
    // - user-defined cleanup for all fields
    // - un-init fields (frees memory)
    // All fields are scanned in reverse order
    // void tfunc_Ctype_Uninit(); // gstatic/amcdb.tfunc:Ctype.Uninit

    // Introduce local variable that points to the parent side of XREF
    // by evaluating the path provided by xref + xreffld + xrefvia records.
    bool ComputeAccess(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FXref &xref, amc::FFunc &func, amc::FGenXref &frame, bool check_null);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_XrefMaybe(); // gstatic/amcdb.tfunc:Ctype.XrefMaybe
    // void tfunc_Ctype_Unref(); // gstatic/amcdb.tfunc:Ctype.Unref
    // void tfunc_Ctype_Hash(); // gstatic/amcdb.tfunc:Ctype.Hash

    // Comparison function for ctype
    // In amc, equality is not strictly a property of a type; it can be overriden on a per-field basis
    // For example, the same field (algo.Smallstr100 for instance) can be made case-insensitive
    // for the purposes of comparison within some ctype
    // That's why the Cmp function for ctype does not call out fo
    // void tfunc_Ctype_Cmp(); // gstatic/amcdb.tfunc:Ctype.Cmp

    // Less-than function for ctype
    // void tfunc_Ctype_Lt(); // gstatic/amcdb.tfunc:Ctype.Lt
    // void tfunc_Ctype_Init(); // gstatic/amcdb.tfunc:Ctype.Init
    // void tfunc_Ctype_Update(); // gstatic/amcdb.tfunc:Ctype.Update
    // void tfunc_Ctype_Min(); // gstatic/amcdb.tfunc:Ctype.Min
    // void tfunc_Ctype_Max(); // gstatic/amcdb.tfunc:Ctype.Max
    // void tfunc_Ctype_UpdateMin(); // gstatic/amcdb.tfunc:Ctype.UpdateMin
    // void tfunc_Ctype_UpdateMax(); // gstatic/amcdb.tfunc:Ctype.UpdateMax

    // Equality function for ctype
    // The function proceeds field-by-field, using the following rules:
    // - pad bytes are ignored
    // - if a field has an Eq function defined (usually because its equality has been customized with the fcmp record)
    // we call out to the defined Eq function.
    // - pointers and big-endian fields are compared bitwise for equality
    // - any string fields are compared as regular strings
    // - if no custom Eq function is defined on a field, but the field's type has an Eq function, it is used
    // - for all other fields, c++ operator "==" is used. it better be defined
    // void tfunc_Ctype_Eq(); // gstatic/amcdb.tfunc:Ctype.Eq
    // void tfunc_Ctype_ToCmdline(); // gstatic/amcdb.tfunc:Ctype.ToCmdline

    // Used with command lines
    // void tfunc_Ctype_NArgs(); // gstatic/amcdb.tfunc:Ctype.NArgs
    // void tfunc_Ctype_Print(); // gstatic/amcdb.tfunc:Ctype.Print
    // void tfunc_Ctype_Read(); // gstatic/amcdb.tfunc:Ctype.Read
    // void tfunc_Ctype_EqEnum(); // gstatic/amcdb.tfunc:Ctype.EqEnum
    // void tfunc_Ctype_GetAnon(); // gstatic/amcdb.tfunc:Ctype.GetAnon
    // void tfunc_Ctype_GetMsgLength(); // gstatic/amcdb.tfunc:Ctype.GetMsgLength
    // void tfunc_Ctype_GetMsgMemptr(); // gstatic/amcdb.tfunc:Ctype.GetMsgMemptr
    // void tclass_Ctype2(); // gstatic/amcdb.tclass:Ctype2

    // Generate constructor.
    // void tfunc_Ctype2_Ctor(); // gstatic/amcdb.tfunc:Ctype2.Ctor
    // void tfunc_Ctype2_FieldwiseCtor(); // gstatic/amcdb.tfunc:Ctype2.FieldwiseCtor
    // void tfunc_Ctype2_EnumCtor(); // gstatic/amcdb.tfunc:Ctype2.EnumCtor

    // Generator copy constructor or assignment operator
    // (the two functions are very similar)
    void GenCopyCtorOrAssignOp(bool copyctor);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype2_CopyCtor(); // gstatic/amcdb.tfunc:Ctype2.CopyCtor
    // void tfunc_Ctype_AssignOp(); // gstatic/amcdb.tfunc:Ctype.AssignOp
    // void tfunc_Ctype2_Dtor(); // gstatic/amcdb.tfunc:Ctype2.Dtor
    // void tfunc_Ctype_EqOp(); // gstatic/amcdb.tfunc:Ctype.EqOp
    // void tfunc_Ctype_NeOp(); // gstatic/amcdb.tfunc:Ctype.NeOp
    // void tfunc_Ctype_LtOp(); // gstatic/amcdb.tfunc:Ctype.LtOp
    // void tfunc_Ctype_GtOp(); // gstatic/amcdb.tfunc:Ctype.GtOp
    // void tfunc_Ctype_LeOp(); // gstatic/amcdb.tfunc:Ctype.LeOp
    // void tfunc_Ctype_GeOp(); // gstatic/amcdb.tfunc:Ctype.GeOp
    // void tfunc_Ctype_EqOpAryptr(); // gstatic/amcdb.tfunc:Ctype.EqOpAryptr

    // -------------------------------------------------------------------
    // cpp/amc/delptr.cpp -- Delptr reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Delptr(); // gstatic/amcdb.tclass:Delptr
    // void tfunc_Delptr_Init(); // gstatic/amcdb.tfunc:Delptr.Init
    // void tfunc_Delptr_Uninit(); // gstatic/amcdb.tfunc:Delptr.Uninit
    // void tfunc_Delptr_Delete(); // gstatic/amcdb.tfunc:Delptr.Delete
    // void tfunc_Delptr_Access(); // gstatic/amcdb.tfunc:Delptr.Access

    // -------------------------------------------------------------------
    // cpp/amc/dflt.cpp -- Field default
    //
    tempstr DfltExprVal(amc::FField &field);
    tempstr DfltExprBitfld(amc::FField &field);
    void Field_UpdateDflt(amc::FField &field);

    // -------------------------------------------------------------------
    // cpp/amc/disp/call.cpp -- Dispatch call
    //

    // Generator for call-type dispatch (where we invoke a user function
    // for each case)
    void Disp_Call(amc::FDispatch& dispatch);

    // -------------------------------------------------------------------
    // cpp/amc/disp/casetype.cpp -- Dispatch casetype generator
    //

    // Determine common header for all messages in the dispatch
    // Compute values of P_CTYPE_HDR and P_CASETYPE.
    // P_CTYPE_HDR is non-NULL if all of the messages in the dispatch share the same message
    // header.
    // P_CASETYPE is always non-NULL. It is set to the type field of the common message
    // header, if one exists, or to a newly created enum.
    void Disp_CreateCasetype(amc::FDispatch &dispatch);

    // -------------------------------------------------------------------
    // cpp/amc/disp/del.cpp
    //

    // Generate Dispatch_Delete function
    void Disp_Delete(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/disp/filter.cpp -- Dispatch filter
    //

    // Create new fields for dispatch filters.
    void Filter_NewField();
    void Filter_Gen(amc::FNs &ns);

    // -------------------------------------------------------------------
    // cpp/amc/disp/kafka_decode.cpp
    //
    void Disp_KafkaDecode(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/disp/kafka_encode.cpp
    //
    void Disp_KafkaEncode(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/disp/main.cpp -- Dispatch main
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_dispatch(); // gstatic/amcdb.gen:ns_dispatch

    // Create new fields for dispatch filters.
    void Disp_NewField();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_dispenum(); // gstatic/amcdb.gen:dispenum

    // -------------------------------------------------------------------
    // cpp/amc/disp/msg.cpp -- Dispatch on message
    //

    // Create a new dispatch collecting all messages
    // using a given message header (e.g. find all xyz messages
    // and create a dispatch called xyz::MsgHeaderMsgs, that
    // can be used to both read and print xyz message types
    void Disp_CreateFromMsg();

    // -------------------------------------------------------------------
    // cpp/amc/disp/print.cpp -- Dispatch print
    //

    // Generate Dispatch_Print function
    void Disp_Print(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/disp/read.cpp -- Dispatch read from string
    //
    amc::FCfmt *FindStringRead(amc::FCtype &ctype);

    // Generate Dispatch_Read function
    void Disp_Read(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/enum.cpp -- Enumerated types
    //

    // emit constants
    // - c++ enums (fconst for integers)
    // - extern strings (fconst for non-integers)
    void Main_GenEnum(amc::FNs& ns, amc::FCtype &ctype);
    tempstr Enumtype(amc::FField &field);

    // -------------------------------------------------------------------
    // cpp/amc/exec.cpp -- Exec reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Exec(); // gstatic/amcdb.tclass:Exec
    // void tfunc_Exec_Init(); // gstatic/amcdb.tfunc:Exec.Init
    // void tfunc_Exec_Uninit(); // gstatic/amcdb.tfunc:Exec.Uninit
    // void tfunc_Exec_Wait(); // gstatic/amcdb.tfunc:Exec.Wait
    // void tfunc_Exec_Kill(); // gstatic/amcdb.tfunc:Exec.Kill
    // void tfunc_Exec_Start(); // gstatic/amcdb.tfunc:Exec.Start
    // void tfunc_Exec_StartRead(); // gstatic/amcdb.tfunc:Exec.StartRead
    // void tfunc_Exec_Exec(); // gstatic/amcdb.tfunc:Exec.Exec
    // void tfunc_Exec_ExecX(); // gstatic/amcdb.tfunc:Exec.ExecX
    // void tfunc_Exec_ToCmdline(); // gstatic/amcdb.tfunc:Exec.ToCmdline
    // void tfunc_Exec_ToArgv(); // gstatic/amcdb.tfunc:Exec.ToArgv
    // void tfunc_Exec_Execv(); // gstatic/amcdb.tfunc:Exec.Execv
    void NewFieldExec();

    // -------------------------------------------------------------------
    // cpp/amc/fast.cpp -- Fixfast generator
    //

    // Prepare FAST data
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_fast_presence(); // gstatic/amcdb.gen:fast_presence

    // Type FAST encode
    // void tfunc_Ctype_FastEncode(); // gstatic/amcdb.tfunc:Ctype.FastEncode

    // Type FAST decode
    // void tfunc_Ctype_FastDecode(); // gstatic/amcdb.tfunc:Ctype.FastDecode
    // void tfunc_Ctype_FixEncode(); // gstatic/amcdb.tfunc:Ctype.FixEncode

    // FAST - ns functions
    // void gen_ns_fast(); // gstatic/amcdb.gen:ns_fast

    // -------------------------------------------------------------------
    // cpp/amc/fbuf.cpp -- Byte buffer
    //

    //
    // see tex/amc/amc_fbuf.tex for documentation
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fbuf(); // gstatic/amcdb.tclass:Fbuf
    // void tfunc_Fbuf_BeginRead(); // gstatic/amcdb.tfunc:Fbuf.BeginRead
    // void tfunc_Fbuf_GetMsg(); // gstatic/amcdb.tfunc:Fbuf.GetMsg
    // void tfunc_Fbuf_Init(); // gstatic/amcdb.tfunc:Fbuf.Init
    // void tfunc_Fbuf_Max(); // gstatic/amcdb.tfunc:Fbuf.Max
    // void tfunc_Fbuf_N(); // gstatic/amcdb.tfunc:Fbuf.N
    // void tfunc_Fbuf_Refill(); // gstatic/amcdb.tfunc:Fbuf.Refill
    // void tfunc_Fbuf_RemoveAll(); // gstatic/amcdb.tfunc:Fbuf.RemoveAll
    // void tfunc_Fbuf_ScanMsg(); // gstatic/amcdb.tfunc:Fbuf.ScanMsg
    // void tfunc_Fbuf_Shift(); // gstatic/amcdb.tfunc:Fbuf.Shift
    // void tfunc_Fbuf_SkipBytes(); // gstatic/amcdb.tfunc:Fbuf.SkipBytes
    // void tfunc_Fbuf_SkipMsg(); // gstatic/amcdb.tfunc:Fbuf.SkipMsg
    // void tfunc_Fbuf_WriteReserve(); // gstatic/amcdb.tfunc:Fbuf.WriteReserve
    // void tfunc_Fbuf_WriteAll(); // gstatic/amcdb.tfunc:Fbuf.WriteAll
    // void tfunc_Fbuf_WriteMsg(); // gstatic/amcdb.tfunc:Fbuf.WriteMsg
    // void tfunc_Fbuf_BeginWrite(); // gstatic/amcdb.tfunc:Fbuf.BeginWrite
    // void tfunc_Fbuf_Outflow(); // gstatic/amcdb.tfunc:Fbuf.Outflow
    // void tfunc_Fbuf_EndRead(); // gstatic/amcdb.tfunc:Fbuf.EndRead
    // void tfunc_Fbuf_EndWrite(); // gstatic/amcdb.tfunc:Fbuf.EndWrite
    // void tfunc_Fbuf_Realloc(); // gstatic/amcdb.tfunc:Fbuf.Realloc
    // void tfunc_Fbuf_Uninit(); // gstatic/amcdb.tfunc:Fbuf.Uninit

    // -------------------------------------------------------------------
    // cpp/amc/fcast.cpp -- Implicit casts
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fcast(); // gstatic/amcdb.tclass:Fcast
    // void tfunc_Fcast_Cast(); // gstatic/amcdb.tfunc:Fcast.Cast

    // -------------------------------------------------------------------
    // cpp/amc/fcmap.cpp -- Fcmap generation (enum<->enum conversion)
    //

    // Get the conversion function if it exists
    amc::FFunc* ConvertFuncMaybe(amc::FCtype& from, amc::FCtype& to);

    // TODO::warn about the following case:
    // A.b = A.c, A.d = A.e. That setup yields only one function A_FromA() instead of 2 (and a compile error).
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_fcmap(); // gstatic/amcdb.gen:fcmap

    // -------------------------------------------------------------------
    // cpp/amc/fconst.cpp -- Constants, string <-> int conversion
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fconst(); // gstatic/amcdb.tclass:Fconst
    // void tfunc_Fconst_GetEnum(); // gstatic/amcdb.tfunc:Fconst.GetEnum
    // void tfunc_Fconst_SetEnum(); // gstatic/amcdb.tfunc:Fconst.SetEnum
    // void tfunc_Fconst_ToCstr(); // gstatic/amcdb.tfunc:Fconst.ToCstr
    // void tfunc_Fconst_Print(); // gstatic/amcdb.tfunc:Fconst.Print
    // void tfunc_Fconst_SetStrptrMaybe(); // gstatic/amcdb.tfunc:Fconst.SetStrptrMaybe
    // void tfunc_Fconst_SetStrptr(); // gstatic/amcdb.tfunc:Fconst.SetStrptr
    // void tfunc_Fconst_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Fconst.ReadStrptrMaybe

    // -------------------------------------------------------------------
    // cpp/amc/fdec.cpp -- Decimal types
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Dec(); // gstatic/amcdb.tclass:Dec
    // void tfunc_Dec_GetDouble(); // gstatic/amcdb.tfunc:Dec.GetDouble
    // void tfunc_Dec_GetInt(); // gstatic/amcdb.tfunc:Dec.GetInt
    // void tfunc_Dec_GetScale(); // gstatic/amcdb.tfunc:Dec.GetScale
    // void tfunc_Dec_qSetDouble(); // gstatic/amcdb.tfunc:Dec.qSetDouble
    // void tfunc_Dec_SetDoubleMaybe(); // gstatic/amcdb.tfunc:Dec.SetDoubleMaybe
    // void tfunc_Dec_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Dec.ReadStrptrMaybe
    // void tfunc_Dec_Print(); // gstatic/amcdb.tfunc:Dec.Print

    // -------------------------------------------------------------------
    // cpp/amc/field.cpp -- Generic field genrator
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Field(); // gstatic/amcdb.tclass:Field
    // void tclass_Field2(); // gstatic/amcdb.tclass:Field2
    // void tfunc_Field_Cleanup(); // gstatic/amcdb.tfunc:Field.Cleanup
    // void tfunc_Field_Userinit(); // gstatic/amcdb.tfunc:Field.Userinit
    // void tfunc_Field_Cascdel(); // gstatic/amcdb.tfunc:Field.Cascdel
    // void tfunc_Field2_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Field2.ReadStrptrMaybe
    // void tfunc_Field_Concat(); // gstatic/amcdb.tfunc:Field.Concat

    // True if ReadFieldExpr for the field
    bool HasReadExprQ(amc::FField &field);

    // Return an expression, with type bool,
    // reading field FIELD in struct PARENT from value STRVAL
    // If impossible, return FALSE;
    tempstr ReadFieldExpr(amc::FField &field, algo::strptr parent, algo::strptr strval);

    // -------------------------------------------------------------------
    // cpp/amc/func.cpp -- C++ function output
    //

    // Add argument to the most recent function call/declaration inside given string.
    // TEXT: AddCallArg("abcd();","x") -> "abcd(x);"
    // TEXT: AddCallArg("abcd(y) abc","x") -> "abcd(y, x) abc"
    // Experimental, helps in readability: If CONDITION is false, do nothing
    void AddArg(cstring &text, strptr cppexpr, bool condition);

    // Same but without conditions
    void AddArg(cstring &text, strptr cppexpr);

    // Add prototype arg to function FUNC
    // The argument type is TYPE, and name is NAME
    // Argument is added only if CONDITION is true
    amc::Funcarg* AddProtoArg(amc::FFunc &func, strptr type, strptr name, bool condition);

    // Same but without condition
    amc::Funcarg* AddProtoArg(amc::FFunc &func, strptr type, strptr name);

    // Declare return value for function FUNC.
    // Return types are stored in Funcarg array, and marked with RETVAL=true
    // The value has type TYPE, name NAME, and is initialized with INITALIZER.
    // If NAME is not an empty string, a variable with this name and type is also declared.
    // The call to "return <retval>" is automatically inserted during function "finalize" step
    // If INITIALIZER is "", the value is declared "as-is"
    // TYPE NAME;
    // Otherwise, equals sign is used:
    // TYPE NAME = INITIALIZER;
    amc::Funcarg* AddRetval(amc::FFunc &func, strptr type, strptr name, strptr initializer);

    // Add a type argument to function FUNC,
    // TYPE is a c++ expression.
    // Function becomes a template.
    amc::Funcarg* AddTypearg(amc::FFunc &func, strptr type);
    void SetPresent(amc::FFunc &func, strptr ref, amc::FField &field);

    // Create function for current field (amc::_db.genctx.p_field) & tfunc (amc::_db.genctx.p_tfunc)
    // The function name is constructed from the field name and the tfunc name, e.g. "field_FuncName"
    // unless an explicit function name is passed through argument FUNCNAME.
    //
    // Return a reference to the resulting FUNC record
    // The following options are set in FUNC based on the TFUNC prototype:
    // glob, wur, inl, globns, isalloc, nothrow, ismacro, pure.
    // The function comment is initialized from tfunc prototype
    // Function is DISABLED if namespace has exceptions disabled and the tfunc is labeled HASTHROW.
    // If PROTO flag is set, function prototype (FUNC.PROTO) string is initialized to the function name,
    // and a default first argument is added unless the function is on a global field.
    amc::FFunc &CreateCurFunc(bool proto = false, algo::strptr funcname = "");

    // Return non-null pointer to ctype's init function.
    amc::FFunc *init_GetOrCreate(amc::FCtype &ctype);

    // Emit function prototype into string OUT.
    // If ctype_context is specified, then the declaration is intended to be
    // used inside the struct, so different C++ syntax rules apply.
    void PrintFuncProto(amc::FFunc& func, amc::FCtype *ctype_context, cstring &out, bool querymode);
    void PrintFuncBody(amc::FNs& ns, amc::FFunc& func);
    bool SetRetType(amc::FFunc &func, amc::FCtype &ctype);
    void MaybeUnused(amc::FFunc &func, strptr name);
    amc::Funcarg* FindArg(amc::FFunc &func, strptr name);

    // Create function marked as inline,nothrow,ret.
    amc::FFunc &CreateInlineFunc(algo::strptr name);

    // -------------------------------------------------------------------
    // cpp/amc/fwddecl.cpp -- Forward declarations
    //

    // Return TRUE if namespace NS already contains a forward declaration
    // of FWDDECL, either directly, or transitively through one of the namespaces
    // included in ns_gen.h
    bool FwdDeclExistsQ(amc::FNs &ns, amc::FFwddecl &fwddecl);

    // Return TRUE if FIELD requires a forward declaration for its type
    // This may be required since the type may be unknown at the point where the field is being
    // defined.
    bool FwdDeclQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_fwddecl2(); // gstatic/amcdb.gen:ns_fwddecl2

    // emit forward-declarations of steps
    // void gen_ns_fwddecl(); // gstatic/amcdb.gen:ns_fwddecl

    // -------------------------------------------------------------------
    // cpp/amc/gen.cpp -- AMC Generators (amcdb.gen)
    //
    amc::FField *InsField(const dmmeta::Field &field);

    // Move FIELD so that it appears immediately after field AFTER.
    void PlaceFieldAfter(amc::FField &field, amc::FField &after);
    bool HasAllocQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_usedns(); // gstatic/amcdb.gen:usedns
    // void gen_include(); // gstatic/amcdb.gen:include
    // void gen_ns_check_main(); // gstatic/amcdb.gen:ns_check_main
    // void gen_check_cascdel(); // gstatic/amcdb.gen:check_cascdel
    // void gen_check_ssimsort(); // gstatic/amcdb.gen:check_ssimsort
    // void gen_countxref(); // gstatic/amcdb.gen:countxref

    // Check that a struct marked cheap_copy does not contain structs not marked that way
    // void gen_check_cheapcopy(); // gstatic/amcdb.gen:check_cheapcopy
    // void gen_check_prefix(); // gstatic/amcdb.gen:check_prefix
    // void gen_basepool(); // gstatic/amcdb.gen:basepool

    // TODO: also check that targdep path exists between namespaces?
    // void gen_check_basepool(); // gstatic/amcdb.gen:check_basepool
    // void gen_check_bitfld(); // gstatic/amcdb.gen:check_bitfld

    // Rewrite reftype:Pkey fields into reftype:Val
    // void gen_lookuppkey(); // gstatic/amcdb.gen:lookuppkey

    // Ensure fregx record exists for each field of type Regx or RegxSql
    // Rewrite RegxSql fields as Regx, regxtype Sql
    // (generalization of original RegxSql reftype)
    // void gen_rewrite_regx(); // gstatic/amcdb.gen:rewrite_regx
    // void gen_check_reftype(); // gstatic/amcdb.gen:check_reftype
    // void gen_detectinst(); // gstatic/amcdb.gen:detectinst
    // void gen_check_cpptype(); // gstatic/amcdb.gen:check_cpptype

    // Count # fields, excluding substrings, cppfuncs, and bitfields
    int NValFields(amc::FCtype &ctype);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_field(); // gstatic/amcdb.gen:prep_field
    // void gen_check_bigend(); // gstatic/amcdb.gen:check_bigend

    // walk over all xrefs and populate ctype.c_parent array,
    // which lists all the "parent" ctypes (ones used by this ctype)
    // check
    // void gen_xref_parent(); // gstatic/amcdb.gen:xref_parent
    // void gen_datafld(); // gstatic/amcdb.gen:datafld
    // void gen_ctype_toposort(); // gstatic/amcdb.gen:ctype_toposort
    void PlaindataVisit(amc::FCtype &ctype);

    // recursively determine for each type whether it's "plaindata".
    // set ctype.plaindata flag.
    // plaindata structs can be copied with memcpy.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_plaindata(); // gstatic/amcdb.gen:plaindata
    tempstr Argtype(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_ctype(); // gstatic/amcdb.gen:prep_ctype
    // void gen_xref2(); // gstatic/amcdb.gen:xref2
    // void gen_select_ns(); // gstatic/amcdb.gen:select_ns

    //
    // Generate Fconst from a column of some table.
    //
    //
    // void gen_gconst(); // gstatic/amcdb.gen:gconst

    //
    // Generate Fconst from a column of some table.
    //
    // void gen_bitfldenum(); // gstatic/amcdb.gen:bitfldenum
    // void gen_prep_fconst(); // gstatic/amcdb.gen:prep_fconst

    // Generate numeric fconsts from string fconsts
    // void gen_clonefconst(); // gstatic/amcdb.gen:clonefconst
    // void gen_newfield_exec(); // gstatic/amcdb.gen:newfield_exec
    // void gen_newfield_count(); // gstatic/amcdb.gen:newfield_count

    // convert cbase records into fields of reftype Base
    // void gen_newfield_cbase(); // gstatic/amcdb.gen:newfield_cbase

    // convert cbase records into fields of reftype Base
    // void gen_check_basefield(); // gstatic/amcdb.gen:check_basefield
    // void gen_newfield_sortfld(); // gstatic/amcdb.gen:newfield_sortfld
    // void gen_newfield_ptrary(); // gstatic/amcdb.gen:newfield_ptrary
    // void gen_newfield_dispatch(); // gstatic/amcdb.gen:newfield_dispatch
    // void gen_newfield_cfmt(); // gstatic/amcdb.gen:newfield_cfmt
    // void gen_ns_check_pack(); // gstatic/amcdb.gen:ns_check_pack

    // A singly linked list requires scanning to locate the element to delete.
    // It is considered an access path with quadratic deletion cost.
    // So is a Ptrary.
    bool SlowDelQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_check_path(); // gstatic/amcdb.gen:ns_check_path
    // void gen_ns_pkeytypedef(); // gstatic/amcdb.gen:ns_pkeytypedef
    // void gen_ns_enums(); // gstatic/amcdb.gen:ns_enums
    // void gen_ns_include(); // gstatic/amcdb.gen:ns_include
    // void gen_ns_funcindex(); // gstatic/amcdb.gen:ns_funcindex

    // Print static function prototype declarations
    // to the cpp file
    // void gen_ns_print_proto(); // gstatic/amcdb.gen:ns_print_proto
    // void gen_ns_print_struct(); // gstatic/amcdb.gen:ns_print_struct
    // void gen_ns_curstext(); // gstatic/amcdb.gen:ns_curstext
    // void gen_ns_pnew(); // gstatic/amcdb.gen:ns_pnew
    // void gen_ns_func(); // gstatic/amcdb.gen:ns_func
    // void gen_ns_operators(); // gstatic/amcdb.gen:ns_operators
    // void gen_ns_check_lim(); // gstatic/amcdb.gen:ns_check_lim
    // void gen_proc(); // gstatic/amcdb.gen:proc
    // void gen_check_fcurs(); // gstatic/amcdb.gen:check_fcurs
    // void gen_check_varlen(); // gstatic/amcdb.gen:check_varlen

    // Generate a global list c_ssimfile_sorted
    // which indexes ssimfiles in topological order
    // void gen_sortssimfile(); // gstatic/amcdb.gen:sortssimfile
    // void gen_create_userfunc(); // gstatic/amcdb.gen:create_userfunc
    // void gen_table_write(); // gstatic/amcdb.gen:table_write

    // -------------------------------------------------------------------
    // cpp/amc/global.cpp -- Global (FDb)
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Global(); // gstatic/amcdb.tclass:Global
    // void tfunc_Global_Init(); // gstatic/amcdb.tfunc:Global.Init
    int c_parentns_FindIndex(amc::FNs& ns, amc::FNs *val);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Global_LoadTuplesMaybe(); // gstatic/amcdb.tfunc:Global.LoadTuplesMaybe
    // void tfunc_Global_LoadTuplesFile(); // gstatic/amcdb.tfunc:Global.LoadTuplesFile
    // void tfunc_Global_LoadTuplesFd(); // gstatic/amcdb.tfunc:Global.LoadTuplesFd
    // void tfunc_Global_SaveTuples(); // gstatic/amcdb.tfunc:Global.SaveTuples
    // void tfunc_Global_InsertStrptrMaybe(); // gstatic/amcdb.tfunc:Global.InsertStrptrMaybe
    // void tfunc_Global_InitReflection(); // gstatic/amcdb.tfunc:Global.InitReflection
    // void tfunc_Global_LoadSsimfileMaybe(); // gstatic/amcdb.tfunc:Global.LoadSsimfileMaybe
    // void tfunc_Global_main(); // gstatic/amcdb.tfunc:Global.main
    // void tfunc_Global_WinMain(); // gstatic/amcdb.tfunc:Global.WinMain
    // void tfunc_Global_MainLoop(); // gstatic/amcdb.tfunc:Global.MainLoop
    // void tfunc_Global_Steps(); // gstatic/amcdb.tfunc:Global.Steps
    // void tfunc_Global_Step(); // gstatic/amcdb.tfunc:Global.Step
    // void tfunc_Global_Main(); // gstatic/amcdb.tfunc:Global.Main

    // Return TRUE if FIELD (in command line context) requires no argument
    // This is true for bool fields or fields with "emptyval" provided
    bool CmdArgValueRequiredQ(amc::FField &field);

    // True if field is a required command-line argument
    bool CmdArgRequiredQ(amc::FField &field);

    // Pick a field to extract enums from.
    // Handle the case of a single-field ctype with enums in it
    amc::FField *GetEnumField(amc::FField &field);
    tempstr GetUsageString(amc::FNs &ns, amc::FFcmdline &cmdline);

    // Return expression
    // $cpptype &NAME = $ns::$_db.$fieldname
    // where
    // $cpptype is the type of FIELD
    // $ns is the namespace of FIELD
    // $_db is the global instance in $ns
    // $fieldname is the name of the field
    tempstr VarRefToGlobal(amc::FField &field, strptr name);

    // Namespace ReadArgv function to read command line
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Global_ReadArgv(); // gstatic/amcdb.tfunc:Global.ReadArgv

    // -------------------------------------------------------------------
    // cpp/amc/gsymbol.cpp -- Generate strings from tables
    //

    // Generate regular c++ symbols from tables
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_gsymbol(); // gstatic/amcdb.gen:ns_gsymbol

    // -------------------------------------------------------------------
    // cpp/amc/hook.cpp -- Hook (function pointer)
    //

    // Determine if hook is static (i.e. parent ctype uses gstatic
    // to assign all hook values at start time).
    // Non-static hooks get a Set function.
    bool StaticQ(amc::FHook &hook);

    // Create a new ctype for the hook
    // Name construct is <ctype>_<field>_hook, similar in style to
    // that of cursors.
    void NewFieldHook(amc::FHook &hook);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_newfield_hook(); // gstatic/amcdb.gen:newfield_hook
    // void tclass_Hook(); // gstatic/amcdb.tclass:Hook
    // void tfunc_Hook_Set0(); // gstatic/amcdb.tfunc:Hook.Set0
    // void tfunc_Hook_Set1(); // gstatic/amcdb.tfunc:Hook.Set1
    // void tfunc_Hook_Set2(); // gstatic/amcdb.tfunc:Hook.Set2

    // Implement _Call function for the hook
    // void tfunc_Hook_Call(); // gstatic/amcdb.tfunc:Hook.Call
    bool InlaryQ(amc::FHook& hook);

    // -------------------------------------------------------------------
    // cpp/amc/include.cpp -- Manage includes for generated files
    //

    // Emit #pragma once for all headers and inline headers
    // Emit #includes for all cpp files
    // Emit include scan guards for all headers
    void GenInclude();

    // Determine .cpp and .h dependencies between namespaces based
    // on fields
    // I.e. if a struct in ns A includes by-value a struct from ns B,
    // then there is a header dependency between namespaces.
    // If struct in ns A has a Tary of structs from ns B,
    // then there is a cpp dependency. Etc.
    // Non-field dependencies are created by dispatch, pnew, and some cursors.
    void GenUsedNs();

    // Add .h dependencies to NS on PREREQ
    void AddHeaderDep(amc::FNs &ns, amc::FNs &prereq);

    // Add .cpp dependency to NS on PREREQ
    void AddSourceDep(amc::FNs &ns, amc::FNs &prereq);

    // Add .cpp and .h dependencies to NS on PREREQ
    void AddFullDep(amc::FNs &ns, amc::FNs &prereq);

    // Check if the header for NS includes header for OTHER
    bool HdrIncludesQ(amc::FNs &ns, amc::FNs &other);

    // -------------------------------------------------------------------
    // cpp/amc/inlary.cpp -- Inline array
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Inlary(); // gstatic/amcdb.tclass:Inlary
    // void tfunc_Inlary_AllocMem(); // gstatic/amcdb.tfunc:Inlary.AllocMem
    // void tfunc_Inlary_EmptyQ(); // gstatic/amcdb.tfunc:Inlary.EmptyQ
    // void tfunc_Inlary_Fill(); // gstatic/amcdb.tfunc:Inlary.Fill
    // void tfunc_Inlary_Find(); // gstatic/amcdb.tfunc:Inlary.Find
    // void tfunc_Inlary_Getary(); // gstatic/amcdb.tfunc:Inlary.Getary
    // void tfunc_Inlary_Init(); // gstatic/amcdb.tfunc:Inlary.Init
    // void tfunc_Inlary_Eq(); // gstatic/amcdb.tfunc:Inlary.Eq
    // void tfunc_Inlary_Cmp(); // gstatic/amcdb.tfunc:Inlary.Cmp
    // void tfunc_Inlary_Max(); // gstatic/amcdb.tfunc:Inlary.Max
    // void tfunc_Inlary_N(); // gstatic/amcdb.tfunc:Inlary.N
    // void tfunc_Inlary_RemoveAll(); // gstatic/amcdb.tfunc:Inlary.RemoveAll
    // void tfunc_Inlary_RemoveLast(); // gstatic/amcdb.tfunc:Inlary.RemoveLast
    // void tfunc_Inlary_RowidFind(); // gstatic/amcdb.tfunc:Inlary.RowidFind
    // void tfunc_Inlary_Setary(); // gstatic/amcdb.tfunc:Inlary.Setary
    // void tfunc_Inlary_Uninit(); // gstatic/amcdb.tfunc:Inlary.Uninit
    // void tfunc_Inlary_qFind(); // gstatic/amcdb.tfunc:Inlary.qFind
    // void tfunc_Inlary_rowid_Get(); // gstatic/amcdb.tfunc:Inlary.rowid_Get
    // void tfunc_Inlary_curs(); // gstatic/amcdb.tfunc:Inlary.curs

    // Read/Accumulate inline array from string.
    // For fixed arrays:
    // multiple reads into fixed array will leave old data behind
    // (e.g. reading 1 element after reading 2 elements will leave the 2nd element from initial read)
    // we don't clear the tail of the array with default values.
    //
    // arg:char or u8
    // the array is flushed befor reading
    // input is a string that is copied to the array.
    // if the input is too large, it is silently truncated
    // any other type, with separator:
    // the array is flushed before reading
    // input string is split on separator character, elements are appended one by one
    // if the input is too large, it is silently truncated
    // any other type, without separator:
    // one element is read from input string and appended to the array without flushing.
    // if the element doesn't fit, function returns false.
    // (for fixed array, element #0 is read)
    //
    // void tfunc_Inlary_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Inlary.ReadStrptrMaybe

    // Print array to string
    // char/u8 are printed as-is
    // pad_byte is ignored
    // all other types are printed separated by SEP
    // if none of the above conditions are present, the print function is not generated.
    // This function is reused by Tary and Varlen, so it cannot really assume
    // we're dealing with an Inlary
    // void tfunc_Inlary_Print(); // gstatic/amcdb.tfunc:Inlary.Print

    // -------------------------------------------------------------------
    // cpp/amc/io.cpp -- I/O functions
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Io_InputMaybe(); // gstatic/amcdb.tfunc:Io.InputMaybe
    // void tfunc_Io_Input(); // gstatic/amcdb.tfunc:Io.Input
    // void tfunc_Io_SaveSsimfile(); // gstatic/amcdb.tfunc:Io.SaveSsimfile
    // void tclass_Io(); // gstatic/amcdb.tclass:Io

    // Generate a name for the user-defined function based on the specified hook
    // I.e. amc::tclass_Io (hook name 'step' -- legacy -- all hooks before this feature
    // were called 'step' and function name didn't depend on hook name)
    // or amc::tclass_Io_newfield (hook name 'newfield')
    tempstr StaticHookName(amc::FField &field, strptr suffix);
    tempstr StaticHookPkey(amc::FField &field, strptr suffix);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Io_LoadStatic(); // gstatic/amcdb.tfunc:Io.LoadStatic

    // -------------------------------------------------------------------
    // cpp/amc/js.cpp
    //

    // add separator if lhs is not empty
    void MaybeSep(cstring &lhs, strptr sep = ", ");

    // whether ctype is js or ts
    bool JsQ(amc::FCtype &ctype);

    // whether ctype is ts
    bool TsQ(amc::FCtype &ctype);

    // compute javascript class name
    tempstr JsType(amc::FCtype &ctype);
    bool JsBooleanQ(amc::FField &field);
    bool JsNumberQ(algo::strptr arg);
    bool JsBigintQ(algo::strptr arg);
    bool JsStringQ(amc::FField &field);

    // only inline arrays of u8 and char are supported
    bool JsInlaryQ(amc::FField &field);
    bool JsByteArrayQ(amc::FField &field);
    bool JsVarlenStringQ(amc::FField &field);
    tempstr JsGetArg(amc::FField &field);

    // json-serialized type for fields
    tempstr JsJsonArg(amc::FField &field);

    // explicit cast to javascript type
    tempstr JsCastType(amc::FField &field);

    // compute expression for default value of field
    tempstr JsDefaultExpr(amc::FField &field);

    // Calculate if the field's type is locally defined in this namespace
    bool TsLocalTypeQ(amc::FField &field);

    // add import statement for name if not already in place
    void TsImportMaybe(amc::FField &field, strptr name);

    // Return a javscript expression evaluating NAME, which is of type CTYPE
    // to a number
    tempstr JsGetnumExpr(amc::FCtype &ctype, algo::strptr name);
    void JsGenBitfld();
    void TsGenEnum();
    void JsGenField();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_nsjs_field(); // gstatic/amcdb.gen:nsjs_field
    void TsEncodeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field);
    void TsGenEncode();
    void TsDecodeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field);
    void TsGenDecode();
    void TsGenJsonInterface();
    void JsGenCtype();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_nsjs_ctype(); // gstatic/amcdb.gen:nsjs_ctype
    void TsGenMsgtype();
    void JsGenNs();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_nsjs_ns(); // gstatic/amcdb.gen:nsjs_ns
    // void gen_nsjs_module(); // gstatic/amcdb.gen:nsjs_module

    // -------------------------------------------------------------------
    // cpp/amc/kafka.cpp
    //
    void KafkaCodec(int dir);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_KafkaEncode(); // gstatic/amcdb.tfunc:Ctype.KafkaEncode
    // void tfunc_Ctype_KafkaDecode(); // gstatic/amcdb.tfunc:Ctype.KafkaDecode

    // -------------------------------------------------------------------
    // cpp/amc/lary.cpp -- Level array with permanent pointers
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Lary(); // gstatic/amcdb.tclass:Lary
    // void tfunc_Lary_AllocMem(); // gstatic/amcdb.tfunc:Lary.AllocMem
    // void tfunc_Lary_EmptyQ(); // gstatic/amcdb.tfunc:Lary.EmptyQ
    // void tfunc_Lary_Find(); // gstatic/amcdb.tfunc:Lary.Find
    // void tfunc_Lary_Init(); // gstatic/amcdb.tfunc:Lary.Init
    // void tfunc_Lary_Last(); // gstatic/amcdb.tfunc:Lary.Last
    // void tfunc_Lary_N(); // gstatic/amcdb.tfunc:Lary.N
    // void tfunc_Lary_RemoveAll(); // gstatic/amcdb.tfunc:Lary.RemoveAll
    // void tfunc_Lary_RemoveLast(); // gstatic/amcdb.tfunc:Lary.RemoveLast
    // void tfunc_Lary_RowidFind(); // gstatic/amcdb.tfunc:Lary.RowidFind
    // void tfunc_Lary_Uninit(); // gstatic/amcdb.tfunc:Lary.Uninit
    // void tfunc_Lary_qFind(); // gstatic/amcdb.tfunc:Lary.qFind
    // void tfunc_Lary_qLast(); // gstatic/amcdb.tfunc:Lary.qLast
    // void tfunc_Lary_curs(); // gstatic/amcdb.tfunc:Lary.curs

    // -------------------------------------------------------------------
    // cpp/amc/llist.cpp -- Linked lists
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Llist(); // gstatic/amcdb.tclass:Llist
    // void tfunc_Llist_Cascdel(); // gstatic/amcdb.tfunc:Llist.Cascdel
    // void tfunc_Llist_DestructiveFirst(); // gstatic/amcdb.tfunc:Llist.DestructiveFirst
    // void tfunc_Llist_EmptyQ(); // gstatic/amcdb.tfunc:Llist.EmptyQ
    // void tfunc_Llist_First(); // gstatic/amcdb.tfunc:Llist.First
    // void tfunc_Llist_InLlistQ(); // gstatic/amcdb.tfunc:Llist.InLlistQ
    // void tfunc_Llist_Init(); // gstatic/amcdb.tfunc:Llist.Init
    // void tfunc_Llist_Last(); // gstatic/amcdb.tfunc:Llist.Last
    // void tfunc_Llist_N(); // gstatic/amcdb.tfunc:Llist.N
    // void tfunc_Llist_Next(); // gstatic/amcdb.tfunc:Llist.Next
    // void tfunc_Llist_Prev(); // gstatic/amcdb.tfunc:Llist.Prev
    // void tfunc_Llist_Remove(); // gstatic/amcdb.tfunc:Llist.Remove
    // void tfunc_Llist_RemoveAll(); // gstatic/amcdb.tfunc:Llist.RemoveAll
    // void tfunc_Llist_RemoveFirst(); // gstatic/amcdb.tfunc:Llist.RemoveFirst
    // void tfunc_Llist_RotateFirst(); // gstatic/amcdb.tfunc:Llist.RotateFirst
    // void tfunc_Llist_Insert(); // gstatic/amcdb.tfunc:Llist.Insert
    // void tfunc_Llist_qLast(); // gstatic/amcdb.tfunc:Llist.qLast
    // void tclass_ZSListMT(); // gstatic/amcdb.tclass:ZSListMT
    // void tfunc_ZSListMT_DestructiveFirst(); // gstatic/amcdb.tfunc:ZSListMT.DestructiveFirst
    // void tfunc_ZSListMT_RemoveFirst(); // gstatic/amcdb.tfunc:ZSListMT.RemoveFirst
    // void tfunc_ZSListMT_InLlistQ(); // gstatic/amcdb.tfunc:ZSListMT.InLlistQ
    // void tfunc_ZSListMT_Insert(); // gstatic/amcdb.tfunc:ZSListMT.Insert
    // void tfunc_ZSListMT_Remove(); // gstatic/amcdb.tfunc:ZSListMT.Remove
    // void tfunc_ZSListMT_Init(); // gstatic/amcdb.tfunc:ZSListMT.Init

    // Generate cursor for llist
    void Llist_curs(bool needdel);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Llist_curs(); // gstatic/amcdb.tfunc:Llist.curs
    // void tfunc_Llist_delcurs(); // gstatic/amcdb.tfunc:Llist.delcurs

    // -------------------------------------------------------------------
    // cpp/amc/lpool.cpp -- Variable-length free pool
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Lpool(); // gstatic/amcdb.tclass:Lpool
    // void tfunc_Lpool_FreeMem(); // gstatic/amcdb.tfunc:Lpool.FreeMem
    // void tfunc_Lpool_AllocMem(); // gstatic/amcdb.tfunc:Lpool.AllocMem
    // void tfunc_Lpool_ReserveBuffers(); // gstatic/amcdb.tfunc:Lpool.ReserveBuffers
    // void tfunc_Lpool_ReallocMem(); // gstatic/amcdb.tfunc:Lpool.ReallocMem
    // void tfunc_Lpool_Init(); // gstatic/amcdb.tfunc:Lpool.Init
    // void tfunc_Lpool_N(); // gstatic/amcdb.tfunc:Lpool.N

    // -------------------------------------------------------------------
    // cpp/amc/main.cpp -- Main driver
    //

    // Return TRUE if field is a padding field with no semantic significance
    bool PadQ(amc::FField &field);
    bool CanDeleteQ(amc::FCtype &ctype);

    // Check if this field is a Lenfld and return pointer
    // If not, return NULL
    amc::FLenfld *GetLenfld(amc::FField &field);

    // Compose expression for deleting child pointer CHILDREF
    // corresponding to FIELD, in the scope of parent PARENTREF.
    tempstr DeleteExpr(amc::FField &field, strptr parentref, strptr childref);
    amc::FField *GetViafld(amc::FXref &xref);
    amc::FField *GetKeyfld(amc::FXref &xref);
    void BeginNsBlock(cstring &out, amc::FNs &ns, strptr tag);
    void EndNsBlock(cstring &out, amc::FNs &ns, strptr tag);
    bool PtrQ(amc::FField &field);

    // make sure the specified type is forward-declared
    // in given namespace.
    void AddFwdDecl(amc::FNs &ns, amc::FCtype &ctype);
    tempstr Refto(strptr s);
    tempstr ConstRefto(strptr s);
    tempstr Castto(strptr type, strptr expr);
    bool CastsToU64Q(amc::FField& field);

    // True if ctype is instantiated through a memory pool.
    bool HasPoolQ(amc::FCtype &ctype);
    bool HasFinputsQ(amc::FNs &ns);
    tempstr PkeyCppident(algo::Tuple &tuple);
    amc::FField *FirstInst(amc::FCtype &ctype);

    // Return TRUE if the given step executes directly from the scheduling Step function
    // Indirect steps are those called through a Timehook.
    bool DirectStepQ(amc::FFstep &fstep);
    bool ExternStepQ(amc::FFstep &fstep);

    // add text to c++ body of class
    void InsStruct(algo_lib::Replscope &R, amc::FCtype *ctype, const algo::strptr &text);

    // Insert variable declaration into body of struct corresponding to CTYPE,.
    // NOTE: field must be explicitly initialized in Init function, this function
    // will not provide initialization in constructor!
    void InsVar(algo_lib::Replscope &R, amc::FCtype *ctype, const algo::strptr &type, const algo::strptr &name, const algo::strptr &dflt, const algo::strptr &comment);

    // True if ctype can be read from string
    bool ConstructFromStringQ(amc::FCtype &ctype);
    bool HasRemoveLastQ(amc::FReftype &reftype);
    bool GenThrowQ(amc::FNs &ns);
    amc::FThash *PrimaryIndex(amc::FCtype &ctype);

    // Compute expression needed to cast the default value (field.dflt) of
    // the field to the value that's being stored in the field
    // By default, this is just the field's cpp_type
    // But if the field has an fcast attached to it, it's the fcast expression
    tempstr Initcast(amc::FField &field);
    void GenPrintStmt(cstring &out, amc::FCtype &parenttype, amc::FField &field, strptr strname, strptr parentname);
    bool FldfuncQ(amc::FField &field);
    bool CanCopyQ(amc::FCtype &ctype);
    bool PoolVarlenQ(amc::FField &field);
    bool FieldStringQ(amc::FField &field);
    bool FieldNumericQ(amc::FField &field);

    // Convert constant string to case label
    // RHS       string to convert (1..8 chars)
    // RETURN    constant string converted to switch case label
    // !!! experimental -  export from amc_enum.cpp to amc_pool.cpp
    tempstr ConstStringToCaseLabel(strptr rhs);

    // Convert variable string to to switch value
    // NAME          variable name
    // LEN           length of string (1..8 chars)
    // RETURN        variable string converted to switch value
    tempstr VarStringToInteger(strptr name, i32 len);
    void GetMinMax(amc::FCtype& _ctype, u64 &min, u64 &max, bool &issigned);
    bool CheapCopyQ(amc::FField &field);
    bool HasFcast(amc::FCtype& ctype);

    // Determine if ctype should have a private constructor
    bool CopyPrivQ(amc::FCtype &ctype);

    // Return c++ type for rowid of FCtype
    // By default this is a u64, but if the struct has a field marked rowid,
    // then the type of that field is the row id type.
    tempstr EvalRowid(amc::FCtype &ctype);

    // True if given ctype is a global, i.e. it has only a single instance
    // of type Global.
    bool GlobalQ(amc::FCtype &ctype);

    // Pick a name with which to refer to a record of type CTYPE
    tempstr Refname(amc::FCtype &ctype, algo::strptr dflt = "parent");

    // Returns TRUE if the field is an inline value, except for Varlen and Opt
    bool ValQ(amc::FField &field);
    bool ComputedFieldQ(amc::FField &field);
    tempstr char_ToCppSingleQuote(char c);

    // return separator to be used for array field FIELD
    // if not found, return 0 (NUL)
    char GetSep(amc::FField &field);

    // Evaluate value of SSIM attribute as described by field FIELD
    // given tuple TUPLE
    tempstr EvalAttr(Tuple &tuple, amc::FField &field);
    i32 WidthMin(amc::FField &field);
    i32 WidthMax(amc::FField &field);
    bool IdentQ(strptr type);
    int Field_Sizeof(amc::FField &field);

    // Remove naming layers from ctype (i.e. as long as ctype contains
    // a single val field, keep going down, then finally return the basic type).
    // If the ctype contains more than one field at any given layer, return DFLT.
    amc::FCtype *StripWrappers(amc::FCtype &ctype, amc::FCtype *dflt);
    bool FixaryQ(amc::FField &field);

    // Return C++ expression for accessing the 'value' of the field
    // given parent reference PARNAME.
    // if CTYPE is NULL, it is assumed to be FIELD.P_CTYPE
    // The field may be an immediate field of CTYPE, or a field of one of the Val fields
    // of CTYPE; But the search must yield exactly one match, otherwise it is an error
    // If the field is accessed via a Get function, the corresponding expression is constructed.
    // i.e. the outputs of this function could be
    // parent.subfield.field
    // parent.field
    // field_Get(parent)
    // field_Get(parent.subfield)
    tempstr FieldvalExpr(amc::FCtype *ctype, amc::FField &field, strptr parname, strptr fldname = strptr());

    // Return C++ expression computing total length of ctype CTYPE
    // accessible via name NAME.
    tempstr LengthExpr(amc::FCtype &ctype, strptr name);

    // Return C++ expression assigning value VALUE to field FIELD
    // given parent reference PARNAME.
    // If NEEDS_CAST is set, a cast is added to the target type
    tempstr AssignExpr(amc::FField &field, strptr parname, strptr value, bool needs_cast, strptr fldname = strptr());

    // Heuristically determine if the type is a string type.
    // When in doubt, the function answers no.
    bool StringQ(amc::FCtype &ctype);

    // True if ctype contains a varlen field
    bool VarlenQ(amc::FCtype &ctype);

    // check if ctype has a string print function
    bool HasStringPrintQ(amc::FCtype &ctype);

    // check if ctype has an argv print function
    bool HasArgvPrintQ(amc::FCtype &ctype);

    // check if ctype has a string read function
    bool HasStringReadQ(amc::FCtype &ctype);
    bool HasReadQ(amc::FCtype &ctype);
    bool HasArgvReadQ(amc::FCtype &ctype);

    // Set IDENT to sanitized version of FROM
    // if MASK_KEYWORD is set, and FROM is a known c++ keyword, prepend '_' to it.
    // If FROM is an empty string, use "_"
    // Replace /, -, ., etc. with underscores.
    // Replace + with P, ' with A, " with Q (rationale?)
    // Example:
    // strptr_PrintCppIdent("abcd")   -> abcd
    // strptr_PrintCppIdent("ab.cd")  -> ab_cd
    // strptr_PrintCppIdent("+-$")    -> P__
    // strptr_PrintCppIdent("int")    -> _int
    // strptr_PrintCppIdent("")       -> _
    void strptr_PrintCppIdent(strptr from, cstring &out, bool mask_keyword);
    tempstr strptr_ToCppIdent(strptr s, bool mask_keyword);

    //
    // IDENT        identifier in question
    // RETURN       identifier with . replaced with ::
    //
    tempstr NsToCpp(strptr ident);
    tempstr NsTo_(strptr ident);

    // Return TRUE if the type is relational (ctype is in a ssimdb namespace)
    bool RelationalQ(amc::FCtype &ctype);

    // Return base class of ctype, DFLT if it doesn't have a base
    // Base is found by locating a field of reftype Base.
    amc::FCtype *GetBaseType(amc::FCtype &ctype, amc::FCtype *dflt);
    amc::FCtype *UltimateBaseType(amc::FCtype *ctype, amc::FCtype *dflt);
    tempstr SsimFilename(strptr root, amc::FCtype& ctype, bool do_throw);

    //
    // Append horizontal separator to STR
    //
    void CppSection(algo::cstring &out, strptr label, bool major);
    void CppSectionAll(amc::FNs& ns, strptr label);
    void TopoSortVisit(amc::FNs& ns, amc::FCtype& ctype);
    bool ExeQ(amc::FNs &ns);

    // Return C++ expression for the arg type for ctype CTYPE
    // argtype is how the type is passed to a function
    // If the type is "cheap copy", the value is type itself (T). Otherwise
    // it is T&.
    // The optional ISCONST argument marks the expression as const
    // For cheap types, 'const' is omitted since there is no sense in protecting
    // a copy.
    tempstr ByvalArgtype(amc::FCtype &ctype, bool isconst = false);
    void Main_CloneFconst_Field(amc::FField &field);

    // HOW IS THIS DIFFERENT FROM INSFIELD?
    void Field_AddChild(amc::FField &parent, const dmmeta::Field &field);
    bool NeedSetQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cppstack(); // gstatic/amcdb.tclass:Cppstack
    // void tclass_Pkey(); // gstatic/amcdb.tclass:Pkey
    int c_anonfld_N(amc::FCtype &ctype);

    // Go over `amcdb.gen` table and invoke each global generator
    // Collect performance data long the way
    // This is where amc spends most of the time. See the `amcdb.gen` table for more information.
    void Main_Gen();

    // OUTPUT        Generate files for the namespace(s)
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:amc

    // -------------------------------------------------------------------
    // cpp/amc/malloc.cpp -- Malloc allocator
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Malloc(); // gstatic/amcdb.tclass:Malloc
    // void tfunc_Malloc_AllocMem(); // gstatic/amcdb.tfunc:Malloc.AllocMem
    // void tfunc_Malloc_ReallocMem(); // gstatic/amcdb.tfunc:Malloc.ReallocMem
    // void tfunc_Malloc_FreeMem(); // gstatic/amcdb.tfunc:Malloc.FreeMem

    // -------------------------------------------------------------------
    // cpp/amc/msgcurs.cpp -- Message scanning cursor
    //

    // Generate a message-scanning cursor for each message header with length
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_msgcurs(); // gstatic/amcdb.gen:msgcurs

    // -------------------------------------------------------------------
    // cpp/amc/nstype.cpp -- Namespace types
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ssimdb(); // gstatic/amcdb.gen:ssimdb
    // void gen_ns_check_nstype(); // gstatic/amcdb.gen:ns_check_nstype

    // -------------------------------------------------------------------
    // cpp/amc/numstr.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Numstr(); // gstatic/amcdb.tclass:Numstr
    // void tfunc_Numstr_Getnum(); // gstatic/amcdb.tfunc:Numstr.Getnum
    // void tfunc_Numstr_GetnumDflt(); // gstatic/amcdb.tfunc:Numstr.GetnumDflt
    // void tfunc_Numstr_Geti64(); // gstatic/amcdb.tfunc:Numstr.Geti64
    // void tfunc_Numstr_SetnumMaybe(); // gstatic/amcdb.tfunc:Numstr.SetnumMaybe
    // void tfunc_Numstr_ForAllStrings(); // gstatic/amcdb.tfunc:Numstr.ForAllStrings
    // void gen_parsenum(); // gstatic/amcdb.gen:parsenum

    // -------------------------------------------------------------------
    // cpp/amc/opt.cpp -- Opt reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Opt(); // gstatic/amcdb.tclass:Opt
    // void tfunc_Opt_Get(); // gstatic/amcdb.tfunc:Opt.Get
    // void tfunc_Opt_Getary(); // gstatic/amcdb.tfunc:Opt.Getary
    // void tfunc_Opt_Print(); // gstatic/amcdb.tfunc:Opt.Print
    // void tfunc_Opt_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Opt.ReadStrptrMaybe

    // -------------------------------------------------------------------
    // cpp/amc/outfile.cpp -- Ouptut functions
    //

    // Write output file to disk
    // and deallocate memory associated with it
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_write(); // gstatic/amcdb.gen:ns_write

    // Create outfile record for specified filename
    // T here is one outfile per generated output file
    amc::FOutfile &outfile_Create(strptr filename);

    // -------------------------------------------------------------------
    // cpp/amc/pmask.cpp -- Presence masks
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_pmask(); // gstatic/amcdb.gen:pmask
    // void tclass_Pmask(); // gstatic/amcdb.tclass:Pmask

    // Create multiple functions, one for each pmask of which this field is a member
    // void tfunc_Pmask_PresentQ(); // gstatic/amcdb.tfunc:Pmask.PresentQ

    // Return C++ expression(s) setting the present bit for the field
    // in all presence masks of which the field is a member
    // If no presence masks are defined, return empty string;
    tempstr SetPresentExpr(amc::FField &field, strptr parent);

    // Create multiple functions, one for each pmask of which this field is a member
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Pmask_SetPresent(); // gstatic/amcdb.tfunc:Pmask.SetPresent

    // Create multiple functions, one for each pmask of which this field is a member
    // void tfunc_Pmask_GetBit(); // gstatic/amcdb.tfunc:Pmask.GetBit

    // Return FPmaskfld which filters printing for ctype CTYPE
    // NULL if none
    amc::FPmaskfld *GetPrintFilter(amc::FCtype &ctype);

    // Find PMASKFLD_MEMBER record for field FIELD and pmask PMASKFLD
    // NULL if none
    amc::FPmaskfldMember *FindMember(amc::FField &field, amc::FPmaskfld *pmaskfld);

    // -------------------------------------------------------------------
    // cpp/amc/pnew.cpp -- Custom constructors
    //

    // Generate various constructors
    void GenPnew(amc::FNs& ns, amc::FPnew& pnew, amc::FCtype& ctype);

    // -------------------------------------------------------------------
    // cpp/amc/pool.cpp -- Generic pool functions
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Pool(); // gstatic/amcdb.tclass:Pool
    // void tfunc_Pool_AllocMaybe(); // gstatic/amcdb.tfunc:Pool.AllocMaybe
    // void tfunc_Pool_Alloc(); // gstatic/amcdb.tfunc:Pool.Alloc

    // Generate Alloc function that takes 2 additional arguments
    // -- pointer to extra (varlen portion) memory to tack onto the record,
    // and number of bytes
    // void tfunc_Pool_AllocExtraMaybe(); // gstatic/amcdb.tfunc:Pool.AllocExtraMaybe

    // Like AllocExtraMaybe, but die on out-of-memory
    // void tfunc_Pool_AllocExtra(); // gstatic/amcdb.tfunc:Pool.AllocExtra

    // VarlenMaybe -- array of fixed-size structs tacked onto the end of a struct
    // void tfunc_Pool_AllocVarlenMaybe(); // gstatic/amcdb.tfunc:Pool.AllocVarlenMaybe

    // Like VarlenMaybe, but die on out-of-memory
    // void tfunc_Pool_AllocVarlen(); // gstatic/amcdb.tfunc:Pool.AllocVarlen
    // void tfunc_Pool_InsertMaybe(); // gstatic/amcdb.tfunc:Pool.InsertMaybe
    amc::FField *FindFieldByName(amc::FCtype &ctype, algo::strptr name);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Pool_UpdateMaybe(); // gstatic/amcdb.tfunc:Pool.UpdateMaybe
    // void tfunc_Pool_Delete(); // gstatic/amcdb.tfunc:Pool.Delete

    // Ensure that all instances of CTYPE's record are varlen pools
    void EnsureVarlenPool(amc::FCtype &ctype);

    // Find pool used for allocating values of type CTYPE
    amc::FField *FindPool(amc::FCtype &ctype);

    // Find default used for allocating things in namespace NS
    amc::FField *DefaultPool(amc::FNs &ns);

    // -------------------------------------------------------------------
    // cpp/amc/print.cpp -- Print ctype to string
    //
    tempstr CheckDfltExpr(algo_lib::Replscope &R, amc::FField &field, strptr text, bool canskip);
    void GenPrintJson(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &, amc::FFunc &fmtjson);
    void GenPrint(amc::FCtype &parent, amc::FCfmt &cfmt);

    // -------------------------------------------------------------------
    // cpp/amc/protocol.cpp -- Protocol functions
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ns(); // gstatic/amcdb.tclass:Ns
    // void tfunc_Ns_StaticCheck(); // gstatic/amcdb.tfunc:Ns.StaticCheck

    // -------------------------------------------------------------------
    // cpp/amc/ptr.cpp -- Ptr reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ptr(); // gstatic/amcdb.tclass:Ptr
    // void tfunc_Ptr_Init(); // gstatic/amcdb.tfunc:Ptr.Init
    // void tfunc_Ptr_InsertMaybe(); // gstatic/amcdb.tfunc:Ptr.InsertMaybe
    // void tfunc_Ptr_Remove(); // gstatic/amcdb.tfunc:Ptr.Remove
    // void tfunc_Ptr_Cascdel(); // gstatic/amcdb.tfunc:Ptr.Cascdel

    // -------------------------------------------------------------------
    // cpp/amc/ptrary.cpp -- Ptrary reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ptrary(); // gstatic/amcdb.tclass:Ptrary
    // void tfunc_Ptrary_Cascdel(); // gstatic/amcdb.tfunc:Ptrary.Cascdel
    // void tfunc_Ptrary_EmptyQ(); // gstatic/amcdb.tfunc:Ptrary.EmptyQ
    // void tfunc_Ptrary_Find(); // gstatic/amcdb.tfunc:Ptrary.Find
    // void tfunc_Ptrary_InAryQ(); // gstatic/amcdb.tfunc:Ptrary.InAryQ
    // void tfunc_Ptrary_qFind(); // gstatic/amcdb.tfunc:Ptrary.qFind
    // void tfunc_Ptrary_qLast(); // gstatic/amcdb.tfunc:Ptrary.qLast
    // void tfunc_Ptrary_Getary(); // gstatic/amcdb.tfunc:Ptrary.Getary
    // void tfunc_Ptrary_Init(); // gstatic/amcdb.tfunc:Ptrary.Init
    // void tfunc_Ptrary_Insert(); // gstatic/amcdb.tfunc:Ptrary.Insert
    // void tfunc_Ptrary_InsertMaybe(); // gstatic/amcdb.tfunc:Ptrary.InsertMaybe
    // void tfunc_Ptrary_ScanInsertMaybe(); // gstatic/amcdb.tfunc:Ptrary.ScanInsertMaybe
    // void tfunc_Ptrary_N(); // gstatic/amcdb.tfunc:Ptrary.N
    // void tfunc_Ptrary_Remove(); // gstatic/amcdb.tfunc:Ptrary.Remove
    // void tfunc_Ptrary_RemoveFirst(); // gstatic/amcdb.tfunc:Ptrary.RemoveFirst
    // void tfunc_Ptrary_First(); // gstatic/amcdb.tfunc:Ptrary.First
    // void tfunc_Ptrary_RemoveLast(); // gstatic/amcdb.tfunc:Ptrary.RemoveLast
    // void tfunc_Ptrary_Last(); // gstatic/amcdb.tfunc:Ptrary.Last
    // void tfunc_Ptrary_RemoveAll(); // gstatic/amcdb.tfunc:Ptrary.RemoveAll
    // void tfunc_Ptrary_Reserve(); // gstatic/amcdb.tfunc:Ptrary.Reserve
    // void tfunc_Ptrary_Uninit(); // gstatic/amcdb.tfunc:Ptrary.Uninit
    void Ptrary_curs(bool once);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ptrary_curs(); // gstatic/amcdb.tfunc:Ptrary.curs
    // void tfunc_Ptrary_oncecurs(); // gstatic/amcdb.tfunc:Ptrary.oncecurs

    // -------------------------------------------------------------------
    // cpp/amc/query.cpp -- Query mode
    //
    void Main_Querymode();

    // True if amc was invoked in query-only mode
    bool QueryModeQ();

    // -------------------------------------------------------------------
    // cpp/amc/read.cpp -- Read ctype from string
    //

    // Dispatach on field name, and read appropriate field
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_ReadFieldMaybe(); // gstatic/amcdb.tfunc:Ctype.ReadFieldMaybe
    void GenRead(amc::FCtype &ctype, amc::FCfmt &cfmt);

    // -------------------------------------------------------------------
    // cpp/amc/regx.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_RegxSql(); // gstatic/amcdb.tclass:RegxSql
    // void tfunc_RegxSql_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:RegxSql.ReadStrptrMaybe
    // void tfunc_RegxSql_Print(); // gstatic/amcdb.tfunc:RegxSql.Print
    // void tfunc_RegxSql_Init(); // gstatic/amcdb.tfunc:RegxSql.Init
    // void tclass_Regx(); // gstatic/amcdb.tclass:Regx
    // void tfunc_Regx_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Regx.ReadStrptrMaybe
    // void tfunc_Regx_Print(); // gstatic/amcdb.tfunc:Regx.Print
    // void tfunc_Regx_Init(); // gstatic/amcdb.tfunc:Regx.Init

    // -------------------------------------------------------------------
    // cpp/amc/sbrk.cpp -- Sbrk allocator
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Sbrk(); // gstatic/amcdb.tclass:Sbrk
    // void tfunc_Sbrk_AllocMem(); // gstatic/amcdb.tfunc:Sbrk.AllocMem
    // void tfunc_Sbrk_FreeMem(); // gstatic/amcdb.tfunc:Sbrk.FreeMem
    // void tfunc_Sbrk_Init(); // gstatic/amcdb.tfunc:Sbrk.Init

    // -------------------------------------------------------------------
    // cpp/amc/signature.cpp -- Signature calculation for protocols
    //

    // Recursive function to compute all ctype dependencies
    // (transitive/reflexive closure for ctype on fields)
    // Global purpose is to detect the change in deep bottom type
    // which impacts binary footprint of the top type
    void SignatureVisit(amc::FCtype &ctype);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_signature(); // gstatic/amcdb.gen:prep_signature

    // compute order-independent superposition of two signatures
    // that means H(A,B) == H(B,A)
    // sum byte-wise without carry
    // exception if length does not match
    void CombineSignaturesUnordered(algo::Sha1sig &dst, algo::Sha1sig src);

    // -------------------------------------------------------------------
    // cpp/amc/size.cpp -- Compute struct sizes
    //

    // Query whether CTYPE fields should be packed (field alignment=1)
    bool PackQ(amc::FCtype &ctype);

    // Recursively compute sizes of all ctypes,
    // and generate per-namespace SizeCheck function,
    // asserting that amc-computed sizes are the same as gcc-computed sizes.
    // The sizes are computed by scanning the actual generated struct for each ctype.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_compute_size(); // gstatic/amcdb.gen:compute_size

    // Generate symbol corresponding to the size of ctype CTYPE.
    // SizeEnums are generated only for occasional ctypes, and are used
    // to avoid circular dependencies in headers
    tempstr SizeEnum(amc::FCtype &ctype);

    // Generate compile-time constants for sizes of certain ctypes
    // accessed from namespace NS, without having to include corresponding header.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_size_enums(); // gstatic/amcdb.gen:ns_size_enums

    // -------------------------------------------------------------------
    // cpp/amc/smallstr.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Smallstr(); // gstatic/amcdb.tclass:Smallstr
    // void tfunc_Smallstr_Init(); // gstatic/amcdb.tfunc:Smallstr.Init
    // void tfunc_Smallstr_Getary(); // gstatic/amcdb.tfunc:Smallstr.Getary
    // void tfunc_Smallstr_Add(); // gstatic/amcdb.tfunc:Smallstr.Add
    // void tfunc_Smallstr_AddStrptr(); // gstatic/amcdb.tfunc:Smallstr.AddStrptr
    // void tfunc_Smallstr_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Smallstr.ReadStrptrMaybe
    // void tfunc_Smallstr_Print(); // gstatic/amcdb.tfunc:Smallstr.Print

    // compute length
    // void tfunc_Smallstr_N(); // gstatic/amcdb.tfunc:Smallstr.N

    // Max # of elements (constant)
    // void tfunc_Smallstr_Max(); // gstatic/amcdb.tfunc:Smallstr.Max

    // Set value as strptr
    // For a padded string, the string value is allowed to use the pad character
    // inside the string, i.e. a space-padded field can have a space ("abc def").
    // Length of a padded string is determined by stripping the padded characters
    // from the appropriate end.
    // void tfunc_Smallstr_SetStrptr(); // gstatic/amcdb.tfunc:Smallstr.SetStrptr

    // Assignment operator from strptr
    // Generated only if the containing struct has only one field
    // void tfunc_Smallstr_AssignStrptr(); // gstatic/amcdb.tfunc:Smallstr.AssignStrptr

    // Construct from strptr
    // Generated only if the containing struct has only one field
    // void tfunc_Smallstr_CtorStrptr(); // gstatic/amcdb.tfunc:Smallstr.CtorStrptr

    // -------------------------------------------------------------------
    // cpp/amc/sort.cpp -- Comparison & Sorting routines
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Sort(); // gstatic/amcdb.tclass:Sort
    // void tfunc_Sort_Swap(); // gstatic/amcdb.tfunc:Sort.Swap
    // void tfunc_Sort_Rotleft(); // gstatic/amcdb.tfunc:Sort.Rotleft
    // void tfunc_Sort_Lt(); // gstatic/amcdb.tfunc:Sort.Lt
    // void tfunc_Sort_SortedQ(); // gstatic/amcdb.tfunc:Sort.SortedQ
    // void tfunc_Sort_IntInsertionSort(); // gstatic/amcdb.tfunc:Sort.IntInsertionSort
    // void tfunc_Sort_IntHeapSort(); // gstatic/amcdb.tfunc:Sort.IntHeapSort
    // void tfunc_Sort_IntQuickSort(); // gstatic/amcdb.tfunc:Sort.IntQuickSort
    // void tfunc_Sort_InsertionSort(); // gstatic/amcdb.tfunc:Sort.InsertionSort
    // void tfunc_Sort_HeapSort(); // gstatic/amcdb.tfunc:Sort.HeapSort
    // void tfunc_Sort_QuickSort(); // gstatic/amcdb.tfunc:Sort.QuickSort

    // -------------------------------------------------------------------
    // cpp/amc/step.cpp -- Step functions
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Step(); // gstatic/amcdb.tclass:Step
    // void tfunc_Step_UpdateCycles(); // gstatic/amcdb.tfunc:Step.UpdateCycles
    // void tfunc_Step_Step(); // gstatic/amcdb.tfunc:Step.Step
    // void tfunc_Step_Init(); // gstatic/amcdb.tfunc:Step.Init
    // void tfunc_Step_Call(); // gstatic/amcdb.tfunc:Step.Call
    // void tfunc_Step_FirstChanged(); // gstatic/amcdb.tfunc:Step.FirstChanged
    // void tfunc_Step_SetDelay(); // gstatic/amcdb.tfunc:Step.SetDelay

    // -------------------------------------------------------------------
    // cpp/amc/struct.cpp -- C++ struct output
    //

    // True if the field can be passed via constructor
    bool PassFieldViaArgQ(amc::FField &field, amc::FCtype &ctype);
    void GenStruct(amc::FNs& ns, amc::FCtype& ctype);

    // -------------------------------------------------------------------
    // cpp/amc/substr.cpp -- Substr fields
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Substr(); // gstatic/amcdb.tclass:Substr
    // void tfunc_Substr_Get(); // gstatic/amcdb.tfunc:Substr.Get
    // void tfunc_Substr_Get2(); // gstatic/amcdb.tfunc:Substr.Get2

    // -------------------------------------------------------------------
    // cpp/amc/tableid.cpp -- Per-namespace enum of tables
    //
    bool HasFinputQ(amc::FCtype &ctype);
    void GenTableId(amc::FNs &ns);

    // create TableId type.
    // generate an enum representing tables in the in-memory database
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_tableenum(); // gstatic/amcdb.gen:tableenum
    void GenFieldId(amc::FNs &ns);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_fieldid(); // gstatic/amcdb.gen:fieldid

    // -------------------------------------------------------------------
    // cpp/amc/tary.cpp -- Tary (vector) reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Tary(); // gstatic/amcdb.tclass:Tary
    // void tfunc_Tary_Addary(); // gstatic/amcdb.tfunc:Tary.Addary
    // void tfunc_Tary_Alloc(); // gstatic/amcdb.tfunc:Tary.Alloc
    // void tfunc_Tary_AllocAt(); // gstatic/amcdb.tfunc:Tary.AllocAt
    // void tfunc_Tary_AllocN(); // gstatic/amcdb.tfunc:Tary.AllocN
    // void tfunc_Tary_AllocNAt(); // gstatic/amcdb.tfunc:Tary.AllocNAt
    // void tfunc_Tary_AllocNVal(); // gstatic/amcdb.tfunc:Tary.AllocNVal
    // void tfunc_Tary_EmptyQ(); // gstatic/amcdb.tfunc:Tary.EmptyQ
    // void tfunc_Tary_Find(); // gstatic/amcdb.tfunc:Tary.Find
    // void tfunc_Tary_Getary(); // gstatic/amcdb.tfunc:Tary.Getary
    // void tfunc_Tary_Init(); // gstatic/amcdb.tfunc:Tary.Init
    // void tfunc_Tary_Last(); // gstatic/amcdb.tfunc:Tary.Last
    // void tfunc_Tary_Max(); // gstatic/amcdb.tfunc:Tary.Max
    // void tfunc_Tary_N(); // gstatic/amcdb.tfunc:Tary.N
    // void tfunc_Tary_Remove(); // gstatic/amcdb.tfunc:Tary.Remove
    // void tfunc_Tary_RemoveAll(); // gstatic/amcdb.tfunc:Tary.RemoveAll
    // void tfunc_Tary_RemoveLast(); // gstatic/amcdb.tfunc:Tary.RemoveLast
    // void tfunc_Tary_AbsReserve(); // gstatic/amcdb.tfunc:Tary.AbsReserve
    // void tfunc_Tary_Reserve(); // gstatic/amcdb.tfunc:Tary.Reserve
    // void tfunc_Tary_RowidFind(); // gstatic/amcdb.tfunc:Tary.RowidFind
    // void tfunc_Tary_Setary(); // gstatic/amcdb.tfunc:Tary.Setary
    // void tfunc_Tary_Setary2(); // gstatic/amcdb.tfunc:Tary.Setary2
    // void tfunc_Tary_Uninit(); // gstatic/amcdb.tfunc:Tary.Uninit
    // void tfunc_Tary_qFind(); // gstatic/amcdb.tfunc:Tary.qFind
    // void tfunc_Tary_qLast(); // gstatic/amcdb.tfunc:Tary.qLast
    // void tfunc_Tary_rowid_Get(); // gstatic/amcdb.tfunc:Tary.rowid_Get
    // void tfunc_Tary_Eq(); // gstatic/amcdb.tfunc:Tary.Eq
    // void tfunc_Tary_Cmp(); // gstatic/amcdb.tfunc:Tary.Cmp
    // void tfunc_Tary_curs(); // gstatic/amcdb.tfunc:Tary.curs

    // Read/Accumulate Tary from string.
    // arg:char & U8
    // the array is flushed befor reading
    // input is a string that is copied to the array.
    // any other type, with separator:
    // the array is flushed before reading
    // input string is split on separator character, elements are appended one by one
    // if any element cannot be read, function returns false
    // (but array retains values read so far)
    // any other type, without separator:
    // one element is read from input string and appended to the array without flushing.
    // if the element cannot be read, the array is unchanged
    //
    // void tfunc_Tary_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Tary.ReadStrptrMaybe
    // void tfunc_Tary_Print(); // gstatic/amcdb.tfunc:Tary.Print

    // Assignment operator from aryptr
    // Generated only if the containing struct has only one field
    // void tfunc_Tary_AssignAryptr(); // gstatic/amcdb.tfunc:Tary.AssignAryptr

    // Construct from aryptr
    // Generated only if the containing struct has only one field
    // void tfunc_Tary_CtorAryptr(); // gstatic/amcdb.tfunc:Tary.CtorAryptr
    // void tfunc_Tary_Insary(); // gstatic/amcdb.tfunc:Tary.Insary

    // -------------------------------------------------------------------
    // cpp/amc/tclass.cpp -- Driver for tfuncs
    //
    void ResetVars(amc::Genctx &ctx);

    // Call tclass, tfunc, and cursor generators for this template
    // Context is provided via _db.genctx:
    // - genctx.p_field   --- field pointer (NULL for tclass, cursor, and Ctype, set for individual fields)
    // - genctx.p_ctype   --- current ctype, never NULL
    // - genctx.p_tfunc   --- pointer to tfunc, never NULL
    // First, the tclass function is called
    // Then, for each tfunc, its function is called
    // The tfunc may be a cursor generator (as indicated by tcurs table)
    // If it's a cursor generator, then a forward-declaration for the cursor is created.
    // In this case, variables $fcurs (cursor key) and $curstype (cursor type) are set.
    void GenTclass(amc::FTclass &tclass);

    // Get pointer to the field's memory pool
    // Any field can be assigned a custom memory pool with the basepool table.
    // Field doesn't have a custom pool, then the namespace custom pool (as specified in nsx table)
    // is used.
    amc::FField *GetBasepool(amc::FField &field);

    // Call tfunc generators for every field in this ctype
    // Each field triggers zero or more tclass generators
    // (template class, no relation to C++ notion of template or class)
    // based on its type and associated records, and each tclass generates zero or more tfuncs
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_tclass_field(); // gstatic/amcdb.gen:ns_tclass_field

    // Call tfunc generators without field context (Ctype generators)
    // This must be called after field-specific generators, since by this time
    // ctype sizes have been computed.
    // void gen_ns_tclass_ctype(); // gstatic/amcdb.gen:ns_tclass_ctype
    // void gen_ns_tclass_ns(); // gstatic/amcdb.gen:ns_tclass_ns

    // -------------------------------------------------------------------
    // cpp/amc/thash.cpp -- Hash tables
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Thash(); // gstatic/amcdb.tclass:Thash
    // void tfunc_Thash_Find(); // gstatic/amcdb.tfunc:Thash.Find
    // void tfunc_Thash_FindX(); // gstatic/amcdb.tfunc:Thash.FindX
    // void tfunc_Thash_Reserve(); // gstatic/amcdb.tfunc:Thash.Reserve
    // void tfunc_Thash_AbsReserve(); // gstatic/amcdb.tfunc:Thash.AbsReserve
    // void tfunc_Thash_GetOrCreate(); // gstatic/amcdb.tfunc:Thash.GetOrCreate
    // void tfunc_Thash_N(); // gstatic/amcdb.tfunc:Thash.N
    // void tfunc_Thash_EmptyQ(); // gstatic/amcdb.tfunc:Thash.EmptyQ
    // void tfunc_Thash_InsertMaybe(); // gstatic/amcdb.tfunc:Thash.InsertMaybe
    // void tfunc_Thash_Cascdel(); // gstatic/amcdb.tfunc:Thash.Cascdel
    // void tfunc_Thash_Remove(); // gstatic/amcdb.tfunc:Thash.Remove
    // void tfunc_Thash_FindRemove(); // gstatic/amcdb.tfunc:Thash.FindRemove
    // void tfunc_Thash_Init(); // gstatic/amcdb.tfunc:Thash.Init
    // void tfunc_Thash_Uninit(); // gstatic/amcdb.tfunc:Thash.Uninit
    // void tfunc_Thash_curs(); // gstatic/amcdb.tfunc:Thash.curs

    // -------------------------------------------------------------------
    // cpp/amc/tpool.cpp -- Tpool refetype (fixed-length freelist)
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Tpool(); // gstatic/amcdb.tclass:Tpool
    // void tfunc_Tpool_ReserveMem(); // gstatic/amcdb.tfunc:Tpool.ReserveMem
    // void tfunc_Tpool_Reserve(); // gstatic/amcdb.tfunc:Tpool.Reserve
    // void tfunc_Tpool_AllocMem(); // gstatic/amcdb.tfunc:Tpool.AllocMem
    // void tfunc_Tpool_FreeMem(); // gstatic/amcdb.tfunc:Tpool.FreeMem
    // void tfunc_Tpool_Init(); // gstatic/amcdb.tfunc:Tpool.Init

    // -------------------------------------------------------------------
    // cpp/amc/trace.cpp -- Generate trace code
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_trace(); // gstatic/amcdb.gen:trace

    // -------------------------------------------------------------------
    // cpp/amc/upptr.cpp -- Upptr reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Upptr(); // gstatic/amcdb.tclass:Upptr
    // void tfunc_Upptr_Init(); // gstatic/amcdb.tfunc:Upptr.Init

    // -------------------------------------------------------------------
    // cpp/amc/val.cpp -- Val reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Val(); // gstatic/amcdb.tclass:Val
    // void tfunc_Val_Get(); // gstatic/amcdb.tfunc:Val.Get
    // void tfunc_Val_Set(); // gstatic/amcdb.tfunc:Val.Set
    // void tfunc_Val_Init(); // gstatic/amcdb.tfunc:Val.Init
    // void tfunc_Val_RowidFind(); // gstatic/amcdb.tfunc:Val.RowidFind
    // void tfunc_Val_N(); // gstatic/amcdb.tfunc:Val.N

    // -------------------------------------------------------------------
    // cpp/amc/varlen.cpp -- Varlen reftype
    //
    amc::FField *LengthField(amc::FCtype &ctype);
    tempstr LengthType(amc::FCtype &ctype);
    tempstr VarlenEndName(amc::FField &field);
    tempstr VarlenEndExpr(strptr parname, amc::FField &field);
    tempstr VarlenEndAssign(strptr parname, amc::FField &field, strptr value);
    tempstr VarlenEndIncr(strptr parname, amc::FField &field, strptr incr);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Varlen(); // gstatic/amcdb.tclass:Varlen
    // void tfunc_Varlen_Addr(); // gstatic/amcdb.tfunc:Varlen.Addr
    // void tfunc_Varlen_Getary(); // gstatic/amcdb.tfunc:Varlen.Getary
    // void tfunc_Varlen_N(); // gstatic/amcdb.tfunc:Varlen.N
    // void tfunc_Varlen_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Varlen.ReadStrptrMaybe
    // void tfunc_Varlen_curs(); // gstatic/amcdb.tfunc:Varlen.curs
    // void tfunc_Varlen_Print(); // gstatic/amcdb.tfunc:Varlen.Print
    // void tfunc_Varlen_Init(); // gstatic/amcdb.tfunc:Varlen.Init
}
