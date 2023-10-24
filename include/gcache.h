// Copyright (C) 2023 AlgoRND
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
    void RemoveOldFilesRecurse(strptr dir, algo::UnTime del_thresh, algo::UnTime access_thresh);

    // Remove log entries older than THRESH
    void CleanLog(algo::UnTime thresh);

    // cahe dir: remove files older than one week
    void RemoveOldFiles();

    // Process command line
    void ProcessCommandLine();

    // Transform compilation command
    // If COMPILING:
    // - replace -c by "-fpreprocessed -c _db.preproc_file"
    // OTHERWISE (preprocessing)
    // - replace -o target by temporary file
    // - replace -c by -E
    // If the gcache is disabled (_db.dir is empty)
    // or the preprocessed file doesn't exist (_db.preproc_file is empty -- happens
    // when the command line has -E or -S options), then the command is returned as-is
    tempstr MakeCmd(bool compiling);

    // run specified cmd under bash, return exit code
    int RunCmd(strptr cmd);

    // Update SHA1 from file
    void UpdateFromFile(Sha1Ctx &sha1ctx, strptr filename);

    // if command is eligible to be cached:
    // - compose preprocessor command line
    // - preprocess source
    // - compute SHA1 hash from command line, and preprocessed content
    // - compose path of cached file
    void ComputeHash();

    // Copy from one fd to another
    void CopyX(algo::Fildes from, algo::Fildes to);

    // if cached file exists, copy it to target;
    // othewise run command, atomically copy result to cache.
    void Cache();

    // Append report to log file
    void Log();
    void Report();

    // main routine
    void Main();
}
