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
// Source: cpp/atf_comp/acr.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_BadInsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
}

// A line the tuple parser rejects (unterminated quote) is reported with its
// file:line, fails the run, and blocks the -write, so the file still holds
// every line it held -- including the one that did not load.
void atf_comp::comptest_acr_BadLine() {
    atf_comp::ProcStart("bash -c 'printf \"dmmeta.dispsig  dispsig:a.X  signature:1\\ndmmeta.dispsig  dispsig:b.Y  signature:\\042unterminated\\ndmmeta.dispsig  dispsig:c.Z  signature:3\\n\" > $tempdir/t.ssim"
                        "; echo dmmeta.dispsig  dispsig:d.W  signature:4 | $bindir/acr -insert -write -in:$tempdir/t.ssim %"
                        "; echo acr_exit:$?; cat $tempdir/t.ssim'");
}

void atf_comp::comptest_acr_BadNs() {
    // Check against a small fixed namespace set fed via -in, so the
    // "Valid values" suggestion does not depend on the live namespace list.
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:<(echo 'dmmeta.ns  ns:foo  nstype:exe  license:GPL  comment:\"\"' && echo 'dmmeta.ns  ns:bar  nstype:lib  license:GPL  comment:\"\"') -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:x.Y");
}

// A cpbuf row naming a protobuf syntax that is not one of the two the codec
// knows. cpbuf.syntax references the dmmeta.pbsyntax table, so the reference
// check names the attribute and lists the syntax variants; a misspelling is
// rejected here instead of silently selecting the proto2 wire shape.
// The pbsyntax and ctype rows come in through -in so the suggestion does not
// depend on the live database.
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
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
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
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  license:GPL  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:\"\"");
}

void atf_comp::comptest_acr_DeleteReinsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns  ns:amc");
    atf_comp::ProcWrite(proc, "acr.insert dmmeta.ns  ns:amc  nstype:exe  license:GPL  comment:\"Algo Cross-Reference - ssimfile database & update tool\"");
}

// acr whose -in names a dataset directory it cannot search: the run must fail
// naming the directory.
// The directory exists, so acr selects dataset mode and reads each table on
// demand, which leaves the failure reported one ssimfile at a time and the
// directory itself never named. A run that reaches no table is worse: it
// reports success over no data at all. A -check is that run -- it answers
// records:0 n_err:0 for a dataset it could not open, so the integrity gate
// passes having validated nothing.
// The rule the probe applies is search (execute) permission, which is what
// opening an ssimfile by name needs. A directory listing is never performed, so
// the search-only mode is accepted alongside the readable one; both are
// controls here. A symlink resolves to its target, so one pointing at a
// searchable directory is accepted, and a dangling one names nothing and is
// rejected by the same branch as a mistyped path.
// The rejecting steps assert a nonzero exit rather than exit code 1: the exit
// code counts diagnosed failures, and a query that reaches a table over an
// unsearchable directory diagnoses both the directory and that table.
// Every mode the test installs on the directory withholds write permission, so
// a directory left in one of them cannot be removed by the next run's tempdir
// wipe, and that run fails on a leftover it can neither read nor recreate. An
// EXIT trap restores the mode, and a TERM trap reaches that EXIT trap when the
// harness timeout kills the shell; the same pair guards the read-only directory
// of atf_comp::DenyWriteCmd (cpp/atf_comp/proc.cpp). A mode-000 plain file,
// such as the one acr.FileReadDeny leaves behind, needs no trap: unlinking it
// asks for write permission on its parent only.
// Like every chmod-deny fixture, this assumes an unprivileged runner: under
// CAP_DAC_OVERRIDE (a root container) the search succeeds and the golden
// mismatches.
void atf_comp::comptest_acr_DsetDirReadDeny() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p d/dmmeta"
                        " && printf \"dmmeta.dispsig  dispsig:xx.aa  signature:1\\n\" > d/dmmeta/dispsig.ssim"
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

