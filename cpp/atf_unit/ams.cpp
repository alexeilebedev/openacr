// Copyright (C) 2023-2024,2026 AlgoRND
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
// Source: cpp/atf_unit/ams.cpp
//

#include "include/atf_unit.h"
#include "include/gen/ams_gen.h"
#include "include/gen/ams_gen.inl.h"
#include "include/lib_ams.h"

// --------------------------------------------------------------------------------

void atf_unit::unittest_ams_StreamId() {
    // Compose a shm ID and check that it prints correctly
    ams::Proctype proctype=ams_Proctype_;
    ams::Shmtype shmtype=ams_Shmtype_ctl;
    ams::ProcId proc_id = lib_ams::MakeProcId(proctype,1,2);
    ams::ShmId id = ams::ShmId(proc_id,shmtype,1);
    prlog(id);
    vrfy_((tempstr() << id) == "-1-2.ctl-1");

    // Read the ID back and compare with original
    ams::ShmId id2;
    ams::ShmId_ReadStrptrMaybe(id2,(tempstr()<<id));
    vrfy_(id2==id);
}
