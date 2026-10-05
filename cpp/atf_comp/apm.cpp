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
// Source: cpp/atf_comp/apm.cpp
//
// Comptests for apm, the package manager. Every apm action ends in one
// transaction: a shell script apm composes and then either runs or, under
// -dry_run, prints. These tests pin that transaction's contract.

#include "include/algo.h"
#include "include/atf_comp.h"

// A dry run reports the plan instead of running it, so it cannot depend on the
// transaction script being writable. The test runs apm from its own tempdir,
// which holds no temp/ subdirectory, so temp/apm.script.sh cannot be created
// there; the plan is printed anyway and the run exits 0. The trailing test is
// the other half of the contract: no script is left behind. The action under
// test is -e, whose whole plan is one acr command; an update's plan is computed
// from files under temp/, and a dry run of one writes those files.
// apm and the schema are reached through $OLDPWD, the checkout root the test
// cd'd out of, so the run reads the real data/ while writing nowhere near it
// and nothing here depends on how deep the tempdir sits.  The package comes
// from test/apm/one_package.ssim, since a tree that publishes no package
// definitions has none to name.
void atf_comp::comptest_apm_DryRun() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/apm -in:$$OLDPWD/data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -e -dry_run"
                        " && test ! -e temp && echo apm.no_script'");
}

// An update is a chain of steps, and each step runs a child process whose output
// a later step reads back out of a file. The sides of the record merge are two
// such files, the merged records a third, and the table of files to merge a
// fourth. Every one of those files is truncated by the redirect before its child
// runs, so a child that fails leaves an empty file rather than no file -- and an
// empty file is a legitimate value at every point it is read. An empty "theirs"
// says the incoming version of the package deleted every one of its records, and
// the plan computed from it deletes the package's records from the tree with
// nothing to put back. An update whose step failed therefore has to stop at that
// step rather than compose a plan out of what the step did not produce.
// The test drives the whole chain once per step, failing that step while every
// other step succeeds, and pins apm's exit code and the size of the plan it
// printed for each. Stubs in the test's own bin/ stand in for the children so
// that any one of them can be made to fail; every stub not chosen as the fate
// hands the call on to the real tool, so the plan the control run prints is the
// one the real merge rules produce. The control is the run that fails no step:
// it prints a two-record plan and exits 0, which is what shows that the checks
// added for the other runs cannot fire on a healthy update.
// The fixture lives in test/apm/update_fate.sh, which explains each stub, and
// the run works from the test's own tempdir with -dry_run, so no fate here can
// reach the checkout.
void atf_comp::comptest_apm_UpdateFate() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && bash $$OLDPWD/test/apm/update_fate.sh $$OLDPWD $bindir'");
}

// A transaction that fails has to exit with the transaction script's own exit
// code, because that code is the machine-readable result the caller reads. The
// process apm waits for is the bash running the script, and waitpid reports a
// wait status rather than an exit code: a script that exits 1 yields the status
// 256, whose low eight bits are zero, and a script killed by SIGTERM yields the
// status 15, the same word a script calling exit(15) produces. So the status
// has to be decoded before it becomes apm's own exit code.
// The test pins the whole table of fates a transaction script can meet. The one
// command an -e plan runs is bin/acr, named relative to the current directory,
// so a shim under the test's own bin/ chooses the fate: exit 0, exit 1, exit
// 127, exit 255, and death by SIGTERM, which the shim delivers to the bash
// running the plan by signalling its own parent. Each exit code has to come
// back unchanged and the signal has to come back as 143, the shell convention
// of 128 plus the signal number. Exit 0 and exit 255 are the controls.
// The shim that signals its parent outlives it, and it inherited the pipe the
// harness reads the test's output from, so it closes its own copy before it
// sleeps out its lifetime; a shim that kept the pipe open would hold the test
// running for as long as it slept.
// The run works from the test's own tempdir with data/ symlinked in from the
// checkout root, and -e writes nothing outside temp/, so no fate here can
// disturb the checkout.  The package comes from test/apm/one_package.ssim, as
// in apm.DryRun.
void atf_comp::comptest_apm_TransactionExit() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p bin temp && ln -sfn $$OLDPWD/data data"
                        " && for fate in 0 1 127 255; do"
                        " echo \"#!/bin/bash\" > bin/acr; echo \"exit $$fate\" >> bin/acr; chmod +x bin/acr;"
                        " $$OLDPWD/$bindir/apm -in:data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -e;"
                        " echo \"apm.transaction_exit  script:exit-$$fate  exit:$$?\";"
                        " done;"
                        " echo \"#!/bin/bash\" > bin/acr; echo \"kill -TERM \\$$PPID\" >> bin/acr;"
                        " echo \"exec 1>&- 2>&-\" >> bin/acr; echo \"sleep 5\" >> bin/acr; chmod +x bin/acr;"
                        " $$OLDPWD/$bindir/apm -in:data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -e;"
                        " echo \"apm.transaction_exit  script:sigterm  exit:$$?\"'");
}

