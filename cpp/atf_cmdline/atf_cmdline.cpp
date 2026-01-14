// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: atf_cmdline (exe) -- Test tool for command line parsing
// Exceptions: yes
// Source: cpp/atf_cmdline/atf_cmdline.cpp
//

#include "include/algo.h"
#include "include/atf_cmdline.h"

#define PRINT(name)                             \
    prlog(#name ":" << _db.cmdline.name)

#define PRINT_TARY(name)                                            \
    ind_beg(command::atf_cmdline_##name##_curs,name,_db.cmdline) {  \
        prlog(#name "." << ind_curs(name).index << ":" << name);    \
    }ind_end


void atf_cmdline::Main() {
    tempstr err;
    if (_db.cmdline.exec) {
        command::atf_cmdline_proc proc;
        proc.path = algo_lib::_db.argv[0];
        algo::TSwap(proc.cmd, _db.cmdline);
        proc.cmd.exec = false;
        prlog(atf_cmdline_ToCmdline(proc.cmd));
        vrfy_(command::atf_cmdline_Execv(proc)==0);
    } else {
        PRINT(astr);
        PRINT(anum);
        PRINT(adbl);
        PRINT(aflag);
        PRINT(str);
        PRINT(num);
        PRINT(dbl);
        PRINT(flag);
        PRINT(dstr);
        PRINT(dnum);
        PRINT(ddbl);
        PRINT(dflag);
        PRINT_TARY(mstr);
        PRINT_TARY(mnum);
        PRINT_TARY(mdbl);
        PRINT_TARY(amnum);
        prlog("fconst:" << fconst_ToCstr(_db.cmdline));
        PRINT(cconst);
        PRINT(dregx.expr);
        PRINT(dpkey);
        prlog("verbose:"<<algo_lib::_db.cmdline.verbose);
        prlog("debug:"<<algo_lib::_db.cmdline.debug);
    }
}
