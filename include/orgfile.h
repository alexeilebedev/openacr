// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Target: orgfile (exe) -- Organize and deduplicate files by timestamp and by contents
// Exceptions: yes
// Header: include/orgfile.h
//

#include "include/algo.h"
#include "include/gen/orgfile_gen.h"
#include "include/gen/orgfile_gen.inl.h"

namespace orgfile { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/orgfile.cpp
    //

    // Access filename entry for file FNAME.
    // Also compute file's hash.
    // filename->p_filehash fetches the file hash entry.
    // A single filehash may have multiple filenames associated with it.
    orgfile::FFilename *AccessFilename(strptr fname);

    // Determine new filename for FNAME.
    tempstr GetTgtFname(strptr pathname);

    // Read filenames from STDIN.
    // For each file, compute its file hash.
    // Delete file file if it's a duplicate (and -commit was specified)
    void DedupFile(strptr pathname);

    // Move file SRC to TGTFNAME.
    // If destination file exists, it is pointed to by TGT.
    // If the move succeeds, source entry is deleted to reflect this.
    void MoveFile(orgfile::FFilename *src, orgfile::FFilename *tgt, strptr tgtfname);

    // Read filenames files from STDIN (one per line).
    // For each file, determine its new destination by calling GetTgtFname.
    // Create new directory structure as appropriate.
    // Move the file into place if there was no conflict, or if the file content
    // hash exactly matches
    void MoveFile(strptr pathname);
    bool RawMove(strptr line);
    bool RawDedup(strptr line);
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:orgfile
}
