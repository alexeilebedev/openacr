// (C) 2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr/ed/sandbox.cpp
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

void acr_ed::BeginSandbox() {
    acr_ed::_db.orig_dir = algo::GetCurDir();
    acr_ed::_db.sandbox_dir = algo::DirFileJoin(algo::GetCurDir(),"temp/acr_ed");
    CreateDirRecurse(acr_ed::_db.sandbox_dir);
    prerr("acr_ed.begin_sandbox"
          <<Keyval("orig_dir",_db.orig_dir)
          <<Keyval("sandbox",_db.sandbox_dir)
          );
    // files/directories to clone to the subdirectory
    tempstr gitfiles(".gitignore");
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        gitfiles<<" "<<sandbox.sandbox;
    }ind_end;
    SysCmd(tempstr()<<"rsync --delete -ac "<<gitfiles<<" "<<_db.sandbox_dir<<"/", FailokQ(false));
    EnterSandboxX();
    // prevent git from escaping the sandbox
    SysCmd("git init .", FailokQ(false));
    // make sure git ls-files knows about these files;
    // acr_ed schedules update-gitfile, and there may be gsymbols
    // generated off of gitfile.ssim, so the list of versioned files
    // must be reasonably valid.
    SysCmd(tempstr()<<"git add -f --ignore-removal "<<gitfiles, FailokQ(false));
    SysCmd("git clean -df .", FailokQ(false));
    SysCmd("git commit --allow-empty -m initial-commit | grep -v 'create mode'", FailokQ(false));
    (void)mkdir("temp", 0755);// may fail
}

// -----------------------------------------------------------------------------

// Change to sandbox directory
void acr_ed::EnterSandboxX() {
    errno_vrfy(chdir(Zeroterm(_db.sandbox_dir)) == 0, "chdir");// we are now in sandbox
}

// -----------------------------------------------------------------------------

// Return from sandbox to original directory.
// Sandbox stays in place.
void acr_ed::ExitSandbox() {
    if (acr_ed::_db.orig_dir != "") {
        prerr("acr_ed.end_sandbox"
              <<Keyval("orig_dir",_db.orig_dir));
        errno_vrfy(chdir(Zeroterm(_db.orig_dir)) == 0, "chdir");
    }
}
