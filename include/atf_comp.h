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
// Header: include/atf_comp.h
//

#include "include/gen/atf_comp_gen.h"
#include "include/gen/atf_comp_gen.inl.h"

namespace atf_comp { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_BadInsert(); // atfdb.comptest:acr.BadInsert

    // Feed acr a file with a line the tuple parser rejects, and check that the line
    // fails the run and blocks the -write:
    // 1. Write a three-line file whose second line has an unterminated quote.
    // 2. Insert a record into the file with acr -insert -write.
    // 3. Check that acr reports the bad line with its file:line and exits nonzero.
    // 4. Check that the file still holds every line it held, the one that failed to
    // load included.
    // void comptest_acr_BadLine(); // atfdb.comptest:acr.BadLine

    // Run acr -version with its stdout closed, and check that acr exits nonzero.
    // A tool whose output was lost has failed its job, even though what it printed
    // was right.  The closed stdout makes the write fail with EBADF on every platform
    // and raises no signal, so the exit code is the only report of the loss.
    // -version prints before acr opens any file of its own, so descriptor 1 is still
    // closed when acr writes the line.
    // void comptest_acr_OutputLost(); // atfdb.comptest:acr.OutputLost
    // void comptest_acr_BadNs(); // atfdb.comptest:acr.BadNs

    // Insert a dmmeta.cpbuf row whose syntax is Proto3, a misspelling of a syntax the
    // codec knows, and check that acr -insert -check rejects the row, names the
    // attribute and lists proto2 and proto3 as the valid values.
    // cpbuf.syntax references the dmmeta.pbsyntax table, so the reference check
    // catches the misspelling before it silently selects the proto2 wire shape.  The
    // pbsyntax and ctype rows come in through -in, so the list of valid values stays
    // independent of the live database.
    // void comptest_acr_BadPbufSyntax(); // atfdb.comptest:acr.BadPbufSyntax
    // void comptest_acr_BadPkey(); // atfdb.comptest:acr.BadPkey
    // void comptest_acr_BadReftype(); // atfdb.comptest:acr.BadReftype
    // void comptest_acr_CascDel(); // atfdb.comptest:acr.CascDel
    // void comptest_acr_CascDel2(); // atfdb.comptest:acr.CascDel2
    // void comptest_acr_CascDel3(); // atfdb.comptest:acr.CascDel3
    // void comptest_acr_DelField(); // atfdb.comptest:acr.DelField
    // void comptest_acr_DelRecord(); // atfdb.comptest:acr.DelRecord
    // void comptest_acr_DeleteReinsert(); // atfdb.comptest:acr.DeleteReinsert

    // Point acr's -in at a dataset directory under several modes, and check that acr
    // fails naming the directory whenever it lacks search permission:
    // 1. Make dataset directory d with one ssimfile, a symlink dlink to d, and a
    // symlink dangling to the missing path nowhere.
    // 2. Query through dlink, and check that the query succeeds.
    // 3. Install an EXIT trap that restores the mode of d, and a TERM trap that
    // reaches it.
    // 4. Query d under mode 500 (readable) and mode 100 (search only), and check that
    // both queries succeed.
    // 5. Query d under mode 400 (list only) and mode 000, and check that each query
    // names the directory and exits nonzero.
    // 6. Run acr -check over d under mode 000, and check that it names the directory
    // and exits nonzero.
    // 7. Restore mode 755, query through dangling, and check that the query exits
    // nonzero.
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
    // void comptest_acr_DsetDirReadDeny(); // atfdb.comptest:acr.DsetDirReadDeny

    // Load a dataset that holds unreadable ssimfiles, and check that every read of one
    // fails the run naming the path and leaves the file and the worktree alone:
    // 1. Make dataset d with one dispsig row, and keep a copy of its ssimfile.
    // 2. Query an empty ssimfile and an absent one, and check that each loads as an
    // empty table and exits 0.
    // 3. Insert a dev.gitfile row with acr -insert -write -g.  Check that the script
    // runs and creates the marker, and that acr exits 1 with the acr.git_script
    // diagnostic.
    // 4. Keep a copy of dev/gitfile.ssim, and chmod 000 it and the dispsig ssimfile.
    // 5. Query dispsig, and check that the query names the path and exits 1.
    // 6. Insert a dispsig row with -insert -write, and check that the run exits 1.
    // 7. Edit dispsig with -e -write, and check that the run exits 1 and the editor
    // never launches.
    // 8. Insert a dev.gitfile row with -g, and check that the run exits 1 and the
    // script never runs.
    // 9. Restore both modes, and check with cmp that both ssimfiles are
    // byte-unchanged.
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
    // void comptest_acr_DsetFileReadDeny(); // atfdb.comptest:acr.DsetFileReadDeny

    // Run acr -e with an editor that changes records in two ssimfiles, and check that
    // acr writes the edit back, reports n_file_mod:2 and exits 0.
    // The exit code means success or failure only, never the number of files
    // modified.  The editor is a cp command that replaces the edit buffer with the two
    // edited records.
    // void comptest_acr_EditFileModCount(); // atfdb.comptest:acr.EditFileModCount

    // Run acr -e with a missing -in path, and check that acr skips the editor session
    // and exits nonzero.
    // Without the gate the editor would open over an empty selection.  The write-back
    // would then discard whatever the user typed into it.  The editor is a touch
    // command whose marker file the test expects to find absent after the run.
    // void comptest_acr_EditFileReadFail(); // atfdb.comptest:acr.EditFileReadFail

    // Run acr -e with an editor that inserts a record whose ssimfile write fails, and
    // check that acr reports n_file_mod:0 and exits nonzero.
    // Only a failure exits nonzero, so a dropped write stays distinguishable from an
    // edit that modified files.  A file named gendb occupies the path of the gendb
    // directory and blocks the write, as in the test/acr/writefail fixture.
    // void comptest_acr_EditFileWriteFail(); // atfdb.comptest:acr.EditFileWriteFail
    // void comptest_acr_Fields(); // atfdb.comptest:acr.Fields
    // void comptest_acr_FieldsComma(); // atfdb.comptest:acr.FieldsComma

    // Insert two records that land in two ssimfiles with acr -insert -write, and check
    // that the report's n_file_mod counts both files.
    // n_file_mod is the one carrier of that count.  amc's table write adds it to its
    // own count of modified files.  acr creates the gendb/ and dmmeta/ directories on
    // write under the tempdir, which serves as an empty dataset.
    // void comptest_acr_FileModCount(); // atfdb.comptest:acr.FileModCount

    // Run acr with an -in path that names neither a file nor a dataset directory, and
    // check that acr fails naming the path.
    // A run that loaded no data would exit 0 with n_select:0.  A mistyped -in, or a
    // wrong working directory, would then produce an answer over no data that looks
    // like a true empty result.
    // void comptest_acr_FileReadFail(); // atfdb.comptest:acr.FileReadFail

    // Run acr with an -in file of mode 000, and check that acr fails naming the path.
    // Read as empty, an unreadable input would pass for a true empty result.  The open
    // fails before acr reads a byte, so the seed file is empty.  The test leaves the
    // mode-000 file behind without a restore trap, because unlinking a file needs
    // write permission on its directory only.  The per-run tempdir wipe therefore
    // removes it whatever its mode.  DenyWriteCmd's 0555 directory would block the
    // wipe, so its test restores the mode.  Like every chmod-deny fixture, this test
    // assumes an unprivileged runner.  Under CAP_DAC_OVERRIDE (a root container) the
    // read succeeds and the golden mismatches.
    // void comptest_acr_FileReadDeny(); // atfdb.comptest:acr.FileReadDeny

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
    // void comptest_acr_FileWriteFail(); // atfdb.comptest:acr.FileWriteFail
    // void comptest_acr_GitTrigger1(); // atfdb.comptest:acr.GitTrigger1

    // Rename a dev.gitfile row into a directory that is still missing, and check that
    // acr -g moves the file there:
    // 1. Make a git repository in the tempdir, and stage a/x.txt in it.
    // 2. Make a dataset whose one dev.gitfile row names a/x.txt.
    // 3. Rename the row to b/c/x.txt with acr -rename -write -g.
    // 4. Check that acr exits 0, that b/c/x.txt holds the file, and that the row
    // names b/c/x.txt.
    // An ssimfile renamed into a new namespace takes this path, because its rows move
    // to data/<newns>/, a directory that holds no file yet.  The git script creates
    // the directory before git mv.
    // void comptest_acr_GitRenameNewDir(); // atfdb.comptest:acr.GitRenameNewDir
    // void comptest_acr_Insert(); // atfdb.comptest:acr.Insert
    // void comptest_acr_InsertDelete(); // atfdb.comptest:acr.InsertDelete
    // void comptest_acr_Merge(); // atfdb.comptest:acr.Merge
    // void comptest_acr_Meta1(); // atfdb.comptest:acr.Meta1

    // Run acr -meta over a dataset given by -in, and check that it answers from the
    // directory that -schema names:
    // 1. Make dataset d with one dev.license row, and query license:% over it.
    // 2. Query license:% with -meta, and check that the ctype dev.License and its
    // fields print as they do against the tree.
    // 3. Query % with -meta, and check that 2 lines name the ctype dev.License and 3
    // name dmmeta.Ctype.
    // 4. Copy the schema's dmmeta, and run -meta -write with -schema naming the copy.
    // 5. Check that the run reports n_file_mod:0, that the dataset keeps its one file,
    // and that the copy stays identical to the original.
    // A dataset given by -in carries no dmmeta rows of its own.  dev.License, the
    // matched row's ctype, appears on its dmmeta.ctype and gendb.ctypelen rows.
    // dmmeta.Ctype, one of the tables the dataset leaves empty, appears on those two
    // rows and on its dmmeta.cpptype row.  A % that binds every table also selects the
    // ctype of every empty one, which is acr's rule for an empty match.  A query that
    // names one table gets that table's ctype alone, because the query leaves none of
    // the tables that -meta binds to answer with empty.  -meta reads the schema rows
    // for answering only, so -write touches no file.  The test strips the padding from
    // the counts, which BSD wc adds and GNU wc omits.
    // void comptest_acr_MetaDset(); // atfdb.comptest:acr.MetaDset
    // void comptest_acr_Meta2(); // atfdb.comptest:acr.Meta2
    // void comptest_acr_Meta3(); // atfdb.comptest:acr.Meta3
    // void comptest_acr_NullTrunc(); // atfdb.comptest:acr.NullTrunc
    // void comptest_acr_QueryCtype(); // atfdb.comptest:acr.QueryCtype

    // Print a table whose rows refer to rows of the same table, and check that it
    // prints before every table that refers to it, though the dependent's name sorts
    // first:
    // 1. Copy dmmeta, and add two tables to the copy with acr -insert -write:
    // dsmp.node, whose row names its parent node, and dsmp.grant, whose row names
    // a node.
    // 2. Feed acr one grant and three nodes, dependents first, and print them all.
    // 3. Check that the nodes print first, sorted by key with a parent before its
    // children, and that the grant prints after them.
    // The test feeds the rows dependents first, so the output order is acr's own.
    // void comptest_acr_SelfRefOrder(); // atfdb.comptest:acr.SelfRefOrder
    // void comptest_acr_RenameCollision(); // atfdb.comptest:acr.RenameCollision
    // void comptest_acr_RenameField(); // atfdb.comptest:acr.RenameField
    // void comptest_acr_RenameRecord(); // atfdb.comptest:acr.RenameRecord
    // void comptest_acr_Replace(); // atfdb.comptest:acr.Replace
    // void comptest_acr_Select(); // atfdb.comptest:acr.Select
    // void comptest_acr_SelectInProcSubst(); // atfdb.comptest:acr.SelectInProcSubst
    // void comptest_acr_SelectStdin(); // atfdb.comptest:acr.SelectStdin
    // void comptest_acr_SelectTree(); // atfdb.comptest:acr.SelectTree
    // void comptest_acr_TooManyArgs(); // atfdb.comptest:acr.TooManyArgs
    // void comptest_acr_Trunc(); // atfdb.comptest:acr.Trunc

    // Run acr % -write over a file that holds a blank line, a comment line and a tuple
    // of an unknown ctype.  Check that acr reports the unknown tuple, exits nonzero
    // and leaves the file as it was, and that the blank and comment lines draw no
    // report.
    // void comptest_acr_UnknownCtype(); // atfdb.comptest:acr.UnknownCtype
    // void comptest_acr_UpdateBad(); // atfdb.comptest:acr.UpdateBad
    // void comptest_acr_UpdateGood(); // atfdb.comptest:acr.UpdateGood
    // void comptest_acr_Where(); // atfdb.comptest:acr.Where

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_compl.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_compl_A01a(); // atfdb.comptest:acr_compl.A01a
    // void comptest_acr_compl_A01b(); // atfdb.comptest:acr_compl.A01b
    // void comptest_acr_compl_A01c(); // atfdb.comptest:acr_compl.A01c
    // void comptest_acr_compl_A02a(); // atfdb.comptest:acr_compl.A02a
    // void comptest_acr_compl_A02b(); // atfdb.comptest:acr_compl.A02b
    // void comptest_acr_compl_A02c(); // atfdb.comptest:acr_compl.A02c
    // void comptest_acr_compl_A03a(); // atfdb.comptest:acr_compl.A03a
    // void comptest_acr_compl_A03b(); // atfdb.comptest:acr_compl.A03b
    // void comptest_acr_compl_A03c(); // atfdb.comptest:acr_compl.A03c
    // void comptest_acr_compl_A04a(); // atfdb.comptest:acr_compl.A04a
    // void comptest_acr_compl_A04b(); // atfdb.comptest:acr_compl.A04b
    // void comptest_acr_compl_A04c(); // atfdb.comptest:acr_compl.A04c
    // void comptest_acr_compl_A05a(); // atfdb.comptest:acr_compl.A05a
    // void comptest_acr_compl_A05b(); // atfdb.comptest:acr_compl.A05b
    // void comptest_acr_compl_A05c(); // atfdb.comptest:acr_compl.A05c
    // void comptest_acr_compl_A06a(); // atfdb.comptest:acr_compl.A06a
    // void comptest_acr_compl_A06b(); // atfdb.comptest:acr_compl.A06b
    // void comptest_acr_compl_A06c(); // atfdb.comptest:acr_compl.A06c
    // void comptest_acr_compl_A07a(); // atfdb.comptest:acr_compl.A07a
    // void comptest_acr_compl_A07b(); // atfdb.comptest:acr_compl.A07b
    // void comptest_acr_compl_A07c(); // atfdb.comptest:acr_compl.A07c
    // void comptest_acr_compl_Acr01(); // atfdb.comptest:acr_compl.Acr01
    // void comptest_acr_compl_Acr02(); // atfdb.comptest:acr_compl.Acr02
    // void comptest_acr_compl_Acr03(); // atfdb.comptest:acr_compl.Acr03
    // void comptest_acr_compl_Acr04(); // atfdb.comptest:acr_compl.Acr04
    // void comptest_acr_compl_Acr05(); // atfdb.comptest:acr_compl.Acr05
    // void comptest_acr_compl_Acr06(); // atfdb.comptest:acr_compl.Acr06
    // void comptest_acr_compl_Acr07(); // atfdb.comptest:acr_compl.Acr07
    // void comptest_acr_compl_Acr08(); // atfdb.comptest:acr_compl.Acr08
    // void comptest_acr_compl_Acr09(); // atfdb.comptest:acr_compl.Acr09
    // void comptest_acr_compl_Acr10(); // atfdb.comptest:acr_compl.Acr10
    // void comptest_acr_compl_Acr11(); // atfdb.comptest:acr_compl.Acr11
    // void comptest_acr_compl_Acr12(); // atfdb.comptest:acr_compl.Acr12
    // void comptest_acr_compl_Acr13(); // atfdb.comptest:acr_compl.Acr13
    // void comptest_acr_compl_BadExe(); // atfdb.comptest:acr_compl.BadExe
    // void comptest_acr_compl_BadOpt(); // atfdb.comptest:acr_compl.BadOpt
    // void comptest_acr_compl_BadOptColon(); // atfdb.comptest:acr_compl.BadOptColon
    // void comptest_acr_compl_BadOptColonSpace(); // atfdb.comptest:acr_compl.BadOptColonSpace
    // void comptest_acr_compl_BadOptSpace(); // atfdb.comptest:acr_compl.BadOptSpace
    // void comptest_acr_compl_Bare(); // atfdb.comptest:acr_compl.Bare
    // void comptest_acr_compl_DblColon(); // atfdb.comptest:acr_compl.DblColon
    // void comptest_acr_compl_DblColonList(); // atfdb.comptest:acr_compl.DblColonList
    // void comptest_acr_compl_DblSpace(); // atfdb.comptest:acr_compl.DblSpace
    // void comptest_acr_compl_DblSpaceList(); // atfdb.comptest:acr_compl.DblSpaceList
    // void comptest_acr_compl_EnumCtypeColon(); // atfdb.comptest:acr_compl.EnumCtypeColon
    // void comptest_acr_compl_EnumCtypeColonList(); // atfdb.comptest:acr_compl.EnumCtypeColonList
    // void comptest_acr_compl_EnumCtypeSpace(); // atfdb.comptest:acr_compl.EnumCtypeSpace
    // void comptest_acr_compl_EnumCtypeSpaceList(); // atfdb.comptest:acr_compl.EnumCtypeSpaceList
    // void comptest_acr_compl_EnumFieldColon(); // atfdb.comptest:acr_compl.EnumFieldColon
    // void comptest_acr_compl_EnumFieldColonList(); // atfdb.comptest:acr_compl.EnumFieldColonList
    // void comptest_acr_compl_EnumFieldSpace(); // atfdb.comptest:acr_compl.EnumFieldSpace
    // void comptest_acr_compl_EnumFieldSpaceList(); // atfdb.comptest:acr_compl.EnumFieldSpaceList
    // void comptest_acr_compl_FlagColon(); // atfdb.comptest:acr_compl.FlagColon
    // void comptest_acr_compl_FlagColonList(); // atfdb.comptest:acr_compl.FlagColonList
    // void comptest_acr_compl_FlagSpace(); // atfdb.comptest:acr_compl.FlagSpace
    // void comptest_acr_compl_FlagSpaceList(); // atfdb.comptest:acr_compl.FlagSpaceList
    // void comptest_acr_compl_Install(); // atfdb.comptest:acr_compl.Install
    // void comptest_acr_compl_NumColon(); // atfdb.comptest:acr_compl.NumColon
    // void comptest_acr_compl_NumColonList(); // atfdb.comptest:acr_compl.NumColonList
    // void comptest_acr_compl_NumSpace(); // atfdb.comptest:acr_compl.NumSpace
    // void comptest_acr_compl_NumSpaceList(); // atfdb.comptest:acr_compl.NumSpaceList
    // void comptest_acr_compl_OptCumul(); // atfdb.comptest:acr_compl.OptCumul
    // void comptest_acr_compl_OptCumulAlias(); // atfdb.comptest:acr_compl.OptCumulAlias
    // void comptest_acr_compl_OptD(); // atfdb.comptest:acr_compl.OptD
    // void comptest_acr_compl_OptDList(); // atfdb.comptest:acr_compl.OptDList
    // void comptest_acr_compl_OptH(); // atfdb.comptest:acr_compl.OptH
    // void comptest_acr_compl_OptHList(); // atfdb.comptest:acr_compl.OptHList
    // void comptest_acr_compl_OptNonCumul(); // atfdb.comptest:acr_compl.OptNonCumul
    // void comptest_acr_compl_OptNonCumulAlias(); // atfdb.comptest:acr_compl.OptNonCumulAlias
    // void comptest_acr_compl_OptPkeyColon(); // atfdb.comptest:acr_compl.OptPkeyColon
    // void comptest_acr_compl_OptPkeyColonFull(); // atfdb.comptest:acr_compl.OptPkeyColonFull
    // void comptest_acr_compl_OptPkeyColonFullList(); // atfdb.comptest:acr_compl.OptPkeyColonFullList
    // void comptest_acr_compl_OptPkeyColonList(); // atfdb.comptest:acr_compl.OptPkeyColonList
    // void comptest_acr_compl_OptPkeyColonPrefix(); // atfdb.comptest:acr_compl.OptPkeyColonPrefix
    // void comptest_acr_compl_OptPkeyColonPrefixList(); // atfdb.comptest:acr_compl.OptPkeyColonPrefixList
    // void comptest_acr_compl_OptPkeyColonSubstr(); // atfdb.comptest:acr_compl.OptPkeyColonSubstr
    // void comptest_acr_compl_OptPkeyColonSubstrList(); // atfdb.comptest:acr_compl.OptPkeyColonSubstrList
    // void comptest_acr_compl_OptPkeySpace(); // atfdb.comptest:acr_compl.OptPkeySpace
    // void comptest_acr_compl_OptPkeySpaceFull(); // atfdb.comptest:acr_compl.OptPkeySpaceFull
    // void comptest_acr_compl_OptPkeySpaceFullList(); // atfdb.comptest:acr_compl.OptPkeySpaceFullList
    // void comptest_acr_compl_OptPkeySpaceList(); // atfdb.comptest:acr_compl.OptPkeySpaceList
    // void comptest_acr_compl_OptPkeySpacePrefix(); // atfdb.comptest:acr_compl.OptPkeySpacePrefix
    // void comptest_acr_compl_OptPkeySpacePrefixList(); // atfdb.comptest:acr_compl.OptPkeySpacePrefixList
    // void comptest_acr_compl_OptPkeySpaceSubstr(); // atfdb.comptest:acr_compl.OptPkeySpaceSubstr
    // void comptest_acr_compl_OptPkeySpaceSubstrList(); // atfdb.comptest:acr_compl.OptPkeySpaceSubstrList
    // void comptest_acr_compl_OptRegxColon(); // atfdb.comptest:acr_compl.OptRegxColon
    // void comptest_acr_compl_OptRegxColonFull(); // atfdb.comptest:acr_compl.OptRegxColonFull
    // void comptest_acr_compl_OptRegxColonFullList(); // atfdb.comptest:acr_compl.OptRegxColonFullList
    // void comptest_acr_compl_OptRegxColonList(); // atfdb.comptest:acr_compl.OptRegxColonList
    // void comptest_acr_compl_OptRegxColonPrefix(); // atfdb.comptest:acr_compl.OptRegxColonPrefix
    // void comptest_acr_compl_OptRegxColonPrefixList(); // atfdb.comptest:acr_compl.OptRegxColonPrefixList
    // void comptest_acr_compl_OptRegxColonSubstr(); // atfdb.comptest:acr_compl.OptRegxColonSubstr
    // void comptest_acr_compl_OptRegxColonSubstrList(); // atfdb.comptest:acr_compl.OptRegxColonSubstrList
    // void comptest_acr_compl_OptRegxSpace(); // atfdb.comptest:acr_compl.OptRegxSpace
    // void comptest_acr_compl_OptRegxSpaceFull(); // atfdb.comptest:acr_compl.OptRegxSpaceFull
    // void comptest_acr_compl_OptRegxSpaceFullList(); // atfdb.comptest:acr_compl.OptRegxSpaceFullList
    // void comptest_acr_compl_OptRegxSpaceList(); // atfdb.comptest:acr_compl.OptRegxSpaceList
    // void comptest_acr_compl_OptRegxSpacePrefix(); // atfdb.comptest:acr_compl.OptRegxSpacePrefix
    // void comptest_acr_compl_OptRegxSpacePrefixList(); // atfdb.comptest:acr_compl.OptRegxSpacePrefixList
    // void comptest_acr_compl_OptRegxSpaceSubstr(); // atfdb.comptest:acr_compl.OptRegxSpaceSubstr
    // void comptest_acr_compl_OptRegxSpaceSubstrList(); // atfdb.comptest:acr_compl.OptRegxSpaceSubstrList
    // void comptest_acr_compl_OptSig(); // atfdb.comptest:acr_compl.OptSig
    // void comptest_acr_compl_OptSigList(); // atfdb.comptest:acr_compl.OptSigList
    // void comptest_acr_compl_OptV(); // atfdb.comptest:acr_compl.OptV
    // void comptest_acr_compl_OptVList(); // atfdb.comptest:acr_compl.OptVList
    // void comptest_acr_compl_R01a(); // atfdb.comptest:acr_compl.R01a
    // void comptest_acr_compl_R01b(); // atfdb.comptest:acr_compl.R01b
    // void comptest_acr_compl_R01c(); // atfdb.comptest:acr_compl.R01c
    // void comptest_acr_compl_R01d(); // atfdb.comptest:acr_compl.R01d
    // void comptest_acr_compl_R02a(); // atfdb.comptest:acr_compl.R02a
    // void comptest_acr_compl_R02b(); // atfdb.comptest:acr_compl.R02b
    // void comptest_acr_compl_R02c(); // atfdb.comptest:acr_compl.R02c
    // void comptest_acr_compl_R02d(); // atfdb.comptest:acr_compl.R02d
    // void comptest_acr_compl_StrColon(); // atfdb.comptest:acr_compl.StrColon
    // void comptest_acr_compl_StrColonList(); // atfdb.comptest:acr_compl.StrColonList
    // void comptest_acr_compl_StrSpace(); // atfdb.comptest:acr_compl.StrSpace
    // void comptest_acr_compl_StrSpaceList(); // atfdb.comptest:acr_compl.StrSpaceList
    // void comptest_acr_compl_T01(); // atfdb.comptest:acr_compl.T01
    // void comptest_acr_compl_T02(); // atfdb.comptest:acr_compl.T02
    // void comptest_acr_compl_T03(); // atfdb.comptest:acr_compl.T03
    // void comptest_acr_compl_T04(); // atfdb.comptest:acr_compl.T04
    // void comptest_acr_compl_T05(); // atfdb.comptest:acr_compl.T05
    // void comptest_acr_compl_T06(); // atfdb.comptest:acr_compl.T06
    // void comptest_acr_compl_T07(); // atfdb.comptest:acr_compl.T07
    // void comptest_acr_compl_T08(); // atfdb.comptest:acr_compl.T08
    // void comptest_acr_compl_T09(); // atfdb.comptest:acr_compl.T09
    // void comptest_acr_compl_T10(); // atfdb.comptest:acr_compl.T10

    // Validate `atf_cmdline -str:hello`, a command line with a named option, with
    // acr_compl -check, and check that it passes with exit 0.
    // void comptest_acr_compl_CheckValid(); // atfdb.comptest:acr_compl.CheckValid

    // Validate `atf_cmdline -blah` with acr_compl -check, and check that it reports the
    // unknown option and exits 1.
    // void comptest_acr_compl_CheckUnknownOpt(); // atfdb.comptest:acr_compl.CheckUnknownOpt

    // Validate `nonexistent_tool -x` with acr_compl -check, and check that it reports
    // the unknown command and exits 1.
    // void comptest_acr_compl_CheckUnknownCmd(); // atfdb.comptest:acr_compl.CheckUnknownCmd

