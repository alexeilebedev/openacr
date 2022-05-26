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
// Target: ntup (exe) -- Tuple utility
// Exceptions: yes
// Source: cpp/ntup.cpp
//

#include "include/algo.h"
#include "include/ntup.h"

// -----------------------------------------------------------------------------

void ntup::Cmd(algo::Tuple &tuple) {
    tempstr cmd;

    // show whole line
    cmd << "tuple=";
    tempstr temp;
    Tuple_Print(tuple, temp);
    strptr_PrintBash(temp, cmd);
    cmd<< eol;

    // show tuple head
    cmd << "head=";
    strptr_PrintBash(tuple.head.value, cmd);
    cmd<<eol;

    // print the command
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        cmd << attr.name << "=";
        strptr_PrintBash(attr.value, cmd);
        cmd << eol;
    }ind_end;

    cmd << _db.cmdline.cmd<<eol;
    prlog(cmd);
}

// -----------------------------------------------------------------------------

void ntup::Main() {
    ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptr(tuple,line,false)) {
            if (Regx_Match(_db.cmdline.typetag,tuple.head.value)) {
                if (_db.cmdline.cmd != "") {
                    Cmd(tuple);
                }
                // select field
                if (_db.cmdline.field != "") {
                    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
                        if (attr.name == _db.cmdline.field) {
                            prlog(attr.value);
                        }
                    }ind_end;
                }
            }
        }
    }ind_end;
}
