// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:ssimfilt
}