    // Pipe three acr_compl.checkreq rows into acr_compl -check_batch: a valid line, an
    // unknown command and an unknown option.  Check that the batch prints one
    // acr_compl.checkerr per failing line, stays silent on the valid line, and exits
    // nonzero.
    // void comptest_acr_compl_CheckBatch(); // atfdb.comptest:acr_compl.CheckBatch

    // Validate in batch a line whose -- ends its named options, then a line with an
    // unknown option, and check that the batch reports the second line alone:
    // 1. Validate `gcache -- g++ -c x.cpp`, and check that g++ and -c pass as
    // positional values.
    // 2. Validate `acr -nosuchflag`, and check that the batch still reports its
    // unknown option.
    // Every word after -- is a positional value of its line, so acr_compl leaves g++
    // and -c unchecked against gcache's option names.  The batch parses each line from
    // scratch.  An anon flag left set from the previous line would route every word of
    // every later line down the positional branch, which never reaches the
    // unknown-option check.  A whole documentation corpus would then validate clean
    // after one line that carries --.
    // void comptest_acr_compl_CheckBatchAnon(); // atfdb.comptest:acr_compl.CheckBatchAnon

    // Feed acr_compl -check_batch request rows it fails to parse among valid ones, and
    // check that the batch reports each one and fails the run:
    // 1. Send a request with an unknown option, and check that the batch reports it.
    // 2. Send a request with the unknown attr zzz, as a version-skewed producer would,
    // and a line that is no checkreq tuple at all.  Check that the batch reports
    // each as acr_compl.badreq.
    // 3. Send an empty line, and check that the batch reads it as no request and no
    // error.
    // 4. Send another request with an unknown option, and check that the batch still
    // validates it, reports it, and exits 1.
    // A dropped request would look the same as one that validated clean.
    // void comptest_acr_compl_CheckBatchBadReq(); // atfdb.comptest:acr_compl.CheckBatchBadReq

    // Validate in batch consecutive lines that name different commands, and check that
    // acr_compl checks each line against its own command's option names:
    // 1. Validate the bare command amc twice, and check that both lines pass.
    // 2. Validate `acr -write`, and check that it passes.
    // 3. Validate `amc -write x`, and check that the batch reports -write as an
    // unknown option of amc, though an earlier line used it legally.
    // -write belongs to acr and is missing from amc.  An option-name index accumulated
    // across the batch would hold the union of every command seen so far and accept
    // the flag.  A bare command name with no argument leaves a completion offer behind
    // it.  The repeat of that line must parse against an empty completion set, because
    // the leading offer of the previous line names no option field.
    // void comptest_acr_compl_CheckBatchCmd(); // atfdb.comptest:acr_compl.CheckBatchCmd

    // Export the bash completion variables COMP_LINE, COMP_POINT and COMP_TYPE, then
    // run acr_compl -check_batch with a valid and an invalid request.  Check that the
    // batch reports the invalid request and exits 1.
    // COMP_LINE names the line bash asked to complete, a line unrelated to the command
    // line this process was given.  A validator spawned from a shell that exported it
    // inherits it.  Read as the entrypoint selector, it would discard the process's own
    // argv.  The process would then leave -check_batch unparsed and fd 0 unread, and
    // exit 0, which the caller reads as every queued command validated clean.  The
    // command line decides what runs.
    // void comptest_acr_compl_CheckBatchCompLine(); // atfdb.comptest:acr_compl.CheckBatchCompLine

    // Run acr_compl -check_batch with -schema:- and -data:-, and check that acr_compl
    // rejects the combination before either reader opens fd 0.
    // The requests and the ssim tuples would both arrive on fd 0.  The loader that runs
    // first would consume the whole stream and leave the batch with zero requests to
    // validate.  The rejection keeps a batch from passing clean because the schema
    // loader ate its requests.
    // void comptest_acr_compl_CheckBatchSchemaStdin(); // atfdb.comptest:acr_compl.CheckBatchSchemaStdin

    // Run acr_compl -check_batch with -install, and check that acr_compl rejects the
    // combination, so the response stream carries responses alone.
    // The installation script's `complete` command would go to the same stdout as the
    // response rows, ahead of them.  A consumer reading the stream as tuples would have
    // no way to tell the command from a response row.
    // void comptest_acr_compl_CheckBatchInstall(); // atfdb.comptest:acr_compl.CheckBatchInstall

    // Run acr_compl -check_batch with -check and a line, and check that acr_compl
    // rejects the combination.
    // -check and -check_batch are two validation modes, of which only one runs.  The
    // single-line check would drop without a word.  A validation silently skipped is
    // what this mode exists to prevent.
    // void comptest_acr_compl_CheckBatchCheck(); // atfdb.comptest:acr_compl.CheckBatchCheck

    // Validate `acr % -t`, a command line with a boolean flag, with acr_compl -check,
    // and check that it passes with exit 0.
    // void comptest_acr_compl_CheckValidFlag(); // atfdb.comptest:acr_compl.CheckValidFlag

    // Validate `acr %`, a command line with an anonymous arg, with acr_compl -check, and
    // check that it passes with exit 0.
    // void comptest_acr_compl_CheckValidAnon(); // atfdb.comptest:acr_compl.CheckValidAnon

    // Validate `atf_cmdline -str:hello -num:5 -dbl:1.0 -flag`, a command line with
    // several options, with acr_compl -check, and check that it passes with exit 0.
    // void comptest_acr_compl_CheckMultiOpt(); // atfdb.comptest:acr_compl.CheckMultiOpt

    // Complete -dvariety: without -create, the trigger flag that its dmmeta.finsertwhen
    // row names, and check that the composite Pkey flag falls through to the lookup of
    // existing pkeys.
    // void comptest_acr_compl_InsertWhenLookup(); // atfdb.comptest:acr_compl.InsertWhenLookup

    // Complete -dvariety: with the trigger flag -create and an empty value, and check
    // that insert mode offers the LEFT component values (flowers), each suffixed with
    // the / separator.
    // void comptest_acr_compl_InsertWhenLeft(); // atfdb.comptest:acr_compl.InsertWhenLeft

    // Complete -dvariety:rose/, a value that holds the separator, with the trigger flag
    // -create.  Check that insert mode offers the RIGHT component values (fruits), with
    // the typed left half as a prefix.
    // void comptest_acr_compl_InsertWhenRight(); // atfdb.comptest:acr_compl.InsertWhenRight

    // Complete -dvariety:tu, a partly typed left component, with the trigger flag
    // -create, and check that insert mode filters the flowers by the typed prefix.
    // void comptest_acr_compl_InsertWhenLeftPrefix(); // atfdb.comptest:acr_compl.InsertWhenLeftPrefix

    // Make the directories rose, rosemary and 1511-userproc-pidns and the file roseleaf
    // under bed/, and complete -dflower:ro.  Check that the typed prefix completes to a
    // flower row and to a directory under bed/ alike, and that acr_compl offers rose,
    // which is both, once.  The dmmeta.fcompldir row of -dflower names the directory
    // bed.
    // void comptest_acr_compl_DirPrefix(); // atfdb.comptest:acr_compl.DirPrefix

    // Make the directories rose, 1511-userproc-pidns and the hidden .pidns under bed/,
    // and complete -dflower:pid%ns.  Check that the directory name that the SQL pattern
    // matches only in the middle completes, because no name starts with the value, and
    // that the hidden directory stays out.
    // void comptest_acr_compl_DirSubstr(); // atfdb.comptest:acr_compl.DirSubstr

    // Complete -dflower:ro in a tempdir that lacks the directory bed/, and check that
    // the field completes from its table's keys alone.
    // void comptest_acr_compl_DirMissing(); // atfdb.comptest:acr_compl.DirMissing

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_dm.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_dm_Conflict(); // atfdb.comptest:acr_dm.Conflict
    // void comptest_acr_dm_Merge(); // atfdb.comptest:acr_dm.Merge
    // void comptest_acr_dm_RenameTuple(); // atfdb.comptest:acr_dm.RenameTuple

    // Merge a field table in which ours adds ns.Alpha.a0 at the top and theirs appends
    // ns.Beta.z at the bottom, and check that each addition stays where its branch made
    // it: a0 first, z last.
    // The field table sorts by ctype and leaves the order within a ctype to the file.
    // A row's position in the file is therefore part of the record, so a merge that
    // moves the row changes the schema.
    // void comptest_acr_dm_FieldOrder(); // atfdb.comptest:acr_dm.FieldOrder

    // Merge a 100k-row file that both branches add whole, and check that the merge
    // prints every row:
    // 1. Write 100k rows to new.ssim, and create an empty empty.ssim.
    // 2. Merge with empty.ssim as the base and new.ssim as both ours and theirs.
    // 3. Count the lines of the result with awk.
    // A file that a branch adds whole shares no row with the base, so every row it holds
    // belongs to a single run.  acr_dm places the run as a unit, which keeps the walk
    // over the result one level deep.  Placing each row after the row above it would
    // make the file a chain and recurse once per row.  A recursion that deep fails at a
    // few tens of thousands of rows.  100k rows is past that point and well under a real
    // limit.
    // void comptest_acr_dm_DeepRun(); // atfdb.comptest:acr_dm.DeepRun

    // Merge two branches that each add a run of two fields after ns.C.a, and check that
    // the rebase produces the same file:
    // 1. Merge symmetry1, symmetry2 and symmetry3 into merge.ssim.
    // 2. Run the rebase, the same three files with ours and theirs swapped, into
    // rebase.ssim.
    // 3. Print merge.ssim, and check that rebase.ssim matches it (rebase_same:Y).
    // A rebase hands acr_dm the same three files a merge does, with ours and theirs
    // swapped.  Two runs added at one place leave an ordering decision to make.  acr_dm
    // keeps each run whole and orders the runs against each other by key, the same way
    // round either time.
    // void comptest_acr_dm_Symmetry(); // atfdb.comptest:acr_dm.Symmetry

    // Merge a move that upstream made with an append that the branch made, and check
    // that both survive and that the rebase produces the same file:
    // 1. Merge reorder1, reorder2 and reorder3 into merge.ssim.  Upstream moves
    // ns.Alpha.c to the front of its ctype, and the branch appends ns.Beta.y.
    // 2. Run the rebase, the same three files with ours and theirs swapped, into
    // rebase.ssim.
    // 3. Print merge.ssim, where c comes first, and check that rebase.ssim matches it
    // (rebase_same:Y).
    // A row's position inside its ctype is the member order of a generated struct.  A
    // merge that hands back the base file's order gives the branch a struct that upstream
    // never asked for.  Every gate passes such a merge, because the schema is consistent
    // with itself.  Only upstream's own copy says otherwise.
    // void comptest_acr_dm_Reorder(); // atfdb.comptest:acr_dm.Reorder

    // Move one row to two different places on two branches, and check that the merge
    // reports a conflict and that the rebase leaves the row in the same place:
    // 1. Merge movecfl1, movecfl2 and movecfl3 into merge.ssim, and print its exit code.
    // One file moves ns.C.b after ns.C.c, and the other moves it after ns.C.d.
    // 2. Run the rebase, the same files with ours and theirs swapped, into rebase.ssim.
    // 3. Print merge.ssim.
    // 4. Strip the marker lines from both results, and check that the rest matches
    // (rebase_same:Y).
    // Two files that change one thing to two different values conflict, the same as for
    // an attribute.  The two sides of the marker block are the same row twice, because
    // the text of a row carries no trace of where it sits.  The line above the block
    // therefore names the two positions that disagreed.  A conflict still leaves the row
    // somewhere, and the rebase has to leave it in the same place.  acr_dm orders the two
    // positions by key, so the order is the same either way round.  The marker labels
    // name the file in each slot and swap with it, so the comparison skips them.
    // void comptest_acr_dm_MoveConflict(); // atfdb.comptest:acr_dm.MoveConflict

    // Merge a manifest whose rows carry comment lines, and check that each comment
    // merges as its row's own text.  Ours changes the checksum above cpp/two.cpp.
    // Theirs adds cpp/new.cpp with a checksum of its own.  Check that both survive with
    // each comment still above its row.  Check also that the last line, which belongs to
    // no row, comes out after every row.
    // A line that names no row belongs to the row below it.  apm writes such a line
    // above each dev.gitfile row of its manifest: a checksum of the file that row names.
    // void comptest_acr_dm_Comment(); // atfdb.comptest:acr_dm.Comment

    // Merge with test/acr_dm/nosuchfile.ssim as the third file, and check that acr_dm
    // prints the two files that exist, whole, between markers.
    // A missing file would otherwise read as a file with no rows in it.  The merge would
    // then report that theirs deleted every row, write an empty result and exit zero.
    // Two whole files between markers is the shape of every failure that stops the
    // driver.
    // void comptest_acr_dm_MergeFail(); // atfdb.comptest:acr_dm.MergeFail

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_ed.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_ed_CreateCtype(); // atfdb.comptest:acr_ed.CreateCtype
    // void comptest_acr_ed_CreateMsg(); // atfdb.comptest:acr_ed.CreateMsg
    // void comptest_acr_ed_CreateSrcfileTarget(); // atfdb.comptest:acr_ed.CreateSrcfileTarget
    // void comptest_acr_ed_CreateSsimfile(); // atfdb.comptest:acr_ed.CreateSsimfile
    // void comptest_acr_ed_CreateSsimfileBadNs(); // atfdb.comptest:acr_ed.CreateSsimfileBadNs
    // void comptest_acr_ed_CreateTarget(); // atfdb.comptest:acr_ed.CreateTarget

    // Delete a field of a ssimfile's ctype and a field of an in-memory ctype, and check
    // that only the first delete rewrites a ssimfile:
    // 1. Delete dmmeta.Ns.license, and check that acr_ed drops the schema row and then
    // rewrites the ssimfile that held the field's values.
    // 2. Delete amc.FNs.p_license, and check that acr_ed emits the delete alone.
    // A field on an in-memory ctype has no ssimfile that holds its values.
    // void comptest_acr_ed_DelField(); // atfdb.comptest:acr_ed.DelField

    // Rename a field with four spellings of the new name, and check which ones acr_ed
    // accepts:
    // 1. Rename dmmeta.Ns.license to the bare name licence, and check that acr_ed
    // resolves it against the field's own ctype.
    // 2. Rename it to field:dmmeta.Ns.licence, and check that acr_ed refuses the query
    // prefix.
    // 3. Rename it to dmmeta.Ctype.license, and check that acr_ed refuses the move to a
    // new ctype.
    // 4. Rename amc.FNs.p_license to amc.FCtype.p_license, and check that the move goes
    // through.
    // The two refused spellings mean something other than what they look like.  After
    // the move in step 3 the old rows would keep a column that no field claims.  Two
    // in-memory ctypes have no such column, so the move in step 4 goes through.
    // void comptest_acr_ed_RenameField(); // atfdb.comptest:acr_ed.RenameField

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_in.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_in_Reverse(); // atfdb.comptest:acr_in.Reverse
    // void comptest_acr_in_Simple(); // atfdb.comptest:acr_in.Simple
    // void comptest_acr_in_Tree(); // atfdb.comptest:acr_in.Tree

    // -------------------------------------------------------------------
    // cpp/atf_comp/amc.cpp
    //

    // Give amc a field of reftype Ptrary without its dmmeta.ptrary record, and check
    // that amc reports amc.missing_ptrary, modifies no file and exits nonzero.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_amc_MissingPtrary(); // atfdb.comptest:amc.MissingPtrary

    // Dump with acr_in the schema closure that the live tree declares as amc's own
    // input, feed it to amc on stdin, and check that amc generates every namespace the
    // tree defines without error.
    // Every other test in this file gives amc a fixture of a handful of ctypes.  Such a
    // fixture reaches only the generator modules that its ctypes need.  The empty
    // -out_dir: keeps this run from writing the code it generates.
    // NOTE: the test runs with memcheck:none because amc over the whole schema
    // closure is too slow under valgrind.
    // void comptest_amc_SchemaClosure(); // atfdb.comptest:amc.SchemaClosure

    // Give amc a big-endian u16 field in a universe without the u16 csize row, and
    // check that amc.bigend_size names the missing dmmeta.csize row.
    // The type's width is unknown here, and u16 itself supports big-endian storage.  A
    // diagnostic that prescribed a width change would send the user to fix a width that
    // is already right.
    // void comptest_amc_BadBigendNosize(); // atfdb.comptest:amc.BadBigendNosize

    // Give amc a big-endian field of reftype Inlary on a ctype with a fieldwise
    // constructor, and check that amc reports amc.bad_bigend and exits nonzero.
    // amc reports the rejection and continues generating to the end of the run.  Each
    // further schema error adds to the exit code.
    // void comptest_amc_BadBigendReftype(); // atfdb.comptest:amc.BadBigendReftype

    // Give amc a big-endian u32 field whose bltin row sets bigendok:N, and check that
    // amc reports amc.bigend_deadend.
    // The u32 width has a byteswap primitive.  The bltin row still forbids storing u32
    // byteswapped, and the Val accessors would swap it anyway.  The check that reads
    // the flag runs apart from the width check, so it needs its own pin.
    // void comptest_amc_BadBigendDeadend(); // atfdb.comptest:amc.BadBigendDeadend

    // Give amc a big-endian u128 field whose bltin row claims bigendok, and check that
    // amc reports amc.bigend_width.
    // Byteswap primitives exist for 16, 32 and 64 bits.  The Val accessors would call
    // be128toh and htobe128, which do not exist.  The width check rejects the field
    // whatever the bltin row claims.
    // void comptest_amc_BadBigendU128(); // atfdb.comptest:amc.BadBigendU128

    // Give amc one field for each of the four ways to pair reftype Bitfld with a
    // dmmeta.bitfld record, and check that amc reports amc.missing_record for both
    // mismatched fields in one run.
    // Reftype Bitfld and the dmmeta.bitfld record imply each other.  A Bitfld field
    // without the record (flag) carries no bit offset or width.  A record on a field of
    // another reftype (notbitfld) gets a full-width member, which the ctype's own
    // compare, print and read walks pass over.  The correctly paired field (good) and
    // the plain Val fields draw no report.
    // void comptest_amc_BadBitfldReftype(); // atfdb.comptest:amc.BadBitfldReftype

    // Give amc a bitset over a 9-byte (72-bit) Smallstr element, and check that amc
    // reports amc.bitset_elem.
    // The element is no unsigned integer builtin, so it has no exact bit index/shift
    // split.
    // void comptest_amc_BadBitsetElem(); // atfdb.comptest:amc.BadBitsetElem

    // Give amc a bitset over an i8 element, and check that amc reports amc.bitset_elem.
    // A signed element compiles and sign-extends on read, so bit queries such as Sup
    // return wrong values at run time.
    // void comptest_amc_BadBitsetSigned(); // atfdb.comptest:amc.BadBitsetSigned

    // Give amc a bitset over a u32 element in a universe without the u32 csize row, and
    // check that amc.bitset_elem_size names the missing csize row.
    // The u32 element type is valid.  amc lacks only its width.  A diagnostic that
    // prescribed a different element type would send the user to replace a valid type.
    // void comptest_amc_BadBitsetNosize(); // atfdb.comptest:amc.BadBitsetNosize

    // Give amc five bitsets, three of them over elements whose csize row states a width
    // other than the width in the type's name.  Check that amc reports
    // amc.bitset_elem_width for each of the three, and that the two controls draw no
    // report.
    // Each type name passes the unsigned-builtin check, while amc generates the
    // accessors from the csize row.  A u32 of 3 bytes is 24 bits, which the FloorLog2
    // index split truncates.  A u128 of 32 bytes is 256 bits, past the widest bit-scan
    // helper.  A u16 of 8 bytes is 64 bits.  The accessors index that width well.  The
    // struct declares a 16-bit member, though, so the shifts run off its end.  The u64
    // and u8 elements are the controls, because their csize rows agree with their names.
    // void comptest_amc_BadBitsetWidth(); // atfdb.comptest:amc.BadBitsetWidth

    // Give amc two fixed-inlary field defaults that name the record by the Init
    // parameter name, in shapes outside what the copy-constructor inliner rewrites.
    // Check that amc reports amc.copyctor_init for both and exits nonzero.
    // One default names the record bare (sizeof parent).  Inlining would leak that name
    // into the member copy constructor as an undeclared identifier.  The other reaches
    // the name through a member operator (gcfg.parent.n), where the name belongs to
    // another record.  The member-access rewrite would take the operator with it, and
    // turn the default into a different expression that still compiles.
    // void comptest_amc_BadCopyctorInit(); // atfdb.comptest:amc.BadCopyctorInit

    // Give amc inlary fields in each shape that the copy-constructor inliner covers,
    // print algo_lib.Rec..CopyCtor, and check that the constructor body retargets each
    // reference to the record to *this and copies the rest of each default verbatim.
    // A variable inlary's Init macro carries the two shapes that the generators emit:
    // member access on the record, and the record passed to a function by reference.
    // The inliner retargets both to *this.  A fixed inlary's default is arbitrary user
    // text.  A default that names the record as *this (size) reaches the inliner
    // spelled as the Init parameter, and returns to *this.  The inliner copies verbatim
    // a default that names no record (lit).  It also copies gparent.n verbatim (fix),
    // because a longer identifier that merely ends with the parameter name is a
    // different name.
    // The str, note and raw defaults carry the parameter name inside a string literal,
    // a block comment, and a raw string literal whose payload holds the quote that ends
    // an ordinary one.  Each is content, so the inliner copies each verbatim and amc
    // reports none.  The chr default puts a reference to the record after a character
    // literal that holds a quote.  A scan that read that quote as opening a string
    // literal would lose the reference, and leave the undeclared parameter name in the
    // constructor.  With the literal last, the same misreading would run out of text
    // and emit the same body, so the test puts the reference behind it.
    // void comptest_amc_CopyctorInit(); // atfdb.comptest:amc.CopyctorInit

    // Give amc one Bitfld field per combination of arg type and width, print each
    // field's ReadStrptrMaybe, and check which readers bound the value by the field's
    // store.
    // A read parses into a temporary of the field's arg type, and the Bitfld setter
    // masks the store.  So the store truncates a value too wide for the bits, unless
    // the reader refuses it first.  The reader of a field with a symbolic constant
    // tries the symbol and falls back to the arg type.  amc emits the refusal there,
    // with a term for each end of the arg type's range that the field's bits fall
    // short of.
    // At the high end narrow (u32 in 3 bits) and qos (a u32 wrapped in one field, in 4
    // bits) draw a bound.  full (u8 in 8 bits), signed_full (i8 in 8 bits) and flag
    // (bool in 1 bit) span their whole arg type and draw none.
    // At the low end a store that drops the arg type's sign bit reads a negative back
    // as a positive number.  sig (an i32 wrapped in one field, in 4 bits) refuses a
    // negative on top of its high-end bound.  The bits of sig_edge (an i8 wrapper in 7
    // bits) hold every positive value of its arg type, so it refuses a negative with
    // no high-end bound.  sig_full (the same wrapper in 8 bits) keeps the sign and
    // draws neither term.
    // plain is narrow again with no symbolic constant, and its reader carries no term.
    // Its three bits take a "9" as 1, and the read reports success.
    // An fdec field carries its bound inside the loops that its reader parses with.
    // dec scales two decimal places into 12 bits.  So the digit loop takes a digit
    // only while the value it builds still fits 4095, the bound of the field's own
    // store.  The u64 that the digits accumulate into is wider.  The loop that fills
    // the decimal places a short text leaves out tests the same bound.  The reader
    // leaves the finished magnitude untested, because it bounded every digit that went
    // into it.
    // amc rejects one combination at generation time, a width past the arg type, so
    // it never reaches a reader.  amc also rejects a narrower width over a signed arg
    // type, in a universe that carries the amcdb.bltin table.  This fixture carries
    // none.  Signedness has two sources.  The range machinery reads it from the type's
    // name, so i8 and i32 are signed here.  The size pass reads the type's amcdb.bltin
    // row and finds none, so its amc.bitfield_signed_narrow never fires.  That is why
    // sig and sig_edge reach a reader in this golden.  With the table, amc reports
    // both of them and generates no code.  The name-based source is deliberate.  A
    // universe without a signed integer's amcdb.bltin row still gives it signed range
    // checks, and this fixture is that universe.
    // void comptest_amc_BitfldReadRange(); // atfdb.comptest:amc.BitfldReadRange

    // Give amc a cascdel Ptrary whose row ctype has no instance pool, and check that amc
    // reports amc.nopool on the xref and then the cascdel pool vrfy error.
    // With no pool to delete a row from, DeleteExpr would be empty.  The Cascdel loop
    // would then spin forever.  The test pins the existing rejection, so the loop stays
    // unreachable.
    // void comptest_amc_BadCascdelNopool(); // atfdb.comptest:amc.BadCascdelNopool

    // Give amc a cascdel unique:Y Ptrary whose field has no dmmeta.xref record, and
    // check that amc reports amc.cascdel_xref.
    // The generated Cascdel deletes the last row until the array is empty.  Only an
    // xref makes the row's delete unlink it, so without one the loop would never end.
    // void comptest_amc_BadCascdelXref(); // atfdb.comptest:amc.BadCascdelXref

    // Give amc a buffer condition on a global (FDb) fbuf, and check that amc reports
    // amc.fcond_arg.
    // The record that enters the list is the buffer's parent row.  A global has no row
    // to enter it with.
    // void comptest_amc_BadFbufCondGlobal(); // atfdb.comptest:amc.BadFbufCondGlobal

    // Give amc cfmts across the strfmt and printfmt pairs, and check that amc reports
    // amc.badcfmt, naming both attributes, for each print:Y pair with no print path.
    // Such a cfmt would ship a Print function with an empty body.  amc rejects one
    // ctype per axis.  Sample has the Tuple strfmt, which has no print path under any
    // printfmt.  Compact has the CompactSep printfmt under the String strfmt, and the
    // String arm leaves that printfmt unhandled.  The rest of the table are controls
    // that draw no diagnostic: String with Tuple, Auto, Raw, Sep and Bitset; Argv and
    // Json, which take any printfmt because neither reads one; the Extern printfmt,
    // which hands the body to the user even under the Tuple strfmt; and a print:N cfmt
    // (Quiet) that carries the rejected printfmt and generates no print function.
    // void comptest_amc_BadCfmtPrint(); // atfdb.comptest:amc.BadCfmtPrint

    // Give amc read:Y cfmts across the whole table of strfmt and printfmt pairs that
    // the read emitter decides, and check that amc reports amc.badcfmt, naming both
    // attributes, for the one pair with no read path.
    // Such a cfmt would ship a ReadStrptrMaybe with an empty body.  That reader returns
    // success for every input, and leaves the destination holding its old value.  A
    // caller then has no way to tell a parsed value from an unparsed one.  The rejected
    // pair is the CompactSep printfmt under the String strfmt (RCompact), the one
    // printfmt that the String arm has no reader for.  The rest of the table are
    // controls that draw no diagnostic: String with Tuple, Auto, Raw, Sep, Bitset and
    // Extern, where Auto reads a tuple the way the print side prints one; the Tuple
    // strfmt, whose reader consumes an already-parsed tuple and ignores the printfmt,
    // so it accepts CompactSep and still hands Extern to the user; the Json strfmt,
    // which the read emitter passes over; and a read:N cfmt (RQuiet) that carries the
    // rejected printfmt and generates no read function.
    // void comptest_amc_BadCfmtRead(); // atfdb.comptest:amc.BadCfmtRead

