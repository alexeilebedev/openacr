// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/edit.cpp -- Implementation of -e
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

void src_func::Main_EditFunc() {
    // tempstr out;
    // show loaded functions in sorted order
    // ind_beg(src_func::_db_bh_func_curs,func,src_func::_db) {
    //     if (func.select) {
    //         PrintProto(func,out);
    //         out << eol;
    //     }
    // }ind_end;
    command::src_func cmd;
    cmd.e            = false;
    cmd.in           = src_func::_db.cmdline.in;
    cmd.targsrc.expr = src_func::_db.cmdline.targsrc.expr;
    cmd.name.expr    = src_func::_db.cmdline.name.expr;
    cmd.body.expr    = src_func::_db.cmdline.body.expr;
    cmd.func.expr    = src_func::_db.cmdline.func.expr;
    cmd.comment.expr = src_func::_db.cmdline.comment.expr;
    cmd.iffy         = src_func::_db.cmdline.iffy;
    cmd.listfunc     = src_func::_db.cmdline.listfunc;
    cmd.proto        = src_func::_db.cmdline.proto;
    cmd.gen          = src_func::_db.cmdline.gen;
    cmd.showloc      = true;
    cmd.showstatic   = src_func::_db.cmdline.showstatic;
    cmd.showsortkey  = src_func::_db.cmdline.showsortkey;
    cmd.sortname     = src_func::_db.cmdline.sortname;
    SysCmd(tempstr()<<"errlist '"<<src_func_ToCmdline(cmd)<<"'");
}
