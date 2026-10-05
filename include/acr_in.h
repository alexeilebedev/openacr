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
// Target: acr_in (exe) -- ACR Input - compute set of ssimfiles or tuples used by a specific target
// Exceptions: yes
// Header: include/acr_in.h
//

#include "include/algo.h"
#include "include/gen/acr_in_gen.h"
#include "include/gen/acr_in_gen.inl.h"

namespace acr_in { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr_in/acr_in.cpp
    //

    // Find or create an nsssimfile entry
    // Nsssimfile = ns/ssimfile, representing a finput relation between ns and ssimfile
    acr_in::FNsssimfile &ind_nsssimfile_GetOrCreate(algo::strptr ns, algo::strptr ssimfile);

    // True if we are doing a reverse lookup of namespaces by ssimfile,
    // as opposed to a forward lookup of ssimfiles by namespace
    bool ReverseLookupQ();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:acr_in

    // -------------------------------------------------------------------
    // cpp/acr_in/data.cpp
    //

    // Create table acr_in::_db.tuple containing all tuples that should be printed
    // for this run
    void Main_Data();
}
