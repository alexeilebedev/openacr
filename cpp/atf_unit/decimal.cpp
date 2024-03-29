// Copyright (C) 2024 AlgoRND
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/decimal.cpp
//

#include "include/algo.h"
#include "include/atf_unit.h"

// --------------------------------------------------------------------------------

static void TestDecimalReadPrint(strptr str, i64 mantissa, i32 exponent) {
    verblog(str);
    {
        algo::Decimal result;
        vrfy_(Decimal_ReadStrptrMaybe(result,str));
        vrfyeq_(result.mantissa,mantissa);
        vrfyeq_(result.exponent,exponent);
    }
    {
        algo::Decimal value(exponent,mantissa);
        cstring result;
        Decimal_Print(value,result);
        vrfyeq_(result,str);
    }
}

static void TestDecimalGetSetDouble(strptr str, i64 mantissa, i32 exponent) {
    verblog(str);
    {
        algo::Decimal value(exponent,mantissa);
        double f = Decimal_GetDouble(value);
        tempstr result;
        double f0(0);
        double_ReadStrptrMaybe(f0,str);
        double e = algo::Abs(f0) * 1e-15;
        double delta = algo::Abs(f-f0);
        //prlog(e);
        //prlog(delta)
        vrfy_(algo::Abs(delta) <= e);
    }
    {
        double f(0);
        double_ReadStrptrMaybe(f,str);
        algo::Decimal d;
        Decimal_SetDouble(d,f);
        //prlog(d.mantissa<<"e"<<d.exponent);
        while (d.exponent > exponent) {
            d.mantissa += d.mantissa<0 ? -5 : 5;
            ++exponent;
        }
        while (d.exponent < exponent) {
            d.mantissa += d.mantissa<0 ? -5 : 5;
            d.mantissa /= 10;
            ++d.exponent;
        }
        vrfyeq_(d.mantissa,mantissa);
    }
}

void atf_unit::unittest_algo_lib_Decimal() {
    TestDecimalReadPrint("0",0,0);
    TestDecimalReadPrint("1",1,0);
    TestDecimalReadPrint("-1",-1,0);
    TestDecimalReadPrint("10",1,1);
    TestDecimalReadPrint("100",1,2);
    TestDecimalReadPrint("1000",1,3);
    TestDecimalReadPrint("-10",-1,1);
    TestDecimalReadPrint("-100",-1,2);
    TestDecimalReadPrint("-1000",-1,3);
    TestDecimalReadPrint("0.1",1,-1);
    TestDecimalReadPrint("0.01",1,-2);
    TestDecimalReadPrint("0.001",1,-3);
    TestDecimalReadPrint("-0.1",-1,-1);
    TestDecimalReadPrint("-0.01",-1,-2);
    TestDecimalReadPrint("-0.001",-1,-3);
    // max i64 mantissa
    TestDecimalReadPrint("-9223372036854775808000000000000000000000000000000000000000000000000000000000000000",-9223372036854775807-1,63);
    TestDecimalReadPrint("-0.000000000000000000000000000000000000000000009223372036854775808",-9223372036854775807-1,-63);
    TestDecimalReadPrint("9223372036854775807000000000000000000000000000000000000000000000000000000000000000",9223372036854775807,63);
    TestDecimalReadPrint("0.000000000000000000000000000000000000000000009223372036854775807",9223372036854775807,-63);

    TestDecimalGetSetDouble("0",0,0);
    TestDecimalGetSetDouble("1",1,0);
    TestDecimalGetSetDouble("-1",-1,0);
    TestDecimalGetSetDouble("10",1,1);
    TestDecimalGetSetDouble("100",1,2);
    TestDecimalGetSetDouble("1000",1,3);
    TestDecimalGetSetDouble("-10",-1,1);
    TestDecimalGetSetDouble("-100",-1,2);
    TestDecimalGetSetDouble("-1000",-1,3);
    TestDecimalGetSetDouble("0.1",1,-1);
    TestDecimalGetSetDouble("0.01",1,-2);
    TestDecimalGetSetDouble("0.001",1,-3);
    TestDecimalGetSetDouble("-0.1",-1,-1);
    TestDecimalGetSetDouble("-0.01",-1,-2);
    TestDecimalGetSetDouble("-0.001",-1,-3);
    // double guarantees print loop of 15 exact decimal digits
    TestDecimalGetSetDouble("-123456789012345000000000000000000000000000000000000000000000000000000000000000",-123456789012345,63);
    TestDecimalGetSetDouble("-0.000000000000000000000000000000000000000000000000123456789012345",-123456789012345,-63);
    TestDecimalGetSetDouble("123456789012345000000000000000000000000000000000000000000000000000000000000000",123456789012345,63);
    TestDecimalGetSetDouble("0.000000000000000000000000000000000000000000000000123456789012345",123456789012345,-63);
}
