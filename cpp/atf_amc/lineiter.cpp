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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/lineiter.cpp
//

#include "include/atf_amc.h"


void atf_amc::amctest_LineIter() {
    // line, line, empty line with carriage return
    {
        strptr text("abc\ndef\n\r\n");
        int idx = 0;
        ind_beg(Line_curs, line, text) {
            if (idx==0) vrfyeq_(line, "abc");
            if (idx==1) vrfyeq_(line, "def");
            if (idx==2) vrfyeq_(line, "");
            vrfyeq_(idx, ind_curs(line).i);
            idx++;
        }ind_end;
        vrfyeq_(idx, 3);
    }

    // empty line
    {
        strptr text("");
        int idx = 0;
        ind_beg(Line_curs, line, text) {
            (void)line;
            vrfyeq_(idx, ind_curs(line).i);
            idx++;
        }ind_end;
        vrfyeq_(idx, 0);
    }

    // single unterminated line
    {
        strptr text("abc");
        int idx = 0;
        ind_beg(Line_curs, line, text) {
            if (idx == 0) vrfyeq_(line, "abc");
            vrfyeq_(idx, ind_curs(line).i);
            idx++;
        }ind_end;
        vrfyeq_(idx, 1);
    }
}
