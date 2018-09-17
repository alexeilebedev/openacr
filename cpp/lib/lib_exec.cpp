// (C) 2014-2019 NYSE | Intercontinental Exchange
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
// Target: lib_exec (lib) -- Build and run a dag of subprocesses with N parallel jobs
// Exceptions: NO
// Source: cpp/lib/lib_exec.cpp
//
// Created By: david.hawthorne alexei.lebedev
// Authors: alexei.lebedev david.hawthorne
// Recent Changes: alexei.lebedev jeremy.xue jeffrey.wang
//

#include "include/lib/lib_exec.h"
#include <sys/wait.h>
#include <sys/resource.h>   // setrlimit()

i64 lib_exec::execkey_Get(lib_exec::FSyscmd &cmd) {
    return (i64(cmd.nprereq) << 32) | (i64(cmd.syscmd) & 0xffffffff);
}

// -----------------------------------------------------------------------------

static void ShowOutput(lib_exec::FSyscmd &cmd) {
    bool print_msg = true;
    bool show_out = cmd.show_out && ValidQ(cmd.stdout_fd.fd);
    bool show_err = cmd.show_out && ValidQ(cmd.stderr_fd.fd);

    // In dry run mode, print commands only
    // Reveal command name if command failed or produced any stderr output (that will be printed).
    bool print_cmd = lib_exec::_db.cmdline.dry_run
        || (show_err && GetFileSize(cmd.stderr_fd.fd) > 0)
        || (show_out && cmd.status != 0);

    print_msg &= !lib_exec::_db.cmdline.q; // determine if we want to show user-supplied message
    print_msg &= ch_N(cmd.message) > 0;    // don't if empty
    print_msg &= !cmd.fail_prereq;         // don't print if pre-requisite command failed
    const char *sep = "\n";
    if (print_msg) {                       // Print command message (supplied by user)
        prlog(sep << cmd.message);
        sep = "";
    }

    print_cmd &= ch_N(cmd.command) > 0;    // don't print if there was no command
    print_cmd &= !cmd.fail_prereq;         // if the command is failing because some prior command failed, don't print
                                           // print command name and status
    if (print_cmd) {
        tempstr outstr;
        outstr << sep << cmd.command;
        if (cmd.status != 0) {
            outstr << ": status "<<cmd.status;
        }
        prerr(outstr);
    }

    // Dump stdout.
    // Dump stderr.
    // Remove stdout, stderr files
    if (show_out) {
        (void)algo::CopyFd(cmd.stdout_fd.fd, Fildes(1));
        Refurbish(cmd.stdout_fd);
    }
    if (show_err) {
        (void)algo::CopyFd(cmd.stderr_fd.fd, Fildes(2));
        Refurbish(cmd.stderr_fd);// delete
    }
}

// -----------------------------------------------------------------------------

static void SetupRedirect(lib_exec::FSyscmd &cmd) {
    // Strip group,other writable bits before creating temp file
    umask(S_IWGRP | S_IWOTH);
    // do redirect to temporary file
    // (redirect is skipped if only one node is being used)
    char buf[128];
    strcpy(buf,"/tmp/execXXXXXX");
    cmd.stdout_fd.fd = algo::Fildes(mkstemp(buf));
    unlink(buf);// unlink temporary file from filesystem

    strcpy(buf,"/tmp/execXXXXXX");
    cmd.stderr_fd.fd = algo::Fildes(mkstemp(buf));
    unlink(buf);// unlink temporary file from filesystem
}

// -----------------------------------------------------------------------------

static void Child(lib_exec::FSyscmd &cmd) {
    // show executing command before redirecting output
    verblog(cmd.command);
    if (cmd.maxtime > 0) {// alarm in child process
        struct rlimit rlim;
        rlim.rlim_cur = cmd.maxtime;
        rlim.rlim_max = cmd.maxtime;
        setrlimit(RLIMIT_CPU, &rlim);// RLIMIT_CPU will not be hit for a hanging process
        alarm(cmd.maxtime);// limit physical time as well
    }
    algo_lib::DieWithParent();
    // Redirect output of this shell command to make sure any output
    // by the shell itself becomes part of stderr.
    if (cmd.redir_out) {// child
        dup2(cmd.stdout_fd.fd.value, 1);  // closes 1 first if necessary
        dup2(cmd.stderr_fd.fd.value, 2);  // closes 2 first if necessary
        Refurbish(cmd.stdout_fd);
        Refurbish(cmd.stderr_fd);
    }
    // execute child process
    int ret = execl("/bin/sh", "/bin/sh", "-c", Zeroterm(cmd.command), (char*)NULL);
    errno_vrfy(ret==0, "can't exec");
    _exit(1);// not sure if _exit or exit should be called here
}


