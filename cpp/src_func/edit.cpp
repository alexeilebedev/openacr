// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/edit.cpp -- Implementation of -e
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

void src_func::Main_EditFunc() {
    cstring out;
    if (_db.editloc != "") {
        out << _db.editloc;
    } else {
        ind_beg(src_func::_db_bh_func_curs, func, src_func::_db) {
            if (func.select) {
                out << Location(func, 0) << eol;
            }
        }ind_end;
    }
    StringToFile(out, "temp/src_func.txt");
    SysCmd("errlist cat temp/src_func.txt");
}

void src_func::Main_CreateMissing() {
    ind_beg(_db_cppsym_curs,cppsym,_db) if (cppsym.extrn) {
        src_func::FCppsym *alias = ind_cppname_Find(cppname_Get(cppsym));
        src_func::FTarget *target = ind_target_Find(Pathcomp(cppname_Get(cppsym),".LL"));
        // Only a target whose sources were actually scanned (selected by
        // -targsrc) can tell us a function is missing.  Outside the scan
        // scope the absent instance is meaningless -- without this a scoped
        // run (e.g. -targsrc:amc/%) would stub every user function in the tree
        // whose definition merely wasn't scanned.
        bool scanned = false;
        if (target) {
            ind_beg(target_cd_targsrc_curs,targsrc,*target) {
                scanned = scanned || targsrc.select;
            }ind_end;
        }
        if (!zd_func_EmptyQ(cppsym)) {
            // instances exist
        } else if (alias && alias!=&cppsym) {
            // instances don't exist...
            // but another function with the same cpp name exists.
            // this means we can't reliably determine if the function is missing
            // because we don't parse prototypes
        } else if (!scanned) {
            // target out of scan scope -- can't tell whether it is missing
        } else {
            // step 1: determine prefix
            src_func::FGenaffix *affix=FindAffix(cppname_Get(cppsym));
            // step 2: scan source files and count which files have the most functions with the same prefix
            src_func::FTargsrc *bestsrc=NULL;
            ind_beg(target_cd_targsrc_curs,targsrc,*target) if (!StartsWithQ(src_Get(targsrc),"cpp/gen")) {
                targsrc.counter=0;
                ind_beg(targsrc_zd_func_curs,func,targsrc) {
                    src_func::FGenaffix *thisaffix=FindAffix(func.name);
                    if (thisaffix == affix) {
                        targsrc.counter++;
                    }
                }ind_end;
                if (!bestsrc || targsrc.counter>bestsrc->counter) {
                    bestsrc=&targsrc;
                }
            }ind_end;
            if (!bestsrc) {
                bestsrc=cd_targsrc_First(*target);
            }
            // step 3: write missing function to the file
            if (bestsrc) {
                tempstr text = tempstr() << eol << Trimmed(SysEval(tempstr()<<"amc -showcomment:N -report:N "<<cppname_Get(cppsym),FailokQ(true),102400));
                Replace(text,";"," {\n}\n");
                int nline=0;
                ind_beg(algo::FileLine_curs,line,src_Get(*bestsrc)) {
                    (void)line;
                    nline++;
                }ind_end;
                _db.editloc << src_Get(*bestsrc)<<":"<<nline+2<<":"<<eol;
                prlog("# adding "<<cppname_Get(cppsym)<<" to "<<src_Get(*bestsrc));
                prlog(text);
                StringToFile(text, src_Get(*bestsrc), algo_FileFlags_append);
            }
        }
    }ind_end;
}
