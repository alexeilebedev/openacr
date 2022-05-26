// (C) AlgoEngineering LLC 2008-2013
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
// Source: cpp/amc/fconst.cpp -- Constants, string <-> int conversion
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

static bool FieldBitsetQ(amc::FField &field) {
    return !bh_bitfld_EmptyQ(field);
}

void amc::tclass_Fconst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    bool is_string = FieldStringQ(field);

    vrfy(!FldfuncQ(field), "amc.gen_field_fconst  error:'fldfunc may not be combined with fconst'");
    Set(R, "$Enumtype", Enumtype(field));

    // Automatically create a ccast record for a single-field non-string ctype
    // Example:
    //
    // +    Status(tdm_StatusEnum arg) : status(bool(arg)) {}
    // +    operator tdm_StatusEnum() const { return tdm_StatusEnum(status); }
    if (!is_string && c_field_N(*field.p_ctype) == 1 && field.c_fcast && !FieldBitsetQ(field)) {
        field.c_fcast->expr = Enumtype(field);
    }

    // Get field as enum
    if (is_string) {
        tempstr text;
        text << "// compile-time string constants for "<<field.field<<"\n";
        int n=0;
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            text << "const char *";
            text << fconst.cpp_name;
            text << "\t = ";
            strptr_PrintCppQuoted(fconst.value.value, text, '"');
            text << ";\n";
            n++;
            // pick a break-point that avoids global diffs
            if (strptr_Hash(0,fconst.cpp_name)%100 > 90) {
                text << "\n";
                *ns.cpp << Tabulated(text, "\t", "ll", 2);
                ch_RemoveAll(text);
                n=0;
            }
        }ind_end;
        if (n > 0) {
            text << "\n";
            *ns.cpp << Tabulated(text, "\t", "ll", 2);
        }
    }
}

void amc::tfunc_Fconst_GetEnum() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& getenum = amc::CreateCurFunc();
        Ins(&R, getenum.comment, "Get value of field as enum type");
        Ins(&R, getenum.ret  , "$Enumtype", false);
        Ins(&R, getenum.proto, "$name_GetEnum($Cparent)", false);
        if (!field.c_fbigend) {
            Ins(&R, getenum.body, "return $Enumtype($parname.$name);");
        } else {
            Ins(&R, getenum.body, "return $Enumtype($name_Get($parname));");
        }
    }
}

void amc::tfunc_Fconst_SetEnum() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& setenum = amc::CreateCurFunc();
        Ins(&R, setenum.comment, "Set value of field from enum type.");
        Ins(&R, setenum.ret  , "void", false);
        Ins(&R, setenum.proto, "$name_SetEnum($Parent, $Enumtype rhs)", false);
        Ins(&R, setenum.body, AssignExpr(field, "$parname", "rhs", true)<<";");
    }
}

void amc::tfunc_Fconst_ToCstr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& tocstr = amc::CreateCurFunc();
        Ins(&R, tocstr.comment, "Convert numeric value of field to one of predefined string constants.");
        Ins(&R, tocstr.comment, "If string is found, return a static C string. Otherwise, return NULL.");
        Ins(&R, tocstr.ret  , "const char*", false);
        Ins(&R, tocstr.proto, "$name_ToCstr($Cparent)", false);
        Ins(&R, tocstr.body,  "const char *ret = NULL;");
        Ins(&R, tocstr.body,  "switch($name_GetEnum($parname)) {");
        // compose switch statement.
        // note: some fields have two or more names for one numeric value.
        // only the first will be returned.
        ind_beg(amc::field_c_fconst_curs, fconst, field) if (amc::ind_fconst_int_InsertMaybe(fconst)) {
            tempstr caselabel;
            strptr_PrintPadRight(fconst.cpp_name, caselabel,  30);
            Set(R, "$caselabel" , caselabel);
            Set(R, "$casestrval", name_Get(fconst));
            Ins(&R, tocstr.body,  "    case $caselabel: ret = \"$casestrval\";  break;");
        }ind_end;
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            amc::ind_fconst_int_Remove(fconst);
        }ind_end;
        Ins(&R, tocstr.body,  "}");
        Ins(&R, tocstr.body,  "return ret;");
    }
}

void amc::tfunc_Fconst_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        Set(R, "$getexpr", amc::FieldvalExpr(field.p_ctype,field,"$parname"));
        amc::FFunc& prn = amc::CreateCurFunc();
        Ins(&R, prn.comment, "Convert $name to a string. First, attempt conversion to a known string.");
        Ins(&R, prn.comment, "If no string matches, print $name as a numeric value.");
        Ins(&R, prn.ret  , "void", false);
        Ins(&R, prn.proto, "$name_Print($Cparent, algo::cstring &lhs)", false);
        Ins(&R, prn.body,  "const char *strval = $name_ToCstr($parname);");
        Ins(&R, prn.body,  "if (strval) {");
        Ins(&R, prn.body,  "    lhs << strval;");
        Ins(&R, prn.body,  "} else {");
        Ins(&R, prn.body,  "    lhs << $getexpr;");
        Ins(&R, prn.body,  "}");
    }
}

