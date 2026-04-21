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
// Source: cpp/atf_comp/mdbg.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_mdbg_OutOfOrderArgs() {
    atf_comp::ProcStart("$bindir/mdbg -args:x y");
}

void atf_comp::comptest_mdbg_Smoke() {
    atf_comp::ProcStart("$bindir/mdbg -dry_run mdbg mdbg");
}

void atf_comp::comptest_mdbg_SmokeBreak() {
    atf_comp::ProcStart("$bindir/mdbg -dry_run -tui mdbg mdbg -b a,b,c,d,e");
}

void atf_comp::comptest_mdbg_SmokeBreak2() {
    atf_comp::ProcStart("$bindir/mdbg -dry_run -tui mdbg mdbg -b a -b b -b c");
}
