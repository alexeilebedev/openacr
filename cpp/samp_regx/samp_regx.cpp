// Copyright (C) 2026 AlgoX2 Corp
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
// Target: samp_regx (exe) -- Test tool for regular expressions
// Exceptions: NO
// Source: cpp/samp_regx/samp_regx.cpp
//

#include "include/algo.h"
#include "include/samp_regx.h"

void samp_regx::Main() {
    algo_lib::Regx regx;
    trace_Set(regx.flags,_db.cmdline.regxtrace);
    capture_Set(regx.flags,_db.cmdline.capture);
    if (_db.cmdline.f) {
        vrfy(!_db.cmdline.regxtrace, "-show only works with a single string");
    }
    algo_lib::Regx_ReadStyle(regx,_db.cmdline.expr,_db.cmdline.style,_db.cmdline.full);
    if (_db.cmdline.regxtrace) {
        prlog("expr: "<<regx.expr);
        prlog("style: "<<regx.style);
        prlog("flags: "<<regx.flags);
    }
    int nmatch=0;
    if (_db.cmdline.f) {
        ind_beg(algo::FileLine_curs,line,_db.cmdline.string) {
            if (Regx_Match(regx,line)) {
                prlog(line);
                nmatch++;
            }
        }ind_end;
    } else {
        nmatch += Regx_Match(regx,_db.cmdline.string);
        if (_db.cmdline.regxtrace) {
            prlog("match: "<<bool(nmatch>0));
        }
        if (_db.cmdline.capture) {
            algo::ListSep ls(", ");
            cstring out("match range: ");
            ind_beg(algo::I32RangeAry_ary_curs,range,algo_lib::_db.regxm.matchrange) {
                out<<ls<<range;
            }ind_end;
            prlog(out);
        }
    }
    if (_db.cmdline.match) {
        algo_lib::_db.exit_code=nmatch>0 ? 0:1;
    }
}
