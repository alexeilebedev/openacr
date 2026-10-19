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
// Source: cpp/atf_comp/acr.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_BadInsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
}

// Feed acr a file with a line the tuple parser rejects, and check that the line
// fails the run and blocks the -write:
// 1. Write a three-line file whose second line has an unterminated quote.
// 2. Insert a record into the file with acr -insert -write.
// 3. Check that acr reports the bad line with its file:line and exits nonzero.
// 4. Check that the file still holds every line it held, the one that failed to
//    load included.
void atf_comp::comptest_acr_BadLine() {
    atf_comp::ProcStart("bash -c 'printf \"gendb.dispsig  dispsig:a.X  signature:1\\ngendb.dispsig  dispsig:b.Y  signature:\\042unterminated\\ngendb.dispsig  dispsig:c.Z  signature:3\\n\" > $tempdir/t.ssim"
                        "; echo gendb.dispsig  dispsig:d.W  signature:4 | $bindir/acr -insert -write -in:$tempdir/t.ssim %"
                        "; echo acr_exit:$?; cat $tempdir/t.ssim'");
}

// Run acr -version with its stdout closed, and check that acr exits nonzero.
// A tool whose output was lost has failed its job, even though what it printed
// was right.  The closed stdout makes the write fail with EBADF on every platform
// and raises no signal, so the exit code is the only report of the loss.
// -version prints before acr opens any file of its own, so descriptor 1 is still
// closed when acr writes the line.
void atf_comp::comptest_acr_OutputLost() {
    atf_comp::ProcStart("bash -c '$bindir/acr -version >&-; echo acr_exit:$?'");
}

void atf_comp::comptest_acr_BadNs() {
    // Check against a small fixed namespace set fed via -in, so the
    // "Valid values" suggestion does not depend on the live namespace list.
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:<(echo 'dmmeta.ns  ns:foo  nstype:exe  license:GPL  comment:\"\"' && echo 'dmmeta.ns  ns:bar  nstype:lib  license:GPL  comment:\"\"') -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:x.Y");
}

// Insert a dmmeta.cpbuf row whose syntax is Proto3, a misspelling of a syntax the
// codec knows, and check that acr -insert -check rejects the row, names the
// attribute and lists proto2 and proto3 as the valid values.
// cpbuf.syntax references the dmmeta.pbsyntax table, so the reference check
// catches the misspelling before it silently selects the proto2 wire shape.  The
// pbsyntax and ctype rows come in through -in, so the list of valid values stays
// independent of the live database.
void atf_comp::comptest_acr_BadPbufSyntax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:<(echo 'dmmeta.pbsyntax  pbsyntax:proto2  comment:\"\"' && echo 'dmmeta.pbsyntax  pbsyntax:proto3  comment:\"\"' && echo 'dmmeta.ctype  ctype:pbt.Msg  comment:\"\"') -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:Proto3");
}

void atf_comp::comptest_acr_BadPkey() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.ctype");
}

void atf_comp::comptest_acr_BadReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.field field:command.acr_ed.blah arg:bool");
}

void atf_comp::comptest_acr_CascDel() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:ns1 -del");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ns1.c comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ns1.c.d arg:i32 reftype:Val comment:\"Will be deleted as well\"");
}

void atf_comp::comptest_acr_CascDel2() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"Will be inserted and deleted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ns1.c comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ns1.c.d arg:i32 reftype:Val comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns  ns:ns1");
}

void atf_comp::comptest_acr_CascDel3() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -replace");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.field  field:acr.FBltin.base");
}

void atf_comp::comptest_acr_DelField() {
    atf_comp::ProcStart("$bindir/acr field:dmmeta.Ns.license -del");
}

void atf_comp::comptest_acr_DelRecord() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- target:samp -del");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  license:GPL  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:''");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:''");
}

void atf_comp::comptest_acr_DeleteReinsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns  ns:amc");
    atf_comp::ProcWrite(proc, "acr.insert dmmeta.ns  ns:amc  nstype:exe  license:GPL  comment:\"Algo Cross-Reference - ssimfile database & update tool\"");
}

