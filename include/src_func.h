// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Header: include/src_func.h
//

#include "include/algo.h"
#include "include/gen/src_func_gen.h"
#include "include/gen/src_func_gen.inl.h"

namespace src_func { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/src_func/edit.cpp -- Implementation of -e
    //
    void Main_EditFunc();
    void Main_CreateMissing();

    // -------------------------------------------------------------------
    // cpp/src_func/fileloc.cpp -- Location in file, for each function
    //

    // Remember current file location
    void SaveFileloc(src_func::FTargsrc &targsrc, int lineno);

    // Get current file location in the form 'filename:lineno: '
    tempstr GetFileloc();

    // Get function file location in the form 'filename:lineno: '
    // Second argument specifies line offset within the function (starting with 0)
    tempstr Location(src_func::FFunc &func, int lineoffset);

    // -------------------------------------------------------------------
    // cpp/src_func/header.cpp -- Generate prototypes in header
    //

    // Go over all sources (includes & sources!)
    // Look for 'update-hdr' sections and insert function prototypes there.
    void Main_UpdateHeader();

    // -------------------------------------------------------------------
    // cpp/src_func/iffy.cpp -- Check iffy function prototypes
    //

    // Check if the function is iffy and set FUNC.IFFY flag if so.
    // Checks can be disabled with ignore: annotations in function pre-comment
    void ComputeIffy(src_func::FFunc &func);

    // -------------------------------------------------------------------
    // cpp/src_func/list.cpp -- Print list of functions
    //

    // Collect list of global function prototypes
    // in target TARGET with namespace NS
    void PrintGlobalProtos(src_func::FTarget &target, strptr ns, strptr srcfilter, cstring &out);
    void Main_ListFunc();

    // -------------------------------------------------------------------
    // cpp/src_func/main.cpp -- Main file
    //

    // Remove single-line C++ comment from file
    // and return result
    strptr StripComment(strptr line);

    // Get first line of function definition
    // Remove open curly
    // Replace DFLTVAL(x) with =x (for headers)
    // Replace FUNCATTR(x) with x (for headers)
    tempstr GetProto(src_func::FFunc &func);

    // Check if line contains function start
    // Criteria are:
    // - first character nonblank
    // - has parentheses
    // - last nonblank character is {
    // - it's not namespace, enum or struct
    bool FuncstartQ(strptr line, strptr trimmedline);

    // Extract function namespace name
    // void *ns::blah(arg1, arg2) -> ns
    tempstr GetFuncNs(src_func::FFunc &func);

    // Filter functions based on parameters provided on command line.
    bool MatchFuncQ(src_func::FFunc &func);

    // Get srcfile filter from update-hdr line.
    // If none specified, use %
    tempstr Nsline_GetSrcfile(strptr line);

    // extract namespace name from a line like 'namespace xyz {'
    strptr Nsline_GetNamespace(strptr str);
    void RewriteOpts();

    // Calculate a set of prefixes & suffixes (together:affixes)
    // which "look like generated code"
    // We will not generate prototypes for functions that look like generated code
    // (this is a heuristic, not a hard rule, but it saves hours of debugging when it works)
    // The reason is that if something changes in the underlying table where the userfunc
    // no longer gets generated (and thus expected) by amc, the user function should trigger
    // a compile error from lack of prototype.
    // Also, for each prototype that we refused to generate because a function matched
    // a known gen affix, we add a comment to the include file.
    void CalcGenaffix();
    src_func::FGenaffix *FindAffix(strptr cppname);

    // Main
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:src_func

    // -------------------------------------------------------------------
    // cpp/src_func/nextfile.cpp -- Find next file in target
    //

    // Find and print next/previous file
    void Main_Nextfile();

    // -------------------------------------------------------------------
    // cpp/src_func/scan.cpp -- Scan sources to collect functions
    //

    // Read functions from all sources
    void Main_ScanFiles();
}
