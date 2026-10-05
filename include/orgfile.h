// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
    // A hash command that fails, or prints nothing, stops the run.  Its files would
    // otherwise all share the empty hash, and -dedup would delete every one of them
    // but the first, while -move would overwrite a target as a proven duplicate.
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
    // void Main(); // dmmeta.main:orgfile
}
