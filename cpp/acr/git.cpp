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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/git.cpp -- Git triggers
//

#include "include/algo.h"
#include "include/acr.h"

void acr::Main_GitTriggers() {
    acr::FSsimfile *gitfile = ind_ssimfile_Find("dev.gitfile");
    algo::cstring script;
    if (gitfile) {
        ind_beg(acr::ctype_zd_selrec_curs,selrec,*gitfile->p_ctype) {
            if (selrec.del) {
                script << "git rm --force "<<strptr_ToBash(selrec.pkey)<<eol;
            } else if (selrec.oldpkey && *selrec.oldpkey != selrec.pkey) {
                script << "git mv "<<strptr_ToBash(*selrec.oldpkey)<<" "<<strptr_ToBash(selrec.pkey)<<eol;
            } else if (selrec.isnew) {
                script << "touch "<<strptr_ToBash(selrec.pkey)<<eol;
                script << "git add "<<strptr_ToBash(selrec.pkey)<<eol;
            }
        }ind_end;
    }
    if (_db.cmdline.write) {
        command::bash_proc bash;
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile,"acr");
        StringToFile(script,tempfile.filename);
        bash.fstdin << "<"<<tempfile.filename;
        bash_Exec(bash);
    } else {
        prlog("# acr output script");
        prlog(script);
    }
}
