// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Header: include/amc.h -- Main header
//

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
    // cpp/amc/alias.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Alias();
    // void tfunc_Alias_Get();
    // void tfunc_Alias_Set();
    // void tfunc_Alias_ReadStrptrMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/avl.cpp
    //

    // Initialize the structs etc.
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Atree();

    // Predicate function, can take parent as argument in the case of complicated comparisons.
    // void tfunc_Atree_ElemLt();

    // Generate the Prev function for the element.
    // Always symmetrical to Next through substitution of
    // First -> Last and Left -> Right.
    // void tfunc_Atree_Prev();

    // Generate the Next function for the element
    // void tfunc_Atree_Next();

    // Initialize Root and Number of elements
    // void tfunc_Atree_Init();

    // Returns true if the element is in tree.
    // void tfunc_Atree_InTreeQ();

    // Returns true if the tree is empty.
    // Plant a tree if empty!
    // void tfunc_Atree_EmptyQ();

    // Given an element finds the smallest element in the subtree.
    // void tfunc_Atree_FirstImpl();

    // Given an element finds the largest element in the subtree.
    // void tfunc_Atree_LastImpl();

    // Returns the smallest element
    // void tfunc_Atree_First();

    // Returns the largest element.
    // void tfunc_Atree_Last();

    // Finds the child that violates the balance. Left child if no disbalance.
    // void tfunc_Atree_TallerChild();

    // Disconnect the node from its parent.
    // void tfunc_Atree_Disconnect();

    // Rotates the tree from the direction from->to
    // Assumption is that from is the child of to.
    // void tfunc_Atree_Turn();

    // 1. Find disbalanced child.
    // 2. Find disbalanced grandchild.
    // 3. turn the grandchild toward child to have a-b-c simple case.
    // 4. Turn child onto node.
    // 5. Make sure to NOT turn the grandchild into child if child is balanced(Many hours of debugging).
    // void tfunc_Atree_Rebalance();
    // void tfunc_Atree_UpdateDepth();

    // Keep updating the depth and propagating up.
    // If a node needs rebalancing we rebalance and go up
    // Notice that after rebalance a node can go deeper down the tree.
    // void tfunc_Atree_Propagate();

    // Iterate down the tree starting from the parent and place
    // the element in the appropriate leaf.
    // Note that balance might be broken after this operation.
    // void tfunc_Atree_InsertImpl();

    // 1.Insert element starting from the root.
    // 2. Rebalance if necessary.
    // 3. Assign new root.
    // void tfunc_Atree_Insert();

    // Leftbalance - Rightbalance
    // void tfunc_Atree_Balance();

    // A recursive function to delete elements.
    // if del flag is set it also destroys the elements.
    // void tfunc_Atree_RemoveAllImpl();
    // void tfunc_Atree_Cascdel();

    // Remove all elements without deleting them.
    // void tfunc_Atree_RemoveAll();

    // Remove the first element. Rebalance. Update root.
    // void tfunc_Atree_RemoveFirst();

    // Reinsert an element
    // void tfunc_Atree_Reinsert();

    // Connect 2 elements (either can be NULL).
    // void tfunc_Atree_Connect();

    // 1. Find next/prev element in my subtree (opposite of imbalance direction).
    // 2. If that element has a child, swap it with the child by turning from child.
    // 3. Swap the element to be removed with the next.
    // 4. Remove the element.
    // 5. Propagate up from the next's parent.
    // void tfunc_Atree_Remove();

    // First element that is greater or equal to the given value
    // void tfunc_Atree_FirstGe();

    // First element that is smaller than given value
    // void tfunc_Atree_LastLt();

    // Generate cursor related funcs and struct.
    // Pretty straightforward since Next is done without extra state.
    // void tfunc_Atree_curs();

    // -------------------------------------------------------------------
    // cpp/amc/base.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Base();

    // Generate a function to copy fields from a ctype to its base type
    // void tfunc_Base_CopyOut();
    // void tfunc_Base_CopyIn();
    // void tfunc_Base_Castbase();
    // void tfunc_Base_Castdown();
    double ChildRowid(double rowid);
    void CloneFields(amc::FCtype &from, amc::FCtype &to, double next_rowid, amc::FField &basefield);

    // -------------------------------------------------------------------
    // cpp/amc/bheap.cpp -- Binary heaps
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bheap();
    // void tfunc_Bheap_Upheap();
    // void tfunc_Bheap_Downheap();
    // void tfunc_Bheap_Reheap();
    // void tfunc_Bheap_ReheapFirst();
    // void tfunc_Bheap_Set();
    // void tfunc_Bheap_SetIfBetter();
    // void tfunc_Bheap_Cascdel();
    // void tfunc_Bheap_RemoveFirst();
    // void tfunc_Bheap_First();
    // void tfunc_Bheap_InBheapQ();
    // void tfunc_Bheap_Insert();
    // void tfunc_Bheap_Compact();
    // void tfunc_Bheap_Remove();
    // void tfunc_Bheap_N();
    // void tfunc_Bheap_EmptyQ();
    // void tfunc_Bheap_RemoveAll();
    // void tfunc_Bheap_Dealloc();
    // void tfunc_Bheap_Reserve();
    // void tfunc_Bheap_Init();
    // void tfunc_Bheap_Uninit();
    // void tfunc_Bheap_ElemLt();
    // void tfunc_Bheap_ElemLtval();
    // void tfunc_Bheap_unordcurs();
    // void tfunc_Bheap_curs();

    // -------------------------------------------------------------------
    // cpp/amc/bitfld.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitfld();
    // void tfunc_Bitfld_Init();
    // void tfunc_Bitfld_Get();
    // void tfunc_Bitfld_Set();

    // -------------------------------------------------------------------
    // cpp/amc/bitset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Bitset();
    // void tfunc_Bitset_N();
    // void tfunc_Bitset_qFind();
    // void tfunc_Bitset_NBits();
    // void tfunc_Bitset_qGetBit();
    // void tfunc_Bitset_GetBit();
    // void tfunc_Bitset_Sum1s();
    // void tfunc_Bitset_BitsEmptyQ();
    // void tfunc_Bitset_qClearBit();
    // void tfunc_Bitset_ClearBit();
    // void tfunc_Bitset_qSetBit();
    // void tfunc_Bitset_SetBit();
    // void tfunc_Bitset_qSetBitVal();
    // void tfunc_Bitset_qOrBitVal();
    // void tfunc_Bitset_ClearBitsAll();
    // void tfunc_Bitset_ClearBits();
    // void tfunc_Bitset_OrBits();
    // void tfunc_Bitset_ExpandBits();
    // void tfunc_Bitset_AllocBit();
    // void tfunc_Bitset_Sup();
    // void tfunc_Bitset_bitcurs();

    // -------------------------------------------------------------------
    // cpp/amc/blkpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Blkpool();
    // void tfunc_Blkpool_AllocMem();
    // void tfunc_Blkpool_SetBufferSize();
    // void tfunc_Blkpool_ReserveBuffers();
    // void tfunc_Blkpool_FreeMem();
    // void tfunc_Blkpool_Init();

    // -------------------------------------------------------------------
    // cpp/amc/cget.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_cget();

    // -------------------------------------------------------------------
    // cpp/amc/charset.cpp -- Charset functions
    //

    // Preprocess charsets
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_newfield_charset();
    // void tclass_Charset();
    // void tfunc_Charset_Match();

    // -------------------------------------------------------------------
    // cpp/amc/checkxref.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_check_xref();

    // -------------------------------------------------------------------
    // cpp/amc/cmp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cmp();
    // void tfunc_Cmp_Nextchar();
    // void tfunc_Cmp_Cmp();
    // void tfunc_Cmp_Lt();

    // -------------------------------------------------------------------
    // cpp/amc/concat.cpp
    //
    void GenerateSetForCoveredTrees(amc::FField& sourceField, u64 filledRange, int lastIndex);

    // -------------------------------------------------------------------
    // cpp/amc/copypriv.cpp
    //

    // Calculate value of copy_priv and return it.
    // This function can only be called once; addition of new fields after calling
    // this function is not supported.
    bool CopyPrivQ(amc::FCtype &ctype);

    // -------------------------------------------------------------------
    // cpp/amc/count.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Count();
    // void tfunc_Count_Insert();
    // void tfunc_Count_Remove();
    // void tfunc_Count_N();

    // -------------------------------------------------------------------
    // cpp/amc/cppfunc.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cppfunc();
    // void tfunc_Cppfunc_Get();
    // void tfunc_Cppfunc_Init();
    // void tfunc_Cppfunc_Set();

    // -------------------------------------------------------------------
    // cpp/amc/ctype/main.cpp -- Ctype code generators
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ctype();
    // void tfunc_Ctype_Uninit();
    // void tfunc_Ctype_XrefMaybe();
    // void tfunc_Ctype_Unref();
    // void tfunc_Ctype_Hash();
    // void tfunc_Ctype_Cmp();
    // void tfunc_Ctype_Lt();
    // void tfunc_Ctype_Init();
    // void tfunc_Ctype_Update();
    // void tfunc_Ctype_Min();
    // void tfunc_Ctype_Max();
    // void tfunc_Ctype_UpdateMin();
    // void tfunc_Ctype_UpdateMax();
    // void tfunc_Ctype_Eq();
    // void tfunc_Ctype_EqStrptr();
    // void tfunc_Ctype_PrintArgv();
    // void tfunc_Ctype_ToCmdline();

    // Used with command lines
    // void tfunc_Ctype_NArgs();
    // void tfunc_Ctype_Print();
    // void tfunc_Ctype_FmtJson();
    // void tfunc_Ctype_EqEnum();
    // void tfunc_Ctype_GetAnon();
    // void tfunc_Ctype_GetMsgLength();
    // void tfunc_Ctype_GetMsgMemptr();

    // -------------------------------------------------------------------
    // cpp/amc/ctype/print.cpp -- Print ctype to string
    //
    tempstr CheckDfltExpr(amc::FField &field, strptr text, bool canskip);
    void GenPrint(amc::FCtype &ctype, amc::FCfmt &cfmt);

    // -------------------------------------------------------------------
    // cpp/amc/ctype/read.cpp -- Read ctype from string
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ctype_ReadTupleMaybe();
    // void tfunc_Ctype_ReadStrptrMaybe();
    // void tfunc_Ctype_ReadFieldMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/delptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Delptr();
    // void tfunc_Delptr_Init();
    // void tfunc_Delptr_Uninit();
    // void tfunc_Delptr_Delete();
    // void tfunc_Delptr_Access();

    // -------------------------------------------------------------------
    // cpp/amc/dflt.cpp -- Field default
    //
    tempstr DfltExprVal(amc::FField &field);
    tempstr DfltExprBitfld(amc::FField &field);
    void Field_UpdateDflt(amc::FField &field);

    // -------------------------------------------------------------------
    // cpp/amc/disp/call.cpp
    //

    // Generator for call-type dispatch (where we invoke a user function
    // for each case)
    void Disp_Call(amc::FDispatch& dispatch);

    // -------------------------------------------------------------------
    // cpp/amc/disp/casetype.cpp
    //

    // Determine common header for all messages in the dispatch
    // Compute values of P_CTYPE_HDR and P_CASETYPE.
    // P_CTYPE_HDR is non-NULL if all of the messages in the dispatch share the same message
    // header.
    // P_CASETYPE is always non-NULL. It is set to the type field of the common message
    // header, if one exists, or to a newly created enum.
    void Disp_CreateCasetype(amc::FDispatch &dispatch);

    // -------------------------------------------------------------------
    // cpp/amc/disp/filter.cpp -- Dispatch filter
    //

    // Create new fields for dispatch filters.
    void Filter_NewField();
    void Filter_Gen(amc::FNs &ns);

    // -------------------------------------------------------------------
    // cpp/amc/disp/main.cpp -- Dispatch
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_dispatch();

    // Create new fields for dispatch filters.
    void Disp_NewField();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_dispenum();

    // -------------------------------------------------------------------
    // cpp/amc/disp/msg.cpp
    //

    // Create a new dispatch collecting all messages
    // using a given message header (e.g. find all xyz messages
    // and create a dispatch called xyz::MsgHeaderMsgs, that
    // can be used to both read and print xyz message types
    void Disp_CreateFromMsg();

    // -------------------------------------------------------------------
    // cpp/amc/disp/print.cpp
    //

    // Generate Dispatch_Print function
    void Disp_Print(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/disp/read.cpp
    //
    amc::FCfmt *FindStringRead(amc::FCtype &ctype);

    // Generate Dispatch_Read function
    void Disp_Read(amc::FDispatch &disp);

    // -------------------------------------------------------------------
    // cpp/amc/enum.cpp
    //

    // emit constants
    // - c++ enums (fconst for integers)
    // - extern strings (fconst for non-integers)
    void Main_GenEnum(amc::FNs& ns, amc::FCtype &ctype);
    tempstr Enumtype(amc::FField &field);

    // -------------------------------------------------------------------
    // cpp/amc/exec.cpp -- reftype Exec
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Exec();
    // void tfunc_Exec_Init();
    // void tfunc_Exec_Uninit();
    // void tfunc_Exec_Wait();
    // void tfunc_Exec_Kill();
    // void tfunc_Exec_Start();

    // // Should this be generated?
    // static algo::Fildes bash_StartRead(command::bash_proc &bash, algo_lib::FFildes &read) {
    // int pipefd[2];
    // int rc=pipe(pipefd);
    // (void)rc;
    // read.fd.value = pipefd[0];
    // bash.fstdout  << ">&" << pipefd[1];
    // bash_Start(bash);
    // (void)close(pipefd[1]);
    // return read.fd;
    // }
    // void tfunc_Exec_StartRead();
    // void tfunc_Exec_Exec();
    // void tfunc_Exec_ExecX();
    // void tfunc_Exec_ToCmdline();
    // void tfunc_Exec_Execv();
    void NewFieldExec();

    // -------------------------------------------------------------------
    // cpp/amc/fbuf.cpp -- Byte buffer
    //

    //
    // see tex/amc/amc_fbuf.tex for documentation
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fbuf();
    // void tfunc_Fbuf_BeginRead();
    // void tfunc_Fbuf_GetMsg();
    // void tfunc_Fbuf_Init();
    // void tfunc_Fbuf_Max();
    // void tfunc_Fbuf_N();
    // void tfunc_Fbuf_Refill();
    // void tfunc_Fbuf_RemoveAll();
    // void tfunc_Fbuf_Scanmsg();
    // void tfunc_Fbuf_Shift();
    // void tfunc_Fbuf_SkipBytes();
    // void tfunc_Fbuf_SkipMsg();
    // void tfunc_Fbuf_WriteAll();
    // void tfunc_Fbuf_WriteMsg();
    // void tfunc_Fbuf_BeginWrite();
    // void tfunc_Fbuf_Outflow();
    // void tfunc_Fbuf_EndRead();
    // void tfunc_Fbuf_EndWrite();
    // void tfunc_Fbuf_Realloc();
    // void tfunc_Fbuf_Uninit();

    // -------------------------------------------------------------------
    // cpp/amc/fcast.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fcast();
    // void tfunc_Fcast_Cast();

    // -------------------------------------------------------------------
    // cpp/amc/fcmap.cpp -- Fcmap generation
    //

    // Get the conversion function if it exists
    amc::FFunc* ConvertFuncMaybe(amc::FCtype& from, amc::FCtype& to);

    // TODO::warn about the following case:
    // A.b = A.c, A.d = A.e. That setup yields only one function A_FromA() instead of 2 (and a compile error).
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_fcmap();

    // -------------------------------------------------------------------
    // cpp/amc/fconst.cpp -- Constants, string <-> int conversion
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Fconst();
    // void tfunc_Fconst_GetEnum();
    // void tfunc_Fconst_SetEnum();
    // void tfunc_Fconst_ToCstr();
    // void tfunc_Fconst_Print();
    // void tfunc_Fconst_SetStrptrMaybe();
    // void tfunc_Fconst_SetStrptr();
    // void tfunc_Fconst_ReadStrptrMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/fdec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Dec();
    // void tfunc_Dec_GetDouble();
    // void tfunc_Dec_GetInt();
    // void tfunc_Dec_GetScale();
    // void tfunc_Dec_qSetDouble();
    // void tfunc_Dec_SetDoubleMaybe();
    // void tfunc_Dec_ReadStrptrMaybe();
    // void tfunc_Dec_Print();

    // -------------------------------------------------------------------
    // cpp/amc/field.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Field();
    // void tclass_Field2();
    // void tfunc_Field_Cleanup();
    // void tfunc_Field_Userinit();
    // void tfunc_Field_Cascdel();
    // void tfunc_Field2_ReadStrptrMaybe();
    // void tfunc_Field_Concat();

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

    // The declaration for retval is emitted when the initializer becomes available.
    // This allows initialization of references (i.e. X &retval = <expr>)
    // and also results in shorter code.
    void GenRetvalInit(amc::FFunc &func, amc::Funcarg &funcarg, strptr initializer);

    // Declare return value for function FUNC.
    // The value has type TYPE, name NAME< and is initialized with INITALIZER.
    //
    amc::Funcarg* AddRetval(amc::FFunc &func, strptr type, strptr name, strptr initializer);

    // Add a type argument to function FUNC,
    // TYPE is a c++ expression.
    // Function becomes a template.
    amc::Funcarg* AddTypearg(amc::FFunc &func, strptr type);
    void SetPresent(amc::FFunc &func, strptr ref, amc::FField &field);
    amc::FFunc &CreateCurFunc(bool proto);
    amc::FFunc &CreateCurFunc();

    // Return non-null pointer to ctype's init function.
    amc::FFunc *init_GetOrCreate(amc::FCtype &ctype);

    // Emit function prototype into string OUT.
    // If ctype_context is specified, then the declaration is intended to be
    // used inside the struct, so different C++ syntax rules apply.
    void PrintFuncProto(amc::FFunc& func, amc::FCtype *ctype_context, cstring &out);
    void PrintFuncBody(amc::FNs& ns, amc::FFunc& func);
    bool SetRetType(amc::FFunc &func, amc::FCtype &ctype);
    void MaybeUnused(amc::FFunc &func, strptr name);
    amc::Funcarg* FindArg(amc::FFunc &func, strptr name);

    // -------------------------------------------------------------------
    // cpp/amc/fwddecl.cpp
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
    // void gen_ns_fwddecl2();

    // emit forward-declarations of steps
    // void gen_ns_fwddecl();

    // -------------------------------------------------------------------
    // cpp/amc/gen.cpp
    //
    amc::FField *InsField(const dmmeta::Field &field);

    // Move FIELD so that it appears immediately after field AFTER.
    void PlaceFieldAfter(amc::FField &field, amc::FField &after);
    bool HasAllocQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_usedns();
    // void gen_include();
    // void gen_ns_check_main();
    // void gen_check_cascdel();
    // void gen_countxref();

    // Check that a struct marked cheap_copy does not contain structs not marked that way
    // void gen_check_cheapcopy();
    // void gen_check_prefix();
    // void gen_basepool();

    // TODO: also check that targdep path exists between namespaces?
    // void gen_check_basepool();
    // void gen_check_bitfld();

    // Rewrite reftype:Pkey fields into reftype:Val
    // void gen_lookuppkey();

    // Ensure fregx record exists for each field of type Regx or RegxSql
    // Rewrite RegxSql fields as Regx, regxtype Sql
    // (generalization of original RegxSql reftype)
    // void gen_rewrite_regx();
    // void gen_check_reftype();
    // void gen_detectinst();
    // void gen_check_cpptype();

    // Count # fields, excluding substrings, cppfuncs, and bitfields
    int NValFields(amc::FCtype &ctype);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_field();
    // void gen_check_bigend();
    // void gen_xref_parent();
    // void gen_datafld();
    // void gen_ctype_toposort();
    tempstr Argtype(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_ctype();
    // void gen_copypriv();
    // void gen_pmask();
    // void gen_xref2();
    // void gen_select_ns();

    //
    // Generate Fconst from a column of some table.
    //
    //
    // void gen_gconst();

    //
    // Generate Fconst from a column of some table.
    //
    // void gen_bitfldenum();
    // void gen_prep_fconst();

    // Generate numeric fconsts from string fconsts
    // void gen_clonefconst();
    // void gen_newfield_exec();
    // void gen_newfield_count();

    // convert cbase records into fields of reftype Base
    // void gen_newfield_cbase();

    // convert cbase records into fields of reftype Base
    // void gen_check_basefield();
    // void gen_newfield_sortfld();
    // void gen_newfield_ptrary();
    // void gen_newfield_dispatch();
    // void gen_newfield_cfmt();
    // void gen_ns_check_pack();
    // void gen_ns_check_path();
    // void gen_ns_pkeytypedef();
    // void gen_ns_enums();
    // void gen_ns_field();
    // void gen_ns_include();
    // void gen_ns_funcindex();

    // Print static function prototype declarations
    // to the cpp file
    // void gen_ns_print_proto();
    // void gen_ns_print_struct();
    // void gen_ns_curstext();
    // void gen_ns_pnew();
    // void gen_ns_func();
    // void gen_ns_operators();
    // void gen_ns_check_lim();
    // void gen_proc();
    // void gen_check_fcurs();
    // void gen_check_varlen();

    // -------------------------------------------------------------------
    // cpp/amc/global.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Global();
    // void tfunc_Global_Init();
    // void tfunc_Global_LoadTuplesMaybe();
    // void tfunc_Global_SaveTuples();
    // void tfunc_Global_InsertStrptrMaybe();
    // void tfunc_Global_InitReflection();
    // void tfunc_Global_LoadSsimfileMaybe();
    // void tfunc_Global_main();
    // void tfunc_Global_WinMain();
    // void tfunc_Global_MainLoop();
    // void tfunc_Global_Steps();
    // void tfunc_Global_Step();
    // void tfunc_Global_Main();
    // void tfunc_Global_StaticCheck();

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
    // void tfunc_Global_ReadArgv();

    // -------------------------------------------------------------------
    // cpp/amc/gsymbol.cpp -- Generate strings from tables
    //

    // Generate regular c++ symbols from tables
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_gsymbol();

    // -------------------------------------------------------------------
    // cpp/amc/hook.cpp
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
    // void gen_newfield_hook();
    // void tclass_Hook();
    // void tfunc_Hook_Set0();
    // void tfunc_Hook_Set1();
    // void tfunc_Hook_Set2();

    // Implement _Call function for the hook
    // void tfunc_Hook_Call();

    // -------------------------------------------------------------------
    // cpp/amc/include.cpp
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
    // cpp/amc/inlary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Inlary();
    // void tfunc_Inlary_AllocMem();
    // void tfunc_Inlary_EmptyQ();
    // void tfunc_Inlary_Fill();
    // void tfunc_Inlary_Find();
    // void tfunc_Inlary_Getary();
    // void tfunc_Inlary_Init();
    // void tfunc_Inlary_Eq();
    // void tfunc_Inlary_Cmp();
    // void tfunc_Inlary_Max();
    // void tfunc_Inlary_N();
    // void tfunc_Inlary_RemoveAll();
    // void tfunc_Inlary_RemoveLast();
    // void tfunc_Inlary_RowidFind();
    // void tfunc_Inlary_Setary();
    // void tfunc_Inlary_Uninit();
    // void tfunc_Inlary_qFind();
    // void tfunc_Inlary_rowid_Get();
    // void tfunc_Inlary_curs();
    // void tfunc_Inlary_Print();
    // void tfunc_Inlary_ReadStrptrMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/io.cpp -- I/O functions
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Io_InputMaybe();
    // void tfunc_Io_Input();
    // void tfunc_Io_SaveSsimfile();
    // void tclass_Io();

    // Generate a name for the user-defined function based on the specified hook
    // I.e. amc::tclass_Io (hook name 'step' -- legacy -- all hooks before this feature
    // were called 'step' and function name didn't depend on hook name)
    // or amc::tclass_Io_newfield (hook name 'newfield')
    tempstr StaticHookName(amc::FField &field, strptr suffix);
    tempstr StaticHookPkey(amc::FField &field, strptr suffix);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Io_LoadStatic();

    // -------------------------------------------------------------------
    // cpp/amc/json.cpp
    //

    // Generate a function to convert ctype to Json
    // Called from amc::tfunc_Ctype_FmtJson
    void GenFmtJson(amc::FCtype &ctype);

    // -------------------------------------------------------------------
    // cpp/amc/lary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Lary();
    // void tfunc_Lary_AllocMem();
    // void tfunc_Lary_EmptyQ();
    // void tfunc_Lary_Find();
    // void tfunc_Lary_Init();
    // void tfunc_Lary_Last();
    // void tfunc_Lary_N();
    // void tfunc_Lary_RemoveAll();
    // void tfunc_Lary_RemoveLast();
    // void tfunc_Lary_RowidFind();
    // void tfunc_Lary_Uninit();
    // void tfunc_Lary_qFind();
    // void tfunc_Lary_qLast();
    // void tfunc_Lary_curs();

    // -------------------------------------------------------------------
    // cpp/amc/llist.cpp -- Linked lists
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Llist();
    // void tfunc_Llist_Cascdel();
    // void tfunc_Llist_DestructiveFirst();
    // void tfunc_Llist_EmptyQ();
    // void tfunc_Llist_First();
    // void tfunc_Llist_InLlistQ();
    // void tfunc_Llist_Init();
    // void tfunc_Llist_Last();
    // void tfunc_Llist_N();
    // void tfunc_Llist_Next();
    // void tfunc_Llist_Prev();
    // void tfunc_Llist_Remove();
    // void tfunc_Llist_RemoveAll();
    // void tfunc_Llist_RemoveFirst();
    // void tfunc_Llist_RotateFirst();
    // void tfunc_Llist_Insert();
    // void tfunc_Llist_qLast();
    // void tclass_ZSListMT();
    // void tfunc_ZSListMT_DestructiveFirst();
    // void tfunc_ZSListMT_RemoveFirst();
    // void tfunc_ZSListMT_InLlistQ();
    // void tfunc_ZSListMT_Insert();
    // void tfunc_ZSListMT_Remove();
    // void tfunc_ZSListMT_Init();

    // Generate cursor for llist
    void Llist_curs(bool needdel);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Llist_curs();
    // void tfunc_Llist_delcurs();

    // -------------------------------------------------------------------
    // cpp/amc/lpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Lpool();
    // void tfunc_Lpool_FreeMem();
    // void tfunc_Lpool_AllocMem();
    // void tfunc_Lpool_ReserveBuffers();
    // void tfunc_Lpool_ReallocMem();
    // void tfunc_Lpool_Init();
    // void tfunc_Lpool_N();

    // -------------------------------------------------------------------
    // cpp/amc/main.cpp -- Main driver
    //
    bool PadQ(amc::FField &field);
    bool CanDeleteQ(amc::FCtype &ctype);
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
    tempstr Initcast(amc::FField &field);

    // True if ctype is instantiated through a memory pool that has an Alloc function.
    bool PoolHasAllocQ(amc::FCtype &ctype);
    void GenPrintStmt(cstring &out, amc::FCtype &parenttype, amc::FField &field, strptr strname, strptr parentname);

    // print binary octet string as hex byte array initializer
    void memptr_PrintOctetsHexArray(algo::memptr ary, cstring &out, bool caps);
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
    tempstr EvalRowid(amc::FCtype &ctype);

    // True if given ctype is a global, i.e. it has only a single instance
    // of type Global.
    bool GlobalQ(amc::FCtype &ctype);

    // Pick a name with which to refer to a record of type CTYPE
    tempstr Refname(amc::FCtype &ctype);

    // Returns TRUE if the field is an inline value, except for Varlen and Opt
    bool ValQ(amc::FField &field);
    bool ComputedFieldQ(amc::FField &field);

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
    // The parent ctype is CTYPE -- it may not necessarily be a direct parent of FIELD.
    // if CTYPE is NULL, it is assumed to be field.p_ctype
    tempstr FieldvalExpr(amc::FCtype *ctype, amc::FField &field, strptr name);

    // Return C++ expression computing total length of ctype CTYPE
    // accessible via name NAME.
    tempstr LengthExpr(amc::FCtype &ctype, strptr name);

    // Return C++ expression string assigning value VALUE to field FIELD
    // given parent reference PARNAME.
    // If NEEDS_CAST is set, a cast is added to the target type
    tempstr AssignExpr(amc::FField &field, strptr parname, strptr value, bool needs_cast);

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
    // if FROM is a known c++ keyword, prepend '_' to it.
    // If FROM is an empty string, use "_default"
    // Replace /, -, ., etc. with underscores.
    // Example:
    // strptr_PrintCppIdent("abcd")   -> abcd
    // strptr_PrintCppIdent("ab.cd")  -> ab_cd
    // strptr_PrintCppIdent("int")    -> _int
    // strptr_PrintCppIdent("")       -> _default
    void strptr_PrintCppIdent(strptr from, cstring &out);

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
    tempstr ByvalArgtype(amc::FCtype &ctype);
    void Main_CloneFconst_Field(amc::FField &field);

    // HOW IS THIS DIFFERENT FROM INSFIELD?
    void Field_AddChild(amc::FField &parent, const dmmeta::Field &field);
    bool NeedSetQ(amc::FField &field);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Cppstack();
    // void tclass_Pkey();
    int c_anonfld_N(amc::FCtype &ctype);

    // Go over `amcdb.gen` table and invoke each global generator
    // Then, go over namespaces
    // Go over per-namespace `amcdb.gen` entries and invoke them.
    // Collect performance data long the way
    // This is where amc spends most of the time. See the `amcdb.gen` table for more information.
    void Main_Gen();

    // OUTPUT        Generate files for the namespace(s)
    void Main();

    // -------------------------------------------------------------------
    // cpp/amc/malloc.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Malloc();
    // void tfunc_Malloc_AllocMem();
    // void tfunc_Malloc_ReallocMem();
    // void tfunc_Malloc_FreeMem();

    // -------------------------------------------------------------------
    // cpp/amc/msgcurs.cpp -- Message scanning cursor
    //

    // Generate a message-scanning cursor for each message header with length
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_msgcurs();

    // -------------------------------------------------------------------
    // cpp/amc/nstype.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ssimdb();
    // void gen_ns_check_nstype();

    // -------------------------------------------------------------------
    // cpp/amc/numstr.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Numstr();
    // void tfunc_Numstr_qGetnum();
    // void tfunc_Numstr_GetnumDflt();
    // void tfunc_Numstr_Geti64();
    // void tfunc_Numstr_SetnumMaybe();
    // void tfunc_Numstr_ForAllStrings();
    // void gen_parsenum();

    // -------------------------------------------------------------------
    // cpp/amc/opt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Opt();
    // void tfunc_Opt_Get();
    // void tfunc_Opt_Getary();
    // void tfunc_Opt_Print();
    // void tfunc_Opt_ReadStrptrMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/outfile.cpp
    //

    // Write output file to disk
    // and deallocate memory associated with it
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_write();

    // Create outfile record for specified filename
    // T here is one outfile per generated output file
    amc::FOutfile &outfile_Create(strptr filename);

    // -------------------------------------------------------------------
    // cpp/amc/pmask.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Pmask();
    // void tfunc_Pmask_PresentQ();
    // void tfunc_Pmask_SetPresent();

    // -------------------------------------------------------------------
    // cpp/amc/pnew.cpp -- Custom constructors
    //

    // Generate various constructors
    void GenPnew(amc::FNs& ns, amc::FPnew& pnew, amc::FCtype& ctype);

    // -------------------------------------------------------------------
    // cpp/amc/pool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Pool();
    // void tfunc_Pool_AllocMaybe();
    // void tfunc_Pool_Alloc();

    // Generate Alloc function that takes 2 additional arguments
    // -- pointer to extra (varlen portion) memory to tack onto the record,
    // and number of bytes
    // void tfunc_Pool_AllocExtraMaybe();

    // Like AllocExtraMaybe, but die on out-of-memory
    // void tfunc_Pool_AllocExtra();

    // VarlenMaybe -- array of fixed-size structs tacked onto the end of a struct
    // void tfunc_Pool_AllocVarlenMaybe();

    // Like VarlenMaybe, but die on out-of-memory
    // void tfunc_Pool_AllocVarlen();
    // void tfunc_Pool_InsertMaybe();
    amc::FField *FindFieldByName(amc::FCtype &ctype, algo::strptr name);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Pool_UpdateMaybe();
    // void tfunc_Pool_Delete();

    // Ensure that all instances of CTYPE's record are varlen pools
    void EnsureVarlenPool(amc::FCtype &ctype);

    // Find pool used for allocating values of type CTYPE
    amc::FField *FindPool(amc::FCtype &ctype);

    // Find default used for allocating things in namespace NS
    amc::FField *DefaultPool(amc::FNs &ns);

    // -------------------------------------------------------------------
    // cpp/amc/protocol.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Protocol();
    // void tfunc_Protocol_StaticCheck();
    // void gen_prep_proto();

    // -------------------------------------------------------------------
    // cpp/amc/ptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ptr();
    // void tfunc_Ptr_Init();
    // void tfunc_Ptr_InsertMaybe();
    // void tfunc_Ptr_Remove();
    // void tfunc_Ptr_Cascdel();
    // void tfunc_Ptr_Print();

    // -------------------------------------------------------------------
    // cpp/amc/ptrary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Ptrary();
    // void tfunc_Ptrary_Cascdel();
    // void tfunc_Ptrary_EmptyQ();
    // void tfunc_Ptrary_Find();
    // void tfunc_Ptrary_Getary();
    // void tfunc_Ptrary_Init();
    // void tfunc_Ptrary_Insert();
    // void tfunc_Ptrary_InsertMaybe();
    // void tfunc_Ptrary_ScanInsertMaybe();
    // void tfunc_Ptrary_N();
    // void tfunc_Ptrary_Remove();
    // void tfunc_Ptrary_RemoveAll();
    // void tfunc_Ptrary_Reserve();
    // void tfunc_Ptrary_Uninit();
    void Ptrary_curs(bool once);
    //     (user-implemented function, prototype is in amc-generated header)
    // void tfunc_Ptrary_curs();
    // void tfunc_Ptrary_oncecurs();

    // -------------------------------------------------------------------
    // cpp/amc/query.cpp
    //
    void Main_Querymode();
    tempstr Query_GetKey();
    tempstr Query_GetValue();

    // Parse query argument, return regex of namespaces
    tempstr Query_GetNs();

    // True if amc was invoked in query-only mode
    bool QueryModeQ();

    // -------------------------------------------------------------------
    // cpp/amc/regx.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_RegxSql();
    // void tfunc_RegxSql_ReadStrptrMaybe();
    // void tfunc_RegxSql_Print();
    // void tfunc_RegxSql_Init();
    // void tclass_Regx();
    // void tfunc_Regx_ReadStrptrMaybe();
    // void tfunc_Regx_Print();
    // void tfunc_Regx_Init();

    // -------------------------------------------------------------------
    // cpp/amc/sbrk.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Sbrk();
    // void tfunc_Sbrk_AllocMem();
    // void tfunc_Sbrk_FreeMem();
    // void tfunc_Sbrk_Init();

    // -------------------------------------------------------------------
    // cpp/amc/signature.cpp
    //

    // Recursive function to compute all ctype dependencies
    // (transitive/reflexive closure for ctype on fields)
    // Global purpose is to detect the change in deep bottom type
    // which impacts binary footprint of the top type
    void SignatureVisit(amc::FCtype &ctype);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_prep_signature();

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_compute_size();

    // Generate symbol corresponding to the size of ctype CTYPE.
    // SizeEnums are generated only for occasional ctypes, and are used
    // to avoid circular dependencies in headers
    tempstr SizeEnum(amc::FCtype &ctype);

    // Generate compile-time constants for sizes of certain ctypes
    // accessed from namespace NS, without having to include corresponding header.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_size_enums();

    // -------------------------------------------------------------------
    // cpp/amc/smallstr.cpp -- Small strings
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Smallstr();
    // void tfunc_Smallstr_Init();
    // void tfunc_Smallstr_Getary();
    // void tfunc_Smallstr_Add();
    // void tfunc_Smallstr_AddStrptr();
    // void tfunc_Smallstr_ReadStrptrMaybe();
    // void tfunc_Smallstr_Print();
    // void tfunc_Smallstr_HashStrptr();

    // compute length
    // void tfunc_Smallstr_N();

    // Max # of elements (constant)
    // void tfunc_Smallstr_Max();

    // Set value as strptr
    // void tfunc_Smallstr_SetStrptr();

    // Copy from same type
    // void tfunc_Smallstr_Set();

    // Assignment operator from strptr
    // void tfunc_Smallstr_AssignStrptr();

    // Construct from same type
    // void tfunc_Smallstr_Ctor();

    // Construct from strptr
    // void tfunc_Smallstr_CtorStrptr();

    // -------------------------------------------------------------------
    // cpp/amc/sort.cpp -- Comparison & Sorting routines
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Sort();
    // void tfunc_Sort_Swap();
    // void tfunc_Sort_Rotleft();
    // void tfunc_Sort_Lt();
    // void tfunc_Sort_SortedQ();
    // void tfunc_Sort_IntInsertionSort();
    // void tfunc_Sort_IntHeapSort();
    // void tfunc_Sort_IntQuickSort();
    // void tfunc_Sort_InsertionSort();
    // void tfunc_Sort_HeapSort();
    // void tfunc_Sort_QuickSort();

    // -------------------------------------------------------------------
    // cpp/amc/step.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Step();
    // void tfunc_Step_UpdateCycles();
    // void tfunc_Step_Step();
    // void tfunc_Step_Init();
    // void tfunc_Step_Call();
    // void tfunc_Step_FirstChanged();
    // void tfunc_Step_SetDelay();

    // -------------------------------------------------------------------
    // cpp/amc/struct.cpp
    //
    void GenStruct(amc::FNs& ns, amc::FCtype& ctype);

    // -------------------------------------------------------------------
    // cpp/amc/substr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Substr();
    // void tfunc_Substr_Get();
    // void tfunc_Substr_Get2();

    // -------------------------------------------------------------------
    // cpp/amc/tableid.cpp -- Per-namespace enum of tables
    //
    bool HasFinputQ(amc::FCtype &ctype);
    void GenTableId(amc::FNs &ns);

    // create TableId type.
    // generate an enum representing tables in the in-memory database
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_tableenum();
    void GenFieldId(amc::FNs &ns);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_fieldid();

    // -------------------------------------------------------------------
    // cpp/amc/tary.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Tary();
    // void tfunc_Tary_Addary();
    // void tfunc_Tary_Alloc();
    // void tfunc_Tary_AllocAt();
    // void tfunc_Tary_AllocN();
    // void tfunc_Tary_AllocNVal();
    // void tfunc_Tary_EmptyQ();
    // void tfunc_Tary_Find();
    // void tfunc_Tary_Getary();
    // void tfunc_Tary_Init();
    // void tfunc_Tary_Last();
    // void tfunc_Tary_Max();
    // void tfunc_Tary_N();
    // void tfunc_Tary_Remove();
    // void tfunc_Tary_RemoveAll();
    // void tfunc_Tary_RemoveLast();
    // void tfunc_Tary_AbsReserve();
    // void tfunc_Tary_Reserve();
    // void tfunc_Tary_RowidFind();
    // void tfunc_Tary_Setary();
    // void tfunc_Tary_Setary2();
    // void tfunc_Tary_Uninit();
    // void tfunc_Tary_qFind();
    // void tfunc_Tary_qLast();
    // void tfunc_Tary_rowid_Get();
    // void tfunc_Tary_Eq();
    // void tfunc_Tary_Cmp();
    // void tfunc_Tary_curs();
    // void tfunc_Tary_ReadStrptrMaybe();

    // -------------------------------------------------------------------
    // cpp/amc/tclass.cpp
    //

    // Call tclass, tfunc, and cursor generators for this template
    // Context is provided via _db.genfield:
    // - genfield.p_field   --- field pointer (NULL for tclass, cursor, and Ctype, set for individual fields)
    // - genfield.p_ctype   --- current ctype, never NULL
    // - genfield.p_tfunc   --- pointer to tfunc, never NULL
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
    void GenTclass_Fields(amc::FCtype &ctype);

    // Call tfunc generators without field context (Ctype generators)
    // This must be called after field-specific generators, since by this time
    // ctype sizes have been computed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_ns_ctype();

    // -------------------------------------------------------------------
    // cpp/amc/thash.cpp -- Hash tables
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Thash();
    // void tfunc_Thash_Find();
    // void tfunc_Thash_FindX();
    // void tfunc_Thash_Reserve();
    // void tfunc_Thash_GetOrCreate();
    // void tfunc_Thash_N();
    // void tfunc_Thash_EmptyQ();
    // void tfunc_Thash_InsertMaybe();
    // void tfunc_Thash_Cascdel();
    // void tfunc_Thash_Remove();
    // void tfunc_Thash_FindRemove();
    // void tfunc_Thash_Init();
    // void tfunc_Thash_Uninit();
    // void tfunc_Thash_curs();

    // -------------------------------------------------------------------
    // cpp/amc/tpool.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Tpool();
    // void tfunc_Tpool_ReserveMem();
    // void tfunc_Tpool_Reserve();
    // void tfunc_Tpool_AllocMem();
    // void tfunc_Tpool_FreeMem();
    // void tfunc_Tpool_Init();

    // -------------------------------------------------------------------
    // cpp/amc/trace.cpp -- Generate trace code
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gen_trace();

    // -------------------------------------------------------------------
    // cpp/amc/upptr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Upptr();
    // void tfunc_Upptr_Init();

    // -------------------------------------------------------------------
    // cpp/amc/val.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Val();
    // void tfunc_Val_Get();
    // void tfunc_Val_Set();
    // void tfunc_Val_Init();
    // void tfunc_Val_RowidFind();
    // void tfunc_Val_N();

    // -------------------------------------------------------------------
    // cpp/amc/varlen.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void tclass_Varlen();
    // void tfunc_Varlen_Addr();
    // void tfunc_Varlen_Getary();
    // void tfunc_Varlen_N();
    // void tfunc_Varlen_ReadStrptrMaybe();
    // void tfunc_Varlen_curs();
}
