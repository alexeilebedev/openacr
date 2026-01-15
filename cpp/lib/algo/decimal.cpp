// Copyright (C) 2024-2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/decimal.cpp
//

#include "include/algo.h"

// Normalize decimal, so mantissa does not contain tail 0
void algo::Decimal_Normalize(algo::Decimal &parent) {
    while (parent.exponent<63 && parent.mantissa && parent.mantissa % 10 == 0) {
        parent.mantissa /= 10;
        ++parent.exponent;
    }
    if (!parent.mantissa) {
        parent.exponent = 0;
    }
}

// Try parse Decimal [+-][0-9]*[.][0-9]*
// Stop after first non-conforming char, error if no digit has been met.
bool algo::TryParseDecimal(algo::StringIter &iter, algo::Decimal &result) {
    bool ret(true);
    algo::Decimal value;
    bool negate(false);
    bool fraction(false);
    int &index = iter.index;
    enum {ws, digits, end} state(ws);
    bool done(false);
    bool digit(false);
    while  (!done && index<elems_N(iter.expr)) {
        char c = iter.expr[index];
        switch (state) {
        case end:
            done = true;
            break;
        case ws:
            if (c=='-') {
                negate = true;
                state = digits;
                ++index;
            } else if (c== '+') {
                state = digits;
                ++index;
            } else if (c== '.') {
                fraction = true;
                state = digits;
                ++index;
            } else if (algo_lib::DigitCharQ(c)) {
                state = digits;
            } else if (algo_lib::WhiteCharQ(c)) {
                ++index;
            } else {
                ret = false;
                state = end;
            }
            break;
        case digits:
            if (c== '.') {
                if (!fraction) {
                    fraction = true;
                    ++index;
                } else {
                    state = end;
                }
            } else if (algo_lib::DigitCharQ(c)) {
                digit = true;
                if (value.mantissa < INT64_MIN/10 || value.mantissa > INT64_MAX/10) {
                    value.exponent += 1-fraction;
                } else {
                    c -= '0';
                    value.mantissa *= 10;
                    value.mantissa += negate ? -c : c;
                    value.exponent -= fraction;
                }
                ++index;
            } else {
                ret = digit;
                state = end;
            }
            break;
        }
    }
    if (ret) {
        Decimal_Normalize(value);
        result = value;
    }
    return ret;
}

// Print Decimal
void algo::Decimal_Print(algo::Decimal parent, algo::cstring &str) {
    if (parent.exponent >= 0) {
        str << parent.mantissa;
        char_PrintNTimes('0',str,parent.exponent);
    } else {
        bool neg = parent.mantissa < 0;
        if (neg) {
            str << "-";
        }
        int exp = -parent.exponent;
        tempstr temp;
        temp << parent.mantissa;
        int len = ch_N(temp) - neg;
        if (len > exp) {
            int intlen = len - exp;
            str << ch_GetRegion(temp,+neg,intlen) << "." << ch_RestFrom(temp,intlen+neg);
        } else {
            str << "0.";
            char_PrintNTimes('0',str,exp-len);
            str << ch_RestFrom(temp,+neg);
        }
    }
}

// Read Decimal from string
bool algo::Decimal_ReadStrptrMaybe(algo::Decimal &parent, algo::strptr in_str) {
    algo::StringIter iter(in_str);
    return TryParseDecimal(iter,parent);
}

// Convert Decimal to double
double algo::Decimal_GetDouble(algo::Decimal parent) {
    return double(parent.mantissa) * pow(10.,parent.exponent);
}

// Convert double to Decimal
void algo::Decimal_SetDouble(algo::Decimal &parent, double value) {
    // need a better way
    tempstr temp;
    char buf[30];
    int len = snprintf(buf,sizeof buf - 1,"%.17g",value);
    algo::StringIter it(strptr(buf,len));
    i32 exponent(0);
    TryParseDecimal(it,parent) && SkipChar(it,'e') && TryParseI32(it,exponent);
    parent.exponent += exponent;
}