// Point acr's -in at a dataset directory under several modes, and check that acr
// fails naming the directory whenever it lacks search permission:
// 1. Make dataset directory d with one ssimfile, a symlink dlink to d, and a
//    symlink dangling to the missing path nowhere.
// 2. Query through dlink, and check that the query succeeds.
// 3. Install an EXIT trap that restores the mode of d, and a TERM trap that
//    reaches it.
// 4. Query d under mode 500 (readable) and mode 100 (search only), and check that
//    both queries succeed.
// 5. Query d under mode 400 (list only) and mode 000, and check that each query
//    names the directory and exits nonzero.
// 6. Run acr -check over d under mode 000, and check that it names the directory
//    and exits nonzero.
// 7. Restore mode 755, query through dangling, and check that the query exits
//    nonzero.
// The directory exists, so acr reads it in dataset mode and opens each table on
// demand.  Without the probe of the directory, acr would report the failure one
// ssimfile at a time and never name the directory.  A run that reaches no table
// would report success over no data at all.  A -check is such a run.  It would
// answer records:0 n_err:0 for a dataset it failed to open, so the integrity gate
// would pass without validating a record.
// The probe tests search (execute) permission, because opening an ssimfile by name
// needs it.  acr never lists the directory, so it accepts the search-only mode
// alongside the readable one.  A symlink resolves to its target.  acr rejects a
// dangling one in the same branch as a mistyped path.
// Steps 5 to 7 check for a nonzero exit, because the exit code counts diagnosed
// failures.  A query that reaches a table over an unsearchable directory diagnoses
// both the directory and that table.
// Every mode the test sets on d withholds write permission.  A d left in one of
// them would block the next run's tempdir wipe and fail that run on the leftover.
// The EXIT trap restores the mode.  The TERM trap reaches the EXIT trap when the
// harness timeout kills the shell.  The same pair guards the
// read-only directory of atf_comp::DenyWriteCmd (cpp/atf_comp/proc.cpp).  A
// mode-000 plain file, such as the one acr.FileReadDeny leaves behind, needs no
// trap, because unlinking it asks for write permission on its parent only.
// Like every chmod-deny fixture, the test assumes an unprivileged runner.  Under
// CAP_DAC_OVERRIDE (a root container) the search succeeds and the golden
// mismatches.
void atf_comp::comptest_acr_DsetDirReadDeny() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p d/gendb"
                        " && printf \"gendb.dispsig  dispsig:xx.aa  signature:1\\n\" > d/gendb/dispsig.ssim"
                        " && ln -s d dlink && ln -s nowhere dangling"
                        " && $$OLDPWD/$bindir/acr -in:dlink -schema:$$OLDPWD/data dispsig:% -print:N && echo symlinked_dir_ok"
                        " && trap \"chmod 755 d\" EXIT && trap \"exit 1\" TERM"
                        " && chmod 500 d"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N && echo readable_dir_ok"
                        " && chmod 100 d"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N && echo searchonly_dir_ok;"
                        " chmod 400 d;"
                        " $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N;"
                        " test $$? != 0 && echo listonly_dir_failed;"
                        " chmod 000 d;"
                        " $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N;"
                        " test $$? != 0 && echo denied_dir_failed;"
                        " $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -check -print:N;"
                        " test $$? != 0 && echo denied_dir_check_failed;"
                        " chmod 755 d;"
                        " $$OLDPWD/$bindir/acr -in:dangling -schema:$$OLDPWD/data dispsig:% -print:N;"
                        " test $$? != 0 && echo dangling_symlink_failed'");
}

