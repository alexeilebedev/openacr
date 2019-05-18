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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/fconst.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"


// FCONST tests
void atf_amc::amctest_Fconst() {
    // dflt ctor
    {
        atf_amc::Typefconst x;
        vrfy_(x.value == 0);
    }
    // parametric ctor - u32
    {
        atf_amc::Typefconst x(u32(100));
        vrfy_(x.value == 100);
    }
    // parametric ctor - enum value
    {
        atf_amc::Typefconst x(atf_amc_Typefconst_value_strval1);
        vrfy_(x.value == 1);
    }
    // _GetEnum
    {
        atf_amc::Typefconst x;
        x.value = 1;
        vrfy_(atf_amc_Typefconst_value_strval1 == value_GetEnum(x));
        x.value = 2;
        vrfy_(atf_amc_Typefconst_value_strval2 == value_GetEnum(x));
    }
    // _SetEnum
    {
        atf_amc::Typefconst x;
        value_SetEnum(x,atf_amc_Typefconst_value_strval1);
        vrfy_(x.value == 1);
        value_SetEnum(x,atf_amc_Typefconst_value_strval2);
        vrfy_(x.value == 2);
    }
    // _ToCstr
    {
        atf_amc::Typefconst x;

        x.value = 1;
        vrfy_(0 == strcmp("strval1", value_ToCstr(x)));

        x.value = 2;
        vrfy_(0 == strcmp("strval2", value_ToCstr(x)));

        x.value = 3;
        vrfy_(NULL == value_ToCstr(x));
    }

    // _Print
    {
        cstring s1,s2,s3;
        atf_amc::Typefconst x;

        x.value = 1;
        value_Print(x,s1);
        vrfy_(s1 == "strval1");

        x.value = 2;
        value_Print(x,s2);
        vrfy_(s2 == "strval2");

        x.value = 3;
        value_Print(x,s3);
        vrfy_(s3 == "3");
    }
    // _SetStrptrMaybe
    {
        atf_amc::Typefconst x;
        vrfy_(value_SetStrptrMaybe(x,"strval1"));
        vrfy_(x.value == 1);
        vrfy_(value_SetStrptrMaybe(x,"strval2"));
        vrfy_(x.value == 2);
        vrfy_(!value_SetStrptrMaybe(x,"strval3"));
        vrfy_(x.value == 2);
    }
    // _SetStrptr
    {
        atf_amc::Typefconst x;
        value_SetStrptr(x,"strval1",atf_amc_Typefconst_value_strval2);
        vrfy_(x.value == 1);
        value_SetStrptr(x,"strval2",atf_amc_Typefconst_value_strval1);
        vrfy_(x.value == 2);
        value_SetStrptr(x,"strval3",atf_amc_Typefconst_value_strval1);
        vrfy_(x.value == 1);
    }
    // print
    {
        cstring s1,s2,s3;
        atf_amc::Typefconst x;

        x.value = 1;
        s1 << x;
        vrfy_(s1 == "strval1");

        x.value = 2;
        s2 << x;
        vrfy_(s2 == "strval2");

        x.value = 3;
        s3 << x;
        vrfy_(s3 == "3");
    }
    // read field
    {
        atf_amc::Typefconst x;
        atf_amc::value_ReadStrptrMaybe(x, "strval1");
        vrfy_(x.value == 1);

        atf_amc::value_ReadStrptrMaybe(x, "strval2");
        vrfy_(x.value == 2);

        atf_amc::value_ReadStrptrMaybe(x, "3");
        vrfy_(x.value == 3);
    }
    // read tuple
    {
        Tuple t;
        attr_Add(t, "value", "strval1");
        atf_amc::Typefconst x;
        Typefconst_ReadTupleMaybe(x,t);
        vrfy_(x.value == 1);

        attr_Find(t,"value")->value = "strval2";
        Typefconst_ReadTupleMaybe(x,t);
        vrfy_(x.value == 2);

        attr_Find(t,"value")->value = "3";
        Typefconst_ReadTupleMaybe(x,t);
        vrfy_(x.value == 3);
    }
}
