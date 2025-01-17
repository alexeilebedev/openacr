// Copyright (C) 2023-2024 AlgoRND
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
// Target: ssimfilt (exe) -- Tuple utility
// Exceptions: yes
// Header: include/ssimfilt.h
//

#include "include/algo.h"
#include "include/gen/ssimfilt_gen.h"
#include "include/gen/ssimfilt_gen.inl.h"

namespace ssimfilt { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/ssimfilt.cpp
    //
    void PrintCmd(algo::Tuple &tuple);
    bool MatchOutputAttr(algo::Attr &attr);

    // Select attrs to display and print as ssim
    void PrintSsim(algo::Tuple &tuple);
    void PrintStable(algo::strptr line, algo::Tuple &tuple);

    // Print selected fields one by one
    void PrintField(algo::Tuple &tuple);

    // Check if input tuple matches filters
    bool MatchInputTuple(algo::Tuple &tuple);
    void PrintJson(algo::Tuple &tuple, cstring &out, bool toplevel);

    // Print tuple as a JSON object
    // if schema is available (ctype found), determine if the field is a bool, print numeric types
    // without quotes.
    // if no schema is available, all field values are quoted
    void PrintJson(algo::Tuple &tuple);
    void PrintCsv(algo::Tuple &tuple);
    void Table_Save(algo::Tuple &tuple);
    void MDTable_Save(algo::Tuple &tuple);
    void Table_Flush();
    void Main();
}
