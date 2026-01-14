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
// Target: gcache (exe) -- Compiler cache
// Exceptions: yes
// Header: include/gcache.h
//

#include "include/gen/gcache_gen.h"
#include "include/gen/gcache_gen.inl.h"
#include "include/sha.h"

namespace gcache { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/gcache/gcache.cpp
    //

    // get fill path of cache dir if exists
    void ManageCacheDir();

    // recursively remove older files
    void RemoveOldFilesRecurse(strptr dir, algo::UnTime del_thresh, algo::UnTime access_thresh, bool subdir = false);

    // Remove log entries older than THRESH
    void CleanLog(algo::UnTime thresh);

    // cache administration
    void Clean();

    // Process command line
    void ProcessCommandLine();

    // Transform compilation command
    // Recognize phase by target suffix:
    // .ii -- preprocess -- replace -c by -E
    // .gch -- precompile -- replace -x lang by -x lang-header
    // When supplied, replace source, target, add extra flags.
    tempstr MakeCmd(strptr source = "", strptr target = "", strptr flags = "");

    // run specified cmd under bash, return exit code
    int RunCmd(strptr cmd);

    // Compose cached file name from sha1 hash
    tempstr CachedFile(strptr sha1);

    // Copy contents of file descriptor FROM
    // to file TO_FNAME
    // The copying is done into a temporary file, which is then renamed to TO_FNAME
    // If at any stage the operation fails, the temporary file is deleted
    bool FdToFile(algo::Fildes from, algo::cstring &to_fname);

    // Append report to log file
    void Log();
    void Report();

    // Precompile header
    // Parse preprocessed file
    // Get source line number information
    // as per https://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
    // `# linenum filename flags`
    // - flag 1 - start of included file;
    // - flag 2 - returning to a file after having inclided another file.
    // Build index array with the following information:
    // - pointer to parent
    // - name - included file name
    // - begin - character position of file begin (including flag 1 marker);
    // - inner_end - character position of file end (excluding flag 2 marker);
    // - outer_end - character position of file end (including flag 2 marker);
    // - mlines_before - number of meaningful source lines before start of the file.
    //
    // The following function prototype is used as directive that this
    // file is eligible to be precompiled:
    // void __gcache_pragma_pch_preprocess();
    //
    // Important condition is that there is no any meaningful source line before this file.
    // Note that only one precompiled header is possible per compilation unit.
    // However, it is possible to put the directive in multiple files.
    // As result, the latest file  having no any meaningful source line before is taken.
    //
    void Pch();

    // main routine
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:gcache
}