    // Give amc a String cfmt with printfmt:CompactSep, and check that the print and the
    // read emitter each report amc.badcfmt and that the run reaches its exit with a
    // nonzero code.
    // The printfmt table declares the value.  amc generates neither a printer nor a
    // reader for it, so the request would ship an empty print body and an empty read
    // body.  Each diagnostic names the cfmt, the strfmt and the offending printfmt.
    // This golden pins the two diagnostics of one cfmt.  BadCfmtPrint and BadCfmtRead
    // pin the wider table that each emitter decides.  The Sep ctype (Ok) beside it is
    // the control, and draws no diagnostic on either side because amc generates its
    // printfmt.
    // void comptest_amc_BadCompactSep(); // atfdb.comptest:amc.BadCompactSep

    // Give amc the table of command-line shapes that the reader's gates decide, and
    // check that amc reports amc.badargvread for each rejected shape, naming the reader
    // and the companion that it lacks.
    // amc assembles the field-aware command-line reader from companion functions, and
    // emits each companion under its own gate.  A read cfmt with strfmt:Argv yields the
    // reader and NArgs.  A read cfmt whose printfmt is neither Raw nor Extern yields
    // ReadFieldMaybe and GetAnon.  The namespace-level ReadArgv calls the cmdline
    // ctype's reader.
    // These shapes draw no diagnostic: command.good (Argv read:Y printfmt Auto over a
    // base that supplies both companions); command.goodnobase (no basecmdline, so the
    // reader calls no base companion); command.goodraw (its Argv cfmt is Raw, and a
    // second read cfmt supplies ReadFieldMaybe and GetAnon for its anonfld); and
    // namespace okapp over command.good.
    // amc rejects the rest: command.raw and command.xtern, whose only read cfmt is Raw
    // or Extern, so they lack the store and the positional lookup; command.basenoread
    // and command.baseraw, whose base lacks NArgs or the store; algo_lib.CmdlineRaw,
    // which is a base and a broken reader at once; and namespaces noreadapp and gnuapp,
    // whose cmdline ctype yields no reader.  The Argv cfmt behind noreadapp is read:N.
    // ArgvGnu, behind gnuapp, generates no reader however it is spelled.
    // void comptest_amc_BadArgvRead(); // atfdb.comptest:amc.BadArgvRead

    // Give amc Bytebuf, Linebuf and Msgbuf fields over elements of several widths and
    // comparison sources, and check that amc reports amc.fbuf_elem for each buffer
    // whose element breaks the way the buffer measures or scans a message.
    // A Bytebuf or Linebuf measures its message in bytes and hands it back as an
    // element count.  A wider element makes the returned message run past the data.
    // amc takes the element's width from the declared csize of a bltin, and from the
    // emitted struct body of a generated ctype.  The fixture crosses both buffer types
    // with both width sources.  amc rejects a byte buffer of u16, of u64, of an
    // eight-byte generated struct and of an extern struct of unknown width, and a line
    // buffer of u32.  The accepted controls are both buffer types over a one-byte bltin
    // and over a one-byte generated struct, and a Msgbuf over the eight-byte struct.  A
    // Msgbuf carries its length in the message header.
    // A Linebuf needs more than a one-byte element.  Its generated ScanMsg finds the
    // end of the message by comparing each element with the delimiter.  So the same
    // fixture crosses the sources of an operator == with the shapes that emit no
    // comparison:
    //
    // Linebuf element                    | ScanMsg      | accepted
    //
    // one-byte bltin (char)              | generated    | yes
    // struct, ccmp genop:Y               | generated    | yes
    // struct, ccmp extrn:Y               | generated    | yes
    // struct, ccmp order:Y genop:N       | generated    | no
    // struct, no ccmp                    | generated    | no
    // struct, no ccmp                    | ffunc extrn:Y| yes
    // struct, no ccmp, out direction     | not emitted  | yes
    //
    // The Bytebuf over the same ccmp-less struct stays accepted throughout, because it
    // returns every available byte and compares no element.
    // void comptest_amc_BadFbufElem(); // atfdb.comptest:amc.BadFbufElem

    // Give amc Varlen, Opt and Val fields over an element type with a destructor and
    // over a plain one, and check that amc reports amc.opt_dtor for the Varlen and the
    // Opt field over the element with a destructor.
    // A Varlen or an Opt field addresses its element in place inside the message that
    // carries it.  The message allocates no storage for the element, and destroying
    // the message ends no lifetime of the element's own.  The program would construct
    // an element whose type owns a destructor and never destroy it, which leaks
    // whatever the element holds.  So amc refuses the schema at generation time.
    // The ctype decides whether an element type owns a destructor.  The pass that also
    // decides whether a value is plain data computes this flag for the whole ctype
    // graph, so this check reads a flag that a different pass fills in.  Run before
    // that pass, the check would read the flag's initial value, find every element
    // type destructor-free, and pass a schema that it exists to refuse.  So the
    // fixture carries the accepted rows next to the refused ones.  A check that has
    // gone quiet treats both kinds alike.
    //
    // field reftype | element type       | accepted
    //
    // Varlen        | has destructor     | no
    // Opt           | has destructor     | no
    // Varlen        | plain data         | yes
    // Opt           | plain data         | yes
    // Val           | has destructor     | yes
    //
    // The element that owns a destructor is an extern ctype declared plaindata:N, the
    // shortest way to say that destroying it does something.  The plain element is a
    // packed ctype of one integer field.  The Val row is the control on the rule's
    // other edge.  That reftype gives its member a lifetime of its own, so the member's
    // destructor runs and the element type is unconstrained.
    // void comptest_amc_BadOptDtor(); // atfdb.comptest:amc.BadOptDtor

    // Give amc a universe with an FDb index of each reftype and a condition whose ins
    // index is a hash, and check that amc reports amc.fcond_ins for the hash.
    // A condition inserts into its ins index by name, one row at a time.  So the index
    // must let a single row in and out.  A hash is the tempting mistake, because it
    // holds the same rows and its name reads like a list's.  It offers InsertMaybe in
    // place of Insert, so the emitted call fails to compile.  The rule states the
    // requirement positively, as an Llist or a Bheap.  That one rule refuses every
    // other shape a field can have: a pool and a pointer array, which lack a
    // single-row remove; a count, which keeps a tally and hands no row back; and a
    // tree or a sorted list, whose insert takes more than the row.
    // The fcond rules are vrfy checks, so the first violation ends the run.  Each run
    // therefore names one rejected row, and a fixture crossing every reftype would
    // report the same single line.  The accepted control is a condition on the
    // sibling buffer that names an Llist of the watched ctype, and it draws no
    // diagnostic.
    // void comptest_amc_BadFcondIns(); // atfdb.comptest:amc.BadFcondIns

    // Give amc a TimeHookOnce fstep on an Llist field, and check that amc reports
    // amc.fstep_bheap.
    // The step reads the expiration time from the first row's sort field, and only a
    // Bheap step field provides one.
    // void comptest_amc_BadFstepBheap(); // atfdb.comptest:amc.BadFstepBheap

    // Give amc a TimeHookRecur fstep on an Upptr field, and check that amc reports
    // amc.fstep_first.
    // The generated code arms the time hook from the index's first row, and only an
    // Llist or Bheap step field provides one.
    // void comptest_amc_BadFstepFirst(); // atfdb.comptest:amc.BadFstepFirst

    // Give amc a TimeHookRecur fstep on an Atree field, and check that amc reports
    // amc.fstep_first.
    // FirstChanged calls arm the hook, and only the Llist and Bheap generators emit
    // them.  An Atree step would compile and never fire.
    // void comptest_amc_BadFstepAtree(); // atfdb.comptest:amc.BadFstepAtree

    // Give amc an fstep on a fixed Inlary field, and check that amc reports
    // amc.fstep_reftype.
    // The step's loop condition tests the index with EmptyQ, which amc generates for a
    // variable Inlary alone.  A fixed Inlary always holds max elements and offers no
    // emptiness test.
    // void comptest_amc_BadFstepInlary(); // atfdb.comptest:amc.BadFstepInlary

    // Give amc an InlineRecur fstep with fdelay scale:Y on a Val field, and check that
    // amc reports amc.fstep_scale.
    // The scaled delay divides by the step index's row count, and a Val field has no
    // row count.
    // void comptest_amc_BadFstepScale(); // atfdb.comptest:amc.BadFstepScale

    // Give amc a Callback fstep on a ZSListMT field, and check that amc reports
    // amc.fstep_zslistmt.
    // The list is concurrent and has no EmptyQ, so the step's loop condition tests
    // DestructiveFirst.  Only the Inline and InlineRecur call shapes embed that test.
    // void comptest_amc_BadFstepZslistmt(); // atfdb.comptest:amc.BadFstepZslistmt

    // Give amc an fdelay row on an Inline fstep, and check that amc reports
    // amc.fstep_fdelay.
    // Only the InlineRecur and TimeHookRecur call shapes read the delay between
    // invocations.  On any other steptype the row would go unused.
    // void comptest_amc_BadFstepFdelay(); // atfdb.comptest:amc.BadFstepFdelay

    // Give amc an fstep on an Lpool field, and check that amc reports
    // amc.fstep_reftype.
    // The step's loop condition tests the field for pending work.  A memory pool
    // offers no emptiness test, so the generated condition would call an EmptyQ that
    // does not exist.
    // void comptest_amc_BadFstepReftype(); // atfdb.comptest:amc.BadFstepReftype

    // Give amc InlineRecur fsteps with fdelay scale:Y on a Blkhash and on a Lary field,
    // and check that amc generates the schema without a diagnostic.
    // Both reftypes generate the N function that the scaled delay divides by.
    // void comptest_amc_FstepScaleBlkhash(); // atfdb.comptest:amc.FstepScaleBlkhash

    // Give amc an fstep on a field of a non-global ctype, and check that amc reports
    // amc.fstep_global.
    // The generated step would read the field through the namespace global, and the
    // FDb has no such member.
    // void comptest_amc_BadFstepGlobal(); // atfdb.comptest:amc.BadFstepGlobal

    // Give an executable an Alias field on its FDb that names a library's list, with an
    // fstep on the alias.  Print the executable's Steps(), and check that amc generates
    // the alias step in the executable and calls it there.
    // void comptest_amc_FstepAlias(); // atfdb.comptest:amc.FstepAlias

    // Give an executable an alias step on a library list that has a step of its own.
    // Print the executable's Steps(), and check that it calls the alias step in the
    // library step's slot and never the library's step.
    // void comptest_amc_FstepAliasOverride(); // atfdb.comptest:amc.FstepAliasOverride

    // Give an executable an alias step whose band (idle) differs from the band of the
    // step it overrides, and check that amc reports amc.fstep_alias_band.
    // The override runs in the library step's slot.  A band of its own would have no
    // say in its position, and would still change how amc generates its wrapper.
    // void comptest_amc_BadFstepAliasBand(); // atfdb.comptest:amc.BadFstepAliasBand

    // Give an executable an alias step of steptype TimeHookRecur, and check that amc
    // reports amc.fstep_alias_steptype.
    // The list's FirstChanged arms the time hook, and amc generates FirstChanged in the
    // list's own namespace.
    // void comptest_amc_BadFstepAliasSteptype(); // atfdb.comptest:amc.BadFstepAliasSteptype

    // Give one executable's closure two alias steps on one list, one in the executable
    // and one in a library it links, and check that amc reports amc.fstep_alias_rival.
    // The two steps share the process's main loop and would race for the list's rows.
    // void comptest_amc_BadFstepAliasRival(); // atfdb.comptest:amc.BadFstepAliasRival

    // Give an executable an alias step on a list whose own step is a TimeHookRecur,
    // and check that amc reports amc.fstep_alias_timehook.
    // The list's own step runs from the time hook and never from Steps(), so the alias
    // step would find no slot in Steps() to take over.
    // void comptest_amc_BadFstepAliasTimehook(); // atfdb.comptest:amc.BadFstepAliasTimehook

    // Give an executable an alias step that names a field of a non-global ctype, and
    // check that amc reports amc.fstep_alias_list.
    // Such a field is no list of another namespace's FDb.
    // void comptest_amc_BadFstepAliasList(); // atfdb.comptest:amc.BadFstepAliasList

    // Give an executable steps in the input, work, output and idle bands, and a library
    // step in the work band.  Print the executable's Steps(), and check that it calls
    // the steps band by band: the input step, the work band, the output step and then
    // the idle step.
    // Namespaces run in dependency order within a band, so the executable's own work
    // step precedes the library's.
    // void comptest_amc_FstepBand(); // atfdb.comptest:amc.FstepBand

    // Give an executable a TimeHookRecur step in the idle band, print the list's
    // FirstChanged, and check that it arms the hook on the idle heap of algo_lib.
    // void comptest_amc_FstepTimehookIdle(); // atfdb.comptest:amc.FstepTimehookIdle

    // Give an executable a TimeHookRecur step in the output band, which has no algo_lib
    // heap, and check that amc refuses it with amc.fstep_timehook_band.
    // void comptest_amc_BadFstepTimehookBand(); // atfdb.comptest:amc.BadFstepTimehookBand

    // Give an executable an idle step with fsteprate 2/3, print the step's Call, and
    // check that each pass earns two thirds of a call, that the credit caps at 4, and
    // that the step also runs in a pass that has time to give up.
    // The cap at 4 loses none of the credit.
    // void comptest_amc_FstepRate(); // atfdb.comptest:amc.FstepRate

    // Give an executable an idle step with no fsteprate row, print the step's Call, and
    // check that the step runs at the idle band's default rate, 1/8.
    // void comptest_amc_FstepIdleRate(); // atfdb.comptest:amc.FstepIdleRate

    // Give an executable an InlineRecur step with an fsteprate row, and check that amc
    // reports amc.fstep_rate_steptype.
    // An InlineRecur step keeps its own clock.
    // void comptest_amc_BadFstepRateSteptype(); // atfdb.comptest:amc.BadFstepRateSteptype

    // Give an executable a step with an fsteprate of zero calls, and check that amc
    // reports amc.fstep_rate_zero.
    // A rate of zero calls would never run the step.
    // void comptest_amc_BadFstepRateZero(); // atfdb.comptest:amc.BadFstepRateZero

    // Give amc a dispatch filter over a message field that has its own Print function
    // (an fconst field), print the filter's MatchEv, and check that the regx match
    // prints the field through that function with the message as the parent argument.
    // The golden pins the whole emitted Match body, including the well-formed Print
    // call.
    // void comptest_amc_DispfilterFieldPrint(); // atfdb.comptest:amc.DispfilterFieldPrint

    // Give amc a dispatch filter declared match_all:Y over two messages that between
    // them cover every shape the rule has to place.  Print the filter's Match
    // functions, and check how each one reads or refuses its message.
    // Ev carries both filter fields, so its Match body reads the message.  The
    // equality and the range tests take the big-endian field through its Get function.
    // The regx match prints the message's own field through its fconst Print function
    // before matching.  Ev2 carries neither field.  So each of Ev's filter fields turns
    // into a present test that refuses the message, which is what match_all means.
    // Ev2's own field is the control on the other side.  Ev's present tests for it are
    // the same refusal in reverse.
    // void comptest_amc_DispfilterMatchAll(); // atfdb.comptest:amc.DispfilterMatchAll

    // Run amc over test/amc/bad_gconst_badline, whose gconst value table has a fourth
    // and a fifth line that are no tuples, and print algo_lib.ColorId.  Check that amc
    // reports amc.bad_gconst for both lines with the file, the line number and the
    // line's text.
    // Each bad line carries an unterminated quoted value.  The reader keeps what it
    // read before the quote ran out and jumps to end of line.  The truncated value
    // would then read back as a constant name the table never wrote, and every
    // attribute past the bad quote would drop, the comment among them.  The tuple
    // reader itself supplies none of the file, the line number or the line's text.
    // The printed enum pins the controls too: a blank line and a comment line draw no
    // diagnostic; the two well-formed rows still emit; and the auto-numbering counts
    // only the lines that produced a constant.
    // void comptest_amc_BadGconstBadline(); // atfdb.comptest:amc.BadGconstBadline

    // Run amc over test/amc/bad_gconst_dup, whose gconst value table holds one name
    // twice, and check that amc reports amc.gconst_dup.
    // The second row would silently vanish from the enum and still take a numbering
    // index.  The numbering would then have a gap on a clean exit.
    // void comptest_amc_BadGconstDup(); // atfdb.comptest:amc.BadGconstDup

    // Run amc over test/amc/bad_gconst_headonly, whose gconst value table has a line
    // with a head and no attribute, and check that amc reports amc.gconst_headonly.
    // The line creates no constant, yet auto-numbering would take an index for it.
    // That would silently renumber every constant that follows.
    // void comptest_amc_BadGconstHeadonly(); // atfdb.comptest:amc.BadGconstHeadonly

    // Run amc over test/amc/bad_gconst_ctype, whose gconst idfld names an existing
    // field of a ctype other than the namefld's, and check that amc reports
    // amc.gconst_idfld.
    // The value table's rows lack that field, so every fconst value would come out
    // empty.
    // void comptest_amc_BadGconstCtype(); // atfdb.comptest:amc.BadGconstCtype

    // Run amc over test/amc/bad_gconst_idfld, whose gconst idfld names a field that
    // does not exist, and check that amc reports amc.gconst_idfld.
    // The report keeps the id column from silently falling back to the name column.
    // void comptest_amc_BadGconstIdfld(); // atfdb.comptest:amc.BadGconstIdfld

    // Run amc over test/amc/bad_gconst_ctype_char, with two gconsts whose idfld is bad,
    // the first on a char-typed field.  Check that amc reports amc.gconst_idfld for
    // both and exits.
    // A run that went on to process the known-bad row would abort the scan on a
    // char-parse failure that names no context.
    // void comptest_amc_BadGconstCtypeChar(); // atfdb.comptest:amc.BadGconstCtypeChar

    // Run amc over test/amc/bad_gconst_char, whose char-typed gconst has a value table
    // with multi-character values, and print algo_lib.ColorId.  Check that amc reports
    // amc.gconst_char for each offending row with its gconst, name and value, and
    // scans on to the end of the table.
    // The printed enum pins the table's one valid row too.  The row survives its
    // skipped neighbors and emits as a single-quoted character constant.
    // void comptest_amc_BadGconstChar(); // atfdb.comptest:amc.BadGconstChar

    // Run amc over test/amc/gconst_indir, whose gconst value table exists only inside
    // that directory.  Check that amc side-loads the values from the -in_dir root and
    // prints no amc.sideload notice for a directory universe.
    // data/ under the CWD lacks the table, so only the -in_dir root can supply it.
    // void comptest_amc_GconstIndir(); // atfdb.comptest:amc.GconstIndir

    // Run amc over test/amc/gconst_missing, whose -in_dir root lacks the gconst's value
    // table, and check that amc reports amc.load with the file and exits nonzero.
    // The table's rows are the enum's constants.  Loading the table as zero rows would
    // compile an empty enum and exit 0.
    // void comptest_amc_GconstLoadFail(); // atfdb.comptest:amc.GconstLoadFail

    // Give amc a gconst and a gstatic over each of two ctypes that reach no ssimfile,
    // and check that amc reports amc.gconst_nossimfile and amc.gstatic_nossimfile for
    // all four rows in one run.
    // The ctype that the rows would come from reaches no ssimfile, so amc has no file
    // to look for.  This is the other way a table can be absent.  amc reports it apart
    // from a named file that fails to read, because a reader that was never given a
    // file has no file to report.  amc.GconstLoadFail and amc.GstaticLoadFail cover
    // the named file.  Their ctypes reach a ssimfile, one of its own and one through a
    // base, which makes them the controls for this test.
    // A ctype reaches no ssimfile in two ways, and the fixture holds both for each
    // consumer.  Free declares no base, and Derived has a base that owns no ssimfile
    // either.  Each report names the offending gconst or gstatic field and lets the
    // run continue, so one run reports all four rows.
    // void comptest_amc_SideloadNossimfile(); // atfdb.comptest:amc.SideloadNossimfile

    // Run amc over test/amc/gstatic_missing, whose -in_dir root lacks the table file of
    // a gstatic field, and check that amc reports amc.load with the file and exits
    // nonzero.
    // The rows would become the compiled-in registry.  Loading the table as zero rows
    // would silently drop every entry.
    // void comptest_amc_GstaticLoadFail(); // atfdb.comptest:amc.GstaticLoadFail

    // Run amc over test/amc/gstatic_badline, whose gstatic table has a second line with
    // an unterminated quoted value, and print algo_lib.FDb.color.LoadStatic.  Check
    // that amc reports amc.bad_gstatic with the field that was reading, the file, the
    // line number and the line's text, and exits nonzero.
    // The bad line would silently drop out of the compiled-in registry.  The tuple
    // reader itself supplies none of the file, the line number or the line's text.
    // The printed loader pins the two well-formed lines around the bad one as well.
    // They come back as the loader's own rows, and a run that abandoned the table at
    // the first bad line would show there.  The universe also carries an fconst whose
    // value is a character literal.  So an earlier pass leaves a bad-number tag that
    // names that valid value in algo_lib's shared error buffer.  A message taken from
    // that buffer would name the fconst, and miss the file that is malformed.
    // void comptest_amc_GstaticBadLine(); // atfdb.comptest:amc.GstaticBadLine

    // Run amc over test/amc/gsymbol_badline, whose gsymbol table has a fourth and a
    // fifth line that are no tuples, each with an unterminated quoted value.  Check
    // that amc reports amc.bad_gsymbol for both lines with the file, the line number
    // and the line's text, and exits nonzero.
    // The truncated text would become a symbol name and value the table never wrote.
    // Every reference to the intended symbol would then fail to link.
    // void comptest_amc_GsymbolBadline(); // atfdb.comptest:amc.GsymbolBadline

    // Run amc over test/amc/gsymbol_missing, whose -in_dir root lacks a gsymbol's
    // table, and check that amc reports amc.load with the file and exits nonzero.
    // The table's rows are the symbols that the namespace exports.  An empty symbol
    // block and exit 0 would leave every reference to them unresolved at link time.
    // void comptest_amc_GsymbolLoadFail(); // atfdb.comptest:amc.GsymbolLoadFail

    // Give amc a gsymbol in a single-file universe, and check that the amc.sideload
    // notice names the fallback root and the file that amc reads from it.
    // With no directory root, the symbol table side-loads from the default data/ root
    // of the current directory.  That root may belong to a different universe than
    // the -in_dir schema.
    // void comptest_amc_GsymbolSideload(); // atfdb.comptest:amc.GsymbolSideload

    // Give amc an Llist field without its dmmeta.llist record, and check that amc
    // reports amc.missing_record for the field and then stops at the reftype gate.
    // Later gen phases dereference the per-reftype records that this phase found
    // missing.
    // void comptest_amc_MissingLlist(); // atfdb.comptest:amc.MissingLlist

    // Give amc a ccmp on a global ctype, and check that amc reports amc.ccmp_global.
    // A global ctype has exactly one instance.  A global field's Get accessor takes no
    // parent argument, so the generated Cmp/Lt bodies would silently compare the
    // singleton with itself.
    // void comptest_amc_BadCcmpGlobal(); // atfdb.comptest:amc.BadCcmpGlobal

    // Give amc a chash on a global ctype, and check that amc reports amc.chash_global.
    // The rule is the one for ccmp.  The generated Hash body would silently hash the
    // singleton's own fields, whatever argument the caller passes.
    // void comptest_amc_BadChashGlobal(); // atfdb.comptest:amc.BadChashGlobal

    // Give amc an fcmp on a global ctype's field, and check that amc reports
    // amc.fcmp_global.
    // The rule is the one for ccmp, one level down.  Both operands of the generated
    // Cmp/Lt body would collapse to the singleton's value, and the function would
    // compare the field with itself.
    // void comptest_amc_BadFcmpGlobal(); // atfdb.comptest:amc.BadFcmpGlobal

    // Give amc a ctype with both a Tpool and a Global instance, hashed by an FDb Thash,
    // and check that amc reports amc.global_inst.
    // The pooled rows would reach field accessors that take no parent and read the
    // global.  The hash would then compare the singleton with itself.  So the Global
    // instance must be the ctype's only instance.
    // void comptest_amc_BadGlobalInst(); // atfdb.comptest:amc.BadGlobalInst

    // Give amc a minmax:Y ccmp on a non-builtin ctype without genop:Y order:Y, and
    // check that amc reports amc.ccmp_minmax.
    // The Min/Max functions compare with a raw <, and neither a generated nor a native
    // operator provides one.
    // void comptest_amc_BadMinmax(); // atfdb.comptest:amc.BadMinmax

    // Give amc a minmax:Y ccmp on a builtin and on an extrn:Y ctype, neither with
    // order:Y genop:Y, and check that amc generates the schema without a diagnostic.
    // The builtin has a native operator <, and the extrn:Y ctype has a user-defined
    // comparison.  Both provide the operator < that the Min/Max bodies compare with.
    // void comptest_amc_MinmaxNative(); // atfdb.comptest:amc.MinmaxNative

    // Give amc a ctype with one fdec field and a ctype with two, print each field's
    // GetScale, and check that the one-fdec ctype keeps the ctype-named GetScale and
    // the two-fdec ctype has none.
    // amc emits the ctype-named GetScale convenience for a ctype with a single fdec
    // field, because two fdec fields would give colliding definitions.
    // void comptest_amc_FdecGetScale(); // atfdb.comptest:amc.FdecGetScale

    // Query amc for records in namespaces that have no dmmeta.nscpp row, and check that
    // each such match reports amc.query_nocpp and counts toward the run's exit code:
    // 1. Query two ctypes at once, algo_lib.CppWidget in a namespace with the row and
    // ncns.Widget in a namespace without it.  Check that amc reports the ncns match,
    // still prints the algo_lib struct, and exits 1 for the one report.
    // 2. Query testdb.%, an ssimdb namespace projected to TypeScript alone, under the
    // default key, so both query walkers run.  Check that the two ctypes and the
    // seven generated funcs each draw a report and that the run exits 9.
    // 3. Repeat the testdb.% query with stderr discarded, and check that stdout is
    // empty.
    // 4. Query testdb.% and algo_lib.trace..Ctor together with stderr discarded, and
    // check that stdout holds the generated function and none of the nine reports
    // that the same run writes to stderr.
    // A namespace without the row emits no C++, so the query has no code to print for
    // its record.  Query mode's stdout carries the generated C++ alone, so a consumer
    // that wants code alone drops stderr.  The fourth query spans a nocpp namespace and
    // a code-generating one, the case that tells the two streams apart.
    // void comptest_amc_QueryNocpp(); // atfdb.comptest:amc.QueryNocpp