// -----------------------------------------------------------------------------

// Spawn process associated with command.
// If the command was started successfully, its pid can be
// looked up in ind_running, and the command is added to zd_started list.
void lib_exec::StartCmd(lib_exec::FSyscmd &cmd) {
    if (lib_exec::_db.cmdline.dry_run || !ch_N(cmd.command) || cmd.fail_prereq) {
        if (cmd.fail_prereq) {
            cmd.status = 1;
        }
    } else {
        if (cmd.redir_out) {
            SetupRedirect(cmd);
        }
        int pid = fork();
        if (pid == 0) {
            Child(cmd);
        } else if (pid > 0) {// parent, good fork
            cmd.pid = pid;
            lib_exec::ind_running_InsertMaybe(cmd);
            lib_exec::zd_started_Insert(cmd);
        } else {// parent, nothing happened
            cmd.status = 1;
        }
    }
}

// -----------------------------------------------------------------------------

static void MarkStopped(lib_exec::FSyscmd *syscmd) {
    ShowOutput(*syscmd);
    lib_exec::zd_started_Remove(*syscmd);
}

// -----------------------------------------------------------------------------

static void MarkStoppedInorder() {
    // show output of all commands that completed in the order in which they
    // were issued.
    while (lib_exec::FSyscmd *syscmd = lib_exec::zd_started_First()) {
        bool good = syscmd->completed;
        // skip past nodes that are clearly there for dependency only -- this helps
        // keep total # of open file descriptors down.
        good |= !ch_N(syscmd->command) && !ch_N(syscmd->message);
        if (good) {
            MarkStopped(syscmd);
        } else {
            break;
        }
    }
}

// -----------------------------------------------------------------------------

// execute all commands in lib_exec::syscmd
// using fork/exec according to dependencies specified in lib_exec::cmddep
// Collect stdout/stderr results from each command into temporary files.
// If a command fails or produces any stderr output, print the output.
// Output buffering prevents interleaved/incomprehensible output from sub-commands.
// Output appears in order of spawning, so it is immune to racing conditions
// between commands. Only final execution time goes down.
//
//
// MAX_JOBS at a time are kept running.
// Algorithm:
// insert all entries into heap (lib_exec::bh_syscmd) with key =
// number of prior deps.
// remove items one by one
//    if one of the pre-requisites failed, fail this command as well,
//    setting fail_prereq flag.
//    otherwise spawn subprocess, record pid into hash index ind_running.
// go until no commands left and no children running left.
//
// exceptions:
//   - dependency loop
//   - child didn't exit (bug in dependency graph)
// screen output:
//   - for all failed commands, print the command line (again)
//
// Return number of failed commands
//
// Limitation: operates on entire lib_exec::syscmd table.
// The table and its associated indices (ind_running, ind_syscmd, cmddep, bh_syscmd) have to be
// cleaned before next use.
//

// BUGS: this is a copy of the code in abt.cpp. UNIFY??
// This function blocks. Wouldn't it be nicer to rewrite it in terms of SIGCHLD
// and a non-blocking step function...
//
//

