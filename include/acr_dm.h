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

    // Attach ROW under ANCHOR as element SEQ of the run named by RUNKEY.
    // Rows a branch added come out before the base file's own row at that anchor, which is
    // what puts an insertion between the two rows it was written between.  Two branches
    // that both inserted at one anchor are separated by RUNKEY, the key of the first row
    // of each run: the only comparison ever made between two branches is a comparison of
    // keys, and a key cannot tell which branch git passed second.
    void AnchorTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, bool basefile, strptr runkey, int seq);

    // Load all files
    void Main_LoadFiles();

    // Print list of sources (for dump only)
    void PrintSource(acr_dm::Source &source, cstring &out);

    // Dump
    void Main_Dump();

    // whether an item has been removed in some source
    inline bool RemovedQ(acr_dm::Source source);

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

    // Print every row that came after TUPLE, each one followed in turn by the rows that
    // came after it.  The bheap hands the rows back in sortkey order, so the rows a branch
    // added come out ordered by key and the rows of the base file come out in the order
    // the base file had them, the added ones ahead of the base row they were placed before.
    void PrintSubtree(acr_dm::FTuple &tuple, int &pos, cstring &out);

    // Merge
    void Main_Merge();

    // Main routine
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:acr_dm
}
