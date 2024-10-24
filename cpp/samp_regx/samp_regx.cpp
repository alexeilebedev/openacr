// Copyright (C) 2023-2024 AlgoRND
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
    switch (_db.cmdline.style) {
    case command_samp_regx_style_acr: Regx_ReadAcr(regx,_db.cmdline.expr,true); break;
    case command_samp_regx_style_shell: Regx_ReadShell(regx,_db.cmdline.expr,true); break;
    case command_samp_regx_style_classic: Regx_ReadDflt(regx,_db.cmdline.expr); break;
    case command_samp_regx_style_literal: Regx_ReadLiteral(regx,_db.cmdline.expr); break;
    }
    if (_db.cmdline.show) {
        prlog("expr: "<<regx.expr);
        prlog("states: "<<regx.state_n);
        prlog("accept state: "<<regx.accept);
        prlog("parseerror: "<<regx.parseerror);
        prlog("accepts_all: "<<regx.accepts_all);
        prlog("literal: "<<regx.literal);
        ind_beg(algo_lib::regx_state_curs,state,regx) {
            prlog("state #"<<ind_curs(state).index);
            ind_beg(algo_lib::RegxState_ch_class_curs,ch_class,state) {
                prlog("  "<<ch_class.beg<<".."<<ch_class.end);
            }ind_end;
            tempstr out;
            algo::ListSep ls;
            ind_beg(algo_lib::Bitset_ary_bitcurs,outstate,state.out) {
                out<<ls<<outstate;
            }ind_end;
            prlog("  accept_all:"<<state.accept_all);
            prlog("  out:"<<out);
        }ind_end;
    }
    if (_db.cmdline.match) {
        algo_lib::_db.exit_code=Regx_Match(regx,_db.cmdline.string) ? 0:1;
    }
}
