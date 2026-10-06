// Copyright (C) 2026 AlgoX2 Corp
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/fdec.cpp
//

#include "include/atf_amc.h"

// fdec on a plain FDb field: the getter reads the value through _db
// and the setter writes it through _db.
void atf_amc::amctest_DecGlobal() {
    atf_amc::testdec_qSetDouble(1.25);
    vrfyeq_(atf_amc::_db.testdec, i64(125));
    vrfyeq_(atf_amc::testdec_GetDouble(), 1.25);
    vrfyeq_(atf_amc::testdec_GetInt(), i64(1));
}


// Two fdec fields on one ctype: each field keeps its own accessors with its
// own scale; the ctype-named GetScale convenience is not generated.
void atf_amc::amctest_DecTwoFields() {
    atf_amc::DecQuote quote;
    bid_qSetDouble(quote, 1.25);
    ask_qSetDouble(quote, 1.25);
    vrfyeq_(quote.bid, u64(125));
    vrfyeq_(quote.ask, u64(12500));
    vrfyeq_(bid_GetDouble(quote), 1.25);
    vrfyeq_(ask_GetDouble(quote), 1.25);
}

// Printing a signed fdec at the underlying type's minimum: the minimum's
// magnitude has no representation in the type itself, so Print must widen
// before negating -- an in-type negation wraps, and the wrapped i32/i16
// value then sign-extends into a garbage u64 magnitude. Pins the exact
// digits at the i32 and i64 minimum, plus an ordinary negative value.
void atf_amc::amctest_DecPrintMin() {
    algo::I32Dec1 dec32;
    dec32.value = i32(0x80000000);
    tempstr out32;
    value_Print(dec32, out32);
    vrfy_(out32 == "-214748364.8");

    algo::I64Dec2 dec64;
    dec64.value = i64(0x8000000000000000ULL);
    tempstr out64;
    value_Print(dec64, out64);
    vrfy_(out64 == "-92233720368547758.08");

    algo::I64Dec2 dec64neg;
    dec64neg.value = -150;
    tempstr outneg;
    value_Print(dec64neg, outneg);
    vrfy_(outneg == "-1.50");
}