    // Give amc an fdec on two bitfld fields of a global ctype.  Print decfrac's
    // GetDouble and qSetDouble and decnarrow's SetDoubleMaybe and ReadStrptrMaybe, and
    // check that each body reaches the value through the field's accessors and bounds
    // it by the field's width.
    // The Get/Set accessors of a field with a global parent take no parent argument.
    // decnarrow is ten bits wide inside a u32 word, and pins the range that the
    // emitters bound it by.  A bitfld setter masks the store to the declared width.  So
    // the field holds at most 1023, below the 4294967295 that its arg would suggest.
    // SetDoubleMaybe compares against 1023, and the decimal reader carries that
    // magnitude split by ten, 102 and 3.
    // Each of the three stores casts the value to the field's own type on the way in,
    // so the setter receives exactly the type it declares.  The double that the double
    // setters compute is a different type.  So is the u64 magnitude that the reader
    // accumulates, and so is an arg that names a ctype standing for an integer builtin.
    // void comptest_amc_FdecGlobalBitfld(); // atfdb.comptest:amc.FdecGlobalBitfld

    // Give amc one fdec field per decimal-place count, arg and bitfld width that the
    // rule decides, and print algo_lib.DecPlaceCast.value.%.  Check that amc reports
    // each count, arg and conversion that it has no consistent arithmetic for, and
    // that the controls draw no diagnostic.
    // Zero places is the nplace default, which a forgotten attr leaves behind.  It
    // emits a reader that accumulates no digit.  A count of -1 draws the same report.
    // Eighteen places outrun the powers of ten that the double accessors substitute,
    // so they and the string accessors would disagree about the scale.  Thirty-one
    // places outrun the printer's scratch buffer.  Twelve places on an i32 field
    // outrun the field's own arg, which the emitted GetScale returns the scale as.
    // That diagnostic states the narrower range that the arg allows.  A u8 arg is the
    // narrowest range that the rule admits from the arg side.  Two places fit its 255,
    // three exceed it, and the diagnostic states that bound.
    // The fixture also holds one field per arg with no numeric range: bool, char,
    // u128, float, double, pad_byte and a plain struct ctype.  amc reports each as an
    // unusable arg.  The char field's count is zero as well, and amc still checks it
    // against the powers-of-ten limit, so that field draws both diagnostics in one run.
    // The remaining fields cover the other way to fix a field's range.  A Bitfld field
    // stores into a slice of another field's word, and its setter masks the store.  So
    // the width bounds the count, and the arg plays no part.  A ten-bit slice of a u32
    // takes three places and rejects four.  A forty-bit slice of a u64 takes twelve and
    // rejects thirteen.  A three-bit slice holds no decimal place, and the diagnostic
    // states exactly that.
    // amc refuses a slice of a signed arg narrower than its own type, because such a
    // field does not sign-extend on read.  The ten-bit i32 field draws the bitfield
    // diagnostic that refuses it.  The refusal follows the type that the arg stands
    // for, so a ten-bit slice of a ctype whose single Val field is an i32 draws it too.
    // A seven-bit slice of an i8 draws it at the narrowest signed arg there is.  A
    // slice whose arg has no numeric range takes no bound from its width.  amc reports
    // the four-bit char field as an unusable arg, and then holds its count against the
    // powers-of-ten limit alone.
    // The controls draw no diagnostic: the one-place and seventeen-place i64 fields
    // sit at the bottom and the top of the supported range; each of the eight integer
    // builtins carries a count its own range allows, which covers every width and both
    // signs; and a slice as wide as its arg takes what the arg takes at both signs.
    // u32 and i32 at thirty-two bits each take nine places; a u64 at sixty-four bits
    // takes seventeen; and an i8 at eight bits takes the two places its 127 allows.
    // An arg that stands for the builtin through another ctype stores that ctype.
    // Every quantity the emitted functions compute is a number of the builtin, so the
    // value crosses between the two at each end.  It leaves the wrapper through the
    // operator that dmmeta.fcast generates.  It enters the wrapper through the
    // fieldwise constructor that a dmmeta.cpptype row asking for a constructor
    // provides.  The fixture carries one arg per combination of the two.
    // DecPlaceCast carries both, and amc accepts it.  The query pins its emitted
    // functions, so the golden compares what an accepted wrapper compiles to.
    // DecPlaceFcast carries the operator alone, DecPlaceCtor the constructor alone,
    // and DecPlaceWrap neither.  amc reports each of the three, naming what is
    // missing.  DecPlaceChain stands for the builtin two ctypes deep, where the
    // constructor takes the inner ctype in place of the builtin.  amc reports it as
    // standing for the builtin only through another ctype.  The same rule reaches a
    // Bitfld arg.  The ten-bit slice of a wrapped u32 carries both conversions and
    // draws its count diagnostic alone.  The wrapped i32 slice carries neither, and
    // draws the conversion diagnostic beside its own.
    // void comptest_amc_BadFdecNplace(); // atfdb.comptest:amc.BadFdecNplace

    // Give amc a Bitfld field with a declared width of zero and a decimal count, and
    // check that amc reports the width as out of bounds and that the count draws no
    // diagnostic of its own.
    // The decimal range takes no bound from such a width.  A bound computed from it
    // would state that the field holds no decimal place.  The bitfield accessors
    // refuse the width once generation reaches the field, and the run ends there.
    // That is why the width error follows the bounds report.  It is also why a fixture
    // reaches the decimal rule with one bad width at most.  BadFdecBitwidthNeg covers
    // the negative width.
    // void comptest_amc_BadFdecBitwidth(); // atfdb.comptest:amc.BadFdecBitwidth

    // Give amc a Bitfld field with a declared width of -64 and a decimal count, and
    // check that amc reports the width as out of bounds and that the count draws no
    // diagnostic of its own.
    // This is the zero-width fixture's counterpart on the other side of the same rule.
    // The decimal range takes no bound from a negative width.  The magnitude is
    // sixty-four, because the test checks the rule through its output.  A bound built
    // from a negative width shifts by a negative count.  A compiler that takes the
    // count modulo sixty-four then computes a bound of zero from this width, and
    // reports the field as holding no decimal place.  A run that keeps the width out
    // of the shift prints two rows.  A run that lets it in prints three.
    // void comptest_amc_BadFdecBitwidthNeg(); // atfdb.comptest:amc.BadFdecBitwidthNeg

    // Give a global ctype a String cfmt with printfmt Tuple, print the emitted Print
    // body, and check that it reaches field values through the row argument, including
    // the cursor walk of an array field.
    // void comptest_amc_PrintGlobalTuple(); // atfdb.comptest:amc.PrintGlobalTuple

    // Give a global ctype a read:Y Tuple cfmt, and print its ReadFieldMaybe, its
    // ReadStrptrMaybe and the maxjobs field reader.  Check that the ReadFieldMaybe
    // dispatcher keeps the ctype-name prefix its callers link against and passes no
    // parent to the field readers, and that the Set-backed reader assigns through the
    // collapsed Set accessor.
    // Like every field-level function of a global, the field readers take no parent
    // argument.
    // void comptest_amc_ReadGlobalTuple(); // atfdb.comptest:amc.ReadGlobalTuple

    // Give amc an ArgvGnu cfmt, print its PrintArgv and ToArgv, and check that they
    // emit the gnu form of the matrix that ArgvGlobal pins for Argv: a one-character
    // field name takes a single dash and a longer name takes two; PrintArgv separates
    // the name from its value with a space where Argv writes a colon; ToArgv appends
    // the value as its own argv token; an array field repeats the name once per
    // element; and args[0] is the bare command name, with no bin/ prefix.
    // An ArgvGnu command is external and resolves on PATH.  atf_unit ExecToArgvSyntax
    // asserts that against the real bash command.
    // void comptest_amc_ArgvGnu(); // atfdb.comptest:amc.ArgvGnu

    // Give a global ctype and a regular one an Argv cfmt, print the PrintArgv and
    // ToArgv bodies of both, and check that the value of a field with its own Print
    // function (an fconst field) prints through that function.
    // That function's parent argument collapses for the global.  For the regular ctype
    // the argument is the row itself, with no const_cast.
    // void comptest_amc_ArgvGlobal(); // atfdb.comptest:amc.ArgvGlobal

    // Give amc command ctypes with fields of many shapes, and print the PrintArgv
    // bodies of FDb, Cmd and Flags and the ToArgv bodies of Cmd and Flags.  Check which
    // fields become command-line tokens and how each token reaches its value.
    // These fields become tokens: a plain Val; a Val cloned in from a Base; an fconst
    // Val and a Smallstr, through their own Print functions; a Tary, through a cursor
    // walk; and a big-endian Val and a bitfield, through their Get accessors.
    // The rest become no token: the Global anchor and the Base anchor, which name no
    // member of their own; the source word that a bitfield lives in, whose bits print
    // individually outside Raw form; and a Ptr, whose address has no command-line
    // spelling that ReadArgv could take back.
    // void comptest_amc_ArgvField(); // atfdb.comptest:amc.ArgvField

    // Give fconsts to two fields of a global ctype, print state's ToCstr, Print and
    // SetStrptrMaybe and the bitfld mode's GetEnum, and check that the calls between
    // the fconst accessors pass no parent.
    // The fconst accessors of a global take no parent argument.  The golden pins
    // ToCstr's GetEnum switch, Print's ToCstr call, SetStrptrMaybe's SetEnum calls for
    // short names and for names beyond 8 bytes, and, through the bitfld member,
    // GetEnum's Get call.
    // void comptest_amc_FconstGlobal(); // atfdb.comptest:amc.FconstGlobal

    // Pipe a universe to amc on stdin (-in_dir:-), query a function that only the piped
    // rows define, and check that amc prints it.
    // A caller composes a universe this way, as an acr_in dump plus extra rows, without
    // a temp file.  "-" names no directory, so side table data would load from the
    // default root.  This universe has no side table to load, so the run prints no
    // amc.sideload notice.
    // void comptest_amc_StdinUniverse(); // atfdb.comptest:amc.StdinUniverse

    // Give a global ctype an unsigned and a signed numstr field, print each field's
    // Geti64 and GetnumDflt, and check that they call Getnum bare.
    // The numstr accessors of a global take no parent argument.  The golden pins both
    // Geti64 arms, the overflow-checked arm of the u64 numtype and the plain arm, and
    // GetnumDflt's body.
    // void comptest_amc_NumstrGlobal(); // atfdb.comptest:amc.NumstrGlobal

    // Give a global ctype a pmask, print one member's accessors and the filtered Print,
    // and check that the member accessors and the print filter call the pmask's bitset
    // accessors bare.
    // The pmask's bitset accessors take no parent argument.  The golden pins the
    // member's PresentQ, SetPresent and Set bodies and the filtered Print.
    // void comptest_amc_PmaskGlobal(); // atfdb.comptest:amc.PmaskGlobal

    // Give a kafka codec a Bitfld field, print its KafkaEncode and KafkaDecode, and
    // check that encode reads the field through the Get accessor and decode stores a
    // temporary through the Set accessor.
    // The field has no direct member, so raw parent.<name> access fails to compile.
    // Encode takes the Get accessor from FieldvalExpr.  Decode picks the Set accessor
    // by NoDirectMemberQ, which is deliberately narrower than AssignViaSetQ.  This
    // codec's presence bits follow the wire's null flag, and a pmask Set would mark a
    // null field present.
    // void comptest_amc_KafkaBitfld(); // atfdb.comptest:amc.KafkaBitfld

    // Give a kafka nullable array to a ctype whose pmask field is named field_mask, and
    // print KafkaEncode, KafkaDecode and the presence accessors.  Check that the
    // encoder's presence checks use the same derived name as the decoder's set side.
    // The presence accessors derive their names from the pmask field, here
    // val_FieldMaskQ and val_SetFieldMask.  The golden pins the KafkaEncode presence
    // checks and the KafkaDecode present-mark.
    // void comptest_amc_KafkaPmaskName(); // atfdb.comptest:amc.KafkaPmaskName

    // Give an optional fpbuf field to a ctype whose pmask field is named field_mask,
    // and print PbufEncode, PbufDecode, the field's Set and the presence accessors.
    // Check that both sides of the protobuf codec spell the name that the pmask field
    // derives.
    // The derived-name rule is the one of KafkaPmaskName.  The encoder's presence guard
    // reads val_FieldMaskQ.  The decoder stores the plain Val field into its own member
    // and marks presence with val_SetFieldMask, the same update that the field's own
    // Set carries.
    // void comptest_amc_PbufPmaskName(); // atfdb.comptest:amc.PbufPmaskName

    // Give a bitfld an fpbuf on a ctype with a pmask, print PbufEncode, PbufDecode and
    // the field's Set, and check that the encoder reads the field through the Get
    // accessor and the decoder stores a temporary through the Set accessor.
    // The field's value lives inside its source field, with no member of its own.  The
    // temporary starts empty, because the varint decoder replaces whatever it
    // receives.  That Set writes the slice of the source word and leaves the pmask
    // alone, so the decoder marks presence itself.
    // void comptest_amc_PbufBitfld(); // atfdb.comptest:amc.PbufBitfld

    // Give a bitfld an fpbuf on a proto3 ctype with no pmask, print PbufEncode,
    // PbufDecode and the field's Set, and check that the encode guard reads the field
    // through its Get accessor.
    // Presence is implicit, so the encode guard is the pbtype's default-value check.
    // A bitfield has no member of its own to compare against zero.  The golden also
    // pins the Set body, which stores straight into the source word because no
    // presence bit follows.
    // void comptest_amc_PbufBitfldNondflt(); // atfdb.comptest:amc.PbufBitfldNondflt

    // Give amc a repeated fpbuf field that is a pmask member, print PbufDecode and the
    // filtered Print, and check that the decoder calls SetPresent in both the packed
    // and the one-element-per-tag decode branch.
    // The print filter consults the same bit.  A decoder that left the bit clear when
    // elements arrive would make the filter drop the decoded elements.
    // void comptest_amc_PbufRepeatedPmask(); // atfdb.comptest:amc.PbufRepeatedPmask

    // Give amc one message with each singular decode store shape and one with the
    // repeated controls, print both PbufDecode bodies, and check the store path that
    // each field takes.
    // A singular field decodes into its own member.  Under pb_type message that member
    // already holds the earlier occurrence, so the nested decoder merges into it
    // (pbt.Msg.bare and .sub).  A pmask member decodes in place too, and the decoder
    // marks it present afterwards (.opt, .sub).  Its own Set would mark it as well.
    // Storing through that Set would copy the value out and back on every occurrence
    // of the field number.
    // The shapes with no member to decode into pay for a temporary: .slice through the
    // bitfld accessors, and .netval through the byte-swapping pair.  So does .cond,
    // whose Set moves the record between fcond lists.  Of the eight, only .computed
    // seeds its temporary from the field's current value, because it alone is both
    // accessor-routed and a message.  Get is then the merge's only way to see what the
    // earlier occurrence set.  pbt.Rep appends through Alloc in every branch.
    // void comptest_amc_PbufStore(); // atfdb.comptest:amc.PbufStore

    // Give a global ctype an Opt field, print the field's Print, and check that amc
    // rejects the field as amc.infinity_pool and that the Print body emitted beside
    // the error holds no bare (void); statement for the absent parent argument.
    // void comptest_amc_OptGlobalPrint(); // atfdb.comptest:amc.OptGlobalPrint

    // Give amc a base-1 numstr and a base-37 one, and check that amc reports
    // amc.bad_numstr for both fields, naming each.
    // Neither base has a digit encoding.  Unary needs one digit per unit of value, and
    // bases 37..94 have no character map.
    // void comptest_amc_BadNumstrBase(); // atfdb.comptest:amc.BadNumstrBase

    // Give amc three numstr fields whose min_len breaks a width contract, and check
    // that amc reports amc.bad_numstr for all three.
    // LnumStr9_U32's min_len is wider than the string itself.  LspaceStr100_U64's is
    // wider than SetnumMaybe's 64-character pad budget.  The signed LspaceStr6_I32's
    // equals the string length, which leaves the sign no slot, so every negative value
    // would fail at run time.  amc has no way to honor any of these width contracts.
    // void comptest_amc_BadNumstrMinlen(); // atfdb.comptest:amc.BadNumstrMinlen

    // Give amc numstr fields over numtypes that stand for no integer bltin, over names
    // that disagree with their numtype, and over wrappers with each combination of
    // conversions, and print LnumStr6_U32's Getnum and Geti64.  Check that amc reports
    // each bad numtype, each lying name and each missing conversion, and that the
    // controls draw no diagnostic.
    // The fixture holds one field per numtype that stands for no integer bltin: double
    // and float carry no integer range; u128 is wider than the range amc represents;
    // pad_byte is no number; and a plain struct ctype stands for no bltin.  Each falls
    // back on the widest signed bounds, which admit every value and make the sign
    // tests fire even for an unsigned numtype.  A range comparison built from those
    // bounds would never reject.  One fdec field trips the same check from the other
    // side.
    // The next fields exercise the agreement between a ctype name and its numtype.  amc
    // enforces it so that a name never lies about the range its string holds.
    // LnumStr6_I32 says I32 over a u32 numtype, and LnumStr9_U64 says U64 over the
    // same one.  amc rejects a name that lies about the sign and one that lies about
    // the width alike.  LnumStr4_I16 says I16 over algo_lib.U32Cast, a ctype whose
    // single Val field holds a u32.  amc judges the numtype by the u32 it stands for,
    // so it catches the lie through the wrapper.  The controls draw no diagnostic from
    // the same run: LnumStr7_U32 states a truthful token over a bare u32; LnumStr6_U32
    // states one over the wrapper; LnumStrPlain carries no token to judge; and
    // LnumStr7_I8 carries a false one under strict:N, which asks for no check.
    // A numtype that stands for the builtin through another ctype moves a value both
    // ways.  Getnum constructs the numtype from the digits it parsed, and Geti64 reads
    // that result as an i64.  The constructor comes from a dmmeta.cpptype row asking
    // for one.  The read comes from the operator that dmmeta.fcast generates.  The
    // fixture carries one numtype per combination of the two.  LnumStr6_U32 names a
    // wrapper with both, and amc accepts it.  The query pins its Getnum and Geti64, the
    // two statements that the conversions have to satisfy.  LnumStr8_U32 names a
    // wrapper with the operator alone, LnumStr10_U32 one with the constructor alone,
    // and LnumStr5_U32 one with neither.  amc reports each, naming what is missing.
    // LnumStr11_U32 names a wrapper two ctypes above the builtin, whose constructor
    // takes the inner wrapper in place of the builtin.  amc reports it as standing for
    // the builtin only through another ctype.  The bare builtins are the accepted edge
    // of the same rule.  All eight are present with a truthful name token, so the test
    // covers both signs and every width.
    // void comptest_amc_BadNumstrNumtype(); // atfdb.comptest:amc.BadNumstrNumtype

    // Give amc numstr and smallstr fields whose pads cover the digits of each base, the
    // sign character and the NUL pad, and check that amc rejects each pad that reads
    // back as part of a number and passes each control.
    // A left-pad that is a nonzero digit of the base strips leading value digits on
    // read: '!' is digit 1 in base 95; 'c' is a digit in base 36; and '5' is a digit
    // in base 10.  A right-pad that is any digit of the base divides the stored value
    // by a power of the base: NUL in base 256, ' ' in base 95, 'A' in base 36 and '7'
    // in base 10.  The pad is a C++ character literal, so amc judges an escaped
    // spelling by the byte it denotes.  A base-16 right-pad '\x30' is the digit zero
    // and draws a rejection.  A base-95 left-pad '\0' is no digit of that base and
    // passes.  Only the zero digit is a safe left-pad, and no digit is a safe
    // right-pad, so amc rejects eight of these nine numstr fields.
    // Four more fields carry no numstr row and meet the plain smallstr right-pad check
    // (amc.invalidpad).  That check refuses any pad character that a reader could take
    // for part of a number, and it too reads an escaped pad by its denoted byte.  It
    // refuses the digit '3' and the escaped digit '\x30'.  The escaped 'A' and the
    // escaped NUL belong to no number and pass.
    // The sign character is the other pad that a signed numstr is unable to read back.
    // The reader strips a left pad from the end that holds the sign.  So a signed
    // base-10 field padded with '-' loses the sign of every negative value, and amc
    // rejects it.  Its three neighbors are controls that draw no diagnostic: the same
    // '-' pad on an unsigned field, which writes no sign; the same '-' pad on the
    // right, stripped from the end away from the sign; and '+' on the left of a signed
    // field, a character the reader never takes for a sign.  A signed field
    // left-padded with the zero digit is the fourth control, because leading zeros
    // carry no value and strip safely.
    // A signed numstr may not use the NUL pad either, and that case shows again that
    // amc judges a pad by the byte it denotes.  amc rejects all three spellings of
    // that one pad on a signed field: the bare 0, the empty pad value that amc fills
    // in as 0, and the escaped '\0'.  It rejects them on the left and on the right
    // alike.  The same empty pad on an unsigned field passes at either end.  The two
    // rpascal fields are the controls for the strtype dimension.  Such a string
    // carries its own length and strips no pad, so neither the NUL pad on a signed
    // field nor a digit pad draws a diagnostic.
    // void comptest_amc_BadNumstrPad(); // atfdb.comptest:amc.BadNumstrPad

    // Give amc rpascal strings of 255, 256 and 300 characters and a 300-character
    // right-padded string, and check that amc reports smallstr.toobig for the two long
    // rpascal strings and exits with 2.
    // An rpascal string stores its length in one byte, so 255 is the longest one whose
    // length that byte can hold.  Each diagnostic raises the error count, so a later
    // one never discards what an earlier one reported.  The controls draw no
    // diagnostic.  A 255-character rpascal string is the longest accepted one.  A
    // 300-character right-padded string carries no length byte, so the bound belongs
    // to the rpascal strtype alone.
    // void comptest_amc_BadSmallstrToobig(); // atfdb.comptest:amc.BadSmallstrToobig

    // Give amc a signed base-95 numstr and a signed base-256 one, and check that amc
    // rejects both fields with amc.bad_numstr.
    // '-' is a digit in both bases, so a stored digit string looks the same as a
    // signed one.  Both fields left-pad with ' ', which is digit 0 in base 95 and
    // digit 32 in base 256.  So the base-256 field draws a second rejection from the
    // pad check, because a left-pad that is a nonzero digit of the base strips leading
    // value digits on read.
    // void comptest_amc_BadNumstrSignedBase(); // atfdb.comptest:amc.BadNumstrSignedBase

    // Give one cpbuf ctype every illegal field number, and check that amc reports
    // amc.pbuf_field_number for each in one run.
    // A duplicate within the ctype gives two identical decoder case labels, which fail
    // to compile.  Number 0 is wire tag 0, which the protobuf format reserves as
    // illegal.  2^29 overflows the (number<<3)|wire_type tag and aliases another
    // field.  Protobuf reserves 19000..19999 for its own implementation, so amc
    // rejects both ends of that range and accepts 18999 and 20000 beside it.
    // void comptest_amc_BadPbufFieldNumber(); // atfdb.comptest:amc.BadPbufFieldNumber

    // Give one cpbuf ctype every arg and reftype shape that the pbuf codec fails to
    // compile against, and check that amc rejects each in one run.
    // The lib_pb codec accepts only the pbtype's argtype, so pb_type string on a u32
    // fails.  pb_type message needs a cpbuf arg.  An Inlary field has no wire mapping.
    // pb_type enum needs an arg with the fconst-carrying i32 value shape.  One enum
    // arg has no value field, and the other's value field is u8.  The lib_pb varint
    // codec binds i32.
    // void comptest_amc_BadPbufArg(); // atfdb.comptest:amc.BadPbufArg

    // Give amc three fpbuf rows that ask for packed encoding, and check that amc
    // reports amc.pbuf_packed for the two where the protobuf format has none.
    // One of the two is on a singular field, and the other is on a repeated field of a
    // length-delimited pb_type.  The emitter reads packed only in the repeated
    // non-length-delimited branch.  It would silently ignore both rows, and the author
    // would get the ordinary encoding.  The third row, packed on a repeated varint
    // field, is the legal shape and draws no report.
    // void comptest_amc_BadPbufPacked(); // atfdb.comptest:amc.BadPbufPacked

    // Give amc two fpbuf rows that would generate no wire code and a cpbuf ctype that
    // inherits a base, and check that amc reports each of the three.
    // One fpbuf row is on a field of a ctype with no cpbuf row, and amc generates no
    // codec without one (amc.pbuf_ctype).  The other is on a Base field, and emission
    // never writes a base's fields to the wire (amc.pbuf_reftype).  pbt.Msg2 is a
    // cpbuf ctype with a Base field, so the base's fields would be absent from its
    // message (amc.pbuf_base).  Each report keeps a row from sitting silently dead.
    // void comptest_amc_BadPbufNoCodec(); // atfdb.comptest:amc.BadPbufNoCodec

    // Give amc four cpbuf ctypes, two with a valid syntax, one misspelled and one empty,
    // and print their PbufEncode.  Check that amc reports amc.pbuf_syntax for the
    // misspelling and the empty value, and that the two controls show what each
    // accepted syntax emits.
    // cpbuf.syntax names a row of dmmeta.pbsyntax, the table that says whether the
    // syntax has implicit presence.  acr -check catches the spelling when the row goes
    // through the database.  amc also reads a schema out of -in_dir, so it rejects a
    // value with no row itself.  proto2 writes the scalar unconditionally, and proto3
    // guards it with the default-value test.
    // void comptest_amc_BadPbufSyntax(); // atfdb.comptest:amc.BadPbufSyntax

    // Give amc four ctypes that each declare a oneof, and check that amc reports
    // amc.pbuf_oneof for the three whose variants share no single pmask.
    // A oneof is a union.  The encoder writes at most one variant only because a store
    // into one variant clears the presence bits of its siblings on the same pmask.
    // Three shapes break that.  NoMask's variants have no pmask, so the proto3
    // default-value test guards each, and every nonzero variant goes on the wire.
    // Split's variants sit on two pmasks, and a store clears siblings only on the mask
    // it writes.  Half's pmask has an explicit member list that covers one variant and
    // leaves out the other.  The first ctype, pbt.Msg, is the control, with both
    // variants on one pmask, and it draws no diagnostic.
    // void comptest_amc_BadPbufOneof(); // atfdb.comptest:amc.BadPbufOneof

    // Give one cpbuf ctype each singular store shape and another the repeated
    // controls, and check that amc reports amc.pbuf_store for the three shapes with a
    // Get and no Set.
    // The decoder of a singular field with no member of its own stores through the
    // field's generated Set.  A shape that has a Get and no Set would emit a call to a
    // function amc never writes.  The three rejected rows are a cppfunc with set:N, a
    // substr and a falias.  The shapes that have a Set stand beside them as controls
    // that draw no diagnostic: an fbigend field, a bitfld, a cppfunc with set:Y, and a
    // plain Val field stored in place.  The second ctype carries the repeated
    // controls.  There the decoder appends through Alloc, and a fldfunc element type
    // needs no Set.
    // void comptest_amc_BadPbufStore(); // atfdb.comptest:amc.BadPbufStore

