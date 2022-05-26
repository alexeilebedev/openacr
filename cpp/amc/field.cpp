// (C) AlgoEngineering LLC 2008-2012
// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/field.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
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

void amc::tclass_Field() {
    if (algo_lib::_db.cmdline.verbose) {
        amc::FField &field = *amc::_db.genfield.p_field;
        CheckUnderscores(field);
    }
}

void amc::tclass_Field2() {
}

void amc::tfunc_Field_Cleanup() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // cleanup function -- call user-defined function
    // at destruct time
    if (field.c_fcleanup) {
        amc::FFunc& cleanup = amc::CreateCurFunc(true); {
            AddRetval(cleanup, "void", "", "");
        }
        cleanup.extrn = true;
        Ins(&R, cleanup.comment, "User-defined cleanup function invoked for field $name of $Partype");
    }
}

void amc::tfunc_Field_Userinit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.c_fuserinit) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "void", "", "");
        }
        func.extrn = true;
        Ins(&R, func.comment, "User-defined init function invoked for field $name of $Partype");
    }
}

void amc::tfunc_Field_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

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

void amc::tfunc_Field2_ReadStrptrMaybe() {
    // provide a read function for the field if it already has a Set function,
    // or if the underlying type supports read.
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCtype &ctype = *field.p_ctype;
    bool ok = true;
    // need srcfield for raw
    ok = ok && !PadQ(field);
    ok = ok && !field.c_substr;// do not read these
    ok = ok && !field.c_cppfunc;// do not read these
    ok = ok && !GetLenfld(field);  // this field cannot be read
    ok = ok && !field.c_typefld; // this field cannot be read
    ok = ok && !(ctype.c_pmaskfld && ctype.c_pmaskfld->p_field == &field);
    ok = ok && (ValQ(field) || field.reftype == dmmeta_Reftype_reftype_Bitfld);
    ok = ok && !amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"ReadStrptrMaybe"));
    bool has_set = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Set"));
    bool has_ctyperead = !field.c_bitfld && !field.c_fbigend && HasReadQ(*field.p_arg) && !field.c_inlary;
    if (HasReadQ(*field.p_ctype) && ok && (has_set || has_ctyperead)) {
        amc::FFunc& doread = amc::CreateCurFunc();
        doread.inl = true;
        doread.priv = true;
        Ins(&R, doread.ret    , "bool",false);
        Ins(&R, doread.proto  , "$name_ReadStrptrMaybe($Partype &parent, algo::strptr in_str)",false);
        Ins(&R, doread.body     , "bool retval = true;");
        if (has_set) {// set function
            Set(R, "$Fldcpptype", field.cpp_type);
            Ins(&R, doread.body , "$Fldcpptype $name_tmp;");
            Ins(&R, doread.body , "retval = $Cpptype_ReadStrptrMaybe($name_tmp, in_str);");
            Ins(&R, doread.body , "if (retval) {");
            Ins(&R, doread.body , "    $name_Set(parent, $name_tmp);");
            Ins(&R, doread.body , "}");
            // don't set present flag, the _Set function already does it
        } else if (has_ctyperead) {
            Ins(&R, doread.body , "retval = $Cpptype_ReadStrptrMaybe(parent.$name, in_str);");
            SetPresent(doread,"parent",field);
            if (!ctype.c_pmaskfld) {
                // implementation involves calling $Cpptype_ReadStrptr --
                // as an optimization (that saves ~120K lines of generated code),
                // leave it marked internal and set special flag.
                // THIS IS IRREGULAR -- BUT I LIKE 120K LESS GARBAGE.
                doread.ismacro=true;
                field.ctype_read=true;
            }
        }
        Ins(&R, doread.body     , "return retval;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Field_Concat(){
    amc::FField* field = amc::_db.genfield.p_field;
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
