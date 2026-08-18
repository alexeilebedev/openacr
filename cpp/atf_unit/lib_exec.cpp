// Copyright (C) 2023-2024,2026 AlgoRND
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

// --------------------------------------------------------------------------------

// Verify amc-generated _ToArgv forwards -debug to subprocess argv with one fewer
// level, matching how -verbose is already forwarded. Source of truth: the loop
// emitted by amc::tfunc_Exec_ToArgv in cpp/amc/exec.cpp.
void atf_unit::unittest_lib_exec_DebugForward() {
    u8 saved_debug = algo_lib::_db.cmdline.debug;
    u8 saved_verbose = algo_lib::_db.cmdline.verbose;
    algo_lib::_db.cmdline.debug = 3;
    algo_lib::_db.cmdline.verbose = 2;

    command::atf_unit_proc cmd;
    cmd.path = "/proc/self/exe";

    lib_exec::syscmd_RemoveAll();
    lib_exec::FSyscmd &syscmd = lib_exec::NewCmd(NULL, NULL);
    atf_unit_ToArgv(cmd, syscmd.args);

    int n_debug = 0;
    int n_verbose = 0;
    ind_beg(algo::StringAry_ary_curs, arg, syscmd.args) {
        if (arg == "-debug") {
            n_debug++;
        }
        if (arg == "-verbose") {
            n_verbose++;
        }
    }ind_end;

    algo_lib::_db.cmdline.debug = saved_debug;
    algo_lib::_db.cmdline.verbose = saved_verbose;
    lib_exec::syscmd_RemoveAll();

    TESTCMP(n_debug, 2);
    TESTCMP(n_verbose, 1);
}

// --------------------------------------------------------------------------------

static tempstr JoinArgv(algo::StringAry &args) {
    tempstr out;
    ind_beg(algo::StringAry_ary_curs, arg, args) {
        if (ch_N(out)) { out << " "; }
        out << arg;
    }ind_end;
    return out;
}

// Verify amc-generated _ToArgv picks -name:value vs -name value based on whether
// the wrapped command has a ccmdline. acr_ed has one (amc-built; its ReadArgv
// parses colon syntax); bash does not (external tool; needs two-token form).
// Source of truth: the branch on cmdtype.c_ccmdline in amc::tfunc_Exec_ToArgv
// in cpp/amc/exec.cpp.
void atf_unit::unittest_lib_exec_ExecToArgvSyntax() {
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.in = "other";
    acr_ed.cmd.create = true;
    acr_ed.cmd.target = "foo";
    algo::StringAry acr_ed_args;
    command::acr_ed_ToArgv(acr_ed, acr_ed_args);
    TESTCMP(JoinArgv(acr_ed_args), tempstr("bin/acr_ed -in:other -create:Y -target:foo"));

    command::bash_proc bash;
    bash.cmd.c = "echo hi";
    algo::StringAry bash_args;
    command::bash_ToArgv(bash, bash_args);
    TESTCMP(JoinArgv(bash_args), tempstr("bash -c echo hi"));

    // The Ctype.ToArgv overload builds argv straight from the bare command ctype
    // (command::<X>, not the _proc wrapper), ready to hand to algo_lib::ProcStart.
    // args[0] matches $X_ToCmdline and command::<X>_proc.path: an Argv command
    // (acr_ed) is an amc tool under the repo bin/, so it gets the bin/ prefix; an
    // ArgvGnu command (bash, below) is external and stays a bare PATH name.
    command::acr_ed acr_ed_cmd;
    acr_ed_cmd.in = "other";
    acr_ed_cmd.create = true;
    acr_ed_cmd.target = "foo";
    algo::StringAry ctype_args;
    command::acr_ed_ToArgv(acr_ed_cmd, ctype_args);
    TESTCMP(JoinArgv(ctype_args), tempstr("bin/acr_ed -in:other -create:Y -target:foo"));

    command::bash bash_cmd;
    bash_cmd.c = "echo hi";
    algo::StringAry bash_ctype_args;
    command::bash_ToArgv(bash_cmd, bash_ctype_args);
    TESTCMP(JoinArgv(bash_ctype_args), tempstr("bash -c echo hi"));
}
// --------------------------------------------------------------------------------

void atf_unit::unittest_lib_exec_PtyIn() {
#ifndef __CYGWIN__
    // with pty_in, the child sees a tty on stdin while stdout/stderr still
    // flow through the capture tmpfiles with clean newlines
    lib_exec::syscmd_RemoveAll();
    lib_exec::FSyscmd &cmd = lib_exec::NewCmd(NULL,NULL);
    cmd.command << "test -t 0 && echo tty_on_stdin";
    cmd.pty_in = true;
    cmd.show_out = false;
    lib_exec::SyscmdExecute();
    TESTCMP(cmd.completed, true);
    TESTCMP(cmd.status, 0);
    tempstr out(Trimmed(algo::FdToString(cmd.stdout_fd.fd)));
    TESTCMP(out, tempstr("tty_on_stdin"));
#endif
}