    // Give an fpbuf field a pb_type with no amcdb.pbtype row, and check that amc
    // reports amc.pbuf_type.
    // The table supplies the wire type and the codec binding.  Without them the
    // encoder would write the field tag with no payload, and the decoder case would
    // consume the tag and leave the value unread.
    // void comptest_amc_BadPbufType(); // atfdb.comptest:amc.BadPbufType

    // Give a u8 pmask nine member fields, and check that amc reports amc.pmask_width.
    // Nine presence bits overflow the pmask field's eight bits.
    // void comptest_amc_BadPmaskWidth(); // atfdb.comptest:amc.BadPmaskWidth

    // Give amc explicit pmaskfld_member rows, one per field kind that the membership
    // rule excludes, and check that amc reports amc.pmask_member for each row by name.
    // The excluded fields are a field of another ctype, the Global anchor, the pmask
    // word itself, a length field, a type field and a Base field.  None of them has a
    // presence bit that the pmask's parent struct could back.  Auto-populated members
    // satisfy the rule by construction.  An explicit row must pass the same
    // validation, and one row per exclusion cause keeps every arm of the rule
    // load-bearing.  A type field and a Base field never sit on one ctype.  The base
    // declares the type field, and the deriving ctype declares the Base field as its
    // first field.  So Hdr and Msg carry those two, each with its own pmask.
    // The width check counts a rejected row's bit like any other, because the
    // membership rule reports a row and leaves it on the pmask.  This fixture declares
    // the pmask a u32 and puts five member rows on it.  The count then stays far inside
    // the width, and the golden carries the membership diagnostics alone.
    // void comptest_amc_BadPmaskMember(); // atfdb.comptest:amc.BadPmaskMember

    // Give amc two ctypes that contain each other by value, one that contains itself,
    // and two bitfields over those cycles.  Check that amc reports each decimal
    // field's arg as amc.bad_numtype and ends the run at the circular definition.
    // No finite layout exists for such a cycle, so no struct in it has a size.
    // Each cycle also carries a decimal field, which asks for the arg's numeric range
    // before the size computation runs.  amc resolves the range by walking the chain of
    // ctypes whose single Val field names the next one.  A cycle would send that walk
    // around forever.  So the fixture holds the two shapes a cycle can take, a ctype
    // naming itself and a pair naming each other.  amc reports each decimal field as
    // an arg with no numeric range, then reaches the size computation and ends at the
    // circular definition.
    // A bitfield asks the same walk two further questions, and the fixture puts a
    // cycle in front of each.  One is the type that its accessors hand back, which the
    // bitfield's own arg names.  The other is the type that its mask operations run
    // on, which its source field's arg names.  Neither question turns the cycle into
    // an internal error.  The run still ends at the circular definition.
    // void comptest_amc_BadSizeCycle(); // atfdb.comptest:amc.BadSizeCycle

    // Give amc an Inlary of 65537 elements, each a 65536-byte fixed Inlary, and check
    // that amc reports amc.size_overflow.
    // The total byte size exceeds the i32 range that amc computes sizes in.
    // void comptest_amc_BadSizeOverflow(); // atfdb.comptest:amc.BadSizeOverflow

    // Give amc one fixed field of each shape after a Varlen field, and check that amc
    // reports amc.varlen_last for each field that declares a member of its own,
    // attributes it to the varlen field it follows, and exits nonzero.
    // The varlen data begins at the end of the struct.  Writes through the varlen
    // accessors would clobber a trailing fixed member.
    //
    // Only a field that declares a member of its own has storage to clobber, so the
    // golden is an accept/reject table.  amc reports a Val and a Ptr, which declare
    // members directly, and a ZSListMT, whose read-side and write-side list heads are
    // two more members of the parent.  amc passes over the rest: a Bitfld, which
    // re-slices the word of a field declared before the varlen data; the three
    // fldfunc kinds, a cppfunc, a falias and a substr, whose value amc computes and
    // never stores; Malloc, Global and Cppstack, whose generators put no member in the
    // struct; and an Opt, which draws a comment line where its optional trailing
    // element would sit.  Count and Exec are the intermediate case.  The declared
    // field draws no report, and amc reports the Val children it generates from it in
    // their own right.  That is why cnt_n and the child_* rows appear, and cnt and
    // child do not.
    //
    // The Opt is the one field here that a second rule answers as well.  It claims the
    // end of the fixed portion that the varlen data already claims, so the golden
    // carries its amc.trail_overlay row.  The absence of an amc.varlen_last row beside
    // it pins the Opt as declaring no storage.
    // void comptest_amc_BadVarlenLast(); // atfdb.comptest:amc.BadVarlenLast

    // Give amc ctypes that pair Varlen and Opt fields in each way a ctype can declare
    // them, and check that amc reports amc.trail_overlay once per rejected ctype and
    // exits nonzero.
    // Varlen data and an optional trailing element both begin at the end of the fixed
    // portion, and the generated accessors would overlay one on the other.  Only
    // varlen fields carry the end offset that lets one begin where another ends.  So
    // a ctype may claim that address once, and amc rejects each further claim.
    // amc rejects Varlen with Opt in both declaration orders, one ctype each.  It also
    // rejects a ctype with two Opt fields, whose getters would return the same address
    // twice.  A fourth ctype declares an Opt ahead of two varlen fields and draws one
    // row.  The first varlen field begins at the disputed address, and the second
    // begins where the first ends.  So amc reports a single overlay once, however many
    // varlen fields trail it.
    // Each rejected ctype draws exactly one diagnostic.  The order of a Varlen and an
    // Opt matters to the separate rule that fixed fields may not follow a varlen
    // field.  Reporting the Opt there would state the same overlay twice.  The
    // controls draw no diagnostic: a ctype with one Opt; a ctype with one Varlen; a
    // ctype with two Varlen fields, the accepting arm of the rule, where the second
    // begins where the first ends; and a ctype with neither.  The two-Varlen control
    // carries no pool instance.  The pool generator refuses a pool arg with more than
    // one varlen field, and its diagnostic would arrive beside the ones this test is
    // about.
    // void comptest_amc_BadVarlenOpt(); // atfdb.comptest:amc.BadVarlenOpt

    // Give a varlen ctype a non-varlen pool instance, and check that amc reports the
    // instance as amc.infinity_pool and exits nonzero.
    // The pool allocates fixed-size records, so the trailing varlen data has nowhere
    // to live.
    // void comptest_amc_BadInfinityPool(); // atfdb.comptest:amc.BadInfinityPool

    // Give amc a pool arg with a varlen field and no lenfld, print the pool's Delete,
    // and check that its length statement is well-formed, with no stray semicolon
    // inside the length expression.
    // The element count is an extern user function, and Delete computes the total
    // length from it.
    // void comptest_amc_PoolVarlenExtern(); // atfdb.comptest:amc.PoolVarlenExtern

    // Give amc a pooled ctype whose fixed size plus lenfld extra is no multiple of the
    // lenfld scale, and check that amc reports amc.lenfld_scale.
    // Pool alloc stores len = (size + extra) / scale, which truncates.  The reader then
    // reconstructs less than the pool allocated.
    // void comptest_amc_BadLenfldScale(); // atfdb.comptest:amc.BadLenfldScale

    // Give amc a pool-less message ctype whose fixed size is no multiple of the lenfld
    // scale, and check that amc reports amc.lenfld_scale.
    // The message constructor and the field default store the same truncating length
    // word that pool alloc would.  So the divisibility check must cover message ctypes
    // as well as pooled ones.
    // void comptest_amc_BadLenfldMsgtype(); // atfdb.comptest:amc.BadLenfldMsgtype

    // Give amc a fixed-only ctype whose only length store is its Fmt* constructor (a
    // pnew row, no msgtype, no pool instance), and check that amc reports
    // amc.lenfld_scale.
    // The constructor's run-time total is always the fixed size.  So an indivisible
    // size + extra stores the same truncating length word that pool alloc would, and
    // the divisibility check must cover pnew carriers too.
    // void comptest_amc_BadLenfldPnew(); // atfdb.comptest:amc.BadLenfldPnew

    // Give amc three lenflds with scale:0 on ctypes whose Opt fields consume the length
    // expression, and check that amc reports amc.lenfld_scale for every offender in
    // one run.
    // The reports go through the accumulate path, so the run continues past the first
    // length-expression consumer, where a throw would stop it.  MsgC adds the pooled
    // shape with a u64 length word, the one that reaches the pool InsertMaybe raw-word
    // bound.  That bound divides by the scale, and amc emits it a whole gen step before
    // gen_check_lenfld reports the scale.  Without its own scale test the run would
    // die on SIGFPE with no diagnostic.  The cloned base field (FMsgC.len) reports the
    // row a second time.
    // void comptest_amc_BadLenfldZeroScale(); // atfdb.comptest:amc.BadLenfldZeroScale

    // Give amc four message ctypes whose length field is u128, bool, char and u16, and
    // check that amc reports amc.lenfld_type for the first three in one run.
    // A length word needs a numeric range.  GetMinMax resolves one for the fixed-width
    // integers u8 through i64 alone.  Every range check built from the widest-signed
    // fallback runs vacuously, and the TS encoder omits its range guard outright.  So
    // amc refuses u128, which has no arithmetic amc can do; bool, which holds no
    // magnitude; and char, whose signedness is the compiler's choice.  MsgOk carries
    // the same shape over a u16 word and draws no report.  That control shows that amc
    // rejects the other three for their type and accepts the shape.
    // void comptest_amc_BadLenfldType(); // atfdb.comptest:amc.BadLenfldType

    // Give a pooled pnew ctype a u8 lenfld with scale 4, and a second ctype a u32
    // lenfld with scale 1.  Print the pool's AllocExtraMaybe and both Fmt*
    // constructors, and check that every store of a run-time total guards the length
    // field's storable range as well as scale divisibility.
    // A total beyond the range would wrap mod 2^N through the word's own type, and
    // frame a shorter message than the writer wrote.  The pool's AllocExtraMaybe
    // bounds the caller's addon by the range minus the fixed portion, as a
    // compile-time ssizeof expression.  The Fmt* constructor refuses the total with
    // NULL, as the TS encoder's RangeError guard does.
    // MsgW is the same shape with a wide length field (u32, scale 1).  Its range
    // covers every frame length, so amc emits no lenfld test, and the frame length
    // domain itself remains.  Its buffer is an Alloc, whose size argument is an int.
    // So the emitted bound is the i32 maximum, and the totals it accepts are exactly
    // those that a caller can ask the allocator for.
    // void comptest_amc_LenfldNarrow(); // atfdb.comptest:amc.LenfldNarrow

    // Generate TypeScript for two ctypes that share one u32 lenfld shape and differ in
    // whether the frame length is a compile-time constant, and check that only the
    // variable one's encoder carries a range guard:
    // 1. Make the output directories under the test's tempdir.
    // 2. Run amc from the checkout root with -out_dir under the tempdir, so the
    // derived-table acr child writes into that tempdir too.
    // 3. Print both emitted TypeScript encoders from ts/gen/algo_lib_gen.ts.
    // MsgFix is fixed-size, so the emitted `length` is a literal, and gen_check_lenfld
    // has already proven it storable.  Its encoder carries no range guard.  MsgVar
    // owns a varlen tail, so its length is a run-time sum and the guard stays.
    // void comptest_amc_JsFixedFrame(); // atfdb.comptest:amc.JsFixedFrame

    // Give amc one varlen pool with InsertMaybe per cell of the addon-count rule,
    // print each pool's InsertMaybe and AllocExtraMaybe, and check that InsertMaybe
    // computes the addon count of an untrusted source struct in the i64 domain and
    // bounds it.
    // A multiply in the length word's own type would wrap a huge corrupt stored value
    // mod 2^N into a small positive count.  That count would pass the negative-addon
    // guard and read past the source message.  The widened arithmetic keeps the true
    // total visible, and totals outside [fixed size, i32 max] return NULL.  The
    // widening itself stays exact only while the word's range fits u32.  Beyond that
    // the i64 expansion can overflow, with scale > 1 in the multiply, and even at
    // scale 1 in the extra adjustment after the wrap.  The overflow is undefined
    // behavior, which an optimizer may exploit to elide the range test.  So amc first
    // bounds a wider word by the largest value whose expanded total fits the i32
    // frame-length domain, a generation-time constant.
    //
    // The fixture is the rule's table, one pool ctype per cell.  The arm that applies
    // turns on the word's largest storable value, so the accepted edges sit either
    // side of u32's max.  MsgU32 (u32, scale 4), MsgI32 (i32) and MsgBit32 (a u64 word
    // masked to 32 bits by its Bitfld width) draw no bound.  MsgBit33 (masked to 33
    // bits), MsgU64 (u64, scale 2) and MsgI64 (i64, scale 8) draw one.  A signed word
    // is the cell where the bound, through the u64 conversion in the emitted test,
    // rejects a negative stored length ahead of the addon test.  The bound's constant
    // follows scale and extra.  MsgNeg carries extra -2 and MsgPos extra 4, and the
    // reader formula shifts the constant for each.
    //
    // InsertMaybe reaches the allocation through the die-on-fail AllocExtra.  So a
    // count that AllocExtraMaybe would refuse kills the process, where AllocExtraMaybe
    // would return NULL.  The two functions accept exactly the same counts.  The test
    // therefore prints both for every pool, and a term emitted into one and missing
    // from the other shows up as a diff.  Two cells of the table make that agreement
    // worth pinning.  The bound's constant comes from the row type, and msg6 and msg5
    // show it as block pools whose row carries a buffer member that the base lacks.  A
    // word that stores totals past i32 max leaves the length field with no bound to
    // give.  So every such pool bounds the count by the i32 that a frame length is
    // read back through.
    // void comptest_amc_PoolInsertScale(); // atfdb.comptest:amc.PoolInsertScale

    // Give amc two Bitfld lenflds whose stores mask to the declared width, and check
    // that amc reports amc.lenfld_extra for both in one run, bounded by the
    // width-limited max.
    // MsgWidth (width 4, u32 source word, msgtype store, fixed size 20) stores at most
    // 15, so the minimum frame's length word is unstorable.  MsgExtra (width 6,
    // extra:252) stores at most 63, so no frame length is representable.  A check that
    // bounded by the source word's full range would pass both.
    // void comptest_amc_BadLenfldBitfld(); // atfdb.comptest:amc.BadLenfldBitfld

    // Give amc a u8 lenfld with extra:256, and check that amc refuses the schema at
    // generation time with amc.lenfld_extra.
    // Every value the type can hold maps to a negative frame length, so no frame is
    // representable.  Left unrejected, the TS encoder's range-guard bound
    // maxval*scale - extra wraps in u64, and the emitted guard never fires.
    // void comptest_amc_BadLenfldExtra(); // atfdb.comptest:amc.BadLenfldExtra

    // Give amc two packed TypeScript ctypes whose fixed size plus lenfld extra falls
    // outside the length field's range, with no msgtype and no C++ alloc instance, and
    // check that amc rejects both in one run.
    // The generated TypeScript Encode is their only store site.  MsgLow (u32 lenfld,
    // extra:-8, fixed size 4) would wrap the minimal frame's stored length through
    // setUint32.  MsgHigh (u8 lenfld, extra:255, fixed size 4) would get a range guard
    // that throws on every encode.
    // void comptest_amc_BadLenfldTs(); // atfdb.comptest:amc.BadLenfldTs

    // Give amc packed TypeScript ctypes with fields of each wire form, run it with
    // -out_dir:$tempdir, and check that amc names in one run every field that the
    // TypeScript codec is unable to encode or decode, before it writes any file.
    // MsgBad carries three such fields, which shows that the walk goes on past the
    // first: an inlary of char and an inlary of a cstr ctype, both of which the class
    // projects as `string` with no byte-copy form; and a Smallstr, a storage reftype
    // that the codec maps nowhere.  MsgBadOpt adds the fourth, an Opt of the same cstr
    // ctype, which reads the same byte-copy rule on the other reftype that uses it.
    // MsgBadInlary adds the fifth, an inlary of u8 with min:0 max:8.  Its class
    // projects as a byte array.  A variable inline array, though, stores a live
    // element count after its elements.  So its width differs from max elements, and
    // the count has no member to carry it.  MsgBadVal adds the sixth, a Val of a ctype
    // that declares a Varlen.  The nested codec appends a tail past the width that the
    // outer walk steps over.  MsgBadValOpt adds the seventh, the same rule read on a
    // nested Opt tail.  MsgBadVarlen adds the eighth, a Varlen of the cstr ctype.  Its
    // member projects as a string, while its tail is a run of records.  MsgTagged
    // closes the list with the ninth and tenth, two inline arrays of char on a ctype
    // whose only other field is a length word.
    //
    // MsgNolen and MsgNolenOpt read the other dimension, the ctype's own.  MsgNolen
    // declares two Varlen fields and MsgNolenOpt an Opt, each with no length field to
    // frame the tail.  Each draws amc.ts_lenfld.  MsgFixed is that rule's control, a
    // packed TypeScript ctype with no tail and no length field, and it draws no
    // report.
    //
    // The rest of the fixture is the accepted side of that table and draws no report.
    // MsgOk carries a Base, a lenfld, a plain Val, a Bitfld, a Val of the cstr ctype (a
    // fixed-length string does have a wire form), a Val of a tail-free ctype, an
    // inlary of u8, and the three accepted Varlen args: char for the text tail, and u8
    // and a plain ctype for the byte tail.  MsgOpt carries an Opt whose arg maps to a
    // byte array.
    // The two remaining ctypes fix the check's scope.  MsgUnpacked and MsgNojs both
    // carry the offending inlary of char, and neither draws a wire refusal.
    // MsgUnpacked has no pack row, so amc projects it as a type alone, where the
    // inlary of char is a string.  MsgNojs has no ctypelang row, and amc projects it
    // nowhere.
    //
    // $tempdir holds none of the gen directories that amc writes to.  So a generated
    // file would report amc.outfile_write, naming the path that amc failed to create.
    // The absence of that line is the assertion this run makes about ordering.
    // check_ts sits ahead of every emitting gen.  So amc refuses the schema before the
    // first write, and emits no decoder for the fields it accepted either.
    // void comptest_amc_BadTsWire(); // atfdb.comptest:amc.BadTsWire

    // Give amc three projected ctypes that no codec can write correctly, and check
    // that amc refuses each by name and exits with one error per refusal.
    //
    // Msg, packed and projected into Rust, Go and Python, has a pointer member with no
    // wire form, and draws amc.proj_wire.  The language emitters run after the checks
    // and walk the same fields.  The Python emitter, handed the pointer, takes the
    // process down with SIGSEGV.  The exit code is the assertion that no emitter ran.
    // Three refusals exit 3, and a crash in any emitter exits 139.
    //
    // Lenmsg is a message whose length field is a bitfield of a header word, as in
    // amc.LenfldBitfld.  Every codec stores the frame length at the length field's own
    // slot.  A bitfield owns none, so a codec would write the length over the next
    // member, and amc reports amc.proj_lenfld.
    //
    // Dflt carries two nested members with defaults of their own.  Wrap wraps one u64,
    // so its -1 lands on that scalar and amc admits it.  Pair holds two scalars, so its
    // default has no single scalar to land on, and amc reports amc.proj_dflt.
    // void comptest_amc_BadProj(); // atfdb.comptest:amc.BadProj

    // Give amc two store-carrying ctypes whose fixed size plus lenfld extra falls
    // outside the length field's range, and check that amc rejects both in one run.
    // MsgLow (u32 lenfld, extra:-8, fixed size 4) stores a negative length for the
    // minimum frame.  The value wraps through the unsigned store and frames gigabytes
    // from a 4-byte message.  MsgHigh (u8 lenfld, extra:255, fixed size 4) stores 259,
    // past the type's max.  The emitted range guard bound then falls below the fixed
    // size, and every encode throws at first use.
    // void comptest_amc_BadLenfldMinFrame(); // atfdb.comptest:amc.BadLenfldMinFrame

    // Give amc two packed TypeScript ctypes whose lenfld sits on a Bitfld field over a
    // raw header word, with no msgtype and no C++ alloc instance, and check that amc
    // accepts both with exit code 0.
    // This is header-like framing for an external payload.  The TS fixed walk writes
    // the source word raw from the parent, and emits no store for a Bitfld lenfld.  So
    // under the no-store-site doctrine the length words arrive from outside, and the
    // schemas stay legal.  MsgIndiv (size 4, scale:8) is indivisible, and MsgRange (u8
    // lenfld, extra:252, size 4) has an unstorable minimum frame.  amc still accepts
    // both.
    // void comptest_amc_LenfldBitfld(); // atfdb.comptest:amc.LenfldBitfld

    // Give amc a packed message based on a private header ctype with no dmmeta.pack
    // row, and check that amc reports amc.back_pack and amc.what_the_pack, each
    // followed by the bare dmmeta.pack row that the message tells the author to
    // insert.
    // An author lands in this state after repointing a message's base from
    // ams.MsgHeader to a freshly created header.  The namespace is itself pack:Y, so
    // the unpacked header also draws the namespace-level error.
    // void comptest_amc_MissingPack(); // atfdb.comptest:amc.MissingPack

    // Give amc a pmask field of 536870913 bytes, and check that amc generates the
    // schema without a diagnostic.
    // The field's bit count exceeds i32, and the struct size cap (i32 with alignment
    // headroom) permits that.  The schema is legal, so the bit-width bookkeeping must
    // hold the count without wrapping.
    // void comptest_amc_PmaskGiantField(); // atfdb.comptest:amc.PmaskGiantField

    // Give amc csize rows with alignment 32 and alignment 6, and check that amc
    // reports amc.invalid_size for both.
    // Alignment 32 exceeds the headroom that the struct size cap reserves for RoundUp.
    // Alignment 6 is no power of two, so the rounding mask miscomputes.  Silent
    // acceptance of either would ship a wrong layout.
    // void comptest_amc_BadCsizeAlignment(); // atfdb.comptest:amc.BadCsizeAlignment

    // Give amc a variable char inlary and a variable u8 inlary with min>0, and check
    // that amc reports amc.bad_inlary_min for both.
    // The string read copies the input and sets the count to its length, and a short
    // input puts that count below min.  amc rejects the shape until someone defines
    // floor semantics for it.
    // void comptest_amc_BadInlaryMin(); // atfdb.comptest:amc.BadInlaryMin

    // Give amc an inlary with min>max, and check that amc reports
    // amc.bad_inlary_minmax.
    // Init preallocates min elements against a capacity of max, so the first record
    // constructed would die at run time.  The rejection keeps that trap out of the
    // generated code.
    // void comptest_amc_BadInlaryMinmax(); // atfdb.comptest:amc.BadInlaryMinmax

    // Give amc a variable inlary with fnoremove, and check that amc reports
    // amc.bad_inlary_fnoremove.
    // Setary, the separated-string read and Uninit all empty the array through
    // RemoveAll, which fnoremove suppresses.  The generated code would then call a
    // function that amc never emits.
    // void comptest_amc_BadInlaryFnoremove(); // atfdb.comptest:amc.BadInlaryFnoremove

    // Give amc Json cfmts over fields that no FmtJson branch covers, and check that amc
    // reports amc.badjsonfld for every offender in one run, through the accumulate
    // path.
    // The first cause is a reftype that the walk has no branch for, a raw Smallstr
    // field.  The fixture shows it both as a ctype's single datafld and among several.
    // The second cause is a field type with neither a Json cfmt nor a String print,
    // which leaves no expression to produce its node.  The fixture shows it singular
    // and repeated.
    // void comptest_amc_BadJsonFld(); // atfdb.comptest:amc.BadJsonFld

    // Give amc Json cfmts over fields of each kind, print every algo_lib FmtJson, and
    // check how each field reaches its node.
    // A field whose type carries its own Json cfmt gets that type's FmtJson.  A field
    // whose type carries only a String cfmt prints into a string node.  A repeated
    // field becomes an array node holding one element node per element, by the same
    // two rules.  A ctype with one data field prints as that field's node, with no
    // object wrapper.  The same rules pick the node there: the field type's FmtJson,
    // or a string node that its Print writes into, and for a repeated field an array
    // node holding one element node per element.
    // Computed is the case where the two field selectors could disagree.  Its only
    // data field is a Val, so amc elides the object wrapper and no object node exists.
    // Its other field is a repeated fldfunc, which the array branch would emit an
    // array node into.
    // The remaining ctypes carry a print filter, which prints a field only when its
    // bit is set.  PmFilter shows the three member forms, each inside a test of its
    // bit.  Its presence mask is a data field itself, which alone keeps the ctype from
    // inlining.  In PmInlJson and PmInlPrint the mask is a bitfield of the one data
    // field.  That field carries a bit, so amc keeps it from inlining and prints it as
    // an object member under its test, the first through its type's FmtJson and the
    // second through its type's Print.  PmInlJson carries a second bitfield on the
    // mask.  A bitfield prints no node, so amc opens no test around it.  PmInlFree
    // keeps the same shape and names its member explicitly, which leaves the data
    // field off the mask.  That field inlines as it would with no filter.
    // void comptest_amc_JsonFld(); // atfdb.comptest:amc.JsonFld

    // Give amc a Json cfmt on a base-derived ctype and on a bitfield-carrying ctype,
    // print both FmtJson bodies, and check that amc generates both schemas without a
    // diagnostic.
    // Neither the Base field nor the Bitfld field prints.  The base's cloned fields
    // print as members of the object, and the bitfield's bits travel inside the
    // printed source word.
    // void comptest_amc_JsonBaseBitfld(); // atfdb.comptest:amc.JsonBaseBitfld

    // Give amc Json cfmts over the whole rendering table, print every FmtJson, and
    // check how each field renders and that no field name collides with a local of
    // the walk.
    // A field renders through the Json cfmt of its type when the type has one.
    // Otherwise it renders as a string that holds what its Print writes.  An array
    // field renders as a JSON array whose elements render the same two ways.  A record
    // that stands on a single data field renders as that field's own node.  So an
    // array there is the record's array, and a value there is the record's value.
    // AryPlain, AryElem, ValPlain and ValElem are the four single-field cells.  Rec
    // holds the two array cells as members of an object beside a plain value.
    // RecName is the naming control, and each of its fields bears the name of a local
    // that the walk declares.  A local named after a field takes that name through one
    // role suffix: "_node" for the node that stands for the field, and "_ary" for its
    // element span.  The record's own node is named objnode, which no suffix
    // produces.  So the field named "objnode" reaches for the record's node, the field
    // named "x_field" for the node of the field named "x", and the field named "y_ary"
    // for the element span of the field named "y".  The emitted body declares each of
    // them once.  The field named "object" pins the same rule from the other side.
    // The record's node goes by objnode, so a field may take the name "object".
    // void comptest_amc_JsonAry(); // atfdb.comptest:amc.JsonAry