// acr in dataset mode over a directory holding one ssimfile it cannot read
// (permissions): every read of that ssimfile must fail the run naming the
// path. Read as an empty table instead, the file's rows are missing from the
// answer, and an -insert -write then rewrites the file from the rows that did
// load -- the inserted one alone, with every pre-existing row gone. So the
// test pins the ssimfile byte-unchanged (cmp) after a query, a write, and an
// editor session, and pins that the editor never launched. The two accepted
// edges of the same rule are the controls: an ssimfile absent from the
// dataset, and an empty one, both load as empty tables and exit 0.
// -g is the other side effect a failed load must stand down from. The script
// -g generates removes, renames and creates the source files the selected
// dev.gitfile rows name, so a run that leaves those rows unwritten and still
// runs the script moves the worktree away from the database. The script's own
// `touch` line is the marker: it creates the inserted row's file when the
// script runs. The denied read is an unreadable dev/gitfile.ssim, and it must
// leave both the file and the worktree alone, so no marker appears.
// The control is a clean load, which does run the script, and it doubles as the
// probe of what a failing script does. That script stages the file it just
// created; the tempdir is gitignored, so the `git add` is refused and the script
// exits nonzero. acr must report that rather than exit 0 on a worktree the
// script did not finish arranging. The step therefore asserts three things: the
// marker (the script ran), a nonzero exit (the failure was reported), and the
// acr.git_script diagnostic naming the script. It reads that diagnostic out of a
// file rather than from the captured stream because git's own complaint goes to
// the same stream with a wording that depends on the git version.
// Like every chmod-deny fixture, this assumes an unprivileged runner: under
// CAP_DAC_OVERRIDE (a root container) the read succeeds and the golden
// mismatches.
void atf_comp::comptest_acr_DsetFileReadDeny() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p d/dmmeta d/dev temp"
                        " && printf \"dmmeta.dispsig  dispsig:xx.aa  signature:1\\n\" > d/dmmeta/dispsig.ssim"
                        " && cp d/dmmeta/dispsig.ssim orig"
                        " && printf \"\" > d/dmmeta/ctype.ssim"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data ctype:% -print:N && echo empty_ssimfile_ok"
                        " && $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data ssimfile:% -print:N && echo absent_ssimfile_ok"
                        " && printf \"dev.gitfile  gitfile:added_src\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -g -print:N 2>giterr;"
                        " test $$? = 1 && test -f added_src && echo git_script_failed;"
                        " grep git_script giterr;"
                        " cp d/dev/gitfile.ssim gitorig;"
                        " chmod 000 d/dmmeta/dispsig.ssim d/dev/gitfile.ssim;"
                        " $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -print:N;"
                        " test $$? = 1 && echo query_failed;"
                        " printf \"dmmeta.dispsig  dispsig:xx.bb  signature:2\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -print:N;"
                        " test $$? = 1 && echo write_failed;"
                        " EDITOR=\"touch editor_ran\" $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data dispsig:% -e -write -print:N;"
                        " test $$? = 1 && test ! -f editor_ran && echo editor_not_launched;"
                        " printf \"dev.gitfile  gitfile:denied_src\\n\" | $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data -insert -write -g -print:N;"
                        " test $$? = 1 && test ! -f denied_src && echo git_script_not_run;"
                        " chmod 644 d/dmmeta/dispsig.ssim d/dev/gitfile.ssim;"
                        " cmp d/dmmeta/dispsig.ssim orig && echo ssimfile_unchanged;"
                        " cmp d/dev/gitfile.ssim gitorig && echo gitfile_unchanged'");
}

// acr -e whose editor modifies records in two different ssimfiles: the edit
// is written back, the report's n_file_mod carries the per-file count, and
// the process exits 0 -- the exit code means success or failure only, never
// the number of files modified. The editor is a cp command that replaces the
// edit buffer with the two pre-edited records.
void atf_comp::comptest_acr_EditFileModCount() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp d d/dmmeta"
                        " && printf \"dmmeta.dispsig  dispsig:xx.aa  signature:1\\n\" > d/dmmeta/dispsig.ssim"
                        " && printf \"dmmeta.ctype  ctype:xx.Bb  comment:hello\\n\" > d/dmmeta/ctype.ssim"
                        " && printf \"dmmeta.dispsig  dispsig:xx.aa  signature:2\\ndmmeta.ctype  ctype:xx.Bb  comment:world\\n\" > edited.ssim"
                        " && EDITOR=\"cp edited.ssim\" $$OLDPWD/$bindir/acr -in:d -schema:$$OLDPWD/data %:xx% -e -print:N'");
}

// acr -e whose -in path cannot be loaded: the editor session is skipped and
// the run exits nonzero. Without the gate, the editor opens over an empty
// selection and the write-back then discards whatever the user typed into it.
// The editor is a touch command whose marker file must not exist after the
// run.
void atf_comp::comptest_acr_EditFileReadFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp"
                        " && EDITOR=\"touch editor_ran\" $$OLDPWD/$bindir/acr -in:missing -schema:$$OLDPWD/data ns:% -e -print:N;"
                        " rc=$$?; test ! -f editor_ran && echo editor_not_launched; exit $$rc'");
}

// acr -e whose write-back fails: the run exits nonzero with n_file_mod:0,
// so a dropped write is distinguishable from a modifying edit -- only
// failure exits nonzero. The editor inserts a record whose ssimfile write
// is blocked by a file occupying the dmmeta directory path (the same
// arrangement as the test/acr/writefail fixture).
void atf_comp::comptest_acr_EditFileWriteFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir temp d && touch d/dmmeta"
                        " && printf \"dmmeta.dispsig  dispsig:xx.yy  signature:9\\n\" > edited.ssim"
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

