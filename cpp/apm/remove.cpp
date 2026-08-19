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
// Source: cpp/apm/remove.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// Given zd_sel_package list of packages to remove,
// populate zd_selrec list with records that need to be deleted.
// Records belonging to multiple packages are handled properly
// and are not removed if remaining packages still own them.
static void CollectPkgrecForRemoval() {
    apm::zd_selrec_RemoveAll();
    ind_beg(apm::_db_zd_sel_package_curs, package, apm::_db) {
        apm::SelectPkgRecs(package);
    }ind_end;

    // populate a list of all transitive deps for zd_sel_package
    ind_beg(apm::_db_zd_sel_package_curs, package, apm::_db) {
        apm::zd_temp_package_Insert(package);
    }ind_end;
    ind_beg(apm::_db_zd_temp_package_curs, package, apm::_db) {
        ind_beg(apm::package_c_pkgdep_curs, pkgdep, package) {
            zd_temp_package_Insert(*pkgdep.p_parent);
        }ind_end;
    }ind_end;


    apm::FRec* cur = apm::zd_selrec_First();
    while (cur) {
        // keep a records if some package outside of zd_sel_package transitive deps closure still has it
        bool keep_rec = false;
        ind_beg(apm::rec_zd_rec_pkgrec_curs, pkgrec, *cur) {
            keep_rec |= !apm::zd_temp_package_InLlistQ(*pkgrec.p_package);
        }ind_end;

        apm::FRec* next = apm::zd_selrec_Next(*cur);
        if (keep_rec) {
            apm::zd_selrec_Remove(*cur);
        }
        cur = next;
    }
}

// Remove selected packages
// - Remove package record
// - Remove pkgfile and pkgkey records
// - Remove git files
// TODO: this must be implemented as an UPDATE
// with target being an empty set.
void apm::Main_Remove() {
    vrfy(zd_sel_package_N()>0, "apm: no packages selected");
    cstring acrscript;

    CollectPkgrecForRemoval();
    // The records to delete are read back from the file, not from the list they
    // were selected into, and the write deletes the file before rewriting it. A
    // write that fails therefore leaves no file to read: the walk below finds
    // nothing, so the script deletes no record and no git file, and apm reports a
    // package removed that is still installed. The removal is abandoned instead,
    // which also stops the transaction from running at all.
    bool saved = SaveSelrecToFile(_db.ours_recfile);
    if (!saved) {
        algo_lib::_db.exit_code++;
    } else {
        // remove gitfiles from the records file
        ind_beg(algo::FileLine_curs,line,_db.ours_recfile) {
            dev::Gitfile gitfile;
            if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
                _db.script << "git rm -q --force "<<strptr_ToBash(gitfile.gitfile)<<eol;
            }
            acrscript << "acr.delete "<<line<<eol;
        }ind_end;

        if (acrscript!="") {
            _db.script << "acr -replace -write -print:N -report:N << EOF\n" << acrscript << "\nEOF\n";
        }

        // re-generate code after removing package
        _db.script << "update-gitfile" << eol;
        _db.script << "amc -report:N" << eol;
    }
}