void amc::tfunc_Fconst_SetStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& mbssp = amc::CreateCurFunc();
        Ins(&R, mbssp.comment, "Convert string to field.");
        Ins(&R, mbssp.comment, "If the string is invalid, do not modify field and return false.");
        Ins(&R, mbssp.comment, "In case of success, return true");
        Ins(&R, mbssp.ret  , "bool", false);
        Ins(&R, mbssp.proto, "$name_SetStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, mbssp.body,  "bool ret = false;");
        Ins(&R, mbssp.body,  "switch (elems_N(rhs)) {");
        // compute list of enumstr and index it by length.
        amc::enumstr_len_RemoveAll();
        amc::enumstr_RemoveAll();
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            amc::Enumstr key(ch_N(name_Get(fconst)), algo::RnullStr8(FirstN(strptr(name_Get(fconst)),8)));
            amc::ind_enumstr_len_GetOrCreate(key.len);        // pre-create
            amc::FEnumstr &enumstr = amc::ind_enumstr_GetOrCreate(key);
            c_fconst_Insert(enumstr, fconst);
        }ind_end;
        while (amc::FEnumstrLen *enumstr_len = amc::bh_enumstr_len_RemoveFirst()) {
            int len = enumstr_len->len;
            Ins(&R, mbssp.body, tempstr()<<"case "<<len<<": {");
            if (enumstr_len->len == 0) {
                if (bh_enumstr_N(*enumstr_len) > 1) {
                    // this exception hits if gconst "name" attribute is a bad reference:
                    // in that case a bunch of empty strings is inserted.
                    // TODO FIX!!
                    prerr("Several zero-length strings found. Check gconst table.");
                    vrfy(0,"error");
                }
                if (bh_enumstr_N(*enumstr_len) > 0) {
                    amc::FFconst *fconst = amc::c_fconst_Find(*amc::bh_enumstr_First(*enumstr_len), 0);
                    Set(R, "$Const", fconst->cpp_name);
                    Ins(&R, mbssp.body, tempstr()<<"    $name_SetEnum($parname,$Const);");
                    Ins(&R, mbssp.body, tempstr()<<"    ret = true;");
                }
            } else {
                Ins(&R, mbssp.body, tempstr()<<"    switch ("<<amc::VarStringToInteger("rhs",len <= 8? len : 8)<<") {");
                while (amc::FEnumstr *enumstr = amc::bh_enumstr_RemoveFirst(*enumstr_len)) {
                    Ins(&R, mbssp.body, tempstr()<<"    case "<<amc::ConstStringToCaseLabel(enumstr->enumstr.str)<<": {");
                    ind_beg(amc::enumstr_c_fconst_curs, fconst, *enumstr) {
                        Set(R, "$Const", fconst.cpp_name);
                        if (len>8) {
                            tempstr val;
                            strptr_PrintCpp(RestFrom(strptr(name_Get(fconst)),8), val);
                            Set(R, "$remain", tempstr()<<len-8);
                            Set(R, "$Val", val);
                            Ins(&R, mbssp.body, "        if (memcmp(rhs.elems+8,$Val,$remain)==0) { $name_SetEnum($parname,$Const); ret = true; break; }");
                        } else {
                            Ins(&R, mbssp.body, "        $name_SetEnum($parname,$Const); ret = true; break;");
                        }
                    }ind_end;
                    if (len>8) {
                        Ins(&R, mbssp.body, "        break;");
                    }
                    Ins(&R, mbssp.body, "    }");
                }
                Ins(&R, mbssp.body, "    }");
            }
            Ins(&R, mbssp.body, "    break;");// top case
            Ins(&R, mbssp.body, "}");
        }
        Ins(&R, mbssp.body,  "}");
        Ins(&R, mbssp.body,  "return ret;");
    }
}

void amc::tfunc_Fconst_SetStrptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& ssp = amc::CreateCurFunc();
        Ins(&R, ssp.comment, "Convert string to field.");
        Ins(&R, ssp.comment, "If the string is invalid, set numeric value to DFLT");
        Ins(&R, ssp.ret  , "void", false);
        Ins(&R, ssp.proto, "$name_SetStrptr($Parent, algo::strptr rhs, $Enumtype dflt)", false);
        Ins(&R, ssp.body,  "if (!$name_SetStrptrMaybe($parname,rhs)) $name_SetEnum($parname,dflt);");
    }
}

void amc::tfunc_Fconst_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FieldStringQ(field)  && !FieldBitsetQ(field) && HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, rd.body    , "bool retval = false;");
        Ins(&R, rd.body    , "retval = $name_SetStrptrMaybe($parname,rhs); // try symbol conversion");
        Ins(&R, rd.body    , "if (!retval) { // didn't work? try reading as underlying type");
        if (!field.c_fbigend) {
            Ins(&R, rd.body, "    retval = $Fldtype_ReadStrptrMaybe($parname.$name,rhs);");
        } else {
            Ins(&R, rd.body, "    $Fldtype $name_tmp;");
            Ins(&R, rd.body, "    retval = $Fldtype_ReadStrptrMaybe($name_tmp, rhs);");
            Ins(&R, rd.body, "    if (retval) {");
            Ins(&R, rd.body, "        $name_Set($parname, $name_tmp);");
            Ins(&R, rd.body, "    }");
        }
        Ins(&R, rd.body    , "}");
        Ins(&R, rd.body    , "return retval;");
    }
}
