// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/edit.cpp -- Implementation of -e
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

void src_func::Main_EditFunc() {
    if (_db.editloc != "") {
        StringToFile(_db.editloc,"temp/src_func.loc");
        SysCmd("errlist cat temp/src_func.loc");
    } else {
        command::src_func cmd = _db.cmdline;
        cmd.showloc      = true;
        cmd.showbody     = false;
        cmd.e            = false;
        SysCmd(tempstr()<<"errlist "<<strptr_ToBash(src_func_ToCmdline(cmd)));
    }
}

void src_func::Main_CreateMissing() {
    ind_beg(_db_userfunc_curs,userfunc,_db) {
        if (!zd_func_EmptyQ(userfunc)) {
            // instances exist
        } else if (ind_userfunc_cppname_Find(userfunc.cppname)!=NULL) {
            // instances don't exist...
            // but another function with the same cpp name exists.
            // this means we can't reliably determine if the function is missing
            // because we don't parse prototypes
        } else {
            // step 1: determine prefix
            src_func::FGenaffix *affix=FindAffix(userfunc.cppname);
            // step 2: scan source files and count which files have the most functions with the same prefix
            src_func::FTargsrc *bestsrc=NULL;
            if (src_func::FTarget *target=ind_target_Find(Pathcomp(userfunc.userfunc,".LL"))) {
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
            }
            // step 3: write missing function to the file
            if (bestsrc) {
                tempstr text = tempstr() << eol << Trimmed(SysEval(tempstr()<<"amc -showcomment:N -report:N "<<userfunc.userfunc,FailokQ(true),102400));
                Replace(text,";"," {\n}\n");
                int nline=0;
                ind_beg(algo::FileLine_curs,line,src_Get(*bestsrc)) {
                    (void)line;
                    nline++;
                }ind_end;
                _db.editloc << src_Get(*bestsrc)<<":"<<nline+2<<":"<<eol;
                prlog("# adding "<<userfunc.userfunc<<" to "<<src_Get(*bestsrc));
                prlog(text);
                StringToFile(text, src_Get(*bestsrc), algo_FileFlags_append);
            }
        }
    }ind_end;
}
