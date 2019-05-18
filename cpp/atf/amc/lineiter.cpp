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
// Source: cpp/atf/amc/lineiter.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
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
