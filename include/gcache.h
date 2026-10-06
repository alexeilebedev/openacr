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

    // Set the cache directory up as the command line asks, and resolve _db.dir to
    // the directory the .gcache link names, or to nothing when there is no cache.
    // -install creates the directory named by -dir, writes its marker, makes it
    // group writable with the group inherited below, and implies -enable; -enable
    // links .gcache to the directory; -disable removes the link.
    // The user asked for each of these, and a failure in any of them leaves the
    // cache disabled, or unwritable for the group that shares it, and shows up
    // later only as build wall clock nobody attributes to gcache.  So every step
    // has its status read, a failure is reported once as a gcache.error and reaches
    // the exit code, and `done` is printed only once every step of the install
    // has succeeded.  A request that cannot be honored leaves the link as it was:
    // -enable replaces the link only once the directory it names exists, so a
    // mistyped -dir does not disable a working cache on its way to the error.
    // gcache.InstallFail pins the marker, link and removal failures.
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
    // Rewrite _db.preproc_file so that the compile which follows reads the
    // precompiled header instead of the text it was built from.  A header that
    // does not build leaves the file as it was: it is an optimization, and the
    // compile proceeds without it.
    void Pch();

    // main routine
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:gcache
}
