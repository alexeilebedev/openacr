// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/amc/cmp.cpp
//

#include "include/amc.h"

static bool TrivialCmpQ(amc::FField &field) {
    bool retval =
        field.p_arg->c_ccmp
        && (!field.c_fcmp || (!field.c_fcmp->versionsort && field.c_fcmp->casesens));
    return retval;
}

// Field comparison functions
void amc::tclass_Cmp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    Set(R,"$Chtype",TrivialCmpQ(field) ? "int" : "u64");
    if (field.p_arg->c_cstr && field.arg != "char") {// string comparison for target type
        vrfy(field.reftype==dmmeta_Reftype_reftype_Val, "fcmp implies Val");
        vrfy(c_datafld_N(*field.p_arg)==1, "versionsort/casesens requires 1 data field in target type");
        Set(R,"$ch", name_Get(*c_datafld_Find(*field.p_arg,0)));
    }
}

// Next char function for version sort
void amc::tfunc_Cmp_Nextchar() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFcmp          &fcmp       = *field.c_fcmp;
    if (field.p_arg->c_cstr && !fcmp.extrn) {
        amc::FFunc& nextch = amc::CreateCurFunc();
        nextch.priv = true;
        Ins(&R, nextch.comment, "Extract next character from STR and advance IDX");
        Ins(&R, nextch.ret  , "$Chtype", false);
        Ins(&R, nextch.proto, "$name_Nextchar($Cparent, algo::strptr &str, int &idx)", false);
        Ins(&R,     nextch.body, "(void)$parname;");
        Ins(&R,     nextch.body, "int i = idx;");
        Ins(&R,     nextch.body, "$Chtype ch = str.elems[i];");
        Ins(&R,     nextch.body, "i++;");
        if (!fcmp.casesens) {
            Ins(&R, nextch.body, "// downcase the char for case-insensitive comparison");
            Ins(&R, nextch.body, "if (ch >= 'A' && ch <= 'Z') {");
            Ins(&R, nextch.body, "    ch = ch - 'A' + 'a';");
            Ins(&R, nextch.body, "}");
        }
        if (fcmp.versionsort) {
            Ins(&R, nextch.body, "// detect number in string and eat it up into CH.");
            Ins(&R, nextch.body, "// (this only works for base-10 numbers");
            Ins(&R, nextch.body, "// Digits are sorted before chars");
            Ins(&R, nextch.body, "if (unsigned(ch - '0') < 10) {");
            Ins(&R, nextch.body, "    while(i < str.n_elems) { // resulting char sorts before ascii 47");
            Ins(&R, nextch.body, "        unsigned dig = str.elems[i] - '0';");
            Ins(&R, nextch.body, "        if (dig >= 10) break;");
            Ins(&R, nextch.body, "        ch = ch * 10 + dig;");
            Ins(&R, nextch.body, "        i++;");
            Ins(&R, nextch.body, "    }");
            Ins(&R, nextch.body, "} else if (ch > '9') {");
            Ins(&R, nextch.body, "    // create space between '0'..'9' for 15-digit numbers");
            Ins(&R, nextch.body, "    ch += 1000000000000000LL;");
            Ins(&R, nextch.body, "}");
        }
        Ins(&R,     nextch.body, "idx = i;");
        Ins(&R,     nextch.body, "return ch;");
    }
}

// Comparison function for field
void amc::tfunc_Cmp_Cmp() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFcmp          &fcmp       = *field.c_fcmp;
    amc::FFunc& cmp = amc::CreateCurFunc();

    Set(R, "$Fldtype", field.p_arg->cpp_type);
    Ins(&R, cmp.comment, "Compare two fields.");
    cmp.extrn=fcmp.extrn;
    Ins(&R, cmp.ret  , "i32",false);
    Ins(&R, cmp.proto, "$name_Cmp($Parent, $Partype &rhs)",false);
    if (!fcmp.extrn) {
        Ins(&R, cmp.body, "i32 retval = 0;");
        Set(R, "$a_val", FieldvalExpr(field.p_ctype, field, "$parname"));
        Set(R, "$b_val", FieldvalExpr(field.p_ctype, field, "rhs"));
        if (TrivialCmpQ(field)) {
            cmp.inl = true;
            Ins(&R, cmp.body, "retval = $Fldtype_Cmp($a_val, $b_val);");
        } else if (field.reftype == dmmeta_Reftype_reftype_Smallstr || (field.p_arg->c_cstr && field.arg != "char")) {// string comparison for target type
            if (fcmp.versionsort) {
                Ins(&R, cmp.comment, "Comparison uses version sort (detect embedded integers).");
            }
            if (fcmp.versionsort) {
                Ins(&R, cmp.comment, "Comparison is case-insensitive.");
            }
            Ins(&R, cmp.body, "int idx_a = 0;");
            Ins(&R, cmp.body, "int idx_b = 0;");
            Ins(&R, cmp.body, "algo::strptr str_a = $ch_Getary($a_val);");// convert to strptr (if string is padded, this strips padding once)
            Ins(&R, cmp.body, "algo::strptr str_b = $ch_Getary($b_val);");
            Ins(&R, cmp.body, "int n_a   = elems_N(str_a);");
            Ins(&R, cmp.body, "int n_b   = elems_N(str_b);");
            Ins(&R, cmp.body, "retval    = i32_Cmp(n_a,n_b);");
            Ins(&R, cmp.body, "while (idx_a < n_a && idx_b < n_b) {");
            Ins(&R, cmp.body, "    $Chtype ch_a = $name_Nextchar($pararg, str_a, idx_a);");
            Ins(&R, cmp.body, "    $Chtype ch_b = $name_Nextchar(rhs, str_b, idx_b);");
            Ins(&R, cmp.body, "    if (ch_a != ch_b) {");
            Ins(&R, cmp.body, "        retval = (ch_a > ch_b)*2-1;");
            Ins(&R, cmp.body, "        break;");
            Ins(&R, cmp.body, "    }");
            Ins(&R, cmp.body, "}");
        } else {
            prerr("amc.bad_fcmp"
                  <<Keyval("field",field.field)
                  <<Keyval("offending_type",field.arg)
                  <<Keyval("comment","No comparison defined for target type"));
            algo_lib::_db.exit_code++;
        }
        Ins(&R, cmp.body, "return retval;");
    }
}

// Lt function for field
void amc::tfunc_Cmp_Lt() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFcmp          &fcmp       = *field.c_fcmp;
    amc::FFunc& lt = amc::CreateCurFunc();

    Set(R, "$Fldtype", field.p_arg->cpp_type);
    Ins(&R, lt.comment, "Compare two fields. Comparison is anti-symmetric: if a>b, then !(b>a).");
    lt.extrn=fcmp.extrn;
    Ins(&R, lt.ret  , "bool",false);
    Ins(&R, lt.proto, "$name_Lt($Parent, $Partype &rhs)",false);

    if (!fcmp.extrn) {
        if (TrivialCmpQ(field)) {
            Set(R, "$a_val", FieldvalExpr(field.p_ctype, field, "$parname"));
            Set(R, "$b_val", FieldvalExpr(field.p_ctype, field, "rhs"));
            Ins(&R, lt.body, "return $Cpptype_Lt($a_val,$b_val);");
        } else {
            Ins(&R, lt.body, "return $name_Cmp($parname,rhs) < 0;");
        }
    }
}