// Load a dataset that holds unreadable ssimfiles, and check that every read of one
// fails the run naming the path and leaves the file and the worktree alone:
// 1. Make dataset d with one dispsig row, and keep a copy of its ssimfile.
// 2. Query an empty ssimfile and an absent one, and check that each loads as an
//    empty table and exits 0.
// 3. Insert a dev.gitfile row with acr -insert -write -g.  Check that the script
//    runs and creates the marker, and that acr exits 1 with the acr.git_script
//    diagnostic.
// 4. Keep a copy of dev/gitfile.ssim, and chmod 000 it and the dispsig ssimfile.
// 5. Query dispsig, and check that the query names the path and exits 1.
// 6. Insert a dispsig row with -insert -write, and check that the run exits 1.
// 7. Edit dispsig with -e -write, and check that the run exits 1 and the editor
//    never launches.
// 8. Insert a dev.gitfile row with -g, and check that the run exits 1 and the
//    script never runs.
// 9. Restore both modes, and check with cmp that both ssimfiles are
//    byte-unchanged.
// Read as an empty table, an unreadable file's rows would be missing from the
// answer.  An -insert -write would then rewrite the file from the rows that did
// load, the inserted one alone, and drop every older row.  The empty and
// the absent ssimfile of step 2 are the two accepted edges of the same rule.
// -g is the other side effect that a failed load must stand down from.  The script
// that -g generates removes, renames and creates the source files that the
// selected dev.gitfile rows name.  A run that leaves those rows unwritten and
// still runs the script moves the worktree away from the database.  The script's
// own `touch` line is the marker, because it creates the inserted row's file when
// the script runs.
// Step 3 is the clean load, which runs the script and doubles as the probe of a
// failing script.  The script stages the file it just created.  The tempdir is
// gitignored, so git refuses the `git add` and the script exits nonzero.  acr
// reports that failure on a worktree the script left half-arranged.  The step
// reads the acr.git_script diagnostic from a file, because git's own complaint
// goes to the captured stream in a wording that depends on the git version.
// Like every chmod-deny fixture, the test assumes an unprivileged runner.  Under
// CAP_DAC_OVERRIDE (a root container) the read succeeds and the golden
// mismatches.
void atf_comp::comptest_acr_DsetFileReadDeny() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p d/dmmeta d/gendb d/dev temp"
                        " && printf \"gendb.dispsig  dispsig:xx.aa  signature:1\\n\" > d/gendb/dispsig.ssim"
                        " && cp d/gendb/dispsig.ssim orig"
                        " && printf \"\" > d/dmmeta/ctype.ssim"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data ctype:% -print:N && echo empty_ssimfile_ok"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data ssimfile:% -print:N && echo absent_ssimfile_ok"
                        " && printf \"dev.gitfile  gitfile:added_src\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -g -print:N 2>giterr;"
                        " test $$? = 1 && test -f added_src && echo git_script_failed;"
                        " grep git_script giterr;"
                        " cp d/dev/gitfile.ssim gitorig;"
                        " chmod 000 d/gendb/dispsig.ssim d/dev/gitfile.ssim;"
                        " $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N;"
                        " test $$? = 1 && echo query_failed;"
                        " printf \"gendb.dispsig  dispsig:xx.bb  signature:2\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -print:N;"
                        " test $$? = 1 && echo write_failed;"
                        " EDITOR=\"touch editor_ran\" $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -e -write -print:N;"
                        " test $$? = 1 && test ! -f editor_ran && echo editor_not_launched;"
                        " printf \"dev.gitfile  gitfile:denied_src\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -g -print:N;"
                        " test $$? = 1 && test ! -f denied_src && echo git_script_not_run;"
                        " chmod 644 d/gendb/dispsig.ssim d/dev/gitfile.ssim;"
                        " cmp d/gendb/dispsig.ssim orig && echo ssimfile_unchanged;"
                        " cmp d/dev/gitfile.ssim gitorig && echo gitfile_unchanged'");
}