// A hand-written source references each record whose generated symbol its code
// names.  The test writes srcscan.cpp, which compares against
// dmmeta_Reftype_reftype_Val, calls dmmeta::Reftype_Print, and mentions two
// other reftype constants only in a comment and a string.  Package srcref
// takes the file with ref:Y, so it gains the Val row and the ctype and neither
// of the other two rows.  Package srcbare takes the file alone, so -check
// reports both records as dangling.  srcmixed.cpp defines the comptest functions
// of apm.DryRun and apm.SrcScan and calls apm.UpdateFate's from a body, and
// package srcmixed builds atf_comp and carries DryRun and UpdateFate, so -check
// reports the file as mixed: one record carried, one missing.  The call is a
// use, so UpdateFate does not count on either side.
void atf_comp::comptest_apm_SrcScan() {
    atf_comp::ProcStart("bash -c 'top=$$PWD && cd $tempdir && mkdir -p d/amcdb d/gendb d/dmmeta d/dev"
                        " && ln -s $$top/data/amcdb/bltin.ssim d/amcdb/"
                        " && ln -s $$top/data/gendb/cppsym.ssim d/gendb/"
                        " && for t in ctype field ns ssimfile ssimreq ssimsort substr; do ln -s $$top/data/dmmeta/$$t.ssim d/dmmeta/; done"
                        " && grep ^dev.package $$top/test/apm/srcscan_pkg.ssim > d/dev/package.ssim"
                        " && grep ^dev.pkgkey $$top/test/apm/srcscan_pkg.ssim > d/dev/pkgkey.ssim"
                        " && touch d/dev/pkgdep.ssim"
                        " && printf \"// dmmeta_Reftype_reftype_Ptr appears only in this comment\\n\" > srcscan.cpp"
                        " && printf \"bool ValQ(dmmeta::Reftype &row) {\\n\" >> srcscan.cpp"
                        " && printf \"    const char *text = \\\"dmmeta_Reftype_reftype_Lary\\\";\\n\" >> srcscan.cpp"
                        " && printf \"    dmmeta::Reftype_Print(row, out);\\n\" >> srcscan.cpp"
                        " && printf \"    return row.reftype == dmmeta_Reftype_reftype_Val;\\n}\\n\" >> srcscan.cpp"
                        " && printf \"void atf_comp::comptest_apm_DryRun() {\\n    atf_comp::comptest_apm_UpdateFate();\\n}\\n\" > srcmixed.cpp"
                        " && printf \"void atf_comp::comptest_apm_SrcScan() {\\n}\\n\" >> srcmixed.cpp"
                        " && cp $$top/test/apm/srcscan.ssim recs.ssim && echo dev.gitfile  gitfile:$tempdir/srcscan.cpp >> recs.ssim"
                        " && echo dev.gitfile  gitfile:$tempdir/srcmixed.cpp >> recs.ssim"
                        " && echo dev.targsrc  targsrc:atf_comp/$tempdir/srcmixed.cpp >> recs.ssim"
                        " && cd $$top"
                        " && $bindir/apm srcref -showrec -in:$tempdir/d -data_in:$tempdir/recs.ssim"
                        " && $bindir/apm srcbare -check -in:$tempdir/d -data_in:$tempdir/recs.ssim 2>&1 | grep apm.dangling"
                        "; $bindir/apm srcmixed -check -in:$tempdir/d -data_in:$tempdir/recs.ssim 2>&1 | grep apm.mixedfile'");
}
