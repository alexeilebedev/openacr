// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_exec.cpp
//

#include "include/atf_unit.h"
#include "include/lib_exec.h"
#ifndef WIN32
#include <sys/resource.h>
#endif

void atf_unit::unittest_lib_exec_Parallel1() {
    bool success = false;
    lib_exec::_db.cmdline.maxjobs = 10;
    lib_exec::syscmd_RemoveAll();
    for (int retry=0; retry <3 && !success; retry++) {
        lib_exec::FSyscmd &glob_start    = lib_exec::NewCmd(NULL,NULL);
        lib_exec::FSyscmd &glob_end      = lib_exec::NewCmd(&glob_start,NULL);
        for (int i=0; i<10; i++) {
            lib_exec::FSyscmd &cmd      = lib_exec::NewCmd(&glob_start,&glob_end);
            cmd.command << "sleep 1";
        }
        u64 c = algo::get_cycles();
        lib_exec::SyscmdExecute();
        c = algo::get_cycles() - c;
        success = c < algo::get_cpu_hz() * 3.0;
        success &= !glob_end.fail_prereq;
    }
    vrfy(success, "parallel execution doesn't work");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_lib_exec_TooManyFds() {
    struct rlimit rlim;
    getrlimit(RLIMIT_NOFILE, &rlim);
    rlim.rlim_cur = 100;
    setrlimit(RLIMIT_NOFILE, &rlim);

    lib_exec::_db.cmdline.maxjobs = 10;
    lib_exec::syscmd_RemoveAll();
    lib_exec::FSyscmd &glob_start    = lib_exec::NewCmd(NULL,NULL);
    lib_exec::FSyscmd &glob_end      = lib_exec::NewCmd(&glob_start,NULL);
    for (int i=0; i<100; i++) {
        lib_exec::FSyscmd &cmd      = lib_exec::NewCmd(&glob_start,&glob_end);
        cmd.maxtime = 100;// timeout will not fire, but increases coverage
        if (i == 0) {
            cmd.command << "sleep 10";
        } else {
            cmd.command << "echo lib_exec command "<<i;
        }
    }
    lib_exec::SyscmdExecute();
    vrfy(!glob_end.fail_prereq, "execution failed");

    // restore limit
    rlim.rlim_cur = rlim.rlim_max;
    setrlimit(RLIMIT_NOFILE, &rlim);
    // the long-running first command will keep all other behind
    // FDs because lib_exec retires commands
    // in order of starting, but we shouldn't run out of FDs
    // because it specifically limits max # unretired commands
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_lib_exec_Timeout() {
#ifndef __CYGWIN__
    // run a command with timeout
    algo::SchedTime start(algo::CurrSchedTime());
    lib_exec::syscmd_RemoveAll();
    lib_exec::FSyscmd &glob_start = lib_exec::NewCmd(NULL,NULL);
    lib_exec::FSyscmd &glob_end   = lib_exec::NewCmd(&glob_start,NULL);
    lib_exec::FSyscmd &cmd        = lib_exec::NewCmd(&glob_start,&glob_end);
    cmd.maxtime = 1; // seconds
    cmd.message = "command should exceed time limit";// increases coverage
    cmd.command << "sleep 30";
    lib_exec::SyscmdExecute();
    TESTCMP(cmd.completed, true);
    TESTCMP(cmd.status==0, false);
    // 10 seconds should be enough
    TESTCMP(ElapsedSecs(start,algo::CurrSchedTime()) < 30.0, true);
#endif
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_lib_exec_Dependency() {
    // create script with dependencies
    lib_exec::syscmd_RemoveAll();
    lib_exec::FSyscmd &glob_start = lib_exec::NewCmd(NULL,NULL);
    lib_exec::FSyscmd &glob_end   = lib_exec::NewCmd(&glob_start,NULL);
    lib_exec::FSyscmd &cmd1        = lib_exec::NewCmd(&glob_start,&glob_end);
    lib_exec::FSyscmd &cmd2        = lib_exec::NewCmd(&cmd1,&glob_end);
    lib_exec::FSyscmd &cmd3        = lib_exec::NewCmd(&cmd2,&glob_end);
    lib_exec::FSyscmd &cmd4        = lib_exec::NewCmd(&cmd3,&glob_end);
    unlink("temp/lib_exec.dependency");
    // #AL# echo is not portable
    // especially 'echo -n' -- use printf
    cmd1.command = "sleep 1; printf %d 1 >>temp/lib_exec.dependency";
    cmd2.command = "printf %d 2 >>temp/lib_exec.dependency";
    cmd3.command = "printf %d 3 >>temp/lib_exec.dependency";
    cmd4.command = "printf %d 4 >>temp/lib_exec.dependency";
    lib_exec::SyscmdExecute();

    TESTCMP(glob_end.status, 0);
    vrfy_(algo::FileToString("temp/lib_exec.dependency") == "1234");
    unlink("temp/lib_exec.dependency");
}
