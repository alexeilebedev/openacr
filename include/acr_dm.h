// Copyright (C) 2023 AlgoRND
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

#include "include/gen/acr_dm_gen.h"
#include "include/gen/acr_dm_gen.inl.h"

namespace acr_dm { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr/acr_dm.cpp
    //

    // Get or create tuple attribute
    acr_dm::FAttr &zs_attr_GetOrCreate(FTuple &tuple, strptr name);

    // Get or create attribute value
    acr_dm::FValue &zs_value_GetOrCreate(FAttr &attr, strptr val);

    // Load all files
    void Main_LoadFiles();

    // Print list of sources (for dump only)
    void PrintSource(acr_dm::Source &source, cstring &out);

    // Dump
    void Main_Dump();

    // whether an item has been removed in some source
    inline bool RemovedQ(acr_dm::Source source);

    // print conflict marker
    void PrintConflictMarker(char mark, strptr source_name, cstring &out);

    // Print tuple for given source
    void PrintSourceTuple(FTuple &tuple, int source, cstring &out);

    // Print merged tuple, return false in case of conflict
    bool MergeTuple(FTuple &tuple, cstring &out);

    // Merge
    void Main_Merge();

    // Main routine
    void Main();
}