// Run acr -e with an editor that changes records in two ssimfiles, and check that
// acr writes the edit back, reports n_file_mod:2 and exits 0.
// The exit code means success or failure only, never the number of files
// modified.  The editor is a cp command that replaces the edit buffer with the two
// edited records.
void atf_comp::comptest_acr_EditFileModCount() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp d d/dmmeta d/gendb"
                        " && printf \"gendb.dispsig  dispsig:xx.aa  signature:1\\n\" > d/gendb/dispsig.ssim"
                        " && printf \"dmmeta.ctype  ctype:xx.Bb  comment:hello\\n\" > d/dmmeta/ctype.ssim"
                        " && printf \"gendb.dispsig  dispsig:xx.aa  signature:2\\ndmmeta.ctype  ctype:xx.Bb  comment:world\\n\" > edited.ssim"
                        " && EDITOR=\"cp edited.ssim\" $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data %:xx% -e -print:N'");
}

// Run acr -e with a missing -in path, and check that acr skips the editor session
// and exits nonzero.
// Without the gate the editor would open over an empty selection.  The write-back
// would then discard whatever the user typed into it.  The editor is a touch
// command whose marker file the test expects to find absent after the run.
void atf_comp::comptest_acr_EditFileReadFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp"
                        " && EDITOR=\"touch editor_ran\" $$OLDPWD/$bindir/acr -in:missing -schema:$$OLDPWD/data ns:% -e -print:N;"
                        " rc=$$?; test ! -f editor_ran && echo editor_not_launched; exit $$rc'");
}

// Run acr -e with an editor that inserts a record whose ssimfile write fails, and
// check that acr reports n_file_mod:0 and exits nonzero.
// Only a failure exits nonzero, so a dropped write stays distinguishable from an
// edit that modified files.  A file named gendb occupies the path of the gendb
// directory and blocks the write, as in the test/acr/writefail fixture.
void atf_comp::comptest_acr_EditFileWriteFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp d && touch d/gendb"
                        " && printf \"gendb.dispsig  dispsig:xx.yy  signature:9\\n\" > edited.ssim"
                        " && EDITOR=\"cp edited.ssim\" $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -e -print:N'");
}

void atf_comp::comptest_acr_Fields() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -field:nstype -field:comment");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe       comment:\"comment 1\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    comment:\"comment 2\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol  comment:\"comment 3\"");
}

void atf_comp::comptest_acr_FieldsComma() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -field:nstype,comment");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe       comment:\"comment 1\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    comment:\"comment 2\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol  comment:\"comment 3\"");
}

// Insert two records that land in two ssimfiles with acr -insert -write, and check
// that the report's n_file_mod counts both files.
// n_file_mod is the one carrier of that count.  amc's table write adds it to its
// own count of modified files.  acr creates the gendb/ and dmmeta/ directories on
// write under the tempdir, which serves as an empty dataset.
void atf_comp::comptest_acr_FileModCount() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:$tempdir -insert -write -print:N");
    atf_comp::ProcWrite(proc, "gendb.dispsig  dispsig:xx.yy  signature:9");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:xx.Yy  comment:''");
}

// Run acr with an -in path that names neither a file nor a dataset directory, and
// check that acr fails naming the path.
// A run that loaded no data would exit 0 with n_select:0.  A mistyped -in, or a
// wrong working directory, would then produce an answer over no data that looks
// like a true empty result.
void atf_comp::comptest_acr_FileReadFail() {
    atf_comp::ProcStart("$bindir/acr -in:test/acr/missing ns:%");
}

// Run acr with an -in file of mode 000, and check that acr fails naming the path.
// Read as empty, an unreadable input would pass for a true empty result.  The open
// fails before acr reads a byte, so the seed file is empty.  The test leaves the
// mode-000 file behind without a restore trap, because unlinking a file needs
// write permission on its directory only.  The per-run tempdir wipe therefore
// removes it whatever its mode.  DenyWriteCmd's 0555 directory would block the
// wipe, so its test restores the mode.  Like every chmod-deny fixture, this test
// assumes an unprivileged runner.  Under CAP_DAC_OVERRIDE (a root container) the
// read succeeds and the golden mismatches.
void atf_comp::comptest_acr_FileReadDeny() {
    atf_comp::ProcStart("bash -c 'touch $tempdir/in.ssim && chmod 000 $tempdir/in.ssim"
                        " && $bindir/acr -in:$tempdir/in.ssim ns:%'");
}

