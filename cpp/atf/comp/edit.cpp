// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
// Exceptions: yes
// Source: cpp/atf/comp/edit.cpp -- Edit comptest
//

#include "include/atf_comp.h"

// -----------------------------------------------------------------------------

// Open selected testcases in an editor
// Upon completion, replace all selected testcases with new data
void atf_comp::Main_EditSelected() {
    // print selected testcases to a string
    cstring str;
    Main_Print(str,true,true);
    strptr fname = "temp/atf_comp.ssim";
    SafeStringToFile(str,fname);
    // open in editor
    int rc=SysCmd(tempstr() << "$EDITOR "<<fname);
    if (rc==0) {
        // read back
        algo_lib::FFildes file;
        file.fd = algo::OpenRead(fname);
        Main_Read(file.fd);
        // any selected testcases that are not marked need_write
        // were not read back from the file, which means they were deleted.
        // Remove them from zd_out_comptest so they won't get saved back
        ind_beg(_db_comptest_curs,comptest,_db) {
            if (zd_sel_comptest_InLlistQ(comptest) && !comptest.need_write) {
                ind_beg(comptest_zd_tmsg_curs,tmsg,comptest) {
                    zd_out_tmsg_Remove(tmsg);
                }ind_end;
                if (comptest.c_targs) {
                    zd_out_targs_Remove(*comptest.c_targs);
                }
                if (comptest.c_tfilt) {
                    zd_out_tfilt_Remove(*comptest.c_tfilt);
                }
                zd_out_comptest_Remove(comptest);
                zd_sel_comptest_Remove(comptest);
                _db.nchange++;
            }
        }ind_end;
    } else {
        prlog("Editor return exit code "<<rc<<"; Edited testcases remain in "<<fname);
        prlog("To insert them, use `atf_comp -i < "<<fname<<"`");
    }
}
