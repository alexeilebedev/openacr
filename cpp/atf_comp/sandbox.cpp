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
// Source: cpp/atf_comp/sandbox.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_sandbox_Anon() {
    atf_comp::ProcStart("$bindir/sandbox test.anon.12345 -reset -dry_run");
}

void atf_comp::comptest_sandbox_Clean() {
    atf_comp::ProcStart("$bindir/sandbox amc -clean -dry_run -q");
}

void atf_comp::comptest_sandbox_Command() {
    atf_comp::ProcStart("$bindir/sandbox amc -dry_run -q -- echo hello world");
}

void atf_comp::comptest_sandbox_Create() {
    atf_comp::ProcStart("$bindir/sandbox test.create.xyz -create -dry_run");
}

void atf_comp::comptest_sandbox_Del() {
    atf_comp::ProcStart("$bindir/sandbox amc -del -dry_run -q");
}

void atf_comp::comptest_sandbox_Diff() {
    atf_comp::ProcStart("$bindir/sandbox amc -diff -dry_run -q");
}

void atf_comp::comptest_sandbox_Gc() {
    atf_comp::ProcStart("$bindir/sandbox amc -gc -dry_run -q");
}

void atf_comp::comptest_sandbox_List() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/sandbox -in:- -list -dry_run test%");
    atf_comp::ProcWrite(proc, "dev.sandbox  sandbox:test1  comment:\"Test sandbox 1\"");
    atf_comp::ProcWrite(proc, "dev.sandbox  sandbox:test2  comment:\"Test sandbox 2\"");
}

void atf_comp::comptest_sandbox_Pull() {
    atf_comp::ProcStart("$bindir/sandbox amc -pull -dry_run -q");
}

void atf_comp::comptest_sandbox_Reset() {
    atf_comp::ProcStart("$bindir/sandbox amc -reset -dry_run -q -refs:'HEAD master'");
}

void atf_comp::comptest_sandbox_StdinMulti() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/sandbox amc -dry_run -q");
    atf_comp::ProcWrite(proc, "echo line1");
    atf_comp::ProcWrite(proc, "echo line2");
}