    // Give amc Json cfmts on global ctypes and on ctypes whose one data field carries
    // no data, print every FmtJson, and check that each body holds the ctype's data
    // fields and calls a global's accessors with no parent.
    // The Global anchor field is the singleton instance itself and carries no data.
    // Like Base and Bitfld, it passes the field check without printing, and the object
    // holds the ctype's data fields.  Anchor, Derived and Presence carry that to the
    // degenerate end.  Their one data field carries no data of its own: a Global, a
    // Base and a presence mask.  So the walk emits no member, and each must still
    // return an empty object and never a null node.  One ctype per cause keeps every
    // arm of that rule load-bearing.
    // The FDb ctype also carries the repeated fields, which reach their elements
    // through the field's own Getary.  A global's accessors take no parent argument,
    // so amc emits those calls with none.  The repeated fields are a Tary whose
    // element type prints as a string, a Tary whose element type has a Json cfmt of
    // its own, and an Inlary.  GlobAry is a second global ctype whose only data beside
    // the anchor is one Tary, which keeps the collapse pinned apart from FDb's other
    // members.  The golden pins every emitted FmtJson body in the fixture.
    // void comptest_amc_JsonGlobal(); // atfdb.comptest:amc.JsonGlobal

    // Give a single-file universe a Ptrary, an fcurs request for its cursor and no
    // amcdb.tcurs rows, and check that amc reports amc.bad_fcurs and then
    // amc.missing_tcurs and exits with their count.
    // The request makes the cursor generator run.  The generator reports the missing
    // tcurs row as a generation error and keeps generating.  check_fcurs flags the
    // same request first, so the run accumulates both errors.
    // void comptest_amc_BadMissingTcurs(); // atfdb.comptest:amc.BadMissingTcurs

    // Give a single-file universe a bitset field and no amcdb.tcurs rows, print the
    // bitset's NBits and bitcurs functions, and check that amc emits NBits and no
    // bitcurs cursor.
    // The tcurs row is the only carrier of the dflt flag, so amc has no way to know
    // that the bitcurs cursor is request-only.  Without a dmmeta.fcurs request the
    // cursor stays unemitted.  The NBits witness proves that generation itself ran.
    // void comptest_amc_CursUnrequested(); // atfdb.comptest:amc.CursUnrequested

    // Give amc field defaults that name the containing instance, print the Init of
    // Rec, Msg and Ptr, and check that each reference retargets to the emitting
    // context's instance name, in both spellings that C++ gives that instance.
    // The object spelling *this becomes the instance itself, so Msg Init assigns
    // ssizeof(parent).  The pointer spelling this becomes its address, parenthesized,
    // so that the member access in this->pad binds to the whole address expression.
    // Ptr Init assigns (&parent)->pad.  An identifier that merely starts with the same
    // characters (*thisvalue) refers to no instance and survives verbatim.  Rec's
    // default carries the object spelling next to that longer identifier, and Ptr's
    // carries the pointer spelling next to it.  So each pins that one expression keeps
    // the longer identifier while amc retargets its real reference.
    // void comptest_amc_DfltRetarget(); // atfdb.comptest:amc.DfltRetarget

    // Run amc from a directory with no bin/acr, as outside a checkout, and check that
    // amc reports amc.table_write and fails.
    // The acr subprocess that rewrites the ctypelen, dispsig, tracefld, tracerec and
    // userfunc tables fails to start.  A run that exited 0 would leave those tables
    // silently unwritten.  The command cds into the test's tempdir, so the relative
    // bin/acr path resolves nowhere.  It creates out's gen directories up front, so
    // the output-file writes succeed and only the acr failure is under test.
    // void comptest_amc_TableWriteAcrFail(); // atfdb.comptest:amc.TableWriteAcrFail

    // Point amc at an -out_dir that is a regular file, and check that amc reports
    // amc.outfile_write naming each path and fails.
    // amc is unable to create the gen directories under a file, so every output-file
    // write fails.  A run that exited 0 would leave the generated code silently
    // missing.
    // void comptest_amc_OutfileWriteFail(); // atfdb.comptest:amc.OutfileWriteFail

    // Run amc -e with EDITOR=false, so that its acr -e child exits 1, and check that
    // amc reports amc.edit and fails loudly before regenerating anything.
    // A run that read the nonzero exit as an edit that modified files would regenerate
    // on top of a schema that never changed.  amc skips the phases that would read the
    // input set, and the run ends through its own exit path.  The command cds into the
    // test's tempdir and symlinks bin and data from the checkout.  amc's relative
    // bin/acr spawn and acr's data load then both resolve, while every write stays in
    // the tempdir.
    // void comptest_amc_EditFail(); // atfdb.comptest:amc.EditFail

    // Give a FAST template a presence mask named nullable, and check that the codec
    // calls the presence accessors that the pmask rows generate:
    // 1. Print Msg's FastEncode and FastDecode.  Check that the message's own presence
    // reads through Value_NullableQ and writes through nullable_qSetBitVal, and
    // that the state record's two masks, named assigned and present, keep the
    // accessors those names generate.
    // 2. Rename the FastState presence mask to seen, and add a second template and a
    // namespace zz with a type-encoded cfast.  Check that amc reports
    // amc.fast_state once and exits 1.
    // amc names the presence accessors after the pmask row.  The row's field name
    // prefixes the whole-word bitset accessors, and its funcname spells the per-field
    // ones.  A codec that spelled those names itself would work only for a mask named
    // pmask.  The FAST codec qualifies each field's previous value through two pmasks
    // of the namespace's FastState record, which it tells apart by name.  With the
    // presence mask renamed, the codec has no accessor to call, and the diagnostic
    // and the nonzero exit replace a silently unnamed accessor.  The missing row
    // belongs to the namespace, so step 2 also pins how often amc charges it.  Two
    // templates of the namespace need it, each through an encoder and a decoder, and
    // amc reports it once.  The universe's other two namespaces draw no report.  One
    // holds no cfast ctype.  The other (zz) holds one encoded as a type, which its
    // holder codes inline and which needs no state pmask of its own.
    // void comptest_amc_FastPmaskName(); // atfdb.comptest:amc.FastPmaskName

    // Give amc one ctype per argv strfmt over the same three fields, print the
    // prototypes of their argv accessors, and check which accessors each strfmt
    // generates.
    // Both cfmts ask for print and read alike, so the strfmt in the cfmt key is the
    // only difference between them.  Argv gets the whole surface: the two printers;
    // the ToCmdline convenience, which prefixes bin/ and inherits the verbosity flags;
    // the NArgs token-count table; and the ReadArgv parser that the namespace ReadArgv
    // delegates to.  ArgvGnu gets the two printers alone, so its read:Y buys no
    // command-line reader.  The test prints the prototypes alone, because what is
    // under test is which functions exist.
    // void comptest_amc_ArgvAccessor(); // atfdb.comptest:amc.ArgvAccessor

    // Give amc fconsts across stores of each width and sign, spelled in each form of a
    // C++ integer literal, and check that amc reports amc.fconst_range by name for
    // each value that the field's store fails to hold, stating the range that the
    // store holds.
    // The symbol reaches the field through the generated Set, which for a bitfield
    // masks the store.  So the field would end up holding a value other than the one
    // the schema names, and no accessor could map it back to the symbol.  The rejected
    // values walk the stores: a value past the bits (2000 in ten bits); past the arg
    // type (300 in a u8); past a signed arg type's maximum (2147483648 in an i32); a
    // negative on an unsigned store; and a negative on bits that drop a signed arg
    // type's sign.
    // The value is a C++ integer literal, and amc reads it the way the compiler will.
    // 0377 is octal 255 and fits a u8 store, while 0400 is 256 and exceeds it.  A
    // u/U/l/L suffix names the literal's type and leaves its number unchanged.  A
    // leading plus or minus is the number's sign.  amc carries sign and magnitude
    // apart, so an unsigned 64-bit store holds 0xffffffffffffffff, while an i32 store
    // rejects -0x80000001, one past its minimum.  A magnitude that outruns 64 bits,
    // 0x10000000000000001 or twenty-three decimal digits, fits no store, and amc
    // reports it against the widest one.
    // The controls draw no report.  They are the values at each store's own edges: the
    // u32, u8 and u64 maxima; the i32 minimum in decimal and in hex; 1023 in ten bits;
    // 127 in seven; and -128 in eight, in decimal and in hex.  They also include every
    // spelling that is no whole integer literal and so carries no number to compare: a
    // shift expression, an arithmetic one, digits with trailing text, a character
    // literal, a name only the compiler resolves, and a binary literal, a spelling
    // that amc leaves unread.  A string field's value is a string however it is
    // spelled.  So the string field carries one control reading csv and one reading
    // 99999, and amc bounds neither.  The field's arg is a character array, which
    // resolves to no integer type and has no range to compare with.  The mask of a
    // flag on the top bit of a signed word is unbounded as well, because it is a
    // bitfield's mask and no value of the word.
    // void comptest_amc_BadFconstRange(); // atfdb.comptest:amc.BadFconstRange

    // Give amc one bitfield per declared width, each with fconsts at and past its
    // edge, and check that amc bounds each fconst by its field's width, reports the
    // widths out of bounds, and ends the run at the first width the accessors refuse.
    // A bitfield's Set masks the store to the declared width.  So amc builds the range
    // that it checks an fconst value against by shifting a u64 by that width.  The
    // width is a signed schema argument, and a run reaches this check before the
    // accessors refuse the width.  So the width arrives holding whatever the schema
    // wrote.  The widths from one to sixty-three name bits inside the word.  Each of
    // those fields carries the widest value its bits hold, which draws no report,
    // next to a value past it, which draws one.  A width of sixty-four spans the arg
    // type and masks no bit, so the arg type's own maximum draws no report.  amc
    // reports the three widths outside the range, negative, zero, and sixty-five bits
    // of a u128 source field, as out of bounds, naming the field.  The value 1 on each
    // of them draws no range complaint of its own, because a width that names no bits
    // bounds no value.  A run that computed a bound from one of them would report 1 as
    // a value the field fails to hold, though the field's arg type holds it well.  The
    // bitfield accessors end the run at the first width they refuse, which is the last
    // line of the golden.
    // void comptest_amc_FconstBitfldWidth(); // atfdb.comptest:amc.FconstBitfldWidth

    // -------------------------------------------------------------------
    // cpp/atf_comp/apm.cpp
    //

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_apm_DryRun(); // atfdb.comptest:apm.DryRun

    // Run apm's update chain once per step with test/apm/update_fate.sh, failing that
    // one step each time, and check that the update stops at the failed step:
    // 1. Put stubs for the children in the test's own bin/.  The stub chosen as the
    // fate fails, and every other stub hands the call on to the real tool.
    // 2. Run the control, which fails no step, and check that it prints a two-record
    // plan and exits 0.
    // 3. Run the update with -dry_run from the test's own tempdir once per step, and
    // check apm's exit code and the size of the plan it prints for each run.
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
    // void comptest_apm_UpdateFate(); // atfdb.comptest:apm.UpdateFate

    // Run an apm transaction whose script meets each fate in turn, and check that apm
    // exits with the script's own exit code:
    // 1. Symlink data/ from the checkout root into the test's tempdir.
    // 2. For each of exit 0, 1, 127 and 255, write a bin/acr shim that exits with that
    // code, run apm -e, and check that the exit code comes back unchanged.
    // 3. Write a shim that sends SIGTERM to its parent, the bash running the plan, and
    // then closes its output and sleeps.  Run apm -e, and check that apm exits 143.
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
    // void comptest_apm_TransactionExit(); // atfdb.comptest:apm.TransactionExit

    // Scan hand-written sources for the records that their code names, and check what
    // apm carries and what -check reports:
    // 1. Build a small data directory in the tempdir from test/apm/srcscan_pkg.ssim and
    // links into data/.
    // 2. Write srcscan.cpp, which compares against dmmeta_Reftype_reftype_Val, calls
    // dmmeta::Reftype_Print, and mentions two other reftype constants only in a
    // comment and a string.
    // 3. Write srcmixed.cpp, which defines the comptest functions of apm.DryRun and
    // apm.SrcScan and calls apm.UpdateFate's from a body.
    // 4. Run apm srcref -showrec, where package srcref takes srcscan.cpp with ref:Y.
    // Check that it gains the Val row and the ctype, and neither of the other two
    // rows.
    // 5. Run apm srcbare -check, where package srcbare takes the file alone, and check
    // that it reports both records as dangling (apm.dangling).
    // 6. Run apm srcmixed -check, where package srcmixed builds atf_comp and carries
    // DryRun and UpdateFate.  Check that it reports srcmixed.cpp as mixed
    // (apm.mixedfile): one record carried, one missing.
    // A hand-written source references each record whose generated symbol its code
    // names.  The call to UpdateFate's function is a use, so UpdateFate counts on
    // neither side.
    // void comptest_apm_SrcScan(); // atfdb.comptest:apm.SrcScan

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
    // void comptest_apm_Pkgfile(); // atfdb.comptest:apm.Pkgfile

    // -------------------------------------------------------------------
    // cpp/atf_comp/aqlite.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_aqlite_CompileOptions(); // atfdb.comptest:aqlite.CompileOptions
    // void comptest_aqlite_Constraints(); // atfdb.comptest:aqlite.Constraints
    // void comptest_aqlite_ErrorHandling(); // atfdb.comptest:aqlite.ErrorHandling
    // void comptest_aqlite_Joins(); // atfdb.comptest:aqlite.Joins
    // void comptest_aqlite_Number(); // atfdb.comptest:aqlite.Number
    // void comptest_aqlite_PatternMatching(); // atfdb.comptest:aqlite.PatternMatching
    // void comptest_aqlite_Performance(); // atfdb.comptest:aqlite.Performance
    // void comptest_aqlite_Smoke(); // atfdb.comptest:aqlite.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_cmdline.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_atf_cmdline_Bare(); // atfdb.comptest:atf_cmdline.Bare
    // void comptest_atf_cmdline_Debug(); // atfdb.comptest:atf_cmdline.Debug
    // void comptest_atf_cmdline_Help(); // atfdb.comptest:atf_cmdline.Help
    // void comptest_atf_cmdline_Minimal(); // atfdb.comptest:atf_cmdline.Minimal
    // void comptest_atf_cmdline_MinimalExec(); // atfdb.comptest:atf_cmdline.MinimalExec
    // void comptest_atf_cmdline_Rich(); // atfdb.comptest:atf_cmdline.Rich
    // void comptest_atf_cmdline_RichExec(); // atfdb.comptest:atf_cmdline.RichExec
    // void comptest_atf_cmdline_Sig(); // atfdb.comptest:atf_cmdline.Sig
    // void comptest_atf_cmdline_Verbose(); // atfdb.comptest:atf_cmdline.Verbose
    // void comptest_atf_cmdline_Version(); // atfdb.comptest:atf_cmdline.Version

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_comp.cpp
    //

    // Run every selected comptest and report how many passed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:atf_comp

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_cov.cpp
    //

    // Run atf_cov over profile data that gcov refuses, and check that atf_cov names the
    // failed gcov command and reports the directory lost:
    // 1. Create bad.d holding one x.gcda that is garbage, so gcov refuses it and the
    // notes file beside it, exits 2 and produces no .gcov.
    // 2. Install a stand-in gcov that refuses every file the way GNU gcov does.
    // 3. Run atf_cov with -mergepath:$tempdir/bad.d, and check that it exits 1.
    // 4. Read its stderr back from a file, and check that the atf_cov.gcov_fail line
    // carries exit:2 and the lines gcov printed about x.gcda and x.gcno.
    // 5. Check that the atf_cov.merge line reports
    // n_gcda:1  n_gcov:0  n_fail:1  success:N.
    // The test reads the report from a file because the harness drops every output line
    // that names a .gcda, which is gcov's own noise in a coverage build.
    //
    // The test runs a stand-in because the gcov on a macOS host is LLVM's.  LLVM's gcov
    // words its refusal differently and exits 0 on a file it fails to read.  This test
    // is about what atf_cov makes of a refusal.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_atf_cov_GcovFail(); // atfdb.comptest:atf_cov.GcovFail

    // -------------------------------------------------------------------
    // cpp/atf_comp/credd.cpp
    //

    // Drive the store without a daemon, and check each change that credd takes or
    // refuses:
    // 1. Add token gl to an empty store with password pw1, and check that the first
    // change sets the master password.  List the store as a table, as ssim and as
    // json.
    // 2. Add a github token with the permission bogus, add gl a second time, add a
    // token with no kind, and add a token whose secret is on the command line.
    // Check that credd refuses each.  It checks a token's permissions against its
    // kind, and takes a name once.
    // 3. Show gl, and check that credd refuses to show a secret with no daemon.
    // 4. Run -check with a long window and with a short one, and check that it finds
    // what expires.
    // 5. Add an aws credential, remove gl, and check that a second remove of gl fails.
    // 6. Change the password to pw2 with -passwd, which asks for the old one.  Check
    // that pw1 then fails to open the store and pw2 opens it.
    // 7. Dump what the files hold, and dump it as json.  Check that the json form has
    // one array per table, and that credd refuses -dump -format:text.
    // 8. List the store's files, and check that the removed row is gone from them.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_credd_Store(); // atfdb.comptest:credd.Store

    // Wait until the credd daemon of the scenario's home, $HOMEDIR, answers
    // -status, which it does once its socket is bound; a daemon that never answers
    // within 200 tries fails the test, and the failure carries what the last
    // -status printed, since that is the client's own account of why.
    void WaitCredd();

    // Run the credd daemon, and check that it serves tokens, survives a bad peer and a
    // bad reload, feeds its agent and follows links:
    // 1. Add token gl, and start credd -daemon in the foreground under the harness with
    // the password in its environment.
    // 2. Show gl, and check that the daemon opens it.  Show nope, and check that the
    // daemon refuses the unknown token.
    // 3. Send a request from a peer that closes before it reads the reply, and check
    // that the daemon goes on answering.
    // 4. Put a row of an unknown kind in the store, and ask the daemon to reload.
    // Check that the daemon refuses the reload and goes on serving gl.
    // 5. Make key k1 with -keygen, and add key k2 from a file.  Check that both reach
    // the agent at the reload that the command line sends.  Remove k1, and check that
    // it leaves the agent.
    // 6. Link gli-mr to gl.  Check that credd refuses a link to gli-mr and a link to an
    // absent target.  Check that gli-mr answers with gl's secret, and that credd
    // refuses to remove gl while a link points at it.
    // 7. Check that credd refuses to renew key k2 from the token file.  Renew gli-mr
    // from stdin, and check that gl shows the renewed secret.
    // 8. Remove gli-mr and gl, and run -stop.  Check that the daemon exits 0, and that
    // the agent socket and the daemon socket are gone.
    // The harness starts the daemon itself, so the test tracks its pid and asserts its
    // exit.  -stop ends the daemon and the agent with it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_credd_Daemon(); // atfdb.comptest:credd.Daemon

    // Run the daemon that a service host runs, with -nostore, and check that it serves
    // what -seed sends over its socket and writes no table that holds a secret:
    // 1. Start credd -nostore -daemon with no password anywhere, and check its status.
    // 2. Seed key ci, and check that it reaches the agent.
    // 3. Seed ci again from another key, and check that the agent holds the new key
    // alone.
    // 4. Seed a file that holds no key, and check that the seed exits 1 and that the
    // agent still holds what it held.
    // 5. Seed ci under another kind, seed an empty file, and seed an unknown kind.
    // Check that each seed exits 1.
    // 6. Run -add, and check that credd refuses it outright.
    // 7. Seed an openai credential of two lines from a pipe with -src:-, and check that
    // -show answers with both lines.
    // 8. Check that the store holds no cred.ssim with content (no secret on disk).
    // 9. Stop the daemon, and check that it exits 0 and that both its sockets are gone.
    //
    // A -nostore daemon reads no store and writes none.  It comes up holding no
    // credential, and every credential it serves arrives over its socket.  A refusal is
    // a reply, so the exit code is what a caller reads.  Every refused seed exits 1.
    // Otherwise a job would go on believing it had an identity.  Honoring -add would ask
    // for a new password and write the store that -nostore exists to avoid.
    // void comptest_credd_Nostore(); // atfdb.comptest:credd.Nostore

    // Walk a home with the guided path -add:%, add single files with -add:<file>, and
    // check what credd offers, takes and refuses:
    // 1. Write a home that holds gli.ssim with two token records, a record naming no
    // server and an unparsed line.  Add an aws *.token file, a binary *.token file,
    // a plain key and a passphrase-protected key.
    // 2. Add the plain key's public half with no password, and check that credd refuses
    // it and writes no store.
    // 3. Walk the home.  Take one gli record, the aws token and the plain key, and
    // decline the other record.  Check that the locked key fails on a wrong
    // passphrase, and that the walk passes over the binary file.  Check that the walk
    // passes over the record naming no server, with a line.
    // 4. Walk again, and check that the walk skips what the store holds and takes the
    // locked key with the right passphrase.
    // 5. Write a token file, a key file and a token file without a suffix.  Add each
    // with -add:<file>, and check that credd offers it.
    // 6. Check that credd refuses an empty file, an option meant for -add:<name>, and
    // a public key.
    // 7. Walk a third time.  Check that the walk adds the declined record and drops its
    // token from gli.ssim, while the record bound in step 3 keeps its own.
    // 8. Add a file whose content opens with the github prefix, and check that credd
    // offers it as a github token.
    // 9. List the store, and print gli.ssim and the directory.  Check that gli.ssim
    // gains a binding line and keeps its token, in the shape an older gli reads.
    // void comptest_credd_Import(); // atfdb.comptest:credd.Import

    // Mint credentials that the tree declares, with no server to mint them at, and
    // check what -mint prints, takes and refuses:
    // 1. Write a tree under $tempdir/data that declares four credentials: bot, a group
    // token acting as developer with two scopes; pat, a personal token; norole, a
    // group token with no role; and acct, a group token that the test gives no
    // group.
    // 2. Mint bot and pat.  Check that -mint prints the steps a person follows, with
    // the name, role and scopes filled in, and takes the token per -src.
    // 3. Mint bot again, then ghost, norole and acct.  Check that credd refuses a name
    // already taken, a name the tree does not declare, a group token with no role,
    // and one with no group.
    // 4. List the store, and renew bot.  Check that -renew names the role the tree
    // gives bot.
    // The store holds no gitlab token to ask with, so -mint prints the steps.
    // void comptest_credd_Mint(); // atfdb.comptest:credd.Mint

    // -------------------------------------------------------------------
    // cpp/atf_comp/gcache.cpp -- Comptests for gcache
    //

    // Run gcache -enable on a cache directory that does not exist, wrapping the command
    // true, and check that gcache reports the missing directory and exits 1.
    // A wrapped command's status adds to the run's exit code, so a successful compile
    // never masks the setup failure reported ahead of it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_gcache_CacheDirFail(); // atfdb.comptest:gcache.CacheDirFail

    // Restore a coverage cache hit with one half of the pair unwritable, and check that
    // gcache leaves neither half fresh on its own and fails the run:
    // 1. Publish x.cpp compiled with --coverage.
    // 2. Hold x.o with a directory, and compile again.  Check that the run exits 1, and
    // check the state of x.gcno.
    // 3. Hold x.gcno with a directory, and compile again.  Check that the run exits 1,
    // and check the state of x.o.
    // 4. Free both paths and compile again, the control.  Check that the pair restores
    // and the run succeeds.
    // A cached coverage entry is one blob that holds both halves, so that a hit never
    // pairs a fresh .o with a stale .gcno.  The restore has to keep that promise when
    // only one half is writable.
    // void comptest_gcache_CoverageRestoreFail(); // atfdb.comptest:gcache.CoverageRestoreFail

    // Forge each kind of broken coverage entry, and check that gcache counts it as a
    // miss, compiles again and republishes the entry:
    // 1. Publish x.cpp compiled with --coverage.
    // 2. Run the stages below in order.  Each forges one kind of entry, marks x.gcno
    // with a recognizable string, removes x.o and compiles.  Check in every stage
    // that the object comes out non-empty and that the marked x.gcno is gone.
    // 3. WHOLE: leave the entry as published, the first control, and check that it
    // reports a hit.
    // 4. Forge the rejected forms: the notes half truncated away, an offset that
    // describes an empty object half, a bare .o left under the coverage key, fewer
    // bytes than the offset itself, and the key held by a directory.  Check that
    // each reports a miss.
    // 5. AGAIN: compile once more, the second control, and check that it reports a hit.
    // The cache directory outlives the builds that write it.  Another tool, an older
    // gcache or an interrupted write can leave a file under a coverage key that is not
    // two non-empty halves.  Restoring from one puts a fresh object next to whatever
    // .gcno the working directory holds.  gcov then rejects the pair on a stamp
    // mismatch and reports 0% coverage for that object.  The build sees no error,
    // because a restore asked for half a pair delivers that half and reports success.
    // gcache therefore judges an entry from its bytes.  It uses an entry only when the
    // offset that the entry starts with leaves bytes on both sides of itself.  The pair
    // the compile leaves behind always comes from one compile.
    // void comptest_gcache_CoverageBlobMiss(); // atfdb.comptest:gcache.CoverageBlobMiss

    // Compile through gcache with g++ under each spelling of the coverage flags and to
    // each kind of object name, and check which compiles get the coverage cache format:
    // 1. Compile x.cpp under --coverage, -coverage, -ftest-coverage, -fprofile-arcs and
    // no coverage flag.
    // 2. Compile x.cpp with --coverage to x.obj and to foo.
    // 3. GCHNAME: compile h.hpp with --coverage to h.hpp.gch.
    // Each compile runs twice, with the object and any notes removed in between.  Check
    // that the first run misses and the second hits, and that the notes come back
    // exactly for the compiles that produce them.
    //
    // A coverage entry is one blob that holds the object and its coverage notes, so the
    // format fits exactly the compiles that write notes.  -ftest-coverage writes them.
    // --coverage asks for instrumentation and notes together, and -coverage is that
    // flag with one dash.  -fprofile-arcs instruments the object and writes no notes
    // file at all.  gcache must still cache a compile with no notes to publish, as a
    // bare object.  Asking it for a blob leaves it with one half, unpublishable and so
    // uncached forever: a recompile, a warning, and the same miss on the next build.
    // The plain compile is the control for the bare-object format.  -fprofile-arcs is
    // the control that keeps the rule from reading as a rule about how many dashes a
    // flag carries.  gcache.CoverageSpelling pins the long forms that gcc alone
    // accepts, --test-coverage and --profile-arcs, through a driver that runs no
    // compiler.
    //
    // The name of the object is the format's other input, because the notes take the
    // object's own name with its extension replaced.  gcc answers an output named x.obj
    // with x.gcno, and one named foo with foo.gcno.  A compile whose object lacks the
    // .o ending therefore writes notes all the same and needs the same blob.
    //
    // An output named like a precompiled header looks like an exception.  gcc answers
    // an output named h.hpp.gch with h.hpp.gcno under -c, and with h.hpp.gch-h.gcno
    // without it.  gcache caches only compiles that carry -c, so the name it derives is
    // the name gcc writes.  The GCHNAME stage pins the notes that come back from a hit
    // on such an output.  What the stage leaves at h.hpp.gch is an object, because
    // gcache compiles the preprocessed text of the translation unit.  The stage pins
    // the derivation from the name, and leaves open how gcache caches a precompiled
    // header.
    // void comptest_gcache_CoverageFlag(); // atfdb.comptest:gcache.CoverageFlag

