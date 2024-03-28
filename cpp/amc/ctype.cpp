// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/ctype.cpp -- Ctype code generators
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Ctype() {
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Uninit() {
    // de-initialization order:
    // - cascdel fields  in reverse order
    // - any unred_body text created by generators.
    // - unref fields in reverse order (remove x-references)
    // - user-specified cleanup in reverse order
    // - un-init fields in reverse order
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    tempstr uninit_text;
    tempstr unref_text;
    tempstr refname = amc::Refname(ctype);
    bool inl = true;
    bool glob = GlobalQ(ctype);
    Set(R, "$Ctype"  , ctype.ctype);

    // Cascade-delete fields
    amc::c_tempfield_RemoveAll();
    ind_beg(ctype_c_field_curs,field,ctype) {
        c_tempfield_Insert(field);
    }ind_end;
    rrep_(i, amc::c_tempfield_N()) {
        amc::FField &field = *amc::c_tempfield_Find(i);
        if (field.c_cascdel) {
            Set(R, "$field", field.field);
            Set(R, "$name", name_Get(field));
            Set(R, "$pararg", glob ? strptr("") : refname);
            Ins(&R, unref_text, "$name_Cascdel($pararg); // dmmeta.cascdel:$field");
        }
    }

    // Grab parent's Unref text
    if (amc::FFunc* unref = amc::ind_func_Find(Subst(R,"$Ctype..Unref"))) {
        unref_text << unref->body;
    }

    // Unref fields in reverse order
    rrep_(i, amc::c_tempfield_N()) {
        amc::FField &field = *c_tempfield_Find(i);
        if (field.c_fcleanup) {
            Set(R, "$field", field.field);
            Set(R, "$name", name_Get(field));
            Set(R, "$pararg", glob ? strptr("") : refname);
            Ins(&R, unref_text, "$name_Cleanup($pararg); // dmmeta.fcleanup:$field");
        }
        if (amc::FFunc *func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Unref"))) {
            if (ch_N(func->body)) {
                Set(R, "$field", field.field);
                Ins(&R, unref_text, "");
                Ins(&R, unref_text, tempstr()<<"// $field.Unref ("<<field.reftype<<")  //"<<field.comment);
                unref_text << func->body;
                inl = inl && func->inl;
            }
        }
    }

    // Uninit fields in reverse order (release memory) -- ALL after user-specified cleanup
    rrep_(i, amc::c_tempfield_N()) {
        amc::FField &field = *c_tempfield_Find(i);
        if (amc::FFunc *func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Uninit"))) {
            if (ch_N(func->body)) {
                Set(R, "$field", field.field);
                Ins(&R, uninit_text, "");
                Ins(&R, uninit_text, tempstr()<<"// $field.Uninit ("<<field.reftype<<")  //"<<field.comment);
                uninit_text << func->body;
                inl = inl && func->inl;
            }
        }
    }

    if (ch_N(unref_text)) {
        inl = false;
    }

    if (ch_N(uninit_text) || ch_N(unref_text) || glob) {
        Set(R, "$thisparname", glob ? strptr("_db") : refname);
        Set(R, "$Parent" , glob ? strptr("")    : strptr(tempstr() << ctype.cpp_type << "& " << refname));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name"   , name_Get(ctype));
        amc::FFunc& uninit = amc::CreateCurFunc();
        Ins(&R, uninit.ret  , "void", false);
        Ins(&R, uninit.proto, "$Name_Uninit($Parent)", false);
        Ins(&R, uninit.body, "$Cpptype &row = $thisparname; (void)row;");
        // init function MUST be inline at the moment.
        uninit.inl = inl && (!ctype.c_init || ctype.c_init->inl) && c_tempfield_N() < 10;
        uninit.body << unref_text;
        uninit.body << uninit_text;
    };
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
static bool ComputeAccess(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FXref &xref, amc::FFunc &func, amc::FGenXref &frame, bool check_null) {
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

static void Ctype_Xref(amc::FCtype &ctype) {
    algo_lib::Replscope R;
    amc::FNs &ns = *ctype.p_ns;
    amc::FField &inst = *FirstInst(ctype);
    amc::FGenXref frame;
    Set(R, "$Cpptype"  , ctype.cpp_type);
    Set(R, "$inst"  , inst.field);
    Set(R, "$name" , name_Get(inst));
    Set(R, "$Name" , name_Get(ctype));
    Set(R, "$Ctype" , ctype.ctype);
    // xref function for parent field??
    amc::FFunc& xrefmaybe = amc::ind_func_GetOrCreate(Subst(R,"$inst.XrefMaybe"));
    Ins(&R, xrefmaybe.comment, "Insert row into all appropriate indices. If error occurs, store error");
    Ins(&R, xrefmaybe.comment, "in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.");
    Ins(&R, xrefmaybe.ret  , "bool", false);
    Ins(&R, xrefmaybe.body , "bool retval = true;");
    if (!GlobalQ(ctype)) {
        Ins(&R, xrefmaybe.body , "(void)row;");
    }
    Ins(&R, xrefmaybe.proto, (GlobalQ(ctype) ? "$name_XrefMaybe()" : "$name_XrefMaybe($Cpptype &row)"), false);
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
    Ins(&R, xrefmaybe.body, "return retval;");
}

// -----------------------------------------------------------------------------

static void Ctype_Unref(amc::FCtype &ctype) {
    algo_lib::Replscope R;
    amc::FNs &ns = *ctype.p_ns;
    amc::FField &inst = *FirstInst(ctype);
    amc::FGenXref frame;
    Set(R, "$Cpptype"  , ctype.cpp_type);
    Set(R, "$inst"  , inst.field);
    Set(R, "$name" , name_Get(inst));
    Set(R, "$Name" , name_Get(ctype));
    Set(R, "$Ctype" , ctype.ctype);
    amc::FFunc& unref = amc::ind_func_GetOrCreate(Subst(R,"$Ctype..Unref"));
    unref.priv =true;
    unref.ismacro=true;
    Ins(&R, unref.comment, "Remove row from all global and cross-reference indices");
    Ins(&R, unref.comment, "Row may be only partially x-referenced with any parents.");
    Ins(&R, unref.ret  , "void", false);
    Ins(&R, unref.proto, "$Name_Unref($Cpptype &row)", false);
    ind_beg(amc::ctype_zs_xref_curs, xref, ctype) if (xref.p_field->reftype != dmmeta_Reftype_reftype_Upptr) {
        Set(R, "$xrefname", name_Get(xref));
        if (ns.c_globfld && xref.p_field->p_ctype == ns.c_globfld->p_ctype) {
            Ins(&R, unref.body, "$xrefname_Remove(row); // remove $name from index $xrefname");
        } else {
            ComputeAccess(R,ctype,xref,unref,frame, false);
            Ins(&R, unref.body, "if (p_$parent)  {");
            Ins(&R, unref.body, "    $xrefname_Remove(*p_$parent, row);// remove $name from index $xrefname");
            Ins(&R, unref.body, "}");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_XrefMaybe() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    amc::FNs &ns = *ctype.p_ns;

    if (FirstInst(ctype) && ns.c_globfld) {
        Ctype_Xref(ctype);
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Unref() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    amc::FNs &ns = *ctype.p_ns;

    if (FirstInst(ctype) && ns.c_globfld) {
        Ctype_Unref(ctype);
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_Hash() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    amc::FNs &ns = *ctype.p_ns;

    if (ctype.c_chash) {
        vrfy(!ctype.c_varlenfld,tempstr()<< "hash function of varlength records is not supported ("<<ctype.ctype<<")");
        Set(R, "$Cpptype", (ctype.c_cpptype && ctype.c_cpptype->cheap_copy ? strptr(ctype.cpp_type) : strptr(tempstr() << "const " <<  ctype.cpp_type << " &")));
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));
        ns.nhash++;

        amc::FFunc& hash = amc::CreateCurFunc();
        Ins(&R, hash.ret  , "u32", false);
        Ins(&R, hash.proto, "$Name_Hash(u32 prev, $Cpptype rhs)", false);
        hash.extrn = ctype.c_chash->hashtype == dmmeta_Hashtype_hashtype_Extern;
        hash.inl = c_datafld_N(ctype) < 5;
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            if (PadQ(field)) {
            } else if (FldfuncQ(field)) {
            } else if (field.c_smallstr) {
                hash.inl=false;
                Set(R, "$name", name_Get(field));
                Ins(&R, hash.body, "algo::strptr $name_strptr = $name_Getary(rhs);");
                Ins(&R, hash.body, "prev = ::strptr_Hash(prev, $name_strptr);");
            } else if (field.reftype == dmmeta_Reftype_reftype_Ptr) {
                Set(R, "$name", name_Get(field));
                Ins(&R, hash.body, "prev = u64_Hash(prev, u64(rhs.$name));");
            } else if (field.reftype == dmmeta_Reftype_reftype_Upptr) {
                Set(R, "$name", name_Get(field));
                Ins(&R, hash.body, "prev = u64_Hash(prev, u64(rhs.$name));");
            } else if (FixaryQ(field)) {
                Set(R, "$Fldname", name_Get(*field.p_arg));
                Set(R, "$name", name_Get(field));
                Set(R, "$width", tempstr() << WidthMin(field));
                Ins(&R, hash.body, "frep_(i,$width) prev = $Fldname_Hash(prev, rhs.$name_elems[i]);");
            } else if (ValQ(field)) {
                Set(R, "$Fldname", name_Get(*field.p_arg));
                Set(R, "$gethashfld", FieldvalExpr(&ctype, field,"rhs"));
                Ins(&R, hash.body, tempstr()<<"prev = $Fldname_Hash(prev, $gethashfld);");
            }
        }ind_end;
        Ins(&R, hash.body, "return prev;");
    }
}

// Comparison function for ctype
// In amc, equality is not strictly a property of a type; it can be overriden on a per-field basis
// For example, the same field (algo.Smallstr100 for instance) can be made case-insensitive
// for the purposes of comparison within some ctype
// That's why the Cmp function for ctype does not call out fo
void amc::tfunc_Ctype_Cmp() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp) {
        vrfy(!ctype.c_varlenfld,tempstr()<< "comparison function of varlength records is not supported ("<<ctype.ctype<<")");
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));

        amc::FFunc& cmp = amc::CreateCurFunc();
        Ins(&R, cmp.ret  , "i32", false);
        Ins(&R, cmp.proto, "$Name_Cmp($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        cmp.extrn = ctype.c_ccmp->extrn;
        cmp.inl = c_datafld_N(ctype) < 4;
        Ins(&R, cmp.body    , "i32 retval = 0;");
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
                // compare bases???
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
        Ins(&R, cmp.body, "return retval;");
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
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    if (ctype.c_ccmp && ctype.c_ccmp->order) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));

        amc::FFunc& oplt = amc::CreateCurFunc();
        Ins(&R, oplt.ret  , "bool", false);
        Ins(&R, oplt.proto, "$Name_Lt($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        oplt.extrn = ctype.c_ccmp->extrn;
        oplt.inl = true;
        // Lt function is faster than Cmp because there are fewer calculations to do
        // But if there is more than one field the advantage goes away because if a.x is not less
        // than b.x, you would have to check whether a.x==b.x before proceeding to the next field
        // So if there is more than one field, we revert to the generic Cmp.
        if (c_datafld_N(ctype) == 1) { // special case -- single field.
            Ctype_Lt_SingleField(R,ctype,oplt);
        } else {
            Ins(&R, oplt.body    , "return $Name_Cmp(lhs,rhs) < 0;");
        }
    }
}

