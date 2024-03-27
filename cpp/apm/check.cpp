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
// Source: cpp/apm/check.cpp
//

#include "include/algo.h"
#include "include/apm.h"

void apm::Main_Check() {
    // each pkgkey must evaluate to at least one record
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,package) {
            if (c_pkgrec_N(pkgkey)==0) {
                prlog("apm.emptykey"
                      <<Keyval("pkgkey",pkgkey.pkgkey)
                      <<Keyval("comment","this key doesn't match any records"));
                dev::Pkgkey out;
                pkgkey_CopyOut(pkgkey,out);
                prlog("acr.delete  "<<out);
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;

    // each namespace must belong to a package

    // ssimfiles under data/ should not be included in pkgkey, since ssim records
    // are picked up by the corresponding namespace
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,package) {
            if (StartsWithQ(key_Get(pkgkey),"dmmeta.ssimfile:data/")) {
                prlog("apm.badkey"
                      <<Keyval("pkgkey",pkgkey.pkgkey)
                      <<Keyval("comment","ssimfiles should not be included in package definition directly"));
                dev::Pkgkey out;
                pkgkey_CopyOut(pkgkey,out);
                prlog("acr.delete  "<<out);
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;

    // any pkgkey that's dominated by another pkgkey can be eliminated

    // if any record belongs to more than one package, then
    // there must be a package dependency between a later package and an earlier package

    // package must have its own entry under pkgkey
    // because pkgkeys are part of the package definition
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        tempstr key=tempstr()<<package.package<<"/dev.package:"<<package.package;
        if (!ind_pkgkey_Find(key)) {
            prlog("apm.bad_package_def"
                  <<Keyval("package",package.package)
                  <<Keyval("missing_pkgrec",key)
                  <<Keyval("comment","required record missing"));
            dev::Pkgkey pkgkey;
            pkgkey.pkgkey=key;
            prlog("acr.insert  "<<pkgkey);
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}
