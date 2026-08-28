// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Exceptions: yes
// Source: cpp/amc/pool.cpp -- Generic pool functions
//
// #AL# TODO: Pool.Delete: HANDLE NOREMOVE
// Problem is with insertion code, which is forced to remove
// last element when xref fails, it needs the Delete call

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Check if we need to generate a variable-length alloc function
// This is true if the arg is a varlen type
// Example:
//   dmmeta.field  field:some_exe.FDb.symbol  arg:some_exe.FTable  reftype:Lpool  dflt:""  comment:""
// This sfield NeedAllocExtraQ
// because some_exe.FTable has an Opt field
static bool NeedAllocExtraQ(amc::FField &field) {
    amc::FCtype &ctype = *field.p_arg;
    return ctype.c_lenfld && amc::RuntimeFrameLenQ(ctype);
}

// -----------------------------------------------------------------------------

void amc::tclass_Pool() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;

    amc::FCtype& fldtype = *field.p_arg;
    // TODO remove this restricion
    vrfy(zd_varlenfld_N(fldtype)<=1, tempstr()
         <<Keyval("field",field.field)
         <<Keyval("arg",field.arg)
         <<Keyval("comment","Pool arg having more than one varlen field is currently unsupported"));

    if (zd_varlenfld_First(fldtype)) {
        Set(R, "$vartype", zd_varlenfld_First(fldtype)->cpp_type);
        Set(R, "$varfld", name_Get(*zd_varlenfld_First(fldtype)));
    }
    if (NeedAllocExtraQ(field) && !field.p_reftype->varlen) {
        prerr("amc.one_size_doesnt_fit_all"
              <<Keyval("field",field.field)
              <<Keyval("reftype",field.reftype)
              <<Keyval("comment","This pool doesn't support Varlen records. Please use a varlen pool (Blkpool,Lpool,Malloc,Sbrk)"));
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

// Return the C++ boolean expression accepting NBYTEXTRA, the caller-chosen
// byte count that the pool tacks onto its row's fixed size ($Cpptype), and
// append to COMMENT the reason for each term emitted.
//
// The length field stores (size + extra) / scale (the $setlen store in
// GenAllocFunc). gen_check_lenfld guarantees the fixed size divides evenly
// (except for a cextern ctype, whose size amc does not know and cannot
// check), but the addon comes on top of it: a byte count that is not a
// scale multiple has no representable length word, and the truncated store
// would make the reader reconstruct less than was allocated. A negative
// count is never valid either -- a caller can arrive at one (InsertMaybe
// subtracts the fixed size from a corrupt length word), it underallocates
// the fixed portion, and the extra-bytes memcpy converts it to a huge
// size_t.
//
// The count also carries an upper bound, always. A total beyond the length
// word's storable range wraps mod 2^N through the word's own type, and a
// total past i32 max is read back negative whatever the word holds, since
// every frame length reaches its reader through an i32 (LengthExpr) and the
// allocation takes the count in one. The bound is therefore the smaller of
// the two ranges with the fixed portion already spoken for, written as a
// compile-time ssizeof expression like the sibling emitters: the numeric
// size is not computed at this stage (and a cextern arg never has one amc
// could fold).
//
// Both the allocation (AllocExtraMaybe, which returns NULL on an
// unrepresentable count) and the caller that reaches it through the
// die-on-fail AllocExtra (InsertMaybe, which must reject first) are written
// from this one expression, so the two state one accept set. The comptest
// amc.PoolInsertScale prints both functions for every pool of its fixture,
// and a term emitted into one and not the other shows up as a diff there.
static tempstr AllocExtraOkExpr(algo_lib::Replscope &R, amc::FLenfld *lenfld, strptr nbytextra, cstring &comment) {
    u64 maxlen = lenfld ? amc::LenfldMaxLen(*lenfld) : u64(0xffffffffffffffff);
    tempstr cond;
    cond << nbytextra << " >= 0";
    comment << "a negative count would underallocate the fixed portion";
    if (lenfld && lenfld->scale > 1) {
        Set(R, "$scale", tempstr() << lenfld->scale);
        cond << Subst(R, tempstr() << " && " << nbytextra << " % $scale == 0");
        comment << "; only a multiple of scale is representable in the length field";
    }
    if (maxlen <= amc::FrameLenMax()) {
        cond << Subst(R, tempstr() << " && " << nbytextra << " <= " << maxlen << " - ssizeof($Cpptype)");
        comment << "; larger totals do not fit the length field";
    } else {
        tempstr framemax;
        algo::u64_PrintHex(amc::FrameLenMax(), framemax, 1, true, false);
        cond << Subst(R, tempstr() << " && " << nbytextra << " <= " << framemax << " - ssizeof($Cpptype)");
        comment << "; a larger total does not fit the i32 a frame length is read back through";
    }
    return cond;
}

// -----------------------------------------------------------------------------

static void GenAllocFunc(algo_lib::Replscope &R, amc::FFunc &func, amc::FField &field) {
    bool poolvarlen = PoolVarlenQ(field);
    amc::FLenfld *lenfld = (*field.p_arg).c_lenfld;
    tempstr alloccall(Subst(R,"($Cpptype*)$name_AllocMem($pararg)"));
    amc::AddArg(alloccall, Subst(R,"$poolvarlen"), poolvarlen);
    bool checkextra = FindArg(func,"nbyte_extra") != NULL;
    if (checkextra) {
        // an unrepresentable size fails the allocation (NULL) like any
        // other alloc failure, before any memory is taken
        tempstr condcomment;
        tempstr cond = AllocExtraOkExpr(R, lenfld, "nbyte_extra", condcomment);
        amc::AddRetval(func, Subst(R,"$Cpptype*"), "row", "NULL");
        func.body << "if (" << cond << ") { // " << condcomment << eol;
        func.body << "    row = " << alloccall << ";" << eol;
        Ins(&R, func.body, "}");
    } else {
        amc::AddRetval(func, Subst(R,"$Cpptype*"), "row", alloccall);
    }
    Ins(&R, func.body , "if (row) {");
    Ins(&R, func.body , "    new (row) $Cpptype; // call constructor");
    if (BlkpoolQ(field)) {
        Ins(&R, func.body , "    row->$name_buf = $parname.$name_cur;");
    }
    // initialize rowid
    ind_beg(amc::ctype_c_field_curs, rowidfield, *field.p_arg) if (rowidfield.c_rowid) {
        Set(R, "$setrowid", AssignExpr(rowidfield, "*row", "$name_N($pararg) - 1", true));
        Ins(&R, func.body, "    $setrowid;");
    }ind_end;
    // initialize length field with the store formula (LenfldStoreExpr)
    if (lenfld) {
        Set(R, "$setlen", AssignExpr(*lenfld->p_field, "*row", LenfldStoreExpr(*lenfld, "$poolvarlen"), true));
        Ins(&R, func.body, "    $setlen;");
    }
    // initialize type field
    amc::FTypefld *typefld = (*field.p_arg).c_typefld;
    if (typefld && field.p_arg->c_msgtype) {
        Set(R, "$settype", AssignExpr(*typefld->p_field, "*row", field.p_arg->c_msgtype->type.value, true));
        Ins(&R, func.body, "    $settype;");
    }
    // Copy extra bytes (if non-NULL pointer provided)
    if (FindArg(func,"extra")) {
        Ins(&R, func.body    , "    if (extra) {");
        Ins(&R, func.body    , "        memcpy((u8*)row + sizeof($Cpptype), extra, nbyte_extra);");
        Ins(&R, func.body    , "    }");
    }
    if (field.do_trace) {
        Set(R, "$partrace", Varname(*field.p_ctype));
        Ins(&R, func.body, "    ++$ns::_db.trace.alloc_$partrace_$name;");
    }
    Ins(&R, func.body    , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_AllocMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!field.p_arg->c_optfld) {
        amc::FCtype& fldtype = *field.p_arg;
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func,"i32","n_varfld",zd_varlenfld_First(fldtype));
        if (NeedAllocExtraQ(field)) {
            // this is a varlen struct -- no opt
            func.inl = true;
            AddRetval(func, Subst(R,"$Cpptype*"), "row", Subst(R,"$name_AllocExtraMaybe($pararg, NULL, sizeof($vartype) * n_varfld)"));
        } else {
            Set(R, "$poolvarlen", "sizeof($Cpptype)");
            GenAllocFunc(R,func,field);
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_Alloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!field.p_arg->c_optfld) {
        amc::FCtype& fldtype = *field.p_arg;
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func, "i32", "n_varfld", zd_varlenfld_First(fldtype));
        AddRetval(func, Subst(R,"$Cpptype&"), "", "");
        Ins(&R, func.body , "$Cpptype* row = $name_AllocMaybe($pararg);");
        AddArg(func.body, "n_varfld", zd_varlenfld_First(fldtype));
        if (zd_varlenfld_First(fldtype)) {
            Ins(&R, func.comment, "Allocate memory for a new row with N_VARFLD var-len elements");
        } else {
            Ins(&R, func.comment, "Allocate memory for new default row.");
        }
        Ins(&R, func.comment, "If out of memory, process is killed.");
        Ins(&R, func.body, "if (UNLIKELY(row == NULL)) {");
        Ins(&R, func.body, "    FatalErrorExit(\"$ns.out_of_mem  field:$field  comment:'Alloc failed'\");");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return *row;");
    }
}

