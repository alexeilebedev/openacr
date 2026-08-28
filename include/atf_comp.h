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
    // void comptest_acr_BadInsert(); // gstatic/atfdb.comptest:acr.BadInsert

    // A line the tuple parser rejects (unterminated quote) is reported with its
    // file:line, fails the run, and blocks the -write, so the file still holds
    // every line it held -- including the one that did not load.
    // void comptest_acr_BadLine(); // gstatic/atfdb.comptest:acr.BadLine
    // void comptest_acr_BadNs(); // gstatic/atfdb.comptest:acr.BadNs

    // A cpbuf row naming a protobuf syntax that is not one of the two the codec
    // knows. cpbuf.syntax references the dmmeta.pbsyntax table, so the reference
    // check names the attribute and lists the syntax variants; a misspelling is
    // rejected here instead of silently selecting the proto2 wire shape.
    // The pbsyntax and ctype rows come in through -in so the suggestion does not
    // depend on the live database.
    // void comptest_acr_BadPbufSyntax(); // gstatic/atfdb.comptest:acr.BadPbufSyntax
    // void comptest_acr_BadPkey(); // gstatic/atfdb.comptest:acr.BadPkey
    // void comptest_acr_BadReftype(); // gstatic/atfdb.comptest:acr.BadReftype
    // void comptest_acr_CascDel(); // gstatic/atfdb.comptest:acr.CascDel
    // void comptest_acr_CascDel2(); // gstatic/atfdb.comptest:acr.CascDel2
    // void comptest_acr_CascDel3(); // gstatic/atfdb.comptest:acr.CascDel3
    // void comptest_acr_DelField(); // gstatic/atfdb.comptest:acr.DelField
    // void comptest_acr_DelRecord(); // gstatic/atfdb.comptest:acr.DelRecord
    // void comptest_acr_DeleteReinsert(); // gstatic/atfdb.comptest:acr.DeleteReinsert

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
    // void comptest_acr_DsetDirReadDeny(); // gstatic/atfdb.comptest:acr.DsetDirReadDeny

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
    // void comptest_acr_DsetFileReadDeny(); // gstatic/atfdb.comptest:acr.DsetFileReadDeny

    // acr -e whose editor modifies records in two different ssimfiles: the edit
    // is written back, the report's n_file_mod carries the per-file count, and
    // the process exits 0 -- the exit code means success or failure only, never
    // the number of files modified. The editor is a cp command that replaces the
    // edit buffer with the two pre-edited records.
    // void comptest_acr_EditFileModCount(); // gstatic/atfdb.comptest:acr.EditFileModCount

    // acr -e whose -in path cannot be loaded: the editor session is skipped and
    // the run exits nonzero. Without the gate, the editor opens over an empty
    // selection and the write-back then discards whatever the user typed into it.
    // The editor is a touch command whose marker file must not exist after the
    // run.
    // void comptest_acr_EditFileReadFail(); // gstatic/atfdb.comptest:acr.EditFileReadFail

    // acr -e whose write-back fails: the run exits nonzero with n_file_mod:0,
    // so a dropped write is distinguishable from a modifying edit -- only
    // failure exits nonzero. The editor inserts a record whose ssimfile write
    // is blocked by a file occupying the dmmeta directory path (the same
    // arrangement as the test/acr/writefail fixture).
    // void comptest_acr_EditFileWriteFail(); // gstatic/atfdb.comptest:acr.EditFileWriteFail
    // void comptest_acr_Fields(); // gstatic/atfdb.comptest:acr.Fields
    // void comptest_acr_FieldsComma(); // gstatic/atfdb.comptest:acr.FieldsComma

    // acr -insert -write of records landing in two different ssimfiles: the
    // report's n_file_mod must count each modified file -- it is the one
    // carrier of that count (amc's table write accumulates it into its own
    // files-modified count). The tempdir serves as an empty dataset; acr
    // creates the dmmeta/ directory under it on write.
    // void comptest_acr_FileModCount(); // gstatic/atfdb.comptest:acr.FileModCount

    // acr with an -in path that names neither an existing file nor a dataset
    // directory: the run must fail naming the path instead of loading nothing
    // and exiting 0 with n_select:0 -- a mistyped -in (or a wrong working
    // directory) would otherwise produce an answer computed over no data,
    // indistinguishable from a true empty result.
    // void comptest_acr_FileReadFail(); // gstatic/atfdb.comptest:acr.FileReadFail

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
    // void comptest_acr_FileReadDeny(); // gstatic/atfdb.comptest:acr.FileReadDeny

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
    // void comptest_acr_FileWriteFail(); // gstatic/atfdb.comptest:acr.FileWriteFail
    // void comptest_acr_GitTrigger1(); // gstatic/atfdb.comptest:acr.GitTrigger1
    // void comptest_acr_Insert(); // gstatic/atfdb.comptest:acr.Insert
    // void comptest_acr_InsertDelete(); // gstatic/atfdb.comptest:acr.InsertDelete
    // void comptest_acr_Merge(); // gstatic/atfdb.comptest:acr.Merge
    // void comptest_acr_Meta1(); // gstatic/atfdb.comptest:acr.Meta1
    // void comptest_acr_Meta2(); // gstatic/atfdb.comptest:acr.Meta2
    // void comptest_acr_Meta3(); // gstatic/atfdb.comptest:acr.Meta3
    // void comptest_acr_NullTrunc(); // gstatic/atfdb.comptest:acr.NullTrunc
    // void comptest_acr_QueryCtype(); // gstatic/atfdb.comptest:acr.QueryCtype
    // void comptest_acr_RenameCollision(); // gstatic/atfdb.comptest:acr.RenameCollision
    // void comptest_acr_RenameField(); // gstatic/atfdb.comptest:acr.RenameField
    // void comptest_acr_RenameRecord(); // gstatic/atfdb.comptest:acr.RenameRecord
    // void comptest_acr_Replace(); // gstatic/atfdb.comptest:acr.Replace
    // void comptest_acr_Select(); // gstatic/atfdb.comptest:acr.Select
    // void comptest_acr_SelectInProcSubst(); // gstatic/atfdb.comptest:acr.SelectInProcSubst
    // void comptest_acr_SelectStdin(); // gstatic/atfdb.comptest:acr.SelectStdin
    // void comptest_acr_SelectTree(); // gstatic/atfdb.comptest:acr.SelectTree
    // void comptest_acr_TooManyArgs(); // gstatic/atfdb.comptest:acr.TooManyArgs
    // void comptest_acr_Trunc(); // gstatic/atfdb.comptest:acr.Trunc

    // A parsed tuple naming a ctype acr does not know is reported, fails the
    // run, and blocks the -write; the blank and comment-only lines in the same
    // file stay accepted and draw no report.
    // void comptest_acr_UnknownCtype(); // gstatic/atfdb.comptest:acr.UnknownCtype
    // void comptest_acr_UpdateBad(); // gstatic/atfdb.comptest:acr.UpdateBad
    // void comptest_acr_UpdateGood(); // gstatic/atfdb.comptest:acr.UpdateGood
    // void comptest_acr_Where(); // gstatic/atfdb.comptest:acr.Where

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_compl.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_compl_A01a(); // gstatic/atfdb.comptest:acr_compl.A01a
    // void comptest_acr_compl_A01b(); // gstatic/atfdb.comptest:acr_compl.A01b
    // void comptest_acr_compl_A01c(); // gstatic/atfdb.comptest:acr_compl.A01c
    // void comptest_acr_compl_A02a(); // gstatic/atfdb.comptest:acr_compl.A02a
    // void comptest_acr_compl_A02b(); // gstatic/atfdb.comptest:acr_compl.A02b
    // void comptest_acr_compl_A02c(); // gstatic/atfdb.comptest:acr_compl.A02c
    // void comptest_acr_compl_A03a(); // gstatic/atfdb.comptest:acr_compl.A03a
    // void comptest_acr_compl_A03b(); // gstatic/atfdb.comptest:acr_compl.A03b
    // void comptest_acr_compl_A03c(); // gstatic/atfdb.comptest:acr_compl.A03c
    // void comptest_acr_compl_A04a(); // gstatic/atfdb.comptest:acr_compl.A04a
    // void comptest_acr_compl_A04b(); // gstatic/atfdb.comptest:acr_compl.A04b
    // void comptest_acr_compl_A04c(); // gstatic/atfdb.comptest:acr_compl.A04c
    // void comptest_acr_compl_A05a(); // gstatic/atfdb.comptest:acr_compl.A05a
    // void comptest_acr_compl_A05b(); // gstatic/atfdb.comptest:acr_compl.A05b
    // void comptest_acr_compl_A05c(); // gstatic/atfdb.comptest:acr_compl.A05c
    // void comptest_acr_compl_A06a(); // gstatic/atfdb.comptest:acr_compl.A06a
    // void comptest_acr_compl_A06b(); // gstatic/atfdb.comptest:acr_compl.A06b
    // void comptest_acr_compl_A06c(); // gstatic/atfdb.comptest:acr_compl.A06c
    // void comptest_acr_compl_A07a(); // gstatic/atfdb.comptest:acr_compl.A07a
    // void comptest_acr_compl_A07b(); // gstatic/atfdb.comptest:acr_compl.A07b
    // void comptest_acr_compl_A07c(); // gstatic/atfdb.comptest:acr_compl.A07c
    // void comptest_acr_compl_Acr01(); // gstatic/atfdb.comptest:acr_compl.Acr01
    // void comptest_acr_compl_Acr02(); // gstatic/atfdb.comptest:acr_compl.Acr02
    // void comptest_acr_compl_Acr03(); // gstatic/atfdb.comptest:acr_compl.Acr03
    // void comptest_acr_compl_Acr04(); // gstatic/atfdb.comptest:acr_compl.Acr04
    // void comptest_acr_compl_Acr05(); // gstatic/atfdb.comptest:acr_compl.Acr05
    // void comptest_acr_compl_Acr06(); // gstatic/atfdb.comptest:acr_compl.Acr06
    // void comptest_acr_compl_Acr07(); // gstatic/atfdb.comptest:acr_compl.Acr07
    // void comptest_acr_compl_Acr08(); // gstatic/atfdb.comptest:acr_compl.Acr08
    // void comptest_acr_compl_Acr09(); // gstatic/atfdb.comptest:acr_compl.Acr09
    // void comptest_acr_compl_Acr10(); // gstatic/atfdb.comptest:acr_compl.Acr10
    // void comptest_acr_compl_Acr11(); // gstatic/atfdb.comptest:acr_compl.Acr11
    // void comptest_acr_compl_Acr12(); // gstatic/atfdb.comptest:acr_compl.Acr12
    // void comptest_acr_compl_Acr13(); // gstatic/atfdb.comptest:acr_compl.Acr13
    // void comptest_acr_compl_BadExe(); // gstatic/atfdb.comptest:acr_compl.BadExe
    // void comptest_acr_compl_BadOpt(); // gstatic/atfdb.comptest:acr_compl.BadOpt
    // void comptest_acr_compl_BadOptColon(); // gstatic/atfdb.comptest:acr_compl.BadOptColon
    // void comptest_acr_compl_BadOptColonSpace(); // gstatic/atfdb.comptest:acr_compl.BadOptColonSpace
    // void comptest_acr_compl_BadOptSpace(); // gstatic/atfdb.comptest:acr_compl.BadOptSpace
    // void comptest_acr_compl_Bare(); // gstatic/atfdb.comptest:acr_compl.Bare
    // void comptest_acr_compl_DblColon(); // gstatic/atfdb.comptest:acr_compl.DblColon
    // void comptest_acr_compl_DblColonList(); // gstatic/atfdb.comptest:acr_compl.DblColonList
    // void comptest_acr_compl_DblSpace(); // gstatic/atfdb.comptest:acr_compl.DblSpace
    // void comptest_acr_compl_DblSpaceList(); // gstatic/atfdb.comptest:acr_compl.DblSpaceList
    // void comptest_acr_compl_EnumCtypeColon(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeColon
    // void comptest_acr_compl_EnumCtypeColonList(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeColonList
    // void comptest_acr_compl_EnumCtypeSpace(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeSpace
    // void comptest_acr_compl_EnumCtypeSpaceList(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeSpaceList
    // void comptest_acr_compl_EnumFieldColon(); // gstatic/atfdb.comptest:acr_compl.EnumFieldColon
    // void comptest_acr_compl_EnumFieldColonList(); // gstatic/atfdb.comptest:acr_compl.EnumFieldColonList
    // void comptest_acr_compl_EnumFieldSpace(); // gstatic/atfdb.comptest:acr_compl.EnumFieldSpace
    // void comptest_acr_compl_EnumFieldSpaceList(); // gstatic/atfdb.comptest:acr_compl.EnumFieldSpaceList
    // void comptest_acr_compl_FlagColon(); // gstatic/atfdb.comptest:acr_compl.FlagColon
    // void comptest_acr_compl_FlagColonList(); // gstatic/atfdb.comptest:acr_compl.FlagColonList
    // void comptest_acr_compl_FlagSpace(); // gstatic/atfdb.comptest:acr_compl.FlagSpace
    // void comptest_acr_compl_FlagSpaceList(); // gstatic/atfdb.comptest:acr_compl.FlagSpaceList
    // void comptest_acr_compl_Install(); // gstatic/atfdb.comptest:acr_compl.Install
    // void comptest_acr_compl_NumColon(); // gstatic/atfdb.comptest:acr_compl.NumColon
    // void comptest_acr_compl_NumColonList(); // gstatic/atfdb.comptest:acr_compl.NumColonList
    // void comptest_acr_compl_NumSpace(); // gstatic/atfdb.comptest:acr_compl.NumSpace
    // void comptest_acr_compl_NumSpaceList(); // gstatic/atfdb.comptest:acr_compl.NumSpaceList
    // void comptest_acr_compl_OptCumul(); // gstatic/atfdb.comptest:acr_compl.OptCumul
    // void comptest_acr_compl_OptCumulAlias(); // gstatic/atfdb.comptest:acr_compl.OptCumulAlias
    // void comptest_acr_compl_OptD(); // gstatic/atfdb.comptest:acr_compl.OptD
    // void comptest_acr_compl_OptDList(); // gstatic/atfdb.comptest:acr_compl.OptDList
    // void comptest_acr_compl_OptH(); // gstatic/atfdb.comptest:acr_compl.OptH
    // void comptest_acr_compl_OptHList(); // gstatic/atfdb.comptest:acr_compl.OptHList
    // void comptest_acr_compl_OptNonCumul(); // gstatic/atfdb.comptest:acr_compl.OptNonCumul
    // void comptest_acr_compl_OptNonCumulAlias(); // gstatic/atfdb.comptest:acr_compl.OptNonCumulAlias
    // void comptest_acr_compl_OptPkeyColon(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColon
    // void comptest_acr_compl_OptPkeyColonFull(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonFull
    // void comptest_acr_compl_OptPkeyColonFullList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonFullList
    // void comptest_acr_compl_OptPkeyColonList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonList
    // void comptest_acr_compl_OptPkeyColonPrefix(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonPrefix
    // void comptest_acr_compl_OptPkeyColonPrefixList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonPrefixList
    // void comptest_acr_compl_OptPkeyColonSubstr(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonSubstr
    // void comptest_acr_compl_OptPkeyColonSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonSubstrList
    // void comptest_acr_compl_OptPkeySpace(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpace
    // void comptest_acr_compl_OptPkeySpaceFull(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceFull
    // void comptest_acr_compl_OptPkeySpaceFullList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceFullList
    // void comptest_acr_compl_OptPkeySpaceList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceList
    // void comptest_acr_compl_OptPkeySpacePrefix(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpacePrefix
    // void comptest_acr_compl_OptPkeySpacePrefixList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpacePrefixList
    // void comptest_acr_compl_OptPkeySpaceSubstr(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceSubstr
    // void comptest_acr_compl_OptPkeySpaceSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceSubstrList
    // void comptest_acr_compl_OptRegxColon(); // gstatic/atfdb.comptest:acr_compl.OptRegxColon
    // void comptest_acr_compl_OptRegxColonFull(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonFull
    // void comptest_acr_compl_OptRegxColonFullList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonFullList
    // void comptest_acr_compl_OptRegxColonList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonList
    // void comptest_acr_compl_OptRegxColonPrefix(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonPrefix
    // void comptest_acr_compl_OptRegxColonPrefixList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonPrefixList
    // void comptest_acr_compl_OptRegxColonSubstr(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonSubstr
    // void comptest_acr_compl_OptRegxColonSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonSubstrList
    // void comptest_acr_compl_OptRegxSpace(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpace
    // void comptest_acr_compl_OptRegxSpaceFull(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceFull
    // void comptest_acr_compl_OptRegxSpaceFullList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceFullList
    // void comptest_acr_compl_OptRegxSpaceList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceList
    // void comptest_acr_compl_OptRegxSpacePrefix(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpacePrefix
    // void comptest_acr_compl_OptRegxSpacePrefixList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpacePrefixList
    // void comptest_acr_compl_OptRegxSpaceSubstr(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceSubstr
    // void comptest_acr_compl_OptRegxSpaceSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceSubstrList
    // void comptest_acr_compl_OptSig(); // gstatic/atfdb.comptest:acr_compl.OptSig
    // void comptest_acr_compl_OptSigList(); // gstatic/atfdb.comptest:acr_compl.OptSigList
    // void comptest_acr_compl_OptV(); // gstatic/atfdb.comptest:acr_compl.OptV
    // void comptest_acr_compl_OptVList(); // gstatic/atfdb.comptest:acr_compl.OptVList
    // void comptest_acr_compl_R01a(); // gstatic/atfdb.comptest:acr_compl.R01a
    // void comptest_acr_compl_R01b(); // gstatic/atfdb.comptest:acr_compl.R01b
    // void comptest_acr_compl_R01c(); // gstatic/atfdb.comptest:acr_compl.R01c
    // void comptest_acr_compl_R01d(); // gstatic/atfdb.comptest:acr_compl.R01d
    // void comptest_acr_compl_R02a(); // gstatic/atfdb.comptest:acr_compl.R02a
    // void comptest_acr_compl_R02b(); // gstatic/atfdb.comptest:acr_compl.R02b
    // void comptest_acr_compl_R02c(); // gstatic/atfdb.comptest:acr_compl.R02c
    // void comptest_acr_compl_R02d(); // gstatic/atfdb.comptest:acr_compl.R02d
    // void comptest_acr_compl_StrColon(); // gstatic/atfdb.comptest:acr_compl.StrColon
    // void comptest_acr_compl_StrColonList(); // gstatic/atfdb.comptest:acr_compl.StrColonList
    // void comptest_acr_compl_StrSpace(); // gstatic/atfdb.comptest:acr_compl.StrSpace
    // void comptest_acr_compl_StrSpaceList(); // gstatic/atfdb.comptest:acr_compl.StrSpaceList
    // void comptest_acr_compl_T01(); // gstatic/atfdb.comptest:acr_compl.T01
    // void comptest_acr_compl_T02(); // gstatic/atfdb.comptest:acr_compl.T02
    // void comptest_acr_compl_T03(); // gstatic/atfdb.comptest:acr_compl.T03
    // void comptest_acr_compl_T04(); // gstatic/atfdb.comptest:acr_compl.T04
    // void comptest_acr_compl_T05(); // gstatic/atfdb.comptest:acr_compl.T05
    // void comptest_acr_compl_T06(); // gstatic/atfdb.comptest:acr_compl.T06
    // void comptest_acr_compl_T07(); // gstatic/atfdb.comptest:acr_compl.T07
    // void comptest_acr_compl_T08(); // gstatic/atfdb.comptest:acr_compl.T08
    // void comptest_acr_compl_T09(); // gstatic/atfdb.comptest:acr_compl.T09
    // void comptest_acr_compl_T10(); // gstatic/atfdb.comptest:acr_compl.T10

    // valid command line with named option
    // void comptest_acr_compl_CheckValid(); // gstatic/atfdb.comptest:acr_compl.CheckValid

    // unknown option
    // void comptest_acr_compl_CheckUnknownOpt(); // gstatic/atfdb.comptest:acr_compl.CheckUnknownOpt

    // unknown command
    // void comptest_acr_compl_CheckUnknownCmd(); // gstatic/atfdb.comptest:acr_compl.CheckUnknownCmd

    // batch-mode validation: pipe acr_compl.checkreq rows in via stdin;
    // expect one acr_compl.checkerr per failing line, silence on success,
    // and a non-zero exit code if any line failed.
    // void comptest_acr_compl_CheckBatch(); // gstatic/atfdb.comptest:acr_compl.CheckBatch

    // batch-mode validation of a line whose -- ends its named options: every
    // word after -- is a positional value of that line, so g++ and -c are not
    // checked against gcache's option names. The next line in the batch is
    // parsed from scratch and its unknown option is still reported -- an anon
    // flag left set from the previous line would route every word of every
    // later line down the positional branch, which never reaches the
    // unknown-option check, and a whole documentation corpus would validate
    // clean after one line carrying --.
    // void comptest_acr_compl_CheckBatchAnon(); // gstatic/atfdb.comptest:acr_compl.CheckBatchAnon

    // batch-mode validation fed a request row it cannot parse: an unknown attr
    // (a version-skewed producer) and a line that is not a checkreq tuple at
    // all. A dropped request is indistinguishable from one that validated
    // clean, so every unparsed line is reported and fails the run; the
    // surrounding requests are still validated, and an empty line carries no
    // request and is not an error.
    // void comptest_acr_compl_CheckBatchBadReq(); // gstatic/atfdb.comptest:acr_compl.CheckBatchBadReq

    // batch-mode validation of consecutive lines naming different commands. Each
    // line is checked against its own command's option names, so -write, which
    // belongs to acr and not to amc, is reported on the amc line even though an
    // earlier line used it legally. An option-name index accumulated across the
    // batch would hold the union of every command seen so far and accept the
    // flag. A bare command name with no argument leaves a completion offer behind
    // it; a repeat of that line must be parsed against an empty completion set,
    // since the leading offer of the previous line names no option field.
    // void comptest_acr_compl_CheckBatchCmd(); // gstatic/atfdb.comptest:acr_compl.CheckBatchCmd

    // batch-mode validation started from a shell that exported the bash
    // completion environment. COMP_LINE names the line bash asked to complete and
    // says nothing about the command line this process was given, so a validator
    // spawned from such a shell inherits it. Read as the entrypoint selector, it
    // discarded the process's own argv: -check_batch was never parsed, fd 0 was
    // never read, and the run exited 0, which the caller reads as every queued
    // command validated clean. The command line decides what runs; the inherited
    // variable does not.
    // void comptest_acr_compl_CheckBatchCompLine(); // gstatic/atfdb.comptest:acr_compl.CheckBatchCompLine

    // batch-mode validation asked to read the schema or the data from stdin as
    // well: the requests and the ssim tuples would both arrive on fd 0, and the
    // loader that runs first consumes the whole stream, leaving the batch with
    // zero requests to validate. The combination is rejected before either
    // reader opens fd 0, so a batch is never reported clean because the schema
    // loader ate its requests.
    // void comptest_acr_compl_CheckBatchSchemaStdin(); // gstatic/atfdb.comptest:acr_compl.CheckBatchSchemaStdin

    // batch-mode validation asked to print the completion installation script as
    // well: the `complete` command goes to the same stdout that carries the
    // response rows, ahead of them and not a response row, and a consumer reading
    // the stream as tuples has no way to tell the difference. The combination is
    // rejected, so nothing but responses ever reaches the response stream.
    // void comptest_acr_compl_CheckBatchInstall(); // gstatic/atfdb.comptest:acr_compl.CheckBatchInstall

    // batch-mode validation asked to check a single line as well: -check and
    // -check_batch are two validation modes, only one of them runs, and the
    // single-line check would be dropped without a word -- a validation silently
    // not performed is what this mode exists to prevent. The combination is
    // rejected instead.
    // void comptest_acr_compl_CheckBatchCheck(); // gstatic/atfdb.comptest:acr_compl.CheckBatchCheck

    // valid command with boolean flag
    // void comptest_acr_compl_CheckValidFlag(); // gstatic/atfdb.comptest:acr_compl.CheckValidFlag

    // valid command with anonymous arg
    // void comptest_acr_compl_CheckValidAnon(); // gstatic/atfdb.comptest:acr_compl.CheckValidAnon

    // valid command with multiple options
    // void comptest_acr_compl_CheckMultiOpt(); // gstatic/atfdb.comptest:acr_compl.CheckMultiOpt

    // dmmeta.finsertwhen: composite Pkey flag falls through to the existing-pkey
    // lookup when its trigger flag is absent
    // void comptest_acr_compl_InsertWhenLookup(); // gstatic/atfdb.comptest:acr_compl.InsertWhenLookup

    // trigger flag present, empty value: insert mode offers the LEFT component
    // (flower) values, each suffixed with the / separator
    // void comptest_acr_compl_InsertWhenLeft(); // gstatic/atfdb.comptest:acr_compl.InsertWhenLeft

    // trigger flag present, value contains the separator: insert mode offers
    // the RIGHT component (fruit) values, with the typed left half as a prefix
    // void comptest_acr_compl_InsertWhenRight(); // gstatic/atfdb.comptest:acr_compl.InsertWhenRight

    // trigger flag present, partially-typed left component: insert mode filters
    // flowers by prefix
    // void comptest_acr_compl_InsertWhenLeftPrefix(); // gstatic/atfdb.comptest:acr_compl.InsertWhenLeftPrefix

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_dm.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_dm_Conflict(); // gstatic/atfdb.comptest:acr_dm.Conflict
    // void comptest_acr_dm_Merge(); // gstatic/atfdb.comptest:acr_dm.Merge
    // void comptest_acr_dm_RenameTuple(); // gstatic/atfdb.comptest:acr_dm.RenameTuple

    // The field table sorts by ctype and leaves the order within a ctype to the file,
    // so a row's position in the file is the record, and a merge that moves it changes
    // the schema.  Ours adds ns.Alpha.a0 at the top of the file, theirs appends
    // ns.Beta.z at the bottom, and each addition has to survive at the place it was
    // made -- a0 first, z last.
    // void comptest_acr_dm_FieldOrder(); // gstatic/atfdb.comptest:acr_dm.FieldOrder

    // A file that one branch adds whole shares no row with the base, so every row it
    // holds belongs to a single run.  Placing the run as a unit is what keeps the walk
    // over the result one level deep here; placing each row after the row above it would
    // make the file a chain and recurse once per row, which stops being survivable at a
    // few tens of thousands.  100k rows is past that point and well under a real limit.
    // void comptest_acr_dm_DeepRun(); // gstatic/atfdb.comptest:acr_dm.DeepRun

    // A rebase hands acr_dm the same three files a merge does, with ours and theirs
    // swapped, so the two must produce the same file.  Both branches add a run of two
    // fields after ns.C.a, which is the case that has an ordering decision to make: the
    // runs come out whole, ordered against each other by key, the same way round either
    // time.
    // void comptest_acr_dm_Symmetry(); // gstatic/atfdb.comptest:acr_dm.Symmetry

    // Upstream moved ns.Alpha.c to the front of its ctype and the branch appended
    // ns.Beta.y, which are edits to two different parts of the file and have to survive
    // together.  A row's position inside its ctype is the member order of a generated
    // struct, so a merge that hands back the base file's order gives the branch a struct
    // upstream never asked for and no gate objects: the schema is consistent with itself,
    // and only upstream's own copy says otherwise.  So c comes first here, and the rebase
    // -- the same three files with ours and theirs swapped -- produces the same file.
    // void comptest_acr_dm_Reorder(); // gstatic/atfdb.comptest:acr_dm.Reorder

    // Both files move ns.C.b, one after ns.C.c and the other after ns.C.d, which is two
    // files changing one thing to two different values -- a conflict, the same as it would
    // be for an attribute.  The two sides of the marker block are the same row twice,
    // because the text of a row carries no trace of where it sits, so the line above the
    // block names the two positions that disagreed.
    // A conflict still leaves the row somewhere, and the rebase -- the same files with ours
    // and theirs swapped -- has to leave it in the same place, which is why the two
    // positions are ordered by key rather than by the file that claimed one first.  The
    // marker labels do name the file in each slot, and those do swap, so the comparison is
    // of everything else.
    // void comptest_acr_dm_MoveConflict(); // gstatic/atfdb.comptest:acr_dm.MoveConflict

    // A line that names no row belongs to the row below it, which is what apm writes above
    // each dev.gitfile row of its manifest: a checksum of the file that row names.  Held
    // that way, the line merges as the row's own text -- ours changes the checksum above
    // cpp/two.cpp, theirs adds cpp/new.cpp with a checksum of its own, and both survive
    // with each comment still above its row.  The last line belongs to no row at all, and
    // a file's tail comes out after every row there is.
    // void comptest_acr_dm_Comment(); // gstatic/atfdb.comptest:acr_dm.Comment

    // The third file is not there, and a file that is not there would otherwise read as a
    // file with no rows in it: the merge would report that theirs deleted every row, write
    // an empty result, and exit zero.  What comes out instead is both of the files that do
    // exist, whole, between markers, which is the shape of every failure the driver cannot
    // merge past.
    // void comptest_acr_dm_MergeFail(); // gstatic/atfdb.comptest:acr_dm.MergeFail

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_ed.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_ed_CreateCtype(); // gstatic/atfdb.comptest:acr_ed.CreateCtype
    // void comptest_acr_ed_CreateMsg(); // gstatic/atfdb.comptest:acr_ed.CreateMsg
    // void comptest_acr_ed_CreateSrcfileTarget(); // gstatic/atfdb.comptest:acr_ed.CreateSrcfileTarget
    // void comptest_acr_ed_CreateSsimfile(); // gstatic/atfdb.comptest:acr_ed.CreateSsimfile
    // void comptest_acr_ed_CreateSsimfileBadNs(); // gstatic/atfdb.comptest:acr_ed.CreateSsimfileBadNs
    // void comptest_acr_ed_CreateTarget(); // gstatic/atfdb.comptest:acr_ed.CreateTarget

    // A field delete drops the schema row and then rewrites the ssimfile that held
    // the field's values.  A field on an in-memory ctype has no such file, so the
    // second invocation emits the delete alone.
    // void comptest_acr_ed_DelField(); // gstatic/atfdb.comptest:acr_ed.DelField

    // A field rename resolves a bare new name against the field's own ctype, and
    // refuses the two spellings that cannot mean what they look like: a query
    // prefix, and a new ctype whose old rows would keep a column no field claims.
    // Between two in-memory ctypes there is no such column, so the move goes
    // through.
    // void comptest_acr_ed_RenameField(); // gstatic/atfdb.comptest:acr_ed.RenameField

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_in.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_in_Reverse(); // gstatic/atfdb.comptest:acr_in.Reverse
    // void comptest_acr_in_Simple(); // gstatic/atfdb.comptest:acr_in.Simple
    // void comptest_acr_in_Tree(); // gstatic/atfdb.comptest:acr_in.Tree

    // -------------------------------------------------------------------
    // cpp/atf_comp/amc.cpp
    //

    // A field of reftype Ptrary without its dmmeta.ptrary record is an input
    // error: amc reports amc.missing_ptrary and exits nonzero without writing
    // any output.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_amc_MissingPtrary(); // gstatic/atfdb.comptest:amc.MissingPtrary

    // The universe is the schema closure the live tree declares as amc's own
    // input, dumped by acr_in and read from stdin. Every other test in this file
    // gives amc a fixture holding a handful of ctypes, which reaches only the
    // generator modules those shapes need; this one generates every namespace the
    // tree defines, and the empty -out_dir: keeps the run from writing any of it.
    // void comptest_amc_SchemaClosure(); // gstatic/atfdb.comptest:amc.SchemaClosure

    // A big-endian u16 field in a universe lacking the u16 csize row: the
    // type's width is unknown, not unsupported, so the diagnostic names the
    // missing dmmeta.csize row instead of prescribing a width change for a
    // type whose width is already right.
    // void comptest_amc_BadBigendNosize(); // gstatic/atfdb.comptest:amc.BadBigendNosize

    // A big-endian field of reftype Inlary on a ctype with a fieldwise
    // constructor: the rejection reports and generation continues to the end
    // of the run, accumulating any further schema errors into the exit code.
    // void comptest_amc_BadBigendReftype(); // gstatic/atfdb.comptest:amc.BadBigendReftype

    // A big-endian u32 field whose bltin row withholds bigendok: the width has
    // a byteswap primitive, but the type is not one the schema allows stored
    // byteswapped, and the Val accessors would swap it anyway.  The check that
    // reads the flag is separate from the width check, so it needs its own pin.
    // void comptest_amc_BadBigendDeadend(); // gstatic/atfdb.comptest:amc.BadBigendDeadend

    // A big-endian u128 field whose bltin row claims bigendok: byteswap
    // primitives exist for 16, 32, and 64 bits only, so the Val accessors would
    // call a nonexistent be128toh/htobe128. The width check rejects the field
    // independently of the bltin claim.
    // void comptest_amc_BadBigendU128(); // gstatic/atfdb.comptest:amc.BadBigendU128

    // Reftype Bitfld and the dmmeta.bitfld record imply each other, and the
    // universe holds one field for each of the four combinations.  A Bitfld
    // field without the record carries no bit offset or width, and a record on
    // a field of another reftype gets a full-width member the ctype's own
    // compare, print and read walks pass over.  Both are reported in one run;
    // the correctly paired field and the plain Val fields draw nothing.
    // void comptest_amc_BadBitfldReftype(); // gstatic/atfdb.comptest:amc.BadBitfldReftype

    // A bitset over a 9-byte (72-bit) Smallstr element: not an unsigned integer
    // builtin, so no exact bit index/shift split exists.
    // void comptest_amc_BadBitsetElem(); // gstatic/atfdb.comptest:amc.BadBitsetElem

    // A bitset over an i8 element: a signed element compiles but sign-extends on
    // read, so bit queries such as Sup return wrong values at runtime.
    // void comptest_amc_BadBitsetSigned(); // gstatic/atfdb.comptest:amc.BadBitsetSigned

    // A bitset over a u32 element whose csize row is missing from the universe:
    // the element type is fine; only its width is unknown. The diagnostic names
    // the missing csize row rather than prescribing a different element type.
    // void comptest_amc_BadBitsetNosize(); // gstatic/atfdb.comptest:amc.BadBitsetNosize

    // Bitset elements whose csize row states a width other than the one the
    // element type's own name states: the name passes the unsigned-builtin check
    // in each case, while the accessors are generated from the csize row. A u32
    // of 3 bytes is 24 bits, which the FloorLog2 index split truncates; a u128 of
    // 32 bytes is 256 bits, past the widest bit-scan helper; a u16 of 8 bytes is
    // 64 bits, a width the accessors can index perfectly well but not the width
    // of the member the struct declares, so the shifts run off its end. The u64
    // and u8 elements of the same universe, whose csize rows agree with their
    // names, draw nothing and stand as the controls.
    // void comptest_amc_BadBitsetWidth(); // gstatic/atfdb.comptest:amc.BadBitsetWidth

    // Fixed-inlary field defaults that name the record by the Init parameter
    // name in a shape the copy-constructor inliner's rewrite does not cover.
    // One field names it bare (sizeof parent), which inlining would leak into
    // the member copy constructor as an undeclared identifier. The other reaches
    // it through a member operator (gcfg.parent.n), where the name belongs to
    // another record and the member-access rewrite would take the operator with
    // it, turning the default into a different expression that still compiles.
    // Both are reported and the run exits nonzero.
    // void comptest_amc_BadCopyctorInit(); // gstatic/atfdb.comptest:amc.BadCopyctorInit

    // The shapes the copy-constructor inliner does cover, pinned through the
    // emitted constructor body. A variable inlary's Init macro carries the two
    // shapes the generators emit -- member access on the record, and the record
    // passed to a function by reference -- and both are retargeted to *this. A
    // fixed inlary's default is arbitrary user text: a default naming the record
    // as *this arrives at the inliner spelled as the Init parameter and returns
    // to *this, a default naming no record at all is copied verbatim, and so is
    // one naming a longer identifier that merely ends with the parameter name
    // (gparent.n), which is a different name and not a reference to the record.
    // The remaining defaults carry the parameter name where it is not an
    // identifier at all: inside a string literal, inside a block comment, and
    // inside a raw string literal whose payload carries the quote that ends an
    // ordinary one. Each is content rather than code, so each is copied into the
    // constructor verbatim and none is reported. A character literal holding a
    // quote is followed by a reference to the record, since a default whose
    // literal is its last content pins nothing: the scan would read the quote as
    // opening a string literal, run out of text and emit the same body. With the
    // reference behind it, that misreading loses the reference and leaves the
    // parameter name in the constructor, where it is undeclared.
    // void comptest_amc_CopyctorInit(); // gstatic/atfdb.comptest:amc.CopyctorInit

    // Which bitfield reads bound the value by the field's store, over one universe
    // holding a field per combination of arg type and width. A read parses into a
    // temporary of the field's arg type and the Bitfld setter masks the store, so a
    // value the bits cannot hold is stored truncated unless the reader refuses it
    // first. The refusal is emitted for a field that carries a symbolic constant,
    // whose reader tries the symbol and falls back to the arg type, and each end of
    // the arg type's range draws a term there exactly when the field's bits do not
    // reach that end. At the high end narrow (u32 in 3 bits) and qos (a u32 wrapped
    // in one field, in 4 bits) draw a bound, while full (u8 in 8 bits), signed_full
    // (i8 in 8 bits) and flag (bool in 1 bit) span their whole arg type and draw
    // none. At the low end a store that drops the arg type's sign bit reads a
    // negative back as a positive number: sig (an i32 wrapped in one field, in 4
    // bits) refuses a negative on top of its high-end bound, and sig_edge (an i8
    // wrapper in 7 bits), whose bits hold every positive value its arg type holds,
    // refuses one with no high-end bound at all, while sig_full (the same wrapper in
    // 8 bits) keeps the sign and draws neither term. plain is narrow again with no
    // symbolic constant on it, and its reader carries no term at all: three bits
    // take a "9" as 1 and the read reports success. An fdec field carries its bound
    // inside the loops its reader parses with: dec scales two decimal places into 12
    // bits, so the digit loop takes a digit only while the value it is building still
    // fits 4095 -- the field's own store, not the u64 the digits accumulate into --
    // and the loop filling the decimal places a short text leaves out tests the same
    // bound. The finished magnitude is tested against nothing, every digit that went
    // into it having been bounded already. One combination never reaches a reader, rejected at generation
    // time: a width past the arg type. A narrower width over a signed arg type is
    // rejected too, but only in a universe that carries the amcdb.bltin table, and
    // this fixture carries none. Signedness has two sources: the range machinery
    // reads it from the type's name, so i8 and i32 are signed here, while the size
    // pass reads the type's amcdb.bltin row and finds none, so its
    // amc.bitfield_signed_narrow never fires. That is why sig and sig_edge reach a
    // reader in this golden -- adding the table makes amc report both of them and
    // generate nothing at all. The name-based source is the deliberate one: a
    // signed integer named by a universe that lacks the type's amcdb.bltin row
    // still gets signed range checks, and this fixture is that universe.
    // void comptest_amc_BitfldReadRange(); // gstatic/atfdb.comptest:amc.BitfldReadRange

    // A cascdel Ptrary whose row ctype has no instance pool: with nothing to
    // delete a row from, DeleteExpr would be empty and the Cascdel loop would
    // spin forever. Pins the existing rejection (amc.nopool on the xref, then
    // the cascdel pool vrfy) so the loop stays unreachable.
    // void comptest_amc_BadCascdelNopool(); // gstatic/atfdb.comptest:amc.BadCascdelNopool

    // A cascdel unique:Y Ptrary whose field has no dmmeta.xref record: the
    // generated Cascdel deletes the last row until the array is empty, and only
    // an xref makes the row's delete unlink it, so the loop would never
    // terminate.
    // void comptest_amc_BadCascdelXref(); // gstatic/atfdb.comptest:amc.BadCascdelXref

    // A buffer condition on a global (FDb) fbuf: the record that enters the list
    // is the buffer's parent row, and a global has no row to enter it with.
    // void comptest_amc_BadFbufCondGlobal(); // gstatic/atfdb.comptest:amc.BadFbufCondGlobal

    // A print:Y cfmt whose (strfmt, printfmt) pair has no print path would ship a
    // Print function with an empty body, so it is rejected and the diagnostic
    // names both attributes. Two ctypes are rejected, one per axis: the Tuple
    // strfmt, for which no print path exists under any printfmt, and the
    // CompactSep printfmt under the String strfmt, which the String arm does not
    // handle. The controls are the whole rest of the table and draw no
    // diagnostic: String with Tuple, Auto, Raw, Sep and Bitset; Argv and Json,
    // which take any printfmt because neither reads one; the Extern printfmt,
    // which hands the body to the user even under the Tuple strfmt; and a print:N
    // cfmt carrying the rejected printfmt, which generates no print function at
    // all.
    // void comptest_amc_BadCfmtPrint(); // gstatic/atfdb.comptest:amc.BadCfmtPrint

    // A read:Y cfmt whose (strfmt, printfmt) pair has no read path would ship a
    // ReadStrptrMaybe with an empty body. Such a reader returns success for every
    // input and leaves the destination holding whatever it held before the call,
    // so no caller can tell a parsed value from an unparsed one. The read emitter
    // rejects the cfmt instead and names both attributes. The fixture is the whole
    // table the emitter decides. Rejected on one axis: the CompactSep printfmt
    // under the String strfmt, the one printfmt the String arm has no reader for.
    // The controls are the rest of the table and draw no diagnostic: String with
    // Tuple, Auto, Raw, Sep, Bitset and Extern, Auto reading as a tuple the way
    // the print side prints one; the Tuple strfmt, whose reader consumes an
    // already-parsed tuple and so is chosen without consulting the printfmt, which
    // is why it accepts CompactSep and still hands Extern to the user; the Json
    // strfmt, for which the read emitter draws nothing at all; and a read:N cfmt
    // carrying the rejected printfmt, which generates no read function to be empty.
    // void comptest_amc_BadCfmtRead(); // gstatic/atfdb.comptest:amc.BadCfmtRead

    // A String cfmt asking for printfmt:CompactSep: the printfmt table declares
    // the value, but amc generates neither a printer nor a reader for it, so the
    // request would ship an empty print body and an empty read body. Both emitters
    // reject it, each naming the cfmt, the strfmt and the offending printfmt, and
    // the run still reaches its exit with a nonzero code. The two diagnostics of
    // one cfmt are what this golden pins; the wider table each emitter decides is
    // pinned by BadCfmtPrint and BadCfmtRead. The Sep ctype beside it is the
    // control: the printfmt amc does generate draws no diagnostic on either side.
    // void comptest_amc_BadCompactSep(); // gstatic/atfdb.comptest:amc.BadCompactSep

    // The field-aware command-line reader is assembled from companion functions,
    // and each companion is emitted under a gate of its own: the reader and NArgs
    // come from a read cfmt with strfmt:Argv, ReadFieldMaybe and GetAnon from a read
    // cfmt whose printfmt is neither Raw nor Extern, and the namespace-level ReadArgv
    // is a call to the cmdline ctype's reader. The fixture is the table of shapes
    // those gates decide. Accepted, drawing no diagnostic: command.good (Argv read:Y
    // printfmt Auto over a base that supplies both companions), command.goodnobase
    // (no basecmdline, so no base companion is called), command.goodraw (its Argv
    // cfmt is Raw, but a second read cfmt supplies ReadFieldMaybe and GetAnon for its
    // anonfld), and namespace okapp over command.good. Rejected: command.raw and
    // command.xtern, whose only read cfmt is Raw or Extern, so the store and the
    // positional lookup are missing; command.basenoread and command.baseraw, whose
    // base is missing NArgs or the store; algo_lib.CmdlineRaw, which is a base and a
    // broken reader at once; and namespaces noreadapp and gnuapp, whose cmdline ctype
    // yields no reader at all -- one because its Argv cfmt is read:N, the other
    // because ArgvGnu generates no reader however it is spelled.
    // void comptest_amc_BadArgvRead(); // gstatic/atfdb.comptest:amc.BadArgvRead

    // A Bytebuf or Linebuf measures its message in bytes and hands it back as an
    // element count, so a wider element makes the returned message run past the
    // data. The element's width comes from the declared csize of a bltin and from
    // the emitted struct body of a generated ctype, so the fixture crosses both
    // buffer types with both width sources: the rejected rows are a byte buffer of
    // u16, of u64, of an eight-byte generated struct and of an extern struct of
    // unknown width, and a line buffer of u32; the accepted controls are both
    // buffer types over a one-byte bltin and over a one-byte generated struct, and
    // a Msgbuf, which carries its length in the message header, over the
    // eight-byte struct.
    // A one-byte element is not enough for a Linebuf, whose generated ScanMsg finds
    // the end of the message by comparing each element with the delimiter, so the
    // same fixture crosses the sources of an operator == with the shapes that emit
    // no comparison at all:
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
    // The Bytebuf over the same ccmp-less struct stays accepted throughout: it
    // returns every available byte and compares nothing.
    // void comptest_amc_BadFbufElem(); // gstatic/atfdb.comptest:amc.BadFbufElem

    // The element of a Varlen or an Opt field is addressed in place inside the
    // message that carries it: the message allocates nothing for it, and destroying
    // the message ends no lifetime of its own. An element type that owns a
    // destructor would therefore be constructed and never destroyed, which leaks
    // whatever the element holds, so the schema is refused at generation time.
    // Whether an element type owns a destructor is not a property of the field. It
    // is a property of the ctype, computed for the whole ctype graph by the pass
    // that also decides whether a value is plain data, so this check reads a flag a
    // different pass fills in. Run before that pass, it reads the flag's initial
    // value, finds every element type destructor-free, and passes a schema it was
    // written to refuse. That is why the fixture carries the accepted rows next to
    // the refused ones: a check that has gone quiet reports nothing on either.
    //
    // field reftype | element type       | accepted
    //
    // Varlen        | has destructor     | no
    // Opt           | has destructor     | no
    // Varlen        | plain data         | yes
    // Opt           | plain data         | yes
    // Val           | has destructor     | yes
    //
    // The element that owns a destructor is an extern ctype declared plaindata:N,
    // which is the shortest way to say that destroying it does something; the plain
    // element is a packed ctype of one integer field. The Val row is the control on
    // the rule's other edge: that reftype gives its member a lifetime of its own, so
    // the destructor is called and the element type is unconstrained.
    // void comptest_amc_BadOptDtor(); // gstatic/atfdb.comptest:amc.BadOptDtor

    // A condition's ins index is inserted into by name, one row at a time, so the
    // index has to be one a single row can be put into and taken out of. A hash is
    // the tempting mistake: it holds the same rows and its name reads like a list's,
    // but it offers InsertMaybe rather than Insert, so the emitted call does not
    // compile. The rule states the requirement positively -- an Llist or a Bheap --
    // which refuses in one test every other shape a field can have: a pool and a
    // pointer array, which have no single-row remove; a count, which keeps a tally
    // and hands no row back; and a tree or a sorted list, whose insert takes more
    // than the row.
    // The fixture's universe therefore carries an FDb index of each of those
    // reftypes, and its rejected row names the hash. Only that one is named per
    // run: the fcond rules are vrfy checks, so the first violation ends the run, and
    // a fixture crossing every reftype would report the same single line. The
    // accepted control is a condition on the sibling buffer naming an Llist of the
    // watched ctype, which draws no diagnostic.
    // void comptest_amc_BadFcondIns(); // gstatic/atfdb.comptest:amc.BadFcondIns

    // A TimeHookOnce fstep on an Llist field: the step reads the expiration time
    // from the first row's sort field, which only a Bheap step field provides.
    // void comptest_amc_BadFstepBheap(); // gstatic/atfdb.comptest:amc.BadFstepBheap

    // A TimeHookRecur fstep on an Upptr field: the time hook is armed from the
    // index's first row, which only an Llist or Bheap step field provides.
    // void comptest_amc_BadFstepFirst(); // gstatic/atfdb.comptest:amc.BadFstepFirst

    // A TimeHookRecur fstep on an Atree field: the hook is armed by FirstChanged
    // calls, which only the Llist and Bheap generators emit -- an Atree step
    // would compile and never fire.
    // void comptest_amc_BadFstepAtree(); // gstatic/atfdb.comptest:amc.BadFstepAtree

    // An fstep on a fixed Inlary field: the step's loop condition tests the
    // index with EmptyQ, which generates only for a variable Inlary -- a fixed
    // one always holds max elements and offers no emptiness test.
    // void comptest_amc_BadFstepInlary(); // gstatic/atfdb.comptest:amc.BadFstepInlary

    // An InlineRecur fstep with fdelay scale:Y on a Val field: the scaled delay
    // divides by the step index's row count, which a Val field does not have.
    // void comptest_amc_BadFstepScale(); // gstatic/atfdb.comptest:amc.BadFstepScale

    // A Callback fstep on a ZSListMT field: the list is concurrent and has no
    // EmptyQ, so the step's loop condition tests DestructiveFirst, which only
    // the Inline and InlineRecur call shapes embed.
    // void comptest_amc_BadFstepZslistmt(); // gstatic/atfdb.comptest:amc.BadFstepZslistmt

    // An fdelay row on an Inline fstep: the delay between invocations is read
    // only by the InlineRecur and TimeHookRecur call shapes, so on any other
    // steptype the row configures nothing.
    // void comptest_amc_BadFstepFdelay(); // gstatic/atfdb.comptest:amc.BadFstepFdelay

    // An fstep on an Lpool field: the step's loop condition tests the field
    // for pending work, and a memory pool offers no emptiness test, so the
    // generated condition would call an EmptyQ that does not exist.
    // void comptest_amc_BadFstepReftype(); // gstatic/atfdb.comptest:amc.BadFstepReftype

    // InlineRecur fsteps with fdelay scale:Y on a Blkhash and on a Lary field:
    // both reftypes generate the N function the scaled delay divides by, so the
    // schema generates cleanly.
    // void comptest_amc_FstepScaleBlkhash(); // gstatic/atfdb.comptest:amc.FstepScaleBlkhash

    // An fstep on a field of a non-global ctype: the generated step would read
    // the field through the namespace global, a member FDb does not have.
    // void comptest_amc_BadFstepGlobal(); // gstatic/atfdb.comptest:amc.BadFstepGlobal

    // A dispatch filter over a message field that has its own Print function
    // (an fconst field): the filter's regx match prints the field through that
    // function, passing the message as the parent argument. Pins the emitted
    // Match body, including the well-formed Print call.
    // void comptest_amc_DispfilterFieldPrint(); // gstatic/atfdb.comptest:amc.DispfilterFieldPrint

    // A dispatch filter declared match_all:Y, whose two messages between them cover
    // every shape the rule has to place. Ev carries both filter fields, so its Match
    // body reads the message: the equality and the range tests take the big-endian
    // field through its Get function, and the regx match prints the message's own
    // field through its fconst Print function before matching. Ev2 carries neither,
    // so every filter field of Ev's turns into a present test that refuses the
    // message, which is what match_all means. Ev2's own field is the control on the
    // other side, and Ev's present tests for it are the same refusal in reverse.
    // void comptest_amc_DispfilterMatchAll(); // gstatic/atfdb.comptest:amc.DispfilterMatchAll

    // A gconst value table whose fourth and fifth lines are not tuples: each
    // carries an unterminated quoted value. The reader keeps what it read before
    // the quote ran out and jumps to end of line, so the truncated value would
    // read back as a constant name the table never wrote, with every attribute
    // past the bad quote -- the comment among them -- dropped. Both lines are
    // reported with the file, the line number and the line's text, none of which
    // the tuple reader itself supplies. The query prints the resulting enum, so
    // the controls are pinned too: a blank line and a comment line draw no
    // diagnostic, the two well-formed rows still emit, and the auto-numbering
    // consumes no index for a line that produced no constant.
    // void comptest_amc_BadGconstBadline(); // gstatic/atfdb.comptest:amc.BadGconstBadline

    // A gconst value table in which the same name appears twice: the second row
    // would silently vanish from the enum while still consuming a numbering
    // index, gapping the numbering on a clean exit.
    // void comptest_amc_BadGconstDup(); // gstatic/atfdb.comptest:amc.BadGconstDup

    // A gconst value table containing a line with a head but no attribute: the
    // line creates no constant, yet auto-numbering would consume an index for
    // it, silently renumbering every constant that follows.
    // void comptest_amc_BadGconstHeadonly(); // gstatic/atfdb.comptest:amc.BadGconstHeadonly

    // A gconst whose idfld names an existing field of a ctype other than the
    // namefld's: the field is absent from the value table's rows, so every
    // fconst value would come out empty.
    // void comptest_amc_BadGconstCtype(); // gstatic/atfdb.comptest:amc.BadGconstCtype

    // A gconst whose idfld names no existing field: the id column must not
    // silently fall back to the name column.
    // void comptest_amc_BadGconstIdfld(); // gstatic/atfdb.comptest:amc.BadGconstIdfld

    // Two gconsts with a bad idfld, the first on a char-typed field: the run
    // reports both and exits, instead of processing the known-bad row and
    // aborting the scan on a context-free char-parse failure.
    // void comptest_amc_BadGconstCtypeChar(); // gstatic/atfdb.comptest:amc.BadGconstCtypeChar

    // A char-typed gconst whose value table holds multi-character values: each
    // offending row is reported with its gconst, name and value, and the scan
    // continues to the end of the table.  The query prints the resulting enum,
    // so the table's one valid row is pinned too: it survives the skipped
    // neighbors and emits as a single-quoted character constant.
    // void comptest_amc_BadGconstChar(); // gstatic/atfdb.comptest:amc.BadGconstChar

    // A gconst over a value table that exists only inside the fixture universe:
    // the values side-load from the -in_dir root, not from data/ under the CWD,
    // and no amc.sideload notice appears for a directory universe.
    // void comptest_amc_GconstIndir(); // gstatic/atfdb.comptest:amc.GconstIndir

    // A gconst whose value table is absent from the -in_dir root: the table's rows
    // are the enum's constants, so loading it as zero rows would compile an empty
    // enum and exit 0. The run reports the file and exits nonzero.
    // void comptest_amc_GconstLoadFail(); // gstatic/atfdb.comptest:amc.GconstLoadFail

    // A gconst and a gstatic whose table has no name at all: the ctype the rows
    // would come from reaches no ssimfile, so there is no file to look for. This is
    // the other way a table can be absent, and it is reported apart from a named
    // file that cannot be read, because the reader can say nothing about a file it
    // was never given -- amc.GconstLoadFail and amc.GstaticLoadFail are that case,
    // and their ctypes reach a ssimfile of their own and through a base
    // respectively, which is what makes them the controls for this one.
    // A ctype reaches no ssimfile two ways, and the fixture holds both for each
    // consumer: it declares no base at all, or a base that owns no ssimfile either.
    // Both name the offending gconst or gstatic field and let the run continue, so
    // one run reports all four rows rather than ending at the first.
    // void comptest_amc_SideloadNossimfile(); // gstatic/atfdb.comptest:amc.SideloadNossimfile

    // A gstatic field whose table file is absent from the -in_dir root: the
    // rows would become the compiled-in registry, so loading the table as
    // zero rows would silently drop every entry. The run reports the file
    // and exits nonzero.
    // void comptest_amc_GstaticLoadFail(); // gstatic/atfdb.comptest:amc.GstaticLoadFail

    // A gstatic table whose second line is not a tuple: an unterminated quoted
    // value. The line would silently drop out of the compiled-in registry, so the
    // run reports it and exits nonzero. The diagnostic names the field that was
    // reading, the file, the line number and the line's text, none of which the
    // tuple reader itself supplies. The query prints the loader the table compiles
    // into, so the two well-formed lines around the bad one are pinned as well: they
    // come back as the loader's own rows, which is how a run that abandoned the
    // table at the first bad line would show itself. The universe also carries an
    // fconst whose value is a character literal, so an earlier pass has left a
    // bad-number tag naming that valid value in algo_lib's shared error buffer: a
    // message taken from there would name the fconst instead of the file that is
    // actually malformed.
    // void comptest_amc_GstaticBadLine(); // gstatic/atfdb.comptest:amc.GstaticBadLine

    // A gsymbol table whose fourth and fifth lines are not tuples: each carries an
    // unterminated quoted value. The truncated text would become a symbol name and
    // value the table never wrote, so every reference to the intended symbol fails
    // to link. Both lines are reported with the file, the line number and the
    // line's text, and the run exits nonzero.
    // void comptest_amc_GsymbolBadline(); // gstatic/atfdb.comptest:amc.GsymbolBadline

    // A gsymbol whose table is absent from the -in_dir root: the table's rows are
    // the symbols the namespace exports, so emitting an empty symbol block and
    // exiting 0 would leave every reference to them unresolved at link time. The
    // run reports the file and exits nonzero.
    // void comptest_amc_GsymbolLoadFail(); // gstatic/atfdb.comptest:amc.GsymbolLoadFail

    // A gsymbol in a single-file universe: with no directory root the symbol
    // table side-loads from the default data/ root of the current directory,
    // which may belong to a different universe than the -in_dir schema; the
    // amc.sideload notice names the fallback root and the file read from it.
    // void comptest_amc_GsymbolSideload(); // gstatic/atfdb.comptest:amc.GsymbolSideload

    // An Llist field without its dmmeta.llist record: reported per field, then
    // the run stops at the reftype gate, since later gen phases dereference the
    // per-reftype records this phase found missing.
    // void comptest_amc_MissingLlist(); // gstatic/atfdb.comptest:amc.MissingLlist

    // A ccmp on a global ctype: there is exactly one instance, and the generated
    // Cmp/Lt bodies would silently compare the singleton with itself (a global
    // field's Get accessor takes no parent argument).
    // void comptest_amc_BadCcmpGlobal(); // gstatic/atfdb.comptest:amc.BadCcmpGlobal

    // A chash on a global ctype: same rule as ccmp -- the generated Hash body
    // would silently hash the singleton's own fields whatever argument is passed.
    // void comptest_amc_BadChashGlobal(); // gstatic/atfdb.comptest:amc.BadChashGlobal

    // An fcmp on a global ctype's field: same rule as ccmp, one level down --
    // both operands of the generated Cmp/Lt body would collapse to the
    // singleton's value and the function would compare the field with itself.
    // void comptest_amc_BadFcmpGlobal(); // gstatic/atfdb.comptest:amc.BadFcmpGlobal

    // A ctype with both a Tpool and a Global instance, hashed by an FDb Thash:
    // the pooled rows would reach field accessors that take no parent and read
    // the global, so the hash would compare the singleton with itself. The
    // Global instance must be the ctype's only instance.
    // void comptest_amc_BadGlobalInst(); // gstatic/atfdb.comptest:amc.BadGlobalInst

    // A minmax:Y ccmp on a non-builtin ctype without genop:Y order:Y: the
    // Min/Max functions compare with a raw <, which no generated or native
    // operator provides.
    // void comptest_amc_BadMinmax(); // gstatic/atfdb.comptest:amc.BadMinmax

    // minmax:Y ccmp on a builtin (native operator <) and on an extrn:Y ctype
    // (user-defined comparison), neither with order:Y genop:Y: both provide the
    // operator < the Min/Max bodies compare with, so the schema generates
    // cleanly.
    // void comptest_amc_MinmaxNative(); // gstatic/atfdb.comptest:amc.MinmaxNative

    // The ctype-named GetScale convenience exists only for a ctype with a
    // single fdec field; with two fdec fields the definitions would collide.
    // Pins that the one-fdec ctype keeps it and the two-fdec ctype has none.
    // void comptest_amc_FdecGetScale(); // gstatic/atfdb.comptest:amc.FdecGetScale

    // A query match in a namespace with no dmmeta.nscpp row reports
    // amc.query_nocpp and counts toward the run's exit code, because the namespace
    // emits no C++ and the query has nothing to print for that record.
    // The first invocation matches two ctypes at once, one in a namespace that has
    // the row (algo_lib.CppWidget) and one in a namespace that does not
    // (ncns.Widget): the ncns match is reported, the algo_lib match still generates
    // and prints its struct, and the run exits 1 for the one report.
    // The second invocation queries testdb, an ssimdb namespace projected to
    // TypeScript alone, under the default key, so both query walkers run: the two
    // ctypes and the seven generated funcs each draw a report and the run exits 9.
    // The third and fourth invocations discard stderr. Query mode's stdout carries
    // the generated C++ and nothing else, so a consumer that wants code alone drops
    // stderr, and for a query wholly inside a nocpp namespace its stdout is empty.
    // The fourth query spans a nocpp namespace and a code-generating one, the case
    // that tells the two streams apart: its stdout holds the generated function and
    // none of the nine reports the same run writes to stderr.
    // void comptest_amc_QueryNocpp(); // gstatic/atfdb.comptest:amc.QueryNocpp

    // An fdec on a bitfld field of a global ctype: the value is reached through
    // the field's Get/Set accessors, which for a global parent take no parent
    // argument. Pins the emitted GetDouble/qSetDouble bodies.
    // The second field is ten bits wide inside a u32 word, and pins the range
    // the emitters bound it by. A bitfld setter masks the store to the declared
    // width, so the magnitude the field holds is 1023 and not the 4294967295 its
    // arg would suggest: SetDoubleMaybe compares against 1023 and the decimal
    // reader carries that magnitude split by ten, 102 and 3.
    // Each of the three stores casts the value to the field's own type on the
    // way in, so the setter is handed exactly the type it declares. The double
    // the double setters compute and the u64 magnitude the reader accumulates
    // are neither of them that type, and an arg naming a ctype that stands for
    // an integer builtin is not that type either.
    // void comptest_amc_FdecGlobalBitfld(); // gstatic/atfdb.comptest:amc.FdecGlobalBitfld

    // A decimal scale amc cannot generate consistent arithmetic for. Zero places
    // -- the nplace default, so what a forgotten attr leaves behind -- emits a
    // reader that accumulates no digit at all; eighteen places outrun the powers
    // of ten the double accessors substitute, so they and the string accessors
    // would disagree about the scale; thirty-one places outrun the printer's
    // scratch buffer; twelve places on an i32 field outrun the field's own arg,
    // which the emitted GetScale returns the scale as, and the diagnostic states
    // the narrower range that arg allows. A u8 arg is the narrowest range the
    // rule admits from the arg side: two places fit its 255 and three do not, and
    // the diagnostic states that bound.
    // The fixture also holds one field per arg that carries no numeric range at
    // all -- bool, char, u128, float, double, pad_byte, and a plain struct ctype
    // -- and each is reported as an unusable arg. The char field's count is zero
    // as well, and it is still examined against the powers-of-ten limit, so that
    // field draws both diagnostics in one run.
    // The remaining fields cover the other way a field's range is fixed. A
    // Bitfld field stores into a slice of another field's word, and its setter
    // masks the store, so the width and not the arg is what bounds the count: a
    // ten-bit slice of a u32 takes three places and rejects four, a forty-bit
    // slice of a u64 takes twelve and rejects thirteen, and a three-bit slice
    // holds no decimal place at all, which the diagnostic states rather than
    // naming an empty range. A signed arg cannot be sliced narrower than its own
    // type at all: the ten-bit i32 field draws the bitfield diagnostic that
    // refuses it, since such a field does not sign-extend on read. That refusal
    // follows the type the arg stands for rather than the arg's own name, so a
    // ten-bit slice of a ctype whose single Val field is an i32 draws it too, and
    // a seven-bit slice of an i8 draws it at the narrowest signed arg there is.
    // A slice whose arg carries no numeric range takes no bound from its width:
    // the four-bit char field is reported as an unusable arg, and its count is
    // then held against the powers-of-ten limit alone.
    // The controls draw no diagnostic: the one-place and seventeen-place i64
    // fields sit at the bottom and the top of the supported range, each of the
    // eight integer builtins carries a count its own range allows, so no width
    // and neither sign is left untested, and a slice as wide as its arg takes
    // what the arg takes at both signs -- u32 and i32 at thirty-two bits each
    // take nine places, a u64 at sixty-four takes seventeen, and an i8 at eight
    // takes the two places its 127 allows.
    // An arg standing for the builtin through another ctype stores that ctype,
    // while every quantity the emitted functions compute is a number of the
    // builtin, so the value crosses between the two at each end: out of the
    // wrapper through the operator dmmeta.fcast generates, and into it through
    // the fieldwise constructor a dmmeta.cpptype row asking for a constructor
    // provides. The fixture carries one arg per combination of the two.
    // DecPlaceCast carries both and is accepted, and the query pins its emitted
    // functions, so what an accepted wrapper compiles to is part of what this
    // test compares rather than something no golden looks at. DecPlaceFcast
    // carries the operator alone, DecPlaceCtor the constructor alone, and
    // DecPlaceWrap neither, and each is reported naming what is missing.
    // DecPlaceChain stands for the builtin two ctypes deep, where the
    // constructor takes the inner ctype rather than the builtin, and is reported
    // as standing for the builtin only through another ctype. The same rule
    // reaches a Bitfld arg: the ten-bit slice of a wrapped u32 carries both
    // conversions and draws its count diagnostic alone, while the wrapped i32
    // slice carries neither and draws the conversion diagnostic beside its own.
    // void comptest_amc_BadFdecNplace(); // gstatic/atfdb.comptest:amc.BadFdecNplace

    // A Bitfld field whose declared width is zero, carrying a decimal count. The
    // width is reported as out of bounds. The decimal range takes no bound from
    // such a width: a bound computed from it would state that the field holds no
    // decimal place at all, so the count draws no diagnostic of its own. The
    // bitfield accessors refuse the width once generation reaches the field, and
    // the run ends there -- which is why the width error follows the bounds
    // report, and why a fixture reaches the decimal rule with one bad width at
    // most. The negative width has a fixture of its own.
    // void comptest_amc_BadFdecBitwidth(); // gstatic/atfdb.comptest:amc.BadFdecBitwidth

    // The zero-width fixture's counterpart on the other side of the same rule: a
    // Bitfld field whose declared width is negative, carrying a decimal count.
    // The width is reported as out of bounds and the decimal range takes no bound
    // from it, so the count draws no diagnostic of its own. The magnitude is
    // sixty-four rather than one because the rule is checked through its output:
    // a bound built from a negative width shifts by a negative count, and a
    // compiler that takes the count modulo sixty-four then computes a bound of
    // zero from this width and reports the field as holding no decimal place. A
    // run that keeps the width out of the shift prints two rows; one that lets it
    // in prints three.
    // void comptest_amc_BadFdecBitwidthNeg(); // gstatic/atfdb.comptest:amc.BadFdecBitwidthNeg

    // A Tuple-printfmt String cfmt on a global ctype: field values are reached
    // through the print function's row argument, including the cursor walk of an
    // array field. Pins the emitted Print body.
    // void comptest_amc_PrintGlobalTuple(); // gstatic/atfdb.comptest:amc.PrintGlobalTuple

    // A read:Y Tuple cfmt on a global ctype: the ReadFieldMaybe dispatcher
    // keeps the ctype-name prefix its callers link against and passes no
    // parent to the field readers, which -- like every field-level function
    // of a global -- take no parent argument; the Set-backed reader assigns
    // through the collapsed Set accessor.
    // void comptest_amc_ReadGlobalTuple(); // gstatic/atfdb.comptest:amc.ReadGlobalTuple

    // An ArgvGnu cfmt: the gnu form of the same emission matrix ArgvGlobal pins
    // for Argv. A one-character field name takes a single dash, longer names take
    // two, PrintArgv separates the name from its value with a space where Argv
    // writes a colon, ToArgv appends the value as its own argv token, and an array
    // field repeats the name once per element. args[0] is the bare command name,
    // no bin/ prefix: an ArgvGnu command is external and resolves on PATH, which
    // atf_unit ExecToArgvSyntax asserts against the real bash command.
    // void comptest_amc_ArgvGnu(); // gstatic/atfdb.comptest:amc.ArgvGnu

    // An Argv cfmt on a global ctype and on a regular one: the value of a field
    // with its own Print function (an fconst field) prints through that
    // function, whose parent argument collapses for the global and is the row
    // itself -- no const_cast -- for the regular ctype. Pins the PrintArgv and
    // ToArgv bodies of both.
    // void comptest_amc_ArgvGlobal(); // gstatic/atfdb.comptest:amc.ArgvGlobal

    // Which field of a command ctype becomes a command-line token, and how its
    // value is reached. Tokens: a plain Val, a Val cloned in from a Base, an
    // fconst Val and a Smallstr through their own Print functions, a Tary through
    // a cursor walk, a big-endian Val and a bitfield through their Get accessors.
    // No token: the Global anchor and the Base anchor, which name no member of
    // their own; the source word a bitfield lives in, whose bits print
    // individually outside Raw form; and a Ptr, whose address has no command-line
    // spelling ReadArgv could take back. Pins the PrintArgv bodies of all three
    // ctypes and the ToArgv bodies of the two that carry the interesting fields.
    // void comptest_amc_ArgvField(); // gstatic/atfdb.comptest:amc.ArgvField

    // An fconst on a global ctype's field: the fconst accessors take no parent
    // argument, so the calls between them pass none. Pins ToCstr's GetEnum
    // switch, Print's ToCstr call, SetStrptrMaybe's SetEnum calls (short and
    // beyond-8-byte names), and -- via a bitfld member -- GetEnum's Get call.
    // void comptest_amc_FconstGlobal(); // gstatic/atfdb.comptest:amc.FconstGlobal

    // The universe read from stdin (-in_dir:-), which is how a caller composes
    // one -- an acr_in dump plus extra rows -- without a temp file.  The query
    // prints a function that can only come from the piped rows, and "-" names no
    // directory, so side table data loads from the default root and the run says
    // so.
    // void comptest_amc_StdinUniverse(); // gstatic/atfdb.comptest:amc.StdinUniverse

    // A numstr on a global ctype's field: the numstr accessors take no parent
    // argument, so Geti64 and GetnumDflt call Getnum bare. Pins both Geti64
    // arms (the u64 numtype's overflow-checked arm and the plain arm, via an
    // unsigned and a signed field) and GetnumDflt's body.
    // void comptest_amc_NumstrGlobal(); // gstatic/atfdb.comptest:amc.NumstrGlobal

    // A pmask on a global ctype: the pmask's bitset accessors take no parent
    // argument, so the member accessors and the print filter call them bare.
    // Pins one member's PresentQ/SetPresent/Set bodies and the filtered Print.
    // void comptest_amc_PmaskGlobal(); // gstatic/atfdb.comptest:amc.PmaskGlobal

    // A Bitfld field in a kafka codec: the field has no direct member, so raw
    // parent.<name> access is uncompilable. Encode reads through the Get
    // accessor (FieldvalExpr); decode lands in a temporary stored through the
    // Set accessor (NoDirectMemberQ -- deliberately narrower than
    // AssignViaSetQ, since this codec's presence bits follow the wire's null
    // flag and a pmask Set would mark a null field present).
    // void comptest_amc_KafkaBitfld(); // gstatic/atfdb.comptest:amc.KafkaBitfld

    // A kafka nullable array on a ctype whose pmask field is not named "pmask":
    // the presence accessors derive their names from the pmask field
    // (val_FieldMaskQ / val_SetFieldMask here), and the encoder's presence
    // checks must use the same derived name the decoder's set side does.
    // Pins the KafkaEncode presence checks and the KafkaDecode present-mark.
    // void comptest_amc_KafkaPmaskName(); // gstatic/atfdb.comptest:amc.KafkaPmaskName

    // An optional fpbuf field on a ctype whose pmask field is not named "pmask":
    // same derived-name rule as KafkaPmaskName, and both sides of the protobuf
    // codec have to spell the name the pmask field derives. The encoder's presence
    // guard reads val_FieldMaskQ; the decoder stores the plain Val field into its
    // own member and marks presence with val_SetFieldMask, which is the same
    // update the field's own Set carries.
    // Pins the PbufEncode guard, the PbufDecode present-mark, the Set, and the
    // accessors.
    // void comptest_amc_PbufPmaskName(); // gstatic/atfdb.comptest:amc.PbufPmaskName

    // A bitfld carrying an fpbuf, on a ctype with a pmask: the field's value
    // lives inside its source field, not in a member of its own, so the encoder
    // reads it through the Get accessor and the decoder stores a temporary through
    // the Set accessor. The temporary starts empty, the varint decoder replacing
    // whatever it is handed. That Set writes the slice of the source word and
    // leaves the pmask alone, so the decoder marks presence itself.
    // Pins the accessor-routed PbufEncode and PbufDecode, and the Set body.
    // void comptest_amc_PbufBitfld(); // gstatic/atfdb.comptest:amc.PbufBitfld

    // A bitfld carrying an fpbuf on a proto3 ctype with no pmask: presence is
    // implicit, so the encode guard is the pbtype's default-value check, and that
    // check must read the field through its Get accessor -- a bitfield has no
    // member of its own to compare against zero. Pins the guard, and the Set body
    // storing straight into the source word since no presence bit follows.
    // void comptest_amc_PbufBitfldNondflt(); // gstatic/atfdb.comptest:amc.PbufBitfldNondflt

    // A repeated fpbuf field that is a pmask member: the decoder must mark the
    // field present when elements arrive, or the print filter that consults the
    // same bit drops the decoded elements. Pins the SetPresent call in both the
    // packed and the one-element-per-tag decode branch, and the filtered Print.
    // void comptest_amc_PbufRepeatedPmask(); // gstatic/atfdb.comptest:amc.PbufRepeatedPmask

    // Every decode store path in one run: one message carrying each singular
    // shape, one carrying the repeated controls. A singular field decodes into its
    // own member, and under pb_type message that member already holds the earlier
    // occurrence, so the nested decoder merges into it -- pbt.Msg.bare and .sub.
    // A pmask member decodes in place too and is marked present afterwards (.opt,
    // .sub): its own Set would have marked it, but storing through that Set would
    // copy the value out and back on every occurrence of the field number. The
    // shapes with no member to decode into do pay for a temporary -- .slice
    // through the bitfld accessors, .netval through the byte-swapping pair -- and
    // so does .cond, whose Set moves the record between fcond lists. Of the eight,
    // only .computed seeds its temporary from the field's current value, being the
    // one that is both accessor-routed and a message: Get is then the only way for
    // the merge to see what the earlier occurrence set. pbt.Rep appends through
    // Alloc in every branch.
    // void comptest_amc_PbufStore(); // gstatic/atfdb.comptest:amc.PbufStore

    // An Opt field on a global ctype: rejected as amc.infinity_pool, and the
    // Print body emitted alongside the error must not contain a bare (void);
    // statement for the absent parent argument.
    // void comptest_amc_OptGlobalPrint(); // gstatic/atfdb.comptest:amc.OptGlobalPrint

    // A base-1 numstr and a base-37 one: neither base has a digit encoding
    // (unary needs one digit per unit of value; 37..94 has no character map),
    // so amc rejects both fields, naming each.
    // void comptest_amc_BadNumstrBase(); // gstatic/atfdb.comptest:amc.BadNumstrBase

    // A numstr min_len wider than the string itself, one wider than
    // SetnumMaybe's 64-character pad budget, and a signed one equal to the
    // string length: the sign then has no slot, so every negative value would
    // fail at runtime. None of these width contracts can be honored, so amc
    // rejects all three fields.
    // void comptest_amc_BadNumstrMinlen(); // gstatic/atfdb.comptest:amc.BadNumstrMinlen

    // A numtype that does not stand for an integer bltin. The fixture holds one
    // field per such numtype: double and float carry no integer range, u128 is
    // wider than the range amc represents, pad_byte is no number at all, and a
    // plain struct ctype stands for no bltin whatever. Each falls back on the
    // widest signed bounds, which admit every value and make the sign tests fire
    // even for an unsigned numtype, so a range comparison built from one would
    // never reject. One fdec field trips the same check from the other side.
    // The rest of the fields exercise the agreement between a ctype name and its
    // numtype, which amc enforces so that a name cannot lie about the range its
    // string holds. LnumStr6_I32 says I32 over a u32 numtype and LnumStr9_U64
    // says U64 over the same one: a name is rejected for lying about the sign
    // and for lying about the width alike. LnumStr4_I16 says I16 over
    // algo_lib.U32Val, a ctype whose single Val field holds a u32, and the
    // numtype is judged by the u32 it stands for, so the lie is caught through
    // the wrapper. The controls draw no diagnostic from the same run:
    // LnumStr7_U32 states a truthful token over a bare u32, LnumStr6_U32 states
    // one over the wrapper, LnumStrPlain carries no token to judge, and
    // LnumStr7_I8 carries a false one under strict:N, which asks for no check.
    // A numtype standing for the builtin through another ctype has to move a
    // value both ways: Getnum constructs the numtype from the digits it parsed,
    // and Geti64 reads that result as an i64. The constructor comes from a
    // dmmeta.cpptype row asking for one and the read from the operator
    // dmmeta.fcast generates, and the fixture carries one numtype per
    // combination of the two. LnumStr6_U32 names a wrapper carrying both and is
    // accepted, and the query pins its Getnum and Geti64, which are the two
    // statements the conversions have to satisfy. LnumStr8_U32 names one with
    // the operator alone, LnumStr10_U32 one with the constructor alone, and
    // LnumStr5_U32 one with neither, and each is reported naming what is
    // missing. LnumStr11_U32 names a wrapper two ctypes above the builtin, whose
    // constructor takes the inner wrapper rather than the builtin, and is
    // reported as standing for the builtin only through another ctype. The bare
    // builtins are the accepted edge of the same rule, and all eight are present
    // with a truthful name token, so neither sign nor any width is left
    // untested.
    // void comptest_amc_BadNumstrNumtype(); // gstatic/atfdb.comptest:amc.BadNumstrNumtype

    // Pads that read back as digits. A left-pad that is a nonzero digit of the
    // base strips leading value digits on read: '!' is digit 1 in base 95, 'c'
    // is a digit in base 36, '5' is a digit in base 10. A right-pad that is any
    // digit of the base divides the stored value by a power of the base: NUL in
    // base 256, ' ' in base 95, 'A' in base 36, '7' in base 10. The pad is a C++
    // character literal, so its escaped spellings are judged by the byte they
    // denote: a base-16 right-pad '\x30' is the digit zero and is rejected,
    // while a base-95 left-pad '\0' is no digit of that base and passes. Only
    // the zero digit is a safe left-pad and no digit is a safe right-pad, so amc
    // rejects eight of the nine numstr fields. Four more fields carry no numstr
    // row at all and meet the plain smallstr right-pad check, which refuses any
    // pad character that can be confused with a number and reads an escaped pad
    // by its denoted byte just the same: the digit '3' and the escaped digit
    // '\x30' are refused, while the escaped 'A' and the escaped NUL are no part
    // of any number and pass.
    // The sign character is the other pad a signed numstr cannot read back. A
    // left pad is stripped from the end the sign is written to, so a signed
    // base-10 field padded with '-' loses the sign of every negative value and
    // is rejected. The three neighbours of that case are the controls and draw
    // nothing: the same '-' pad on an unsigned field, which writes no sign; the
    // same '-' pad on the right, stripped from the end the sign is not at; and
    // '+' on the left of a signed field, which is not the character the reader
    // takes for a sign. A signed field left-padded with the zero digit is the
    // fourth control -- leading zeros carry no value, so they strip safely.
    // The NUL pad, which a signed numstr may not use either, shows that a pad is
    // judged by the byte it denotes and not by the spelling it was written as.
    // The three spellings of that one pad -- the bare 0, the empty pad value amc
    // fills in as 0, and the escaped '\0' -- are all rejected on a signed field,
    // on the left and on the right alike, while the same empty pad on an unsigned
    // field passes at either end. The two rpascal fields are the controls for the
    // strtype dimension: such a string carries its own length and strips no pad,
    // so neither the NUL pad on a signed field nor a digit pad draws anything.
    // void comptest_amc_BadNumstrPad(); // gstatic/atfdb.comptest:amc.BadNumstrPad

    // An rpascal string longer than its length byte can count. The length of an
    // rpascal string is stored in one byte, so 255 is the longest one whose
    // length is representable, and amc rejects a longer one. Two such fields
    // appear in the fixture and the run exits with 2: each diagnostic raises the
    // error count, so a later one cannot discard what an earlier one reported.
    // The controls draw no diagnostic: a 255-character rpascal string is the
    // longest accepted one, and a 300-character right-padded string carries no
    // length byte, so the bound is the rpascal strtype's alone.
    // void comptest_amc_BadSmallstrToobig(); // gstatic/atfdb.comptest:amc.BadSmallstrToobig

    // A signed base-95 numstr and a signed base-256 one: '-' is a digit in
    // both bases, so a stored digit string is indistinguishable from a signed
    // one, and amc rejects both fields. Both fields left-pad with ' ', which is
    // digit 0 in base 95 and digit 32 in base 256, so the base-256 field draws
    // a second rejection from the pad check: a left-pad that is a nonzero digit
    // of the base strips leading value digits on read.
    // void comptest_amc_BadNumstrSignedBase(); // gstatic/atfdb.comptest:amc.BadNumstrSignedBase

    // One cpbuf ctype with every illegal field number in one run: a duplicate
    // within the ctype (two identical decoder case labels: uncompilable),
    // number 0 (wire tag 0, reserved as illegal by the protobuf format), and
    // 2^29 (overflows the (number<<3)|wire_type tag, aliasing another field).
    // void comptest_amc_BadPbufFieldNumber(); // gstatic/atfdb.comptest:amc.BadPbufFieldNumber

    // One cpbuf ctype with every arg/reftype shape the pbuf codec cannot
    // compile against, rejected in one run: pb_type string on a u32 (the
    // lib_pb codec accepts only the pbtype's argtype), pb_type message on a
    // non-cpbuf arg, an Inlary field (no wire mapping), and pb_type enum on
    // args without the fconst-carrying i32 value shape (one with no value
    // field at all, one whose value field is u8 -- the lib_pb varint codec
    // binds i32).
    // void comptest_amc_BadPbufArg(); // gstatic/atfdb.comptest:amc.BadPbufArg

    // Two fpbuf rows asking for packed encoding where the protobuf format has
    // none: on a singular field, and on a repeated field of a length-delimited
    // pb_type. The emitter reads packed only in the repeated non-length-delimited
    // branch, so both rows would be silently ignored and the author would get the
    // ordinary encoding. The third row, packed on a repeated varint field, is the
    // legal shape and is not reported.
    // void comptest_amc_BadPbufPacked(); // gstatic/atfdb.comptest:amc.BadPbufPacked

    // Two fpbuf rows that would generate no wire code at all, rejected rather
    // than left silently dead: one on a field of a ctype with no cpbuf row (no
    // codec is generated without one), one on a Base field (emission never
    // writes a base's fields to the wire).
    // void comptest_amc_BadPbufNoCodec(); // gstatic/atfdb.comptest:amc.BadPbufNoCodec

    // The syntax dimension of cpbuf, over four ctypes in one run. cpbuf.syntax
    // names a row of dmmeta.pbsyntax, the table that says whether the syntax has
    // implicit presence, and a value with no row is rejected: acr -check catches
    // the spelling when the row goes through the database, but amc reads a schema
    // out of -in_dir too. A misspelling and an empty value are both reported. The
    // two accepted values stand as controls, and the report shows what each one
    // emits: proto2 writes the scalar unconditionally, proto3 guards it with the
    // default-value test.
    // void comptest_amc_BadPbufSyntax(); // gstatic/atfdb.comptest:amc.BadPbufSyntax

    // The presence dimension of a oneof, over four ctypes in one run. A oneof is
    // a union, and the encoder writes at most one variant only because a store
    // into one variant clears the presence bits of its siblings on the same
    // pmask. Three shapes break that and are rejected: variants with no pmask at
    // all (each guarded by the proto3 default-value test, so every nonzero
    // variant goes on the wire), variants split across two pmasks (a store
    // clears siblings only on the mask it writes), and a pmask whose explicit
    // member list covers one variant and not the other. The first ctype is the
    // control, with both variants on one pmask, and it draws no diagnostic.
    // void comptest_amc_BadPbufOneof(); // gstatic/atfdb.comptest:amc.BadPbufOneof

    // The whole store dimension of one cpbuf ctype in one run: the decoder of a
    // singular field with no member of its own stores through the field's
    // generated Set, so a shape that has a Get and no Set emits a call to a
    // function amc never writes. Three such rows are rejected -- a cppfunc with
    // set:N, a substr, a falias -- and the shapes that do have a Set stand
    // alongside them as controls drawing no diagnostic: an fbigend field, a
    // bitfld, a cppfunc with set:Y, and a plain Val field stored in place. The
    // second ctype carries the repeated controls, where the decoder appends
    // through Alloc and a fldfunc element type needs no Set at all.
    // void comptest_amc_BadPbufStore(); // gstatic/atfdb.comptest:amc.BadPbufStore

    // An fpbuf field whose pb_type has no amcdb.pbtype row: without the table's
    // wire type and codec binding the encoder would write the field tag with no
    // payload and the decoder case would consume the tag but not the value, so
    // amc rejects the field.
    // void comptest_amc_BadPbufType(); // gstatic/atfdb.comptest:amc.BadPbufType

    // A u8 pmask covering nine member fields: nine presence bits cannot fit in
    // the pmask field's eight bits.
    // void comptest_amc_BadPmaskWidth(); // gstatic/atfdb.comptest:amc.BadPmaskWidth

    // Explicit pmaskfld_member rows naming, one per row, every field the
    // membership rule excludes: a field of another ctype, the Global anchor, the
    // pmask word itself, a length field, a type field, and a Base field. None of
    // them has a presence bit the pmask's parent struct could back, so amc
    // rejects each row by name. Auto-populated members satisfy the rule by
    // construction; an explicit row must pass the same validation, and one row
    // per exclusion cause keeps every arm of the rule load-bearing. The type
    // field and the Base field cannot both sit on one ctype -- a type field is
    // declared on the base, a Base field is the deriving ctype's first field --
    // so Hdr and Msg carry those two, each with its own pmask.
    // The width check counts a rejected row's bit like any other: the membership
    // rule reports a row and leaves it on the pmask. This fixture declares the
    // pmask a u32 and puts five member rows on it, so the count stays far inside
    // the width and the golden carries the membership diagnostics alone.
    // void comptest_amc_BadPmaskMember(); // gstatic/atfdb.comptest:amc.BadPmaskMember

    // Two ctypes containing each other by value, and one containing itself: no
    // finite layout exists, so no struct's size can be computed.
    // Each cycle also carries a decimal field, which asks for the arg's numeric
    // range before the size computation runs. The range is resolved by walking
    // the chain of ctypes whose single Val field names the next one, and that
    // walk is what a cycle would send around forever, so the fixture holds the
    // two shapes a cycle can take -- a ctype naming itself and a pair naming each
    // other. Each field is reported as an arg carrying no numeric range, and the
    // run reaches the size computation and ends at the circular definition.
    // A bitfield asks the same walk two further questions, and the fixture puts a
    // cycle in front of each: the type its accessors hand back, which the
    // bitfield's own arg names, and the type its mask operations run on, which
    // its source field's arg names. Neither question turns the cycle into an
    // internal error; the run still ends at the circular definition.
    // void comptest_amc_BadSizeCycle(); // gstatic/atfdb.comptest:amc.BadSizeCycle

    // An Inlary of 65537 elements, each itself a 65536-byte fixed Inlary: the
    // total byte size exceeds the i32 range amc computes sizes in.
    // void comptest_amc_BadSizeOverflow(); // gstatic/atfdb.comptest:amc.BadSizeOverflow

    // A fixed field after a Varlen field: the varlen data begins at the end of
    // the struct, so writes through the varlen accessors would clobber the
    // trailing fixed member. The schema is rejected with a nonzero exit, the
    // offender attributed to the varlen field it follows.
    //
    // Only a field that declares a member of its own can be clobbered, so the
    // fixture places one field of each shape after the varlen field and the
    // golden is the resulting accept/reject table. Reported: a Val and a Ptr,
    // which declare members directly, and a ZSListMT, whose read-side and
    // write-side list heads are two more members of the parent. Not reported:
    // a Bitfld, which re-slices the word of a field declared before the varlen
    // data; the three fldfunc kinds, whose value is computed rather than stored
    // -- a cppfunc, a falias and a substr; Malloc, Global and Cppstack, whose
    // generators put no member in the struct at all; and an Opt, which draws a
    // comment line where its optional trailing element would sit. Count and Exec are
    // the intermediate case -- the declared field draws nothing, while the Val
    // children amc generates from it are reported in their own right, which is
    // why cnt_n and the child_* rows appear and cnt and child do not.
    //
    // The Opt is the one field here that a second rule answers as well: it
    // claims the end of the fixed portion that the varlen data already claims,
    // so the golden carries its amc.trail_overlay row, and the absence of an
    // amc.varlen_last row beside it is what pins the Opt as declaring no
    // storage.
    // void comptest_amc_BadVarlenLast(); // gstatic/atfdb.comptest:amc.BadVarlenLast

    // Fields that begin at the end of the fixed portion: varlen data and an
    // optional trailing element both start there, and the generated accessors
    // would overlay one on the other. Only varlen fields carry the end offset
    // that lets one begin where another ends, so a ctype may claim that address
    // once, and each further claim is rejected with a nonzero exit.
    // The fixture pins the rule over the pairs a ctype can declare. Rejected:
    // Varlen with Opt in both declaration orders, one ctype each, and a ctype
    // with two Opt fields, whose getters would otherwise return the same address
    // twice. A fourth ctype declares an Opt ahead of two varlen fields and draws
    // one row, not two: the first varlen field is the one that begins at the
    // disputed address, and the second begins where the first ends, so a single
    // overlay is reported once however many varlen fields trail it.
    // Each rejected ctype draws exactly one diagnostic -- the order of a
    // Varlen and an Opt matters to the separate rule that fixed fields may not
    // follow a varlen field, and reporting the Opt there would state the same
    // overlay twice. The controls draw none: a ctype with one Opt, a ctype with
    // one Varlen, a ctype with two Varlen fields -- the accepting arm of the
    // rule, the second beginning where the first ends -- and a ctype with
    // neither. The two-Varlen control carries no pool instance, since a pool arg
    // with more than one varlen field is refused by the pool generator, whose
    // diagnostic would then arrive alongside the ones this test is about.
    // void comptest_amc_BadVarlenOpt(); // gstatic/atfdb.comptest:amc.BadVarlenOpt

    // A non-varlen pool instance of a varlen ctype: the pool allocates fixed-size
    // records, so the trailing varlen data has nowhere to live. The offending
    // instance is reported and the schema is rejected with a nonzero exit.
    // void comptest_amc_BadInfinityPool(); // gstatic/atfdb.comptest:amc.BadInfinityPool

    // A pool arg with a varlen field but no lenfld: the element count is an
    // extern user function, and Delete computes the total length from it. The
    // query prints the generated Delete; the length statement must be
    // well-formed (no stray semicolon inside the length expression).
    // void comptest_amc_PoolVarlenExtern(); // gstatic/atfdb.comptest:amc.PoolVarlenExtern

    // A pooled ctype whose fixed size plus lenfld extra is not a multiple of the
    // lenfld scale: pool alloc stores len = (size + extra) / scale, which
    // truncates, and the reader reconstructs less than was allocated.
    // void comptest_amc_BadLenfldScale(); // gstatic/atfdb.comptest:amc.BadLenfldScale

    // A pool-less message ctype whose fixed size is not a multiple of the lenfld
    // scale: the message constructor and the field default store the same
    // truncating length word pool alloc would, so the divisibility check must
    // cover message ctypes, not only pooled ones.
    // void comptest_amc_BadLenfldMsgtype(); // gstatic/atfdb.comptest:amc.BadLenfldMsgtype

    // A fixed-only ctype whose only length store is its Fmt* constructor (a pnew
    // row, no msgtype, no pool instance): the constructor's runtime total is
    // always the fixed size, so an indivisible size + extra stores the same
    // truncating length word pool alloc would, and the divisibility check must
    // cover pnew carriers too.
    // void comptest_amc_BadLenfldPnew(); // gstatic/atfdb.comptest:amc.BadLenfldPnew

    // Three lenflds with scale:0 on ctypes whose Opt fields consume the length
    // expression: every offender is reported in one run through the accumulate
    // path, not a single throw at the first length-expression consumer. MsgC adds
    // the pooled shape with a u64 length word, the one that reaches the pool
    // InsertMaybe raw-word bound -- that bound divides by the scale, and it is
    // emitted a whole gen step before gen_check_lenfld reports the scale, so
    // without its own scale test the run would die on SIGFPE with no diagnostic
    // at all. The cloned base field (FMsgC.len) reports the row a second time.
    // void comptest_amc_BadLenfldZeroScale(); // gstatic/atfdb.comptest:amc.BadLenfldZeroScale

    // The declared-type dimension of the length field, one ctype per case. A
    // length word needs a numeric range: GetMinMax resolves one for the
    // fixed-width integers u8 through i64 and for nothing else, and every range
    // check built from the widest-signed fallback runs vacuously (the TS encoder's
    // range guard is omitted outright). So u128 is refused for having no
    // arithmetic amc can do, bool for holding no magnitude, and char for having
    // the compiler's choice of signedness -- three rejections in one run. MsgOk
    // carries the same shape over a u16 word and draws nothing, which is what
    // makes the three above a rejection of the type rather than of the shape.
    // void comptest_amc_BadLenfldType(); // gstatic/atfdb.comptest:amc.BadLenfldType

    // A u8 lenfld (scale 4) on a pooled + pnew ctype: every store of a runtime
    // total guards the length field's storable range, not just scale
    // divisibility -- a total beyond the range would wrap mod 2^N through the
    // word's own type, framing a shorter message than was written. The pool's
    // AllocExtraMaybe bounds the caller's addon (range minus the fixed portion,
    // as a compile-time ssizeof expression) and the Fmt* constructor refuses
    // the total (NULL), mirroring the TS encoder's RangeError guard.
    // MsgW is the same shape with a wide length field (u32, scale 1): its range
    // covers every frame length, so no lenfld test is emitted, and what remains
    // is the frame length domain itself. Its buffer is an Alloc, whose size
    // argument is an int, so the emitted bound is the i32 maximum and the totals
    // it accepts are exactly those the allocator can be asked for.
    // void comptest_amc_LenfldNarrow(); // gstatic/atfdb.comptest:amc.LenfldNarrow

    // Two jstype ctypes sharing one u32 lenfld shape, differing only in whether
    // the frame length is a compile-time constant. MsgFix is fixed-size, so the
    // emitted `length` is a literal and gen_check_lenfld has already proven it
    // storable: the encoder carries no range guard at all. MsgVar owns a varlen
    // tail, so its length is a runtime sum and the guard stays. The test writes
    // real output (amc runs from the checkout root with -out_dir under the test's
    // tempdir, so the derived-table acr child writes into that tempdir too) and
    // pins both emitted TypeScript encoders.
    // void comptest_amc_JsFixedFrame(); // gstatic/atfdb.comptest:amc.JsFixedFrame

    // Varlen pools with InsertMaybe: the addon count of an untrusted source
    // struct is computed in i64 domain. A multiply in the length word's own
    // type would wrap a huge corrupt stored value mod 2^N into a small
    // positive count that passes the negative-addon guard and reads past the
    // source message; the widened arithmetic keeps the true total visible,
    // and totals outside [fixed size, i32 max] return NULL. That widening is
    // itself exact only while the word's range fits u32. Beyond it the i64
    // expansion can overflow -- with scale > 1 in the multiply, and even at
    // scale 1 in the extra adjustment after the wrap -- undefined behavior an
    // optimizer may exploit to elide the range test -- so a wider word is
    // first bounded by the largest value whose expanded total fits the i32
    // frame-length domain, a generation-time constant.
    //
    // The fixture is the rule's table, one pool ctype per cell. Which arm
    // applies turns on the word's largest storable value, so the accepted
    // edges sit either side of u32's max: MsgU32 (u32, scale 4), MsgI32
    // (i32) and MsgBit32 (u64 word masked to 32 bits by its Bitfld width)
    // draw no bound, while MsgBit33 (masked to 33 bits), MsgU64 (u64,
    // scale 2) and MsgI64 (i64, scale 8) do. A signed word is the cell
    // where the bound rather than the addon test rejects a negative stored
    // length, through the u64 conversion in the emitted test. The bound's
    // constant follows scale and extra: MsgNeg carries extra -2 and MsgPos
    // extra 4, each shifting the constant by the reader formula.
    //
    // InsertMaybe reaches the allocation through the die-on-fail AllocExtra,
    // so a count AllocExtraMaybe would refuse kills the process instead of
    // returning NULL, and the two accept exactly the same counts. Both
    // functions are therefore printed for every pool in the fixture, and a
    // term emitted into one and not the other shows up as a diff here. Two
    // cells of the table make that agreement worth pinning: the bound's
    // constant is the row type's, not the base type's, which msg6 and msg5
    // show as block pools whose row carries a buffer member the base does
    // not; and a word storing totals past i32 max leaves the length field
    // with no bound to give, so every such pool bounds the count by the i32
    // a frame length is read back through instead.
    // void comptest_amc_PoolInsertScale(); // gstatic/atfdb.comptest:amc.PoolInsertScale

    // Two Bitfld lenflds whose stores mask to the declared width: MsgWidth
    // (width 4, u32 source word, msgtype store, fixed size 20) can store at
    // most 15, so the minimum frame's length word is unstorable; MsgExtra
    // (width 6, extra:252) can store at most 63, so no frame length is
    // representable at all. Both range checks must bound by the width-limited
    // max, not the source word's full range, and report in one run.
    // void comptest_amc_BadLenfldBitfld(); // gstatic/atfdb.comptest:amc.BadLenfldBitfld

    // A u8 lenfld with extra:256: every value the type can hold maps to a
    // negative frame length, so no frame at all is representable. Left
    // unrejected, the TS encoder's range-guard bound maxval*scale - extra
    // wraps in u64 and the emitted guard never fires; the schema must be
    // refused at generation time instead.
    // void comptest_amc_BadLenfldExtra(); // gstatic/atfdb.comptest:amc.BadLenfldExtra

    // Two packed jstype ctypes whose fixed size plus lenfld extra falls outside
    // the length field's range, with no msgtype and no C++ alloc instance: the
    // generated TypeScript Encode is their only store site. MsgLow (u32 lenfld,
    // extra:-8, fixed size 4) would wrap the minimal frame's stored length
    // through setUint32; MsgHigh (u8 lenfld, extra:255, fixed size 4) would get
    // a range guard that throws on every encode. Both offenders are rejected in
    // one run.
    // void comptest_amc_BadLenfldJstype(); // gstatic/atfdb.comptest:amc.BadLenfldJstype

    // The wire-form dimension of a packed jstype ctype's fields: a field the
    // TypeScript codec can neither encode nor decode is refused, and every
    // offender is named in one run. MsgBad carries three -- an inlary of char and
    // an inlary of a cstr ctype, both of which the class projects as `string`
    // with no byte-copy form, and a Smallstr, a storage reftype the codec maps
    // nowhere -- so the walk cannot be stopping at the first. MsgBadOpt adds the
    // fourth, an Opt of the same cstr ctype, which is the same byte-copy rule
    // read on the other reftype that uses it. MsgBadInlary adds the fifth, an
    // inlary of u8 with min:0 max:8: the class projects as a byte array, but a
    // variable inline array stores a live element count after its elements, so
    // its width is not max elements and the count has no member to carry it.
    // MsgBadVal adds the sixth, a Val of a ctype that declares a Varlen: the
    // nested codec appends a tail past the width the outer walk steps over.
    // MsgBadValOpt adds the seventh, the same rule read on a nested Opt tail.
    // MsgBadVarlen adds the eighth, a Varlen of the cstr ctype, whose member
    // projects as a string while its tail is a run of records rather than text.
    // MsgTagged closes the list with the ninth and tenth, two inline arrays of
    // char on a ctype that carries a length word and nothing else.
    //
    // MsgNolen and MsgNolenOpt read the other dimension, the ctype's own: each
    // declares a tail -- two Varlen fields, and an Opt -- with no length field to
    // frame it, and each draws amc.jstype_lenfld. MsgFixed is that rule's control,
    // a packed jstype ctype with no tail and no length field, and it draws
    // nothing.
    //
    // The rest of the fixture is the accepted side of that table and draws
    // nothing: MsgOk carries a Base, a lenfld, a plain Val, a Bitfld, a Val of the
    // cstr ctype (a fixed-length string does have a wire form), a Val of a
    // tail-free ctype, an inlary of u8, and the three accepted Varlen args -- char
    // for the text tail, u8 and a plain ctype for the byte tail; MsgOpt carries an
    // Opt whose arg maps to a byte array.
    // The two remaining ctypes fix the check's scope: MsgUnpacked and MsgNojs both
    // carry the offending inlary of char, and neither is checked, because a ctype
    // with no pack row and a ctype with no jstype row get no codec to be wrong.
    //
    // The run writes into $tempdir, which holds none of the gen directories amc
    // writes to, so a generated file would report amc.outfile_write naming the
    // path it could not create. No such line appears, and that absence is the
    // assertion this run makes about ordering: check_jstype sits ahead of every
    // emitting gen, so a refused schema is refused before the first write and no
    // decoder is emitted for the fields it accepted either.
    // void comptest_amc_BadJstypeWire(); // gstatic/atfdb.comptest:amc.BadJstypeWire

    // Two store-carrying ctypes whose fixed size plus lenfld extra falls outside
    // the length field's range. MsgLow (u32 lenfld, extra:-8, fixed size 4): the
    // minimum frame's stored length goes negative and wraps through the unsigned
    // store, framing gigabytes from a 4-byte message. MsgHigh (u8 lenfld,
    // extra:255, fixed size 4): the stored value 259 exceeds the type's max, so
    // the emitted range guard bound falls below the fixed size and every encode
    // throws at first use. Both offenders are rejected in one run.
    // void comptest_amc_BadLenfldMinFrame(); // gstatic/atfdb.comptest:amc.BadLenfldMinFrame

    // Two packed jstype ctypes whose lenfld sits on a Bitfld field over a raw
    // header word, with no msgtype and no C++ alloc instance -- header-like
    // framing for an external payload. The TS fixed walk writes the source word
    // raw from the parent and emits no store for a Bitfld lenfld, so per the
    // no-store-site doctrine the length words arrive from outside and the
    // schemas stay legal: MsgIndiv (size 4, scale:8) is indivisible and MsgRange
    // (u8 lenfld, extra:252, size 4) has an unstorable minimum frame, yet amc
    // accepts both with exit code 0.
    // void comptest_amc_LenfldBitfld(); // gstatic/atfdb.comptest:amc.LenfldBitfld

    // A packed message based on a private header ctype that has no dmmeta.pack
    // row -- the state an author lands in after repointing a message's base from
    // ams.MsgHeader to a freshly created header; the namespace is itself pack:Y,
    // so the unpacked header also draws the namespace-level error. Pins both
    // rejections (amc.back_pack, amc.what_the_pack), each followed by the bare
    // dmmeta.pack row the message tells the author to insert.
    // void comptest_amc_MissingPack(); // gstatic/atfdb.comptest:amc.MissingPack

    // A pmask field of 536870913 bytes: its bit count exceeds i32, which the
    // struct size cap (i32 with alignment headroom) permits. The bit-width
    // bookkeeping must not wrap; the schema is legal and generates cleanly.
    // void comptest_amc_PmaskGiantField(); // gstatic/atfdb.comptest:amc.PmaskGiantField

    // csize rows with alignment 32 (beyond the headroom the struct size cap
    // reserves for RoundUp) and alignment 6 (not a power of two, so the rounding
    // mask miscomputes): both must be rejected, not silently accepted.
    // void comptest_amc_BadCsizeAlignment(); // gstatic/atfdb.comptest:amc.BadCsizeAlignment

    // A variable char/u8 inlary with min>0: the string read copies the input
    // and sets the count to its length, which a short input puts below min.
    // The shape is rejected until someone defines floor semantics for it.
    // void comptest_amc_BadInlaryMin(); // gstatic/atfdb.comptest:amc.BadInlaryMin

    // An inlary with min>max: Init preallocates min elements against a
    // capacity of max, so the first record constructed would die at runtime.
    // amc rejects the shape instead of shipping the trap.
    // void comptest_amc_BadInlaryMinmax(); // gstatic/atfdb.comptest:amc.BadInlaryMinmax

    // A variable inlary with fnoremove: Setary, the separated-string read, and
    // Uninit all empty the array through RemoveAll, which fnoremove suppresses,
    // so the generated code would call a function that is never emitted.
    // void comptest_amc_BadInlaryFnoremove(); // gstatic/atfdb.comptest:amc.BadInlaryFnoremove

    // A Json cfmt over fields no FmtJson branch covers. Two causes: a reftype the
    // walk has no branch for -- a raw Smallstr field, shown both as a ctype's
    // single datafld and among several -- and a field type carrying neither a Json
    // cfmt nor a String print, so no expression exists to produce its node, shown
    // singular and repeated. Every offender is rejected in one run through the
    // accumulate path.
    // void comptest_amc_BadJsonFld(); // gstatic/atfdb.comptest:amc.BadJsonFld

    // How each field of a Json cfmt reaches its node. A field whose type carries
    // its own Json cfmt gets that type's FmtJson; a field whose type carries only
    // a String cfmt prints into a string node instead. A repeated field becomes an
    // array node holding one element node per element, by the same two rules.
    // A ctype with one data field prints as that field's node, with no object
    // wrapper, and the same rules pick the node there: the field type's FmtJson,
    // or a string node its Print writes into, and for a repeated field an array
    // node holding one element node per element.
    // One ctype is the case where the two field selectors used to disagree: its
    // only data field is a Val, so the object wrapper is elided and no object node
    // exists, and its other field is a repeated fldfunc, which the array branch
    // would have emitted an array node into.
    // The remaining ctypes carry a print filter, which prints a field only when
    // its bit is set. PmFilter shows the three member forms each inside a test of
    // their bit, and its presence mask is a data field itself, which by itself
    // keeps the ctype from inlining. PmInlJson and PmInlPrint are the shapes where
    // the mask is a bitfield of the one data field: that field carries a bit, so
    // it is not inlined and prints as an object member under its test -- the first
    // through its type's FmtJson, the second through its type's Print. PmInlJson
    // carries a second bitfield on the mask, and a bitfield prints nothing, so no
    // test is opened around it. PmInlFree keeps the same shape but names its
    // member explicitly, leaving the data field off the mask, and that field
    // inlines as it would with no filter at all.
    // void comptest_amc_JsonFld(); // gstatic/atfdb.comptest:amc.JsonFld

    // A Json cfmt on a base-derived ctype and on a bitfield-carrying ctype:
    // neither the Base field nor the Bitfld field prints — the base's cloned
    // fields print as members of the object, and the bitfield's bits travel
    // inside the printed source word — so both schemas generate cleanly.
    // Pins both emitted FmtJson bodies.
    // void comptest_amc_JsonBaseBitfld(); // gstatic/atfdb.comptest:amc.JsonBaseBitfld

    // A Json cfmt over the whole rendering table: a field renders through the
    // Json cfmt of its type when the type has one, and otherwise as a string
    // holding what its Print writes; an array field renders as a JSON array
    // whose elements render the same two ways; and a record standing on a single
    // data field renders as that field's own node, so an array there is the
    // record's array and a value there is the record's value. AryPlain, AryElem,
    // ValPlain and ValElem are the four single-field cells, Rec the two array
    // cells as members of an object beside a plain value.
    // RecName is the naming control, and every one of its fields is named for a
    // local the walk declares. A local named after a field takes that name through
    // one role suffix -- "_node" for the node standing for the field, "_ary" for
    // its element span -- and the record's own node is named objnode, which no
    // suffix produces. So the field named "objnode" reaches for the record's node,
    // the field named "x_field" reaches for the node of the field named "x", and
    // the field named "y_ary" reaches for the element span of the field named "y",
    // and the emitted body declares each of them once. The field named "object"
    // pins the same rule from the other side: the record's node no longer answers
    // to that name, so a field may.
    // void comptest_amc_JsonAry(); // gstatic/atfdb.comptest:amc.JsonAry

    // A Json cfmt on a global (FDb) ctype: the Global anchor field is the
    // singleton instance itself and carries no data, so — like Base and Bitfld —
    // it passes the field check without printing, and the object holds the
    // ctype's data fields. Anchor, Derived and Presence carry that to the
    // degenerate end: their one and only data field carries nothing of its own (a
    // Global, a Base, a presence mask), so the walk emits no member at all and
    // each must still return an empty object rather than a null node. One ctype
    // per cause keeps every arm of that rule load-bearing.
    // The FDb ctype also carries the repeated fields, which reach their elements
    // through the field's own Getary. A global's accessors take no parent
    // argument, so those calls are emitted with none: a Tary whose element type
    // prints as a string, a Tary whose element type has a Json cfmt of its own,
    // and an Inlary. GlobAry is a second global ctype whose only data beside the
    // anchor is one Tary, which keeps the collapse pinned apart from FDb's other
    // members. Pins every emitted FmtJson body in the fixture.
    // void comptest_amc_JsonGlobal(); // gstatic/atfdb.comptest:amc.JsonGlobal

    // A single-file universe with a Ptrary, an fcurs request for its cursor, and
    // no amcdb.tcurs rows: the request makes the cursor generator run, and the
    // generator reports the missing tcurs row as a generation error and keeps
    // generating. check_fcurs flags the same request first (bad_fcurs),
    // so the run accumulates both errors and exits with their count.
    // void comptest_amc_BadMissingTcurs(); // gstatic/atfdb.comptest:amc.BadMissingTcurs

    // A single-file universe with a bitset field and no amcdb.tcurs rows: the
    // tcurs row is the only carrier of the dflt flag, so amc cannot know the
    // bitcurs cursor is request-only. Without a dmmeta.fcurs request the cursor
    // must not be emitted; the NBits witness proves generation itself ran.
    // void comptest_amc_CursUnrequested(); // gstatic/atfdb.comptest:amc.CursUnrequested

    // A field default naming the containing instance retargets to the emitting
    // context's instance name, in both spellings C++ gives that instance. The
    // object spelling *this becomes the instance itself (Msg Init assigns
    // ssizeof(parent)), and the pointer spelling this becomes its address,
    // parenthesized, so that the member access in this->pad binds to the whole
    // address expression rather than to the member (Ptr Init assigns
    // (&parent)->pad). An identifier that merely starts with the same characters
    // (*thisvalue) is not an instance reference and survives verbatim. Rec's default
    // carries the object spelling next to that longer identifier and Ptr's carries
    // the pointer spelling next to it, so each pins that one expression keeps the
    // longer identifier while its real reference is retargeted.
    // void comptest_amc_DfltRetarget(); // gstatic/atfdb.comptest:amc.DfltRetarget

    // amc run from a directory with no bin/acr (outside a checkout): the acr
    // subprocess that rewrites the ctypelen/dispsig/tracefld/tracerec/userfunc
    // tables cannot start, and the run must fail instead of exiting 0 with the
    // tables silently unwritten. The command cds into the test's tempdir so
    // the relative bin/acr path resolves nowhere; out's gen directories are
    // created up front so the output-file writes succeed and only the acr
    // failure is under test.
    // void comptest_amc_TableWriteAcrFail(); // gstatic/atfdb.comptest:amc.TableWriteAcrFail

    // amc pointed at an -out_dir whose gen subdirectories do not exist: every
    // output-file write fails, and the run must fail naming each path instead
    // of exiting 0 with the generated code silently missing.
    // void comptest_amc_OutfileWriteFail(); // gstatic/atfdb.comptest:amc.OutfileWriteFail

    // amc -e whose acr -e child fails (exit 1, here via an editor that fails):
    // the run must fail loudly before regenerating anything, instead of reading
    // the nonzero exit as an edit that modified files and regenerating on top of
    // a schema that never changed. The failure is reported and the phases that
    // would read the input set are skipped, so the run ends through its own exit
    // path and its report names the files it did not write. The command cds into the test's tempdir and
    // symlinks bin and data from the checkout, so amc's relative bin/acr spawn
    // and acr's data load both resolve while every write stays in the tempdir.
    // void comptest_amc_EditFail(); // gstatic/atfdb.comptest:amc.EditFail

    // A FAST template whose presence mask is a field named nullable, not pmask.
    // The presence accessors amc generates are named after the pmask row -- the
    // row's field name prefixes the whole-word bitset accessors and its funcname
    // spells the per-field ones -- so a codec that spells those names itself
    // works only for a mask that happens to be called pmask. The first run pins
    // the emitted encoder and decoder: the message's own presence reads through
    // Value_NullableQ and writes through nullable_qSetBitVal, while the state
    // record's two masks, named assigned and present, keep the accessors those
    // names generate.
    // The FAST codec qualifies each field's previous value through two pmasks of
    // the namespace's FastState record, told apart by name; the second run gives
    // the presence one another name, which leaves the codec with no accessor to
    // call, and pins the diagnostic and the nonzero exit that replace the
    // silently unnamed accessor. The row that is missing belongs to the
    // namespace, so that universe also pins how often it is charged: two
    // templates of the namespace need it, each through an encoder and a decoder,
    // and it is reported once with exit code 1. The universe's other two
    // namespaces draw nothing -- one holds no cfast ctype at all, the other holds
    // one encoded as a type, which is coded inline by whichever message holds it
    // and needs no state pmask of its own.
    // void comptest_amc_FastPmaskName(); // gstatic/atfdb.comptest:amc.FastPmaskName

    // Which argv accessors each of the two argv strfmts generates. The universe
    // holds one ctype per strfmt over the same three fields, and both cfmts ask
    // for print and read alike, so the only thing that varies between them is the
    // strfmt in the cfmt key. Argv gets the whole surface: the two printers, the
    // ToCmdline convenience that prefixes bin/ and inherits the verbosity flags,
    // the NArgs token-count table, and the ReadArgv parser the namespace ReadArgv
    // delegates to. ArgvGnu gets the two printers and nothing else, so its read:Y
    // buys no command-line reader; the prototypes are printed rather than the
    // bodies, since what is under test is which functions exist.
    // void comptest_amc_ArgvAccessor(); // gstatic/atfdb.comptest:amc.ArgvAccessor

    // An fconst whose own value the field's store cannot hold. The symbol reaches
    // the field through the generated Set, which for a bitfield masks the store, so
    // the value the schema names is not the value the field ends up holding and no
    // accessor can map it back to the symbol; amc rejects each such row by name and
    // states the range the store does hold. The rejected values walk the stores: a
    // value past the bits (2000 in ten bits), past the arg type (300 in a u8), past
    // a signed arg type's maximum (2147483648 in an i32), a negative on an unsigned
    // store, and a negative on bits that drop a signed arg type's sign.
    // The value is a C++ integer literal, and amc reads it the way the compiler
    // will: 0377 is octal 255 and fits a u8 store, 0400 is 256 and does not, a
    // u/U/l/L suffix belongs to the literal's type rather than to its number, and a
    // leading plus or minus is the number's sign. Sign and magnitude are carried
    // apart, so an unsigned 64-bit store holds 0xffffffffffffffff while an i32
    // store rejects -0x80000001, one past its minimum. A magnitude that outruns 64
    // bits -- 0x10000000000000001, or twenty-three decimal digits -- fits no store
    // and is reported against the widest one.
    // The controls that draw nothing are the values at each store's own edges (the
    // u32, u8 and u64 maxima, the i32 minimum in decimal and in hex, 1023 in ten
    // bits, 127 in seven, -128 in eight in decimal and in hex), and every spelling
    // that is not one whole integer literal and so carries no number to compare: a
    // shift expression, an arithmetic one, digits with trailing text, a character
    // literal, a name only the compiler resolves, and a binary literal, a spelling
    // amc does not read. A string field's value is a string however it is spelled,
    // so the string field carries one control reading csv and one reading 99999,
    // and neither is bounded: the field's arg is a character array, which resolves
    // to no integer type and so has no range to compare with. The mask of a flag on
    // the top bit of a signed word is likewise unbounded, being a bitfield's mask
    // rather than a value of the word.
    // void comptest_amc_BadFconstRange(); // gstatic/atfdb.comptest:amc.BadFconstRange

    // The declared bitfield widths an fconst's store range is computed from, over
    // one universe holding a field per width. A bitfield's Set masks the store to
    // the declared width, so the range an fconst value is checked against is built
    // by shifting a u64 by that width -- and the width is a signed schema argument
    // that a run reaches before the accessors refuse it, so it arrives here holding
    // whatever the schema wrote. The widths from one to sixty-three are the ones
    // that name bits inside the word: each of those fields carries the widest value
    // its bits hold, which draws nothing, next to the value one past it, which is
    // reported. A width of sixty-four spans the arg type and masks nothing, so the
    // arg type's own maximum draws nothing. The three widths outside the range --
    // negative, zero, and sixty-five bits of a u128 source field -- are reported as
    // out of bounds naming the field, and the value 1 that sits on each of them
    // draws no range complaint of its own: a width that names no bits bounds
    // nothing. A run that computes a bound from one of them instead reports 1 as a
    // value the field cannot hold, which the field's arg type holds perfectly well.
    // The bitfield accessors end the run at the first width they refuse, which is
    // the last line of the golden.
    // void comptest_amc_FconstBitfldWidth(); // gstatic/atfdb.comptest:amc.FconstBitfldWidth

    // -------------------------------------------------------------------
    // cpp/atf_comp/apm.cpp
    //

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_apm_DryRun(); // gstatic/atfdb.comptest:apm.DryRun

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
    // void comptest_apm_RecfileWriteFail(); // gstatic/atfdb.comptest:apm.RecfileWriteFail

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
    // void comptest_apm_UpdateFate(); // gstatic/atfdb.comptest:apm.UpdateFate

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
    // void comptest_apm_TransactionExit(); // gstatic/atfdb.comptest:apm.TransactionExit

    // -------------------------------------------------------------------
    // cpp/atf_comp/aqlite.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_aqlite_CompileOptions(); // gstatic/atfdb.comptest:aqlite.CompileOptions
    // void comptest_aqlite_Constraints(); // gstatic/atfdb.comptest:aqlite.Constraints
    // void comptest_aqlite_ErrorHandling(); // gstatic/atfdb.comptest:aqlite.ErrorHandling
    // void comptest_aqlite_Joins(); // gstatic/atfdb.comptest:aqlite.Joins
    // void comptest_aqlite_Number(); // gstatic/atfdb.comptest:aqlite.Number
    // void comptest_aqlite_PatternMatching(); // gstatic/atfdb.comptest:aqlite.PatternMatching
    // void comptest_aqlite_Performance(); // gstatic/atfdb.comptest:aqlite.Performance
    // void comptest_aqlite_Smoke(); // gstatic/atfdb.comptest:aqlite.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_cmdline.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_atf_cmdline_Bare(); // gstatic/atfdb.comptest:atf_cmdline.Bare
    // void comptest_atf_cmdline_Debug(); // gstatic/atfdb.comptest:atf_cmdline.Debug
    // void comptest_atf_cmdline_Help(); // gstatic/atfdb.comptest:atf_cmdline.Help
    // void comptest_atf_cmdline_Minimal(); // gstatic/atfdb.comptest:atf_cmdline.Minimal
    // void comptest_atf_cmdline_MinimalExec(); // gstatic/atfdb.comptest:atf_cmdline.MinimalExec
    // void comptest_atf_cmdline_Rich(); // gstatic/atfdb.comptest:atf_cmdline.Rich
    // void comptest_atf_cmdline_RichExec(); // gstatic/atfdb.comptest:atf_cmdline.RichExec
    // void comptest_atf_cmdline_Sig(); // gstatic/atfdb.comptest:atf_cmdline.Sig
    // void comptest_atf_cmdline_Verbose(); // gstatic/atfdb.comptest:atf_cmdline.Verbose
    // void comptest_atf_cmdline_Version(); // gstatic/atfdb.comptest:atf_cmdline.Version

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_comp.cpp
    //

    // Run every selected comptest and report how many passed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:atf_comp

    // -------------------------------------------------------------------
    // cpp/atf_comp/gcache.cpp -- Comptests for gcache
    //

    // gcache asked to enable a cache directory that does not exist, wrapping a
    // command that succeeds: the missing directory is reported and the run fails.
    // A wrapped command's status adds to the run's exit code instead of replacing
    // it, so a successful compile cannot mask the setup failure reported ahead of
    // it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_gcache_CacheDirFail(); // gstatic/atfdb.comptest:gcache.CacheDirFail

    // A coverage cache hit whose object file cannot be written, and the mirror
    // case where its coverage notes cannot be written. A cached coverage entry is
    // one blob holding both halves, so that a hit can never pair a fresh .o with a
    // stale .gcno; the restore has to keep that promise when only one half is
    // writable. Each half's path is made unwritable in turn by holding it with a
    // directory. Neither half may be left fresh on its own, and the run fails
    // either way. The last run is the control: with both paths free the pair
    // restores and the run succeeds.
    // void comptest_gcache_CoverageRestoreFail(); // gstatic/atfdb.comptest:gcache.CoverageRestoreFail

    // A cache entry that is not a whole coverage blob counts as a miss, so the
    // compile runs again and republishes the entry.
    // The cache directory outlives the builds that write it: another tool, an
    // older gcache, an interrupted write can all leave a file under a coverage key
    // that is not two non-empty halves. Restoring from one of those puts a fresh
    // object next to whatever .gcno the working directory happens to hold, which
    // gcov then rejects on a stamp mismatch and reports as 0% coverage for that
    // object -- and the build sees no error, because a restore that was asked for
    // half a pair and delivered it reports success. So validity is decided from
    // the bytes: an entry is used only when the offset it starts with leaves bytes
    // on both sides of itself.
    // Each stage forges one kind of entry, marks x.gcno with a recognizable string,
    // removes x.o, and runs the compile: WHOLE and AGAIN are the controls that must
    // report a hit, while the rejected forms -- notes half truncated away, offset
    // describing an empty object half, a bare .o left under the coverage key, fewer
    // bytes than the offset itself, and the key held by a directory -- must all
    // report a miss. In every stage the object has to come out non-empty and the
    // marked .gcno has to be gone, because the pair the compile leaves behind is
    // always a pair from one compile.
    // void comptest_gcache_CoverageBlobMiss(); // gstatic/atfdb.comptest:gcache.CoverageBlobMiss

    // Which compiler flags select the coverage cache format, over every spelling
    // gcc accepts for them. A coverage entry is one blob holding the object and its
    // coverage notes, so the format fits exactly the compiles that write notes:
    // -ftest-coverage writes them, --test-coverage is the driver's long form of it,
    // --coverage asks for instrumentation and notes together and -coverage is that
    // flag with one dash, while -fprofile-arcs and its long form --profile-arcs
    // instrument the object and write no notes file at all. A compile that has no
    // notes to publish must still be cached as a bare object; asking it for a blob
    // leaves it with one half, unpublishable, and so uncached forever -- a
    // recompile, a warning, and the same miss on the next build. Each shape is
    // compiled twice with the object and any notes removed in between, so the first
    // run has to miss and the second has to hit, and the notes have to come back
    // exactly for the shapes that produce them. The plain compile with no coverage
    // flag is the control for the bare-object format, and the two spellings of
    // -fprofile-arcs are the controls that keep the rule from reading as a rule
    // about how many dashes a flag carries.
    // The name of the object is the format's other input, because the notes are the
    // object's own name with its extension replaced: an output named x.obj is
    // answered with x.gcno and one named foo with foo.gcno, so a compile whose object
    // does not end in .o writes notes all the same and needs the same blob. Both
    // names are compiled twice the way the flag shapes are, and the notes have to
    // come back for each.
    // An output named the way a precompiled header is named is the third name and the
    // one that looks like an exception: gcc answers an output named h.h.gch with
    // h.h.gcno under -c, and with h.h.gch-h.gcno without it. gcache caches only
    // compiles that carry -c, so the name it derives is the name gcc writes, and the
    // GCHNAME stage pins the notes coming back from a hit on such an output as well.
    // What that stage leaves at h.h.gch is an object and not a precompiled header,
    // because gcache compiles the preprocessed text of the translation unit rather
    // than the header it was handed, so the stage pins the derivation from the name
    // and says nothing about how a precompiled header is cached.
    // void comptest_gcache_CoverageFlag(); // gstatic/atfdb.comptest:gcache.CoverageFlag

    // The modification time of every file a cache hit writes, in both cache
    // formats. A build tool decides that an object is out of date by comparing its
    // modification time against its sources, so an object a hit puts in place has
    // to carry the time of that hit; an object left with an older time is asked for
    // again on the next build, and asked for again after that, because the restore
    // does not clear the condition that triggered it. The case that reaches this is
    // a hit whose cached bytes equal the bytes already at the target -- a header
    // whose touch moved no preprocessed text, so the key still hits while the
    // build tool considers the object stale. Each stage backdates the target to
    // 2020, records MARKER, and runs the compile: the report has to say hit:Y,
    // which is what makes the times meaningful, and each file the hit writes has to
    // come out newer than MARKER. BAREABSENT is the control for a target that is
    // not there at all, and BARESAME is the bare-object format's answer to the same
    // identical-bytes case the coverage format faces in COVSAME.
    // The cache entry the publish writes carries a modification time of its own, and
    // the cleanup reads it: an entry older than the retention window is deleted, and
    // the byte budget evicts in oldest-last-use order. A publish over an entry whose
    // bytes have not changed therefore has to move that time as well, or an entry a
    // build republished every day looks a week old. PUBSAME is that stage: the entry is
    // backdated to 2020 and the same translation unit is published again under -force,
    // with -frandom-seed fixing the coverage notes so the bytes really are identical --
    // which the stage pins next to the time, since a publish that wrote different bytes
    // would freshen the entry whatever rule it used.
    // void comptest_gcache_HitMtime(); // gstatic/atfdb.comptest:gcache.HitMtime

    // A run's exit code sums what it has to report: the wrapped command's own exit
    // status, plus one for each failure that keeps the run from delivering the object
    // it was asked for. So such a failure is not lost behind the command's status or
    // behind another one of its kind. A failure that costs only a future cache miss
    // is reported as a warning and adds nothing, so an entry that cannot be published
    // leaves the code where the compile left it.
    // The failures combined here are independent: the -install marker write, the
    // wrapped command's own status, and a cache hit that cannot write its object
    // file. Each is first shown alone (MARKER_CODE, CMD_CODE, and the object-file
    // case of gcache.CoverageRestoreFail), then paired with the marker failure. The
    // wrapped command in the paired cases is `false`, whose status is 1, so those
    // codes have to reach 2; a command exiting 5 beside the same marker failure
    // exits 6. The marker path is the one setup failure that
    // leaves the cache usable, which is what lets a second, independent failure
    // happen in the same run: the marker is a directory, so writing it fails while
    // the cache directory around it still serves hits. HIT_CODE and CLEAN_CODE are
    // the controls -- a run with nothing to report exits 0.
    // void comptest_gcache_ExitCodeCount(); // gstatic/atfdb.comptest:gcache.ExitCodeCount

    // An entry no compile can be served from -- one of no bytes, or a directory
    // standing at an entry's path -- on the two sides of the cache and in both
    // formats, next to the cleanup pass that plants one.
    // A cache entry can end up empty without anyone noticing. The cleanup keeps an
    // entry a recent log line names by refreshing that entry's modification time, and
    // an entry the byte budget or an earlier age pass already deleted is named by a
    // log line all the same, so a refresh that opens the path for writing recreates
    // it with no bytes in it. The next compile with that key finds a file, calls it a
    // hit, renames the empty file over its object and exits 0; the link that follows
    // is what fails, naming the object as a file of an unrecognized format. Each such
    // hit writes a log line of its own, so the log line stays young and the next
    // cleanup recreates the file the age pass just deleted, and the translation unit
    // stays wedged.
    // A directory standing at an entry's path is the other shape a compile cannot be
    // served from, and the two formats read it differently: the coverage format finds
    // no offset in it and misses, while the bare format took its size for an object's
    // and called it a hit, which fails the copy and leaves the run with no object at
    // all. That one also outlives the build that met it, since a hit publishes
    // nothing, so the key stays wedged until something removes the directory.
    // So an entry has to be a regular file with bytes to be used, and the cleanup has
    // to refresh without creating. The stages follow the chain: RECREATE deletes an
    // entry and runs the cleanup, which must not put the path back; REFRESH backdates an entry
    // past the retention window and runs the cleanup, which must move the entry's
    // time up so the age pass in the same run keeps it, where a refresh that moves no
    // time leaves the entry to be deleted; EMPTYHIT truncates an entry and asks for
    // the compile, which must miss, compile, and republish an entry with bytes;
    // PCHREFRESH is the refresh reaching the other entry a log line can name: a compile
    // that used a precompiled header names the header beside its object, and nothing
    // else moves that header's time, since the compiles that hit it only read it. The
    // stage backdates a freshly built precompiled header past the retention window and
    // runs the cleanup, which must keep it;
    // EMPTYCOV is the same case in the coverage format, which reads the same entry as
    // a blob whose offset leaves no bytes on either side of itself and has always
    // counted it a miss; DIRBARE puts a directory at a bare entry's path and pins that
    // the compile misses, that the publish takes the path back, and that the next build
    // with the same key hits; DIRCOV is the control for that shape in the coverage
    // format, which reached the same verdict from the blob's own bytes; NOPUBLISH
    // compiles to an object that keeps no bytes -- a target symlinked to /dev/null --
    // and pins that nothing is published for it. The
    // last stage is the control, an ordinary hit off an entry that has bytes.
    // void comptest_gcache_EmptyEntry(); // gstatic/atfdb.comptest:gcache.EmptyEntry

    // -------------------------------------------------------------------
    // cpp/atf_comp/jkv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_jkv_ArrayFill(); // gstatic/atfdb.comptest:jkv.ArrayFill
    // void comptest_jkv_ReverseSmoke(); // gstatic/atfdb.comptest:jkv.ReverseSmoke

    // A tool that writes its output file has to check the write: a full disk or an
    // unwritable path otherwise leaves no output and reports success. The output
    // path here has a regular file standing where its parent directory would be, so
    // the write cannot succeed under any filesystem state or privilege level, and
    // jkv names the file and the errno and exits non-zero. The blocking file is
    // seeded in the test's own tempdir, which the harness reclaims per run.
    // void comptest_jkv_WriteFail(); // gstatic/atfdb.comptest:jkv.WriteFail
    // void comptest_jkv_Smoke(); // gstatic/atfdb.comptest:jkv.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/mdbg.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_mdbg_OutOfOrderArgs(); // gstatic/atfdb.comptest:mdbg.OutOfOrderArgs
    // void comptest_mdbg_Smoke(); // gstatic/atfdb.comptest:mdbg.Smoke
    // void comptest_mdbg_SmokeBreak(); // gstatic/atfdb.comptest:mdbg.SmokeBreak
    // void comptest_mdbg_SmokeBreak2(); // gstatic/atfdb.comptest:mdbg.SmokeBreak2

    // -------------------------------------------------------------------
    // cpp/atf_comp/orgfile.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_orgfile_Hash(); // gstatic/atfdb.comptest:orgfile.Hash
    // void comptest_orgfile_MoveByDate(); // gstatic/atfdb.comptest:orgfile.MoveByDate
    // void comptest_orgfile_Dedup(); // gstatic/atfdb.comptest:orgfile.Dedup
    // void comptest_orgfile_DedupPathregx(); // gstatic/atfdb.comptest:orgfile.DedupPathregx
    // void comptest_orgfile_MoveNoop(); // gstatic/atfdb.comptest:orgfile.MoveNoop
    // void comptest_orgfile_ConsumeInput(); // gstatic/atfdb.comptest:orgfile.ConsumeInput
    // void comptest_orgfile_MoveDot(); // gstatic/atfdb.comptest:orgfile.MoveDot

    // -------------------------------------------------------------------
    // cpp/atf_comp/proc.cpp
    //

    // Set test variable and add to Replscope
    void SetVar(strptr name, strptr value);

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

    // Spawn subprocess with $-substitution, return reference.
    atf_comp::FProc &ProcStart(strptr cmd);

    // Write a line to process stdin. If proc.subst, apply $-substitution.
    // A tool that rejects its command line exits before it reads fd 0, so the
    // write can reach a pipe whose read end is already gone. What the tool did
    // with the line reaches the golden through the tool's own output and exit
    // code, and the line itself is recorded above either way, so a broken pipe
    // is the tool's verdict rather than a harness failure and is not diagnosed
    // as one. Any other write error still aborts the test.
    // SIGPIPE is ignored only for the duration of the write: the disposition is
    // process-global and survives exec into every child atf_comp spawns later,
    // so leaving SIG_IGN set would silently change how the tools under test
    // react to their own broken pipes.
    void ProcWrite(atf_comp::FProc &proc, strptr msg);

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

    // Read process stdout until UNTIL pattern is found (or EOF).
    // Empty UNTIL means read until EOF (process exit).
    // Emit output lines to log as "proc -> line".
    // Use Fbuf Linebuf for line reassembly; poll with 1-second timeout
    // for test timeout enforcement.  Without KILL_ON_TIMEOUT a timeout aborts
    // the read (vrfy; the run loop catches it and fails the test).  With it --
    // the cleanup drain -- the proc gets SIGTERM, then SIGKILL after a 5s
    // grace, and reading continues to the EOF the kill produces: cleanup must
    // end the test's procs, never the sweep.
    tempstr ProcRead(atf_comp::FProc &proc, strptr until, bool kill_on_timeout = false);

    // Wait for the cluster under PROC to print READY_FOR_TEST, then take the
    // variables that cluster wrote about itself into the replacement scope.
    //
    // A cluster picks its own ports and addresses as it starts, so a test script
    // cannot name them ahead of time; the cluster writes what it chose to
    // data/atfdb/var.ssim under its instance directory, and from there each becomes
    // a $-substitution the script can use.  Which directory that is comes from the
    // atfdb.testenv row marked vardir -- the same row that told the cluster where
    // to put its instance in the first place, so the two cannot disagree.  A tree
    // with no such row waits for readiness and takes no variables.
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

    // -------------------------------------------------------------------
    // cpp/atf_comp/src_func.cpp -- Comptests for src_func
    //

    // src_func -updateproto against a header whose directory denies the write:
    // the update-hdr block rewrite fails, and the run must fail naming the path
    // instead of exiting 0 with the header silently stale. The seed puts junk
    // inside the update-hdr block so the rewrite has something to change.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_src_func_FileWriteFail(); // gstatic/atfdb.comptest:src_func.FileWriteFail

    // A targsrc source file that does not exist: the scan must fail the run
    // naming the path instead of indexing the file as empty with exit 0, which
    // would silently drop its functions from every listing and header.
    // void comptest_src_func_FileReadFail(); // gstatic/atfdb.comptest:src_func.FileReadFail

    // src_func -updateproto against a targsrc header that does not exist: the
    // rewrite pass must fail the run naming the path instead of exiting 0 with
    // the header's update-hdr block silently left stale.
    // void comptest_src_func_UpdateprotoReadFail(); // gstatic/atfdb.comptest:src_func.UpdateprotoReadFail

    // src_func -createmissing with a targsrc source file that does not exist:
    // the failed scan leaves the function index incomplete, so the stub-creation
    // pass must be skipped entirely. The universe declares userfunc xx.NewFunc
    // with no scanned definition; an ungated pass would append a stub for it to
    // the healthy source, duplicating a definition the scan never saw, so the
    // test pins the source byte-unchanged (cmp) next to exit code 1.
    // void comptest_src_func_CreatemissingScanFail(); // gstatic/atfdb.comptest:src_func.CreatemissingScanFail

    // src_func -updateproto against a header whose update-hdr section is
    // unterminated after an earlier one closed: the rewrite would drop everything
    // from the unterminated marker to EOF and write the truncation back, so the
    // run must fail naming the file, pinned byte-unchanged (cmp) next to exit
    // code 1.
    // void comptest_src_func_UpdateprotoUnterminated(); // gstatic/atfdb.comptest:src_func.UpdateprotoUnterminated

    // A targsrc source that ends inside a function (no closing brace at column
    // 0): the scan is incomplete, so the run must fail, and the header rewrite
    // must be skipped -- the truncated file's functions would otherwise vanish
    // from the header. The test pins the header byte-unchanged (cmp) next to
    // exit code 1.
    // void comptest_src_func_UnfinishedFunc(); // gstatic/atfdb.comptest:src_func.UnfinishedFunc

    // A template parameter list can carry a parenthesized default argument whose
    // expression contains '>' (e.g. 'bool B = (4 > 2)') next to a function-pointer
    // parameter: if the '>' inside the parens ended the template-list strip early,
    // the name extraction would fall into the fn-ptr paren and key the function as
    // the bare namespace. The test scans one such function next to a plain fn-ptr
    // template and pins the printed index rows.
    // void comptest_src_func_TemplateParenDefault(); // gstatic/atfdb.comptest:src_func.TemplateParenDefault

    // The iffy 'bigret' check reports a function that returns cstring by value.
    // It reads the return type as whatever stands left of the parameter list, and
    // finds the parameter list as the first paren on the line. A template
    // parameter list can carry parens of its own -- the function-pointer parameter
    // in 'template<typename T, void (*F)(T)>' -- so the paren search has to run on
    // the line with the template list stripped. Read from the raw line, the type
    // comes out of the template list instead, and the check both fires on
    // functions that are fine and stays silent on functions that are not. The
    // fixture holds the whole table of return-type shapes, controls next to
    // rejects, in source order:
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
    // void comptest_src_func_TemplateIffyRet(); // gstatic/atfdb.comptest:src_func.TemplateIffyRet

    // src_func -updateproto with a targsrc source file that does not exist: the
    // failed scan leaves the function index incomplete, so the header rewrite
    // must be skipped entirely. The seed puts a prototype inside the header's
    // update-hdr block; a rewrite from the incomplete index would strip it, so
    // the test pins the header byte-unchanged (cmp) next to exit code 1.
    // void comptest_src_func_UpdateprotoScanFail(); // gstatic/atfdb.comptest:src_func.UpdateprotoScanFail

    // The dmmeta.userfunc primary key holds the amc function key it is copied
    // from, which is a hundred characters wide. When the key was narrower, amc
    // clipped every longer name on the way out and two functions whose names
    // agreed up to the cut wrote the same row, so one registration disappeared.
    // The accepting universe pins the whole width: a short key, two keys that
    // agree for their first fifty characters and differ past it, and a key at
    // the hundred-character limit -- all four load as distinct rows and each
    // binds to its own scanned function, printed with its own acrkey. The
    // second universe carries a key one character over the limit and must be
    // rejected naming the limit, with the run exiting 1.
    // void comptest_src_func_UserfuncKeyWidth(); // gstatic/atfdb.comptest:src_func.UserfuncKeyWidth

    // The comment lines above a function belong to that function, and the header's
    // update-hdr block mirrors them. The divider line this repo puts between
    // functions belongs to no function, so it ends the comment accumulated above
    // it. A comment line qualifies as a divider by its whole form and not by a
    // substring of it: an ascii table inside a leading comment draws a rule line
    // out of dashes, and reading that rule as a divider drops everything above the
    // table -- the paragraph saying what the function does, and the table's own
    // header row -- from the mirrored header, silently. So a rule line breaks its
    // dashes with something, which every table in the tree does, and the last
    // function of the fixture pins the boundary: a rule line of nothing but dashes
    // still reads as a divider. The fixture holds one function per line shape, in
    // source order, and prints the header the scan produced.
    //
    // A banner of slashes is the same rule read over the other divider this repo
    // draws. It is written both as a bare run of slashes and as a comment holding
    // one, `// ` and then the run, and the second form is what the two banners in
    // the tree are written as; a run of slashes with a word among them is a section
    // title, which is comment text for the reason a rule line broken by a word is.
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
    // a table rule of nothing but dashes       | below the rule
    // void comptest_src_func_Precomment(); // gstatic/atfdb.comptest:src_func.Precomment

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
    // writes the prototypes again, at exit 0. The first
    // section holds the shapes that must not be taken for its close -- a struct's
    // `};`, an `} else {`, and the braces of a block the body itself opened, which
    // stand deeper than the marker -- and an eighth section holds the same case for
    // a namespace, which the body opens and closes inside the replaced text. Reading
    // any of those as the close writes the fresh prototypes there and drops the
    // hand-written lines above it, silently and at exit 0, which is what this test
    // exists to catch. Two further sections
    // hold braces that nest no C++ and must not be counted: one inside a line
    // comment, one inside a one-line block comment, one inside a block comment that
    // spans lines, and braces inside a string literal, a character literal and a
    // literal carrying an escaped quote. The last of them closes a function on the
    // same line as a digit-separated constant, whose quotes are not literal quotes.
    // Three more sections hold the braces of a character literal that carries an
    // encoding prefix -- L, u, U and u8 -- because the prefix is an identifier byte
    // standing where a number's digit would stand, and the last of them holds one
    // inside a block the body opened, where a miscounted brace makes the block's own
    // close look like the section's. A section whose brace count is off by one either
    // closes early, dropping the lines from there to the real close, or never closes
    // at all; the fixture arranges one of each so a run that counts a literal's brace
    // leaves this golden with nothing rewritten.
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
    // void comptest_src_func_SectionClose(); // gstatic/atfdb.comptest:src_func.SectionClose

    // Which namespace's functions an update-hdr section receives, over the shapes a
    // marker line can take.
    // A marker line may open more than one namespace -- `namespace outer { namespace
    // inner {` -- and the section's body then stands inside the innermost of them.
    // The prototypes written there are the ones whose definition names that
    // namespace, and a definition inside outer::inner names inner, so reading the
    // marker's outermost name selects outer's functions and declares them inside
    // inner while inner's own functions stay undeclared. The header the next build
    // reads then holds a prototype for a function that is defined nowhere.
    // The fixture holds a section opening one namespace, one opening two, one opening
    // three, and one whose marker line opens none because its brace stands on the
    // line below; the source defines one function in every namespace those markers
    // name. Each section therefore comes back holding exactly one prototype, and
    // which function it is says which name the section was read as.
    // Five further sections name their namespace in a spelling of its own: the C++17
    // `namespace scope1::scope2 {`, whose innermost component is the name; an attribute
    // standing between the keyword and the name; an attribute carrying a string
    // argument; an attribute whose own name is qualified; and a comment standing between
    // the keyword and the name. Five more carry text after the name that the compiler
    // does not read as code -- a comment spelling a call and a semicolon, a line comment
    // mentioning another namespace, a line comment whose last word is the keyword
    // itself, and a quoted and a raw string literal each spelling a namespace line of
    // their own -- and the name each of those lines gives its namespace stands outside
    // that text.
    // The source defines a function in each of those namespaces too, one in each of
    // the namespaces a reader of the raw bytes names instead -- `deprecated`,
    // `visibility`, `run`, `rest` and `bar` -- and one at global scope. Those six are
    // the controls the spellings need. A name read out of an attribute or a remark
    // selects that namespace's functions, and the section then declares them inside a
    // namespace they are not defined in, so the golden pins each of the five as
    // declared nowhere. A name the reader fails to extract comes back empty, an empty
    // name selects the functions defined at global scope, and the golden pins the
    // global function as declared nowhere for the same reason.
    // void comptest_src_func_SectionNs(); // gstatic/atfdb.comptest:src_func.SectionNs

    // An update-hdr marker line that names no namespace, which the run refuses rather
    // than fill.
    // The prototypes a section receives are the functions whose definition names the
    // section's namespace, so a marker line whose name could not be read has no such
    // set: an empty name selects the functions defined at global scope, and the
    // section comes back declaring those inside whatever the marker opened. An
    // anonymous namespace is the shape that reaches this on legal C++, and the file is
    // left as it stands at exit 1. The second stage is the control, a named section
    // that is rewritten at exit 0.
    // void comptest_src_func_SectionUnnamed(); // gstatic/atfdb.comptest:src_func.SectionUnnamed

    // An update-hdr section whose closing brace stands on its own marker line, in the
    // two shapes that produces.
    // The prototypes a section receives go above the line that closes it, so a
    // section that closes on the line it opened has nowhere to put them and the
    // header cannot be rewritten. `namespace foo { void A(); } // update-hdr` is one
    // such line, and a marker line that closes the namespace enclosing it --
    // `namespace inner { void A(); }} // update-hdr` -- is the other; the second ends
    // one level below where it began.
    // What makes the shape worth a diagnostic of its own is what a run does with a
    // section it never located the opening of: the depth the opening reaches is what
    // tells section body from the lines standing below the section, so the first
    // brace anywhere further down the file is taken for the section's own. The lines
    // that nest under that brace are then dropped as body and the prototypes are
    // written at its close, which for the second shape is a header that comes back
    // with a struct's field deleted and a prototype block inside the struct, at exit
    // 0. Each stage pins the exit code and the header byte-unchanged (cmp).
    // A marker line that leaves a brace open which none of its namespaces opened is
    // refused for a related reason, and the fixture carries the two shapes of it. In
    // `namespace foo { struct S { // update-hdr` the section's depth is the struct's, so
    // its close is located on the struct's `};` and the prototypes are written among the
    // members while the members are dropped; in `namespace one { namespace two { } //
    // update-hdr` the inner namespace the line names is closed again, leaving the depth
    // one level below the namespace whose functions the section receives.
    // Two stages are the controls, and both are rewritten at exit 0: one ordinary
    // section, and one whose marker line opens a brace pair and closes it again --
    // `namespace foo { struct S {}; // update-hdr` -- which is a section one level deep
    // with a body below it, not a section that closed itself and not one holding a brace
    // it does not own. That is the shape reading the marker line's deepest point as its
    // end refuses, so the last stage's header is printed: the prototypes stand inside
    // the section and the lines below it come back.
    // void comptest_src_func_SectionBodyless(); // gstatic/atfdb.comptest:src_func.SectionBodyless

    // The shapes where the depth an update-hdr section reaches and the text of the
    // line that reaches it disagree about where the section ends.
    // A section's lines are dropped as the file is copied, and which lines those are is
    // decided by one model of the file: the depth its braces reach. A brace the scan
    // counts is not always a brace the compiler reads as nesting. `#define OPENBLOCK {`
    // spells a block opener a macro's user will close somewhere else, and the two arms
    // of a preprocessor conditional need not balance, so a section holding either one
    // stands one level deeper than the count says. Its own closing brace then only
    // takes the count back to the depth the marker opened, which is section body, and
    // the copy goes on dropping lines until something further down takes the count
    // lower. The hand-written lines in between are gone, and the header is saved at
    // exit 0.
    // The line's own text is the second description of the same event, and it is the
    // one a miscount cannot move: a section is closed by the brace of the namespace its
    // marker opened, and that brace is the first thing on its line. Where the two
    // descriptions disagree the file is refused rather than rewritten, and the brace
    // count reaching zero at end of file is the whole-file form of the same test.
    // The fixture holds a stage per disagreement. A macro's brace inside a section
    // whose enclosing namespace supplies the closing brace the count settles on is the
    // shape that deletes hand-written lines below the section; a declaration sharing
    // its line with the namespace's brace is the shape that leaves a hand-written
    // declaration inside the region whose banner says it was scanned from sources; a
    // macro's brace undone by a second macro below the section is the shape whose file
    // balances and whose section still ends on the wrong line; and a stray brace
    // standing after a section that was read correctly is refused too, which is what a
    // test on the whole file costs. Two stages are the controls, and both are rewritten
    // at exit 0: a macro's brace undone inside the section it stands in, where the
    // count is wrong in the middle and right again at the close, and a struct declared
    // in a section, whose `};` is a closing brace standing deeper than the marker and
    // is body rather than the close. Each stage pins the exit code and, for the refused
    // ones, the header byte-unchanged (cmp), and each control's file is printed: an exit
    // code and the word `rewritten` say a rewrite happened and nothing about what it
    // produced, so a run that swallowed the line below the section or wrote the
    // prototypes at the wrong depth would satisfy them. Both controls come back with the
    // section's hand-written lines replaced by the prototype banner, the section's own
    // closing brace in place, and the hand-written line below the section standing.
    // void comptest_src_func_SectionUneven(); // gstatic/atfdb.comptest:src_func.SectionUneven

    // The rule that decides which lines open an update-hdr section, over one header
    // carrying every shape it has to judge. A line opens a section only when the
    // compiler reads the namespace and the marker tag as code, so the fixture holds
    // one real marker and four look-alikes: a marker line inside a block comment
    // that spans lines, which is what a hand edit produces when it comments a
    // section out; a namespace line whose tag stands inside a block comment on the
    // line itself; a namespace line that opens a block comment it never closes and
    // carries the tag inside it, which is the same hand edit written on one line; and
    // a namespace line whose tag stands inside a string literal. A fifth shape, a
    // marker behind a `//`, is a look-alike the rule has always rejected. Three more
    // hold the tag inside a raw string: one closed on its own line, one closed on its
    // own line with a delimiter, one spanning lines whose text holds a whole
    // `namespace zz { // update-hdr` line of its own, and one whose namespace line
    // opens a raw string and puts the tag inside it. The third is the shape a
    // line-at-a-time scan reads as a marker, because the line the tag stands on
    // begins inside the literal and by itself looks like code; the fourth is the
    // shape the tag's own test rejects, since a raw string is not the comment a line
    // ends in and leaves the tag below the code the line carries. Reading any of them as a marker opens a section that was never
    // meant to exist and drops every line from there to the next lone brace, so the
    // golden pins the whole header: only the two real sections come back rewritten,
    // and every other line, including the ones the look-alikes enclose and the
    // struct below them, comes back byte for byte.
    // One line carries the tag twice: `namespace requoted { /* was: // update-hdr */
    // // update-hdr` quotes the marker in a remark and then writes the real one.
    // That is the other side of the same rule, and it is the accepting side -- the
    // line is a marker, since the tag stands in the trailing comment -- so the
    // golden pins its section rewritten. A rule reading the first occurrence of the
    // tag anywhere on the line refuses it instead, and refusing a marker leaves that
    // section stale at exit 0 with nothing printed.
    // A `//` comment whose line ends in a backslash is the fourth kind of span a line
    // can begin inside, because C++ splices lines before it recognizes comments: the
    // line below such a comment belongs to it entire, and so does the line below that
    // one when it ends in a backslash too. The fixture holds a look-alike marker one
    // line below such a comment and another two lines below one, and it holds the
    // accepting side as well -- a real marker whose own trailing comment ends in a
    // backslash, whose namespace and tag are code and whose section is rewritten.
    // Which occurrence a line's attributes come from is the same question asked of the
    // same line, so the requoted marker names a source in each of its two tags: the
    // remark names the header itself, which defines no function, and the live tag names
    // src/y.cpp, which defines one. The golden pins that function's prototype in the
    // section under the banner of the source it came from, so a run that read the
    // attributes from the quoted tag comes back with an empty section instead.
    // void comptest_src_func_SectionOpen(); // gstatic/atfdb.comptest:src_func.SectionOpen

    // A header whose first update-hdr section never closes and whose second one
    // does. The run ends with no section open and one section updated, so a write
    // gated on those two alone would save a copy that lost every line from the
    // first marker to the second, including the second marker itself, while the
    // diagnostic printed above it said the file was not rewritten. The test pins
    // the file byte-unchanged (cmp) next to exit code 1.
    // void comptest_src_func_SectionOpenTwice(); // gstatic/atfdb.comptest:src_func.SectionOpenTwice

    // -------------------------------------------------------------------
    // cpp/atf_comp/src_hdr.cpp -- Comptests for src_hdr
    //

    // src_hdr -write twice over a headerless script: the first run adds the
    // header, and the second run must leave the file byte-identical -- a
    // rewrite that keeps changing its own output grows the header on every
    // update-hdr run.
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_src_hdr_Converge(); // gstatic/atfdb.comptest:src_hdr.Converge

    // src_hdr -write against a scriptfile row whose file does not exist (no
    // seed): the run must fail naming the path instead of exiting 0 with the
    // file silently skipped -- a stale scriptfile row would otherwise pass
    // every run unnoticed.
    // void comptest_src_hdr_FileReadFail(); // gstatic/atfdb.comptest:src_hdr.FileReadFail

    // src_hdr -write against a script whose directory denies the write (the
    // DenyWriteCmd scaffold): the header rewrite fails, and the run must fail
    // naming the path instead of exiting 0 with the file silently unchanged.
    // void comptest_src_hdr_FileWriteFail(); // gstatic/atfdb.comptest:src_hdr.FileWriteFail

    // src_hdr -write against a scriptfile whose comment syntax is unknown (a
    // .json file; likewise an extensionless file with no shebang to guess from):
    // every line would parse as header comment and the rewrite would replace the
    // whole file with a bare separator, so the run must fail naming the file,
    // pinned byte-unchanged (cmp) next to exit code 1.
    // void comptest_src_hdr_NoCmtstring(); // gstatic/atfdb.comptest:src_hdr.NoCmtstring

    // src_hdr -write whose header write succeeds but whose trailing
    // src_func -updateproto step fails (the stub bin/src_func exits 1): the run
    // must fail naming the step, since the update-hdr blocks are left stale.
    // void comptest_src_hdr_UpdateprotoFail(); // gstatic/atfdb.comptest:src_hdr.UpdateprotoFail

    // -------------------------------------------------------------------
    // cpp/atf_comp/ssimfilt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_ssimfilt_Csv(); // gstatic/atfdb.comptest:ssimfilt.Csv
    // void comptest_ssimfilt_CsvField(); // gstatic/atfdb.comptest:ssimfilt.CsvField
    // void comptest_ssimfilt_FirstTag(); // gstatic/atfdb.comptest:ssimfilt.FirstTag
    // void comptest_ssimfilt_Json(); // gstatic/atfdb.comptest:ssimfilt.Json
    // void comptest_ssimfilt_JsonRecursive(); // gstatic/atfdb.comptest:ssimfilt.JsonRecursive
    // void comptest_ssimfilt_MatchField(); // gstatic/atfdb.comptest:ssimfilt.MatchField
    // void comptest_ssimfilt_MatchTag(); // gstatic/atfdb.comptest:ssimfilt.MatchTag
    // void comptest_ssimfilt_SelectField(); // gstatic/atfdb.comptest:ssimfilt.SelectField
    // void comptest_ssimfilt_Stable(); // gstatic/atfdb.comptest:ssimfilt.Stable
    // void comptest_ssimfilt_Table(); // gstatic/atfdb.comptest:ssimfilt.Table

    // -------------------------------------------------------------------
    // cpp/atf_comp/sv2ssim.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_sv2ssim_Convert1(); // gstatic/atfdb.comptest:sv2ssim.Convert1
    // void comptest_sv2ssim_Convert1Signed(); // gstatic/atfdb.comptest:sv2ssim.Convert1Signed
    // void comptest_sv2ssim_Convert2(); // gstatic/atfdb.comptest:sv2ssim.Convert2
    // void comptest_sv2ssim_Convert2Tsv(); // gstatic/atfdb.comptest:sv2ssim.Convert2Tsv
    // void comptest_sv2ssim_UniqueFieldName(); // gstatic/atfdb.comptest:sv2ssim.UniqueFieldName

    // -------------------------------------------------------------------
    // cpp/atf_comp/wt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_wt_Anon(); // gstatic/atfdb.comptest:wt.Anon
    // void comptest_wt_Clean(); // gstatic/atfdb.comptest:wt.Clean
    // void comptest_wt_Command(); // gstatic/atfdb.comptest:wt.Command
    // void comptest_wt_Create(); // gstatic/atfdb.comptest:wt.Create
    // void comptest_wt_Del(); // gstatic/atfdb.comptest:wt.Del
    // void comptest_wt_Diff(); // gstatic/atfdb.comptest:wt.Diff
    // void comptest_wt_List(); // gstatic/atfdb.comptest:wt.List
    // void comptest_wt_Reset(); // gstatic/atfdb.comptest:wt.Reset
    // void comptest_wt_StdinMulti(); // gstatic/atfdb.comptest:wt.StdinMulti
}
