// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/charset.cpp -- Charset functions
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
        if (charset.charrange) {
            algo::Charset_ReadStrptrMaybe(charset.chars, charset.expr);
        } else {
            for (int i=0; i<ch_N(charset.expr); i++) {
                algo::ch_SetBit(charset.chars, charset.expr.ch[i]);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::tclass_Charset() {
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FCharset *charset = field.c_charset;
    vrfy(charset,tempstr()<<"amc.need_charset"
         <<Keyval("field",field.field)
         <<Keyval("comment","charset record required"));
    algo_lib::Replscope &R = amc::_db.genctx.R;
    if (!charset->calc) {
        amc::FFunc *init = &amc::ind_func_GetOrCreate(tempstr() << field.field <<".Init");
        init->ismacro = true;
        Set(R, "$name", name_Get(field), false);
        Set(R, "$cppstr", ToCppStringExpr(charset->expr), false);
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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
