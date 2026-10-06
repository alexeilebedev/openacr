// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
    //     (user-implemented function, prototype is in amc-generated header)
    // i64 execkey_Get(lib_exec::FSyscmd &cmd);

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
