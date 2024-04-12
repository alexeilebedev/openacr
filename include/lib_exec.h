// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: lib_exec (lib) -- Build and run a dag of subprocesses with N parallel jobs
// Exceptions: NO
// Header: include/lib_exec.h
//

#include "include/gen/lib_exec_gen.h"
#include "include/gen/lib_exec_gen.inl.h"

namespace lib_exec { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_exec.cpp
    //
    i64 execkey_Get(lib_exec::FSyscmd &cmd);

    // Spawn process associated with command.
    // If the command was started successfully, its pid can be
    // looked up in ind_running, and the command is added to zd_started list.
    void StartCmd(lib_exec::FSyscmd &cmd);

    // Run created command graph and return # of commands that failed.
    // (successful run -> return value 0).
    // Individual command status codes can be examined directly.
    // To reset the graph, call syscmd_RemoveAll().
    int SyscmdExecute();

    // Return a new command record which will be started after START,
    // and is guaranteed to exit before END.
    // This is the basic building block for creating commands.
    lib_exec::FSyscmd &NewCmd(lib_exec::FSyscmd *start, lib_exec::FSyscmd *end);

    // Returns true if command was actually invoked and did exist successfully.
    // If the command never ran, return false.
    bool CompletedOKQ(lib_exec::FSyscmd &cmd);

    // Frees FDs for stdout and stderr
    void RefurbishStd(lib_exec::FSyscmd &cmd);
}
