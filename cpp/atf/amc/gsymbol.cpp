// Copyright (C) 2023 Astra
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/gsymbol.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_Gsymbol() {
    vrfyeq_(sizeof atfdb_test_gsymbol_char_TestChar, sizeof(char*));
    vrfyeq_(sizeof atfdb_test_gsymbol_pkey_TestPkey, sizeof(atfdb::TestGsymbolPkeyPkey));
    vrfyeq_(sizeof atfdb_test_gsymbol_strptr_TestStrptr, sizeof(algo::strptr));

    vrfy_(!strcmp(atfdb_test_gsymbol_char_TestChar,"TestChar"));
    vrfyeq_(atfdb_test_gsymbol_pkey_TestPkey,"TestPkey");
    vrfyeq_(atfdb_test_gsymbol_strptr_TestStrptr,"TestStrptr");
}
