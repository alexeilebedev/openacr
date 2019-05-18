// (C) 2017-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: lib_git (lib) -- Helpful git wrappers
// Exceptions: yes
// Source: cpp/lib/lib_git.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo.h"
#include "include/lib_git.h"

// Return current git ref, e.g. ac18e6cb142a86136b58561f2f1d63e3ff4b749e
// Any error output is suppressed.
tempstr lib_git::CurrentRef() {
    return tempstr(Trimmed(SysEval("git rev-parse HEAD 2>/dev/null", FailokQ(false), 1024)));
}

// -----------------------------------------------------------------------------

// Return symbolic name for current branch, e.g. AB-12345
// Any error output is suppressed.
tempstr lib_git::CurrentBranch() {
    return tempstr(Trimmed(SysEval("git symbolic-ref --short -q HEAD 2>/dev/null", FailokQ(false), 1024)));
}

// -----------------------------------------------------------------------------

// Return date + gitref string describing most recent commit, e.g. 2017-05-24.21f8ce8
tempstr lib_git::ShortRef() {
    return tempstr(Trimmed(SysEval("git log --pretty=format:'%ad.%h' --date=short -1",FailokQ(false),1024)));
}

// -----------------------------------------------------------------------------

// Return most recent log entry
// If there was a merge, this will describe 2 or more commits. If there was a straight
// commit, there will be just 1 entry (compare with git log -1)
tempstr lib_git::LastCommitLog() {
    return tempstr(SysEval("git log HEAD~..HEAD",FailokQ(true),1024*1024));
}

// -----------------------------------------------------------------------------

// Check that specified DIR is free of any git modifications.
// Return a string describing modified files, or an empty string if none
tempstr lib_git::GitModifiedFiles(strptr dir) {
    tempstr modfiles(SysEval(tempstr()<<"git ls-files -m "<<dir, FailokQ(false), 1024*1024));
    tempstr ret;
    bool clean = modfiles == strptr();
    if (!clean) {
        ListSep ls(", ");
        int n = 0;
        ind_beg(Line_curs,line,modfiles) {
            ret << ls << line;
            if (n++ >= 20) {
                ret << " ...";
                break;
            }
        }ind_end;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Check that specified DIR is free of any git modifications.
// If failed, throw exception with MSG.
void lib_git::CheckGitCleanX(strptr dir, strptr msg) {
    tempstr mod = lib_git::GitModifiedFiles(dir);
    vrfy(mod=="", tempstr()<<msg<<Keyval("modified_files",mod));
}

// -----------------------------------------------------------------------------

// Return TRUE if REMOTE is a valid git remote
bool lib_git::GitRemoteExistsQ(strptr remote) {
    tempstr git_rem = SysEval("git remote 2>&1", FailokQ(false), 1024*1024);
    ind_beg(algo::Line_curs, str, git_rem) {
        if (str == remote) {
            return true;
        }
    }ind_end;
    return false;
}

// -----------------------------------------------------------------------------

// Return TRUE if BRANCH is a valid git branch
bool lib_git::GitBranchExistsQ(strptr branch) {
    bool found = false;
    tempstr git_br = SysEval("git branch 2>&1", FailokQ(false), 1024*1024);
    StringIter it(git_br);
    while (!it.EofQ()) {
        if (GetWordCharf(it) == branch) {
            found = true;
            break;
        }
    }
    return found;
}
