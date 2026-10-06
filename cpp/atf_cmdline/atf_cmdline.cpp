// Copyright (C) 2023-2024 AlgoRND
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
