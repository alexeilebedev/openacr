// Copyright (C) 2020-2021 Astra
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
// Target: atf_nrun (exe) -- Run N subprocesses in parallel
// Exceptions: yes
// Source: cpp/atf/atf_nrun.cpp
//

#include "include/algo.h"
#include "include/atf_nrun.h"

// Attempted every minute until zd_todo is empty
// Increases # of jobs
void atf_nrun::zd_todo_Step() {
    if (atf_nrun::ind_running_N() <atf_nrun::_db.cmdline.maxjobs) {
        atf_nrun::FEntry &next = *atf_nrun::zd_todo_RemoveFirst();
        next.job_path = "bash";// for some reason amc supplies 'bin/bash' by default
        next.job_cmd.c = next.command;// command to execute
        job_Start(next);// spawn it
        next.pid=next.job_pid;// amc can't index by job_pid field, must copy to a user-defined field
        if (next.pid != -1) {
            atf_nrun::ind_running_InsertMaybe(next);// index it
        }
        prlog("atf_nrun.spawn"
              <<Keyval("command",next.command)
              <<Keyval("pid",next.pid)
              <<Keyval("ntodo",atf_nrun::zd_todo_N())
              <<Keyval("nrunning",atf_nrun::ind_running_N()));
    } else {
        zd_todo_SetDelay(algo::ToSchedTime(60.0));
    }
}

// Attempted whenever we suspect that waitpid()
// will return something interesting
// Decreases # of jobs
void atf_nrun::ind_running_Step() {
    int status=0;
    int pid = waitpid(-1,&status,WNOHANG);//nonblocking wait
    if (pid > 0) {
        if (FEntry *entry = ind_running_Find(pid)) {
            ind_running_Remove(*entry);// no longer running
            entry->pid = -1;
            entry->job_pid = -1;// clear pid
            entry->job_status = status;// save status
            // cause zd_todo to execute immediately
            zd_todo_SetDelay(algo::SchedTime());
        }
        prlog("atf_nrun.sigchild"
              <<Keyval("pid",pid)
              <<Keyval("status",status)
              );
    } else {
        // let child processes run until next sigchild
        ind_running_SetDelay(algo::ToSchedTime(60.0));
    }
}

// trigger waitpid call
static void SignalHandler(int sig) {
    (void)sig;
    atf_nrun::ind_running_SetDelay(algo::SchedTime());// cause it to execute immediately
    prlog("SIGCHLD");
}

void atf_nrun::Main() {
    struct sigaction sigact;
    sigact.sa_handler = SignalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction(SIGCHLD, &sigact, 0);

    for (int i=0; i<_db.cmdline.ncmd; i++) {
        FEntry &entry = atf_nrun::fentry_Alloc();
        entry.command << "echo "<<i<<"; sleep 1";
        fentry_XrefMaybe(entry);
        zd_todo_Insert(entry);
    }
    atf_nrun::MainLoop();
}
