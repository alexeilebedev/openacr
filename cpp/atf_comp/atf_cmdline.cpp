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
// Source: cpp/atf_comp/atf_cmdline.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_atf_cmdline_Bare() {
    atf_comp::ProcStart("$bindir/atf_cmdline");
}

void atf_comp::comptest_atf_cmdline_Debug() {
    atf_comp::ProcStart("$bindir/atf_cmdline -debug blah -str blah");
}

void atf_comp::comptest_atf_cmdline_Help() {
    atf_comp::ProcStart("$bindir/atf_cmdline -help");
}

void atf_comp::comptest_atf_cmdline_Minimal() {
    atf_comp::ProcStart("$bindir/atf_cmdline blah -str blah");
}

void atf_comp::comptest_atf_cmdline_MinimalExec() {
    atf_comp::ProcStart("$bindir/atf_cmdline -exec blah -str blah");
}

void atf_comp::comptest_atf_cmdline_Rich() {
    atf_comp::ProcStart("$bindir/atf_cmdline -str -STR -num 12 -dbl -0.0003 -flag -dstr -DSTR -dnum -45 -ddbl -0.123 -dflag:N -mstr -MSTR1 -mstr -MSTR2 -mstr -MSTR3 -mnum -1 -mnum -2 -mnum -3 -mdbl -1.1 -mdbl -2.2 -mdbl -3.2 -fconst medium -cconst May -- -ASTR -200 0.0002 Y -100 -200 -300");
}

void atf_comp::comptest_atf_cmdline_RichExec() {
    atf_comp::ProcStart("$bindir/atf_cmdline -exec -verbose -verbose -verbose -debug -str -STR -num 12 -dbl -0.0003 -flag -dstr -DSTR -dnum -45 -ddbl -0.123 -dflag:N -mstr -MSTR1 -mstr -MSTR2 -mstr -MSTR3 -mnum -1 -mnum -2 -mnum -3 -mdbl -1.1 -mdbl -2.2 -mdbl -3.2 -fconst medium -cconst:May -- -ASTR -200 0.0002 Y -100 -200 -300");
}

void atf_comp::comptest_atf_cmdline_Sig() {
    atf_comp::ProcStart("$bindir/atf_cmdline -sig");
}

void atf_comp::comptest_atf_cmdline_Verbose() {
    atf_comp::ProcStart("$bindir/atf_cmdline -verbose -verbose blah -str blah");
}

void atf_comp::comptest_atf_cmdline_Version() {
    atf_comp::ProcStart("$bindir/atf_cmdline -version");
}
