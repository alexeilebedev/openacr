// Copyright (C) 2026 AlgoX2 Corp
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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
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