void amc::tfunc_Ctype_Init() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    Set(R, "$ns", ns_Get(ctype));

    tempstr text;
    tempstr refname = amc::Refname(ctype);
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
            Set(R, "$pararg", refname);
            Set(R, "$name", name_Get(field));
            if (glob) {
                Ins(&R, text, "Userinit(); // dmmeta.fuserinit:$field");
            } else {
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
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->order) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_Update($Cpptype &lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "bool ret = !$Name_Eq(lhs, rhs); // compare values");
        Ins(&R, func.body, "if (ret) {");
        Ins(&R, func.body, "    lhs = rhs; // update");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return ret;");
    }
}

void amc::tfunc_Ctype_Min() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "$Cpptype", false);
        Ins(&R, func.proto, "$Name_Min($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "return lhs < rhs ? lhs : rhs;");
    }
}

void amc::tfunc_Ctype_Max() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "$Cpptype", false);
        Ins(&R, func.proto, "$Name_Max($ByvalArgtype lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "return rhs < lhs ? lhs : rhs;");
    }
}

void amc::tfunc_Ctype_UpdateMin() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_UpdateMin($Cpptype &lhs, $ByvalArgtype rhs)", false);
        Ins(&R, func.body, "bool retval = rhs < lhs;");
        Ins(&R, func.body, "if (retval) {");
        Ins(&R, func.body, "    lhs = rhs;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return retval;");
    }
}

