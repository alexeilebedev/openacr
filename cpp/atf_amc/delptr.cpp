// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
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
