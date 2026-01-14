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
    // void Main(); // main:acr_in

    // -------------------------------------------------------------------
    // cpp/acr_in/data.cpp
    //

    // Create table acr_in::_db.tuple containing all tuples that should be printed
    // for this run
    void Main_Data();
}
