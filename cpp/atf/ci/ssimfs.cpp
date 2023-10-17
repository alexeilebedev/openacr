// Copyright (C) 2023 AlgoRND
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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf/ci/ssimfs.cpp
//

#include "include/atf_ci.h"

// check ssimfs rules
void atf_ci::citest_ssimfs() {
    bool ok=true;
    ind_beg(_db_ssimfs_curs,ssimfs,_db) {
        file_RemoveAll();
        // execute "lscmd", and create File entry.
        // mark it as "file_exists"
        if (ch_N(ssimfs.lscmd)) {
            tempstr files=SysEval(ssimfs.lscmd,FailokQ(true),1024*1024*100);
            ind_beg(algo::Line_curs,line,files) {
                auto &file = ind_file_GetOrCreate(line);
                file.file_exists=true;
            }ind_end;
        }
        // load ssimfile.
        // create File entry for each record that should be considered,
        // and mark it as "record_exists"
        tempstr ssimfname = algo::SsimFname(_db.cmdline.in,ssimfile_Get(ssimfs));
        verblog("loading "<<ssimfname);
        ind_beg(algo::FileLine_curs,line,ssimfname) {
            if (!algo_lib::Regx_Match(ssimfs.excl,line)) {
                algo_lib::Replscope R;
                lib_ctype::FillReplscope(R,line);
                tempstr fname = Subst(R,file_Get(ssimfs));
                auto &file = ind_file_GetOrCreate(fname);
                file.file_exists=FileObjectExistsQ(fname);
                file.record_exists=true;
            }
        }ind_end;
        // scan resulting File records and report mismatches
        // delete extra files
        ind_beg(_db_file_curs,file,_db) {
            if (file.file_exists && !file.record_exists) {
                if (ssimfs.rmfile) {
                    SysCmd(tempstr()<<"git rm -f "<<file.file);
                }
                prlog("atf_ci.ssimfs_extra_file"
                      <<Keyval("file",file.file)
                      <<Keyval("removed",ssimfs.rmfile)
                      <<Keyval("comment",tempstr()<<"not in "<<ssimfile_Get(ssimfs)));
                ok=false;
            } else if (file.record_exists && !file.file_exists) {
                if (ssimfs.needfile) {
                    prlog("atf_ci.ssimfs_missing_file"
                          <<Keyval("file",file.file)
                          <<Keyval("comment",tempstr()<<"required by "<<ssimfile_Get(ssimfs)));
                    ok=false;
                }
            }
        }ind_end;
    }ind_end;
    vrfy(ok,"mismatches between ssim records and the filesystem found");
}
