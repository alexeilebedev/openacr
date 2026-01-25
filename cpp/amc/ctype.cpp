// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/ctype.cpp -- Ctype code generators
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Ctype() {
}

// -----------------------------------------------------------------------------

// Generate de-initialization function:
// - cascdel fields
// - remove record from all indexes
// - user-defined cleanup for all fields
// - un-init fields (frees memory)
// All fields are scanned in reverse order
void amc::tfunc_Ctype_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    amc::FField *inst = FirstInst(ctype);
    if (!ctype.c_cextern) {
        amc::FNs &ns = *ctype.p_ns;
        tempstr refname = amc::Refname(ctype);
        amc::FFunc& uninit = amc::CreateCurFunc(false);
        Ins(&R, uninit.proto, "$Name_Uninit()", false);
        AddProtoArg(uninit, Refto(ctype.cpp_type), refname, !GlobalQ(ctype));
        AddRetval(uninit, "void", "", "");
        Ins(&R, uninit.body, "$Partype &row = $parname; (void)row;");
        uninit.inl = c_field_N(ctype)<10;
        int naction=0;

        // call user-defined cleanup
        rrep_(i, amc::c_field_N(ctype)) {
            amc::FField &field = *c_field_Find(ctype,i);
            if (field.c_fcleanup) {
                Set(R, "$field", field.field);
                Set(R, "$name", name_Get(field));
                Ins(&R, uninit.body, "$name_Cleanup($pararg); // dmmeta.fcleanup:$field");
                naction++;
            }
        }

        // cascdel fields
        rrep_(i, amc::c_field_N(ctype)) {
            amc::FField &field = *amc::c_field_Find(ctype,i);
            if (field.c_cascdel) {
                Set(R, "$field", field.field);
                Set(R, "$name", name_Get(field));
                Ins(&R, uninit.body, "$name_Cascdel($pararg); // dmmeta.cascdel:$field");
                naction++;
            }
        }

        // remove this record from all indexes
        if (inst && ns.c_globfld) {
            amc::FGenXref frame;
            Set(R, "$inst"  , inst->field);
            Set(R, "$name" , name_Get(*inst));
            ind_beg(amc::ctype_zs_xref_curs, xref, ctype) if (xref.p_field->reftype != dmmeta_Reftype_reftype_Upptr) {
                Set(R, "$xrefname", name_Get(xref));
                if (ns.c_globfld && xref.p_field->p_ctype == ns.c_globfld->p_ctype) {
                    Ins(&R, uninit.body, "$xrefname_Remove(row); // remove $name from index $xrefname");
                } else {
                    ComputeAccess(R,ctype,xref,uninit,frame, false);
                    Ins(&R, uninit.body, "if (p_$parent)  {");
                    Ins(&R, uninit.body, "    $xrefname_Remove(*p_$parent, row);// remove $name from index $xrefname");
                    Ins(&R, uninit.body, "}");
                }
                uninit.inl=false;// the operation isn't fast anyway, so uninline the function
                naction++;
            }ind_end;
        }


        // uninit fields (release memory)
        rrep_(i, amc::c_field_N(ctype)) {
            amc::FField &field = *c_field_Find(ctype,i);
            if (amc::FFunc *func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Uninit"))) {
                if (ch_N(func->body)) {
                    Set(R, "$field", field.field);
                    Set(R, "$comment", field.comment, false);
                    Ins(&R, uninit.body, "");
                    Ins(&R, uninit.body, tempstr()<<"// $field.Uninit ("<<field.reftype<<")  //$comment");
                    uninit.body << func->body;
                    uninit.inl = uninit.inl && func->inl;
                    naction++;
                }
            }
        }

        // nothing generated -- disable function
        // but not for FDb
        if (naction==0 && !GlobalQ(ctype)) {
            uninit.disable=true;
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckSeen(amc::FGenXrefSeen &seen, amc::FXref &xref) {
    // computed pointer is a function of the path.
    // check that cached path is the same.
    // the key we use is just a puny variable name (name of xreffld) but the actual path must be considered,
    // it consists of xreffld & via
    amc::FField     *xreffld = GetKeyfld(xref);
    amc::FField *viafld = GetViafld(xref);
    if (viafld && seen.via != viafld->field) {
        prerr("amc.badvia"
              <<Keyval("field",xref.field)
              <<Keyval("xreffld",strptr(xreffld->field))
              <<Keyval("via1",viafld->field)
              <<Keyval("via2",seen.via)
              <<Keyval("comment","Lookup of same target via 2 different paths is not allowed"));
    }
    if (seen.xreffld != xreffld->field) {
        prerr("amc.badxreffld"
              <<Keyval("field",xref.field)
              <<Keyval("xreffld",xreffld->field)
              <<Keyval("xreffld2",seen.xreffld)
              <<Keyval("comment","Lookup of same target via 2 different paths is not allowed"));
    }
}

// -----------------------------------------------------------------------------

// Generate null-acces check for xref function
static void ComputeAccess_CheckNull(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FXref &xref, amc::FFunc &func) {
    amc::FField     *xreffld = GetKeyfld(xref);
    amc::FField *viafld = GetViafld(xref);
    if (xreffld->reftype == dmmeta_Reftype_reftype_Upptr) {
        Ins(&R, func.body, "if (UNLIKELY(!p_$parent)) {");
        Ins(&R, func.body, "    algo_lib::ResetErrtext() << \"$ns.null_ref  xref:$xrefkey\";");
        Ins(&R, func.body, "    return false;");
        Ins(&R, func.body, "}");
    } else if (viafld) {
        Set(R, "$via",viafld->field);
        Set(R, "$getrow", FieldvalExpr(&ctype, *xreffld, "row"));
        Ins(&R, func.body, "if (UNLIKELY(!p_$parent)) {");
        Ins(&R, func.body, "    algo_lib::ResetErrtext() << \"$ns.bad_xref  index:$via\" << Keyval(\"key\", $getrow);");
        Ins(&R, func.body, "    return false;");
        Ins(&R, func.body, "}");
    } else {
        prerr("amc.xref"
              <<Keyval("field",xref.field)
              <<Keyval("comment","xrefvia required"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

// Introduce local variable that points to the parent side of XREF
// by evaluating the path provided by xref + xreffld + xrefvia records.
bool amc::ComputeAccess(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FXref &xref, amc::FFunc &func, amc::FGenXref &frame, bool check_null) {
    bool retval=false;
    amc::FField *keyfld = GetKeyfld(xref);
    amc::FField *viafld = GetViafld(xref);
    if (keyfld) {
        dmmeta::FieldPkey xreffldname(name_Get(*keyfld));
        dmmeta::FieldPkey via(viafld ? strptr(viafld->field) : strptr());
        Set(R, "$parent", xreffldname);
        Set(R, "$access", FieldvalExpr(&ctype, *keyfld, "row"));// execute early to allow more error checking
        amc::FGenXrefSeen *seen = ind_seen_Find(frame,xreffldname);
        if (seen) {
            CheckSeen(*seen,xref);
        }
        if (!seen && !(xref.p_field->p_ctype == ctype.p_ns->c_globfld->p_ctype)) {// ignore global xrefs
            seen = &seen_Alloc(frame);
            seen->value = xreffldname;
            seen->via = via;
            seen->xreffld = keyfld->field;
            ind_seen_InsertMaybe(frame,*seen_Last(frame));
            amc::FCtype          *parentrow = xref.p_field->p_ctype == &ctype ? xref.p_field->p_arg : xref.p_field->p_ctype;
            if (!(keyfld->reftype == dmmeta_Reftype_reftype_Ptr || keyfld->reftype == dmmeta_Reftype_reftype_Upptr)) {
                vrfy(viafld,"Missing xrefvia");
                Set(R, "$xrefvia", tempstr() << ns_Get(*viafld->p_ctype) << "::" << name_Get(*viafld));
                Set(R, "$access", "$xrefvia_Find($access)");
            }
            Set(R, "$ParType", parentrow->cpp_type);
            Ins(&R, func.body, "$ParType* p_$parent = $access;");
            if (check_null) {
                ComputeAccess_CheckNull(R,ctype,xref,func);
            }
            retval=true;
        }
    } else {
        Set(R,"$parent","");
    }
    return retval;
}

static bool ProcessXrefQ(amc::FXref &xref, int iter, bool sort_xrefs) {
    // if xrefs are being sorted, upptrs are processed on first iteration
    // otherwise, all fields are processed on first iteration
    return sort_xrefs ? (xref.p_field->reftype == dmmeta_Reftype_reftype_Upptr)==1-iter
        : iter==0;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_XrefMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    amc::FNs &ns = *ctype.p_ns;

    if (FirstInst(ctype) && ns.c_globfld) {
        amc::FField &inst = *FirstInst(ctype);
        amc::FGenXref frame;
        Set(R, "$inst"  , inst.field);
        Set(R, "$name" , name_Get(inst));
        // xref function for parent field??
        amc::FFunc& xrefmaybe = amc::ind_func_GetOrCreate(Subst(R,"$inst.XrefMaybe"));
        Ins(&R, xrefmaybe.comment, "Insert row into all appropriate indices. If error occurs, store error");
        Ins(&R, xrefmaybe.comment, "in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.");
        AddRetval(xrefmaybe, "bool", "retval", "true");
        if (!GlobalQ(ctype)) {
            Ins(&R, xrefmaybe.body , "(void)row;");
        }
        Ins(&R, xrefmaybe.proto, (GlobalQ(ctype) ? "$name_XrefMaybe()" : "$name_XrefMaybe($Partype &row)"), false);
        bool sort_xrefs = ns.c_nsx && ns.c_nsx->sortxref;

        frep_(iter,2) {
            ind_beg(amc::ctype_zs_xref_curs, xref, ctype) if (xref.inscond.value != "false" && ProcessXrefQ(xref,iter,sort_xrefs)) {
                amc::FField *keyfld = GetKeyfld(xref);
                bool upptr = xref.p_field->reftype == dmmeta_Reftype_reftype_Upptr;
                Set(R, "$xrefname", name_Get(xref));
                Set(R, "$xrefkey", xref.field);
                Set(R, "$ns"  , ns.ns);
                ComputeAccess(R,ctype,xref,xrefmaybe,frame,true);
                Set(R, "$inscond", xref.inscond.value);
                bool glob2 = xref.p_field->p_ctype == ns.c_globfld->p_ctype;
                Set(R, "$thisparname", glob2 || !keyfld ? strptr() : strptr(tempstr() << "*p_"<<name_Get(*keyfld)));
                if (upptr) {
                    Ins(&R, xrefmaybe.body    , "// $name: save pointer to $parent");
                } else {
                    Ins(&R, xrefmaybe.body    , "// insert $name into index $xrefname");
                }
                Ins(&R, xrefmaybe.body    , "if ($inscond) { // user-defined insert condition");
                // check if reftype has an InsertMaybe function that checks for duplicates
                if (xref.p_field->reftype == dmmeta_Reftype_reftype_Thash || xref.p_field->reftype == dmmeta_Reftype_reftype_Ptr) {
                    Ins(&R, xrefmaybe.body, "    bool success = $xrefname_InsertMaybe($thisparname, row);");
                    Ins(&R, xrefmaybe.body, "    if (UNLIKELY(!success)) {");
                    Ins(&R, xrefmaybe.body, "        ch_RemoveAll(algo_lib::_db.errtext);");
                    Ins(&R, xrefmaybe.body, "        algo_lib::_db.errtext << \"$ns.duplicate_key  xref:$xrefkey\"; // check for duplicate key");
                    Ins(&R, xrefmaybe.body, "        return false;");
                    Ins(&R, xrefmaybe.body, "    }");
                } else if (upptr) {
                    Ins(&R, xrefmaybe.body, "    row.$xrefname = p_$parent;");
                } else {
                    Ins(&R, xrefmaybe.body, "    $xrefname_Insert($thisparname, row);");
                }
                Ins(&R, xrefmaybe.body    , "}");
            }ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Unref() {
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Hash() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    amc::FNs &ns = *ctype.p_ns;

    if (ctype.c_chash && ctype.c_chash->hashtype != dmmeta_Hashtype_hashtype_Linear) {
        vrfy(zd_varlenfld_EmptyQ(ctype),tempstr()<< "hash function of varlength records is not supported ("<<ctype.ctype<<")");
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype,true));
        ns.nhash++;

        amc::FFunc& hash = amc::CreateCurFunc();
        Ins(&R, hash.ret  , "u32", false);
        Ins(&R, hash.proto, "$Name_Hash(u32 prev, $ByvalArgtype rhs)", false);
        hash.extrn = ctype.c_chash->hashtype == dmmeta_Hashtype_hashtype_Extern;
        hash.inl = c_datafld_N(ctype) < 5;
        if (ctype.c_bltin) {
            vrfy(ctype.c_csize, tempstr()<<"csize record missing for bltin "<<ctype.ctype);
            int size = ctype.c_csize->size;
            if (size <= 8 && ctype.c_bltin->likeu64) {
                // use an intrinsic without taking address
                Set(R, "$wordsize", tempstr()<<ctype.c_csize->size*8);
                Ins(&R, hash.body, "prev = _mm_crc32_u$wordsize(prev,rhs);");
            } else {
                // go over the bytes
                int offset=0;
                Ins(&R, hash.body, "#pragma GCC diagnostic push");
                Ins(&R, hash.body, "#pragma GCC diagnostic ignored \"-Wstrict-aliasing\"");
                for (int wordsize=8; wordsize >= 1; wordsize = wordsize/2) {
                    for (; size >= wordsize; size -= wordsize, offset += wordsize) {
                        Set(R, "$offset", tempstr()<<offset);
                        Set(R, "$wordsize", tempstr()<<wordsize*8);
                        Ins(&R, hash.body, "u$wordsize val$offset = *(u$wordsize*)((u8*)&rhs + $offset);");
                        Ins(&R, hash.body, "prev = _mm_crc32_u$wordsize(prev, val$offset);");
                    }
                }
                Ins(&R, hash.body, "#pragma GCC diagnostic pop");
            }
        } else {
            ind_beg(amc::ctype_c_field_curs, field, ctype) {
                Set(R, "$name", name_Get(field));
                Set(R, "$Fldtype", name_Get(*field.p_arg));
                if (PadQ(field)) {
                } else if (FldfuncQ(field)) {
                } else if (field.c_smallstr) {
                    // The hash function is calculated on the valid characters of the string.
                    // I.e. if the string representation is changed (from leftpad to rightpad, etc)
                    // the hash function doesn't change.
                    // This guarantee is important, as string lengths often mismatch between keys (e.g.
                    // string 'xyz' stored as RspaceStr10 vs RspaceStr20)
                    hash.inl=false;
                    Ins(&R, hash.body, "algo::strptr $name_strptr = $name_Getary(rhs);");
                    Ins(&R, hash.body, "prev = ::strptr_Hash(prev, $name_strptr);");
                } else if (field.reftype == dmmeta_Reftype_reftype_Ptr) {
                    Ins(&R, hash.body, "prev = u64_Hash(prev, u64(rhs.$name));");
                } else if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                    if (field.p_arg->c_bltin && field.p_arg->c_csize && field.p_arg->c_csize->size < 8) {
                        // hash builtin types as strings
                        // i.e. an array of 2 u32s will not hash to the same value
                        // as two u32 fields, but that's ok because there is no expectation of
                        // that behavior
                        // for types over 8 bytes in length, it's better to compute element-by-element
                        Ins(&R, hash.body, "prev = strptr_Hash(prev, algo::strptr((char*)rhs.$name_elems,rhs.$name_n));");
                    } else {
                        Ins(&R, hash.body, "ind_beg($Parname_$name_curs,elem,($Partype&)rhs) {");
                        Ins(&R, hash.body, "    prev = $Fldtype_Hash(prev,elem);");
                        Ins(&R, hash.body, "}ind_end;");
                    }
                } else if (field.reftype == dmmeta_Reftype_reftype_Upptr) {
                    Ins(&R, hash.body, "prev = u64_Hash(prev, u64(rhs.$name));");
                } else if (FixaryQ(field)) {
                    Set(R, "$width", tempstr() << WidthMin(field));
                    Ins(&R, hash.body, "frep_(i,$width) prev = $Fldtype_Hash(prev, rhs.$name_elems[i]);");
                } else if (ValQ(field)) {
                    Set(R, "$Fldtype", name_Get(*field.p_arg));
                    Set(R, "$gethashfld", FieldvalExpr(&ctype, field,"rhs"));
                    Ins(&R, hash.body, tempstr()<<"prev = $Fldtype_Hash(prev, $gethashfld);");
                }
            }ind_end;
        }
        Ins(&R, hash.body, "return prev;");
    }
}

// Comparison function for ctype
// In amc, equality is not strictly a property of a type; it can be overriden on a per-field basis
// For example, the same field (algo.Smallstr100 for instance) can be made case-insensitive
// for the purposes of comparison within some ctype
// That's why the Cmp function for ctype does not call out fo
void amc::tfunc_Ctype_Cmp() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp) {
        vrfy(zd_varlenfld_EmptyQ(ctype),tempstr()<< "comparison function of varlength records is not supported ("<<ctype.ctype<<")");
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));

        amc::FFunc& cmp = amc::CreateCurFunc();
        Ins(&R, cmp.ret  , "i32", false);
        Ins(&R, cmp.proto, "$Name_Cmp($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        cmp.extrn = ctype.c_ccmp->extrn;
        cmp.inl = c_datafld_N(ctype) < 4;
        AddRetval(cmp, "i32", "retval", "0");
        if (ctype.c_bltin) {
            Ins(&R, cmp.body, "retval = lhs<rhs ? -1 : lhs>rhs;");
        } else {
            bool need_test = false;
            ind_beg(amc::ctype_c_datafld_curs, field,ctype) {
                tempstr test;
                if (PadQ(field)) {
                } else if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Cmp"))) {// check field-spsecific compare
                    Set(R, "$name", name_Get(field));
                    Ins(&R, test    , "retval = $name_Cmp(lhs,rhs);");
                } else if (field.reftype == dmmeta_Reftype_reftype_Upptr || field.reftype == dmmeta_Reftype_reftype_Ptr) {// default for pointers
                    Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
                    Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
                    Set(R, "$Fldtype", field.p_arg->cpp_type);
                    Ins(&R, test, "retval = u64_Cmp((u64)(void*)$a_val, (u64)(void*)$b_val);");
                } else if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {// default for small strings
                    Set(R, "$name", name_Get(field));
                    Ins(&R, test, "retval = algo::strptr_Cmp($name_Getary(lhs), $name_Getary(rhs));");
                } else if (field.reftype == dmmeta_Reftype_reftype_Base) {
                    // base fields have been already copied over
                } else if (field.p_arg->c_ccmp) {// type-specific compare
                    Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
                    Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
                    Set(R, "$Fldtype", field.p_arg->cpp_type);
                    Ins(&R, test, "retval = $Fldtype_Cmp($a_val, $b_val);");
                } else {
                    prerr("amc.bad_cmp"
                          <<Keyval("field",field.field)
                          <<Keyval("offending_type",field.arg)
                          <<Keyval("comment","No comparison function defined for offending type"));
                    algo_lib::_db.exit_code=1;
                }
                if (ch_N(test) > 0) {
                    if (need_test) {
                        Ins(&R, cmp.body, "if (retval != 0) {");
                        Ins(&R, cmp.body, "    return retval;");
                        Ins(&R, cmp.body, "}");
                    }
                    cmp.body << test;
                    need_test = true;
                }
            }ind_end;
        }
    }
}

static void Ctype_Lt_SingleField(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FFunc &oplt) {
    ind_beg(amc::ctype_c_datafld_curs, field,ctype) {
        if (PadQ(field)) {
        } else if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Lt"))) {// check field-spsecific compare
            Set(R, "$name", name_Get(field));
            Ins(&R, oplt.body    , "return $name_Lt(lhs,rhs);");
        } else if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Cmp"))) {// check field-spsecific compare
            Set(R, "$name", name_Get(field));
            Ins(&R, oplt.body    , "return $name_Cmp(lhs,rhs) < 0;");
        } else if (field.reftype == dmmeta_Reftype_reftype_Upptr || field.reftype == dmmeta_Reftype_reftype_Ptr) {// default for pointers
            Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
            Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
            Set(R, "$Fldtype", field.p_arg->cpp_type);
            Ins(&R, oplt.body, "return $a_val < b_val;");
        } else if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {// default for small strings
            Set(R, "$name", name_Get(field));
            Ins(&R, oplt.body, "return algo::strptr_Lt($name_Getary(lhs), $name_Getary(rhs));");
        } else if (field.p_arg->c_ccmp) {// type-specific compare
            Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
            Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
            Set(R, "$Fldtype", field.p_arg->cpp_type);
            Ins(&R, oplt.body, "return $Fldtype_Lt($a_val, $b_val);");
        } else {
            prerr("amc.bad_cmp"
                  <<Keyval("field",field.field)
                  <<Keyval("offending_type",field.arg)
                  <<Keyval("comment","No comparison function defined for offending type"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

// Less-than function for ctype
void amc::tfunc_Ctype_Lt() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;

    if (ctype.c_ccmp && ctype.c_ccmp->order) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Name", name_Get(ctype));

        amc::FFunc& oplt = amc::CreateCurFunc();
        Ins(&R, oplt.ret  , "bool", false);
        Ins(&R, oplt.proto, "$Name_Lt($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        oplt.extrn = ctype.c_ccmp->extrn;
        oplt.inl = true;
        if (ctype.c_bltin) {
            Ins(&R, oplt.body    , "return lhs < rhs;");
        } else if (c_datafld_N(ctype) == 1) { // special case -- single field.
            // Lt function is faster than Cmp because there are fewer calculations to do
            // But if there is more than one field the advantage goes away because if a.x is not less
            // than b.x, you would have to check whether a.x==b.x before proceeding to the next field
            // So if there is more than one field, we revert to the generic Cmp.
            Ctype_Lt_SingleField(R,ctype,oplt);
        } else {
            Ins(&R, oplt.body    , "return $Name_Cmp(lhs,rhs) < 0;");
        }
    }
}

void amc::tfunc_Ctype_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;

    tempstr text;
    bool glob = GlobalQ(ctype);
    ch_RemoveAll(text);
    bool inl = true;
    int nfield = 0;
    ind_beg(amc::ctype_c_field_curs, field,ctype) {
        nfield += field.reftype != dmmeta_Reftype_reftype_Base;
        if (amc::FFunc *func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Init"))) {
            if (ch_N(func->body)) {
                text << func->body;
                inl = inl && func->inl;
            }
        }
        if (field.c_fuserinit) {
            Set(R, "$field", field.field);
            if (field.reftype == dmmeta_Reftype_reftype_Global) {
                Ins(&R, text, "Userinit($pararg); // dmmeta.fuserinit:$field");
            } else {
                Set(R, "$name", name_Get(field));
                Ins(&R, text, "$name_Userinit($pararg); // dmmeta.fuserinit:$field");
            }
        }
    }ind_end;

    if (glob) {
        Ins(&R, text, "");
        Ins(&R, text, "$ns::InitReflection();");
        inl = false;
    }

    if (ch_N(text)) {
        amc::FFunc& init = *amc::init_GetOrCreate(ctype);
        text << init.body; // there may be something already
        init.body = text;  // put it after own init
        init.inl = inl && nfield < 10;
    }
}

void amc::tfunc_Ctype_Update() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->order) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_Update($Partype &lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "bool ret = !$Name_Eq(lhs, rhs); // compare values");
        Ins(&R, func.body, "if (ret) {");
        Ins(&R, func.body, "    lhs = rhs; // update");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return ret;");
    }
}

