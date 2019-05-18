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
// Target: strconv (exe) -- A simple string utility
// Exceptions: yes
// Source: cpp/strconv/strconv.cpp
//

#include "include/algo.h"
#include "include/strconv.h"

void strconv::Main() {
    tempstr out;
    if (strconv::_db.cmdline.tocamelcase) {
        strptr_PrintCamel(strconv::_db.cmdline.str, out);
    } else if (strconv::_db.cmdline.tolowerunder) {
        strptr_PrintLowerUnder(strconv::_db.cmdline.str, out);
    }
    prlog_(out);// raw output
    if (isatty(1)) {
        prlog_("\n");// eol for tty output
    }
}
