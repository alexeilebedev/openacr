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
                        "; $bindir/acr_dm $tempdir/empty.ssim $tempdir/new.ssim $tempdir/new.ssim | wc -l'");
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
