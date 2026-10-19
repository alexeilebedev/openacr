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

// Ask jkv to write $tempdir/notadir/out.json, where a regular file stands in place
// of the parent directory, and check that jkv names the file and the errno and exits
// non-zero.
// A tool that writes its output file has to check the write.  Otherwise a full disk
// or an unwritable path leaves no output while the tool reports success.  The
// blocking file makes the write fail under any filesystem state or privilege level.
// The test seeds it in its own tempdir, which the harness reclaims per run.
void atf_comp::comptest_jkv_WriteFail() {
    atf_comp::ProcStart("bash -c 'touch $tempdir/notadir"
                        " && $bindir/jkv -write -file:$tempdir/notadir/out.json -kv:a=1'");
}

void atf_comp::comptest_jkv_Smoke() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/jkv -");
    atf_comp::ProcWrite(proc, "{ \"A\": { \"B\": \"C\", \"D\": true, \"E\": false, \"F\": null, \"G\": [0.1, 0.2, 0.3], \"H\": {}, \"I\": 0.33 } }");
}
