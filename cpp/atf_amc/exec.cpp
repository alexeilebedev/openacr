// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/exec.cpp
//

#include "include/atf_amc.h"

static void CheckExecStatus(int scenario, int expected_status) {
    int pid=fork();
    if (pid==0) {// child
        algo_lib::DieWithParent();
        verblog("atf_amc.exec_status"
                <<Keyval("scenario",scenario));
        switch(scenario) {
        case 0: _exit(0); break;
        case 1: _exit(33); break;
        case 2: kill(getpid(), SIGINT); _exit(1); break;
        case 3: alarm(1); sleep(1000); break;
        case 4: kill(getpid(), SIGKILL); break;
        case 5: kill(getpid(), SIGTERM); break;
        default: break;
        }
    } else {//parent
        int status = 0;
        int rc_waitpid = waitpid(pid,&status,0);
        if (rc_waitpid == -1) {
            status = errno;
        } else if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            status = WTERMSIG(status);
        }
        //prlog(algo::DescribeWaitStatus(status));
        vrfy_(status == expected_status);
    }
}

void atf_amc::amctest_Exec_Status() {
    CheckExecStatus(0, 0);
    CheckExecStatus(1, 33);
    CheckExecStatus(2, SIGINT);
    CheckExecStatus(3, SIGALRM);
    CheckExecStatus(4, SIGKILL);
    CheckExecStatus(5, SIGTERM);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadProc() {
    // spawn a subprocess and read output line by line
    command::amc_proc amc;
    amc.cmd.query = "command.amc_proc";

    prlog("reading output 1");
    cstring out1;
    amc.fstdout = "|";
    amc_Start(amc);
    ind_beg(algo::FileLine_curs,line,amc.from_stdout) {
        out1 << line << eol;
    }ind_end;

    prlog("reading output 2");
    // check that the output is the same as running SysEval (popen)
    cstring out2 = SysEval("amc command.amc_proc",FailokQ(true),1024*1024);
    vrfy_(out1==out2);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ExecSh() {
    // spawn a shell subprocess...
    {
        command::bash_proc bash;
        bash.cmd.c = "true";
        bash_ExecX(bash);
    }
    // try return value
    {
        command::bash_proc bash;
        bash.cmd.c = "false";
        vrfy_(bash_Exec(bash)!=0);
    }
    // make sure -verbose doesn't pass down...
    {
        command::bash_proc bash;
        algo_lib::_db.cmdline.verbose++;
        bash.cmd.c = "ls";
        bash.fstdout = ">/dev/null";
        vrfy_(FindStr(bash_ToCmdline(bash),"verbose")==-1);
        vrfy_(bash_Exec(bash)==0);
        algo_lib::_db.cmdline.verbose--;
    }
}

void atf_amc::amctest_ExecVerbose() {
    command::amc_proc amc;
    amc.fstdout = ">/dev/null";
    amc.fstderr = ">/dev/null";
    u8 save = algo_lib::_db.cmdline.verbose;
    // this will exec with verbose off by 1 (254)
    // if no sufficient room allocated in argv, this will cause stack corruption
    algo_lib::_db.cmdline.verbose = 255;
    amc_ExecX(amc);
    algo_lib::_db.cmdline.verbose = save;
}
