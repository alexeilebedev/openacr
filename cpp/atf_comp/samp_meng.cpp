// Copyright (C) 2026 AlgoRND
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
// Source: cpp/atf_comp/samp_meng.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_samp_meng_Smoke() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/samp_meng");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"configure\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewSymbolReqMsg symbol:A");
    atf_comp::ProcWrite(proc, "samp_meng.NewSymbolReqMsg symbol:B");
    atf_comp::ProcWrite(proc, "samp_meng.NewSymbolReqMsg symbol:C");
    atf_comp::ProcWrite(proc, "samp_meng.NewSymbolReqMsg symbol:D");
    atf_comp::ProcWrite(proc, "samp_meng.NewUserReqMsg user:1");
    atf_comp::ProcWrite(proc, "samp_meng.NewUserReqMsg user:2");
    atf_comp::ProcWrite(proc, "samp_meng.NewUserReqMsg user:3");
    atf_comp::ProcWrite(proc, "samp_meng.NewUserReqMsg user:4");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"place 2 buy orders on the book\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:1 symbol:A price:10.0 qty:100 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:1 symbol:A price:10.0 qty:100 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"sell order incoming, takes out both orders\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:2 symbol:A price:10.0 qty:-300 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"sell IOC order incoming, does nothing\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:2 symbol:A price:10.0 qty:-300 ioc:true");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"buy order incoming, matches sell side, price improved, rest cancels back\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:3 symbol:A price:11.0 qty:200 ioc:true");
    atf_comp::ProcWrite(proc, "samp_meng.TextMsg text:\"place a few orders; and mass-cancel others\"");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:3 symbol:B price:10.0 qty:100 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:3 symbol:B price:10.0 qty:100 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.NewOrderReqMsg user:3 symbol:B price:10.0 qty:100 ioc:false");
    atf_comp::ProcWrite(proc, "samp_meng.MassCancelReqMsg user:3");
}
