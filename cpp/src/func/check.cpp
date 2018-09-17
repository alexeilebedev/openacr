// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src/func/check.cpp -- Check source code for common errors
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

void src_func::InitBadline() {
    ind_beg(src_func::_db_badline_curs,badline,src_func::_db) {
        (void)Regx_ReadDflt(badline.regx, badline.expr);// full regx
        (void)Regx_ReadSql(badline._targsrc_regx, badline.targsrc_regx, true);// full regx
    }ind_end;

    // construct a gigantic union of badline expressions
    tempstr badline_union(".*(");
    algo::ListSep ls("|");
    ind_beg(src_func::_db_badline_curs,badline,src_func::_db) {
        badline_union << ls << "(" << badline.expr << ")";
    }ind_end;
    badline_union<<").*";
    (void)Regx_ReadDflt(src_func::_db.badline_union_regx, badline_union);
}

// -----------------------------------------------------------------------------

// Check current line for disallowed constructs
// This happens with no regard to position inside source (e.g. could be inside or outside of a function).
//    Function-level checks live in ComputeIffy.
void src_func::CheckLine(strptr trimmedline, strptr fullline) {
    (void)trimmedline;
    if (Regx_Match(src_func::_db.badline_union_regx,fullline)) {
        ind_beg(src_func::_db_badline_curs,badline,src_func::_db) {
            if (Regx_Match(badline.regx, fullline)
                && Regx_Match(badline._targsrc_regx,src_func::_db.c_cur_targsrc->targsrc)) {
                prlog(GetFileloc() << "src_func.badline"
                      <<Keyval("type",badline.badline)
                      <<Keyval("comment",tempstr()<<badline.comment
                               <<". See txt/string-readme.txt for more info"));
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }
}
