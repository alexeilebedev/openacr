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
// Source: cpp/atf_comp/src_hdr.cpp -- Comptests for src_hdr
//
// Comptests for src_hdr, the source-header rewriter.
// Shared fixture: test/src_hdr/onescript.ssim is a single-file dataset
// whose scriptfile row names ro/x.sh in the test's tempdir; each test
// seeds that script, then runs src_hdr -write over the dataset. src_hdr
// ends a -write run by spawning src_func -updateproto, so a test that must
// get past that step installs a stub bin/src_func in the tempdir (PATH
// carries the relative entry bin). Write-denial tests compose their
// command with DenyWriteCmd (cpp/atf_comp/proc.cpp).

#include "include/algo.h"
#include "include/atf_comp.h"

// Run src_hdr -write twice over a headerless script, and check that the second run
// leaves the file byte-identical (second_run_noop).  The first run adds the header.
// A rewrite that keeps changing its own output grows the header on every
// update-hdr run.
void atf_comp::comptest_src_hdr_Converge() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro bin && echo \"echo hi\" > ro/x.sh && echo \"exit 0\" > bin/src_func && chmod +x bin/src_func && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:% && cp ro/x.sh first && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:% && cmp ro/x.sh first && echo second_run_noop'");
}

// Run src_hdr -write against a scriptfile row whose file does not exist, and check
// that the run fails naming the path.  A run that skipped the file silently and
// exited 0 would let a stale scriptfile row pass every run unnoticed.
void atf_comp::comptest_src_hdr_FileReadFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%'");
}

// Run src_hdr -write against a script whose directory denies the write, through the
// DenyWriteCmd scaffold, and check that the failed header rewrite fails the run,
// naming the path.  A run that exited 0 would leave the file silently unchanged.
void atf_comp::comptest_src_hdr_FileWriteFail() {
    atf_comp::ProcStart(DenyWriteCmd("echo \"echo hi\" > ro/x.sh",
                                     "$$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%"));
}

// Run src_hdr -write against a .json scriptfile, whose comment syntax is unknown.
// Check that the run exits 1 naming the file, and that cmp finds the file
// byte-unchanged (file_unchanged).
// Every line of such a file would parse as header comment, and the rewrite would
// replace the whole file with a bare separator.  An extensionless file with no
// shebang to guess from has an unknown comment syntax too.
void atf_comp::comptest_src_hdr_NoCmtstring() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro"
                        " && printf \"{}\\n\" > ro/x.json"
                        " && cp ro/x.json orig"
                        " && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/nocmt.ssim -write -scriptfile:%;"
                        " test $$? = 1 && cmp ro/x.json orig && echo file_unchanged'");
}

// Run src_hdr -write with a stub bin/src_func that exits 1, so the header write
// succeeds and the trailing src_func -updateproto step fails.  Check that the run
// fails naming the step, because the update-hdr blocks stay stale.
void atf_comp::comptest_src_hdr_UpdateprotoFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro bin && echo \"echo hi\" > ro/x.sh && echo \"exit 1\" > bin/src_func && chmod +x bin/src_func && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%'");
}