// -----------------------------------------------------------------------------

// Generate Alloc function that takes 2 additional arguments
// -- pointer to extra (varlen portion) memory to tack onto the record,
// and number of bytes
void amc::tfunc_Pool_AllocExtraMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (NeedAllocExtraQ(field)) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func, "void *", "extra");
        AddProtoArg(func, "i32", "nbyte_extra");
        Set(R, "$poolvarlen", "sizeof($Cpptype) + nbyte_extra");
        GenAllocFunc(R,func,field);
    }
}

// -----------------------------------------------------------------------------

// Like AllocExtraMaybe, but die on out-of-memory
void amc::tfunc_Pool_AllocExtra() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (NeedAllocExtraQ(field)) {
        vrfy_(field.p_reftype->varlen);// sanity
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func, Subst(R,"$Cpptype&"), "", "");
        AddProtoArg(func, "void *", "extra");
        AddProtoArg(func, "i32", "nbyte_extra");
        Ins(&R, func.body, "$Cpptype *row = $name_AllocExtraMaybe($pararg, extra, nbyte_extra);");
        Ins(&R, func.body, "if (UNLIKELY(row == NULL)) {");
        Ins(&R, func.body, "    FatalErrorExit(\"$ns.out_of_mem  field:$field  comment:'Alloc failed'\");");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return *row;");
    }
}

