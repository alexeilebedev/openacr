// (C) 2019 NYSE | Intercontinental Exchange
//
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
//
// Target: atf_amc (exe)
// Exceptions: NO
// Source: cpp/atf/amc/exec.cpp
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

// Should this be generated?
static algo::Fildes amc_StartRead(command::amc_proc &amc, algo_lib::FFildes &read) {
    int pipefd[2];
    int rc=pipe(pipefd);
    (void)rc;
    read.fd.value = pipefd[0];
    amc.stdout  << ">&" << pipefd[1];
    amc_Start(amc);
    (void)close(pipefd[1]);
    return read.fd;
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadProc() {
    // spawn a subprocess and read output line by line
    command::amc_proc amc;
    amc.cmd.query = "command.amc_proc";
    algo_lib::FFildes read;

    prlog("reading output 1");
    cstring out1;
    ind_beg(algo::FileLine_curs,line,amc_StartRead(amc,read)) {
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
        vrfy_(FindStr(bash_ToCmdline(bash),"verbose")==-1);
        vrfy_(bash_Exec(bash)==0);
        algo_lib::_db.cmdline.verbose--;
    }
}
