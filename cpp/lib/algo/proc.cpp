// Copyright (C) 2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/proc.cpp
//
// algo_lib::FProc spawns any external process from a fully-formed argv
// (args[0] is the executable, resolved against PATH). Once argv is built every
// process behaves identically -- redirects, timeout, pipes, wait, kill, status.
// amc generates only the typed-command-to-argv builder; the lifecycle lives here.

#include "include/algo.h"

// Create a pipe pair.  With cloexec, both ends are O_CLOEXEC so the parent-side
// fd doesn't leak into children forked after this one (dup2 in the child clears
// O_CLOEXEC on fds 0/1/2, so the redirected ends still survive execv).  Returns
// 0 on success.
static int MakePipe(int fd[2], bool cloexec) {
    return cloexec ? pipe2(fd, O_CLOEXEC) : pipe(fd);
}

// Close both ends of a pipe pair that are still open. Used in the child after
// dup2 has aliased the right ends onto fds 0/1/2.
static void ClosePipe(int fd[2]) {
    if (fd[0] >= 0) {
        (void)close(fd[0]);
    }
    if (fd[1] >= 0) {
        (void)close(fd[1]);
    }
}

// Build a shell-legible rendering of the command line and redirects, for logging.
// Args are separated by two spaces so a value that itself contains a space stays
// visually distinguishable from the gap between args.
tempstr algo_lib::ProcToCmdline(algo_lib::FProc &proc) {
    tempstr retval;
    ind_beg(algo::StringAry_ary_curs,arg,proc.args) {
        if (ch_N(retval)) {
            retval << "  ";
        }
        retval << strptr_ToBash(arg);
    }ind_end;
    // Only show file redirects; "|" (pipe) and "<&N"/">&N" (fd dup) are internal
    // plumbing, not a useful part of the displayed command line.
    if (algo_lib::RedirectFileQ(proc.fstdin)) {
        retval << " " << proc.fstdin;
    }
    if (algo_lib::RedirectFileQ(proc.fstdout)) {
        retval << " " << proc.fstdout;
    }
    if (algo_lib::RedirectFileQ(proc.fstderr)) {
        retval << " 2" << proc.fstderr;
    }
    return retval;
}

// Replace the calling (child) process image with proc.args. args[0] is the
// executable name and stays argv[0] (so the program sees its own name in error
// messages); a PATH-resolved copy is used only as the execv pathname.
// Returns only on failure, with errno set.
static int ProcExecv(algo_lib::FProc &proc) {
    int ret = 0;
    char **argv = (char**)alloca((ary_N(proc.args)+1)*sizeof(*argv));
    ind_beg(algo::StringAry_ary_curs,arg,proc.args) {
        argv[ind_curs(arg).index] = Zeroterm(arg);
    }ind_end;
    argv[ary_N(proc.args)] = NULL;
    algo::cstring *path = algo::ary_Find(proc.args, 0);
    if (path != NULL) {
        algo::cstring exe(*path);
        algo_lib::ResolveExecFname(exe); // resolve for the pathname only; argv[0] keeps the name
        ret = execv(Zeroterm(exe), argv);
    }
    return ret;
}

