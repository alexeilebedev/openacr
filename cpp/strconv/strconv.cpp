// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: strconv (exe) -- A simple string utility
// Exceptions: yes
// Source: cpp/strconv/strconv.cpp
//

#include "include/algo.h"
#include "include/strconv.h"

void strconv::Main() {
    tempstr str(strconv::_db.cmdline.str);
    tempstr temp;
    if (strconv::_db.cmdline.tocamelcase) {
        strptr_PrintCamel(str, temp);
        str = temp;
    } else if (strconv::_db.cmdline.tolowerunder) {
        strptr_PrintLowerUnder(str, temp);
        str = temp;
    }
    if (_db.cmdline.pathcomp != "") {
        temp = Pathcomp(str, _db.cmdline.pathcomp);
        str = temp;
    }
    prlog_(str << (isatty(1) ? "\n" : ""));
}
