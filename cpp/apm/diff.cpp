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
        cstring origin(_db.cmdline.origin == "" ? GetOrigin(package) : algo::strptr(_db.cmdline.origin));
        cstring baseref(_db.cmdline.ref == "" ? GetBaseref(package) : algo::strptr(_db.cmdline.ref));
        // check out the package into sandbox directory.
        // evaluate symbolic value of baseref into actual git commit
        // An origin that cannot be reached resolves to nothing, and nothing is
        // what the sandbox reads as "the current directory": the base would then
        // be a copy of the very tree being diffed, and apm would print an empty
        // diff and exit zero over a fetch that never happened.  A base that is
        // not the published version is not a base at all, so the run stops here.
        baseref=FetchPackageOrigin(package.package,origin,baseref);
        vrfy(baseref!="", tempstr()<<"failed to resolve the base version of package "<<package.package<<" in "<<origin);
        vrfy(CreatePackageSandbox(_db.base_sandbox,baseref)==0,
             tempstr()<<"failed to check out package "<<package.package<<" at "<<baseref);
    }ind_end;

    cstring base_dir(algo_lib::WtDir(_db.base_sandbox));
    PushDiff(base_dir);

    _db.script << "wt "<<_db.base_sandbox<<" -- git diff"
               <<(_db.cmdline.R ? " -R" : "")
               <<(_db.cmdline.stat ? " --stat" : "")
               << eol;

    _db.script << "echo "<<algo::strptr_ToBash("use 'wt apm-base -shell' to examine changes") << eol;
}
