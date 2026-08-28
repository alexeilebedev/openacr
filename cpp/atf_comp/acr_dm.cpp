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

// The field table sorts by ctype and leaves the order within a ctype to the file,
// so a row's position in the file is the record, and a merge that moves it changes
// the schema.  Ours adds ns.Alpha.a0 at the top of the file, theirs appends
// ns.Beta.z at the bottom, and each addition has to survive at the place it was
// made -- a0 first, z last.
void atf_comp::comptest_acr_dm_FieldOrder() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/fieldorder1.ssim test/acr_dm/fieldorder2.ssim test/acr_dm/fieldorder3.ssim");
}

// A file that one branch adds whole shares no row with the base, so every row it
// holds belongs to a single run.  Placing the run as a unit is what keeps the walk
// over the result one level deep here; placing each row after the row above it would
// make the file a chain and recurse once per row, which stops being survivable at a
// few tens of thousands.  100k rows is past that point and well under a real limit.
void atf_comp::comptest_acr_dm_DeepRun() {
    atf_comp::ProcStart("bash -c '"
                        "seq -f \"a.b  key:%09g\" 0 99999 > $tempdir/new.ssim"
                        "; : > $tempdir/empty.ssim"
                        // awk rather than wc, which pads its count to a column
                        // width on a BSD userland and not on a GNU one
                        "; $bindir/acr_dm $tempdir/empty.ssim $tempdir/new.ssim $tempdir/new.ssim | awk \"END{print NR}\"'");
}

// A rebase hands acr_dm the same three files a merge does, with ours and theirs
// swapped, so the two must produce the same file.  Both branches add a run of two
// fields after ns.C.a, which is the case that has an ordering decision to make: the
// runs come out whole, ordered against each other by key, the same way round either
// time.
void atf_comp::comptest_acr_dm_Symmetry() {
    atf_comp::ProcStart("bash -c '"
                        "$bindir/acr_dm test/acr_dm/symmetry1.ssim test/acr_dm/symmetry2.ssim test/acr_dm/symmetry3.ssim > $tempdir/merge.ssim"
                        "; $bindir/acr_dm test/acr_dm/symmetry1.ssim test/acr_dm/symmetry3.ssim test/acr_dm/symmetry2.ssim > $tempdir/rebase.ssim"
                        "; cat $tempdir/merge.ssim"
                        "; cmp -s $tempdir/merge.ssim $tempdir/rebase.ssim && echo rebase_same:Y || echo rebase_same:N'");
}

// Upstream moved ns.Alpha.c to the front of its ctype and the branch appended
// ns.Beta.y, which are edits to two different parts of the file and have to survive
// together.  A row's position inside its ctype is the member order of a generated
// struct, so a merge that hands back the base file's order gives the branch a struct
// upstream never asked for and no gate objects: the schema is consistent with itself,
// and only upstream's own copy says otherwise.  So c comes first here, and the rebase
// -- the same three files with ours and theirs swapped -- produces the same file.
void atf_comp::comptest_acr_dm_Reorder() {
    atf_comp::ProcStart("bash -c '"
                        "$bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder2.ssim test/acr_dm/reorder3.ssim > $tempdir/merge.ssim"
                        "; $bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder3.ssim test/acr_dm/reorder2.ssim > $tempdir/rebase.ssim"
                        "; cat $tempdir/merge.ssim"
                        "; cmp -s $tempdir/merge.ssim $tempdir/rebase.ssim && echo rebase_same:Y || echo rebase_same:N'");
}

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

// A line that names no row belongs to the row below it, which is what apm writes above
// each dev.gitfile row of its manifest: a checksum of the file that row names.  Held
// that way, the line merges as the row's own text -- ours changes the checksum above
// cpp/two.cpp, theirs adds cpp/new.cpp with a checksum of its own, and both survive
// with each comment still above its row.  The last line belongs to no row at all, and
// a file's tail comes out after every row there is.
void atf_comp::comptest_acr_dm_Comment() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/comment1.ssim test/acr_dm/comment2.ssim test/acr_dm/comment3.ssim");
}

// The third file is not there, and a file that is not there would otherwise read as a
// file with no rows in it: the merge would report that theirs deleted every row, write
// an empty result, and exit zero.  What comes out instead is both of the files that do
// exist, whole, between markers, which is the shape of every failure the driver cannot
// merge past.
void atf_comp::comptest_acr_dm_MergeFail() {
    atf_comp::ProcStart("$bindir/acr_dm test/acr_dm/reorder1.ssim test/acr_dm/reorder2.ssim test/acr_dm/nosuchfile.ssim");
}
