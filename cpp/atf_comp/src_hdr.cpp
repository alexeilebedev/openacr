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

// src_hdr -write twice over a headerless script: the first run adds the
// header, and the second run must leave the file byte-identical -- a
// rewrite that keeps changing its own output grows the header on every
// update-hdr run.
void atf_comp::comptest_src_hdr_Converge() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro bin && echo \"echo hi\" > ro/x.sh && echo \"exit 0\" > bin/src_func && chmod +x bin/src_func && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:% && cp ro/x.sh first && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:% && cmp ro/x.sh first && echo second_run_noop'");
}

// src_hdr -write against a scriptfile row whose file does not exist (no
// seed): the run must fail naming the path instead of exiting 0 with the
// file silently skipped -- a stale scriptfile row would otherwise pass
// every run unnoticed.
void atf_comp::comptest_src_hdr_FileReadFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%'");
}

// src_hdr -write against a script whose directory denies the write (the
// DenyWriteCmd scaffold): the header rewrite fails, and the run must fail
// naming the path instead of exiting 0 with the file silently unchanged.
void atf_comp::comptest_src_hdr_FileWriteFail() {
    atf_comp::ProcStart(DenyWriteCmd("echo \"echo hi\" > ro/x.sh",
                                     "$$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%"));
}

// src_hdr -write against a scriptfile whose comment syntax is unknown (a
// .json file; likewise an extensionless file with no shebang to guess from):
// every line would parse as header comment and the rewrite would replace the
// whole file with a bare separator, so the run must fail naming the file,
// pinned byte-unchanged (cmp) next to exit code 1.
void atf_comp::comptest_src_hdr_NoCmtstring() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro"
                        " && printf \"{}\\n\" > ro/x.json"
                        " && cp ro/x.json orig"
                        " && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/nocmt.ssim -write -scriptfile:%;"
                        " test $$? = 1 && cmp ro/x.json orig && echo file_unchanged'");
}

// src_hdr -write whose header write succeeds but whose trailing
// src_func -updateproto step fails (the stub bin/src_func exits 1): the run
// must fail naming the step, since the update-hdr blocks are left stale.
void atf_comp::comptest_src_hdr_UpdateprotoFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir ro bin && echo \"echo hi\" > ro/x.sh && echo \"exit 1\" > bin/src_func && chmod +x bin/src_func && $$OLDPWD/$bindir/src_hdr -in:$$OLDPWD/test/src_hdr/onescript.ssim -write -scriptfile:%'");
}
