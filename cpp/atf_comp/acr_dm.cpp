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
// Source: cpp/atf_comp/acr_dm.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_dm_Conflict() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file4.ssim");
}

void atf_comp::comptest_acr_dm_Merge() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim");
}

void atf_comp::comptest_acr_dm_RenameTuple() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/renametuple1.ssim test/acr_dm/renametuple2.ssim test/acr_dm/renametuple3.ssim");
}

// Merge a field table in which ours adds ns.Alpha.a0 at the top and theirs appends
// ns.Beta.z at the bottom, and check that each addition stays where its branch made
// it: a0 first, z last.
// The field table sorts by ctype and leaves the order within a ctype to the file.
// A row's position in the file is therefore part of the record, so a merge that
// moves the row changes the schema.
void atf_comp::comptest_acr_dm_FieldOrder() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/fieldorder1.ssim test/acr_dm/fieldorder2.ssim test/acr_dm/fieldorder3.ssim");
}

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
void atf_comp::comptest_acr_dm_DeepRun() {
    atf_comp::ProcStart("bash -c '"
                        "seq -f \"a.b  key:%09g\" 0 99999 > $tempdir/new.ssim"
                        "; : > $tempdir/empty.ssim"
                        // awk rather than wc, which pads its count to a column
                        // width on a BSD userland and not on a GNU one
                        "; $bindir/acr_dm $tempdir/empty.ssim $tempdir/new.ssim $tempdir/new.ssim | awk \"END{print NR}\"'");
}

// Merge two branches that each add a run of two fields after ns.C.a, and check that
// the rebase produces the same file:
// 1. Merge symmetry1, symmetry2 and symmetry3 into merge.ssim.
// 2. Run the rebase, the same three files with ours and theirs swapped, into
//    rebase.ssim.
// 3. Print merge.ssim, and check that rebase.ssim matches it (rebase_same:Y).
// A rebase hands acr_dm the same three files a merge does, with ours and theirs
// swapped.  Two runs added at one place leave an ordering decision to make.  acr_dm
// keeps each run whole and orders the runs against each other by key, the same way
// round either time.
void atf_comp::comptest_acr_dm_Symmetry() {
    atf_comp::ProcStart("bash -c '"
                        "$bindir/acr_dm test/acr_dm/symmetry1.ssim test/acr_dm/symmetry2.ssim test/acr_dm/symmetry3.ssim > $tempdir/merge.ssim"
                        "; $bindir/acr_dm test/acr_dm/symmetry1.ssim test/acr_dm/symmetry3.ssim test/acr_dm/symmetry2.ssim > $tempdir/rebase.ssim"
                        "; cat $tempdir/merge.ssim"
                        "; cmp -s $tempdir/merge.ssim $tempdir/rebase.ssim && echo rebase_same:Y || echo rebase_same:N'");
}

// Merge a move that upstream made with an append that the branch made, and check
// that both survive and that the rebase produces the same file:
// 1. Merge reorder1, reorder2 and reorder3 into merge.ssim.  Upstream moves
//    ns.Alpha.c to the front of its ctype, and the branch appends ns.Beta.y.
// 2. Run the rebase, the same three files with ours and theirs swapped, into
//    rebase.ssim.
// 3. Print merge.ssim, where c comes first, and check that rebase.ssim matches it
//    (rebase_same:Y).
// A row's position inside its ctype is the member order of a generated struct.  A
// merge that hands back the base file's order gives the branch a struct that upstream
// never asked for.  Every gate passes such a merge, because the schema is consistent
// with itself.  Only upstream's own copy says otherwise.
void atf_comp::comptest_acr_dm_Reorder() {
    atf_comp::ProcStart("bash -c '"
                        "$bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder2.ssim test/acr_dm/reorder3.ssim > $tempdir/merge.ssim"
                        "; $bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder3.ssim test/acr_dm/reorder2.ssim > $tempdir/rebase.ssim"
                        "; cat $tempdir/merge.ssim"
                        "; cmp -s $tempdir/merge.ssim $tempdir/rebase.ssim && echo rebase_same:Y || echo rebase_same:N'");
}

// Move one row to two different places on two branches, and check that the merge
// reports a conflict and that the rebase leaves the row in the same place:
// 1. Merge movecfl1, movecfl2 and movecfl3 into merge.ssim, and print its exit code.
//    One file moves ns.C.b after ns.C.c, and the other moves it after ns.C.d.
// 2. Run the rebase, the same files with ours and theirs swapped, into rebase.ssim.
// 3. Print merge.ssim.
// 4. Strip the marker lines from both results, and check that the rest matches
//    (rebase_same:Y).
// Two files that change one thing to two different values conflict, the same as for
// an attribute.  The two sides of the marker block are the same row twice, because
// the text of a row carries no trace of where it sits.  The line above the block
// therefore names the two positions that disagreed.  A conflict still leaves the row
// somewhere, and the rebase has to leave it in the same place.  acr_dm orders the two
// positions by key, so the order is the same either way round.  The marker labels
// name the file in each slot and swap with it, so the comparison skips them.
void atf_comp::comptest_acr_dm_MoveConflict() {
    atf_comp::ProcStart("bash -c '"
                        "$bindir/acr_dm test/acr_dm/movecfl1.ssim test/acr_dm/movecfl2.ssim test/acr_dm/movecfl3.ssim > $tempdir/merge.ssim"
                        "; echo merge_code:$?"
                        "; $bindir/acr_dm test/acr_dm/movecfl1.ssim test/acr_dm/movecfl3.ssim test/acr_dm/movecfl2.ssim > $tempdir/rebase.ssim"
                        "; cat $tempdir/merge.ssim"
                        "; grep -v \"^[<>=]\" $tempdir/merge.ssim > $tempdir/merge.rows"
                        "; grep -v \"^[<>=]\" $tempdir/rebase.ssim > $tempdir/rebase.rows"
                        "; cmp -s $tempdir/merge.rows $tempdir/rebase.rows && echo rebase_same:Y || echo rebase_same:N'");
}

// Merge a manifest whose rows carry comment lines, and check that each comment
// merges as its row's own text.  Ours changes the checksum above cpp/two.cpp.
// Theirs adds cpp/new.cpp with a checksum of its own.  Check that both survive with
// each comment still above its row.  Check also that the last line, which belongs to
// no row, comes out after every row.
// A line that names no row belongs to the row below it.  apm writes such a line
// above each dev.gitfile row of its manifest: a checksum of the file that row names.
void atf_comp::comptest_acr_dm_Comment() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/comment1.ssim test/acr_dm/comment2.ssim test/acr_dm/comment3.ssim");
}

// Merge with test/acr_dm/nosuchfile.ssim as the third file, and check that acr_dm
// prints the two files that exist, whole, between markers.
// A missing file would otherwise read as a file with no rows in it.  The merge would
// then report that theirs deleted every row, write an empty result and exit zero.
// Two whole files between markers is the shape of every failure that stops the
// driver.
void atf_comp::comptest_acr_dm_MergeFail() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder2.ssim test/acr_dm/nosuchfile.ssim");
}
