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
// Source: cpp/atf_comp/jkv.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_jkv_ArrayFill() {
    atf_comp::ProcStart("$bindir/jkv /dev/null -r -- a.b.c-0:true -10:false");
}

void atf_comp::comptest_jkv_ReverseSmoke() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/jkv - -r");
    atf_comp::ProcWrite(proc, "A.B::C");
    atf_comp::ProcWrite(proc, "A.D:true");
    atf_comp::ProcWrite(proc, "A.E:false");
    atf_comp::ProcWrite(proc, "A.F:null");
    atf_comp::ProcWrite(proc, "A.G-0:0.1");
    atf_comp::ProcWrite(proc, "A.G-1:0.2");
    atf_comp::ProcWrite(proc, "A.G-2:0.3");
    atf_comp::ProcWrite(proc, "A.H:{}");
    atf_comp::ProcWrite(proc, "A.I:0.33");
}

// A tool that writes its output file has to check the write: a full disk or an
// unwritable path otherwise leaves no output and reports success. The output
// path here has a regular file standing where its parent directory would be, so
// the write cannot succeed under any filesystem state or privilege level, and
// jkv names the file and the errno and exits non-zero. The blocking file is
// seeded in the test's own tempdir, which the harness reclaims per run.
void atf_comp::comptest_jkv_WriteFail() {
    atf_comp::ProcStart("bash -c 'touch $tempdir/notadir"
                        " && $bindir/jkv -write -file:$tempdir/notadir/out.json -kv:a=1'");
}

void atf_comp::comptest_jkv_Smoke() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/jkv -");
    atf_comp::ProcWrite(proc, "{ \"A\": { \"B\": \"C\", \"D\": true, \"E\": false, \"F\": null, \"G\": [0.1, 0.2, 0.3], \"H\": {}, \"I\": 0.33 } }");
}
