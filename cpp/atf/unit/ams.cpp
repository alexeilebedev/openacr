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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/ams.cpp
//

#include "include/atf_unit.h"
#include "include/gen/ams_gen.h"
#include "include/gen/ams_gen.inl.h"

// --------------------------------------------------------------------------------

void atf_unit::unittest_ams_StreamId() {
    // Compose a stream ID and check that it prints correctly
    ams::ProcType proctype=ams_ProcType_0;
    ams::StreamType streamtype=ams_StreamType_ctl;
    ams::ProcId proc_id(proctype,0);
    ams::StreamId id = ams::StreamId(proc_id,streamtype,1);
    prlog(id);
    vrfy_((tempstr() << id) == "0-0.ctl-1");

    // Read the ID back and compare with original
    ams::StreamId id2;
    ams::StreamId_ReadStrptrMaybe(id2,(tempstr()<<id));
    vrfy_(id2==id);
}