// acr -insert -write of records landing in two different ssimfiles: the
// report's n_file_mod must count each modified file -- it is the one
// carrier of that count (amc's table write accumulates it into its own
// files-modified count). The tempdir serves as an empty dataset; acr
// creates the dmmeta/ directory under it on write.
void atf_comp::comptest_acr_FileModCount() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:$tempdir -insert -write -print:N");
    atf_comp::ProcWrite(proc, "dmmeta.dispsig  dispsig:xx.yy  signature:9");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:xx.Yy  comment:\"\"");
}

// acr with an -in path that names neither an existing file nor a dataset
// directory: the run must fail naming the path instead of loading nothing
// and exiting 0 with n_select:0 -- a mistyped -in (or a wrong working
// directory) would otherwise produce an answer computed over no data,
// indistinguishable from a true empty result.
void atf_comp::comptest_acr_FileReadFail() {
    atf_comp::ProcStart("$bindir/acr -in:test/acr/missing ns:%");
}

// acr with an -in file that exists but cannot be read (permissions): the
// run must fail naming the path instead of answering from nothing --
// reading an unreadable input as empty would pass the bad input off as a
// true empty result. The open fails before any byte is read, so the seed
// file is empty. The mode-000 file is left behind without a restore trap:
// unlinking a file needs only write permission on the containing
// directory, so the per-run tempdir wipe removes it regardless of its
// mode -- unlike DenyWriteCmd's 0555 directory, which would block the
// wipe and must be restored. Like every chmod-deny fixture, this assumes
// an unprivileged runner: under CAP_DAC_OVERRIDE (a root container) the
// read succeeds and the golden mismatches.
void atf_comp::comptest_acr_FileReadDeny() {
    atf_comp::ProcStart("bash -c 'touch $tempdir/in.ssim && chmod 000 $tempdir/in.ssim"
                        " && $bindir/acr -in:$tempdir/in.ssim ns:%'");
}

// acr -insert -write whose ssimfile write fails: the run must fail naming
// the path instead of exiting 0 with the record silently unwritten --
// callers such as amc's table write trust acr's exit status to mean the
// records are on disk. The dataset directory exists (the load is clean);
// the write fails because the checked-in fixture holds a file named dmmeta,
// and another named dev, where each ssimfile's parent directory would go.
// The exit code counts diagnosed failures, so two refused writes exit 2.
// A refused write must also stand -g down. The inserted dev.gitfile row is
// what -g turns into a script, and running that script while the row is not
// on disk moves the worktree away from the database: here it would create and
// stage a file the dataset does not list, and for a deletion the script line
// is git rm --force, which discards the file and any uncommitted work in it.
// So the script is printed, and the printed script is what the golden pins.
void atf_comp::comptest_acr_FileWriteFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:test/acr/writefail -insert -write -g -print:N");
    atf_comp::ProcWrite(proc, "dmmeta.dispsig  dispsig:xx.yy  signature:9");
    atf_comp::ProcWrite(proc, "dev.gitfile  gitfile:test/acr/writefail/newsrc");
}

void atf_comp::comptest_acr_GitTrigger1() {
    atf_comp::ProcStart("$bindir/acr gitfile:cpp/acr/git.cpp -del -g");
}

void atf_comp::comptest_acr_Insert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
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

void atf_comp::comptest_acr_RenameCollision() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:ns1 -rename ns2");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}

void atf_comp::comptest_acr_RenameField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- field:dmmeta.Ns.nstype -rename dmmeta.Ns.blah");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Ns.nstype  arg:dmmeta.Nstype  reftype:Pkey  dflt:\"\"  comment:\"Namespace type\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}

void atf_comp::comptest_acr_RenameRecord() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- target:samp -rename:xyz");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:\"\"");
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
    atf_comp::ProcWrite(proc, "dev.license  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:command  nstype:protocol  license:GPL  comment:\"Command line descriptions\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.acr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.query  arg:algo.cstring  reftype:Val  dflt:'\"\"'  comment:\"Regx to match record\"");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.acr.query  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.schema    arg:algo.cstring  reftype:Val  dflt:'\"data\"'  comment:\"Directory for initializing acr meta-data\"");
    atf_comp::ProcWrite(proc, "dmmeta.floadtuples  field:command.acr.schema    comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelen  ctype:command.acr  len:192  alignment:8  padbytes:21");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:acr.FDb.cmdline  arg:command.acr  reftype:Val  dflt:\"\"  comment:\"command line\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.acr  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
}

void atf_comp::comptest_acr_TooManyArgs() {
    atf_comp::ProcStart("$bindir/acr x y");
}

void atf_comp::comptest_acr_Trunc() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -trunc -report:N");
    atf_comp::ProcWrite(proc, "dev.timefmt timefmt:xyz");
}

// A parsed tuple naming a ctype acr does not know is reported, fails the
// run, and blocks the -write; the blank and comment-only lines in the same
// file stay accepted and draw no report.
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
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}