    // Hit the cache in both formats over targets that are absent, backdated or
    // identical, and check that every file a hit writes carries the time of that hit:
    // 1. Publish x.cpp as a bare object.
    // 2. BAREABSENT: remove x.o and compile, the control for an absent target.
    // 3. BARESAME: backdate x.o to 2020 and compile over the same bytes.
    // 4. Publish x.cpp with --coverage.
    // 5. COVNONOTES: backdate x.o, remove x.gcno and compile.
    // 6. COVSAME: backdate x.o and x.gcno to 2020 and compile over the same bytes.
    // 7. PUBSAME: publish y.cpp with -frandom-seed=t, backdate its entry to 2020, and
    // publish it again under -force.  Check that the entry's bytes stay the same and
    // that its time moves past the backdate.
    // In steps 2, 3, 5 and 6 check that the report says hit:Y, and that each file the
    // hit writes comes out newer than the backdate.
    //
    // A build tool decides that an object is out of date by comparing its modification
    // time against its sources.  An object that a hit puts in place therefore has to
    // carry the time of that hit.  The build tool asks again for an object left with an
    // older time, on every later build, because the restore leaves the triggering
    // condition in place.  A hit whose cached bytes equal the bytes already at the
    // target reaches this case.  A header whose touch moved no preprocessed text keeps
    // the key hitting while the build tool considers the object stale.  hit:Y makes the
    // times meaningful.  BARESAME is the bare-object format's answer to the
    // identical-bytes case that the coverage format faces in COVSAME.
    //
    // The cleanup reads the modification time of each cache entry.  It deletes an entry
    // older than the retention window, and the byte budget evicts in oldest-last-use
    // order.  A publish over an entry with unchanged bytes therefore has to move that
    // time as well.  Otherwise an entry that a build republished every day looks a week
    // old.  -frandom-seed fixes the coverage notes so the bytes really are identical.
    // PUBSAME pins the bytes next to the time, because a publish that wrote different
    // bytes would freshen the entry whatever rule it used.
    // void comptest_gcache_HitMtime(); // atfdb.comptest:gcache.HitMtime

    // Combine independent failures in one gcache run, and check that the exit code sums
    // them:
    // 1. CLEAN: compile x.cpp with --coverage, the first control, and check that the
    // run exits 0.
    // 2. Hold the -install marker cache/.keep with a directory.
    // 3. MARKER: run -install with the command true, and check that the run exits 1.
    // 4. MARKER_CMD: run -install with the command `false`, whose status is 1, and
    // check that the run exits 2.
    // 5. MARKER_RESTORE: hold x.o with a directory, and compile with -install.  Check
    // that the run exits 2, for the marker and for the hit that fails to write its
    // object file.
    // 6. CMD: free the marker, run -install with the command `false`, and check that
    // the run exits 1.
    // 7. HIT: compile again, the second control, and check that the run exits 0.
    // A run's exit code is the wrapped command's own exit status, plus one for each
    // failure that keeps the run from delivering the object that its caller asked for.
    // Such a failure thus stays visible behind the command's status and behind another
    // of its kind.  A failure that costs only a future cache miss is a warning and
    // leaves the exit code alone.  An entry that fails to publish leaves the code where
    // the compile left it.  gcache.CoverageRestoreFail shows the object-file failure
    // alone.
    // The marker write is the one setup failure that leaves the cache usable, which lets
    // a second, independent failure happen in the same run.  The marker is a directory,
    // so writing it fails while the cache directory around it still serves hits.
    // void comptest_gcache_ExitCodeCount(); // atfdb.comptest:gcache.ExitCodeCount

    // Plant entries that no compile can use, an entry of no bytes or a directory at an
    // entry's path, and check that gcache misses on them in both formats and that the
    // cleanup plants none:
    // 1. RECREATE: publish an entry, delete it and run -gc.  Check that the cleanup
    // leaves the path absent.
    // 2. REFRESH: publish an entry, backdate it past the retention window and run -gc.
    // Check that the cleanup moves the entry's time up, so the age pass in the same
    // run keeps it.
    // 3. PCHREFRESH: plant a file where a precompiled header would sit, and rewrite the
    // log so its lines name that file.  Backdate the file past the retention window
    // and run -gc, and check that the cleanup keeps it.
    // 4. EMPTYHIT: publish an entry, truncate it and compile.  Check that the compile
    // misses, compiles, and republishes an entry with bytes.
    // 5. EMPTYCOV: do the same in the coverage format, and check that the compile
    // misses.
    // 6. DIRBARE: put a directory at a bare entry's path and compile.  Check that the
    // compile misses and that the publish takes the path back.  Compile again, and
    // check that the next build with the same key hits.
    // 7. DIRCOV: do the same in the coverage format, the control for that shape.
    // 8. NOPUBLISH: compile to e.o, a symlink to /dev/null that keeps no bytes, and
    // check that gcache publishes no entry for it.
    // 9. CONTROL: hit an ordinary entry that has bytes.
    //
    // A cache entry can end up empty without anyone noticing.  The cleanup keeps an
    // entry that a recent log line names by refreshing that entry's modification time.
    // A log line still names an entry that the byte budget or an earlier age pass
    // already deleted.  A refresh that opens the path for writing then recreates it with
    // no bytes in it.  The next compile with that key finds a file, calls it a hit,
    // renames the empty file over its object and exits 0.  The link that follows fails,
    // naming the object as a file of an unrecognized format.  Each such hit writes a
    // log line of its own, so the log line stays young.  The next cleanup recreates the
    // file that the age pass just deleted, and the translation unit stays wedged.
    //
    // A directory at an entry's path is the other shape that no compile can use, and
    // the two formats read it differently.  The coverage format finds no offset in it
    // and misses.  A bare format that took the directory's size for an object's would
    // call it a hit.  The copy would then fail and leave the run with no object at all.
    // A hit publishes no entry, so the key would stay wedged past the build that met it,
    // until someone removed the directory.
    //
    // gcache therefore uses an entry only when it is a regular file with bytes, and the
    // cleanup has to refresh without creating.  A refresh that moves no time leaves the
    // entry for the age pass to delete.  A compile that used a precompiled header names
    // the header beside its object in the log.  The compiles that hit the header only
    // read it, so the refresh alone moves its time.  PCHREFRESH rewrites the log because
    // the cleanup reads only the log, and what built the header is outside its concern.
    // gcache.Pch builds the header.  The coverage format reads an empty entry as a blob
    // whose offset leaves no bytes on either side of itself, and counts it a miss.
    // void comptest_gcache_EmptyEntry(); // atfdb.comptest:gcache.EmptyEntry

    // Compile through test/gcache/cc under every spelling of the coverage flags that
    // gcache recognizes, and check which spellings select the coverage cache format.
    // Compile each spelling twice, with the object and notes removed in between.  Check
    // that the first run misses and the second hits.  Check that the notes are present
    // after the hit for the four spellings that write notes, and absent for the two
    // spellings of -fprofile-arcs and for the plain compile.
    // gcache.CoverageFlag pins the rule against what a real compiler writes, so it uses
    // only the spellings every driver accepts.  The two remaining ones, --test-coverage
    // and --profile-arcs, are gcc's long forms of -ftest-coverage and -fprofile-arcs.
    // clang refuses them.  test/gcache/cc runs no compiler.  It takes any flag, writes
    // an object of a few bytes, and writes notes beside it every time.  The notes
    // therefore come back from a hit exactly when gcache filed the entry as a blob, so
    // the test reads the classification alone.
    // void comptest_gcache_CoverageSpelling(); // atfdb.comptest:gcache.CoverageSpelling

    // Compile one source in directory a, then in b, sharing one cache, and check that a
    // coverage entry serves the directory that wrote it while a bare entry serves every
    // directory:
    // 1. Install the cache from a and from b.
    // 2. Compile x.cpp without a coverage flag in a and then in b, and check that b
    // hits off a's entry.
    // 3. Compile x.cpp with --coverage in a and then in b, and check that b misses.
    // 4. Check that b's object names b's own directory in its profile name
    // (cov_b_name).  The miss exists to keep this fact.
    // 5. Compile with --coverage in a again, and check that it hits.
    // An object compiled for coverage carries the name of the profile file it writes at
    // run time.  gcc anchors that name at the directory the compile runs in.  The key of
    // a cache entry is the preprocessing command and the preprocessed text.  Both hold
    // only the paths that the command line spelled, so the same source compiled in two
    // directories with a shared cache computes the same key.  If gcache served the
    // entry across directories, the second directory's object would write its profile
    // under the first directory's path.  gcov would then find no data to measure for
    // it.  The key of a coverage compile therefore carries the directory.
    // void comptest_gcache_CoverageCwd(); // atfdb.comptest:gcache.CoverageCwd

    // Fail each setup step of -install, -enable and -disable, and check that gcache
    // reads each step's status, prints `done` only after all of them, and leaves the
    // link as it was when it refuses a request:
    // 1. NODIR: run -install with a cache directory under nofile, a regular file.
    // Check that the marker write reports the failure, that no other step reports it
    // a second time, and that the run exits 1.
    // 2. LINKDIR: put a directory where the .gcache link goes, and run -install.  Check
    // that gcache sets up the cache directory, reports the link failure with its
    // errno, and exits 1.
    // 3. DISABLEDIR: run -disable on that directory, which unlink fails to remove.
    // Check that gcache reports one gcache.error and exits 1.
    // 4. CONTROL: clear the way and run -install.  Check that it says `done`, exits 0
    // and leaves the link.
    // 5. BADDIR: run -enable on a directory that does not exist.  Check that the run
    // exits 1 and that the working link is still there.
    // 6. DISABLE: run -disable, and check that it removes the link and exits 0.
    // 7. REDISABLE: run -disable again, and check that it exits 0, because a link
    // already gone is the state asked for.
    // A machine shares one cache, so a cache that -install left disabled shows up
    // only as build wall clock that nobody attributes to gcache.  The run has to say so
    // and exit non-zero, and it must not say `done` first.  The errno in step 3 differs
    // by platform, EISDIR on Linux and EPERM on macOS, and the golden masks it.
    // void comptest_gcache_InstallFail(); // atfdb.comptest:gcache.InstallFail

    // Compile two units that include a header marked with __gcache_pragma_pch_preprocess
    // first, and check that gcache precompiles the header once and every such unit
    // reuses it:
    // 1. BUILD: compile p.cpp on an empty cache.  Check that the object misses, that
    // gcache builds the header (pch_hit:N) and pch_source names it, and that the .gch
    // the report names is present.
    // 2. REUSE: compile q.cpp, a different unit with the same header first.  Check that
    // the object misses and the header hits.
    // 3. HIT: compile p.cpp again, and check that gcache serves the object and consults
    // no header.
    // 4. FORCE: compile p.cpp under -force.  Check that gcache rebuilds the header and
    // reads it back as built, into the same entry that BUILD wrote (force_entry).
    // gcache walks the preprocessed text for gcc's line markers and cuts the marked
    // header out.  It compiles the header to a .gch keyed on its text, and rewrites the
    // unit to read that .gch through a pch_preprocess pragma.  gcc alone builds such a
    // header, so the test reads the walk through test/gcache/cc.  Under -E that driver
    // expands one level of #include with the same markers.  It writes a .gch when the
    // output carries a .gch name.  Every build of the same header text lands on the
    // same entry.
    // void comptest_gcache_Pch(); // atfdb.comptest:gcache.Pch

    // gcache under clang, through test/gcache/cc run as a link named clang++, which
    // answers gcache's probe with __clang__ and writes an object naming its source.
    // A miss compiles x.cpp from the source, so the object names x.cpp, and the
    // second compile hits on the stored probe answer and the filed object.  The
    // driver edits edit.cpp while compiling it, so the source no longer reads as the
    // text the key was taken from.  gcache files no object under that key, and the
    // second compile of edit.cpp misses again: an object filed there would be the
    // edited source's, served for text that never produced it.
    // void comptest_gcache_ClangSource(); // atfdb.comptest:gcache.ClangSource

    // -------------------------------------------------------------------
    // cpp/atf_comp/jkv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_jkv_ArrayFill(); // atfdb.comptest:jkv.ArrayFill
    // void comptest_jkv_ReverseSmoke(); // atfdb.comptest:jkv.ReverseSmoke

    // Ask jkv to write $tempdir/notadir/out.json, where a regular file stands in place
    // of the parent directory, and check that jkv names the file and the errno and exits
    // non-zero.
    // A tool that writes its output file has to check the write.  Otherwise a full disk
    // or an unwritable path leaves no output while the tool reports success.  The
    // blocking file makes the write fail under any filesystem state or privilege level.
    // The test seeds it in its own tempdir, which the harness reclaims per run.
    // void comptest_jkv_WriteFail(); // atfdb.comptest:jkv.WriteFail
    // void comptest_jkv_Smoke(); // atfdb.comptest:jkv.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/mdbg.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_mdbg_OutOfOrderArgs(); // atfdb.comptest:mdbg.OutOfOrderArgs
    // void comptest_mdbg_Smoke(); // atfdb.comptest:mdbg.Smoke
    // void comptest_mdbg_SmokeBreak(); // atfdb.comptest:mdbg.SmokeBreak
    // void comptest_mdbg_SmokeBreak2(); // atfdb.comptest:mdbg.SmokeBreak2

    // -------------------------------------------------------------------
    // cpp/atf_comp/orgfile.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_orgfile_Hash(); // atfdb.comptest:orgfile.Hash
    // void comptest_orgfile_MoveByDate(); // atfdb.comptest:orgfile.MoveByDate
    // void comptest_orgfile_Dedup(); // atfdb.comptest:orgfile.Dedup
    // void comptest_orgfile_DedupPathregx(); // atfdb.comptest:orgfile.DedupPathregx
    // void comptest_orgfile_MoveNoop(); // atfdb.comptest:orgfile.MoveNoop
    // void comptest_orgfile_ConsumeInput(); // atfdb.comptest:orgfile.ConsumeInput
    // void comptest_orgfile_MoveDot(); // atfdb.comptest:orgfile.MoveDot

    // -------------------------------------------------------------------
    // cpp/atf_comp/proc.cpp
    //

    // Set test variable and add to Replscope
    void SetVar(strptr name, strptr value);

    // Apply $-substitution to string
    //
    // Ins appends an end-of-line by default, because its usual job is to add a
    // line to a text being assembled.  A substituted value here is not a line: it
    // is a command to spawn, a message to write to a process, or the value of an
    // environment variable, and a trailing newline in any of those is wrong.  An
    // environment variable is where the difference shows: a cluster told its
    // instance directory is "temp/atf_comp/<test>\n" creates that directory, and
    // every path a test script builds under it then breaks apart at the newline.
    // Return S with every $-variable of the running test replaced by its value:
    // $tempdir is the test's directory, $bindir the build under test, and `$$`
    // stands for one dollar sign.
    tempstr Subst(strptr s);

    // Return the path of FILE inside the running test's tempdir; an absolute FILE
    // is returned as it is.
    tempstr TempPath(strptr file);

    // Export the environment every test of this tree is run under, as the
    // atfdb.testenv table states it.
    //
    // A comptest that starts a cluster needs that cluster pointed at this test's
    // own directory rather than at whatever instance the host is running, and the
    // name of the variable that does the pointing belongs to the platform under
    // test, not to the runner.  So each row names a variable and a value, the
    // value goes through the same $-substitution a test script does -- `$tempdir`
    // is this test's directory -- and a tree with no such platform simply has no
    // rows.  SLOW says the run is instrumented and therefore far slower than
    // release, which is when a row marked slowonly applies: startup graces belong
    // to that case and would otherwise put progress lines in the goldens.
    void SetTestenv(bool slow);

    // Set the environment variable NAME to VALUE for every process the test starts
    // from here on, and log it as `# setenv NAME:VALUE`.  VALUE is logged as the
    // test wrote it, so the golden shows `$HOMEDIR` and the process sees the
    // $-substituted path.  Whether the line is logged follows from the test's own
    // record: the first SetEnv of a name always logs, and a second SetEnv to the
    // value the test already set does nothing while the environment still holds
    // its substitution, whatever the harness's environment held when it started.
    // A SetVar that moves the substitution between the two makes the second set
    // and log again.  The harness's own value, if it had one, is restored by
    // ClearEnv.
    void SetEnv(strptr name, strptr value);

    // Remove the environment variable NAME from the environment of every process
    // the test starts from here on, and log it as `# unsetenv NAME`.  ClearEnv
    // restores the harness's own value.
    void UnsetEnv(strptr name);

    // Forget every SetEnv and UnsetEnv of the test and put the harness's own
    // environment back, logging `# clearenv` when there was anything to forget.
    // A test calls it where its environment ends, and the harness calls it right
    // after every test's step, whether the step returned or threw, so the wait,
    // the comparison and the capture run in the harness's own environment.
    void ClearEnv();

    // Record the start of the test stage NAME in the log, as `# stage NAME`, so a
    // reader of the golden can tell one run of the tool from the next.
    void Stage(strptr name);

    // Record a fact the test established in C++ in the log, as `# check NAME:VALUE`,
    // beside the lines the processes printed, so the golden pins it.
    void Check(strptr name, strptr value);

    // Record each distinct line of TEXT that REGX matches whole, trimmed and in
    // sorted order, as `# check NAME:<line>`, and return how many lines it recorded.
    // REGX is an SQL regex in the form acr reads: `%` matches any run, `_` any
    // character, and `(a|b)` either branch.  This is the C++ form of
    // `| grep | sort -u` on a tool's output: the test reads the output with SysEval
    // and states the lines that concern it, and a test that expects none of them
    // checks the count.
    int CheckMatch(strptr name, strptr text, strptr regx);

    // Record the state of FILE, a path under the tempdir, as `# check NAME:<state>`:
    // `present` for a regular file holding bytes, `empty` for one holding none,
    // `directory` for a directory, and `absent`.
    void CheckFile(strptr name, strptr file);

    // Record LINE, a line the test selected from FILE, in the log as `FILE -> LINE`,
    // the form a process's output takes, with the fields atfdb.unstableattr names
    // masked as a process's output has them masked, so a golden pins a fragment of
    // a file the test read without pinning the whole file.
    void Excerpt(strptr file, strptr line);

    // Spawn subprocess with $-substitution, return reference.
    atf_comp::FProc &ProcStart(strptr cmd);

    // Write SCRIPT, one shell line per line, to NAME.sh in the test's tempdir and
    // start bash on it from the tempdir; return the process.  Each line is recorded
    // in the log as `# NAME.sh: <line>` before the start, with its $-variables
    // unreplaced, so the golden shows the script as written and reads the same on
    // every machine.
    atf_comp::FProc &ScriptStart(strptr name, strptr script);

    // Connect to ADDR, a $-substituted `host:port`, and return the connection as a
    // process named tcp, tcp-2 and so on.  ProcWrite sends a line over the socket,
    // ProcRead reads the server's lines with each trailing carriage return
    // removed, and ProcWait closes the socket at once, discarding what the test
    // did not read, so the log holds exactly the lines the test read.  KEEP, when not empty, is a
    // `|`-separated list of line prefixes: a line starting with none of them is
    // read past and neither logged nor matched, which keeps out of the golden what
    // the server sends at its own pace, such as a delivery racing a PONG.  ProcWriteEof half-closes the
    // socket, and a ProcRead to end of file then reads until the server closes.
    //
    // A protocol client driven from a comptest has to end when the test is done
    // with it.  nc does not: with the server holding the connection open it waits
    // out its own timeout whatever its stdin does, and every stanza built on it pays
    // that whole bound.  The harness holding the socket itself ends the
    // conversation the moment the test has read what it asserts.
    atf_comp::FProc &TcpStart(strptr addr, strptr keep = "");

    // Keep PROC's writes and reads out of the log while QUIET holds, and log the
    // switch as `# quiet PROC on` or `# quiet PROC off`.  For a probe the test
    // repeats until the cluster answers it, whose number of rounds differs from
    // run to run: the golden holds what the probe decided and not how often it
    // asked.
    void ProcQuiet(atf_comp::FProc &proc, bool quiet);

    // Wait SECS seconds, scaled by the build as a comptest's budget is, and log
    // `# sleep SECS`.  For a window whose length is the assertion: a test that
    // requires something not to happen waits out the time in which it would have,
    // and the wait is a step of its own, visible in the golden.
    void Sleep(double secs);

    // Write MSG to the stdin of PROC as one line, $-substituted, trimmed and
    // ended by TERMINATOR, and log it first as `<proc> <- <msg>` with MSG as the
    // test wrote it, one log line per line of MSG.  A MSG of several lines goes out
    // in one write, for a protocol whose reader must see them arrive together.  A CRLF protocol passes "\r\n" as TERMINATOR.
    // A tool that rejects its command line exits before it reads fd 0, so the
    // write can reach a pipe whose read end is already gone.  What the tool did
    // with the line reaches the golden through the tool's own output and exit
    // code, and the line itself is recorded either way, so a broken pipe is the
    // tool's verdict rather than a harness failure and is not diagnosed as one.
    // Any other write error still aborts the test.
    // SIGPIPE is ignored only for the duration of the write: the disposition is
    // process-global and survives exec into every child atf_comp spawns later,
    // so leaving SIG_IGN set would silently change how the tools under test
    // react to their own broken pipes.
    void ProcWrite(atf_comp::FProc &proc, strptr msg, strptr terminator = "\n");

    // Send SIGNAL to the process group PROC leads, and report whether a live process
    // received it.  The one place that decides which pid a comptest's processes are
    // reached by, and usable from a signal handler because kill is its only call.
    bool ProcSignal(atf_comp::FProc &proc, int signal);

    // Send SIGNAL to every process group this run created.
    void ProcSignalAll(int signal);

    // Send SIGNAL to the process group PROC leads, which holds the shell the
    // command runs under together with any tool that shell forked.  Records the kill
    // so ProcWait reports the status as -1 rather than an exit code.
    void ProcKill(atf_comp::FProc &proc, int signal);

    // Close process stdin (signal EOF)
    void ProcWriteEof(atf_comp::FProc &proc);

    // Read the output of PROC until the text UNTIL appears in it, and return
    // everything read.  Each line is logged as `<proc> -> <line>`.  An empty UNTIL
    // reads to end of file.  End of file before UNTIL fails the test with
    // atf_comp.eof_before: a read the process never answered asserted nothing.  A test timeout
    // during the read aborts the test; with KILL_ON_TIMEOUT it ends PROC instead --
    // SIGTERM, then SIGKILL after five seconds -- and the read goes on to the end
    // of file the kill produces.
    tempstr ProcRead(atf_comp::FProc &proc, strptr until, bool kill_on_timeout = false);

    // Take the variables the test's cluster wrote about itself into the replacement
    // scope.
    //
    // A cluster picks its own ports and addresses as it starts, so a test script
    // cannot name them ahead of time; the cluster writes what it chose to
    // data/atfdb/var.ssim under its instance directory, and from there each becomes
    // a $-substitution the script can use.  Which directory that is comes from the
    // atfdb.testenv row marked vardir -- the same row that told the cluster where
    // to put its instance in the first place, so the two cannot disagree.  A tree
    // with no such row takes no variables.  Call it once the cluster is ready,
    // which is when the file is complete.
    void LoadClusterVar();

    // Wait for the cluster under PROC to print READY_FOR_TEST, then take the
    // variables that cluster wrote about itself into the replacement scope
    // (LoadClusterVar).
    void WaitReadyForTest(atf_comp::FProc &proc);

    // Wait for process to exit; verify it exited with EXPECTED_EXIT (default 0).
    // -1 means the process was killed (via ProcKill).  Procs that are never
    // passed to ProcWait are not checked — ProcWaitAll drains them silently.
    void ProcWait(atf_comp::FProc &proc, int expected_exit = 0);

    // Get process exit code
    int ProcStatus(atf_comp::FProc &proc);

    // Wait for all procs in reverse order; do not enforce exit codes here.
    // Tests assert exit codes with explicit ProcWait; this is the cleanup sweep.
    void ProcWaitAll();

    // Begin part NAME of a comptest that runs several checks on one cluster: wait
    // for every process the previous part started, then log `# part NAME`.
    //
    // A merged comptest starts its cluster once and runs one part per theme on it.
    // A part that left a client connected would carry that client into the next
    // part, where it could receive what the next part publishes, and its end lines
    // would land wherever the test's final sweep falls.  So each part ends where the
    // next begins.  The first call of a test waits for nothing, since the processes
    // already running then are the cluster the parts share.
    void Part(strptr name);

    // Compose the deny-write scaffold: a bash line that prepares the test's
    // tempdir with a read-only directory ro/ and runs TOOL against it, so the
    // tool's attempt to rewrite a file under ro/ fails with EACCES and the test
    // can pin the failure report. SEED creates the fixture file(s) under ro/
    // before the directory is locked.
    //
    // The mode must be restored after the run: a directory left at 0555 cannot
    // be removed by the next run's tempdir cleanup, so one interrupted run would
    // turn every following run of the test red. The EXIT trap restores the mode
    // on normal exit and, via the TERM trap, when the harness timeout kill sends
    // SIGTERM; the SIGKILL that follows 5s later still leaves the directory
    // locked -- unfixable, the shell gets no chance to run anything.
    //
    // chmod-based write denial assumes an unprivileged runner: under
    // CAP_DAC_OVERRIDE (e.g. a root container) the write succeeds and every
    // deny-write golden mismatches. CI uses shell executors today, which run
    // unprivileged.
    tempstr DenyWriteCmd(strptr seed, strptr tool);

    // Return the command that runs the toolchain BIN, taken from environment
    // variable ENVVAR where that is set and from the path otherwise, or an empty
    // string when BIN is nowhere.
    //
    // A toolchain is optional, as it is to the native citests.  A comptest runs what
    // needs one -- the Rust and Go clients, the bpf compiler -- only when it is
    // installed, and it does so out of the test log, so the golden holds what runs on
    // every machine and a machine without the toolchain passes the same golden.  What
    // those steps print is checked in the test itself.
    tempstr GetNativeTool(algo::strptr envvar, algo::strptr bin);

    // Run CMD, with the test's variables substituted, out of the test log, and
    // return what it printed on stdout and stderr.  A command that fails fails the
    // test, naming CMD and its output.
    tempstr EvalUnlogged(algo::strptr cmd);

    // -------------------------------------------------------------------
    // cpp/atf_comp/src_func.cpp -- Comptests for src_func
    //

