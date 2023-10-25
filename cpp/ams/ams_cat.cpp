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
// Target: ams_cat (exe) -- Algo Messaging System sample tool
// Exceptions: yes
// Source: cpp/ams/ams_cat.cpp
//
// Read messages from STDIN and echo any produced messages to
// STDOUT.
// No shared memory. This is a test tool.

#include "include/algo.h"
#include "include/ams_cat.h"
#include "include/lib_ams.h"

// -----------------------------------------------------------------------------

void ams_cat::Main() {
    ams::ProcId my_id(ams_ProcType_amstest,0); // amstest-0
    lib_ams::SetDfltShmSize(1024*32);
    lib_ams::Init("", my_id);
    Regx_ReadSql(lib_ams::_db.regx_trace,"%",true);

    // amstest-0.ctl-0
    lib_ams::FStream &ctl0=lib_ams::ind_stream_GetOrCreate(ams::StreamId(my_id, ams_StreamType_ctl,0));
    vrfy_(lib_ams::AddCtlIn(ctl0));

    // amstest-0.log-0
    lib_ams::FStream &out=lib_ams::ind_stream_GetOrCreate(ams::StreamId(my_id, ams_StreamType_out,0));
    vrfy_(lib_ams::OpenStream(out,ams::StreamFlags(ams_StreamFlags_write|ams_StreamFlags_nonblock),ams::StreamPos()));
    ams_cat::MainLoop();// process events
}