void amc::tfunc_Ctype_UpdateMax() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_ccmp && ctype.c_ccmp->minmax) {
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Name_UpdateMax($Cpptype &lhs, $ByvalArgtype rhs)", false);
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
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    if (ctype.c_ccmp) {
        vrfy(!ctype.c_varlenfld,tempstr()<< "comparison function of varlength records is not supported ("<<ctype.ctype<<")");
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Ctype", ctype.ctype);
        amc::FFunc& opeq = amc::CreateCurFunc(true);
        AddRetval(opeq,"bool","retval","true");
        AddProtoArg(opeq, ByvalArgtype(ctype,true), "lhs");
        AddProtoArg(opeq, ByvalArgtype(ctype,true), "rhs");
        opeq.extrn = ctype.c_ccmp->extrn;
        opeq.inl = c_datafld_N(ctype) < 4;
        bool need_test = false;
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


void amc::tfunc_Ctype_EqStrptr() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    bool single_smallstr = c_datafld_N(ctype)==1 && c_datafld_Find(ctype,0)->reftype==dmmeta_Reftype_reftype_Smallstr;

    if (ctype.c_ccmp && single_smallstr) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func,"bool","","");
        AddProtoArg(func, ByvalArgtype(ctype,true), "lhs");
        AddProtoArg(func, "const algo::strptr&", "rhs");
        ind_beg(amc::ctype_c_datafld_curs, field,ctype) {
            if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {// default for small strings
                Set(R, "$name", name_Get(field));
                Ins(&R, func.body, "return algo::strptr_Eq($name_Getary(lhs), rhs);");
            } else {
                vrfy(0,"error");
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_PrintArgv() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    bool has_print = HasArgvPrintQ(ctype);
    bool gnu = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_ArgvGnu) {
            gnu=true;
        }
    }ind_end;

    if (has_print) {
        Set(R, "$Cpptype",    ctype.cpp_type);
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$ByvalArgtype", ByvalArgtype(ctype));
    }

    if (has_print) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            vrfy(field.reftype == dmmeta_Reftype_reftype_Val
                 || field.reftype == dmmeta_Reftype_reftype_Regx
                 || field.reftype == dmmeta_Reftype_reftype_Tary
                 ,Subst(R,"Only field reftype:Val,Pkey,RegxSql,Tary are supported for $Ctype.PrintArgv"));
            vrfy(!field.c_fbigend,Subst(R,"Big-endian is not supported for $Ctype.PrintArgv"));
        }ind_end;
    }

    if (has_print) {
        amc::FFunc& prnargv = amc::CreateCurFunc();
        Ins(&R, prnargv.ret    , "void",false);
        Ins(&R, prnargv.proto  , "$Name_PrintArgv($ByvalArgtype row, algo::cstring &str)",false);
        Ins(&R, prnargv.comment, "print command-line args of $Cpptype to string  -- cprint:$Ctype.Argv");
        Ins(&R, prnargv.body   , "algo::tempstr temp;");
        Ins(&R, prnargv.body   , "(void)temp;");
        Ins(&R, prnargv.body   , "(void)row;");
        Ins(&R, prnargv.body   , "(void)str;");
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (!FldfuncQ(field)) {
            amc::FCtype& valtype = *(field).p_arg;
            if (gnu) {
                if (ch_N(name_Get(field)) > 1) {
                    Set(R, "$Attr", tempstr() << "--" << name_Get(field) << " ");
                } else {
                    Set(R, "$Attr", tempstr() << "-" << name_Get(field) << " ");
                }
            } else {
                Set(R, "$Attr", tempstr()<<"-"<<name_Get(field)<<":");
            }
            Set(R, "$name", name_Get(field));
            if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                Set(R, "$Ftype", name_Get(valtype));
                Ins(&R, prnargv.body, "ind_beg($Name_$name_curs,value,row) {");
                Ins(&R, prnargv.body, "    ch_RemoveAll(temp);");
                Ins(&R, prnargv.body, "    $Ftype_Print(value, temp);");
                Ins(&R, prnargv.body, "    str << \" $Attr\";");
                Ins(&R, prnargv.body, "    strptr_PrintBash(temp,str);");
                Ins(&R, prnargv.body, "}ind_end;");
            } else {
                tempstr text;
                Ins(&R, text,"ch_RemoveAll(temp);");
                // TODO: this function must be shared with the Tuple version!
                if (amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
                    if (func->ismacro) {// inline the function!
                        Ins(&R, text, func->body, false);
                    } else {
                        Set(R, "$fns", ns_Get(*field.p_ctype));
                        Ins(&R, text, "$fns::$name_Print(const_cast<$Cpptype&>(row), temp);");
                    }
                } else {
                    Set(R, "$Ftype", name_Get(valtype));
                    Ins(&R, text,"$Ftype_Print(row.$name, temp);");
                }
                Ins(&R, text,"str << \" $Attr\";");
                Ins(&R, text,"strptr_PrintBash(temp,str);");
                bool canskip = !field.c_anonfld; // anonymous fields must be printed or meaning of command can change
                prnargv.body << CheckDfltExpr(field,text,canskip);
            }
        }ind_end;
    }
}

