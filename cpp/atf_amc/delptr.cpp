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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/delptr.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_Delptr() {
    atf_amc::DelType1 deltype1;
    vrfy_(!deltype1.u32val);
    u32 &value = u32val_Access(deltype1);
    vrfyeq_(deltype1.u32val==NULL, false);
    vrfyeq_(value,34); // default value
    u32val_Delete(deltype1);
    vrfy_(!deltype1.u32val);
}
