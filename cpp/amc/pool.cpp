// Copyright (C) 2023-2024 AlgoRND
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
// Exceptions: NO
// Source: cpp/amc/pool.cpp
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
    return ctype.c_lenfld && (ctype.c_optfld || ctype.c_varlenfld);
}

// -----------------------------------------------------------------------------

void amc::tclass_Pool() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;

    amc::FCtype& fldtype = *field.p_arg;
    if (fldtype.c_varlenfld) {
        Set(R, "$vartype", fldtype.c_varlenfld->cpp_type);
        Set(R, "$varfld", name_Get(*fldtype.c_varlenfld));
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

static void GenAllocFunc(algo_lib::Replscope &R, amc::FFunc &func, amc::FField &field) {
    bool poolvarlen = PoolVarlenQ(field);
    amc::AddRetval(func, Subst(R,"$Cpptype*"), "row", Subst(R,"($Cpptype*)$name_AllocMem($pararg)"));
    amc::AddArg(func.body, Subst(R,"$totlenexpr"), poolvarlen);
    Ins(&R, func.body , "if (row) {");
    Ins(&R, func.body , "    new (row) $Cpptype; // call constructor");
    // initialize rowid
    ind_beg(amc::ctype_c_field_curs, rowidfield, *field.p_arg) if (rowidfield.c_rowid) {
        Set(R, "$setrowid", AssignExpr(rowidfield, "*row", "$name_N($pararg) - 1", true));
        Ins(&R, func.body, "    $setrowid;");
    }ind_end;
    // initialize length field
    amc::FLenfld *lenfld = (*field.p_arg).c_lenfld;
    if (lenfld) {
        Set(R, "$extra", tempstr() << field.p_arg->c_lenfld->extra);
        Set(R, "$setlen", AssignExpr(*lenfld->p_field, "*row", "$totlenexpr - $extra", true));
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
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, func.body, "    ++$ns::_db.trace.alloc_$partrace_$name;");
    }
    Ins(&R, func.body    , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_AllocMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!field.p_arg->c_optfld) {
        amc::FCtype& fldtype = *field.p_arg;
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func,"i32","n_varfld",fldtype.c_varlenfld);
        if (NeedAllocExtraQ(field)) {
            // this is a varlen struct -- no opt
            func.inl = true;
            AddRetval(func, Subst(R,"$Cpptype*"), "row", Subst(R,"$name_AllocExtraMaybe($pararg, NULL, sizeof($vartype) * n_varfld)"));
        } else {
            Set(R, "$totlenexpr", "sizeof($Cpptype)");
            GenAllocFunc(R,func,field);
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_Alloc() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!field.p_arg->c_optfld) {
        amc::FCtype& fldtype = *field.p_arg;
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func, "i32", "n_varfld", fldtype.c_varlenfld);
        AddRetval(func, Subst(R,"$Cpptype&"), "", "");
        Ins(&R, func.body , "$Cpptype* row = $name_AllocMaybe($pararg);");
        AddArg(func.body, "n_varfld", fldtype.c_varlenfld);
        if (fldtype.c_varlenfld) {
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (NeedAllocExtraQ(field)) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func, "void *", "extra");
        AddProtoArg(func, "i32", "nbyte_extra");
        Set(R, "$totlenexpr", "sizeof($Cpptype) + nbyte_extra");
        GenAllocFunc(R,func,field);
    }
}

// -----------------------------------------------------------------------------

// Like AllocExtraMaybe, but die on out-of-memory
void amc::tfunc_Pool_AllocExtra() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCtype& fldtype = *field.p_arg;
    if (fldtype.c_varlenfld) {
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCtype& fldtype = *field.p_arg;
    if (fldtype.c_varlenfld) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        Ins(&R, func.ret  , "$Cpptype&", false);
        Ins(&R, func.proto, "$name_AllocVarlen($Parent, algo::aryptr<$vartype> $varfld)", false);
        Ins(&R, func.body , "return $name_AllocExtra($pararg, $varfld.elems, $varfld.n_elems * sizeof($vartype));");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;// pool
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
            Set(R,"$inslenexpr", amc::LengthExpr(*field.p_arg, "value"));
            Ins(&R, ins.body    , "u8 *addon_addr = (u8*)&value + ssizeof($Basetype);");
            Ins(&R, ins.body    , "int addon_len = $inslenexpr - ssizeof($Basetype);");
            Ins(&R, ins.body    , "$Cpptype *row = &$name_AllocExtra($pararg, addon_addr, addon_len);");
        } else {
            Ins(&R, ins.body    , "$Cpptype *row = &$name_Alloc($pararg); // if out of memory, process dies. if input error, return NULL.");
        }
        Ins(&R, ins.body    , "$name_CopyIn(*row,const_cast<$Basetype&>(value));");
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &pool = *amc::_db.genfield.p_field;
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
    } else if (ctype->c_varlenfld) {
        Set(R, "$vartype", ctype->c_varlenfld->cpp_type);
        ret << "sizeof($Cpptype) + $varfld_N("<<name<<") * sizeof($vartype);";
    } else {
        ret << "sizeof($Cpptype)";
    }
    return ret;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Pool_Delete() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.p_reftype->del) {
        amc::FFunc& fdel = amc::CreateCurFunc(true);
        AddRetval(fdel,"void","","");
        AddProtoArg(fdel, Subst(R,"$Cpptype &"), "row");
        Set(R, "$partrace", Refname(*field.p_ctype));
        Ins(&R, fdel.comment, "Remove row from all global and cross indices, then deallocate row");
        bool haslen = PoolVarlenQ(field);
        if (haslen) {
            Set(R, "$totlen", TotlenExpr(R, field.p_arg, "row"));
            // fetch the length before the destructor runs
            Ins(&R, fdel.body, "int length = $totlen;");
        }
        if ((field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor)||!field.p_arg->c_cpptype) {
            Ins(&R, fdel.body, "row.~$Ctype();");
        }
        if (PoolVarlenQ(field)) {
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

// Ensure that all instances of CTYPE's record are varlen pools
void amc::EnsureVarlenPool(amc::FCtype &ctype) {
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {
        if (!inst.p_reftype->varlen) {
            prerr("amc.infinity_pool"
                  <<Keyval("field",inst.field)
                  <<Keyval("arg",inst.arg)
                  <<Keyval("reftype",inst.reftype)
                  <<Keyval("comment","Varlen pool must be used for this ctype"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Find pool used for allocating values of type CTYPE
amc::FField *amc::FindPool(amc::FCtype &ctype) {
    amc::FField *ret=NULL;
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {
        if (inst.p_ctype->p_ns == ctype.p_ns && inst.p_reftype->hasalloc) {
            ret=&inst;
            return ret;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Find default used for allocating things in namespace NS
amc::FField *amc::DefaultPool(amc::FNs &ns) {
    return ns.c_nsx ? ns.c_nsx->p_pool : NULL;
}
