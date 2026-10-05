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
    atf_comp::FProc &cli = atf_comp::ProcStart("orgfile");
    atf_comp::ProcWrite(cli, "orgfile.move pathname:xxx/yyy tgtfile:y");
    atf_comp::ProcWriteEof(cli);
}

void atf_comp::comptest_orgfile_MoveDot() {
    atf_comp::ProcStart("find test/orgfile -name 'PSX_*' | bin/orgfile -move:.");
}
