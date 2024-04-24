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
// Source: cpp/atf/comp/read.cpp -- Read comptest from input
//

#include "include/atf_comp.h"

// -----------------------------------------------------------------------------

// Read FD, create testcases based on the following mini-language
// comptest <name> <args> -- component test name & command-line arguments
// comment <text>    -- comment
// exit_code <code>  -- exit code
// filter <code>     -- output filter
// expect <msg>      -- expected outputline
// <anything else>   -- input message
// New testcases are added to zd_out_comptest, and will be subject
// to selection in a subsequent step
void atf_comp::Main_Read(algo::Fildes fd) {
    atf_comp::FComptest *comptest=NULL;// current test
    int rank=0;
    ind_beg(algo::FileLine_curs,line,fd) if (!StartsWithQ(line,"#")) {
        strptr rest=Trimmed(Pathcomp(line," LR"));
        if (StartsWithQ(line,"comptest")) {
            line=rest;
            strptr name=Pathcomp(line," LL");
            rest=Trimmed(Pathcomp(line," LR"));
            comptest = ind_comptest_Find(name);
            if (!comptest) {
                comptest = &comptest_Alloc();
                comptest->comptest=name;
                comptest_XrefMaybe(*comptest);
            } else {
                zd_tmsg_Cascdel(*comptest);// clear our tmsgs under this test
                c_targs_Cascdel(*comptest);
                c_tfilt_Cascdel(*comptest);
            }
            verblog("# read testcase "<<comptest->comptest);
            atf_comp::FTargs &targs=targs_Alloc();
            targs.comptest=comptest->comptest;
            targs_XrefMaybe(targs);
            comptest->c_targs->args=rest;
            comptest->need_write=true;// mark as modified
            _db.nchange++;
            rank=0;
            _db.report.ninsert++;
        } else if (comptest && StartsWithQ(line,"comment")) {
            comptest->comment.value = rest;
        } else if (comptest && StartsWithQ(line,"exit_code")) {
            u8_ReadStrptrMaybe(comptest->exit_code,rest);
        } else if (comptest && StartsWithQ(line,"filter")) {
            if (!comptest->c_tfilt) {
                atf_comp::FTfilt &tfilt=tfilt_Alloc();
                tfilt.comptest=comptest->comptest;
                tfilt_XrefMaybe(tfilt);
            }
            comptest->c_tfilt->filter=rest;
        } else if (comptest) {
            bool expect = StartsWithQ(line,"expect");
            strptr dir = expect ? "exp" : "in";
            strptr msg = algo::Trimmed(expect ? ch_RestFrom(line,6) : strptr(line));
            if (ch_N(msg)) {
                atf_comp::FTmsg &tmsg=tmsg_Alloc();
                tmsg.tmsg=atfdb::Tmsg_Concat_comptest_rank_dir(comptest->comptest,100000+rank*10,dir);
                tmsg.msg=msg;
                rank++;
                (void)tmsg_XrefMaybe(tmsg);// will succeed
            }
        } else {
            prlog("# ignore "<<Keyval("line",line));
        }
    }ind_end;
}
