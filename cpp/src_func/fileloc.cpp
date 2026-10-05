// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/fileloc.cpp -- Location in file, for each function
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