void amc::tfunc_Ctype_Min() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "$Partype", false);
        Ins(&R, func.proto, "$Name_Min($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "return lhs < rhs ? lhs : rhs;");
    }
}

void amc::tfunc_Ctype_Max() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "$Partype", false);
        Ins(&R, func.proto, "$Name_Max($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "return rhs < lhs ? lhs : rhs;");
    }
}

void amc::tfunc_Ctype_UpdateMin() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_UpdateMin($Partype &lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "bool retval = rhs < lhs;");
        Ins(&R, func.body, "if (retval) {");
        Ins(&R, func.body, "    lhs = rhs;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return retval;");
    }
}

void amc::tfunc_Ctype_UpdateMax() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_UpdateMax($Partype &lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "bool retval = lhs < rhs;");
        Ins(&R, func.body, "if (retval) {");
        Ins(&R, func.body, "    lhs = rhs;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return retval;");
    }
}

// -----------------------------------------------------------------------------

// Emit optimal padded string comparison code
// to string OUT
// The fields being compared are FIELD in "lhs" and "rhs"
// The resulting value is written to "retval" which must be initially true.
static bool GenSmallstrEq(algo_lib::Replscope &R, amc::FField &field, cstring &out) {
    bool ret=false;
    amc::FSmallstr *smallstr = field.c_smallstr;
    // compare small strings bytewise
    if (smallstr && (smallstr->strtype == dmmeta_Strtype_strtype_rightpad || smallstr->strtype == dmmeta_Strtype_strtype_leftpad)) {
        int len = smallstr->length;
        // for big strings
        if (len>128) {
            int niter=(len/8);
            Set(R, "$niter", tempstr()<<niter);
            Ins(&R, out, "for (int i=0; i<$niter && retval; i++) {");
            Ins(&R, out, "    retval = ((u64*)lhs.$name)[i] == ((u64*)rhs.$name)[i];");
            Ins(&R, out, "}");
            len -= niter*8;
        }
        // remainder
        if (len>0) {
            Ins(&R, out, "retval = retval ",false);
            while (len>0) {
                Set(R, "$offset", tempstr()<< smallstr->length - len);
                if (len>=8) {
                    Ins(&R, out, "\n &&*(u64*)(lhs.$name+$offset) == *(u64*)(rhs.$name+$offset)",false);
                    len-=8;
                } else if (len >=4) {
                    Ins(&R, out, "\n &&*(u32*)(lhs.$name+$offset) == *(u32*)(rhs.$name+$offset)",false);
                    len-=4;
                } else if (len >=2) {
                    Ins(&R, out, "\n &&*(u16*)(lhs.$name+$offset) == *(u16*)(rhs.$name+$offset)",false);
                    len-=2;
                } else if (len >=1) {
                    Ins(&R, out, "\n &&*(u8*)(lhs.$name+$offset) == *(u8*)(rhs.$name+$offset)",false);
                    len--;
                }
            }
            Ins(&R,out,";");
        }
        ret=true;
    }
    return ret;
}