    // Run src_func -updateproto against a header in a directory that denies the write, and
    // check that the update-hdr block rewrite fails the run and names the path.  The seed
    // puts junk inside the update-hdr block, so the rewrite has a change to make.  An exit
    // code of 0 would leave the header stale with no word of it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_src_func_FileWriteFail(); // atfdb.comptest:src_func.FileWriteFail

    // Run src_func over a targsrc source file that does not exist, and check that the scan
    // fails the run and names the path.  A scan that indexed the file as empty and exited
    // 0 would drop its functions from every listing and header with no word of it.
    // void comptest_src_func_FileReadFail(); // atfdb.comptest:src_func.FileReadFail

    // Run src_func -updateproto against a targsrc header that does not exist, and check
    // that the rewrite pass fails the run and names the path.  An exit code of 0 would
    // leave the header's update-hdr block stale with no word of it.
    // void comptest_src_func_UpdateprotoReadFail(); // atfdb.comptest:src_func.UpdateprotoReadFail

    // Run src_func -createmissing with a targsrc source file that does not exist, and
    // check that it skips the stub-creation pass entirely:
    // 1. Write src/y.cpp with xx::ExistingFunc, and copy it aside.
    // 2. Run src_func -createmissing over a universe that declares userfunc xx.NewFunc
    // with no scanned definition.
    // 3. Check that the run exits with code 1 and that src/y.cpp stays byte-unchanged
    // (cmp).
    // The failed scan leaves the function index incomplete.  An ungated pass would append
    // a stub for xx.NewFunc to the healthy source, duplicating a definition that the scan
    // never saw.
    // void comptest_src_func_CreatemissingScanFail(); // atfdb.comptest:src_func.CreatemissingScanFail

    // Run src_func -updateproto against a header whose second update-hdr section is
    // unterminated after the first one closed.  Check that the run names the file and
    // exits with code 1, and that the header stays byte-unchanged (cmp).  A rewrite would
    // drop everything from the unterminated marker to EOF and write the truncation back.
    // void comptest_src_func_UpdateprotoUnterminated(); // atfdb.comptest:src_func.UpdateprotoUnterminated

    // Run src_func -updateproto over a targsrc source that ends inside a function, with no
    // closing brace at column 0.  Check that the run exits with code 1 and that the header
    // stays byte-unchanged (cmp).  The scan is incomplete, so src_func must fail the run
    // and skip the header rewrite.  A rewrite would drop the truncated file's functions
    // from the header.
    // void comptest_src_func_UnfinishedFunc(); // atfdb.comptest:src_func.UnfinishedFunc

    // Scan a function whose template parameter list holds the parenthesized default
    // argument 'bool B = (4 > 2)' next to a function-pointer parameter, beside a plain
    // fn-ptr template.  Print the index rows, and check that they key the functions as
    // xx.Paren and xx.Fnptr.  If the '>' inside the parens ended the template-list strip
    // early, the name extraction would fall into the fn-ptr paren and key the function as
    // the bare namespace.
    // void comptest_src_func_TemplateParenDefault(); // atfdb.comptest:src_func.TemplateParenDefault

    // Scan one function per return-type shape with src_func -iffy -list, and check which
    // functions the iffy 'bigret' check reports, as the table below says.  The check
    // reports a function that returns cstring by value.  It reads the return type as
    // whatever stands left of the parameter list, and finds the parameter list as the
    // first paren on the line.  A template parameter list can carry parens of its own, as
    // the function-pointer parameter in 'template<typename T, void (*F)(T)>' does.  So the
    // paren search has to run on the line with the template list stripped.  Read from the
    // raw line, the type comes out of the template list.  The check then fires on sound
    // functions and stays silent on faulty ones.  The fixture holds the whole table of
    // return-type shapes, controls next to rejects, in source order:
    //
    // signature shape                                | expected
    // ------------------------------------------------ --------
    // cstring ret                                    | bigret
    // tempstr ret                                    | silent
    // cstring & ret                                  | silent
    // cstring * ret                                  | silent
    // template<class T> cstring ret                  | bigret
    // template<typename T, void (*F)(T)> cstring ret  | bigret
    // template<cstring (*F)(int)> void ret           | silent
    // template<class T, int *P> cstring ret          | bigret
    // template<typename T, void (*F)(T)> cstring & ret | silent
    // void comptest_src_func_TemplateIffyRet(); // atfdb.comptest:src_func.TemplateIffyRet

    // Run src_func -updateproto with a targsrc source file that does not exist, and check
    // that the run exits with code 1 and leaves the header byte-unchanged (cmp).  The seed
    // puts a prototype inside the header's update-hdr block.  The failed scan leaves the
    // function index incomplete, so src_func must skip the header rewrite entirely.  A
    // rewrite from the incomplete index would strip the prototype.
    // void comptest_src_func_UpdateprotoScanFail(); // atfdb.comptest:src_func.UpdateprotoScanFail

    // Load gendb.cppsym keys across the whole 250-character width, and check that each
    // key stays its own row:
    // 1. Write four functions to src/y.cpp.
    // 2. Run src_func -printssim over cppsym_width.ssim, which holds a short key, two keys
    // that agree for their first fifty characters and differ past it, and a key at the
    // 250-character limit.  Check that all four load as distinct rows, and that each
    // binds to its own scanned function and prints with its own acrkey.
    // 3. Run src_func over cppsym_toolong.ssim, whose key runs one character over the
    // limit.  Check that src_func rejects the key naming the limit, and exits with
    // code 1.
    // A gendb.cppsym key is a C++ name and a record key, <cppname>/<acrkey>, and the field
    // is 250 characters wide.  Two user functions whose names agree for a long stretch must
    // stay two rows.  Otherwise one registration disappears, and its function loses the
    // record it binds to.
    // void comptest_src_func_CppsymKeyWidth(); // atfdb.comptest:src_func.CppsymKeyWidth

    // Scan one function per comment-line shape with src_func -updateproto, print the
    // header, and check which of each function's comment lines the header mirrors, as the
    // table below says.  The fixture holds the shapes in source order.
    // The comment lines above a function belong to that function.  The header's update-hdr
    // block mirrors them.  The divider line this repo puts between functions belongs to
    // no function, so it ends the comment accumulated above it.  A comment line qualifies
    // as a divider by its whole form, and a substring of it never suffices.  An ascii
    // table inside a leading comment draws a rule line out of dashes.  Read as a divider,
    // that rule drops everything above the table from the mirrored header with no word of
    // it: the paragraph saying what the function does, and the table's own header row.  So
    // a rule line breaks its dashes with some other character, as every table in the tree
    // does.  The last function of the fixture pins the boundary, because a rule line of
    // dashes alone still reads as a divider.
    //
    // A banner of slashes is the same rule read over the other divider this repo draws.
    // The fixture writes it both as a bare run of slashes and as a comment that holds one,
    // `// ` and then the run.  The two banners in the tree use the second form.  A run of
    // slashes with a word among them is a section title.  It is comment text for the
    // reason a rule line broken by a word is.
    //
    // comment line in the middle of a comment  | mirrored
    // ------------------------------------------ ------------------
    // none, a one-line comment                 | the line
    // a divider of dashes                      | below the divider
    // a banner of slashes                      | below the banner
    // a banner of slashes behind a `//`        | below the banner
    // a slash run under the seventeen bound    | every line
    // a slash run around a word                | every line
    // a table rule of two dash runs            | every line
    // a section marker, dashes around a word   | every line
    // a dash run under the twenty-dash bound   | every line
    // a table rule broken by a pipe            | every line
    // a table rule of dashes alone             | below the rule
    // void comptest_src_func_Precomment(); // atfdb.comptest:src_func.Precomment

    // The rule that locates the closing brace of an update-hdr section, over one
    // header carrying every shape it has to place. A section closes on the lines
    // that return the body to the nesting level the marker opened, whatever else
    // those lines carry, so the fixture closes six sections that way: a bare brace,
    // a brace with a trailing line comment, a brace with a trailing block comment,
    // a brace indented with a tab where the marker was not indented at all, a `};`
    // -- the namespace brace followed by an empty declaration, which is legal C++
    // -- and a marker opening two namespaces whose two braces stand on one line.
    // A seventh opens two namespaces and closes them on two separate lines, so the
    // golden pins that both come back: a rule keeping only the line that reaches
    // the marker's level returns a header one brace short of compiling. Two further
    // sections open several namespaces and stand hand-written lines between the
    // closes -- a struct after the first of two closes, and a declaration followed by
    // a struct after the first of three. Those lines are below the section, so the
    // golden pins them coming back verbatim next to a single prototype block: a
    // section that stayed open past its first close reads the struct's opening brace
    // as body and drops it, then takes the struct's own `};` for a second close and
    // writes the prototypes again, at exit 0.
    // No section body here carries a brace, since a body that does holds hand-written
    // code and is refused (src_func.SectionNested). Three sections hold braces that
    // nest no C++ and must not be counted: one inside a line comment, one inside a
    // one-line block comment, one inside a block comment that spans lines, one inside
    // a line comment standing after a declaration, and braces inside a string
    // literal, a character literal and a literal carrying an escaped quote. The last
    // of them stands on the same line as a digit-separated constant, whose quotes are
    // not literal quotes.
    // Three more sections hold the braces of a character literal that carries an
    // encoding prefix -- L, u, U and u8 -- because the prefix is an identifier byte
    // standing where a number's digit would stand, and the last of them stands after
    // a string literal on the same line. A body line whose literal brace is counted
    // either refuses the whole file or closes the section on a line that does not
    // begin with a brace, which refuses it too, so a run that miscounts leaves this
    // golden with nothing rewritten.
    // Two more sections hold raw strings, whose braces nest no C++ either and among
    // whose bytes no backslash escapes anything. One carries them on single lines: a
    // brace inside a raw string, a raw string whose text holds an odd number of
    // quotes next to a `/*`, a delimited raw string whose text holds the undelimited
    // terminator, a raw string whose text holds a bare paren, each of the four
    // encoding prefixes, and -- as the control -- an ordinary string whose text
    // spells a raw string's opening. Reading the odd quote as the string's close
    // leaves the `/*` outside every literal, the block comment then never ends, and
    // the section never closes, so the whole file is refused at exit 1. The other
    // section opens a raw string that spans lines and holds an unpaired brace, which
    // is counted by any scan that does not carry the open literal from line to line.
    // Three more sections hold an ordinary quoted literal, which spans lines when a
    // backslash at the end of one escapes the newline. Two of them put an unpaired
    // brace in the continuation: an opening one, which leaves the section unable to
    // close and refuses the whole file, and a closing one, which ends the section in
    // the middle of a string and writes the prototypes there at exit 0. The third is
    // the control for the other half of the same rule -- a literal that reaches the
    // end of its line with no backslash is a header cut mid-string and closes there,
    // so the line below it is code.
    // One section is written over two lines, the namespace and marker on one
    // and the opening brace on the next, so the golden pins that the brace line comes
    // back: it is not part of what the section replaces, and a rewrite that dropped
    // it would return a namespace that is never opened at exit 0.
    // Everything below the last section is the control: it is outside every
    // section, so each of its lines must come back unchanged. The blank line
    // standing there is outside what the golden can see, because the component
    // harness logs no empty line, so whitespace below the last section is not
    // pinned and a regression that only moved a blank line would still pass.
    // void comptest_src_func_SectionClose(); // atfdb.comptest:src_func.SectionClose

    // Rewrite a header whose update-hdr sections cover the shapes a marker line can take,
    // and check which namespace's functions each section receives:
    // 1. Copy sectionns.hdr to hdr/x.h.  Write src/y.cpp with one function in every
    // namespace the markers name, one in each namespace that a reader of the raw bytes
    // would name, and one at global scope.
    // 2. Run src_func -updateproto, and print hdr/x.h.
    // 3. Check that four sections each come back holding exactly one prototype: one that
    // opens one namespace, one that opens two, one that opens three, and one whose
    // marker line opens none because its brace stands on the line below.  The function
    // in each section says which name src_func read the section as.
    // 4. Check the same for five sections that name their namespace in a spelling of its
    // own.  The spellings are the C++17 `namespace scope1::scope2 {`, whose innermost
    // component is the name; an attribute between the keyword and the name; an
    // attribute that carries a string argument; an attribute whose own name is
    // qualified; and a comment between the keyword and the name.
    // 5. Check the same for five sections that carry comment or literal text after the
    // name.  The texts are a comment that spells a call and a semicolon; a line comment
    // that mentions another namespace; a line comment whose last word is the keyword
    // itself; and a quoted and a raw string literal, each of which spells a namespace
    // line of its own.  The name each line gives its namespace stands outside that
    // text.
    // 6. Check that no section declares the functions in `deprecated`, `visibility`,
    // `run`, `rest` and `bar`, the namespaces a reader of the raw bytes names, or the
    // function at global scope.
    // A marker line may open more than one namespace, as in
    // `namespace outer { namespace inner {`.  The section's body then stands inside the
    // innermost of them.  The prototypes written there are the ones whose definition names
    // that namespace.  A definition inside outer::inner names inner.  So a run that reads
    // the marker's outermost name selects outer's functions and declares them inside
    // inner, while inner's own functions stay undeclared.  The header that the next build
    // reads then holds a prototype for a function defined nowhere.
    // The six functions of step 6 are the controls the spellings need.  A name read out of
    // an attribute or a remark selects that namespace's functions.  The section then
    // declares them inside a namespace they are not defined in.  A name the reader fails
    // to extract comes back empty.  An empty name selects the functions defined at global
    // scope, so the global function is a control for the same reason.
    // void comptest_src_func_SectionNs(); // atfdb.comptest:src_func.SectionNs

    // Rewrite a header whose update-hdr marker line names no namespace, and check that
    // src_func refuses to fill it:
    // 1. Write src/y.cpp with a function Global at global scope and named::Named.
    // 2. In stage anon, write `namespace { // update-hdr` to hdr/x.h, and run
    // src_func -updateproto.  Check that the run exits with code 1 and leaves the
    // header as it stands.
    // 3. In stage named, the control, write `namespace named { // update-hdr` to hdr/x.h,
    // and run src_func -updateproto.  Check that the run rewrites the header at exit 0,
    // and print it.
    // The prototypes a section receives are the functions whose definition names the
    // section's namespace.  A marker line whose name src_func fails to read has no such
    // set.  An empty name selects the functions defined at global scope.  The section
    // would then come back declaring those inside whatever the marker opened.  An
    // anonymous namespace is the shape of legal C++ that reaches this case.
    // void comptest_src_func_SectionUnnamed(); // atfdb.comptest:src_func.SectionUnnamed

    // Rewrite headers whose update-hdr marker line closes its own section or leaves open a
    // brace that none of its namespaces opened, and check that src_func refuses each:
    // 1. In stage selfclose, run src_func -updateproto on
    // `namespace foo { void A(); } // update-hdr`.  Check that the run exits with
    // code 1 and leaves the header byte-unchanged (cmp).
    // 2. In stage closeouter, do the same for a marker line that closes the namespace
    // enclosing it, `namespace inner { void A(); }} // update-hdr`.  This line ends one
    // level below where it began.
    // 3. In stage structopen, do the same for `namespace foo { struct S { // update-hdr`.
    // 4. In stage innerclosed, do the same for
    // `namespace one { namespace two { } // update-hdr`.
    // 5. In stage control, run src_func -updateproto on an ordinary section, and check
    // that src_func rewrites it at exit 0.
    // 6. In stage innerbrace, run it on `namespace foo { struct S {}; // update-hdr`, and
    // check that src_func rewrites the header at exit 0.  Print the header, and check
    // that the prototypes stand inside the section and the lines below it come back.
    // A section's prototypes go above the line that closes it.  A section that closes on
    // the line it opened leaves them no place, so src_func refuses to rewrite the header.
    // The shape earns a diagnostic of its own because of what a run does with a section
    // whose opening it never located.  The depth the opening reaches tells section body
    // from the lines below the section.  Without it, the run takes the first brace
    // anywhere further down the file for the section's own.  It drops the lines that nest
    // under that brace as body and writes the prototypes at its close.  For the shape of
    // step 2, the header comes back at exit 0 with a struct's field deleted and a
    // prototype block inside the struct.
    // src_func refuses the marker lines of steps 3 and 4 for a related reason, because
    // each leaves open a brace that none of its namespaces opened.  In step 3 the
    // section's depth is the struct's, so a run would locate the close on the struct's
    // `};`.  It would write the prototypes among the members and drop the members.  In
    // step 4 the line closes the inner namespace it names again.  The depth then stands
    // one level below the namespace whose functions the section receives.
    // The marker line of step 6 opens a brace pair and closes it again.  It is a section
    // one level deep with a body below it.  A rule that reads the marker line's deepest
    // point as its end refuses this shape, so the test prints the last stage's header.
    // void comptest_src_func_SectionBodyless(); // atfdb.comptest:src_func.SectionBodyless

    // The shapes where the depth an update-hdr section reaches and the text of the
    // line that reaches it disagree about where the section ends.
    // A section's lines are dropped as the file is copied, and which lines those are is
    // decided by one model of the file: the depth its braces reach. A brace the scan
    // counts is not always a brace the compiler reads as nesting -- `#define OPENBLOCK
    // {` spells a block opener that a macro's user closes somewhere else -- so the
    // line's own text is the second description of the same event. A section is closed
    // by the brace of the namespace its marker opened, and that brace is the first
    // thing on its line. Where the two descriptions disagree the file is refused rather
    // than rewritten, and the brace count reaching zero at end of file is the
    // whole-file form of the same test.
    // The fixture holds a stage per disagreement. A declaration sharing its line with
    // the namespace's brace is the shape that leaves a hand-written declaration inside
    // the region whose banner says it was scanned from sources, and a stray brace
    // standing after a section that was read correctly is refused too, which is what a
    // test on the whole file costs. A macro's brace inside the section is refused
    // before either test sees it, because a body line carrying a brace is hand-written
    // code (src_func.SectionNested). The last stage is the control: an ordinary section
    // with a hand-written line below it, rewritten at exit 0 and printed, so the golden
    // pins the prototype banner inside the section and the line below it standing.
    // Each refused stage pins the exit code and the header byte-unchanged (cmp).
    // void comptest_src_func_SectionUneven(); // atfdb.comptest:src_func.SectionUneven

    // A section whose body holds hand-written code that carries a brace, which the rewrite
    // would drop and no later run could write again.
    // The body of an update-hdr section is replaced by the banner and the prototypes,
    // and neither carries a brace. So a body line that carries one is hand-written,
    // and the section is refused with the file left as it stands. The stages are an
    // inline definition on one line, the same definition with its body on three lines
    // and its closing brace on a line of its own, a struct, a pair of macros that spell
    // a block's braces, a namespace the body opens, and a line that closes the marker's
    // namespace and opens another one, which ends level with where it began. Each
    // refused stage pins the exit code and the header byte-unchanged (cmp), since an
    // exit code alone says nothing about what was written. The last stage is the
    // control: the same header with the definition moved below the section, rewritten
    // at exit 0 and printed, so the golden shows the definition standing.
    // void comptest_src_func_SectionNested(); // atfdb.comptest:src_func.SectionNested

    // Rewrite one header that holds every shape the update-hdr section-open rule has to
    // judge, and check that only the real markers open a section:
    // 1. Copy sectionopen.hdr to hdr/x.h, and write src/y.cpp with requoted::Requoted.
    // 2. Run src_func -updateproto, and print hdr/x.h.
    // 3. Check that four look-alike markers open no section.  One stands inside a block
    // comment that spans lines, as a hand edit leaves it when it comments a section
    // out.  One has its tag inside a block comment on the namespace line itself.  One
    // opens a block comment that it never closes and carries the tag inside it, the
    // same hand edit written on one line.  One has its tag inside a string literal.  A
    // fifth shape, a marker behind a `//`, is a look-alike that the rule rejects too.
    // 4. Check that the lines with the tag inside a raw string open no section: one
    // closed on its own line, one closed on its own line with a delimiter, one that
    // spans lines and whose text holds a whole `namespace zz { // update-hdr` line,
    // and one whose namespace line opens a raw string and puts the tag inside it.
    // 5. Check that two look-alike markers after a `//` comment that ends in a backslash
    // open no section.  One stands one line below such a comment, and the other two
    // lines below one.
    // 6. Check that src_func rewrites the section of
    // `namespace requoted { /* was: // update-hdr */ // update-hdr`, which quotes the
    // marker in a remark and then writes the real one.  Check that the section holds
    // the prototype of requoted::Requoted under the banner of src/y.cpp.
    // 7. Check that src_func rewrites the section of a real marker whose own trailing
    // comment ends in a backslash.
    // 8. Check that every other line comes back byte for byte, including the lines that
    // the look-alikes enclose and the struct below them.
    // A line opens a section only when the compiler reads the namespace and the marker tag
    // as code.  Read as a marker, a look-alike opens a section that nobody meant to open.
    // The rewrite then drops every line from there to the next lone brace.
    // In step 4, a line-at-a-time scan reads the spanning raw string as a marker, because
    // the line that holds the tag begins inside the literal and looks like code by itself.
    // The tag's own test rejects the last raw-string shape, because a tag counts only in
    // the comment that ends its line.  A raw string is code, so the tag inside it stays
    // below the code that the line carries.
    // The marker of step 6 is the accepting side of the same rule, because its tag stands
    // in the trailing comment.  A rule that reads the first occurrence of the tag anywhere
    // on the line refuses this marker.  The section then stays stale at exit 0 with no
    // word of it.  Each of the two tags names a source, because the attributes come from
    // the same occurrence that the rule picks.  The remark names the header itself, which
    // defines no function.  The live tag names src/y.cpp, which defines one.  A run that
    // read the attributes from the quoted tag would come back with an empty section.
    // A `//` comment whose line ends in a backslash is the fourth kind of span that a line
    // can begin inside, because C++ splices lines before it recognizes comments.  The line
    // below such a comment belongs to it entire.  So does the line after that one, when
    // the line below also ends in a backslash.  The marker of step 7 is the accepting side
    // of this rule, because its namespace and tag are code.
    // void comptest_src_func_SectionOpen(); // atfdb.comptest:src_func.SectionOpen

    // Run src_func -updateproto on a header whose first update-hdr section never closes
    // and whose second one does.  Check that the run exits with code 1 and leaves the file
    // byte-unchanged (cmp).  The run ends with no section open and one section updated.  A
    // write gated on those two facts alone would save a copy that lost every line from the
    // first marker to the second, the second marker included.  The diagnostic printed
    // above it would still say that src_func left the file as it was.
    // void comptest_src_func_SectionOpenTwice(); // atfdb.comptest:src_func.SectionOpenTwice

    // -------------------------------------------------------------------
    // cpp/atf_comp/src_hdr.cpp -- Comptests for src_hdr
    //

    // Run src_hdr -write twice over a headerless script, and check that the second run
    // leaves the file byte-identical (second_run_noop).  The first run adds the header.
    // A rewrite that keeps changing its own output grows the header on every
    // update-hdr run.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_src_hdr_Converge(); // atfdb.comptest:src_hdr.Converge

    // Run src_hdr -write against a scriptfile row whose file does not exist, and check
    // that the run fails naming the path.  A run that skipped the file silently and
    // exited 0 would let a stale scriptfile row pass every run unnoticed.
    // void comptest_src_hdr_FileReadFail(); // atfdb.comptest:src_hdr.FileReadFail

    // Run src_hdr -write against a script whose directory denies the write, through the
    // DenyWriteCmd scaffold, and check that the failed header rewrite fails the run,
    // naming the path.  A run that exited 0 would leave the file silently unchanged.
    // void comptest_src_hdr_FileWriteFail(); // atfdb.comptest:src_hdr.FileWriteFail

    // Run src_hdr -write against a .json scriptfile, whose comment syntax is unknown.
    // Check that the run exits 1 naming the file, and that cmp finds the file
    // byte-unchanged (file_unchanged).
    // Every line of such a file would parse as header comment, and the rewrite would
    // replace the whole file with a bare separator.  An extensionless file with no
    // shebang to guess from has an unknown comment syntax too.
    // void comptest_src_hdr_NoCmtstring(); // atfdb.comptest:src_hdr.NoCmtstring

    // Run src_hdr -write with a stub bin/src_func that exits 1, so the header write
    // succeeds and the trailing src_func -updateproto step fails.  Check that the run
    // fails naming the step, because the update-hdr blocks stay stale.
    // void comptest_src_hdr_UpdateprotoFail(); // atfdb.comptest:src_hdr.UpdateprotoFail

    // -------------------------------------------------------------------
    // cpp/atf_comp/ssimfilt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_ssimfilt_Csv(); // atfdb.comptest:ssimfilt.Csv
    // void comptest_ssimfilt_CsvField(); // atfdb.comptest:ssimfilt.CsvField
    // void comptest_ssimfilt_FirstTag(); // atfdb.comptest:ssimfilt.FirstTag
    // void comptest_ssimfilt_Json(); // atfdb.comptest:ssimfilt.Json
    // void comptest_ssimfilt_JsonRecursive(); // atfdb.comptest:ssimfilt.JsonRecursive
    // void comptest_ssimfilt_MatchField(); // atfdb.comptest:ssimfilt.MatchField
    // void comptest_ssimfilt_MatchTag(); // atfdb.comptest:ssimfilt.MatchTag
    // void comptest_ssimfilt_SelectField(); // atfdb.comptest:ssimfilt.SelectField
    // void comptest_ssimfilt_Stable(); // atfdb.comptest:ssimfilt.Stable
    // void comptest_ssimfilt_Table(); // atfdb.comptest:ssimfilt.Table

    // -------------------------------------------------------------------
    // cpp/atf_comp/sv2ssim.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_sv2ssim_Convert1(); // atfdb.comptest:sv2ssim.Convert1
    // void comptest_sv2ssim_Convert1Signed(); // atfdb.comptest:sv2ssim.Convert1Signed
    // void comptest_sv2ssim_Convert2(); // atfdb.comptest:sv2ssim.Convert2
    // void comptest_sv2ssim_Convert2Tsv(); // atfdb.comptest:sv2ssim.Convert2Tsv
    // void comptest_sv2ssim_UniqueFieldName(); // atfdb.comptest:sv2ssim.UniqueFieldName

    // -------------------------------------------------------------------
    // cpp/atf_comp/wt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_wt_Anon(); // atfdb.comptest:wt.Anon
    // void comptest_wt_Clean(); // atfdb.comptest:wt.Clean
    // void comptest_wt_Command(); // atfdb.comptest:wt.Command
    // void comptest_wt_Create(); // atfdb.comptest:wt.Create
    // void comptest_wt_Del(); // atfdb.comptest:wt.Del
    // void comptest_wt_Diff(); // atfdb.comptest:wt.Diff
    // void comptest_wt_Hook(); // atfdb.comptest:wt.Hook
    // void comptest_wt_List(); // atfdb.comptest:wt.List
    // void comptest_wt_Merged(); // atfdb.comptest:wt.Merged
    // void comptest_wt_Reset(); // atfdb.comptest:wt.Reset
    // void comptest_wt_StdinMulti(); // atfdb.comptest:wt.StdinMulti
}
