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

// Run apm -e -dry_run from a tempdir that holds no temp/ subdirectory, and check
// that apm prints the plan, exits 0 and leaves no temp/ behind (apm.no_script).
// A dry run reports the plan and skips running it, so it must work without a
// writable transaction script.  apm fails to create temp/apm.script.sh in this
// tempdir.  The test uses -e because its whole plan is one acr command.  An update
// computes its plan from files under temp/, and a dry run of one writes those files.
// The test reaches apm and the schema through $OLDPWD, the checkout root that it
// cd'd out of.  The run thus reads the real data/ and writes nowhere near it, at any
// tempdir depth.  The package comes from test/apm/one_package.ssim, because a tree
// that publishes no package definitions has none to name.
void atf_comp::comptest_apm_DryRun() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/apm -in:$$OLDPWD/data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -e -dry_run"
                        " && test ! -e temp && echo apm.no_script'");
}

// Run apm's update chain once per step with test/apm/update_fate.sh, failing that
// one step each time, and check that the update stops at the failed step:
// 1. Put stubs for the children in the test's own bin/.  The stub chosen as the
//    fate fails, and every other stub hands the call on to the real tool.
// 2. Run the control, which fails no step, and check that it prints a two-record
//    plan and exits 0.
// 3. Run the update with -dry_run from the test's own tempdir once per step, and
//    check apm's exit code and the size of the plan it prints for each run.
// An update is a chain of steps.  Each step runs a child process whose output a
// later step reads back out of a file.  The two sides of the record merge are two
// such files, the merged records a third, and the table of files to merge a fourth.
// The redirect truncates each file before its child runs, so a failed child leaves
// an empty file in place.  An empty file is a legitimate value at every point that
// apm reads one.  An empty "theirs" says the incoming version of the package
// deleted every one of its records.  The plan computed from it deletes the
// package's records from the tree and puts none back.  An update whose step failed
// therefore has to stop at that step.
// The control shows that the checks added for the other runs stay silent on a
// healthy update.  Its plan is the one the real merge rules produce, because every
// stub it meets hands on to the real tool.  test/apm/update_fate.sh explains each
// stub.  The run uses -dry_run from its own tempdir, so no fate reaches the
// checkout.
void atf_comp::comptest_apm_UpdateFate() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && bash $$OLDPWD/test/apm/update_fate.sh $$OLDPWD $bindir'");
}

// Run an apm transaction whose script meets each fate in turn, and check that apm
// exits with the script's own exit code:
// 1. Symlink data/ from the checkout root into the test's tempdir.
// 2. For each of exit 0, 1, 127 and 255, write a bin/acr shim that exits with that
//    code, run apm -e, and check that the exit code comes back unchanged.
// 3. Write a shim that sends SIGTERM to its parent, the bash running the plan, and
//    then closes its output and sleeps.  Run apm -e, and check that apm exits 143.
// The caller reads the script's exit code as the machine-readable result.  apm
// waits for the bash running the script, and waitpid reports a wait status for it.
// A script that exits 1 yields the status 256, whose low eight bits are zero.  A
// script killed by SIGTERM yields the status 15, the same word that exit(15)
// produces.  apm therefore decodes the status before it becomes apm's own exit
// code.  143 is the shell convention of 128 plus the signal number.  Exit 0 and
// exit 255 are the controls.
// The one command an -e plan runs is bin/acr, named relative to the current
// directory, so a shim under the test's own bin/ chooses the fate.  The signalling
// shim outlives its parent, and it inherited the pipe that the harness reads the
// test's output from.  It closes its own copy before it sleeps out its lifetime.  A
// shim that kept the pipe open would hold the test running for as long as it slept.
// The run works from the test's own tempdir, and -e writes only under temp/, so no
// fate disturbs the checkout.  The package comes from test/apm/one_package.ssim, as
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

// Scan hand-written sources for the records that their code names, and check what
// apm carries and what -check reports:
// 1. Build a small data directory in the tempdir from test/apm/srcscan_pkg.ssim and
//    links into data/.
// 2. Write srcscan.cpp, which compares against dmmeta_Reftype_reftype_Val, calls
//    dmmeta::Reftype_Print, and mentions two other reftype constants only in a
//    comment and a string.
// 3. Write srcmixed.cpp, which defines the comptest functions of apm.DryRun and
//    apm.SrcScan and calls apm.UpdateFate's from a body.
// 4. Run apm srcref -showrec, where package srcref takes srcscan.cpp with ref:Y.
//    Check that it gains the Val row and the ctype, and neither of the other two
//    rows.
// 5. Run apm srcbare -check, where package srcbare takes the file alone, and check
//    that it reports both records as dangling (apm.dangling).
// 6. Run apm srcmixed -check, where package srcmixed builds atf_comp and carries
//    DryRun and UpdateFate.  Check that it reports srcmixed.cpp as mixed
//    (apm.mixedfile): one record carried, one missing.
// A hand-written source references each record whose generated symbol its code
// names.  The call to UpdateFate's function is a use, so UpdateFate counts on
// neither side.
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

// A package ships a file of this tree at another path of its projection when a
// dev.pkgfile row maps it, and ships its front page as txt/README.md when it
// names a readmefile.  The test pushes package pkg, which builds on package base,
// into a fresh origin.  base carries a txt/README.md of its own, and pkg's front
// page has to land there in its place.  pkg's mapped file has to land at
// AGENTS.md, and both sources still go out at their own paths.  A second push,
// with the mapping gone, deletes AGENTS.md from the origin.  Each push prints its
// exit code and every file the origin then tracks, with the file's first line.
// The fixture, test/apm/pkgfile.sh, stands up a small tree of its own in the
// test's tempdir, so the packages and files of the checkout take no part.
void atf_comp::comptest_apm_Pkgfile() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && bash $$OLDPWD/test/apm/pkgfile.sh $$OLDPWD $bindir'");
}
