// Copyright (C) 2024,2026 AlgoRND
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
// Source: cpp/apm/diff.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// -----------------------------------------------------------------------------

// Show local modifications made to package contents
// with respect to baseref
// With -origin, all packages are evaluated with respected to origin URL, and a single diff is shown
// Without -origin, diff must be requested for one package only.
void apm::Main_Diff() {
    vrfy(zd_sel_package_N()>0, "no package selected");
    vrfy(_db.cmdline.origin!="" || zd_sel_package_N()==1,tempstr()<<"apm: please select only 1 package ("<<zd_sel_package_N()
         <<" selected, or use -origin to diff all packages against one source)");
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        // allow overriding origin from command line
        cstring origin(_db.cmdline.origin == "" ? algo::strptr(package.origin) : algo::strptr(_db.cmdline.origin));
        cstring baseref(_db.cmdline.ref == "" ? algo::strptr(package.baseref) : algo::strptr(_db.cmdline.ref));
        // check out the package into sandbox directory.
        // evaluate symbolic value of baseref into actual git commit
        baseref=FetchPackageOrigin(origin,baseref);
        CreatePackageSandbox(_db.base_sandbox,baseref);
    }ind_end;

    cstring base_dir(algo_lib::SandboxDir(_db.base_sandbox));
    PushDiff(base_dir);

    _db.script << "sandbox "<<_db.base_sandbox<<" -- git diff "
               <<(_db.cmdline.R ? "-R" : "")
               <<(_db.cmdline.stat ? "--stat" : "")
               << eol;

    _db.script << "echo "<<algo::strptr_ToBash("use 'sandbox apm-base -shell' to examine changes") << eol;
}
