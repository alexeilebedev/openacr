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
// Source: cpp/atf_comp/sv2ssim.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_sv2ssim_Convert1() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/2.csv -ctype a.B -schema -data");
}

void atf_comp::comptest_sv2ssim_Convert1Signed() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/2.csv -ctype a.B -schema -data -prefer_signed");
}

void atf_comp::comptest_sv2ssim_Convert2() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/1.csv -ctype a.B -schema -data -outseparator:, -header:N");
}

void atf_comp::comptest_sv2ssim_Convert2Tsv() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/1.csv -ctype a.B -schema -data -outseparator:$'\\t' -header:N");
}

void atf_comp::comptest_sv2ssim_UniqueFieldName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/sv2ssim - -ctype a.B -schema -data");
    atf_comp::ProcWrite(proc, "1,1,1,1,,");
    atf_comp::ProcWrite(proc, "2,2,2,2,2");
}