// Insert a dispsig row and a dev.gitfile row with acr -insert -write -g into a
// dataset whose ssimfile writes fail.  Check that acr names both paths, exits 2
// and prints the git script for -g in place of running it.
// Callers such as amc's table write trust acr's exit status to mean the records
// are on disk.  The dataset directory test/acr/writefail exists, so the load is
// clean.  The writes fail because the checked-in fixture holds a file named gendb
// and another named dev, where each ssimfile's parent directory would go.  The
// exit code counts diagnosed failures, so two refused writes exit 2.
// -g turns the inserted dev.gitfile row into a script.  Running that script while
// the row is off disk moves the worktree away from the database.  Here it would
// create and stage a file the dataset leaves out.  For a deletion the script line
// is git rm --force, which discards the file and any uncommitted work in it.
void atf_comp::comptest_acr_FileWriteFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:test/acr/writefail -insert -write -g -print:N");
    atf_comp::ProcWrite(proc, "gendb.dispsig  dispsig:xx.yy  signature:9");
    atf_comp::ProcWrite(proc, "dev.gitfile  gitfile:test/acr/writefail/newsrc");
}

void atf_comp::comptest_acr_GitTrigger1() {
    atf_comp::ProcStart("$bindir/acr gitfile:cpp/acr/git.cpp -del -g");
}

// Rename a dev.gitfile row into a directory that is still missing, and check that
// acr -g moves the file there:
// 1. Make a git repository in the tempdir, and stage a/x.txt in it.
// 2. Make a dataset whose one dev.gitfile row names a/x.txt.
// 3. Rename the row to b/c/x.txt with acr -rename -write -g.
// 4. Check that acr exits 0, that b/c/x.txt holds the file, and that the row
//    names b/c/x.txt.
// An ssimfile renamed into a new namespace takes this path, because its rows move
// to data/<newns>/, a directory that holds no file yet.  The git script creates
// the directory before git mv.
void atf_comp::comptest_acr_GitRenameNewDir() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && git init -q . && mkdir -p d/dev a"
                        " && echo hello > a/x.txt && git add a/x.txt"
                        " && printf \"dev.gitfile  gitfile:a/x.txt\\n\" > d/dev/gitfile.ssim"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data gitfile:a/x.txt -rename:b/c/x.txt -write -g -print:N;"
                        " echo acr_exit:$$?; cat b/c/x.txt; cat d/dev/gitfile.ssim'");
}

void atf_comp::comptest_acr_Insert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2        nstype:lib      license:GPL   comment:\"So will this one\"");
}

void atf_comp::comptest_acr_InsertDelete() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "acr.insert dmmeta.ns ns:ttt");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns ns:ttt");
}

void atf_comp::comptest_acr_Merge() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -merge ns:acr");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:lib         comment:\"nstype attribute will be updated\"");
}

void atf_comp::comptest_acr_Meta1() {
    atf_comp::ProcStart("$bindir/acr anonfld -meta -report:N");
}

