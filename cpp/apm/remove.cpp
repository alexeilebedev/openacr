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
// Source: cpp/apm/remove.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// Remove selected packages
// - Remove package record
// - Remove pkgfile and pkgkey records
// - Remove git files
// TODO: this must be implemented as an UPDATE
// with target being an empty set.
void apm::Main_Remove() {
    vrfy(zd_sel_package_N()>0, "apm: no packages selected");
    cstring acrscript;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        apm::CollectPkgrecToFile(package,_db.ours_recfile);
        // remove gitfiles from the records file
        ind_beg(algo::FileLine_curs,line,_db.ours_recfile) {
            dev::Gitfile gitfile;
            if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
                _db.script << "git rm -q --force "<<strptr_ToBash(gitfile.gitfile)<<eol;
            }
            acrscript << "acr.delete "<<line<<eol;
        }ind_end;
    }ind_end;
    if (acrscript!="") {
        _db.script << "acr -replace -write -print:N -report:N << EOF\n" << acrscript << "\nEOF\n";
    }
    // re-generate code after removing package
    _db.script << "update-gitfile" << eol;
    _db.script << "amc -report:N" << eol;
}
