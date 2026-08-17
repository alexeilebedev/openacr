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

// Report the schema defect that X does not hold, and count it.
// Counting keeps a pass from lowering a code an earlier pass raised. The step
// loop notices a failing pass by the exit code rising, so a pass that printed
// two defects and assigned 1 behind a pass that had already raised the code to
// 2 read as clean, and its two lines went out under a code that had not moved.
// Assignment is how many other checks in amc still leave the code, so the code
// a run exits with is not a count of every defect it printed; the generated main
// clamps it at 255 either way.
// amc.BadOptDtor holds two of these defects alone and exits 2, and
// amc.SideloadNossimfile is the same rule read through four of another kind.
#define amccheck(x,y) { if (!(x)) { prerr(y); algo_lib::_db.exit_code++; } }

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
    void CloneFields(amc::FCtype &from, amc::FCtype &to, double next_rowid);

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

    // Validate the bitfield width (1..64) and set up the substitution
    // variables shared by the bitfield accessors: source field, bit offset,
    // width, mask, and the integer type the mask operations run on.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitfld(); // gstatic/amcdb.tclass:Bitfld

    // Generate the bitfield's contribution to the parent's Init: a Set call
    // storing the field default, emitted only when a default exists.
    // void tfunc_Bitfld_Init(); // gstatic/amcdb.tfunc:Bitfld.Init

    // Generate the bitfield getter: shift and mask the source field's value
    // to extract the field's bits.
    // void tfunc_Bitfld_Get(); // gstatic/amcdb.tfunc:Bitfld.Get

    // Generate the bitfield setter: replace the field's bits within the
    // source field's value, leaving the other bits unchanged.
    // void tfunc_Bitfld_Set(); // gstatic/amcdb.tfunc:Bitfld.Set

    // -------------------------------------------------------------------
    // cpp/amc/bitset.cpp -- Bit sets
    //

    // Check that the bitset element is an unsigned integer builtin of a width the
    // accessors can index, and set up the substitution variables shared by the
    // bitset accessors: element width, the index shift and mask splitting a bit
    // index, and the bit-scan width.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitset(); // gstatic/amcdb.tclass:Bitset

    // Generate the element-count function for a single-element bitset (the
    // constant 1); an array-backed bitset takes it from the array reftype.
    // void tfunc_Bitset_N(); // gstatic/amcdb.tfunc:Bitset.N

    // Generate the element accessor for a single-element bitset: return the
    // value itself, ignoring the index; arrays take it from the array reftype.
    // void tfunc_Bitset_qFind(); // gstatic/amcdb.tfunc:Bitset.qFind

    // Generate the capacity function: number of bits the bitset holds
    // (element count times element width).
    // void tfunc_Bitset_NBits(); // gstatic/amcdb.tfunc:Bitset.NBits

    // Generate the unchecked bit read: fetch one bit without bounds checking.
    // void tfunc_Bitset_qGetBit(); // gstatic/amcdb.tfunc:Bitset.qGetBit

    // Generate the checked bit read: fetch one bit, returning false when the
    // bit index is out of bounds.
    // void tfunc_Bitset_GetBit(); // gstatic/amcdb.tfunc:Bitset.GetBit

    // Generate the population count: total number of set bits over all elements.
    // void tfunc_Bitset_Sum1s(); // gstatic/amcdb.tfunc:Bitset.Sum1s

    // Generate the emptiness predicate: true when every bit is zero.
    // void tfunc_Bitset_BitsEmptyQ(); // gstatic/amcdb.tfunc:Bitset.BitsEmptyQ

    // Generate the unchecked bit clear: zero one bit without bounds checking.
    // void tfunc_Bitset_qClearBit(); // gstatic/amcdb.tfunc:Bitset.qClearBit

    // Generate the checked bit clear: zero one bit, a no-op when the bit
    // index is out of bounds.
    // void tfunc_Bitset_ClearBit(); // gstatic/amcdb.tfunc:Bitset.ClearBit

    // Generate the unchecked bit set: set one bit without bounds checking.
    // void tfunc_Bitset_qSetBit(); // gstatic/amcdb.tfunc:Bitset.qSetBit

    // Generate the checked bit set: set one bit, a no-op when the bit index
    // is out of bounds.
    // void tfunc_Bitset_SetBit(); // gstatic/amcdb.tfunc:Bitset.SetBit

    // Generate the unchecked bit write: overwrite one bit with VAL without
    // bounds checking.
    // void tfunc_Bitset_qSetBitVal(); // gstatic/amcdb.tfunc:Bitset.qSetBitVal

    // Generate the unchecked bit or: or VAL into one bit without bounds checking.
    // void tfunc_Bitset_qOrBitVal(); // gstatic/amcdb.tfunc:Bitset.qOrBitVal

    // Generate the whole-set clear: zero every element; the capacity is
    // unchanged.
    // void tfunc_Bitset_ClearBitsAll(); // gstatic/amcdb.tfunc:Bitset.ClearBitsAll

    // Generate the set difference: clear the bits of PARENT that are set in
    // RHS. Skipped for a global ctype, which has no second instance.
    // void tfunc_Bitset_ClearBits(); // gstatic/amcdb.tfunc:Bitset.ClearBits

    // Generate the set union: or RHS's bits into PARENT. Skipped for a global
    // ctype, which has no second instance.
    // void tfunc_Bitset_OrBits(); // gstatic/amcdb.tfunc:Bitset.OrBits

    // Generate the capacity grower for an expandable array: allocate zeroed
    // elements until at least N_BITS bits exist.
    // void tfunc_Bitset_ExpandBits(); // gstatic/amcdb.tfunc:Bitset.ExpandBits

    // Generate the single-bit grower for an expandable array: ensure bits up
    // to and including BIT_IDX exist, initialized to zero.
    // void tfunc_Bitset_AllocBit(); // gstatic/amcdb.tfunc:Bitset.AllocBit

    // Generate the supremum function: 1 plus the index of the highest set
    // bit, 0 when no bit is set.
    // void tfunc_Bitset_Sup(); // gstatic/amcdb.tfunc:Bitset.Sup

    // Generate the bitcurs cursor (struct plus Reset/ValidQ/Access/Next):
    // iterate the indexes of the set bits in ascending order, skipping zero
    // elements with a bit scan.
    // void tfunc_Bitset_bitcurs(); // gstatic/amcdb.tfunc:Bitset.bitcurs

    // -------------------------------------------------------------------
    // cpp/amc/blkhash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Blkhash(); // gstatic/amcdb.tclass:Blkhash
    // void tfunc_Blkhash_Find(); // gstatic/amcdb.tfunc:Blkhash.Find
    // void tfunc_Blkhash_FindX(); // gstatic/amcdb.tfunc:Blkhash.FindX
    // void tfunc_Blkhash_N(); // gstatic/amcdb.tfunc:Blkhash.N
    // void tfunc_Blkhash_EmptyQ(); // gstatic/amcdb.tfunc:Blkhash.EmptyQ
    // void tfunc_Blkhash_InsertMaybe(); // gstatic/amcdb.tfunc:Blkhash.InsertMaybe
    // void tfunc_Blkhash_Remove(); // gstatic/amcdb.tfunc:Blkhash.Remove
    // void tfunc_Blkhash_FindRemove(); // gstatic/amcdb.tfunc:Blkhash.FindRemove
    // void tfunc_Blkhash_Cascdel(); // gstatic/amcdb.tfunc:Blkhash.Cascdel
    // void tfunc_Blkhash_Init(); // gstatic/amcdb.tfunc:Blkhash.Init
    // void tfunc_Blkhash_Uninit(); // gstatic/amcdb.tfunc:Blkhash.Uninit
    // void tfunc_Blkhash_curs(); // gstatic/amcdb.tfunc:Blkhash.curs

    // -------------------------------------------------------------------
    // cpp/amc/blkpool.cpp -- Block pool
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Blkpool(); // gstatic/amcdb.tclass:Blkpool
    // void tfunc_Blkpool_AllocMem(); // gstatic/amcdb.tfunc:Blkpool.AllocMem
    // void tfunc_Blkpool_SetBufferSize(); // gstatic/amcdb.tfunc:Blkpool.SetBufferSize
    // void tfunc_Blkpool_ReserveBuffers(); // gstatic/amcdb.tfunc:Blkpool.ReserveBuffers
    // void tfunc_Blkpool_FreeMem(); // gstatic/amcdb.tfunc:Blkpool.FreeMem
    // void tfunc_Blkpool_UsedBytes(); // gstatic/amcdb.tfunc:Blkpool.UsedBytes
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
    // cpp/amc/cmdline.cpp
    //

    // Return TRUE if FIELD (in command line context) requires no argument
    // This is true for bool fields or fields with "emptyval" provided
    bool CmdArgValueRequiredQ(amc::FField &field);

    // True if field is a required command-line argument
    bool CmdArgRequiredQ(amc::FField &field);

    // Generate help string for CTYPE, treating it as command line description.
    // If BASE is provided, it is the base command line which is included as well.
    // Generate help strings for command-line tools.
    //
    // Example Output:
    // Usage: dmsess [sess] [options]
    // sess          session name
    // -list         list processes in session
    // -cfg:string   (with -create) debug or release. default: debug
    algo::tempstr GenHelpString(amc::FCtype &ctype, amc::FCtype *base, strptr comment);

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

    // C++ already constructs the field; no extra Init needed.
    // void tfunc_Cppfunc_Init(); // gstatic/amcdb.tfunc:Cppfunc.Init
    // void tfunc_Cppfunc_Set(); // gstatic/amcdb.tfunc:Cppfunc.Set

    // -------------------------------------------------------------------
    // cpp/amc/ctype.cpp -- Ctype code generators
    //

    // Validate per-ctype attributes the generators assume: a global ctype admits
    // no ccmp, no chash, and no second instance, and minmax requires a raw
    // operator < (builtin, extrn ccmp, or genop:Y order:Y ccmp).
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

    // True when the ctype gets a fieldwise constructor: a cpptype row asking for a
    // constructor, over at least one field the constructor can take an argument for.
    bool FieldwiseCtorQ(amc::FCtype &ctype);

    // Generate the ctype's fieldwise constructor: one argument per constructor-passable field, each member initialized from its argument.
    //     (user-implemented function, prototype is in amc-generated header)
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

    // Compute the C++ expression for FIELD's default value: the field's own
    // dflt, or, for a reftype that stores the value inline, the value type's
    // cdflt row. A cextern type initialized by memset gets 0 in place of an
    // empty default. An empty result means the field has no default; print
    // emitters compare against the expression to skip default-valued fields.
    tempstr DfltExprVal(amc::FField &field);

    // Compute the initialization expression for a bitfield: the field's dflt,
    // squashed to empty when it is a no-op (bool false, or 0 when the value
    // type's cdflt is already 0). The source field's initializer zeroes every
    // bit, so an empty result tells the bitfield Init generator to emit no
    // initialization step of its own.
    tempstr DfltExprBitfld(amc::FField &field);

    // Retarget the instance reference in DFLT, a field-default C++ expression:
    // a default may name the containing instance as *this (Field_UpdateDflt's
    // message-length default ssizeof(*this)), and an emitter that places the
    // expression where the instance is named differently -- the Init function's
    // parent parameter, a pool row, _db -- substitutes PAREXPR for the
    // reference. Each reference is substituted as a whole identifier, so a
    // longer identifier that merely starts with the same characters
    // (*thisvalue) is not an instance reference and survives verbatim, even
    // when the same expression also carries a real reference.
    // C++ spells the implicit object two ways, and a default may use either:
    // *this is the object, this is a pointer to it. The pointer spelling
    // substitutes the address of PAREXPR, parenthesized, so that the member
    // access in this->len binds to the whole address expression rather than to
    // the member (&parent->len would take the address of the member). Both
    // passes run: a reference left in either spelling would reach a generated
    // non-member function, where neither name is declared.
    void DfltRetarget(cstring &dflt, strptr parexpr);

    // Compute the field's effective default (field.dflt) from the schema:
    // a field with no explicit default inherits its value type's cdflt;
    // a message's type field defaults to the msgtype constant, and its length
    // field to the message-length expression. This keeps generated initializers
    // consistent with the schema: a freshly constructed message identifies its
    // own type and length without caller involvement.
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

    // Generate Dispatch_Read function.
    // A case whose ctype owns a varlen tail appends the tail's bytes to the
    // buffer and then stores the resulting total through the ctype's length
    // field. That total is a runtime one, so it can exceed what the length word
    // represents: reading a 260-byte tuple into a ctype whose length word is a
    // u8 counting the total minus 2 stores 2, and every walk over the buffer
    // afterwards reads the payload as the next message header. The store
    // therefore goes through the same three expressions as every other
    // runtime-total store (LenfldGuardNeededQ, LenfldCheckExpr, LenfldStoreExpr),
    // and a total the word cannot hold fails the read, leaving the caller to
    // discard the buffer on the false return.
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
    // void tfunc_Exec_Exec(); // gstatic/amcdb.tfunc:Exec.Exec
    // void tfunc_Exec_ExecX(); // gstatic/amcdb.tfunc:Exec.ExecX
    // void tfunc_Exec_ToCmdline(); // gstatic/amcdb.tfunc:Exec.ToCmdline
    // void tfunc_Exec_ToArgv(); // gstatic/amcdb.tfunc:Exec.ToArgv
    // void tfunc_Exec_Execv(); // gstatic/amcdb.tfunc:Exec.Execv
    void NewFieldExec();

    // -------------------------------------------------------------------
    // cpp/amc/fast.cpp -- Fixfast generator
    //

    // Check that every namespace with FAST codecs declares the two pmasks those
    // codecs qualify each field's previous value through.
    // The pmask is a field of the namespace's own FastState record, so the missing
    // row is one defect of the namespace no matter how many messages the namespace
    // codes: a protocol with a hundred templates needs the one row, and reporting
    // it per message, or per codec function of each message, would charge the same
    // defect hundreds of times and count each charge as an error.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_fast(); // gstatic/amcdb.gen:check_fast

    // Prepare FAST data
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

    // Check the element type of every Bytebuf and Linebuf: one byte wide, and for a
    // Linebuf comparable with ==.
    // A Linebuf or Bytebuf measures its message in bytes, and its element type is
    // the unit the message is handed back in. A Bytebuf of u32 keeps a message of
    // 12 bytes as msglen 12 and returns an aryptr<u32> of 12 elements, which is
    // 48 bytes -- 36 of them past the data. A Linebuf of u32 scans hdr[msglen]
    // while stepping msglen by sizeof(u32), so the fourth comparison already reads
    // at byte 48 of a 16-byte line. Only a one-byte element makes the two units
    // the same, so the byte-oriented buffer types take one. A Msgbuf carries its
    // length in the message header and is free to have any element type.
    // Runs after gen_compute_size: only a bltin or extern ctype carries a declared
    // csize, and a generated ctype's width comes from the emitted struct body, so
    // asking in gen:ns_tclass_field would reject a one-byte generated element as
    // too wide. An element whose width is not known in this run cannot be shown to
    // be one byte either, and is rejected under its own reason.
    // The second requirement is on comparability, and falls on the Linebuf alone: a
    // Linebuf finds the end of its message by comparing each element with the
    // delimiter, so its ScanMsg reads `hdr[msglen] == <dflt>`. A one-byte generated
    // struct passes the width test and has no operator == unless its ccmp asks for
    // one, and the run then exits 0 with a generated namespace the C++ compiler
    // rejects. The comparison is emitted only for the reading direction, and only
    // when the scanner is amc's own: an ffunc ScanMsg with extrn:Y hands the scan to
    // the user, who compares the elements however the type allows.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_fbuf(); // gstatic/amcdb.gen:check_fbuf

    // Generate the buffer's members and the functions its direction and buffer
    // type call for; txt/exe/amc/fbuf.md documents the reftype.
    // void tclass_Fbuf(); // gstatic/amcdb.tclass:Fbuf

    // Generate $name_BeginRead: attach a file descriptor to the read buffer and arm its edge-triggered epoll readiness hook.
    // void tfunc_Fbuf_BeginRead(); // gstatic/amcdb.tfunc:Fbuf.BeginRead
    // void tfunc_Fbuf_GetMsg(); // gstatic/amcdb.tfunc:Fbuf.GetMsg
    // void tfunc_Fbuf_Init(); // gstatic/amcdb.tfunc:Fbuf.Init
    // void tfunc_Fbuf_Max(); // gstatic/amcdb.tfunc:Fbuf.Max
    // void tfunc_Fbuf_N(); // gstatic/amcdb.tfunc:Fbuf.N

    // Generate $name_Refill: read available bytes from the file descriptor into the read buffer.
    // void tfunc_Fbuf_Refill(); // gstatic/amcdb.tfunc:Fbuf.Refill
    // void tfunc_Fbuf_RemoveAll(); // gstatic/amcdb.tfunc:Fbuf.RemoveAll
    // void tfunc_Fbuf_ScanMsg(); // gstatic/amcdb.tfunc:Fbuf.ScanMsg
    // void tfunc_Fbuf_Shift(); // gstatic/amcdb.tfunc:Fbuf.Shift

    // Generate $name_SkipBytes: mark a number of buffer bytes as consumed, advancing the read cursor.
    // void tfunc_Fbuf_SkipBytes(); // gstatic/amcdb.tfunc:Fbuf.SkipBytes

    // Generate $name_SkipMsg: advance the read cursor past the current message.
    // void tfunc_Fbuf_SkipMsg(); // gstatic/amcdb.tfunc:Fbuf.SkipMsg
    // void tfunc_Fbuf_WriteReserve(); // gstatic/amcdb.tfunc:Fbuf.WriteReserve

    // Generate $name_WriteAll: append a block of bytes to the write buffer, shifting or flushing to the fd to make room.
    // void tfunc_Fbuf_WriteAll(); // gstatic/amcdb.tfunc:Fbuf.WriteAll
    // void tfunc_Fbuf_WriteMsg(); // gstatic/amcdb.tfunc:Fbuf.WriteMsg

    // Generate $name_BeginWrite: attach a file descriptor to the write buffer and arm its edge-triggered outflow hook.
    // void tfunc_Fbuf_BeginWrite(); // gstatic/amcdb.tfunc:Fbuf.BeginWrite

    // Generate $name_PairReady: turn one descriptor's readiness into the ready condition of each buffer of the pair.
    // void tfunc_Fbuf_PairReady(); // gstatic/amcdb.tfunc:Fbuf.PairReady

    // Generate $name_BeginReadWrite: attach one descriptor to a read/write buffer pair under a single epoll registration.
    // void tfunc_Fbuf_BeginReadWrite(); // gstatic/amcdb.tfunc:Fbuf.BeginReadWrite

    // Generate $name_Outflow: write buffered bytes to the file descriptor, retiring the readiness hook once drained.
    // void tfunc_Fbuf_Outflow(); // gstatic/amcdb.tfunc:Fbuf.Outflow
    // void tfunc_Fbuf_EndRead(); // gstatic/amcdb.tfunc:Fbuf.EndRead
    // void tfunc_Fbuf_EndWrite(); // gstatic/amcdb.tfunc:Fbuf.EndWrite
    // void tfunc_Fbuf_Realloc(); // gstatic/amcdb.tfunc:Fbuf.Realloc

    // Generate the fbuf's contribution to the parent's Uninit: return the
    // buffer's memory to the pool it was taken from and leave the field in the
    // state a fresh Init produces, so a record that is reinitialized in place
    // neither frees the same block twice nor reuses a freed one.
    // void tfunc_Fbuf_Uninit(); // gstatic/amcdb.tfunc:Fbuf.Uninit
    // void tfunc_Fbuf_GetAlloc(); // gstatic/amcdb.tfunc:Fbuf.GetAlloc
    // void tfunc_Fbuf_GetAllocReserve(); // gstatic/amcdb.tfunc:Fbuf.GetAllocReserve

    // Generate $name_BeginAlloc: reserve a contiguous block of bytes in the byte buffer and return a write pointer, or NULL when it does not fit.
    // void tfunc_Fbuf_BeginAlloc(); // gstatic/amcdb.tfunc:Fbuf.BeginAlloc
    // void tfunc_Fbuf_BeginAllocReserve(); // gstatic/amcdb.tfunc:Fbuf.BeginAllocReserve

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
    // cpp/amc/fcond.cpp
    //

    // The Upptr field FCOND redirects through (NULL when via is empty):
    // the pointed-to record, not the record carrying the watched field,
    // is what enters and leaves the list.
    amc::FField *FcondViaField(amc::FFcond &fcond);

    // The tclass condition FCOND watches on FIELD, NULL when FIELD is a Val
    // whose conditions are its own values.  Every other reftype declares the
    // conditions it can report as amcdb.tcond rows of its tclass, and the value
    // component of the fcond key names one of them.
    amc::FTcond *FcondTcond(amc::FField &field, amc::FFcond &fcond);

    // The fcond row declaring condition TCOND on FIELD, NULL when FIELD declares
    // no membership for it.  A tclass generator calls this at each point it can
    // report TCOND, naming the condition by its generated symbol
    // (amcdb_tcond_Fbuf_ready and friends), and emits nothing where the row is
    // absent.  A generator that asks for a condition its own tclass does not
    // declare is a generator bug, and fails here rather than silently generating
    // nothing.
    amc::FFcond *FindFcond(amc::FField &field, algo::strptr tcond);

    // The namespace-qualified name of the function that OPs (Insert or Remove)
    // a record on FCOND's index.
    tempstr FcondOpFunc(amc::FFcond &fcond, algo::strptr op);

    // The insert function of FCOND's index, for a generator that needs the
    // function itself rather than a call to it -- an fbuf hands it to
    // callback_Set1 as the file descriptor's readiness hook.  The function takes
    // the record carrying the watched field, so a via redirection has no way
    // through; FcondNocallbackQ rejects that combination up front.
    tempstr FcondInsFunc(amc::FFcond &fcond);

    // True when FCOND cannot be installed as a plain callback on the record
    // carrying the watched field, because it redirects the membership elsewhere.
    bool FcondNocallbackQ(amc::FFcond &fcond);

    // Emit into BODY the list operation FCOND declares -- OP is Insert or
    // Remove -- performed on the record that carries the watched field, or on
    // the record its via pointer names.  The generated Insert and Remove are
    // self-guarding, so repeating the operation is a no-op, and a NULL via
    // pointer skips it.
    void AddFcondOp(algo::cstring &body, amc::FFcond &fcond, algo::strptr op);

    // Resolve FCOND's value component to the C++ expression the generated
    // setter compares against: the fconst symbol when FIELD's values are
    // named by fconsts, the literal itself for bool and integer fields.
    // An unresolvable value fails with an error naming the record.
    tempstr FcondCppValue(amc::FField &field, amc::FFcond &fcond);

    // Check fcond records: the condition belongs to the watched field's domain
    // -- a value of a Val field, or a condition its reftype's tclass declares --
    // the ins index is intrusive and lives on the namespace global of the same
    // namespace, its xref does not compete for insertion, and the type of
    // the inserted record (the watched ctype, or the via target) matches
    // the index's element type.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_fcond(); // gstatic/amcdb.gen:check_fcond

    // Emit the fcond membership dispatch as the body of SET, the generated
    // setter of FIELD: on a value change, rows matching the old value remove
    // (rem:Y), the assignment happens, and rows matching the new value
    // insert.  A same-value Set performs no list operation -- conditions
    // are level-triggered, and the level did not change.
    void AddFcondSetBody(amc::FFunc &set, amc::FField &field);

    // Emit the initial-membership dispatch into XREFMAYBE, the generated
    // x-reference function of CTYPE: each fcond row on a Val field inserts the
    // record (or its via target) when the field holds the row's value at xref
    // time.  The initial value thereby decides the initial membership -- a
    // record that must start registered leaves the field at its default (or
    // sets it before XrefMaybe); no manual arm is written.  Emitted after
    // the xref inserts, so via pointers assigned by the x-reference step
    // are already in place.  A condition of another reftype has no initial
    // value to read: its tclass reports it when it flips, and a buffer that
    // starts empty simply never flipped.
    void AddFcondXrefMaybeBody(amc::FFunc &xrefmaybe, amc::FCtype &ctype);

    // -------------------------------------------------------------------
    // cpp/amc/fconst.cpp -- Constants, string <-> int conversion
    //

    // Bit mask FIELD's store applies to a value: the declared width for a Bitfld
    // field, whose setter masks the store to those bits, and all ones for any other
    // field, whose store is the arg type itself.
    // The one place the mask is computed. Everything that has to know which bits a
    // field's store keeps asks here -- the bitfield accessors that emit the mask
    // itself, the decimal reader's range, an fconst value's range check, and a
    // length field's largest frame -- because the width the mask is built from is a
    // signed schema argument that a run can reach before anything has refused it,
    // and each of those callers spelling the guard for itself is how they came to
    // spell it differently.
    // The shift computes the mask for the widths from one to sixty-three, the widths
    // that name fewer bits than the word: the mask is a u64 shifted by the width, and
    // a u64 has no shift by sixty-four or more. A width of exactly sixty-four is a
    // legal width whose bits are every bit of the word, so all ones is that width's
    // exact mask rather than a fallback -- the fixture's w64 field is one, and its
    // fconst naming every bit of a u64 draws no range line.
    // A width outside one to sixty-four is out of bounds, and the bitfield check
    // reports it as such naming the field before any accessor is emitted. All ones is
    // what this rule then falls back on, which leaves the store's bounds the arg
    // type's own rather than computed from bits the schema does not describe: the
    // value's own range check says nothing about a value the field's width, not the
    // value, made impossible.
    // test/amc/fconst_bitfld_width.ssim pins the widths this rule accepts and
    // refuses, and amc.FconstBitfldWidth is the golden.
    u64 FieldStoreMask(amc::FField &field);

    // Max value FIELD's store can hold: the arg type's max (GetMinMax), clamped to
    // the bits the store keeps (FieldStoreMask). Returns false when the arg
    // resolves to no integer type: the field then has no numeric range and MAX
    // holds the widest-signed fallback.
    bool FieldMaxStore(amc::FField &field, u64 &max);

    // Min value FIELD's store can hold: the arg type's min (GetMinMax), raised to
    // zero when the store keeps fewer bits than a signed arg type spans.
    // A Bitfld's Set masks the store to the declared width, and its Get casts those
    // bits back to the arg type without sign-extending them. So a 4-bit field with
    // arg i8 stores -1 as 0xf and reads it back as 15. The store keeps the sign only
    // when its bits, minus the one the sign itself takes, still reach the arg type's
    // maximum -- an 8-bit field with arg i8, where the mask round-trips every
    // negative. Returns false when the arg resolves to no integer type: the field
    // then has no numeric range and MIN holds the widest-signed fallback.
    bool FieldMinStore(amc::FField &field, u64 &min);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fconst(); // gstatic/amcdb.tclass:Fconst

    // Generate $name_GetEnum: read the field's stored value as its enum type.
    // void tfunc_Fconst_GetEnum(); // gstatic/amcdb.tfunc:Fconst.GetEnum
    // void tfunc_Fconst_SetEnum(); // gstatic/amcdb.tfunc:Fconst.SetEnum

    // Generate $name_ToCstr: map the field's numeric value to its string constant, or NULL when none matches.
    // void tfunc_Fconst_ToCstr(); // gstatic/amcdb.tfunc:Fconst.ToCstr

    // Generate $name_Print: emit the field as its string constant, falling back to the numeric value.
    // void tfunc_Fconst_Print(); // gstatic/amcdb.tfunc:Fconst.Print

    // Generate $name_SetStrptrMaybe: set the field from a string constant, returning false when the string matches none.
    // void tfunc_Fconst_SetStrptrMaybe(); // gstatic/amcdb.tfunc:Fconst.SetStrptrMaybe

    // Generate $name_SetStrptr: set the field from a string constant, using the supplied default when none matches.
    // void tfunc_Fconst_SetStrptr(); // gstatic/amcdb.tfunc:Fconst.SetStrptr

    // Generate $name_ReadStrptrMaybe: read the field from a string, trying the string constant first, then the underlying type.
    // void tfunc_Fconst_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Fconst.ReadStrptrMaybe

    // Validate each fconst's value against the store of the field it names.
    // A ten-bit bitfield with arg u32 that carries an fconst of 2000 sets 976
    // through the generated Set, so a tuple naming that symbol is accepted and the
    // field comes back holding a number the schema never named, which ToCstr and
    // GetEnum can no longer map to the symbol. The store's bounds (FieldMinStore,
    // FieldMaxStore) are the bounds the same field's reader already imposes on a
    // numeric input, so the symbol carrying the value is held to them too. A value
    // outside them fails the run naming the fconst and the range.
    // The store bounds a magnitude, one side at a time: the positive side reaches
    // the store's maximum, and the negative side reaches the negation of its
    // minimum, which is zero for an unsigned store and for bits that drop a signed
    // arg type's sign. Carrying sign and magnitude apart is what lets an unsigned
    // store hold every value up to 18446744073709551615, which no signed reading of
    // the same 64 bits can express.
    // A value that is not one complete integer literal -- a shift expression, a
    // character literal, an LE_STR, a name that only the C++ compiler resolves,
    // digits with trailing text -- carries no number to compare and is not bounded
    // here. Neither is a field whose arg resolves to no integer type, which has no
    // numeric range at all; the value of a string field is a string rather than a
    // number, and it falls under that same case, since a cstr ctype wraps a
    // character array and GetMinMax resolves only a chain of single Val fields down
    // to an integer bltin. A magnitude that outruns 64 bits fits no store and is
    // reported, since every store's own maximum is a 64-bit number.
    // The fconsts of a field that sources bitfields are the masks of those bitfields
    // rather than values of the field, so a word using its top bit carries a mask
    // past its own signed maximum by design; each such mask is bounded by the bits
    // of the word it sources, which gen_check_bitfld checks on the bitfield itself.
    // void gen_check_fconst(); // gstatic/amcdb.gen:check_fconst

    // -------------------------------------------------------------------
    // cpp/amc/fdec.cpp -- Decimal types
    //

    // Compute the numeric range the fdec field's storage holds, once per field;
    // the Dec tfuncs generate their range checks from the stored bounds. An arg
    // with no integer range cannot back those checks, so the run fails naming
    // the field and its arg.
    // The decimal place count is checked here as well, before any tfunc builds
    // arithmetic from it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Dec(); // gstatic/amcdb.tclass:Dec

    // Generate $name_GetDouble: return the fixed-point field as a double, dividing by the decimal scale.
    // void tfunc_Dec_GetDouble(); // gstatic/amcdb.tfunc:Dec.GetDouble

    // Generate $name_GetInt: return the integer portion of the fixed-point field, dividing by the decimal scale.
    // void tfunc_Dec_GetInt(); // gstatic/amcdb.tfunc:Dec.GetInt

    // Generate the ctype's GetScale accessor returning the fixed-point scale constant, only when the ctype carries a single fdec field.
    // void tfunc_Dec_GetScale(); // gstatic/amcdb.tfunc:Dec.GetScale

    // Generate $name_qSetDouble: set the fixed-point field from a double, rounding to the nearest representable value.
    // void tfunc_Dec_qSetDouble(); // gstatic/amcdb.tfunc:Dec.qSetDouble

    // Generate $name_SetDoubleMaybe: store VAL scaled and rounded to the
    // nearest scaled integer, returning false when that integer is outside the
    // field's range.
    // Both bounds are checked before the rounding, against the field's range
    // widened by the half unit the rounding itself moves: maxval must be accepted
    // (214748364.7 scales to exactly 2147483647, the top of an i32 field with one
    // implied place) while a value that rounds to maxval+1 must not be, and the
    // bottom of the range needs the same allowance or a value that rounds up to
    // minval is refused while its mirror at the top stores. Half a unit is also
    // what keeps the upper bound safe for a field as wide as the double's integer
    // reach: converting the type's maximum to a double rounds it up to the next
    // power of two, and half a unit disappears into that rounding, leaving the
    // bound strict -- so no double at or above the type's magnitude is ever
    // stored, which would be undefined behavior. At the bottom of such a field
    // the type's minimum is itself a power of two, exactly representable, and the
    // half unit disappears the other way: the strict bound then refuses the
    // minimum, a double whose neighbors are a thousand units away.
    // void tfunc_Dec_SetDoubleMaybe(); // gstatic/amcdb.tfunc:Dec.SetDoubleMaybe

    // Generate $name_ReadStrptrMaybe: parse a decimal string into the scaled
    // integer the field stores, returning false on a bad character or a value
    // the field cannot hold.
    // void tfunc_Dec_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Dec.ReadStrptrMaybe

    // Generate $name_Print: format the fixed-point field as a decimal string.
    // void tfunc_Dec_Print(); // gstatic/amcdb.tfunc:Dec.Print

    // -------------------------------------------------------------------
    // cpp/amc/field.cpp -- Generic field generator
    //

    // A field's ffunc names are validated purely by the MarkUsed mechanism: every
    // emission site calls FindFfunc(...,true), so a name that no generator consumes
    // (a typo, or a name not applicable to the field's reftype/tclass) is left
    // unmarked and reported by gen_check_ffunc.  No upfront tclass-membership check
    // is needed -- the set of tfuncs that actually run for the field is the
    // authority (this replaced the old amcdb.tcb table).
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Field(); // gstatic/amcdb.tclass:Field
    // void tclass_Field2(); // gstatic/amcdb.tclass:Field2
    // void tfunc_Field_Cleanup(); // gstatic/amcdb.tfunc:Field.Cleanup
    // void tfunc_Field_Userinit(); // gstatic/amcdb.tfunc:Field.Userinit

    // Find an ffunc of the given name declared on FIELD (via dmmeta.ffunc).
    // Returns NULL if none.  Pkey is '<field>.<name>'; build it via the
    // amc-generated concat helper and look up in the unique ind_ffunc hash.
    // If mark_used is true and the ffunc is found, set ffunc.used so gen_check_ffunc
    // does not warn about it.  Pass true at emission sites, false at
    // classification-only queries (e.g. PlaindataVisit).
    amc::FFfunc *FindFfunc(amc::FField &field, algo::strptr name, bool mark_used = false);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Field_OnXref(); // gstatic/amcdb.tfunc:Field.OnXref
    // void tfunc_Field_OnUnref(); // gstatic/amcdb.tfunc:Field.OnUnref
    // void tfunc_Field_Cascdel(); // gstatic/amcdb.tfunc:Field.Cascdel

    // provide a read function for the field if it already has a Set function,
    // or if the underlying type supports read.
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

    // Append to FUNC's body a call to FIELD's Set<Present> accessor for each
    // pmask of which the field is a member; REF names the parent record
    // (collapsed by ParentArgExpr when the parent is a global)
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

    // Append to FUNC's body a (void)NAME statement unless NAME is already used
    // in the body (as a whole identifier) or consumed by a constructor
    // initializer; keeps generated functions clean under -Wunused-parameter
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

    // Check that each cascdel field names an unambiguous delete target:
    // more than one instance access path to the field's target leaves
    // unclear which instance a cascade delete should remove.
    // void gen_check_cascdel(); // gstatic/amcdb.gen:check_cascdel
    // void gen_check_ssimsort(); // gstatic/amcdb.gen:check_ssimsort
    // void gen_countxref(); // gstatic/amcdb.gen:countxref

    // Check the dependency order of gstatic tables: a table may refer only to
    // tables loaded before it (earlier rowid), and never to a finput table --
    // finput data is not loaded yet when the static initializers run.
    // void gen_check_static(); // gstatic/amcdb.gen:check_static
    // void gen_check_prefix(); // gstatic/amcdb.gen:check_prefix
    // void gen_basepool(); // gstatic/amcdb.gen:basepool

    // TODO: also check that targdep path exists between namespaces?
    // void gen_check_basepool(); // gstatic/amcdb.gen:check_basepool

    // Validate each bitfield against its source field: the source must be a sized
    // integer, the bitfield must fit within its bits, and bitfields sharing a
    // source field must not overlap.
    // The width is bounded above by 64 as well as below by 1, because 64 bits is
    // all a mask over the field can select: the mask is built in a u64. A 65-bit
    // field of a u128 source field has an offset and a width the source field can
    // hold, so the two tests below pass it.
    // The bitfield accessors refuse such a width too, but they refuse it later and
    // one field at a time: the run ends at the first bad width with a message
    // carrying neither the offset nor the width. Reporting it here carries both and
    // counts it as a defect rather than ending the run, so a universe with several
    // bad widths reports every one -- amc.FconstBitfldWidth holds a negative width,
    // a zero one and a 65 and draws a line for each.
    // void gen_check_bitfld(); // gstatic/amcdb.gen:check_bitfld

    // Rewrite reftype:Pkey fields into reftype:Val
    // void gen_lookuppkey(); // gstatic/amcdb.gen:lookuppkey

    // Ensure fregx record exists for each field of type Regx or RegxSql
    // Rewrite RegxSql fields as Regx, regxtype Sql
    // (generalization of original RegxSql reftype)
    // void gen_rewrite_regx(); // gstatic/amcdb.gen:rewrite_regx

    // Validate every ffunc by the code actually generated: each emission site calls
    // FindFfunc with mark_used=true, so an ffunc left unused names a function no
    // generator produced -- a typo, or a name not applicable to the field (e.g.
    // FindRemove on a Val, OnXref on a field whose access path never inserts,
    // InputMaybe on a non-finput field).  This is the sole ffunc-name check; the
    // set of tfuncs that run for the field is the authority (replaced amcdb.tcb).
    // void gen_check_ffunc(); // gstatic/amcdb.gen:check_ffunc

    // Check that each field's reftype is backed by its per-reftype record
    // (dmmeta.tary, dmmeta.thash, ...); later gen phases dereference these
    // records, so reftype errors end the run here.
    // void gen_check_reftype(); // gstatic/amcdb.gen:check_reftype
    // void gen_detectinst(); // gstatic/amcdb.gen:detectinst
    // void gen_prep_field(); // gstatic/amcdb.gen:prep_field

    // Check that each big-endian field can be stored byteswapped: the field
    // is a Val of a builtin type flagged bigendok, and the type's width is one
    // the byteswap primitives cover (be16toh/be32toh/be64toh). The width is
    // verified independently of the bigendok claim: a bltin row could flag a
    // width with no primitive (a 128-bit integer), and Val Get/Set would then
    // emit a call to a nonexistent swap function -- uncompilable output with a
    // zero exit. A width of zero means the width is not known in this universe
    // (the type's csize row is absent), and the diagnostic names the missing
    // row instead of prescribing a width change for a type whose width may
    // already be right. Each rejection reports and continues, accumulating
    // into the exit code.
    // void gen_check_bigend(); // gstatic/amcdb.gen:check_bigend

    // walk over all xrefs and populate ctype.c_parent array,
    // which lists all the "parent" ctypes (ones used by this ctype)
    // check
    // void gen_xref_parent(); // gstatic/amcdb.gen:xref_parent
    // void gen_datafld(); // gstatic/amcdb.gen:datafld
    // void gen_ctype_toposort(); // gstatic/amcdb.gen:ctype_toposort

    // Determine CTYPE's plaindata and has_dtor flags, recursing into the type of
    // every member first, and store both on the ctype. See gen_plaindata for what
    // the two flags mean and why one walk settles both.
    // A ctype already on the visit list keeps the values it was given, so the walk
    // costs one pass over the graph and a ctype containing itself terminates.
    void PlaindataVisit(amc::FCtype &ctype);

    // Determine, for each ctype, how its value behaves: whether it can be copied
    // with memcpy (plaindata) and whether destroying it does anything (has_dtor).
    //
    // The two are separate facts, and one field reftype tells them apart. A Varlen
    // or an Opt member is addressed in place inside its parent, so the parent
    // allocates nothing for it and needs no destructor -- but the member's extent is
    // decided at runtime, so the parent has no fixed size and cannot be memcopied.
    // Conflating the two would report a destructor for every nested variable-length
    // message, and amc refuses to nest one whose element type has a destructor.
    //
    // Both walk the same ctype graph, so one pass computes both.
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

    // Side-load the ssimfile rows behind each gstatic field into the
    // static_tuple table; they become the compiled-in initializers of the
    // gstatic table.
    // void gen_load_gstatic(); // gstatic/amcdb.gen:load_gstatic

    // Generate numeric fconsts from string fconsts
    // void gen_clonefconst(); // gstatic/amcdb.gen:clonefconst
    // void gen_newfield_exec(); // gstatic/amcdb.gen:newfield_exec
    // void gen_newfield_count(); // gstatic/amcdb.gen:newfield_count

    // convert cbase records into fields of reftype Base
    // void gen_newfield_cbase(); // gstatic/amcdb.gen:newfield_cbase

    // convert cbase records into fields of reftype Base
    // void gen_check_basefield(); // gstatic/amcdb.gen:check_basefield
    // void gen_newfield_sortfld(); // gstatic/amcdb.gen:newfield_sortfld

    // Check that every Ptrary field has its dmmeta.ptrary record, and extend unique ptrarys
    // with a membership flag on the target ctype.
    // void gen_newfield_ptrary(); // gstatic/amcdb.gen:newfield_ptrary
    // void gen_newfield_dispatch(); // gstatic/amcdb.gen:newfield_dispatch
    // void gen_newfield_cfmt(); // gstatic/amcdb.gen:newfield_cfmt

    // Check pack consistency within a namespace: every ctype in a packed namespace must be packed, and every field of a packed ctype must be packed.
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

    // Generate code to load gstatic tables
    // (added to FDb Init function)
    // Gstatics are loaded in the order of their ctypes
    // void gen_ns_gstatic(); // gstatic/amcdb.gen:ns_gstatic
    // void gen_ns_check_lim(); // gstatic/amcdb.gen:ns_check_lim
    // void gen_proc(); // gstatic/amcdb.gen:proc

    // Check that each fcurs row names a cursor supported by its field's
    // reftype (the corresponding amcdb.tcurs row is in the input set).
    // void gen_check_fcurs(); // gstatic/amcdb.gen:check_fcurs

    // Check that the element type of a Varlen or Opt field has no destructor.
    // Both reftypes address their element in place inside the enclosing message --
    // there is no separate object whose lifetime amc could end -- so an element
    // type that needs a destructor would never have one called.
    // Check also that no ctype's fields claim the end of the fixed portion twice:
    // varlen data and an optional trailing element both begin there, and only
    // varlen fields carry the end offset that lets one follow another.
    // void gen_check_varlen(); // gstatic/amcdb.gen:check_varlen

    // Generate a global list c_ssimfile_sorted
    // which indexes ssimfiles in topological order
    // void gen_sortssimfile(); // gstatic/amcdb.gen:sortssimfile
    // void gen_create_userfunc(); // gstatic/amcdb.gen:create_userfunc

    // Write the derived tables (ctypelen, dispsig, tracefld, tracerec,
    // userfunc) back to the output dataset through an acr subprocess, so
    // they match the code generated by this run.
    // void gen_table_write(); // gstatic/amcdb.gen:table_write
    // void gen_ssimdb(); // gstatic/amcdb.gen:ssimdb

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

    // Generate <ns>::RemoveStrptrMaybe(strptr str): mirror of InsertStrptrMaybe
    // for the delete branch of Syscmd_SsimMsg.  Switches on the type-tag, parses
    // the tuple, finds the record by pkey via the basetype's Thash, calls
    // <finput-name>_Delete.  Finputs without a pkey-Thash are skipped (the
    // generated case logs a verblog and returns true so unrelated tables don't
    // cause errors).
    // void tfunc_Global_RemoveStrptrMaybe(); // gstatic/amcdb.tfunc:Global.RemoveStrptrMaybe
    // void tfunc_Global_InitReflection(); // gstatic/amcdb.tfunc:Global.InitReflection
    // void tfunc_Global_LoadSsimfileMaybe(); // gstatic/amcdb.tfunc:Global.LoadSsimfileMaybe
    // void tfunc_Global_main(); // gstatic/amcdb.tfunc:Global.main
    // void tfunc_Global_WinMain(); // gstatic/amcdb.tfunc:Global.WinMain
    // void tfunc_Global_MainLoop(); // gstatic/amcdb.tfunc:Global.MainLoop
    // void tfunc_Global_Steps(); // gstatic/amcdb.tfunc:Global.Steps
    // void tfunc_Global_Step(); // gstatic/amcdb.tfunc:Global.Step
    // void tfunc_Global_Main(); // gstatic/amcdb.tfunc:Global.Main

    // Return expression
    // $cpptype &NAME = $ns::$_db.$fieldname
    // where
    // $cpptype is the type of FIELD
    // $ns is the namespace of FIELD
    // $_db is the global instance in $ns
    // $fieldname is the name of the field
    tempstr VarRefToGlobal(amc::FField &field, strptr name);

    // Per-ctype field-aware command-line reader.  Parses ARGS -- a word array
    // already split (from real argv, or from a tokenized command string) -- into
    // the fields of PARENT, accumulating diagnostics in ERR.  This is the single
    // intelligent parser: it consults $Name_NArgs so a bare "-opt" consumes the
    // following word as its value, and it splits "-opt:value".  When the ctype
    // carries a basecmdline (a tool's algo_lib.Cmdline), those base options are
    // read into that global in the same pass.
    void GenReadArgvFunc(amc::FCtype &ctype);

    // Emit $Name_ReadArgv only for ctypes that declare a readable strfmt:Argv
    // cfmt (command lines), the same gate tfunc_Ctype_NArgs uses.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_ReadArgv(); // gstatic/amcdb.tfunc:Ctype.ReadArgv

    // Namespace ReadArgv function to read command line
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

    // Declare the storage for an inlary field: a fixed inlary (min==max) is a
    // bare C array; a variable one is raw element storage plus a live count,
    // with the Pool tfuncs layered on top. Also validates the shape: separator
    // print compatibility, the char/u8 min floor, min against max, and
    // remove-function suppression.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Inlary(); // gstatic/amcdb.tclass:Inlary

    // Generate $name_AllocMem for a variable inlary: hand out the next raw slot, or NULL when the array is full.
    // void tfunc_Inlary_AllocMem(); // gstatic/amcdb.tfunc:Inlary.AllocMem

    // Generate $name_EmptyQ: true when a variable inlary holds no elements.
    // void tfunc_Inlary_EmptyQ(); // gstatic/amcdb.tfunc:Inlary.EmptyQ

    // Generate $name_Fill for a fixed inlary: assign the given value to every slot.
    // void tfunc_Inlary_Fill(); // gstatic/amcdb.tfunc:Inlary.Fill
    // void tfunc_Inlary_Find(); // gstatic/amcdb.tfunc:Inlary.Find

    // Generate $name_Getary: view the inlary's live elements as an aryptr.
    // void tfunc_Inlary_Getary(); // gstatic/amcdb.tfunc:Inlary.Getary

    // Generate the field's Init fragment: a variable inlary starts at zero
    // elements and preallocates its min floor; a fixed inlary with a field
    // default fills every slot with it
    // void tfunc_Inlary_Init(); // gstatic/amcdb.tfunc:Inlary.Init
    // void tfunc_Inlary_Eq(); // gstatic/amcdb.tfunc:Inlary.Eq
    // void tfunc_Inlary_Cmp(); // gstatic/amcdb.tfunc:Inlary.Cmp

    // Generate $name_Max: return the inlary's capacity, its maximum element count.
    // void tfunc_Inlary_Max(); // gstatic/amcdb.tfunc:Inlary.Max
    // void tfunc_Inlary_N(); // gstatic/amcdb.tfunc:Inlary.N

    // Generate $name_RemoveAll for a variable inlary: destroy every element and reset the count to zero.
    // void tfunc_Inlary_RemoveAll(); // gstatic/amcdb.tfunc:Inlary.RemoveAll

    // Generate $name_RemoveLast for a variable inlary: destroy the last element and shrink the count by one.
    // void tfunc_Inlary_RemoveLast(); // gstatic/amcdb.tfunc:Inlary.RemoveLast
    // void tfunc_Inlary_RowidFind(); // gstatic/amcdb.tfunc:Inlary.RowidFind

    // Generate $name_Setary, replacing the array's contents with a copy of the
    // source: plain data is copied with memcpy; otherwise a variable inlary
    // destroys the old elements and rebuilds, and a fixed one assigns in
    // place. A variable inlary with min>0 is topped back up to its floor
    // void tfunc_Inlary_Setary(); // gstatic/amcdb.tfunc:Inlary.Setary

    // Generate the variable inlary's Uninit fragment: destroy its elements when the parent is torn down, skipped in global scope.
    // void tfunc_Inlary_Uninit(); // gstatic/amcdb.tfunc:Inlary.Uninit
    // void tfunc_Inlary_qFind(); // gstatic/amcdb.tfunc:Inlary.qFind

    // Generate $name_rowid_Get: recover an element's rowid from its address within the inlary.
    // void tfunc_Inlary_rowid_Get(); // gstatic/amcdb.tfunc:Inlary.rowid_Get

    // Generate the inlary's cursor: a cursor type and the functions that walk the array's elements in order.
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
    // a variable inlary with min>0 keeps at least min elements through
    // Init, Setary, and this read: after the flush and append, the array is
    // topped back up to min with value-initialized elements, so the result
    // is a function of the input alone. The floor binds only these content
    // producers; explicit removal (RemoveAll, RemoveLast) still empties
    // the array
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

    // Generate <pool>_InputMaybe for a finput field: parse one row and add it to
    // the pool.  The user can take over via ffunc -- InputMaybe (whole function)
    // or Input (amc wraps it) -- see tclass-tfunc.md#ffunc.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Io_InputMaybe(); // gstatic/amcdb.tfunc:Io.InputMaybe

    // Emit the extern prototype for a user-supplied <pool>_Input when the field
    // declares ffunc:<field>.Input extrn:Y; InputMaybe calls it.
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

    // return TRUE if javscript output is requested for this ctype
    bool JsQ(amc::FCtype &ctype);

    // return TRUE if typescript output is requested for this ctype
    bool TsQ(amc::FCtype &ctype);

    // Map FIELD to a JavaScript built-in type.
    // The built-in type is Uint8Array, Array, Map, string, number, boolean, bigint, undefined, and object
    // For unknown types, "object" is returned.
    // From field and jsbltin, a JavaScript classname can be constructed with GetJsClass.
    amc::FJsbltin *GetJsBltin(amc::FField &field, bool json = false);
    algo::tempstr GetJsClass(amc::FField &field, amc::FJsbltin *jsbltin);
    algo::tempstr GetJsClass(amc::FField &field, bool json = false);

    // String constructing specified javascript type from provided value
    tempstr JsDflt(amc::FField &field);

    // String constructing specified javascript type from provided value
    tempstr JsCons(amc::FField &field, algo::strptr val);

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

    // Refuse a jstype'd packed ctype carrying a field with no TypeScript wire
    // form, naming every such field rather than stopping at the first.
    //
    // Consider a message whose second field is an inline array of char followed
    // by a u32 sequence number.  The array has no byte-copy wire form, so the
    // generated decoder reads nothing for it -- and, reading nothing, leaves the
    // running offset where the array began, so the sequence number is read from
    // the array's first four bytes and every later field slides with it.  The
    // frame decodes, no error is raised, and the values are wrong.
    //
    // What makes that possible is a field admitted into the layout with no wire
    // form to give it.  So the schema is refused here, before any code is
    // generated: TsFixedSlotBytes is the one place a field's wire form is
    // decided, and a field it cannot place is a schema error.  Reporting rather
    // than throwing is what lets one run name every offending field; amc writes
    // no file at all once a generation error is recorded, so a refused schema
    // yields no encoder and no decoder rather than a silently wrong one.
    //
    // The ctype as a whole is checked for one thing the same way.  A Varlen or Opt
    // field's bytes are the frame's tail, and the decoder finds where that tail
    // ends by reading the frame's length word out of the ctype's length field.  A
    // ctype that declares a tail and no length field leaves the decoder with no
    // length to read, so it is refused as amc.jstype_lenfld.
    // void gen_check_jstype(); // gstatic/amcdb.gen:check_jstype

    // Generate $ctype_Encode: write PARENT's wire form into VIEW at OFFSET --
    // the varlen and Opt tails first (establishing the frame length), then the
    // fixed-size fields; returns the frame byte count.
    void TsGenEncode();

    // Emit into OUT the TS statement writing (ENCODE) or reading (!ENCODE)
    // FIELD's fixed-size wire form at $offset: DataView get/set for scalars,
    // a NUL-padded byte window for a fixed-length string, and a nested
    // $jsclass_Encode/_Decode call for a ctype-valued field.
    void TsSerdeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field, bool encode);

    // Generate $ctype_Decode: read a $ctype from VIEW at OFFSET -- fixed-size
    // fields at their static offsets, then the varlen tails framed by the
    // message length field.
    void TsGenDecode();

    // Generate $ctype_Print: render a decoded value as text, the TS counterpart of
    // the C++ Tuple print -- space-separated field:value pairs; a ctype with a
    // single printed field prints the bare value.
    // The body is a template literal, so it is assembled raw rather than through
    // Ins (whose $-substitution would eat the literal's ${...} interpolations).
    void TsGenPrint();

    // Generate interface for current ctype that is to be used against JSON payloads
    // With JSON, most fields remain as strings
    void TsGenJsonInterface();
    void JsGenCtype();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_nsjs_ctype(); // gstatic/amcdb.gen:nsjs_ctype
    void TsGenMsgtype();

    // Generate $ns_MsgPrint: decode a packed message by its msgtype and render it
    // with the ctype's _Print; returns null for a type this namespace does not define.
    void TsGenMsgPrint();
    void JsGenNs();

    // nsjs_ns gen phase: emit each nsjs namespace's file-level prologue -- the
    // text codec globals, the msgtype enum, and the message print dispatch.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_nsjs_ns(); // gstatic/amcdb.gen:nsjs_ns
    // void gen_nsjs_module(); // gstatic/amcdb.gen:nsjs_module

    // -------------------------------------------------------------------
    // cpp/amc/kafka.cpp
    //

    // Generate the kafka wire codec for a ctype with a ckafka record:
    // DIR 0 emits the encoder (ctype to byte buffer), any other value the
    // decoder (byte buffer to ctype). Fields outside the message's valid
    // version range are skipped at runtime by generated version checks.
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

    // Insert ROW at an explicit position: before BEFORE, or at the tail when
    // BEFORE is NULL -- the position-addressed primitive of a doubly-linked list,
    // of which head insertion (BEFORE = First) and the listtype's own Insert
    // direction are the two fixed special cases.  Lets a caller rank a row ahead
    // of the list order (an eviction candidate that should go first) without a
    // second list.  Emitted only for non-circular doubly-linked listtypes with a
    // tail pointer: prev links make the splice O(1), and the tail pointer makes
    // the BEFORE = NULL case O(1).
    // void tfunc_Llist_InsertBefore(); // gstatic/amcdb.tfunc:Llist.InsertBefore
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

    // Generate the lpool's untyped allocator. Small size classes are served
    // from a per-class block (a freed record first, else the bump tip); larger
    // sizes take a whole level block. Raw blocks come from the lowest populated
    // free level at or above the request, splitting oversized upper halves back
    // onto the free lists, and are refilled from the base pool at huge-page
    // friendly sizes when every suitable level is empty.
    // void tfunc_Lpool_AllocMem(); // gstatic/amcdb.tfunc:Lpool.AllocMem

    // Generate the lpool's pre-reservation function: allocate NBUF buffers of
    // BUFSIZE bytes through the allocator, then free them all, leaving the
    // free store stocked for later allocation.
    // void tfunc_Lpool_ReserveBuffers(); // gstatic/amcdb.tfunc:Lpool.ReserveBuffers
    // void tfunc_Lpool_ReallocMem(); // gstatic/amcdb.tfunc:Lpool.ReallocMem
    // void tfunc_Lpool_Init(); // gstatic/amcdb.tfunc:Lpool.Init
    // void tfunc_Lpool_N(); // gstatic/amcdb.tfunc:Lpool.N

    // -------------------------------------------------------------------
    // cpp/amc/main.cpp -- Main driver
    //

    // Query whether a value of CTYPE has to be destroyed rather than simply
    // forgotten: whether generated code that stops using one must call a destructor
    // for it.
    // See gen_plaindata, which computes the flag from what the ctype contains -- an
    // owning field, a Cleanup callback, a cascdel dependent, or an index the record
    // must unlink itself from. That list is what a generated destructor does, so a
    // ctype containing none of it has nothing to destroy.
    bool HasDtorQ(amc::FCtype &ctype);

    //
    bool CanCopyQ(amc::FCtype &ctype);

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

    // Emit a statement printing FIELD of the record named PARENTNAME into the
    // string named STRNAME: the field's own Print function when one exists
    // (parent argument collapsed by ParentArgExpr when the parent is a global),
    // otherwise the field type's Print applied to the field value.
    void GenPrintStmt(cstring &out, amc::FCtype &parenttype, amc::FField &field, strptr strname, strptr parentname);
    bool FldfuncQ(amc::FField &field);
    bool PoolVarlenQ(amc::FField &field);
    bool BlkpoolQ(amc::FField &field);
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

    // The builtin CTYPE stands for, as a bltin id: a ctype whose single field is
    // a Val stands for whatever that field holds, so algo.Uint32 -- one Val field
    // over u32 -- stands for u32, and a chain of such wrappers stands for the type
    // at its end. True when the chain ends at a builtin amc knows by name, false
    // when it ends at a struct. The name is the authority, not the amcdb.bltin
    // row: a universe that omits the row still names the builtin the same way.
    // A chain of such wrappers can also close on itself -- a ctype whose one Val
    // field names the ctype itself, or a pair that name each other -- and a walk
    // of it would never reach an end. The descent is therefore capped, and a
    // chain that reaches the cap names no builtin, which is what the callers
    // already report field by field before the size computation reports the
    // cycle: test/amc/bad_size_cycle.ssim carries a cycle of each shape with a
    // decimal field on it, so the run stays a diagnostic and never a hang.
    bool GetBltinId(amc::FCtype& _ctype, amc::BltinId &bltin_id);

    // Why a value of the integer builtin CTYPE stands for cannot move in and out
    // of CTYPE, as a sentence naming what is missing; empty when the value moves
    // both ways.
    // A field declared with a wrapper ctype -- one whose single Val field holds the
    // builtin -- stores the wrapper, while the range checks, scale factors and
    // parse results the numeric generators build are arithmetic on the builtin. So
    // every such generator converts in both directions. Out of the wrapper the
    // conversion is the operator dmmeta.fcast generates. Into it the conversion is
    // the fieldwise constructor, which the ctype gets from a dmmeta.cpptype row
    // asking for a constructor. A ctype carrying only one of the two takes the
    // value in one direction while the generated code names the other anyway, so
    // the answer names the one that is missing and the generator reports the field
    // rather than emitting C++ that does not compile.
    // The fieldwise constructor takes one argument per field, of that field's own
    // type, so it converts from the builtin only when the builtin is what the one
    // field holds. A chain of two wrappers converts from the inner wrapper instead,
    // and is named as standing for the builtin only through another ctype. The
    // builtin itself needs no conversion at all and is the empty answer.
    tempstr BadBltinCast(amc::FCtype &ctype);

    // Numeric range of CTYPE as u64 bit patterns; CTYPE may wrap the bltin in
    // a chain of single-Val-field ctypes. The resolved bltin id determines
    // bounds and signedness alike: a signed integer named by a universe that
    // lacks the type's amcdb.bltin row must still get signed range checks.
    // The types with a range are the fixed-width integers, u8 through i64.
    // Returns false for everything else -- bool, char, pad_byte, u128, float
    // and double -- and hands back the widest signed bounds as a fallback:
    // bool has only two values and cannot carry a magnitude, char's signedness
    // is the compiler's choice, and amc has no arithmetic for the rest.
    // Range checks built from the fallback bound nothing, so each generator
    // that needs a range rejects the field before its checks are emitted
    // (gen_check_lenfld for a length word, tclass_Dec for a fixed-point value,
    // tclass_Numstr for a numeric string), and the run exits nonzero.
    bool GetMinMax(amc::FCtype& _ctype, u64 &min, u64 &max, bool &issigned);

    // Query whether FIELD's type is passed by value; see gen_cheapcopy.
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

    // Byte size of FIELD's storage: the arg's byte size (via Ctype_Nbit: builtin
    // csize, or the totsize_byte computed by gen_compute_size) times the inline
    // array multiple. Before gen_compute_size runs, a non-builtin arg reports 0.
    // i64 like Ctype_Nbit: an uncapped csize times an array multiple can exceed
    // i32, and callers scale the result back up to bits.
    i64 Field_Sizeof(amc::FField &field);

    // Remove naming layers from ctype (i.e. as long as ctype contains
    // a single val field, keep going down, then finally return the basic type).
    // If the ctype contains more than one such field at any given layer, the
    // layer names no single type below it and the answer is DFLT.
    // The layers can also close on themselves -- a ctype whose one Val field
    // names the ctype itself, or a pair that name each other -- and a walk of
    // such a chain reaches no basic type at all. The descent is therefore
    // capped, and a chain that reaches the cap answers DFLT as well: a cycle
    // names no type below it any more than an ambiguous layer does. Every
    // caller already has an answer for DFLT, so a cyclic ctype is left to the
    // diagnostics the schema checks report for it -- the field-level ones
    // naming each field, and the size computation naming the cycle itself.
    // test/amc/bad_size_cycle.ssim carries a bitfield whose arg is a cycle and
    // a cycle under a bitfield's source field, so the run is pinned as a
    // diagnostic rather than an internal error.
    amc::FCtype *StripWrappers(amc::FCtype &ctype, amc::FCtype *dflt);
    bool FixaryQ(amc::FField &field);

    // Return the parent-argument prefix for a call to one of the field
    // accessors of CTYPE (Get, Set, ReadStrptrMaybe, Set<Present>, the pmask
    // bitset accessors): the accessors of a global ctype's fields take no
    // parent argument -- there is exactly one instance, reached through the
    // namespace global -- so for a global CTYPE the result is empty.
    // Otherwise the result is PARENT, followed by ", " when COMMA is set
    // (the parent precedes further arguments in the call).
    tempstr ParentArgExpr(amc::FCtype &ctype, strptr parent, bool comma);

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

    // True when CTYPE's frame length is known only at runtime: a varlen tail
    // appends bytes past the fixed portion, and an Opt field appends one optional
    // element there. A ctype with neither occupies exactly its fixed size, so
    // every store of its length writes a value the generator itself computed.
    // This is the only predicate in amc for the question "is this frame's length
    // fixed": gen_check_lenfld reads it to decide which totals it can prove
    // storable at generation time, and the Fmt* constructor, the TS Encode and
    // the dispatch read consult it before deciding whether their store of a
    // runtime total needs a guard.
    bool RuntimeFrameLenQ(amc::FCtype &ctype);

    // Largest frame byte count the wire format admits, in bytes.
    // Consider a 3GiB frame whose length word is an unsigned 32-bit field: the
    // word holds the count exactly, so a writer that consults only the word's
    // range sends the frame. Its reader reconstructs the total through
    // LengthExpr, which evaluates in i32, and reads a negative length -- the
    // frame is framed as a message nobody wrote. The count a length word can
    // hold is therefore not the count the format admits, and the two backends
    // cannot each pick their own: whatever one encoder produces, the other's
    // reader must be able to represent.
    // The narrower of the two representations is the i32 the C++ reader
    // reconstructs in, which is also what every buffer size argument in the
    // tree is expressed as, so the domain is i32 and it is stated here alone.
    // The C++ range guard drops its maximum term once the length word already
    // covers this bound (LenfldCheckExpr), the message constructor refuses a
    // total past it, and the TypeScript encoder throws at it.
    u64 FrameLenMax();

    // Return C++ expression computing total length of ctype CTYPE
    // accessible via name NAME.
    tempstr LengthExpr(amc::FCtype &ctype, strptr name);

    // Return C++ expression computing total length of ctype CTYPE accessible
    // via name NAME, in i64 domain. LengthExpr runs the scale multiply in the
    // length word's own type, so a corrupt stored value wraps mod 2^N into a
    // small plausible total; here the word is widened to i64 first, and the
    // exact total survives for a caller that validates an untrusted length
    // word against the storable range before narrowing (InsertMaybe).
    // The widening is exact only while the arithmetic fits i64: for a word
    // range wider than u32, at any scale, the caller must bound the raw word
    // before evaluating this expression (InsertMaybe emits a generation-time
    // bound) -- otherwise the scaled multiply, or at scale 1 the extra and
    // fixed-size adjustment after the u64->i64 wrap, itself overflows.
    tempstr LengthExpr64(amc::FCtype &ctype, strptr name);

    // Max value LENFLD's word can store, which is the same question FieldMaxStore
    // answers about the word's field: the arg type's max, clamped to the bits a
    // Bitfld store keeps.
    // Returns false when the arg resolves to no integer type: the word then has
    // no numeric range, MAX holds the widest-signed fallback, and gen_check_lenfld
    // rejects the schema, so generated expressions built from the fallback are
    // never consumed.
    bool LenfldMaxStore(amc::FLenfld &lenfld, u64 &max);

    // Largest frame byte total LENFLD can store: max storable word * scale - extra
    // (the reader formula at the word's max). Exact for a word range within u32 --
    // every range an emitter guards; a wider word stores any frame total either
    // backend can meet (C++ i32, JS 2^53), so the formula, which could wrap u64
    // there (a u64 word with a negative extra), saturates instead. A shipping
    // schema cannot go negative (gen_check_lenfld rejects extra beyond the
    // range); the clamp keeps the reject-run expression harmless.
    u64 LenfldMaxLen(amc::FLenfld &lenfld);

    // True if a runtime byte total stored through LENFLD can fall below the
    // smallest total the word represents, so that the store numerator
    // (total + extra) goes negative and wraps through the word's own type.
    // A total of 1 byte stored through a u8 word with extra:-8 writes 249, and
    // the reader reconstructs 257 -- a frame 256 bytes longer than was written.
    // Only a negative extra can produce it, and only one whose magnitude exceeds
    // the ctype's fixed size: a store site's smallest total is that fixed size,
    // so a schema whose fixed size already covers the extra can never reach the
    // low end. gen_check_lenfld rejects the same shape outright wherever the
    // stored total is the fixed size itself; here the total is a runtime one.
    bool LenfldLowGuardNeededQ(amc::FLenfld &lenfld);

    // True if a runtime byte total stored through LENFLD needs a guard before
    // the store: an indivisible total truncates through the store formula
    // (scale != 1), a total beyond the storable range wraps mod 2^N through
    // the word's own type, and a total below the low end wraps the same way
    // (LenfldLowGuardNeededQ). A scale-1 lenfld whose range covers the whole
    // frame-length domain (FrameLenMax), and whose fixed size covers its extra,
    // can store any total and needs no test at all.
    bool LenfldGuardNeededQ(amc::FLenfld &lenfld);

    // Return C++ boolean expression testing that LENEXPR, an expression for a
    // total byte length, is representable in LENFLD: the store formula
    // (LENEXPR + extra) / scale truncates unless scale divides the numerator
    // (the reader reconstructs stored*scale - extra, so only a multiple
    // round-trips), a total beyond the storable range (LenfldMaxLen) wraps
    // mod 2^N through the word's own type, framing a shorter message than was
    // written, and a total below the low end (LenfldLowGuardNeededQ) wraps the
    // same way, framing a longer one. Emitters of a runtime-length store guard
    // with this expression when LenfldGuardNeededQ; each term is emitted only
    // when its failure is possible, so the expression is never vacuous at a
    // guarded site.
    tempstr LenfldCheckExpr(amc::FLenfld &lenfld, strptr lenexpr);

    // Return C++ expression computing the value stored in LENFLD from LENEXPR,
    // an expression for the total byte length: (LENEXPR + extra) / scale --
    // the exact inverse of the reader formula (LengthExpr: total = stored*scale - extra).
    tempstr LenfldStoreExpr(amc::FLenfld &lenfld, strptr lenexpr);

    // True if the field has no direct member to write: the value lives behind
    // an accessor pair (fbigend byte-swapped storage, a bitfld slice of its
    // source word, a fldfunc computed view), so any store must go through the
    // generated $name_Set. Codecs that manage side effects themselves (the
    // kafka decoder, whose presence bits depend on the wire's null flag and
    // must not be set by a pmask Set) route stores on this predicate alone.
    bool NoDirectMemberQ(amc::FField &field);

    // True if a store to the field must go through the generated $name_Set:
    // the field has no direct member (NoDirectMemberQ), or a Set exists to
    // dispatch side effects (fcond membership, pmask presence).
    // Generated store sites that run after the record is xreffed (assignment,
    // read fallbacks) route on this predicate. Init is the deliberate
    // exception: it writes the raw member of a freshly allocated record,
    // before XrefMaybe derives fcond membership from the value and before
    // the presence mask may be touched.
    bool AssignViaSetQ(amc::FField &field);

    // Return C++ expression assigning value VALUE to field FIELD
    // given parent reference PARNAME.
    // If NEEDS_CAST is set, a cast is added to the target type
    tempstr AssignExpr(amc::FField &field, strptr parname, strptr value, bool needs_cast, strptr fldname = strptr());

    // Heuristically determine if the type is a string type.
    // When in doubt, the function answers no.
    bool StringQ(amc::FCtype &ctype);

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

    // Root for side-loaded table data (gconst and gstatic values, gsymbol
    // tables): the -in_dir root when it names a directory. A single-file or
    // stdin -in_dir provides no directory for side files; the default data
    // root applies. The root is resolved once, in Main -- the answer cannot
    // change within the run, and DataRoot is consulted per gconst, gstatic,
    // and gsymbol row and per manifest side-load.
    algo::strptr DataRoot();

    // Load side table data (gconst and gstatic values, gsymbol tables) from
    // FNAME, a filename already resolved against DataRoot(). When -in_dir is
    // not a directory, side tables resolve against ./data of the current
    // working directory, which may belong to a different universe than the
    // -in_dir schema: a run from an unrelated checkout would quietly generate
    // symbol and constant values from that checkout's tables and exit 0. The
    // first successful load through the fallback root therefore prints a
    // notice naming the root and the file, making the side-load visible in
    // the run's output.
    bool SideloadFile(algo_lib::MmapFile &file, strptr fname);

    // Path under ROOT of the ssimfile CTYPE's rows live in, either because CTYPE
    // owns the ssimfile or because the type it is based on does; empty when CTYPE
    // reaches no ssimfile.
    // A ctype fails to reach a file two ways: it declares no base at all, or a base
    // that owns no ssimfile. Both are the same answer to this function's question,
    // so both come back as an empty path rather than as an abort. Ending the run
    // here would take the diagnostic away from the caller, and the caller is the one
    // that knows which gconst or gstatic field asked -- which is what it names.
    tempstr SsimFilename(strptr root, amc::FCtype& ctype);

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
    // cpp/amc/msgschema.cpp
    //

    // Derive dmmeta.payloadhdr, dmmeta.msg and dmmeta.msgfield rows for every
    // message ctype whose ultimate base is a header with typefld and lenfld,
    // and enforce that msgtype numbers are unique within one payload header.
    // Packing is not required: the layout walk inserts the same alignment
    // padding the compiler does, so an unpacked message derives correct
    // offsets.  Messages whose layout cannot be represented (an unsupported
    // reftype, a non-scalar leaf) are skipped whole, with a verbose log line
    // naming the ctype.  Runs after compute_size (offsets need sizes) and
    // before table_write (which persists the derived rows).
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_msgschema(); // gstatic/amcdb.gen:msgschema

    // -------------------------------------------------------------------
    // cpp/amc/numstr.cpp -- Small strings
    //

    // Set up the numeric-range substitution variables shared by the numstr
    // tfuncs, and fail the amc run on a spec whose stored strings could not
    // parse back to the stored value: a pad or a base the sign cannot be told
    // apart from, a pad that reads as a digit, or a min_len wider than the
    // string, the pad budget, or the room the sign needs
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Numstr(); // gstatic/amcdb.tclass:Numstr

    // Generate $name_Getnum: parse the stored digit string back to the numeric
    // type, clearing and_ok on an invalid digit or a value outside the numtype
    // range; the empty string reads as zero
    // void tfunc_Numstr_Getnum(); // gstatic/amcdb.tfunc:Numstr.Getnum

    // Generate $name_GetnumDflt: read the stored value through Getnum,
    // returning DFLT when the string does not parse
    // void tfunc_Numstr_GetnumDflt(); // gstatic/amcdb.tfunc:Numstr.GetnumDflt

    // Generate $name_Geti64: read the stored value as an i64, failing a value
    // above i64max rather than wrapping it negative through the cast
    // void tfunc_Numstr_Geti64(); // gstatic/amcdb.tfunc:Numstr.Geti64

    // Generate $name_SetnumMaybe: format the number into the string through an
    // auxiliary buffer, refusing -- with the stored string left unchanged -- a
    // value outside the numtype range or digits that do not fit the string
    // void tfunc_Numstr_SetnumMaybe(); // gstatic/amcdb.tfunc:Numstr.SetnumMaybe

    // Generate $ns::ForAllStrings: each numstr field appends a block filling
    // a descriptor -- accessor pointers and the shape facts -- and calling
    // the test function on it
    // void tfunc_Numstr_ForAllStrings(); // gstatic/amcdb.tfunc:Numstr.ForAllStrings
    // void gen_parsenum(); // gstatic/amcdb.gen:parsenum

    // -------------------------------------------------------------------
    // cpp/amc/opt.cpp -- Opt reftype
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Opt(); // gstatic/amcdb.tclass:Opt
    // void tfunc_Opt_Get(); // gstatic/amcdb.tfunc:Opt.Get
    // void tfunc_Opt_Getary(); // gstatic/amcdb.tfunc:Opt.Getary

    // Generate the print function for an Opt field: append the optional
    // trailing element to OUT when present.
    // void tfunc_Opt_Print(); // gstatic/amcdb.tfunc:Opt.Print

    // Generate the read function for an Opt field: parse the optional trailing
    // element from a string into the active varlen buffer and mark the field
    // in the parent's presence mask, when one covers it. An element whose byte
    // total lands off the scale of its lenfld has no representable length word,
    // and the read reports failure; the buffer holds a truncated length word,
    // and the caller discards it on the false return.
    // void tfunc_Opt_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Opt.ReadStrptrMaybe

    // -------------------------------------------------------------------
    // cpp/amc/outfile.cpp -- Output functions
    //

    // Write output file to disk
    // and deallocate memory associated with it
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_write(); // gstatic/amcdb.gen:ns_write

    // Create outfile record for specified filename
    // T here is one outfile per generated output file
    amc::FOutfile &outfile_Create(strptr filename);

    // -------------------------------------------------------------------
    // cpp/amc/pbuf.cpp -- Protobuf wire codec
    //

    // Check that every fpbuf field names a known protobuf type: pb_type must
    // have an amcdb.pbtype row, the table binding each protobuf type to its
    // wire type and lib_pb codec functions. With an unrecognized pb_type the
    // codec would emit an encoder that writes the field tag with no payload and
    // a decoder case that consumes the tag but not the value -- both corrupt
    // the wire stream.
    // The field number is bounded by the wire tag: EncodeTag packs (number<<3)
    // | wire_type into a varint, so a number at or above 2^29 aliases a
    // different field on the wire, and number 0 encodes tag 0, which the
    // protobuf wire format reserves as illegal -- conformant peers reject the
    // message even though it round-trips against amc's own decoder. The span
    // 19000..19999 is rejected on the same ground: the protobuf format reserves
    // it for its own implementation, so protoc refuses to compile the equivalent
    // .proto and a peer rejects the field on the wire. Duplicate numbers within
    // one ctype would emit a decoder switch with two identical case labels:
    // uncompilable generated code.
    // The field's own shape is checked against the declared pb_type: the codec
    // substitutes the field access straight into the lib_pb codec calls, so an
    // arg other than the one the codec functions accept (pbtype.argtype), a
    // non-cpbuf arg under pb_type message, an arg without the enum shape
    // (PbufEnumQ) under pb_type enum, or a reftype with no wire mapping
    // (anything but Val, Tary, Bitfld -- including Base, which emission never
    // writes to the wire) produces generated code that does not compile --
    // shipped at exit 0 unless rejected here. The same holds for a singular
    // field the decoder can only store through a Set that amc never generates
    // (a substr, a falias, a cppfunc with set:N).
    // An fpbuf row can also be silently dead: the codec is generated only for
    // a ctype with a cpbuf row, so fpbuf rows on a cpbuf-less ctype produce no
    // code at all and the omission would surface only as a later link error or
    // as wire messages missing those fields. Such rows are rejected here too.
    // The message's own attribute, cpbuf.syntax, names a row of dmmeta.pbsyntax,
    // the table that says whether the syntax has implicit presence. A syntax
    // with no row leaves the encoder without that answer, and it would emit the
    // explicit-presence shape -- every scalar on the wire whatever its value --
    // for a message the author declared otherwise. acr -check rejects the
    // spelling when the row goes through the database, but amc reads a schema
    // out of -in_dir too, so the same value is checked here.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_pbuf(); // gstatic/amcdb.gen:check_pbuf

    // Check that the variants of every oneof share one presence mask.
    // A oneof is a union: at most one variant may be set, and the encoder must
    // write at most one variant's bytes. Consider a message with variants a and
    // b and no presence mask: the encoder guards each singular field with the
    // proto3 default-value test, so a record with both members nonzero writes
    // both field numbers, and the peer keeps whichever came last -- the sender
    // silently drops the other. Two variants on two different masks fail the
    // same way, because the sibling bits a store clears are the ones on the mask
    // it writes.
    // The mutual exclusion the encoder relies on therefore lives entirely in one
    // mask: a store into a variant sets that variant's bit and clears the bit of
    // every sibling variant on the same mask, so the encoder's presence guard is
    // true for at most one of them. Requiring a mask that holds every variant of
    // the oneof is the condition under which that holds.
    // This runs after gen_pmask rather than alongside the other fpbuf checks
    // because a mask's member set is derived there: a pmaskfld with no explicit
    // member rows takes every field of the ctype, and the membership this rule
    // reads does not exist until that has happened.
    // void gen_check_pbufcase(); // gstatic/amcdb.gen:check_pbufcase

    // Generate the protobuf codec of a cpbuf ctype: DIR 0 emits the encoder
    // (write each fpbuf field to a byte array), DIR 1 the decoder (a
    // tag-driven switch reading fields from a memptr)
    void PbufCodec(int dir);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_PbufEncode(); // gstatic/amcdb.tfunc:Ctype.PbufEncode
    // void tfunc_Ctype_PbufDecode(); // gstatic/amcdb.tfunc:Ctype.PbufDecode

    // -------------------------------------------------------------------
    // cpp/amc/pmask.cpp -- Presence masks
    //

    // Prepare each pmask for generation: derive the accessor name, create the
    // backing bitset, populate the member set (every suitable field, unless an
    // explicit member list was provided), validate the members, and assign
    // each member its bit
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_pmask(); // gstatic/amcdb.gen:pmask

    // Check that every pmask has no more member fields than its pmask field has bits.
    // Runs after gen_compute_size: a pmask field of non-builtin type has no csize,
    // so its byte size is only known once struct sizes are computed.
    // A pmask field whose size is unknown even then (nbit 0) is reported by
    // tclass_Bitset, not here.
    // void gen_check_pmask(); // gstatic/amcdb.gen:check_pmask
    // void tclass_Pmask(); // gstatic/amcdb.tclass:Pmask

    // Create multiple functions, one for each pmask of which this field is a member
    // void tfunc_Pmask_PresentQ(); // gstatic/amcdb.tfunc:Pmask.PresentQ

    // Emit C++ that sets FIELD's bit on the pmaskfld carrying MEMBER and,
    // when FIELD is a oneof variant, clears the bits of its sibling
    // variants within the same oneof on that same pmaskfld.
    tempstr SetPresentForMember(amc::FField &field, amc::FPmaskfldMember &member, strptr parent);

    // Return C++ expression testing the present bit of FIELD, via the
    // generated <field>_<Funcname>Q accessor of each pmask of which the field
    // is a member, AND-joined over the memberships; PARENT names the parent
    // record (collapsed by ParentArgExpr when the parent is a global).
    // Returns empty string if the field has no pmask membership.
    tempstr PresentQExpr(amc::FField &field, strptr parent);

    // Return C++ expression(s) setting the present bit for the field
    // in all presence masks of which the field is a member.
    // If the field is a oneof variant, sibling variants are also cleared.
    // Returns empty string if the field has no pmask membership.
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

    // Check each lenfld against the store formula len = (fixed size + extra) / scale
    // (LenfldStoreExpr, the inverse of the reader formula): when the numerator is
    // not a multiple of scale, the stored length truncates and the reader
    // reconstructs less than was written.
    // Consider a packed jstype ctype with a 6-byte fixed portion, a varlen tail,
    // and an unsigned 32-bit length field at scale 4. Its TypeScript encoder never
    // stores 6: it stores a runtime total that counts the tail, having already
    // guarded that total against the scale. Asking whether 6 divides by 4 rejects
    // the schema over a store that never happens, and leaves the author padding
    // the fixed portion to satisfy it.
    // The two questions a store site raises are distinct. One is whether the fixed
    // size is itself the value stored; the other is whether the ctype stores any
    // total, every one of which is at least the fixed size. A ctype of the first
    // kind needs a fixed size the formula represents exactly, because the
    // generator computes that value and no runtime guard stands in front of it. A
    // ctype of the second kind needs a fixed size the formula can represent at
    // all, that being the smallest total its store will ever see.
    // So the arms hang off two predicates. STOREFIXED names a ctype whose fixed
    // size is the stored value -- pool alloc (see GenAllocFunc), the message
    // constructor and field default, which store ssizeof for any msgtype ctype
    // (a Bitfld lenfld included: AssignExpr routes the store through the bitfld
    // setter), the Fmt* constructor (pnew) of a fixed-only ctype, and the
    // TypeScript Encode of a ctype whose frame length is fixed for the same
    // reason -- and it carries the divisibility arm. STORETOTAL adds the
    // TypeScript Encode of a ctype with a tail, and carries the range and
    // low-end arms.
    // A TypeScript store site is a Val-reftype lenfld of a packed jstype ctype in
    // a typescript namespace: the TS fixed walk skips bitfields and writes the
    // source word raw from the parent, so a Bitfld lenfld carried only by the TS
    // Encode has no store site.
    // Runs after gen_compute_size, which provides the fixed sizes. A ctype whose
    // size amc does not know (size_unknown: cextern) has nothing checkable, and
    // a ctype with no store site is legal even when indivisible: its length
    // words arrive from outside. Stores of runtime totals (AllocExtra, the
    // nested-varlen read, the Opt read, the dispatch read, the Fmt* constructor
    // of a varlen/opt ctype) add bytes on top of the fixed size and guard at
    // runtime instead.
    // The lenfld type also bounds extra: with a positive extra exceeding the
    // type's max times scale, every value the type can hold maps to a negative
    // frame length (reader formula: total = stored*scale - extra), so the field
    // can frame no message at all. Such a lenfld is rejected here for every
    // ctype, store site or not -- even a length word arriving from outside can
    // express no frame -- and downstream emitters (TsLenfldStoreExpr) rely on
    // the resulting invariant max*scale >= extra to compute their runtime
    // guard bound without wrapping.
    // A ctype with a store site tightens the bound: its fixed size is the
    // smallest frame the store ever sees, so the stored value of that frame,
    // (fixed size + extra) / scale, must itself lie in the type's range. Below
    // zero -- a negative extra exceeding the fixed size -- the store expression
    // goes negative and wraps through the unsigned store (setUint32(-1) frames
    // gigabytes from a minimal message); above max, the emitted range guard
    // bound falls below the fixed size and every encode throws at first use.
    // Either way no frame the ctype can produce is storable, so the schema is
    // rejected rather than shipped as a runtime trap.
    // void gen_check_lenfld(); // gstatic/amcdb.gen:check_lenfld

    // Ensure that all instances of CTYPE's record are varlen pools
    void EnsureVarlenPool(amc::FCtype &ctype);

    // True when a pool allocates values of CTYPE, in any namespace
    bool HasAllocInstQ(amc::FCtype &ctype);

    // Find pool used for allocating values of type CTYPE
    amc::FField *FindPool(amc::FCtype &ctype);

    // Find default used for allocating things in namespace NS
    amc::FField *DefaultPool(amc::FNs &ns);

    // -------------------------------------------------------------------
    // cpp/amc/print.cpp -- Print ctype to string
    //

    // Create print function for ctype PARENT based on CFMT.
    // The pair (strfmt, printfmt) selects the body. The Extern printfmt hands the
    // body to the user for every strfmt; otherwise String takes a printfmt of
    // Tuple, Auto, Bitset, Sep or Raw, Argv and ArgvGnu take any printfmt (the
    // command line spells its own separators), and Json takes any printfmt as
    // well. Every other pair -- the Tuple strfmt, which has no print path at all,
    // and the CompactSep printfmt, which has no print path under String -- would
    // leave the function with an empty body, so it is rejected naming both
    // attributes. test/amc/bad_cfmt_print.ssim pins the pairs on both sides of
    // that line.
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

    // Generate $name_Cascdel: delete the last row of the array until it is empty
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

    // Generate $name_ScanInsertMaybe of a non-unique Ptrary: a linear scan
    // finds a row already present; an absent row is appended by delegating
    // to $name_Insert, which also fires any OnXref hook
    // void tfunc_Ptrary_ScanInsertMaybe(); // gstatic/amcdb.tfunc:Ptrary.ScanInsertMaybe
    // void tfunc_Ptrary_N(); // gstatic/amcdb.tfunc:Ptrary.N

    // Generate $name_Remove: unlink the row -- an O(1) swap with the last
    // element for a heaplike array, a backward scan and shift for a unique
    // one, a compacting sweep of every occurrence otherwise -- firing any
    // OnUnref hook
    // void tfunc_Ptrary_Remove(); // gstatic/amcdb.tfunc:Ptrary.Remove
    // void tfunc_Ptrary_RemoveFirst(); // gstatic/amcdb.tfunc:Ptrary.RemoveFirst
    // void tfunc_Ptrary_First(); // gstatic/amcdb.tfunc:Ptrary.First
    // void tfunc_Ptrary_RemoveLast(); // gstatic/amcdb.tfunc:Ptrary.RemoveLast
    // void tfunc_Ptrary_Last(); // gstatic/amcdb.tfunc:Ptrary.Last
    // void tfunc_Ptrary_RemoveAll(); // gstatic/amcdb.tfunc:Ptrary.RemoveAll
    // void tfunc_Ptrary_Reserve(); // gstatic/amcdb.tfunc:Ptrary.Reserve
    // void tfunc_Ptrary_Uninit(); // gstatic/amcdb.tfunc:Ptrary.Uninit

    // Generate the Ptrary cursor: the struct plus Reset/ValidQ/Next/Access;
    // a ONCE cursor detaches the captured run's membership at Reset -- the
    // array empties and every captured element's membership flag clears up
    // front, so a walk that exits early leaves no row claiming membership --
    // and walks the captured run. The run itself stays in the parent's
    // buffer, which the cursor aliases without copying, so inserting into
    // the array during the walk remains forbidden: an insert would
    // overwrite the unread tail of the run
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

    // Emit the base allocator: it serves a block of a granule or more by mapping it,
    // preferring huge pages while the process is under its huge-page ceiling.
    //
    // The ceiling and the kernel's refusal are two different answers and the
    // allocator must not confuse them.  Consider a process whose ceiling is 4GB and
    // whose receive cache asks for one 12GB block: the block cannot be served on
    // huge pages, and it is served on ordinary ones.  If that outcome also retires
    // the huge route, every later block -- a two-megabyte pool block that the
    // ceiling has ample room for -- is mapped on ordinary pages too, and the process
    // runs its whole heap on 4K pages because of one request the ceiling was
    // supposed to merely decline.
    //
    // So the route is retired only when the huge route was actually taken and the
    // kernel refused it: no pages left in the pool, a cgroup allowance already
    // spent, or the mlock that verifies the mapping failing.  Those answers do not
    // change for the life of the process, which is what makes retiring the route the
    // right response to them and the wrong response to a block that was simply too
    // big to count against the ceiling.
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

    // Determine which ctypes are cheap copy, i.e. passed to a function by value
    // rather than by const reference.
    // A ctype is cheap copy when copying it is a memcpy -- that is what plaindata
    // means -- and when it is small enough that the copy costs no more than the
    // reference would. Both halves are computed: the property is a fact about the
    // ctype's contents, so nothing asserts it by hand and nothing can assert it
    // wrongly.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_cheapcopy(); // gstatic/amcdb.gen:cheapcopy

    // Bit width of a ctype's storage: builtins carry an explicit csize; other
    // types carry a computed totsize_byte. Returns 0 when the size is not known
    // in this generation run: neither value is set, or the size computation
    // failed (size_unknown), in which case totsize_byte holds only the partial
    // sum of the members counted before the failure and must not be reported
    // as a width.
    // The result is i64: the size cap (NarrowSize) and the uncapped csize.size
    // both permit byte sizes whose bit count exceeds i32, and a wrapped width
    // would misfire every >0-gated width check downstream.
    i64 Ctype_Nbit(amc::FCtype &ctype);

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

    // The strtype pad character of SMALLSTR: a quoted pad ("'x'") carries the
    // character between the quotes; any other spelling is the NUL byte.
    // The pad is substituted into the generated code as written, so it is a C++
    // character literal and its escaped spellings name the same bytes the
    // compiler will see: "'\0'" is the NUL byte and "'\x30'" is the digit zero,
    // not the backslash that begins them. Reading the second character verbatim
    // would judge every escaped pad against the backslash, and scanning the whole
    // spelling would judge it against the digits of its own escape body -- so a
    // NUL pad, no digit of any base and no part of any number, would be refused
    // for the '0' that spells it.
    char PadChar(amc::FSmallstr &smallstr);
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

    // Validate the fstep against the contract stated at the top of this file
    // and add the step's state fields (next/delay for InlineRecur, the time
    // hook for TimeHook steps); every schema shape the Step tfuncs cannot
    // serve is reported here as a generation error, and the run continues so
    // one pass names every defect -- the error count withholds all output
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Step(); // gstatic/amcdb.tclass:Step
    // void tfunc_Step_UpdateCycles(); // gstatic/amcdb.tfunc:Step.UpdateCycles
    // void tfunc_Step_Step(); // gstatic/amcdb.tfunc:Step.Step

    // Generate the step's Init statements: the delay variable for
    // InlineRecur, the time-hook setup (and its delay) for the TimeHook
    // steptypes
    // void tfunc_Step_Init(); // gstatic/amcdb.tfunc:Step.Init

    // Generate $name_Call: invoke $name_Step on the steptype's schedule --
    // delay-gated for InlineRecur, expiration-driven off the first row's
    // sort field for the Once steptypes, every pass for Inline, bare for
    // the hook- and caller-driven steptypes
    // void tfunc_Step_Call(); // gstatic/amcdb.tfunc:Step.Call
    // void tfunc_Step_FirstChanged(); // gstatic/amcdb.tfunc:Step.FirstChanged
    // void tfunc_Step_SetDelay(); // gstatic/amcdb.tfunc:Step.SetDelay

    // -------------------------------------------------------------------
    // cpp/amc/struct.cpp -- C++ struct output
    //

    // True if the field can be passed via constructor
    bool PassFieldViaArgQ(amc::FField &field, amc::FCtype &ctype);

    // Emit struct definition for CTYPE into NS's header
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
    // void tfunc_Tary_GetAlloc(); // gstatic/amcdb.tfunc:Tary.GetAlloc
    // void tfunc_Tary_BeginAlloc(); // gstatic/amcdb.tfunc:Tary.BeginAlloc
    // void tfunc_Tary_GetAllocAppend(); // gstatic/amcdb.tfunc:Tary.GetAllocAppend
    // void tfunc_Tary_BeginAllocAppend(); // gstatic/amcdb.tfunc:Tary.BeginAllocAppend
    // void tfunc_Tary_RemRegion(); // gstatic/amcdb.tfunc:Tary.RemRegion

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
    // The tfunc may be a cursor generator (indicated by its tcurs row, or by
    // its name when the row is absent from the input set).
    // If it's a cursor generator, then a forward-declaration for the cursor is created.
    // In this case, variables $fcurs (cursor key) and $curstype (cursor type) are set.
    void GenTclass(amc::FTclass &tclass);

    // A cursor generator's amcdb.tcurs row is part of its input set: a universe
    // that reaches the generator without the row is incomplete, so the missing
    // row is reported as a generation error naming the tfunc and the field.
    // Generation continues -- the cursor name derives from the tfunc name, so
    // the emitted cursor is the same one the complete universe produces -- and
    // the error count withholds all output. Called by the generators that
    // consume $curstype (Ptrary, Llist) before they emit anything.
    void RequireTcurs();

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

    // Emit the member variable for a Val field, with the field default
    // retargeted from *this to the instance expression; a fldfunc-computed
    // field emits no variable. Rejects a non-extern finput on a Val field
    // and the fbigend+fldfunc combination.
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

    // Set up the varlen field: declare its end-offset variable when a later
    // varlen field needs one, set the length expressions, ensure the parent
    // pool can hold trailing data, and fail the run when a non-varlen field
    // follows.  gen_check_varlen rejects an element type with a destructor.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Varlen(); // gstatic/amcdb.tclass:Varlen
    // void tfunc_Varlen_Addr(); // gstatic/amcdb.tfunc:Varlen.Addr
    // void tfunc_Varlen_Getary(); // gstatic/amcdb.tfunc:Varlen.Getary
    // void tfunc_Varlen_N(); // gstatic/amcdb.tfunc:Varlen.N

    // Generate $name_ReadStrptrMaybe, appending one element parsed from the
    // string to the varlen buffer under construction: char/u8 bytes are copied
    // raw, a typefld element parses through the message dispatcher, and any
    // other element parses in place -- a nested varlen element collects its
    // own tail in a fresh buffer, stores its length through its own lenfld,
    // and splices in at the field's end offset
    // void tfunc_Varlen_ReadStrptrMaybe(); // gstatic/amcdb.tfunc:Varlen.ReadStrptrMaybe
    // void tfunc_Varlen_curs(); // gstatic/amcdb.tfunc:Varlen.curs
    // void tfunc_Varlen_Print(); // gstatic/amcdb.tfunc:Varlen.Print
    // void tfunc_Varlen_Init(); // gstatic/amcdb.tfunc:Varlen.Init
}
