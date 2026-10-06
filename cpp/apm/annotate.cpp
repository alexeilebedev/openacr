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
