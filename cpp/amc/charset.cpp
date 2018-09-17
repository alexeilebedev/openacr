// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/charset.cpp -- Charset functions
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

static tempstr ToCppStringExpr(strptr str) {
    tempstr out;
    strptr_PrintCppQuoted(str,out,'"');
    return out;
}

// -----------------------------------------------------------------------------

// Preprocess charsets
void amc::gen_newfield_charset() {
    // phase 1: determine if charset will be simple (implemented inline) or
    // implemented with a bitset.
    ind_beg(amc::_db_charset_curs,charset,amc::_db) {
        algo::Charset_ReadStrptrMaybe(charset.chars, charset.expr);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tclass_Charset() {
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCharset *charset = field.c_charset;
    vrfy(charset,tempstr()<<"amc.need_charset"
         <<Keyval("field",field.field)
         <<Keyval("comment","charset record required"));
    algo_lib::Replscope &R = amc::_db.genfield.R;
    if (!charset->calc) {
        amc::FFunc *init = &amc::ind_func_GetOrCreate(tempstr() << field.field <<".Init");
        init->ismacro = true;
        Set(R, "$name", name_Get(field));
        Set(R, "$cppstr", ToCppStringExpr(charset->expr));
        // insert struct field
        InsVar(R, field.p_ctype, field.p_arg->cpp_type, name_Get(field), "", field.comment);
        if (charset->charrange) {
            Ins(&R, init->body, "(void)Charset_ReadStrptrMaybe(_db.$name, $cppstr);");
        } else {
            Ins(&R, init->body, "(void)Charset_ReadStrptrPlain(_db.$name, $cppstr);");
        }
    }
}

// -----------------------------------------------------------------------------

static tempstr CharCppExpr(u32 ch) {
    tempstr str;
    char_PrintCppSingleQuote(ch,str);
    return str;
}

// -----------------------------------------------------------------------------

// Find all consecutive character ranges in CHARSET
// and generate a character match function for character named CH
// Other strategies to try:
//     - Binary search
//     - || instead of |
//     - bit tricks
static void GenCalcMatch(algo_lib::Replscope &R, amc::FFunc &func, amc::FCharset *charset) {
    bool inside=false;
    int beg=0;
    int end=0;
    int sup=ch_Sup(charset->chars);
    Ins(&R, func.body, "bool ret = false;");
    for (int i=0; i<sup+1; i++) {
        if (ch_GetBit(charset->chars,i)) {
            if (bool_Update(inside,true)) {
                beg=i;
            }
        } else {
            if (bool_Update(inside,false)) {
                end=i;
                Set(R,"$charsetmin",CharCppExpr(beg));
                Set(R,"$charsetn",tempstr()<<end-beg);
                if (end == beg+1) {
                    Ins(&R, func.body, "ret |= ch == $charsetmin;");
                } else {
                    Ins(&R, func.body, "ret |= (ch - u32($charsetmin)) < u32($charsetn);");
                }
            }
        }
    }
    vrfy(!inside,"amc.badcharrange");
    Ins(&R, func.body, "return ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Charset_Match() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        func.proto = tempstr()<< name_Get(field)<<"Q()";// special syntax
        AddRetval(func, "bool", "", "");
        AddProtoArg(func, "u32", "ch");
        amc::FCharset *charset = field.c_charset;
        func.inl=true;
        if (charset->calc) {
            GenCalcMatch(R,func,charset);
        } else {
            Ins(&R, func.body, "return ch_GetBit($parname.$name, ch);");
        }
    }
}
