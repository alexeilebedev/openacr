// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/gsymbol.cpp
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
