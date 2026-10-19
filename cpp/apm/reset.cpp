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
// Source: cpp/apm/reset.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// Set the selected package's origin and baseref from the command line, and
// record this tree's HEAD as localref when the origin's commit is the
// projection of HEAD: -reset with -ref closes the sync loop after a push.
// When the two disagree on a file -- this tree gained one after the
// publication, or the destination dropped one that this tree still has -- no
// pair exists, and localref is left empty, with each such file reported as
// apm.syncdrift and the row as apm.unpaired.
// The three values form the dev.pkgupstream row of the destination -dest names,
// which the reset creates when the package has no such row; the destination
// defaults to the package's only one, or to "origin" for a package with none.
// A ref given with -ref is resolved against the origin before it is stored, so
// what lands in the record is a commit id.  Storing the name instead would let
// the origin move the branch afterwards, and the record would then describe a
// version this tree has never merged while still reading as the version it has.
void apm::Main_Reset() {
    vrfy(zd_sel_package_N()==1, "-reset requires a single package to be selected");
    cstring acrscript;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        apm::FPkgupstream *row = GetPkgupstream(package);
        dev::Pkgupstream out;
        out.pkgupstream = dev::Pkgupstream_Concat_package_dest(package.package, _db.cmdline.dest != "" ? algo::strptr(_db.cmdline.dest) : algo::strptr("origin"));
        out.origin = GetOrigin(package);
        out.baseref = GetBaseref(package);
        if (row) {
            pkgupstream_CopyOut(*row,out);
        }
        if (_db.cmdline.origin != "") {
            out.origin=_db.cmdline.origin;
        }
        if (_db.cmdline.ref != "") {
            tempstr gitref = FetchPackageOrigin(out.pkgupstream,out.origin,_db.cmdline.ref);
            vrfy(gitref!="", tempstr()<<"failed to resolve "<<_db.cmdline.ref<<" in "<<out.origin);
            out.baseref=gitref;
            tempstr head(RevParseMaybe("HEAD"));
            int ndrift = CountSyncDrift(package,out.pkgupstream,out.origin,gitref,head);
            out.localref = ndrift == 0 ? algo::strptr(head) : algo::strptr();
            if (ndrift > 0) {
                prlog("apm.unpaired"
                      <<Keyval("pkgupstream",out.pkgupstream)
                      <<Keyval("baseref",gitref)
                      <<Keyval("ndrift",ndrift)
                      <<Keyval("comment","HEAD's projection is not the origin at baseref, so localref is left empty"));
            }
        }
        acrscript << out << eol;
    }ind_end;
    if (acrscript!="") {
        _db.script << "acr -replace -write -report:N << EOF\n" << acrscript << "\nEOF\n";
    }
}
