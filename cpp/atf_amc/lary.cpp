// Copyright (C) 2021 Astra
// Copyright (C) 2018 NYSE | Intercontinental Exchange
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
// Source: cpp/atf_amc/lary.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_LaryFind() {
    atf_amc::Lary32 lary;
    vrfy_(lary_EmptyQ(lary));
    u32 &i = lary_Alloc(lary);
    i = 1;
    u32 &j = lary_Alloc(lary);
    j = 2;
    vrfyeq_(lary_Find(lary, -2), NULL);
    vrfyeq_(lary_Find(lary, -1), NULL);
    vrfyeq_(*lary_Find(lary, 0), i);
    vrfyeq_(*lary_Find(lary, 1), j);
    vrfyeq_(lary_Find(lary, lary_N(lary)), NULL);
}