// Equality function for ctype
// The function proceeds field-by-field, using the following rules:
// - pad bytes are ignored
// - if a field has an Eq function defined (usually because its equality has been customized with the fcmp record)
//   we call out to the defined Eq function.
// - pointers and big-endian fields are compared bitwise for equality
// - any string fields are compared as regular strings
// - if no custom Eq function is defined on a field, but the field's type has an Eq function, it is used
// - for all other fields, c++ operator "==" is used. it better be defined
void amc::tfunc_Ctype_Eq() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;

    if (ctype.c_ccmp) {
        vrfy(zd_varlenfld_EmptyQ(ctype),tempstr()<< "comparison function of varlength records is not supported ("<<ctype.ctype<<")");
        amc::FFunc& opeq = amc::CreateCurFunc(true);
        AddRetval(opeq,"bool","retval","true");
        AddProtoArg(opeq, ByvalArgtype(ctype,false), "lhs");
        AddProtoArg(opeq, ByvalArgtype(ctype,false), "rhs");
        opeq.extrn = ctype.c_ccmp->extrn;
        opeq.inl = c_datafld_N(ctype) < 4;
        bool need_test = false;
        if (ctype.c_bltin) {
            Ins(&R, opeq.body, "retval = lhs == rhs;");
        } else {
            ind_beg(amc::ctype_c_datafld_curs, field,ctype) {
                tempstr test;
                if (PadQ(field)) {
                } else if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Eq"))) {
                    Set(R, "$name", name_Get(field));
                    Ins(&R, test, "retval = $name_Eq(lhs,rhs);");
                } else if (field.reftype == dmmeta_Reftype_reftype_Upptr || field.reftype == dmmeta_Reftype_reftype_Ptr) {// default for pointers
                    Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
                    Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
                    Set(R, "$Fldtype", field.p_arg->cpp_type);
                    Ins(&R, test, "retval = u64_Eq((u64)(void*)$a_val, (u64)(void*)$b_val);");
                } else if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {// default for small strings
                    Set(R, "$name", name_Get(field));
                    if (!GenSmallstrEq(R, field, test)) {
                        Ins(&R, test, "retval = algo::strptr_Eq($name_Getary(lhs), $name_Getary(rhs));");
                    }
                } else if (field.reftype == dmmeta_Reftype_reftype_Base) {
                    // fields have already been imported, ignore
                } else if (field.p_arg->c_ccmp) {// type-specific compare
                    Set(R, "$a_val", FieldvalExpr(&ctype, field, "lhs"));
                    Set(R, "$b_val", FieldvalExpr(&ctype, field, "rhs"));
                    Set(R, "$Fldtype", field.p_arg->cpp_type);
                    Ins(&R, test, "retval = $Fldtype_Eq($a_val, $b_val);");
                } else {
                    Set(R, "$name", name_Get(field));
                    Set(R,"$suffix",field.c_fbigend?"_be":"");
                    // hack: bigend fields do not need swapping to compare
                    Ins(&R, test, "retval = lhs.$name$suffix==rhs.$name$suffix;");
                }
                if (ch_N(test) > 0) {
                    if (need_test) {
                        Ins(&R, opeq.body, "if (!retval) {");
                        Ins(&R, opeq.body, "    return false;");
                        Ins(&R, opeq.body, "}");
                    }
                    opeq.body << test;
                    need_test = true;
                }
            }ind_end;
        }
    }
}