// -----------------------------------------------------------------------------

// VarlenMaybe -- array of fixed-size structs tacked onto the end of a struct
void amc::tfunc_Pool_AllocVarlenMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FCtype& fldtype = *field.p_arg;
    if (zd_varlenfld_First(fldtype)) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        Ins(&R, func.comment, "Allocate memory for a new row; Copy var-len portion from $varfld.");
        Ins(&R, func.comment, "This is a convenience function that uses $name_Alloc");
        Ins(&R, func.ret  , "$Cpptype*", false);
        Ins(&R, func.proto, "$name_AllocVarlenMaybe($Parent, algo::aryptr<$vartype> $varfld)", false);
        Ins(&R, func.body , "return $name_AllocExtraMaybe($pararg, $varfld.elems, $varfld.n_elems * sizeof($vartype));");
    }
}

// -----------------------------------------------------------------------------

// Like VarlenMaybe, but die on out-of-memory
void amc::tfunc_Pool_AllocVarlen() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FCtype& fldtype = *field.p_arg;
    if (zd_varlenfld_First(fldtype)) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        Ins(&R, func.ret  , "$Cpptype&", false);
        Ins(&R, func.proto, "$name_AllocVarlen($Parent, algo::aryptr<$vartype> $varfld)", false);
        Ins(&R, func.body , "return $name_AllocExtra($pararg, $varfld.elems, $varfld.n_elems * sizeof($vartype));");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;// pool
    bool can_copy = CanCopyQ(*field.p_arg);// target type
    if (can_copy) {
        Set(R, "$Basetype", GetBaseType(*field.p_arg,NULL)->cpp_type);
        amc::FFunc& ins = amc::CreateCurFunc();
        Ins(&R, ins.comment, "Create new row from struct.");
        Ins(&R, ins.comment, "Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)");
        Ins(&R, ins.ret  , "$Cpptype*", false);
        Ins(&R, ins.proto, "$name_InsertMaybe($Parent, const $Basetype &value)", false);
        // carry opt/varlen portion into the new field
        // this only occurs when a ctype with Opt/Varlen field is used as a base.
        if (NeedAllocExtraQ(field)) {
            Set(R,"$inslenexpr", amc::LengthExpr64(*field.p_arg, "value"));
            Ins(&R, ins.body    , "u8 *addon_addr = (u8*)&value + ssizeof($Basetype);");
            // The i64 expansion below is itself only overflow-safe for a
            // length word whose range fits u32: for a wider word, a corrupt
            // stored value overflows the signed arithmetic -- with scale > 1
            // a value above i64_max/scale overflows the multiply, and even
            // at scale 1 a value beyond i64_max wraps to a negative i64
            // whose extra/fixed-size adjustment then underflows -- undefined
            // behavior inside the very expression meant to keep the corrupt
            // word visible, entitling the optimizer to elide the addon_len
            // range test. For such a word, first bound the raw word by the
            // largest value whose expanded total fits the i32 frame-length
            // domain, a constant known at generation time; the bounded
            // arithmetic cannot overflow, and the rejected set is exactly
            // what the addon_len test would have rejected.
            // A nonpositive scale has no store formula to invert, and this
            // step runs ahead of gen_check_lenfld, which is what reports it
            // (amc.lenfld_scale): dividing by the scale here would kill the
            // run on SIGFPE before the diagnostic naming the offending row
            // ever reaches the terminal.
            amc::FLenfld *lenfld = field.p_arg->c_lenfld;
            u64 maxstore = 0;
            bool bound = lenfld && lenfld->scale > 0 && amc::LenfldMaxStore(*lenfld, maxstore) && maxstore > u64(0xffffffff);
            if (bound) {
                i64 maxword = i64_Max((i64(0x7fffffff) + lenfld->extra) / lenfld->scale, 0);
                Set(R,"$rawword", FieldvalExpr(field.p_arg, *lenfld->p_field, "value"));
                Set(R,"$maxword", tempstr() << maxword << "ULL");
                Ins(&R, ins.body, "if (u64($rawword) > $maxword) {");
                Ins(&R, ins.body, "    return NULL; // length word beyond any storable total; reject before the i64 expansion can overflow");
                Ins(&R, ins.body, "}");
            }
            Ins(&R, ins.body    , "i64 addon_len = $inslenexpr - ssizeof($Basetype);");
            // A corrupt length word smaller than the fixed size (a zeroed
            // struct, a corrupt wire message) makes the addon count negative,
            // and one beyond the storable range has no total the length
            // field can hold. The i64 arithmetic keeps both cases visible: a
            // scaled multiply in the word's own type would wrap a huge
            // corrupt value mod 2^N into a small plausible count. Either way
            // it is an input error, and failed insertion is this function's
            // contract (NULL); letting the die-on-fail AllocExtra refuse it
            // would kill the process blaming memory instead. The test is
            // therefore the allocation's own accept expression, over the
            // pool row type the allocation sizes against, which is not the
            // base type the addon count is measured from: an F row carrying
            // a link field or a block-pool buffer member is larger than its
            // base, and a bound written in the base's domain would pass a
            // count the allocation refuses.
            tempstr okcomment;
            tempstr okcond = AllocExtraOkExpr(R, lenfld, "addon_len", okcomment);
            Ins(&R, ins.body    , tempstr() << "if (!(" << okcond << ")) { // " << okcomment);
            Ins(&R, ins.body    , "    return NULL; // the addon count is not one the allocation accepts");
            Ins(&R, ins.body    , "}");
            Ins(&R, ins.body    , "$Cpptype *row = &$name_AllocExtra($pararg, addon_addr, i32(addon_len));");
        } else {
            Ins(&R, ins.body    , "$Cpptype *row = &$name_Alloc($pararg); // if out of memory, process dies. if input error, return NULL.");
        }
        Ins(&R, ins.body    , tempstr()<<Instname(*field.p_arg)<<"_CopyIn(*row,const_cast<$Basetype&>(value));");
        Ins(&R, ins.body    , "bool ok = $name_XrefMaybe(*row); // this may return false");
        Ins(&R, ins.body    , "if (!ok) {");
        if (HasRemoveLastQ(*field.p_reftype)) {
            Ins(&R, ins.body, "    $name_RemoveLast($pararg); // delete offending row, any existing xrefs are cleared");
        } else {
            Ins(&R, ins.body, "    $name_Delete($pararg, *row); // delete offending row, any existing xrefs are cleared");
        }
        Ins(&R, ins.body    , "    row = NULL; // forget this ever happened");
        Ins(&R, ins.body    , "}");
        Ins(&R, ins.body    , "return row;");
    }
}

