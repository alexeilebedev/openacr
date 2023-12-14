// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: lib_git (lib) -- Helpful git wrappers
// Exceptions: yes
// Header: include/lib_git.h
//

#include "include/algo.h"
#include "include/gen/lib_git_gen.h"
#include "include/gen/lib_git_gen.inl.h"

namespace lib_git { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_git.cpp
    //

    // Return current git ref, e.g. ac18e6cb142a86136b58561f2f1d63e3ff4b749e
    // Any error output is suppressed.
    tempstr CurrentRef();

    // Return symbolic name for current branch, e.g. AB-12345
    // Any error output is suppressed.
    tempstr CurrentBranch();

    // Return date + gitref string describing most recent commit, e.g. 2017-05-24.21f8ce8
    tempstr ShortRef();

    // Return most recent log entry
    // If there was a merge, this will describe 2 or more commits. If there was a straight
    // commit, there will be just 1 entry (compare with git log -1)
    tempstr LastCommitLog();

    // Check that specified DIR is free of any git modifications.
    // Return a string describing modified files, or an empty string if none
    tempstr GitModifiedFiles(strptr dir);

    // Check that specified DIR is free of any git modifications.
    // If failed, throw exception with MSG.
    void CheckGitCleanX(strptr dir, strptr msg);

    // Return TRUE if REMOTE is a valid git remote
    bool GitRemoteExistsQ(strptr remote);

    // Return TRUE if BRANCH is a valid git branch
    bool GitBranchExistsQ(strptr branch);

    // Get configured user: name <email>
    tempstr GetUser();

    // Get year of latest commit of given file
    u32 GetLastCommitYear(strptr file);
}
