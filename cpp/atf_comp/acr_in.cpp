// Copyright (C) 2026 AlgoX2 Corp
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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/acr_in.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_in_Reverse() {
    atf_comp::ProcStart("$bindir/acr_in -r dev.targdep");
}

void atf_comp::comptest_acr_in_Simple() {
    atf_comp::ProcStart("$bindir/acr_in acr_in");
}

void atf_comp::comptest_acr_in_Tree() {
    atf_comp::ProcStart("$bindir/acr_in acr_in -t");
}
