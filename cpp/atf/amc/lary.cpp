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
// Source: cpp/atf/amc/lary.cpp
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
