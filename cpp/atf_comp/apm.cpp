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
// and nothing here depends on how deep the tempdir sits.
void atf_comp::comptest_apm_DryRun() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/apm -in:$$OLDPWD/data openacr -e -dry_run"
                        " && test ! -e temp && echo apm.no_script'");
}

// Every action reads the records it operates on back from temp/apm.ours.ssim,
// the file the same run wrote them to, and the write deletes that file before
// rewriting it. A write that fails therefore leaves nothing to read, and a
// removal built from an empty record set deletes no record and no git file while
// reporting the package removed. The write has to be checked.
// The test puts a directory where temp/apm.ours.ssim must go, so the write
// cannot succeed under any filesystem state or privilege level. It runs the same
// removal twice in one shell, blocked and then unblocked, and echoes each exit
// code: the blocked run names the file and the errno, emits no plan and exits
// non-zero, and the control run that follows prints the two-record plan it would
// run and exits 0. The two runs share one shell because they take turns owning
// temp/, which two processes started together could not.
// Both runs work from the test's own tempdir with bin/ and data/ symlinked in
// from the checkout root, because apm reaches acr through the relative bin entry
// on PATH and the schema through data/, and the file under test is named
// relative to the current directory. -pkgdata names one package with two keys,
// which is what keeps the plan two records long, and -dry_run keeps either run
// from touching the checkout.
void atf_comp::comptest_apm_RecfileWriteFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -sfn $$OLDPWD/bin bin && ln -sfn $$OLDPWD/data data"
                        " && rm -rf temp && mkdir -p temp/apm.ours.ssim && {"
                        " $$OLDPWD/$bindir/apm -in:data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -remove -dry_run;"
                        " echo apm.blocked_exit:$$?;"
                        " rm -rf temp; mkdir -p temp;"
                        " $$OLDPWD/$bindir/apm -in:data -pkgdata:$$OLDPWD/test/apm/one_package.ssim apm -remove -dry_run;"
                        " echo apm.control_exit:$$?; }'");
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
// disturb the checkout.
void atf_comp::comptest_apm_TransactionExit() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p bin temp && ln -sfn $$OLDPWD/data data"
                        " && for fate in 0 1 127 255; do"
                        " echo \"#!/bin/bash\" > bin/acr; echo \"exit $$fate\" >> bin/acr; chmod +x bin/acr;"
                        " $$OLDPWD/$bindir/apm -in:data openacr -e;"
                        " echo \"apm.transaction_exit  script:exit-$$fate  exit:$$?\";"
                        " done;"
                        " echo \"#!/bin/bash\" > bin/acr; echo \"kill -TERM \\$$PPID\" >> bin/acr;"
                        " echo \"exec 1>&- 2>&-\" >> bin/acr; echo \"sleep 5\" >> bin/acr; chmod +x bin/acr;"
                        " $$OLDPWD/$bindir/apm -in:data openacr -e;"
                        " echo \"apm.transaction_exit  script:sigterm  exit:$$?\"'");
}
