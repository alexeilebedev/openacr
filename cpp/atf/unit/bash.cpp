// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/bash.cpp
//

#include "include/atf_unit.h"

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_PrintBash() {
    {
        tempstr temp;
        strptr_PrintBash("", temp);
        vrfyeq_(temp,"''");
    }
    {
        tempstr temp;
        strptr_PrintBash("test", temp);
        vrfyeq_(temp,"test");
    }
    {
        tempstr temp;
        strptr_PrintBash("test test", temp);
        vrfyeq_(temp,"'test test'");
    }
    {
        tempstr temp;
        strptr_PrintBash("test 'test", temp);
        vrfyeq_(temp,"$'test \\'test'");
    }
    {
        tempstr temp;
        strptr_PrintBash("test \012test", temp);
        vrfyeq_(temp,"$'test \\ntest'");
    }
    // make sure all chars can be printed...
    for (int i=0; i<256; i++) {
        tempstr str("x");
        str.ch_elems[0] = i;
        tempstr out;
        strptr_PrintBash(str,out);
    }
}
