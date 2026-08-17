// Copyright (C) 2026 AlgoRND
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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/acr_ed.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_ed_CreateCtype() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ctype ams.ChildTypeMsg -subset ams.MsgHeader -msgtype:400 -reftype:Base");
}

void atf_comp::comptest_acr_ed_CreateMsg() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ctype ams.AbcMsg -subset ams.MsgHeader");
}

void atf_comp::comptest_acr_ed_CreateSrcfileTarget() {
    atf_comp::ProcStart("$bindir/acr_ed -create -srcfile:cpp/acr_ed/blah.cpp -target:algo_lib");
}

void atf_comp::comptest_acr_ed_CreateSsimfile() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ssimfile dmmeta.xyz");
}

void atf_comp::comptest_acr_ed_CreateSsimfileBadNs() {
    atf_comp::ProcStart("$bindir/acr_ed --create -ssimfile ams.test -subset algo.Smallstr10");
}

void atf_comp::comptest_acr_ed_CreateTarget() {
    atf_comp::ProcStart("$bindir/acr_ed -create -target xyz");
}
