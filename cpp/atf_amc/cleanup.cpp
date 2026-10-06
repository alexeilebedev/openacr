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
// Source: cpp/atf_amc/cleanup.cpp
//

#include "include/atf_amc.h"

// --------------------------------------------------------------------------------

void atf_amc::field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2) {// gets called second
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 0);
    cleanup2.field1 = 0;
}

void atf_amc::field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2) {// gets called first
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 100);
    cleanup2.field2 = 0;
}

void atf_amc::amctest_CleanupOrder() {
    atf_amc::AmcCleanup2 cleanup2;
    vrfyeq_(cleanup2.field1, 0);
    vrfyeq_(cleanup2.field2, 0);
    cleanup2.field1 = 100;
    cleanup2.field2 = 100;
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 100);
    AmcCleanup2_Uninit(cleanup2);

    // check that fields got cleaned up
    vrfyeq_(cleanup2.field1, 0);
    vrfyeq_(cleanup2.field2, 0);

    // initialize again because Uninit is getting called a second time!
    cleanup2.field1 = 100;
    cleanup2.field2 = 100;
}
