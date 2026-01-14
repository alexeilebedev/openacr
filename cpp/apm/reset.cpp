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
// Source: cpp/apm/reset.cpp
//

#include "include/algo.h"
#include "include/apm.h"

void apm::Main_Reset() {
    vrfy(zd_sel_package_N()==1, "-reset requires a single package to be selected");
    cstring acrscript;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        dev::Package out;
        package_CopyOut(package,out);
        if (_db.cmdline.origin != "") {
            out.origin=_db.cmdline.origin;
        }
        if (_db.cmdline.ref != "") {
            out.baseref=_db.cmdline.ref;
        }
        acrscript << out << eol;
    }ind_end;
    if (acrscript!="") {
        _db.script << "acr -replace -write -report:N << EOF\n" << acrscript << "\nEOF\n";
    }
}
