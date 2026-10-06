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
