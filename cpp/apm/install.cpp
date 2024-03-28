// Copyright (C) 2024 AlgoRND
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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/install.cpp
//

#include "include/algo.h"
#include "include/lib_git.h"
#include "include/apm.h"

// Install package specified with options -package, -origin, -ref
// Install is implemented as an update, i.e. 3-way merge with base = current directory
void apm::Main_Install() {
    vrfy(zd_sel_package_N()==0,"apm: the package appears to be already installed");
    vrfy(_db.cmdline.origin != "", "apm: please specify origin URL with -origin");
    vrfy(_db.cmdline.package.expr != "", "apm: please specify package name to install");
    dev::Package out;
    out.package=_db.cmdline.package.expr;
    out.origin=_db.cmdline.origin;
    // specify empty baseref. this means current directory with whatever local changes
    // becomes the base
    out.baseref="";
    apm::FPackage *ret=package_InsertMaybe(out);
    vrfy(ret,"failed to create packate record");
    zd_sel_package_Insert(*ret);
    apm::Main_Update();
}
