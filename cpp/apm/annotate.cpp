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
// Source: cpp/apm/annotate.cpp
//

#include "include/algo.h"
#include "include/apm.h"

void apm::Main_Annotate() {
    algo_lib::FFildes fildes;
    fildes.fd = (_db.cmdline.annotate=="-") ? Fildes(dup(0)) : OpenRead(_db.cmdline.annotate);
    ind_beg(algo::FileLine_curs,line,fildes.fd) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptr(tuple,line,false) && attrs_N(tuple)) {
            apm::FRec *rec = ind_rec_Find(tempstr() << tuple.head << ":" << attrs_Find(tuple,0)->value);
            if (rec) {
                ind_beg(rec_zd_rec_pkgrec_curs,pkgrec,*rec) {
                    attr_Add(tuple,"pkgkey",pkgrec.p_pkgkey->pkgkey);
                }ind_end;
            }
            prlog(tuple);
        }
    }ind_end;
}