void amc::tfunc_Ctype_ToCmdline() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;

    bool has_print = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv) {
        has_print=true;
    }ind_end;

    if (has_print) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func, "tempstr", "ret", "");
        AddProtoArg(func, ByvalArgtype(ctype), "row");
        Ins(&R, func.comment, "Convenience function that returns a full command line");
        Ins(&R, func.comment, "Assume command is in a directory called bin");
        Ins(&R, func.body   , "ret << \"bin/$Name \";");
        Ins(&R, func.body   , "$Name_PrintArgv(row, ret);");
        Ins(&R, func.body   , "// inherit less intense verbose, debug options");
        Ins(&R, func.body   , "for (int i = 1; i < algo_lib::_db.cmdline.verbose; i++) {");
        Ins(&R, func.body   , "    ret << \" -verbose\";");
        Ins(&R, func.body   , "}");
        Ins(&R, func.body   , "for (int i = 1; i < algo_lib::_db.cmdline.debug; i++) {");
        Ins(&R, func.body   , "    ret << \" -debug\";");
        Ins(&R, func.body   , "}");
    }
}

// -----------------------------------------------------------------------------

// Used with command lines
void amc::tfunc_Ctype_NArgs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;

    bool has_read = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.read && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv) {
            has_read=true;
            break;
        }
    }ind_end;
    if (has_read) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Return # of command-line arguments that must follow this argument");
        Ins(&R, func.comment, "If FIELD is invalid, return -1");
        Ins(&R, func.proto  , Subst(R,"$Name_NArgs()"),false);
        AddRetval(func, "i32", "retval", "1");
        AddProtoArg(func, Subst(R,"$ns::FieldId"), "field");
        AddProtoArg(func, "algo::strptr&", "out_dflt");
        AddProtoArg(func, "bool*", "out_anon");
        Ins(&R, func.body   , "switch (field) {");
        ind_beg(ctype_c_field_curs,field,ctype) {
            int nargs=1;
            amc::FField *actualfield = &field;
            tempstr comment;
            if (field.c_falias) {
                actualfield=field.c_falias->p_srcfield;
            }
            Set(R, "$name", name_Get(field));
            Set(R, "$isanon", (field.c_anonfld ? "true" : "false"));
            if (actualfield->arg=="bool") {
                nargs=0;
                Set(R,"$emptyval","\"Y\"");
                comment<<"bool: no argument required but value may be specified as $name:Y";
            } else if (actualfield->c_fflag && actualfield->c_fflag->emptyval != "\"\"") {
                nargs=0;
                Set(R, "$emptyval",actualfield->c_fflag->emptyval);
                comment<<"dmmeta.fflag: emptyval specified; no argument required but value may be specified as $name:value";
            }
            Set(R,"$comment",comment);
            Ins(&R, func.body, "case $ns_FieldId_$name: { // $comment");
            Ins(&R, func.body, "    *out_anon = $isanon;");
            if (nargs==0) {
                Ins(&R, func.body, "    retval=0;");
                Ins(&R, func.body, "    out_dflt=$emptyval;");
            }
            Ins(&R, func.body, "} break;");
        }ind_end;
        Ins(&R, func.body   , "default:");
        Ins(&R, func.body   , "    retval=-1; // unrecognized");
        Ins(&R, func.body   , "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Print() {
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.print) {
            GenPrint(ctype,cfmt);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Read() {
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    ind_beg(amc::ctype_zs_cfmt_curs,cfmt,ctype) {
        if (cfmt.read) {
            GenRead(ctype,cfmt);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_EqEnum() {
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    amc::FField *field = c_datafld_N(ctype) == 1 ? c_datafld_Find(ctype,0) : NULL;
    bool doit = field
        && c_fconst_N(*field)
        && !amc::FieldStringQ(*field)
        && (ctype.c_ccmp && ctype.c_ccmp->genop);
    if (doit) {
        algo_lib::Replscope &R = amc::_db.genctx.R;
        Set(R, "$Get", FieldvalExpr(&ctype,*field,""));
        Set(R, "$Enumtype", Enumtype(*c_datafld_Find(ctype,0)));
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl = true;
        func.member=true;
        func.comment = "define enum comparison operator to avoid ambiguity";
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "operator ==($Enumtype rhs) const", false);
        // field type may be unsigned, in which case signed<>unsigned comparison
        // warning fires. insert an extra cast to Enumtype to avoid it.
        Ins(&R, func.body, "return $Enumtype($Get) == rhs;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_GetAnon() {
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    bool need_anon = false;
    ind_beg(amc::ctype_zs_cfmt_curs,cfmt,ctype) if (cfmt.read
                                                    && !(cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw
                                                         || cfmt.printfmt == dmmeta_Printfmt_printfmt_Extern)) {
        need_anon = true;
    }ind_end;

    int n_anon = c_anonfld_N(ctype);
    if (need_anon && n_anon > 0) {
        algo_lib::Replscope &R = amc::_db.genctx.R;
        amc::FFunc& getanon = amc::CreateCurFunc();
        Ins(&R, getanon.ret  , "algo::strptr", false);
        Ins(&R, getanon.proto, "$Name_GetAnon($Partype &parent, i32 idx)",false);
        MaybeUnused(getanon, "parent");//must call before "return"
        Ins(&R, getanon.body   , "switch(idx) {");
        int idx=0;
        Set(R,"$dfltanon","algo::strptr()");
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.c_anonfld) {
            Set(R, "$fstr"   , strptr(tempstr() << "strptr(\"" << name_Get(field) << "\", "<<ch_N(name_Get(field))<<")"));
            Set(R, "$idx"   , tempstr()<<idx);
            // Tary must be last arg
            if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                Set(R,"$dfltanon","$fstr");
                break;
            } else {
                Ins(&R, getanon.body, "case($idx): return $fstr;");
            }
            idx++;
        }ind_end;
        Ins(&R, getanon.body    , "default: return $dfltanon;");
        Ins(&R, getanon.body    , "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_GetMsgLength() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_lenfld) {
        amc::FFunc& func = amc::CreateCurFunc(true, "GetMsgLength");
        AddProtoArg(func, amc::ConstRefto(ctype.cpp_type)<<" ", "parent");
        AddRetval(func, "i32", "", "");
        func.glob = true;
        Set(R, "$LenExpr", LengthExpr(ctype,"const_cast<$Partype&>(parent)"));
        Ins(&R, func.body , "return $LenExpr;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_GetMsgMemptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_lenfld) {
        amc::FFunc& func = amc::CreateCurFunc(true, "GetMsgMemptr"); {
            AddProtoArg(func, amc::ConstRefto(ctype.cpp_type)<<" ", "row");
            AddRetval(func, "algo::memptr", "", "");
            func.glob = true;
            Set(R, "$LenExpr", LengthExpr(ctype,"const_cast<$Partype&>(row)"));
            Ins(&R, func.body , "return algo::memptr((u8*)&row, $LenExpr);");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Ctype2() {
}

// -----------------------------------------------------------------------------

// Generate constructor.
void amc::tfunc_Ctype2_Ctor() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    // global variables are initialized separately, the order must be
    // controlled.
    if (!GlobalQ(ctype) && !ctype.c_cextern) {
        amc::FFunc *init = amc::ind_func_Find(tempstr() << ctype.ctype << "..Init");
        amc::FFunc &ctor = amc::CreateCurFunc();
        ctor.member=true;
        ctor.isprivate = FindPool(ctype)!=NULL;
        ctor.inl=true;

        Ins(&R, ctor.proto, "$Name()", false);
        // call init function
        if (init) {
            Ins(&R, ctor.body, "$ns::$Name_Init(*this);");
        }
        // produce coverity annotation for intentionally uninitialized fields
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            bool has_uninitfld = false;
            has_uninitfld     |= field.reftype == dmmeta_Reftype_reftype_Fbuf;
            has_uninitfld     |= field.reftype == dmmeta_Reftype_reftype_Inlary
                && field.c_inlary->max > field.c_inlary->min;
            if (has_uninitfld) {
                Set(R,"$field",field.field);
                Set(R,"$reftype",field.reftype);
                Ins(&R, ctor.body, "    // added because $field ($reftype) does not need initialization");
                Ins(&R, ctor.body, "    // coverity[uninit_member]"); // applies to '}', should be last
                break;
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype2_FieldwiseCtor() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    int nfield = 0;
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (PassFieldViaArgQ(field,ctype)) {
        nfield++;
    }ind_end;
    if (nfield > 0 && ctype.c_cpptype && ctype.c_cpptype->ctor) {
        int n_args = 0;
        amc::FFunc &func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$Name()", false);
        func.isexplicit=true;
        func.member=true;
        func.inl=true;
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (PassFieldViaArgQ(field,ctype)) {
            AddProtoArg(func, Argtype(field), tempstr()<<"in_"<<name_Get(field));
            ++n_args;
            if (!FixaryQ(field) && !field.c_fbigend) {
                Set(R, "$name", name_Get(field));
                ary_Alloc(func.initializer) << Subst(R,"$name(in_$name)");
            }
        }ind_end;
        ind_beg(amc::ctype_c_field_curs, fld,ctype) {
            bool val = ValQ(fld);
            amc::FLenfld *lenfld=GetLenfld(fld);
            if (val && (FixaryQ(fld) || fld.c_tary) && !PadQ(fld)) {
                vrfy_(!fld.c_fbigend);
                Set(R, "$name", name_Get(fld));
                Ins(&R, func.body, "    $name_Setary(*this, in_$name);");
            } else if (val && fld.c_typefld && ctype.c_msgtype) {
                Set(R, "$Msgtype", ctype.c_msgtype->type.value);
                Set(R, "$assign", amc::AssignExpr(fld, "*this", "$Msgtype", true));
                Ins(&R, func.body, "    $assign;");
            } else if (val && lenfld && ctype.c_msgtype) {
                Set(R, "$extralen", tempstr() << lenfld->extra);
                Set(R, "$assign", amc::AssignExpr(fld, "*this", "ssizeof(*this) + ($extralen)", true));
                Ins(&R, func.body, "    $assign;");
            } else if (fld.c_fbigend) {
                Set(R, "$name", name_Get(fld));
                Ins(&R, func.body, "    $name_Set(*this,in_$name);");
            }
        }ind_end;
    }
}

void amc::tfunc_Ctype2_EnumCtor() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    int n_args = 0;
    amc::FField *single_arg=NULL;
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (PassFieldViaArgQ(field,ctype)) {
        ++n_args;
        single_arg = &field;
    }ind_end;
    if (n_args == 1 && c_fconst_N(*single_arg) && !FieldStringQ(*single_arg)) {
        amc::FFunc &func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$Name()", false);
        AddProtoArg(func, Enumtype(*single_arg), "arg");
        func.member=true;
        func.inl=true;
        Ins(&R, func.body, tempstr() << AssignExpr(*single_arg, "*this", "arg", true) << ";");
    }
}

// Generator copy constructor or assignment operator
// (the two functions are very similar)
void amc::GenCopyCtorOrAssignOp(bool copyctor) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (!ctype.c_cextern) {
        bool cancopy = !GlobalQ(ctype) && !ctype.c_cextern && ctype.n_xref==0;
        bool needcopy = ctype.n_xref>0;
        bool hasholes = false;
        int n_tary = 0;
        cstring reason;
        ind_beg(amc::ctype_c_field_curs, fld,ctype) {
            if ((fld.c_inlary && fld.c_inlary->min < fld.c_inlary->max)) {
                hasholes=true;
            }
            if (fld.c_smallstr) {
                needcopy=true;
            }
            if (fld.c_smallstr && fld.c_smallstr->strtype == dmmeta_Strtype_strtype_rpascal) {
                hasholes=true;
            }
            if (fld.c_tary) {
                n_tary++;
            }
            // only need copy the struct if it has arrays
            if (!amc::FixaryQ(fld) && (fld.reftype == dmmeta_Reftype_reftype_Tary || fld.reftype == dmmeta_Reftype_reftype_Inlary)) {
                needcopy=true;
            }
            if (fld.c_fbuf != NULL || fld.reftype == dmmeta_Reftype_reftype_Varlen || fld.reftype == dmmeta_Reftype_reftype_Opt) {
                cancopy=false;
                reason<<"field "<<fld.field<<" prevents copy" << eol;
            } else if (fld.p_reftype->isval) {
                if (CopyPrivQ(*fld.p_arg)) {
                    cancopy=false;
                    reason <<"value field "<<fld.field<<" is not copiable"<<eol;
                }
            } else if (fld.reftype == dmmeta_Reftype_reftype_Regx || fld.reftype == dmmeta_Reftype_reftype_RegxSql) {
            } else if (!fld.p_reftype->cancopy) {
                cancopy = false;
                reason<<"reftype "<<fld.reftype<<" of "<<fld.field<<" prohibits copy" << eol;
            } else if (fld.c_xref != NULL) {
                cancopy=false;
                reason<<"x-reference on "<<fld.field<<" prevents copy" << eol;
            } else if (fld.c_cascdel != NULL) {
                cancopy=false;;
                reason<<"cascdel on "<<fld.field<<" prevents copy" << eol;
            } else if (fld.c_fcleanup != NULL) {
                cancopy=false;
                reason<<"user-defined fcleanup on "<<fld.field<<" prevents copy" << eol;
            } else if (fld.c_fuserinit != NULL) {
                cancopy = false;
                reason<<"user-defined fuserinit on "<<fld.field<<" prevents copy" << eol;
            }
        }ind_end;
        // don't memcpy large structs with holes in them
        if (hasholes) {
            needcopy=true;
        }
        if (needcopy) {
            amc::FFunc &func = amc::CreateCurFunc();
            func.member=true;
            func.inl=c_field_N(ctype)<10 && n_tary==0;
            if (copyctor) {
                Ins(&R, func.proto, "$Name(const $Partype &rhs)", false);
            } else {
                Ins(&R, func.proto, "operator =(const $Partype &rhs)", false);
                AddRetval(func, Subst(R,"$Partype&"), "", "");
            }
            if (!cancopy) {
                if (!GlobalQ(ctype)) {
                    func.comment << reason;
                }
                func.deleted=true;
            } else if (ctype.plaindata && !hasholes) {
                Ins(&R, func.body, "// type is plaindata, with no holes, copying as memory");
                Ins(&R, func.body, "memcpy(this,&rhs,sizeof($Partype));");
            } else {
                ind_beg(amc::ctype_c_field_curs, fld,ctype) if (!FldfuncQ(fld)) {
                    Set(R, "$name", name_Get(fld));
                    if (PadQ(fld)) {
                        // ignore
                    } else if (fld.reftype == dmmeta_Reftype_reftype_Base) {
                        // nothing to do
                    } else if (fld.c_fbigend) {
                        if (copyctor) {
                            Ins(&R, ary_Alloc(func.initializer), "$name_be(rhs.$name_be)",false);
                        } else {
                            Ins(&R, func.body, "$name = rhs.$name;");
                        }
                    } else if (fld.reftype == dmmeta_Reftype_reftype_Val
                               || fld.reftype == dmmeta_Reftype_reftype_Ptr
                               || fld.reftype == dmmeta_Reftype_reftype_Upptr
                               || fld.reftype == dmmeta_Reftype_reftype_Regx
                               || fld.reftype == dmmeta_Reftype_reftype_RegxSql) {
                        if (copyctor) {
                            Ins(&R, ary_Alloc(func.initializer), "$name(rhs.$name)",false);
                        } else {
                            Ins(&R, func.body, "$name = rhs.$name;");
                        }
                    } else if (fld.c_smallstr) {
                        if (fld.c_smallstr->strtype == dmmeta_Strtype_strtype_rpascal) {
                            Ins(&R, func.body, "memcpy($name, rhs.$name, rhs.n_$name);");
                            Ins(&R, func.body, "n_$name = rhs.n_$name;");
                        } else {
                            Set(R,"$max_length",tempstr()<<fld.c_smallstr->length);
                            Ins(&R, func.body, "memcpy($name, rhs.$name, $max_length);");
                        }
                    } else if (fld.c_inlary || fld.c_tary) {
                        // use field's Init function to initialize it before copying
                        if (copyctor) {
                            if (amc::FFunc *init = amc::ind_func_Find(tempstr() << fld.field << ".Init")) {
                                if (init->ismacro) {
                                    cstring body = init->body;
                                    Replace(body,Subst(R,"$parname."),"");
                                    func.body << body;
                                } else {
                                    Ins(&R, func.body, "$name_Init(*this);");
                                }
                            }
                        }
                        Ins(&R, func.body, "$name_Setary(*this, $name_Getary(const_cast<$Partype&>(rhs)));");
                    } else {
                        vrfy(0,tempstr()<<"failed to generate copy constructor, field="<<fld.field);
                    }
                }ind_end;
            }
            if (!copyctor) {
                Ins(&R, func.body, "return *this;");
            }
            MaybeUnused(func, "rhs");
        }
    }
}

void amc::tfunc_Ctype2_CopyCtor() {
    GenCopyCtorOrAssignOp(true);
}

void amc::tfunc_Ctype_AssignOp() {
    GenCopyCtorOrAssignOp(false);
}

void amc::tfunc_Ctype2_Dtor() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    amc::FFunc *uninit = amc::ind_func_Find(tempstr()<<ctype.ctype<<"..Uninit");
    // global variables are initialized separately, the order must be
    // controlled.
    if (!GlobalQ(ctype) && !ctype.c_cextern && uninit && !uninit->disable) {
        amc::FFunc &func = amc::CreateCurFunc();
        func.member=true;
        func.isprivate = FindPool(ctype)!=0;
        func.inl=true;

        Ins(&R, func.proto, "~$Name()", false);
        Ins(&R, func.body, "$ns::$Name_Uninit(*this);");
    }
}

void amc::tfunc_Ctype_EqOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator ==(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return $Partype_Eq(const_cast<$Partype&>(*this),const_cast<$Partype&>(rhs));");
    }
}

void amc::tfunc_Ctype_NeOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator !=(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return !$Partype_Eq(const_cast<$Partype&>(*this),const_cast<$Partype&>(rhs));");
    }
}

void amc::tfunc_Ctype_LtOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop && ctype.c_ccmp->order) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator <(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return $Partype_Lt(const_cast<$Partype&>(*this),const_cast<$Partype&>(rhs));");
    }
}

void amc::tfunc_Ctype_GtOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop && ctype.c_ccmp->order) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator >(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return $Partype_Lt(const_cast<$Partype&>(rhs),const_cast<$Partype&>(*this));");
    }
}

void amc::tfunc_Ctype_LeOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop && ctype.c_ccmp->order) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator <=(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return !$Partype_Lt(const_cast<$Partype&>(rhs),const_cast<$Partype&>(*this));");
    }
}

void amc::tfunc_Ctype_GeOp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FCtype &ctype      = *amc::_db.genctx.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->genop && ctype.c_ccmp->order) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.proto, "operator >=(const $Partype &rhs) const", false);
        AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return !$Partype_Lt(const_cast<$Partype&>(*this),const_cast<$Partype&>(rhs));");
    }
}

void amc::tfunc_Ctype_EqOpAryptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    bool single_smallstr = c_datafld_N(ctype)==1 && c_datafld_Find(ctype,0)->reftype==dmmeta_Reftype_reftype_Smallstr;
    if (ctype.c_ccmp && single_smallstr) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        AddRetval(func,"bool","","");
        Ins(&R, func.proto, "operator ==(const algo::aryptr<char> &rhs) const", false);
        ind_beg(amc::ctype_c_datafld_curs, field,ctype) {
            if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {
                Set(R, "$name", name_Get(field));
                Ins(&R, func.body, "return algo::strptr_Eq($name_Getary(*this), rhs);");
            }
        }ind_end;
    }
}
