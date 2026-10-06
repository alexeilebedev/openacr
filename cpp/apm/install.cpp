// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
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
    apm::FPackage *ret=package_InsertMaybe(out);
    vrfy(ret,"failed to create package record");
    // An empty baseref means the current directory, with whatever local changes,
    // is the base of the merge.  The update writes this row to the tree.
    dev::Pkgupstream pkgupstream;
    pkgupstream.pkgupstream=dev::Pkgupstream_Concat_package_dest(out.package, _db.cmdline.dest != "" ? algo::strptr(_db.cmdline.dest) : algo::strptr("origin"));
    pkgupstream.origin=_db.cmdline.origin;
    vrfy(pkgupstream_InsertMaybe(pkgupstream),"failed to create pkgupstream record");
    zd_sel_package_Insert(*ret);
    apm::Main_Update();
}
