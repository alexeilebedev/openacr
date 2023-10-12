// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/src/func/fileloc.cpp -- Location in file, for each function
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

// Remember current file location
void src_func::SaveFileloc(src_func::FTargsrc &targsrc, int lineno) {
    src_func::_db.c_cur_targsrc = &targsrc;
    src_func::_db.cur_line = lineno;
}

// -----------------------------------------------------------------------------

// Get current file location in the form 'filename:lineno: '
tempstr src_func::GetFileloc() {
    return tempstr()<<src_Get(*src_func::_db.c_cur_targsrc)<<":"<<src_func::_db.cur_line<<": ";
}

// -----------------------------------------------------------------------------

// Get function file location in the form 'filename:lineno: '
// Second argument specifies line offset within the function (starting with 0)
tempstr src_func::Location(src_func::FFunc &func, int lineoffset) {
    return tempstr()<<src_Get(*func.p_targsrc) << ":"<< func.line + lineoffset << ": ";
}