void amc::tfunc_Ctype_ToCmdline() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    bool has_print = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv) {
        has_print=true;
    }ind_end;

    if (has_print) {
        Set(R, "$Name", name_Get(ctype));
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
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;

    bool has_read = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.read && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv) {
            has_read=true;
            break;
        }
    }ind_end;
    if (has_read) {
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$ns", ns_Get(ctype));

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
            Ins(&R, func.body, "case $ns_FieldId_$name: { // $comment");
            Ins(&R, func.body, "    *out_anon = $isanon;");
            if (nargs==0) {
                Set(R,"$comment",comment);
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
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_String) {
            GenPrint(ctype,cfmt);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_FmtJson() {
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    GenFmtJson(ctype);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_EqEnum() {
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    amc::FField *field = c_datafld_N(ctype) == 1 ? c_datafld_Find(ctype,0) : NULL;
    // always generate EqEnum function if field has fconsts.
    // TODO: generate EqEnum for field, then promote to a ctype-wide EqEnum
    // if only one exists.
    // NOTE: do not look for ccmp; comparison with enum is generated because the
    // enum exists.
    bool doit = field
        && c_fconst_N(*field)
        && !amc::FieldStringQ(*field)
        && (ctype.c_ccmp && ctype.c_ccmp->genop);
    if (doit) {
        algo_lib::Replscope R;
        Set(R, "$Get", FieldvalExpr(&ctype,*field,"lhs"));
        Set(R, "$ByvalArgtype", amc::ByvalArgtype(ctype));
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));

        Set(R, "$Enumtype", Enumtype(*c_datafld_Find(ctype,0)));
        amc::FFunc& opeqenum = amc::CreateCurFunc();
        opeqenum.comment = "define enum comparison operator to avoid ambiguity";
        Ins(&R, opeqenum.ret  , "bool", false);
        Ins(&R, opeqenum.proto, "$Name_EqEnum($ByvalArgtype lhs, $Enumtype rhs)", false);
        opeqenum.glob = true;
        opeqenum.inl = true;
        // field type may be unsigned, in which case signed<>unsigned comparison
        // warning fires. insert an extra cast to Enumtype to avoid it.
        Ins(&R, opeqenum.body, "return $Enumtype($Get) == rhs;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_GetAnon() {
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    bool need_anon = false;
    ind_beg(amc::ctype_zs_cfmt_curs,cfmt,ctype) if (cfmt.read
                                                    && !(cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw
                                                         || cfmt.printfmt == dmmeta_Printfmt_printfmt_Extern)) {
        need_anon = true;
    }ind_end;

    int n_anon = c_anonfld_N(ctype);
    if (need_anon && n_anon > 0) {
        algo_lib::Replscope R;
        Set(R, "$Ctype", ctype.ctype);
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$Cpptype", ctype.cpp_type);
        amc::FFunc& getanon = amc::CreateCurFunc();
        Ins(&R, getanon.ret  , "algo::strptr", false);
        Ins(&R, getanon.proto, "$Name_GetAnon($Cpptype &parent, i32 idx)",false);
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_lenfld) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddProtoArg(func, amc::ConstRefto(ctype.cpp_type)<<" ", "parent");
        AddRetval(func, "i32", "", "");
        func.glob = true;
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$LenExpr", LengthExpr(ctype,"const_cast<$Cpptype&>(parent)"));
        Ins(&R, func.body , "return $LenExpr;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ctype_GetMsgMemptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FCtype &ctype = *amc::_db.genfield.p_ctype;
    if (ctype.c_lenfld) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddProtoArg(func, amc::ConstRefto(ctype.cpp_type)<<" ", "row");
            AddRetval(func, "algo::memptr", "", "");
            func.glob = true;
            Set(R, "$Cpptype", ctype.cpp_type);
            Set(R, "$LenExpr", LengthExpr(ctype,"const_cast<$Cpptype&>(row)"));
            Ins(&R, func.body , "return algo::memptr((u8*)&row, $LenExpr);");
        }
    }
}
