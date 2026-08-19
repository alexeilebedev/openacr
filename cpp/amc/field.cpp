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
// Source: cpp/amc/field.cpp -- Generic field generator
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void RemoveUnderscores(strptr from, cstring &to) {
    algo::aryptr<char> ignore = ch_AllocN(to,ch_N(from));
    (void)ignore;
    int j=0;
    for (int i=0; i<from.n_elems; i++) {
        if (from.elems[i] != '_') {
            to.ch_elems[j++]=from.elems[i];
        }
    }
    to.ch_n=j;
}

static void CheckUnderscores(amc::FField &field) {
    tempstr t1;
    RemoveUnderscores(name_Get(field),t1);
    tempstr ctype_lu;
    strptr_PrintLowerUnder(name_Get(*field.p_arg),ctype_lu);
    tempstr t2;
    RemoveUnderscores(ctype_lu,t2);
    if (t1 == t2 && name_Get(field) != ctype_lu) {
        prerr("amc.fname"
              <<Keyval("field",field.field)
              <<Keyval("shouldbe",ctype_lu)
              <<Keyval("ctype",field.arg)
              <<Keyval("comment","Field name very similar to a lower_under version of ctype name"));
    }
}

// -----------------------------------------------------------------------------

// A field's ffunc names are validated purely by the MarkUsed mechanism: every
// emission site calls FindFfunc(...,true), so a name that no generator consumes
// (a typo, or a name not applicable to the field's reftype/tclass) is left
// unmarked and reported by gen_check_ffunc.  No upfront tclass-membership check
// is needed -- the set of tfuncs that actually run for the field is the
// authority (this replaced the old amcdb.tcb table).
void amc::tclass_Field() {
    amc::FField &field = *amc::_db.genctx.p_field;
    if (algo_lib::_db.cmdline.verbose) {
        CheckUnderscores(field);
    }
}

void amc::tclass_Field2() {
}

void amc::tfunc_Field_Cleanup() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    // cleanup function -- call user-defined function at destruct time.
    // Cleanup is now an ffunc name (was dmmeta.fcleanup, then fcb/Cleanup).
    if (amc::FindFfunc(field, amcdb_cbtype_Cleanup)) {
        amc::FFunc& cleanup = amc::CreateCurFunc(true); {
            AddRetval(cleanup, "void", "", "");
        }
        cleanup.acrkey << "ffunc:"<<field.field<<"."<<amcdb_cbtype_Cleanup;
        cleanup.extrn = true;
        Ins(&R, cleanup.comment, "User-defined cleanup function invoked for field $name of $Partype");
    }
}

void amc::tfunc_Field_Userinit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    // Userinit is now an ffunc name (was dmmeta.fuserinit, then fcb/Userinit).
    if (amc::FindFfunc(field, amcdb_cbtype_Userinit)) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "void", "", "");
        }
        func.acrkey << "ffunc:"<<field.field<<"."<<amcdb_cbtype_Userinit;
        func.extrn = true;
        Ins(&R, func.comment, "User-defined init function invoked for field $name of $Partype");
    }
}

// Find an ffunc of the given name declared on FIELD (via dmmeta.ffunc).
// Returns NULL if none.  Pkey is '<field>.<name>'; build it via the
// amc-generated concat helper and look up in the unique ind_ffunc hash.
// If mark_used is true and the ffunc is found, set ffunc.used so gen_check_ffunc
// does not warn about it.  Pass true at emission sites, false at
// classification-only queries (e.g. PlaindataVisit).
amc::FFfunc *amc::FindFfunc(amc::FField &field, algo::strptr name, bool mark_used DFLTVAL(false)) {
    amc::FFfunc *ffunc = amc::ind_ffunc_Find(dmmeta::Ffunc_Concat_field_name(field.field, name));
    if (ffunc && mark_used) {
        ffunc->used = true;
    }
    return ffunc;
}

