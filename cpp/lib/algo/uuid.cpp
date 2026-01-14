// Copyright (C) 2025-2026 AlgoRND
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