// -----------------------------------------------------------------------------

static amc::FXref *FindChildXref(amc::FField &field) {// find xref in same namespace
    amc::FXref *retval=NULL;
    ind_beg(amc::field_zd_xref_keyfld_curs,xref,field) {
        if (xref.p_field->p_ctype->p_ns == field.p_ctype->p_ns) {
            retval=&xref;
            break;
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

amc::FField *amc::FindFieldByName(amc::FCtype &ctype, algo::strptr name) {
    return amc::ind_field_Find(dmmeta::Field_Concat_ctype_name(ctype.ctype,name));
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_UpdateMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &pool = *amc::_db.genctx.p_field;
    amc::FCtype &ctype = *pool.p_arg;
    bool can_copy = CanCopyQ(*pool.p_arg);

    amc::FThash *primary_idx = NULL;
    if (can_copy && pool.c_finput && pool.c_finput->update) {
        primary_idx=amc::PrimaryIndex(ctype);
    }
    if (primary_idx) {
        amc::FFunc& update = amc::CreateCurFunc();
        Set(R, "$Basetype", GetBaseType(ctype,NULL)->cpp_type);
        Set(R,"$idxname",name_Get(*primary_idx->p_field));
        Set(R,"$hashfldget",FieldvalExpr(pool.p_arg,*primary_idx->p_hashfld,"value"));
        Ins(&R, update.ret  , "$Cpptype*", false);
        Ins(&R, update.proto, "$name_UpdateMaybe($Parent, $Basetype &value)", false);
        Ins(&R, update.body, "$Cpptype *row = NULL;");
        Ins(&R, update.body, "row = $ns::$idxname_Find($hashfldget);");
        Ins(&R, update.body, "if (row) {");
        // scan access paths for the record being updated
        // remove from any access path other than the primary index
        //
        // NOTE: An xref field (Ptr) might exist which is looked up through the primary index
        // but the condition references a field that might be changed via 'update'
        // Inscond is not a "membership condition" but an "insert condition" so we don't
        // consider them here
        ind_beg(amc::ctype_zd_access_curs,access,ctype) {
            Set(R, "$accessname", name_Get(access));
            bool removable=(access.reftype == dmmeta_Reftype_reftype_Thash && &access != primary_idx->p_field)
                || (access.reftype == dmmeta_Reftype_reftype_Blkhash)
                || (access.reftype == dmmeta_Reftype_reftype_Bheap)
                || (access.reftype == dmmeta_Reftype_reftype_Atree);
            if (removable) {
                if (GlobalQ(*access.p_ctype)) {
                    Ins(&R, update.body, "$accessname_Remove(*row);");
                } else {
                    prerr("amc.update"
                          <<Keyval("ctype",ctype.ctype)
                          <<Keyval("access_path",access.field)
                          <<Keyval("comment","Don't know how to update access path during update (not implemented)"));
                    algo_lib::_db.exit_code=1;
                }
            }
        }ind_end;
        // loop over fields of basetype
        // skip cppfunc, substr, alias, lenfld, typefld, and any field with xref
        amc::FCtype *base = GetBaseType(ctype,pool.p_arg);
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            if (amc::FField *basefield = amc::FindFieldByName(*base, name_Get(field))) {
                (void)basefield;
                Set(R, "$cppname", name_Get(field));
                if (field.c_xref) {
                    Ins(&R, update.body, "    // $cppname: xref exists, not updating");
                } else if (amc::FXref *xref = FindChildXref(field)) {
                    Set(R, "$childxref", xref->field);
                    Ins(&R, update.body, "    // $cppname: target of xreffld ($childxref), not updating");
                } else if (FldfuncQ(field)) {// cppfunc, substr, alias
                } else if (ComputedFieldQ(field)) {// lenfld, typefld
                    Ins(&R, update.body, "    // $cppname: computed, not updating");
                } else if (ValQ(field)) {
                    Ins(&R, update.body, "    row->$cppname = value.$cppname;");
                }
            }
        }ind_end;
        // put the record back on every access path it was taken off.  A
        // secondary index is keyed by a field the copy above may have changed,
        // which is why the record had to leave it; leaving it out is a record
        // that exists and cannot be found -- a record whose secondary key was
        // updated stops resolving by that key, and every lookup reaching it
        // through that index misses.
        ind_beg(amc::ctype_zd_access_curs,access,ctype) {
            Set(R, "$accessname", name_Get(access));
            bool removable=(access.reftype == dmmeta_Reftype_reftype_Thash && &access != primary_idx->p_field)
                || (access.reftype == dmmeta_Reftype_reftype_Blkhash)
                || (access.reftype == dmmeta_Reftype_reftype_Bheap)
                || (access.reftype == dmmeta_Reftype_reftype_Atree);
            if (removable && GlobalQ(*access.p_ctype)) {
                bool wur = access.reftype == dmmeta_Reftype_reftype_Thash
                    || access.reftype == dmmeta_Reftype_reftype_Blkhash
                    || access.reftype == dmmeta_Reftype_reftype_Atree;
                if (wur) {
                    Ins(&R, update.body, "    (void)$accessname_InsertMaybe(*row);");
                } else {
                    Ins(&R, update.body, "    $accessname_Insert(*row);");
                }
            }
        }ind_end;
        Ins(&R, update.body, "} else {");
        Ins(&R, update.body, "    row = $name_InsertMaybe(value);");
        Ins(&R, update.body, "}");
        Ins(&R, update.body, "return row;");
    }
}

// -----------------------------------------------------------------------------

// Return C++ expression computing total length of memory
// pointed to by NAME, whose type is CTYPE
static tempstr TotlenExpr(algo_lib::Replscope &R, amc::FCtype *ctype, strptr name) {
    tempstr ret;
    if (ctype->c_lenfld) {
        ret << amc::LengthExpr(*ctype, name);
    } else if (zd_varlenfld_First(*ctype)) {
        Set(R, "$vartype", zd_varlenfld_First(*ctype)->cpp_type);
        ret << "sizeof($Cpptype) + $varfld_N("<<name<<") * sizeof($vartype)";
    } else {
        ret << "sizeof($Cpptype)";
    }
    return ret;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_Delete() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.p_reftype->del) {
        amc::FFunc& fdel = amc::CreateCurFunc(true);
        AddRetval(fdel,"void","","");
        AddProtoArg(fdel, Subst(R,"$Cpptype &"), "row");
        Set(R, "$partrace", Varname(*field.p_ctype));
        Ins(&R, fdel.comment, "Remove row from all global and cross indices, then deallocate row");
        bool haslen = PoolVarlenQ(field);
        if (haslen) {
            Set(R, "$totlen", TotlenExpr(R, field.p_arg, "row"));
            // fetch the length before the destructor runs
            Ins(&R, fdel.body, "int length = $totlen;");
        }
        bool blkpool = BlkpoolQ(field);
        if (blkpool) {
            Ins(&R, fdel.body, "$name_Bpbuf* buf = row.$name_buf;");
            Ins(&R, fdel.body, "row.$name_buf = ($name_Bpbuf*)-1; // ($field) not in blkpool; FreeMem catches a second delete");
        }
        if (HasDtorQ(*field.p_arg)) {
            Ins(&R, fdel.body, "row.~$Ctype();");
        }
        if (blkpool) {
            Ins(&R, fdel.body, "(void)length;");
            Ins(&R, fdel.body, "$name_FreeMem(buf);");
        } else if (PoolVarlenQ(field)) {
            Ins(&R, fdel.body, "$name_FreeMem($pararg, &row, length);");// FreeMem takes pointer
        } else {
            Ins(&R, fdel.body, "$name_FreeMem($pararg, row);");// FreeMem takes reference
        }
        if (field.do_trace) {
            Ins(&R, fdel.body, "++$ns::_db.trace.del_$partrace_$name;");
        }
    }
}

// -----------------------------------------------------------------------------

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
void amc::gen_check_lenfld() {
    ind_beg(amc::_db_lenfld_curs, lenfld, amc::_db) {
        amc::FCtype &ctype = *lenfld.p_field->p_ctype;
        if (lenfld.scale <= 0) {
            prerr("amc.lenfld_scale"
                  <<Keyval("field",lenfld.field)
                  <<Keyval("scale",lenfld.scale)
                  <<Keyval("comment","lenfld scale must be positive"));
            algo_lib::_db.exit_code++;
        } else {
            u64 maxval = 0;
            // the storable max is the arg type's range, clamped to the
            // declared width for a Bitfld lenfld, whose setter masks the store
            bool known = amc::LenfldMaxStore(lenfld, maxval);
            bool fixedonly_pnew = !c_pnew_EmptyQ(ctype) && !amc::RuntimeFrameLenQ(ctype);
            bool tsstore = amc::TsQ(ctype) && ctype.c_pack != NULL && lenfld.p_field->reftype == dmmeta_Reftype_reftype_Val;
            bool storefixed = ctype.c_msgtype != NULL || amc::HasAllocInstQ(ctype) || fixedonly_pnew || (tsstore && !amc::RuntimeFrameLenQ(ctype));
            bool storetotal = storefixed || tsstore;
            // numerator of the smallest frame's stored length; i64 so a negative extra can surface
            i64 minframe = i64(ctype.totsize_byte) + lenfld.extra;
            // an arg with no numeric range (GetMinMax false) makes every range
            // check below vacuous against the widest-signed fallback bounds;
            // downstream emitters (TsLenfldStoreExpr) also derive their runtime
            // guard from the range, so the schema is rejected here for all of
            // them. The args with a range are the fixed-width integers, u8
            // through i64; a bool word holds no magnitude and a char's
            // signedness is the compiler's choice, so both land here too
            if (!known) {
                prerr("amc.lenfld_type"
                      <<Keyval("field",lenfld.field)
                      <<Keyval("arg",lenfld.p_field->arg)
                      <<Keyval("comment","lenfld arg has no numeric range amc can use for a length word"));
                algo_lib::_db.exit_code++;
            } else if (lenfld.extra > 0 && maxval < (u64(lenfld.extra) + u64(lenfld.scale) - 1) / u64(lenfld.scale)) {
                prerr("amc.lenfld_extra"
                      <<Keyval("field",lenfld.field)
                      <<Keyval("extra",lenfld.extra)
                      <<Keyval("scale",lenfld.scale)
                      <<Keyval("max",maxval)
                      <<Keyval("comment","extra exceeds the length field range; no frame length is representable"));
                algo_lib::_db.exit_code++;
            } else if (storetotal && !ctype.size_unknown && (minframe < 0 || maxval < u64(minframe) / u64(lenfld.scale))) {
                prerr("amc.lenfld_extra"
                      <<Keyval("field",lenfld.field)
                      <<Keyval("size",ctype.totsize_byte)
                      <<Keyval("extra",lenfld.extra)
                      <<Keyval("scale",lenfld.scale)
                      <<Keyval("max",maxval)
                      <<Keyval("comment","fixed size + extra is outside the length field range; the minimum frame's length cannot be stored"));
                algo_lib::_db.exit_code++;
            } else if (storefixed && !ctype.size_unknown && (ctype.totsize_byte + lenfld.extra) % lenfld.scale != 0) {
                prerr("amc.lenfld_scale"
                      <<Keyval("field",lenfld.field)
                      <<Keyval("size",ctype.totsize_byte)
                      <<Keyval("extra",lenfld.extra)
                      <<Keyval("scale",lenfld.scale)
                      <<Keyval("comment","fixed size + extra is not divisible by scale; the generated length store would truncate"));
                algo_lib::_db.exit_code++;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Ensure that all instances of CTYPE's record are varlen pools
void amc::EnsureVarlenPool(amc::FCtype &ctype) {
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {
        if (!inst.p_reftype->varlen) {
            prerr("amc.infinity_pool"
                  <<Keyval("field",inst.field)
                  <<Keyval("arg",inst.arg)
                  <<Keyval("reftype",inst.reftype)
                  <<Keyval("comment","Varlen pool must be used for this ctype"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// First instance of CTYPE that is an allocating pool, or NULL.
// When SAMENS, only a pool defined in CTYPE's own namespace counts.
static amc::FField *FindAllocInst(amc::FCtype &ctype, bool samens) {
    amc::FField *ret=NULL;
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {
        if ((!samens || inst.p_ctype->p_ns == ctype.p_ns) && inst.p_reftype->hasalloc) {
            ret=&inst;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when a pool allocates values of CTYPE, in any namespace
bool amc::HasAllocInstQ(amc::FCtype &ctype) {
    return FindAllocInst(ctype, false) != NULL;
}

// -----------------------------------------------------------------------------

// Find pool used for allocating values of type CTYPE
amc::FField *amc::FindPool(amc::FCtype &ctype) {
    return FindAllocInst(ctype, true);
}

// -----------------------------------------------------------------------------

// Find default used for allocating things in namespace NS
amc::FField *amc::DefaultPool(amc::FNs &ns) {
    return ns.c_nsx ? ns.c_nsx->p_pool : NULL;
}
