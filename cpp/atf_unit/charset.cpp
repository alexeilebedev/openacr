// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/charset.cpp
//

#include "include/atf_unit.h"

inline bool CharInRange(char a, char lo, char hi) {
    return (a>=lo) & (a<=hi);
}

inline bool _WhiteCharQ(char i) {
    return (i=='\t') | (i=='\n') | (i=='\r') | (i==' ');
}

inline bool _IdentCharQ(char i) {
    return CharInRange(i,'a','z') | CharInRange(i,'A','Z') | CharInRange(i,'0','9') | (i=='_');
}

inline bool _IdentStartQ(char i) {
    return CharInRange(i,'a','z') | CharInRange(i,'A','Z') | (i=='_');
}

inline bool _AlphaCharQ(char i) {
    return CharInRange(i,'a','z') | CharInRange(i,'A','Z');
}

inline bool _DigitCharQ(char i) {
    return CharInRange(i,'0','9');
}

inline bool _NewLineCharQ(char i) {
    return (i=='\n') | (i=='\r');
}

inline bool _HexCharQ(char i) {
    return CharInRange(i,'0','9') | CharInRange(i,'a','f') | CharInRange(i,'A','F');
}

inline bool _UpperCharQ(char i) {
    return CharInRange(i,'A','Z');
}

inline bool _LowerCharQ(char i) {
    return CharInRange(i,'a','z');
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Charset() {
    Charset RegxSqlSpecialChars;
    Charset  SsimBreakCharsName;
    Charset  SsimBreakCharsValue;
    Charset BashQuotesafeChars;
    Charset SsimQuotesafeChars;
    (void)Charset_ReadStrptrPlain(RegxSqlSpecialChars,"$.|?*+(){}^\\[]#");
    (void)Charset_ReadStrptrMaybe(SsimQuotesafeChars, "a-zA-Z0-9_;&*^%$@.!:,+/-");
    (void)Charset_ReadStrptrMaybe(BashQuotesafeChars, "a-zA-Z0-9_^%@:./-");
    (void)Charset_ReadStrptrPlain(SsimBreakCharsName, "[]{}()\t \r\n:");
    (void)Charset_ReadStrptrPlain(SsimBreakCharsValue, "[]{}()\t \r\n");

    {
        algo::Charset test;
        Charset_ReadStrptrPlain(test, "abc");
        frep_(i,256) {
            vrfy_((i=='a' || i=='b' || i=='c') || !ch_qGetBit(test,i));
        }
        Charset sep;
        Charset_ReadStrptrPlain(sep, " ,");
        vrfy_(ch_qGetBit(sep, ','));
        vrfy_(ch_qGetBit(sep, ' '));
        vrfy_(!ch_qGetBit(sep, '\t'));
    }

    for (int i=0; i<256; i++) {
        vrfy_(ch_GetBit(BashQuotesafeChars,i) == algo_lib::BashQuotesafeQ(i));
        vrfy_(ch_GetBit(RegxSqlSpecialChars,i) == algo_lib::RegxSqlSpecialQ(i));
        vrfy_(ch_GetBit(SsimBreakCharsName,i) == algo_lib::SsimBreakNameQ(i));
        vrfy_(ch_GetBit(SsimBreakCharsValue,i) == algo_lib::SsimBreakValueQ(i));
        vrfy_(ch_GetBit(SsimQuotesafeChars,i) == algo_lib::SsimQuotesafeQ(i));
        vrfy_(_DigitCharQ(i) == algo_lib::DigitCharQ(i));
        vrfy_(_NewLineCharQ(i) == algo_lib::NewLineCharQ(i));
        vrfy_(_WhiteCharQ(i) == algo_lib::WhiteCharQ(i));
        vrfy_(_IdentCharQ(i) == algo_lib::IdentCharQ(i));
        vrfy_(_IdentStartQ(i) == algo_lib::IdentStartQ(i));
        vrfy_(_AlphaCharQ(i) == algo_lib::AlphaCharQ(i));
        vrfy_(_HexCharQ(i) == algo_lib::HexCharQ(i));
        vrfy_(_UpperCharQ(i) == algo_lib::UpperCharQ(i));
        vrfy_(_LowerCharQ(i) == algo_lib::LowerCharQ(i));
    }
}
