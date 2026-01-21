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
// Target: samp_regx (exe) -- Test tool for regular expressions
// Exceptions: NO
// Source: cpp/samp_regx/samp_regx.cpp
//

#include "include/algo.h"
#include "include/samp_regx.h"

void samp_regx::Main() {
    algo_lib::Regx regx;
    trace_Set(regx.flags,_db.cmdline.trace);
    capture_Set(regx.flags,_db.cmdline.capture);
    if (_db.cmdline.f) {
        vrfy(!_db.cmdline.trace, "-show only works with a single string");
    }
    algo_lib::Regx_ReadStyle(regx,_db.cmdline.expr,_db.cmdline.style,_db.cmdline.full);
    if (_db.cmdline.trace) {
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
        if (_db.cmdline.trace) {
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