static void SyscmdExecuteStep() {
    // Determine next command that can potentially run
    lib_exec::FSyscmd *potential_cmd = lib_exec::bh_syscmd_First();
    if (potential_cmd && potential_cmd->nprereq > 0) {
        potential_cmd = NULL;
    }
    // limit max # of started but un-completed commands, these hold fds
    // open and cause eventual out-of-fds error
    if (potential_cmd && lib_exec::zd_started_N()/2 > lib_exec::_db.cmdline.maxjobs) {
        potential_cmd = NULL;
    }

    // Determine next command to start (if any).
    lib_exec::FSyscmd *start_cmd = NULL;
    if (lib_exec::ind_running_N() < lib_exec::_db.cmdline.maxjobs && potential_cmd) {
        start_cmd = potential_cmd;
        lib_exec::bh_syscmd_RemoveFirst();
    }

    lib_exec::FSyscmd *compl_cmd = NULL;
    if (start_cmd) {
        lib_exec::StartCmd(*start_cmd);
        if (!lib_exec::zd_started_InLlistQ(*start_cmd)) {
            compl_cmd = start_cmd;
        }
    }

    // Wait for a command to complete.
    // If ind_running is full, do a blocking wait
    if (compl_cmd == NULL && lib_exec::ind_running_N() > 0) {
        int flags  = WNOHANG;
        // If it isn't possible to spawn a new command,
        // then the wait should be blocking.
        if (potential_cmd == NULL || lib_exec::ind_running_N() >= lib_exec::_db.cmdline.maxjobs) {
            flags = 0;
        }
        int compl_status=0;
        int compl_pid = waitpid(-1, &compl_status, flags);
        int signal = 0;
        if (WIFSIGNALED(compl_status)) {
            signal = WTERMSIG(compl_status);
        }
        if (WIFSTOPPED(compl_status)) {
            signal = WSTOPSIG(compl_status);
        }
        if (compl_pid > 0) {
            compl_cmd = lib_exec::ind_running_Find(compl_pid);
            if (compl_cmd) {
                // #AL# WEXITSTATUS is only valid if WIFEXITED is true.
                // otherwise, we end up exonerating the child process of its bad exit code!
                // TODO: add ci test for "bad child"
                compl_cmd->status = WIFEXITED(compl_status) ? WEXITSTATUS(compl_status) : 1;
                compl_cmd->signal = signal;
                if (signal == SIGALRM) { // time-out, kill the whole process group
                    kill(-compl_pid, SIGKILL);
                }
            }
        }
    }

    // Enable subsequent commands to run
    if (compl_cmd) {
        compl_cmd->completed = true;
        lib_exec::ind_running_Remove(*compl_cmd);
        ind_beg(lib_exec::syscmd_c_next_curs, dep, *compl_cmd) {
            dep.p_child->fail_prereq |= compl_cmd->status != 0;
            dep.p_child->nprereq--;
            lib_exec::bh_syscmd_Reheap(*dep.p_child);
        }ind_end;
        if (lib_exec::_db.cmdline.complooo) {
            MarkStopped(compl_cmd);
        }
    }
}

// -----------------------------------------------------------------------------

// Run created command graph and return # of commands that failed.
// (successful run -> return value 0).
// Individual command status codes can be examined directly.
// To reset the graph, call syscmd_RemoveAll().
int lib_exec::SyscmdExecute() {
    // compute the execution heap
    ind_beg(lib_exec::_db_syscmd_curs, syscmd,lib_exec::_db) {
        syscmd.nprereq = c_prior_N(syscmd);
        lib_exec::bh_syscmd_Insert(syscmd);
    }ind_end;

    while (lib_exec::bh_syscmd_N() > 0 || lib_exec::ind_running_N() > 0) {
        SyscmdExecuteStep();
        MarkStoppedInorder();
    }
    int nfail = 0;
    ind_beg(lib_exec::_db_syscmd_curs, syscmd,lib_exec::_db) {
        nfail += syscmd.status != 0 && !syscmd.fail_prereq;
    }ind_end;
    return nfail;
}

// -----------------------------------------------------------------------------

// Return a new command record which will be started after START,
// and is guaranteed to exit before END.
// This is the basic building block for creating commands.
lib_exec::FSyscmd &lib_exec::NewCmd(lib_exec::FSyscmd *start, lib_exec::FSyscmd *end) {
    u64 idx = lib_exec::syscmd_N();
    lib_exec::FSyscmd &syscmd = lib_exec::syscmd_Alloc();
    syscmd.syscmd = idx;// same as rowid...
    syscmd_XrefMaybe(syscmd);
    if (start) {
        lib_exec::syscmddep_InsertMaybe(dev::Syscmddep(syscmd.syscmd, start->syscmd));
    }
    if (end) {
        lib_exec::syscmddep_InsertMaybe(dev::Syscmddep(end->syscmd, syscmd.syscmd));
    }
    return syscmd;
}

// -----------------------------------------------------------------------------

// Returns true if command was actually invoked and did exist successfully.
// If the command never ran, return false.
bool lib_exec::CompletedOKQ(lib_exec::FSyscmd &cmd) {
    return cmd.status==0
        && cmd.completed
        && !cmd.fail_prereq;
}

// -----------------------------------------------------------------------------

// Frees FDs for stdout and stderr
void lib_exec::RefurbishStd(lib_exec::FSyscmd &cmd) {
    if (ValidQ(cmd.stdout_fd.fd)){
        Refurbish(cmd.stdout_fd);
    }
    if (ValidQ(cmd.stderr_fd.fd)){
        Refurbish(cmd.stderr_fd);
    }
}
