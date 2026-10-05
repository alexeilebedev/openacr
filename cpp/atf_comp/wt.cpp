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
// Source: cpp/atf_comp/wt.cpp
//
// Comptests for wt. Each test exercises a real sandbox operation inside a
// scratch checkout built by test/wt-scratch (see that script's comment):
// materializing a sandbox copies the checkout, so running wt against the
// real repo would pay for the whole tree on every test, while the scratch
// checkout keeps any materialization to a fraction of a second on any
// host. wt.List feeds the registry on stdin and touches no directory, so
// it runs wt directly.

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_wt_Anon() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.anon.12345 -cow -q 'echo hi' -del");
}

void atf_comp::comptest_wt_Clean() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.clean.xyz -cow -q -clean");
}

void atf_comp::comptest_wt_Command() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.command.xyz -cow -q -del -- echo hello world");
}

void atf_comp::comptest_wt_Create() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.create.xyz -create -cow -q -del true");
}

void atf_comp::comptest_wt_Del() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.del.none -del");
}

void atf_comp::comptest_wt_Diff() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.diff.xyz -cow -q -diff -del true");
}

void atf_comp::comptest_wt_List() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/wt -in:- -list test%");
    atf_comp::ProcWrite(proc, "dev.sandbox  sandbox:test1  comment:\"Test sandbox 1\"");
    atf_comp::ProcWrite(proc, "dev.sandbox  sandbox:test2  cow:Y  comment:\"Test sandbox 2\"");
}

void atf_comp::comptest_wt_Reset() {
    atf_comp::ProcStart("test/wt-scratch $bindir/wt test.reset.xyz -cow -reset -q 'echo done' -del");
}

void atf_comp::comptest_wt_StdinMulti() {
    atf_comp::FProc &proc = atf_comp::ProcStart("test/wt-scratch $bindir/wt test.stdin.xyz -cow -q -i -del");
    atf_comp::ProcWrite(proc, "echo line1");
    atf_comp::ProcWrite(proc, "echo line2");
}
