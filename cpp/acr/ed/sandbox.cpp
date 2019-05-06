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
    acr_ed::_db.orig_dir = GetCurDir();
    acr_ed::_db.sandbox_dir = DirFileJoin(GetCurDir(),".testgen");
    prerr("acr_ed.begin_sandbox"
          <<Keyval("orig_dir",_db.orig_dir)
          <<Keyval("sandbox",_db.sandbox_dir));
    SysCmd(tempstr()<<"rsync --delete -a .gitignore bin dflt.* cpp"
           " include extern data diff "<<_db.sandbox_dir, FailokQ(false));
    EnterSandboxX();
    // prevent git from escaping the sandbox
    SysCmd("git init", FailokQ(false));
    // make sure git ls-files knows about these files;
    // acr_ed schedules update-gitfile, and there may be gsymbols
    // generated off of gitfile.ssim, so the list of versioned files
    // must be reasonably valid.
    SysCmd("git add -f --ignore-removal bin cpp include extern data diff", FailokQ(false));
    SysCmd("git commit --allow-empty -m initial-commit", FailokQ(false));
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

// -----------------------------------------------------------------------------

// Build specified tools inside the sandbox
void acr_ed::BuildX(strptr what) {
    command::abt_proc abt;
    abt.cmd.install = true;
    abt.cmd.target.expr = what;
    abt_ExecX(abt);
}
