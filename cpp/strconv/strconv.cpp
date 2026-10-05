// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
