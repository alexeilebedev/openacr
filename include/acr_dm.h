// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: acr_dm (exe) -- ACR Diff/Merge
// Exceptions: yes
// Header: include/acr_dm.h
//

#include "include/algo.h"
#include "include/gen/acr_dm_gen.h"
#include "include/gen/acr_dm_gen.inl.h"

namespace acr_dm { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr_dm/acr_dm.cpp
    //

    // Get or create tuple attribute
    acr_dm::FAttr &zs_attr_GetOrCreate(FTuple &tuple, strptr name);

    // Get or create attribute value
    acr_dm::FValue &zs_value_GetOrCreate(FAttr &attr, strptr val);

    // Record that file SOURCE showed ROW with TEXT above it.
    // A line that names no row belongs to the row below it, which is what apm means by the
    // `# SHA1 = <base64>` line it writes above each dev.gitfile row.  Held as the row's one
    // nameless attribute, the text merges as every other attribute does, so a checksum that
    // changed on one side is a change and one that changed on both is a conflict.
    void AttachCmt(acr_dm::FTuple &row, strptr text, int source);

    // Attach ROW under ANCHOR as element SEQ of the run named by RUNKEY.
    // Rows a branch added come out before the base file's own row at that anchor, which is
    // what puts an insertion between the two rows it was written between.  Two branches
    // that both inserted at one anchor are separated by RUNKEY, the key of the first row
    // of each run: the only comparison ever made between two branches is a comparison of
    // keys, and a key cannot tell which branch git passed second.
    // The row does not enter its anchor's heap here.  A later file may move it, and a row
    // that is placed once and never removed is a row no heap has to give back.
    void AnchorTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, bool basefile, strptr runkey, int seq);

    // Whether ROW is CAND, or one of the rows CAND hangs off on the way to the root.
    bool AncestorQ(acr_dm::FTuple &row, acr_dm::FTuple &cand);

    // Place ROW where file SOURCE puts it: after the run hanging off ANCHOR, as element SEQ
    // of the run named by RUNKEY.  A row the base file also has, and which this file placed
    // somewhere else, is a row this file moved, and a move is merged the way an attribute is
    // -- the one file that moved it decides where it goes, and two files that moved it apart
    // disagree, which is a conflict.
    // A conflict still has to leave the row somewhere, and which of the two contested
    // positions the file ends up holding cannot depend on which argument slot git put a
    // branch in: a rebase hands over the same files a merge does with ours and theirs
    // swapped.  So the two are ordered by the key of the row each would follow, a key being
    // the one thing that cannot tell one branch from the other, and the conflict names them
    // both either way.
    // Two moves can also ask for a cycle: one file moves a after b while the other moves b
    // after a.  A cycle is refused rather than merged, because the printed file is a walk
    // from the root and a row on a cycle is a row the walk never reaches -- it would come
    // out of the merge missing, which is the one outcome worse than a conflict.
    void MoveTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, strptr runkey, int seq, int source);

    // Load all files
    void Main_LoadFiles();

    // Put every row into the heap of the row it follows, which is what fixes the printed order.
    // Loading decides where a row goes and may revise it, so the heaps are filled once loading
    // is over and nothing is ever taken back out of one.
    void Main_LinkTuple();

    // Print list of sources (for dump only)
    void PrintSource(acr_dm::Source &source, cstring &out);

    // Dump
    void Main_Dump();

    // whether an item has been removed in some source
    inline bool RemovedQ(acr_dm::Source source);

    // Decide the one value ATTR keeps, and answer whether the files can agree on it at all.
    // A single value stands unless some file removed it.  Two values, one of them the base
    // file's, are one file's change to it, and the change stands.  Two values with the base
    // file's in neither, or more than two, are files disagreeing, which is a conflict.
    // PRINT says whether the value belongs in the merged row, VALUE is that value, and the
    // return is false on conflict.
    bool MergeAttr(acr_dm::FAttr &attr, bool &print, strptr &value);

    // Print whatever the caller asked to see beside ROW, which came out at position POS.
    // `acr.rowid` is the position itself, because that is what a reader of the merged file
    // needs: acr orders the rows of one sortkey group by rowid, and a number that only
    // rises keeps every group in the order the merge put it in.  `acr_dm.follows` is the
    // anchor, which is the merge's own account of where it decided the row goes.
    void PrintPos(acr_dm::FTuple &row, int pos, cstring &out);

    // print conflict marker
    void PrintConflictMarker(char mark, strptr source_name, cstring &out);

    // Print tuple for given source
    void PrintSourceTuple(FTuple &tuple, int source, int pos, cstring &out);

    // Print merged tuple, return false in case of conflict
    bool MergeTuple(FTuple &tuple, int pos, cstring &out);

    // Say in the file why the rows between the markers below it disagree, when what they
    // disagree about is where ROW goes.
    // Two files that place one row differently produce a conflict whose two sides are the
    // same row, printed twice, identical to the byte -- the text of a row says nothing about
    // where in the file it sits.  A reader who cannot see the disagreement cannot resolve
    // it, and a marker block that looks like a copy of itself is the kind of artifact that
    // gets resolved by picking a side at random.  So the two positions are named above it.
    void PrintMoveConflict(acr_dm::FTuple &row, cstring &out);

    // Print every row that came after TUPLE, each one followed in turn by the rows that
    // came after it.  The bheap hands the rows back in sortkey order, so the rows a branch
    // added or moved come out ordered by key and the rows of the base file come out in the
    // order the base file had them, the added ones ahead of the base row they were placed
    // before.
    void PrintSubtree(acr_dm::FTuple &tuple, int &pos, cstring &out);

    // Write OUT where the caller asked for the result: over the ours file when git is driving
    // the merge, and to stdout otherwise.
    void PrintResult(algo::strptr out);

    // Merge
    void Main_Merge();

    // Print the contents of FNAME, and nothing at all when there is no such file.
    void PrintFileMaybe(algo::strptr fname, cstring &out);

    // Leave a file that says the merge failed, and say why on stderr.
    // A driver that exits without writing leaves git holding the ours file exactly as it was
    // handed over, which during a rebase is upstream's own content: a complete, well-formed
    // ssimfile carrying no marker, holding none of the rows the branch added, and offering a
    // reader nothing to suggest that anything was dropped.  Staging that file by inspection
    // loses the branch's work, and a resolver that reads "no markers" as "already merged"
    // loses it without anyone looking.  So a failure writes both sides whole, between
    // markers, which nothing can mistake for a merged result.
    void Main_MergeFail(algo::strptr message);

    // Main routine
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:acr_dm
}