// Emit prototype for a user-defined OnXref/OnUnref callback on FIELD.
// CreateCurFunc(true) auto-emits the parent arg only when the parent
// ctype is non-global — so a groupby field gets <name>_OnXref(parent&,
// row&), an FDb-pool field gets <name>_OnXref(row&).  Call sites use
// "$pararg, row" which Replscope collapses to just "row" when the
// parent is global.
static void EmitFfuncProto(amc::FField &field, algo::strptr cbname) {
    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, "void", "", "");
    }
    AddProtoArg(func, amc::Refto(field.p_arg->cpp_type), "row");
    func.acrkey << "ffunc:" << field.field << "." << cbname;
    func.extrn = true;
    func.comment << "User-defined " << cbname << " callback for field " << name_Get(field);
}

void amc::tfunc_Field_OnXref() {
    amc::FField &field = *amc::_db.genctx.p_field;
    if (FindFfunc(field, amcdb_cbtype_OnXref)) {
        EmitFfuncProto(field, amcdb_cbtype_OnXref);
    }
}

void amc::tfunc_Field_OnUnref() {
    amc::FField &field = *amc::_db.genctx.p_field;
    if (FindFfunc(field, amcdb_cbtype_OnUnref)) {
        EmitFfuncProto(field, amcdb_cbtype_OnUnref);
    }
}

void amc::tfunc_Field_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    // cascade delete -- define function
    // implementation will be supplied by the field generator
    if (!FldfuncQ(field) && field.c_cascdel) {
        amc::FField *inst = FirstInst(*field.p_arg);
        vrfy(field.reftype != dmmeta_Reftype_reftype_Upptr, "illegal cascdel of Upptr");
        vrfy(inst, tempstr()<<"cascdel requires a pool to instantiate ["<<field.arg<<"]");
        vrfy(inst->p_ctype == field.p_ctype
             || GlobalQ(*inst->p_ctype)
             , tempstr()<<"can't figure out how to delete "<<field.field);
        Set(R, "$argname", name_Get(*inst));
        amc::FFunc& cascdel = amc::CreateCurFunc(true); {
            Ins(&R, cascdel.comment, "Delete referred-to items.");
            AddRetval(cascdel, "void", "", "");
        }
    }
}

