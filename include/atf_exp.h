// Copyright (C) 2024,2026 AlgoRND
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
// Target: atf_exp (exe) -- ATF expect
// Exceptions: yes
// Header: include/atf_exp.h
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/gen/atf_exp_gen.h"
#include "include/gen/atf_exp_gen.inl.h"
#include "include/lib_ams.h"

namespace atf_exp { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_exp/atf_exp.cpp
    //

    // Add variable NAME=VALUE to global replscope
    // And append a line to $tempdir/vars
    void SetVar(algo::strptr name, algo::strptr value);

    // remove directory
    //     (user-implemented function, prototype is in amc-generated header)
    // void In_RemDirRecurseMsg(ams::RemDirRecurseMsg &msg); // dispatch_msg:atf_exp.In/ams.RemDirRecurseMsg

    // start new subprocess
    // void In_ProcStartMsg(ams::ProcStartMsg &msg); // dispatch_msg:atf_exp.In/ams.ProcStartMsg

    // Switch focus to process, start reading process output
    // and waiting `until` string.
    // Empty string means until process exit.
    // Launch timer.
    // void In_ProcReadMsg(ams::ProcReadMsg &msg); // dispatch_msg:atf_exp.In/ams.ProcReadMsg

    // send message to process
    // void In_ProcMsg(ams::ProcMsg &msg); // dispatch_msg:atf_exp.In/ams.ProcMsg

    // Close process stdin from write side, delay close in case of non-empty buffer
    // void In_ProcEofMsg(ams::ProcEofMsg &msg); // dispatch_msg:atf_exp.In/ams.ProcEofMsg

    // kill process
    // void In_ProcKillMsg(ams::ProcKillMsg &msg); // dispatch_msg:atf_exp.In/ams.ProcKillMsg

    // Terminate
    // void In_TerminateMsg(ams::TerminateMsg &); // dispatch_msg:atf_exp.In/ams.TerminateMsg

    // read stdin
    // void cd_fdin_read_Step(); // fstep:atf_exp.FDb.cd_fdin_read

    // stdin eof
    // void cd_fdin_eof_Step(); // fstep:atf_exp.FDb.cd_fdin_eof

    // Read shelved process output, put to stdout.
    // Search `until` string. When found, stop reading process output,
    // keep the rest of data in the buffer and switch back to stdin.
    // Check process exit, issue process status message, delete process.
    void ProcRead(atf_exp::FProc &proc);

    // timeout hook
    void Timeout();

    // issue process status message
    void ProcStatus(atf_exp::FProc &proc);

    // process read hook:
    // - capture process variables in form of `$name:value`,
    // which could be utilized when launching next process using `$name` substitution
    // in ams.ProcStartMsg  cmd:"... $name ..."
    // - store process output as ams.ProcMsg in process output buffer (shelf)
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_proc_read_Step(); // fstep:atf_exp.FDb.cd_proc_read

    // write to subprocess
    // void cd_proc_outflow_Step(); // fstep:atf_exp.FDb.cd_proc_outflow

    // wait subprocess
    // void ind_proc_pid_Step(); // fstep:atf_exp.FDb.ind_proc_pid

    // Handle SIGCHLD
    void SigchldHandler(int sig);

    // Setup signal handlers
    void SetupSignalHandlers();

    // invoke bash with interactive input
    void TimeoutBash();

    // main routine
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:atf_exp
}