// Start the subprocess if not already running. For each redirect set to "|",
// create a pipe before fork and expose the parent-side fd on proc
// (to_stdin / from_stdout / from_stderr). To merge stderr into the stdout pipe,
// set fstdout="|" and fstderr=">&1". Returns 0, or errno on fork failure.
int algo_lib::ProcStart(algo_lib::FProc &proc) {
    int retval = 0;
    if (proc.pid == 0) {
        verblog(algo_lib::ProcToCmdline(proc)); // maybe print command
#ifdef WIN32
        tempstr cmdline(algo_lib::ProcToCmdline(proc));
        algo::cstring path;
        algo::cstring *p0 = algo::ary_Find(proc.args, 0);
        if (p0 != NULL) {
            path = *p0;
            algo_lib::ResolveExecFname(path);
        }
        proc.pid = dospawn(Zeroterm(path),Zeroterm(cmdline),proc.timeout,proc.fstdin,proc.fstdout,proc.fstderr);
#else
        int in_pipe[2]  = {-1,-1}; // [0]=child stdin (read), [1]=to_stdin (write)
        int out_pipe[2] = {-1,-1}; // [0]=from_stdout (read), [1]=child stdout (write)
        int err_pipe[2] = {-1,-1}; // [0]=from_stderr (read), [1]=child stderr (write)
        if (proc.fstdin  == "|" && MakePipe(in_pipe,  proc.cloexec) == 0) {
            proc.to_stdin.value = in_pipe[1];
        }
        if (proc.fstdout == "|" && MakePipe(out_pipe, proc.cloexec) == 0) {
            proc.from_stdout.value = out_pipe[0];
        }
        if (proc.fstderr == "|" && MakePipe(err_pipe, proc.cloexec) == 0) {
            proc.from_stderr.value = err_pipe[0];
        }
        proc.pid = fork();
        if (proc.pid == 0) { // child
            algo_lib::DieWithParent();
            // inherited signal handlers stay live until exec, so a kill aimed at
            // the child in the fork-to-exec window would run the parent's handler
            // in the child and be consumed instead of killing; restore the default
            // dispositions so the signal does what the sender means
            (void)signal(SIGTERM, SIG_DFL);
            (void)signal(SIGINT , SIG_DFL);
            (void)signal(SIGHUP , SIG_DFL);
            (void)signal(SIGQUIT, SIG_DFL);
            (void)signal(SIGALRM, SIG_DFL);
            if (proc.pgroup) {
                // own process group: the child may spawn descendants of its
                // own (a shell wrapper's command), and a kill by the child's
                // pid alone leaves them orphaned-alive; the group makes the
                // subtree one killable unit (ProcKill sends to -pid)
                (void)setpgid(0, 0);
            }
            if (proc.timeout > 0) {
                alarm(proc.timeout);
            }
            if (retval == 0) {
                retval = algo_lib::ApplyRedirect(proc.fstdin , 0, in_pipe[0]);
            }
            if (retval == 0) {
                retval = algo_lib::ApplyRedirect(proc.fstdout, 1, out_pipe[1]);
            }
            if (retval == 0) {
                retval = algo_lib::ApplyRedirect(proc.fstderr, 2, err_pipe[1]);
            }
            // fds 0/1/2 already alias the right ends; drop every pipe fd
            ClosePipe(in_pipe);
            ClosePipe(out_pipe);
            ClosePipe(err_pipe);
            if (retval == 0) {
                retval = ProcExecv(proc);
            }
            if (retval != 0) { // execv failed
                int err = errno;
                prerr("algo_lib.proc_execv"
                      <<Keyval("errno",err)
                      <<Keyval("errstr",strerror(err))
                      <<Keyval("comment","Execv failed"));
            }
            _exit(127); // if failed to start, exit anyway
        } else if (proc.pid == -1) {
            retval = errno; // failed to fork
        } else if (proc.pgroup) {
            // mirror the child's setpgid: the group must exist the moment fork
            // returns, or a group kill racing the child's first quantum finds no
            // group, loses the signal, and the unkilled child boots into whatever
            // the killer already tore down.  EACCES -- the child exec'd first, its
            // own setpgid won -- is the benign side of the race.
            (void)setpgid(proc.pid, proc.pid);
        }
        // parent: keep only our end of each pipe
        if (in_pipe[0]  >= 0) {
            (void)close(in_pipe[0]);
        }
        if (out_pipe[1] >= 0) {
            (void)close(out_pipe[1]);
        }
        if (err_pipe[1] >= 0) {
            (void)close(err_pipe[1]);
        }
#endif
    }
    proc.status = proc.pid > 0 ? 0 : -1; // if didn't start, set error status
    return retval;
}

// Wait for the subprocess to exit. Close to_stdin first so the child sees EOF,
// waitpid (restarting on EINTR), store status, clear pid, then close the read
// ends. Drain from_stdout / from_stderr before calling to avoid a deadlock.
void algo_lib::ProcWait(algo_lib::FProc &proc) {
    algo_lib::Close(proc.to_stdin);
    if (proc.pid > 0) {
        int wait_flags = 0;
        int wait_status = 0;
        int rc = -1;
        do {
            rc = waitpid(proc.pid,&wait_status,wait_flags);
        } while (rc==-1 && errno==EINTR);
        if (rc == proc.pid) {
            proc.status = wait_status;
            proc.pid = 0;
        }
    }
    algo_lib::Close(proc.from_stdout);
    algo_lib::Close(proc.from_stderr);
}

// Kill the subprocess with SIGKILL and reap it. No-op when not running.
// A pgroup child is killed as a whole group (its descendants with it).
void algo_lib::ProcKill(algo_lib::FProc &proc) {
    if (proc.pid > 0) {
        kill(proc.pgroup ? -proc.pid : proc.pid, 9);
        algo_lib::ProcWait(proc);
    }
}

// Start the subprocess and wait for it; return its wait() status.
int algo_lib::ProcExec(algo_lib::FProc &proc) {
    algo_lib::ProcStart(proc);
    algo_lib::ProcWait(proc);
    return proc.status;
}

// Execute the subprocess; throw a human-readable algo_lib.exec error when the
// child exits non-zero (uses DescribeWaitStatus for the comment).
void algo_lib::ProcExecX(algo_lib::FProc &proc) {
    int rc = algo_lib::ProcExec(proc);
    vrfy(rc==0, tempstr() << "algo_lib.exec"
         << Keyval("cmd", algo_lib::ProcToCmdline(proc))
         << Keyval("comment", algo::DescribeWaitStatus(proc.status)));
}

// Decode the last wait() status into a child exit code, or -1 if the child was
// killed by a signal or has not been reaped.
int algo_lib::ProcExitCode(algo_lib::FProc &proc) {
    int ret = -1;
    if (proc.pid == 0 && WIFEXITED(proc.status)) {
        ret = WEXITSTATUS(proc.status);
    }
    return ret;
}

// On destruction of an FProc, kill and reap the child for forward progress.
void algo_lib::pid_Cleanup(algo_lib::FProc &proc) {
    algo_lib::ProcKill(proc);
}