// provide a read function for the field if it already has a Set function,
// or if the underlying type supports read.
void amc::tfunc_Field2_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    //amc::FCtype &ctype = *field.p_ctype;
    bool ok = true;
    // need srcfield for raw
    ok = ok && !PadQ(field);
    ok = ok && !field.c_substr;// do not read these
    ok = ok && !field.c_cppfunc;// do not read these
    ok = ok && !GetLenfld(field);  // this field cannot be read
    ok = ok && !field.c_typefld; // this field cannot be read
    ok = ok && !field.c_pmaskfld;// this field cannot be "read"
    ok = ok && (ValQ(field) || field.reftype == dmmeta_Reftype_reftype_Bitfld);
    ok = ok && !amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"ReadStrptrMaybe"));
    bool has_set = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Set"));
    bool has_ctyperead = HasReadQ(*field.p_arg) &&
        !(field.c_bitfld || field.c_fbigend || field.c_inlary || field.c_falias);
    vrfy(!(has_set && field.c_fflag && field.c_fflag->cumulative),
         tempstr()<<"amc.fflag"
         <<Keyval("fflag",field.field)
         <<Keyval("comment", "fflag.cumulative can't be used on a field with Set function"));
    if (HasReadQ(*field.p_ctype) && ok) {
        if (has_ctyperead && !has_set && !c_pmaskfld_member_N(field) && !field.c_fflag) {
            field.ctype_read=true;// use ctype's function to read this field
        } else {
            amc::FFunc& doread = amc::CreateCurFunc();
            doread.inl = true;
            doread.priv = true;
            Set(R, "$Fldcpptype", field.cpp_type);
            AddRetval(doread, "bool", "retval", "true");
            // $Parent collapses for a global ctype: the reader, like every
            // field-level function of a global, takes no parent argument
            Ins(&R, doread.proto  , "$name_ReadStrptrMaybe($Parent, algo::strptr in_str)",false);
            // remap empty value to something different
            if (field.c_fflag && field.c_fflag->emptyval != "") {
                Set(R,"$emptyval",field.c_fflag->emptyval);
                Ins(&R, doread.body , "if (in_str == \"\") {");
                Ins(&R, doread.body , "    in_str = $emptyval; // fflag:$field - empty input string");
                Ins(&R, doread.body , "}");
            }
            if (has_set) {// set function
                Ins(&R, doread.body , "$Fldcpptype $name_tmp;");
                Ins(&R, doread.body , "retval = $Cpptype_ReadStrptrMaybe($name_tmp, in_str);");
                Ins(&R, doread.body , "if (retval) {");
                Ins(&R, doread.body , tempstr()<<"    "<<AssignExpr(field,"$parname","$name_tmp",false)<<";");
                Ins(&R, doread.body , "}");
                // don't set present flag, the _Set function already does it
            } else if (field.c_fflag && field.c_fflag->cumulative) {
                Ins(&R, doread.body , "$Fldcpptype $name_tmp;");
                Ins(&R, doread.body , "retval = $Cpptype_ReadStrptrMaybe($name_tmp, in_str);");
                Ins(&R, doread.body , "if (retval) {");
                Ins(&R, doread.body , "    $parname.$name += $name_tmp; // fflag:$field cumulative");
                Ins(&R, doread.body , "}");
                SetPresent(doread,Subst(R,"$parname"),field);
            } else if (has_ctyperead) {
                Ins(&R, doread.body , "retval = $Cpptype_ReadStrptrMaybe($parname.$name, in_str);");
                SetPresent(doread,Subst(R,"$parname"),field);
            } else {
                if (!GlobalQ(*field.p_ctype)) {
                    MaybeUnused(doread, Subst(R,"$parname"));
                }
                MaybeUnused(doread, "in_str");
                Ins(&R, doread.body , "// don't know how to read field $field");
            }
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Field_Concat(){
    amc::FField* field = amc::_db.genctx.p_field;
    if (field->has_substr) {// initialized in gen_prepfield
        // collect all fields that use FIELD as a source
        amc::c_substr_field_RemoveAll();
        ind_beg(amc::ctype_c_field_curs, fld2, *field->p_ctype){
            if (fld2.c_substr && fld2.c_substr->p_srcfield == field) {
                amc::c_substr_field_Insert(*fld2.c_substr);
            }
        }ind_end;
        ind_beg(amc::_db_c_substr_field_curs, subfield, amc::_db){
            int min = 0;
            int max = 32;
            for(int i = 0; i <= ch_N(subfield.expr.value) - 3 ; i += 3){
                int mid = (min + max ) / 2;
                char dir = ch_Getary(subfield.expr.value)[i+2];
                min = (dir == 'R') ? mid : min;
                max = (dir == 'L') ? mid : max;
            }
            subfield.range = (u64(1) << max)  - ((u64)1 << min);
        }ind_end;
        amc::c_substr_field_QuickSort();
        GenerateSetForCoveredTrees(*field, 0, -1);
    }
}

// -----------------------------------------------------------------------------

// True if ReadFieldExpr for the field
bool amc::HasReadExprQ(amc::FField &field) {
    return field.ctype_read || amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"ReadStrptrMaybe"));
}

// -----------------------------------------------------------------------------

// Return an expression, with type bool,
// reading field FIELD in struct PARENT from value STRVAL
// If impossible, return FALSE;
tempstr amc::ReadFieldExpr(amc::FField &field, algo::strptr parent, algo::strptr strval) {
    tempstr ret;
    if (field.ctype_read) {
        ret << field.cpp_type<<"_ReadStrptrMaybe("<<parent<<"."<<name_Get(field)<<", "<<strval<<")";
    } else if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"ReadStrptrMaybe"))) {
        ret << name_Get(field)<<"_ReadStrptrMaybe("<<ParentArgExpr(*field.p_ctype,parent,true)<<strval<<")";
    } else if (field.c_cppfunc && field.c_cppfunc->print) {
        ret << "true"; // TODO support set
    } else {
        ret << "false";
    }
    return ret;
}