// Run acr -meta over a dataset given by -in, and check that it answers from the
// directory that -schema names:
// 1. Make dataset d with one dev.license row, and query license:% over it.
// 2. Query license:% with -meta, and check that the ctype dev.License and its
//    fields print as they do against the tree.
// 3. Query % with -meta, and check that 2 lines name the ctype dev.License and 3
//    name dmmeta.Ctype.
// 4. Copy the schema's dmmeta, and run -meta -write with -schema naming the copy.
// 5. Check that the run reports n_file_mod:0, that the dataset keeps its one file,
//    and that the copy stays identical to the original.
// A dataset given by -in carries no dmmeta rows of its own.  dev.License, the
// matched row's ctype, appears on its dmmeta.ctype and gendb.ctypelen rows.
// dmmeta.Ctype, one of the tables the dataset leaves empty, appears on those two
// rows and on its dmmeta.cpptype row.  A % that binds every table also selects the
// ctype of every empty one, which is acr's rule for an empty match.  A query that
// names one table gets that table's ctype alone, because the query leaves none of
// the tables that -meta binds to answer with empty.  -meta reads the schema rows
// for answering only, so -write touches no file.  The test strips the padding from
// the counts, which BSD wc adds and GNU wc omits.
void atf_comp::comptest_acr_MetaDset() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p d/dev"
                        " && printf \"dev.license  license:MIT  proprietary:N  copyleft:N  comment:\\\"\\\"\\n\" > d/dev/license.ssim"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data license:% -report:N"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data license:% -meta -report:N"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data % -meta -report:N > all.txt"
                        " && grep -c \"ctype:dev.License \" all.txt && grep -c \"ctype:dmmeta.Ctype \" all.txt"
                        " && mkdir s && cp -r $$OLDPWD/data/dmmeta s/dmmeta"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:s license:% -meta -write -print:N | grep -o \"n_file_mod:[0-9]*\""
                        " && find d -type f | wc -l | tr -d \" \" && diff -rq $$OLDPWD/data/dmmeta s/dmmeta | wc -l | tr -d \" \"'");
}

void atf_comp::comptest_acr_Meta2() {
    atf_comp::ProcStart("$bindir/acr ftuple -meta");
}

void atf_comp::comptest_acr_Meta3() {
    atf_comp::ProcStart("$bindir/acr ftuple -meta -cmt");
}

void atf_comp::comptest_acr_NullTrunc() {
    atf_comp::ProcStart("$bindir/acr ns -report:N | $bindir/acr -trunc -insert");
}

void atf_comp::comptest_acr_QueryCtype() {
    atf_comp::ProcStart("$bindir/acr dmmeta.ctype:dmmeta.Ctype");
}

// Print a table whose rows refer to rows of the same table, and check that it
// prints before every table that refers to it, though the dependent's name sorts
// first:
// 1. Copy dmmeta, and add two tables to the copy with acr -insert -write:
//    dsmp.node, whose row names its parent node, and dsmp.grant, whose row names
//    a node.
// 2. Feed acr one grant and three nodes, dependents first, and print them all.
// 3. Check that the nodes print first, sorted by key with a parent before its
//    children, and that the grant prints after them.
// The test feeds the rows dependents first, so the output order is acr's own.
void atf_comp::comptest_acr_SelfRefOrder() {
    atf_comp::FProc &setup = atf_comp::ProcStart("bash -c 'rm -rf $tempdir/s && mkdir -p $tempdir/s && cp -r data/dmmeta $tempdir/s/dmmeta"
                                                 " && $bindir/acr -in:$tempdir/s -insert -write -print:N -report:N'");
    atf_comp::ProcWrite(setup, "dmmeta.ns  ns:dsmp  nstype:ssimdb  license:GPL  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.ctype  ctype:dsmp.Grant  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.ctype  ctype:dsmp.Node  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.field  field:dsmp.Grant.grant  arg:algo.cstring  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.field  field:dsmp.Grant.node  arg:dsmp.Node  reftype:Pkey  dflt:''  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.field  field:dsmp.Node.node  arg:algo.cstring  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.field  field:dsmp.Node.parent  arg:dsmp.Node  reftype:Pkey  dflt:''  comment:''");
    atf_comp::ProcWrite(setup, "dmmeta.ssimfile  ssimfile:dsmp.grant  ctype:dsmp.Grant");
    atf_comp::ProcWrite(setup, "dmmeta.ssimfile  ssimfile:dsmp.node  ctype:dsmp.Node");
    atf_comp::ProcWrite(setup, "dmmeta.ssimsort  ssimfile:dsmp.node  sortfld:dsmp.Node.node  comment:''");
    atf_comp::ProcWriteEof(setup);
    atf_comp::ProcWait(setup);
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- -schema:$tempdir/s % -report:N");
    atf_comp::ProcWrite(proc, "dsmp.grant  grant:g1  node:a/b");
    atf_comp::ProcWrite(proc, "dsmp.node  node:a/b  parent:a");
    atf_comp::ProcWrite(proc, "dsmp.node  node:a  parent:''");
    atf_comp::ProcWrite(proc, "dsmp.node  node:''  parent:''");
}

