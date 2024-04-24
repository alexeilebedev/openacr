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
// Target: sv2ssim (exe) -- sv2ssim - Separated Value file processor
// Exceptions: yes
// Header: include/sv2ssim.h
//

#include "include/algo.h"
#include "include/gen/sv2ssim_gen.h"
#include "include/gen/sv2ssim_gen.inl.h"

namespace sv2ssim { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/sv2ssim.cpp
    //

    // Add a new field with name NAME
    // The name of the field is stripped of all non-identifier characters
    // Field is indexed and added to the selected set (zd_selfield) if it matches _db.cmdline.field regx
    void AllocField(strptr name);

    // Convert line into token array _db.linetok
    // If the file has a headerline, the remove any fields beyond those defined in the header
    // If the file has no headerline, create fields on the fly
    void TokenizeLine(strptr line);

    // Create FIELD records out of current tokens
    void CreateFields();

    // Update FIELD records based on current tokens
    void UpdateFields();

    // Pick types for each field and print ctype definition
    void OutputSchema();
    void ValidateCmdline();

    // Print current tuple according to output format
    // If -outseparator is set, use separator
    // With , -> use csv output files
    // With other separators, strip separator character from token before printing
    tempstr CurlineToString();
    void Main();
}
