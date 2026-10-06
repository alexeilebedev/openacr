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
// Source: cpp/atf_unit/bash.cpp
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