void atf_comp::comptest_acr_RenameCollision() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:ns1 -rename ns2");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:''");
}

void atf_comp::comptest_acr_RenameField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- field:dmmeta.Ns.nstype -rename dmmeta.Ns.blah");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Ns.nstype  arg:dmmeta.Nstype  reftype:Pkey  dflt:''  comment:\"Namespace type\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:''");
}

void atf_comp::comptest_acr_RenameRecord() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- target:samp -rename:xyz");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:''");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:''");
}

void atf_comp::comptest_acr_Replace() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -replace ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be replaced\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will remain\"");
}

void atf_comp::comptest_acr_Select() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:nsi%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"This will NOT be selected\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:nsi1        nstype:exe      license:GPL   comment:\"This will be selected\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:nsi2      nstype:ssimdb    license:GPL  comment:\"This will be selected\"");
}

void atf_comp::comptest_acr_SelectInProcSubst() {
    atf_comp::ProcStart("$bindir/acr -in:<(echo 'dmmeta.ns  ns:nsi1  nstype:exe  license:GPL  comment:\"This will be selected\"' && echo 'dmmeta.ns  ns:other  nstype:lib  license:GPL  comment:\"This will NOT be selected\"') ns:nsi%");
}

void atf_comp::comptest_acr_SelectStdin() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -sel");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr");
}

void atf_comp::comptest_acr_SelectTree() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ctype:command.acr -t");
    atf_comp::ProcWrite(proc, "dev.license  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:command  nstype:protocol  license:GPL  comment:\"Command line descriptions\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.acr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.query  arg:algo.cstring  reftype:Val  dflt:'\"\"'  comment:\"Regx to match record\"");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.acr.query  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.schema    arg:algo.cstring  reftype:Val  dflt:'\"data\"'  comment:\"Directory for initializing acr meta-data\"");
    atf_comp::ProcWrite(proc, "dmmeta.floadtuples  field:command.acr.schema    comment:''");
    atf_comp::ProcWrite(proc, "gendb.ctypelen  ctype:command.acr  len:192  alignment:8  padbytes:21");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:acr.FDb.cmdline  arg:command.acr  reftype:Val  dflt:''  comment:\"command line\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.acr  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
}

void atf_comp::comptest_acr_TooManyArgs() {
    atf_comp::ProcStart("$bindir/acr x y");
}

void atf_comp::comptest_acr_Trunc() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -trunc -report:N");
    atf_comp::ProcWrite(proc, "dev.timefmt timefmt:xyz");
}

// Run acr % -write over a file that holds a blank line, a comment line and a tuple
// of an unknown ctype.  Check that acr reports the unknown tuple, exits nonzero
// and leaves the file as it was, and that the blank and comment lines draw no
// report.
void atf_comp::comptest_acr_UnknownCtype() {
    atf_comp::ProcStart("bash -c 'printf \"\\n# comment line\\ndmmeta.nss  nss:foo\\ndmmeta.ns  ns:zzz  nstype:exe  license:GPL\\n\" > $tempdir/t.ssim"
                        "; $bindir/acr % -write -in:$tempdir/t.ssim"
                        "; echo acr_exit:$?; cat $tempdir/t.ssim'");
}

void atf_comp::comptest_acr_UpdateBad() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -update ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"Namespace doesn't exist, but not a failure\"");
}

void atf_comp::comptest_acr_UpdateGood() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -update ns:acr");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:lib      license:zzz   comment:\"Namespace exists, attribute will be updated\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:ssimdb   comment:\"Namespace exists, attribute will be updated\"");
}

void atf_comp::comptest_acr_Where() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -where:nstype:protocol");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:''");
}
