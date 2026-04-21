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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
// Exceptions: yes
// Source: cpp/atf_comp/orgfile.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_orgfile_Hash() {
    atf_comp::ProcStart("sha1 < test/orgfile/a.txt | awk '{print $1}'");
}

void atf_comp::comptest_orgfile_MoveByDate() {
    atf_comp::ProcStart("find test/orgfile -name 'PSX_*' | bin/orgfile -move:test/orgfile/%Y/%m/%d/");
}

void atf_comp::comptest_orgfile_Dedup() {
    atf_comp::ProcStart("bash -c 'for X in a b c; do echo test/orgfile/$X.txt; done | bin/orgfile -dedup:%'");
}

void atf_comp::comptest_orgfile_DedupPathregx() {
    atf_comp::ProcStart("bash -c '(echo test/orgfile/b.txt; find test/orgfile -name \"*.txt\") | bin/orgfile -dedup:\"%/a.txt\"'");
}

void atf_comp::comptest_orgfile_MoveNoop() {
    atf_comp::ProcStart("find test/orgfile -name 'PSX_*' | bin/orgfile -move:test/orgfile/");
}

void atf_comp::comptest_orgfile_ConsumeInput() {
    atf_comp::ProcStart("echo 'orgfile.move pathname:xxx/yyy tgtfile:y' | orgfile");
}

void atf_comp::comptest_orgfile_MoveDot() {
    atf_comp::ProcStart("find test/orgfile -name 'PSX_*' | bin/orgfile -move:.");
}
