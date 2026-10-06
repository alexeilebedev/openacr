// Copyright (C) 2025-2026 AlgoX2 Corp
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/uuid.cpp
//

#include "include/algo.h"
#include <uuid/uuid.h>

void algo::GenerateRandomUuid(algo::Uuid &uuid) {
    uuid_generate_random(uuid.value_elems);
}

algo::Uuid algo::GenerateRandomUuid() {
    algo::Uuid ret;
    GenerateRandomUuid(ret);
    return ret;
}

bool algo::NullUuidQ(const algo::Uuid &uuid) {
    return uuid_is_null(uuid.value_elems);
}
