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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/memcheckB.cpp
//

#include "include/algo.h"
#include "include/atf_ci.h"


// -----------------------------------------------------------------------------

// First citest of the memcheckB job: build every target with the memcheck cfg.
void atf_ci::citest_mem_prepB() {
    atf_ci::BuildMemcheck();
}

// -----------------------------------------------------------------------------

// Run the comptests tagged memcheck:memcheckB under valgrind (see
// RunMemcheckShard for how the suite is cut).
void atf_ci::citest_atf_comp_memB() {
    atf_ci::RunMemcheckShard(atfdb_cijob_memcheckB);
}
