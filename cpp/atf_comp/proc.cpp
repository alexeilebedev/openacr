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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/proc.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"
#include <sys/wait.h>
#include <poll.h>

// Set test variable and add to Replscope
void atf_comp::SetVar(strptr name, strptr value) {
    Set(_db.R, tempstr() << "$" << name, value);
}

// Apply $-substitution to string
static tempstr Subst(strptr s) {
    tempstr out;
    Ins(&atf_comp::_db.R, out, s);
    return out;
}

// Export the environment every test of this tree is run under, as the
// atfdb.testenv table states it.
//
// A comptest that starts a cluster needs that cluster pointed at this test's
// own directory rather than at whatever instance the host is running, and the
// name of the variable that does the pointing belongs to the platform under
// test, not to the runner.  So each row names a variable and a value, the
// value goes through the same $-substitution a test script does -- `$tempdir`
// is this test's directory -- and a tree with no such platform simply has no
// rows.  SLOW says the run is instrumented and therefore far slower than
// release, which is when a row marked slowonly applies: startup graces belong
// to that case and would otherwise put progress lines in the goldens.
void atf_comp::SetTestenv(bool slow) {
    ind_beg(atf_comp::_db_testenv_curs,testenv,atf_comp::_db) {
        if (slow || !testenv.slowonly) {
            setenv(Zeroterm(tempstr()<<testenv.testenv), Zeroterm(Subst(testenv.value)), 1);
        }
    }ind_end;
}

// Append line to test log; with -v, also print to stdout
static void Log(strptr line) {
    atf_comp::_db.log << line << "\n";
    verblog(line);
}

// If stablefld is set on the current comptest, replace unstable field values with ***
// Check if attribute is unstable by looking up head.attr and %.attr
static bool UnstableAttrQ(strptr head, strptr attrname) {
    bool ret = false;
    if (atf_comp::ind_unstableattr_Find(tempstr() << head << "." << attrname)) {
        ret = true;
    } else if (atf_comp::ind_unstableattr_Find(tempstr() << "%." << attrname)) {
        ret = true;
    }
    return ret;
}

static tempstr StabilizeLine(strptr line) {
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    if (ct && ct->stablefld) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptrMaybe(tuple, line)) {
            bool changed = false;
            ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                if (UnstableAttrQ(tuple.head.value, attr.name)) {
                    attr.value = "***";
                    changed = true;
                }
            }ind_end;
            if (changed) {
                tempstr out;
                Tuple_Print(tuple, out);
                return out;
            }
        }
    }
    return tempstr(line);
}

// Emit output lines to log as "proc -> line"
// Skip gcov profiling noise from coverage-built binaries.
static void LogOutput(atf_comp::FProc &proc, strptr text) {
    ind_beg(algo::Line_curs, line, text) {
        if (ch_N(line) > 0
            && !StartsWithQ(line, "profiling:")
            && algo::FindStr(line, ".gcda:") == -1) {
            Log(tempstr() << proc.proc << " -> " << StabilizeLine(line));
        }
    }ind_end;
}

// Derive unique process name from command.
// Extract basename from first word of cmd, then add -N suffix if name already used.
static tempstr DeriveProcName(strptr cmd) {
    strptr first_word = Pathcomp(cmd, " LL");
    tempstr base(Pathcomp(first_word, "/RR"));
    // find unique name
    tempstr name = tempstr()<< base;
    int suffix = 2;
    while (atf_comp::ind_proc_Find(name)) {
        name = tempstr() << base << "-" << suffix;
        suffix++;
    }
    return name;
}

// Whether a program name is a shell, i.e. a program whose job is to run the
// command it is handed as a child process instead of doing the work itself.
// The whole name is matched, never a suffix: a comptest can name a script
// such as kafka-console-consumer.sh, which is an ordinary program.
static bool ShellQ(strptr name) {
    return name == "sh" || name == "bash" || name == "dash" || name == "ksh" || name == "zsh";
}

// Spawn subprocess with $-substitution, return reference.
atf_comp::FProc &atf_comp::ProcStart(strptr cmd) {
    tempstr name = DeriveProcName(cmd);
    Log(tempstr() << "# start " << name << " cmd:" << strptr_ToSsim(Trimmed(cmd)));
    tempstr cmd_eff = Subst(cmd);
    cmd_eff = tempstr() << algo::TrimmedRight(cmd_eff);
    atf_comp::FProc &proc = proc_Alloc();
    // wrap command based on mode.  Coverage needs no wrapping: under
    // -cfg:coverage the command already names build/coverage/<tool>, and its
    // gcda are routed by the GCC_PROFILE_DIR atf_ci exports.
    u8 mode = atf_comp::_db.cmdline.mode;
    if (mode == command_atf_comp_mode_memcheck && proc_N() == 1) { // first process only; rest are auxiliary
        // Valgrind instruments the process it execs.  Most commands name the
        // tool under test directly, so that process is the tool and memcheck
        // sees the allocations the test is about.  A command written as a
        // shell script ("bash -c 'cd ... && tool'") differs: the process
        // valgrind execs is the shell, the tool runs as its child, and a
        // memory error in the tool passes the run green.  Following children
        // instruments the tool through the shell.
        //
        // A pipeline is out of reach of child tracing.  Bash parses
        // "echo request | tool" after the prefix has been glued on, so the
        // process valgrind execs is the echo, and the tool runs beside it as
        // a sibling.  Child tracing follows a process's descendants, and a
        // sibling is not one of them, so the tool would run uninstrumented
        // and the row would pass green on the echo's allocations.  A comptest
        // that needs input on the tool's stdin writes it with ProcWrite,
        // which leaves the command a simple one naming the tool.
        //
        // Children are followed only for a shell command, because following
        // them also reaches a supervisor's children.  A cluster comptest starts
        // a supervisor, which spawns the node processes; each of those
        // would run instrumented too, and a test that takes four seconds
        // does not finish in twenty minutes.  Those node processes are what
        // the test drives, not the program it tests.
        bool shell_cmd = ShellQ(Pathcomp(cmd_eff, " LL/RR"));
        // Each traced process writes its own log (%p = pid; a shared
        // log file would be clobbered, each process truncates it on open);
        // ProcWait checks every log the run produced. The test's tempdir is
        // wiped before each run, so no stale log can contaminate the glob.
        // The path handed to valgrind must be absolute: each traced process
        // opens its log from its own working directory, and a compound
        // command typically cds into the tempdir before running the tool.
        atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
        tempstr logbase;
        logbase << "temp/atf_comp/" << (ct ? strptr(ct->comptest) : strptr("unknown")) << "/" << name << ".memcheck";
        proc.memcheck_log = logbase;
        // The log path embeds the checkout's absolute directory, and two
        // parsers read it in turn: bash (the whole command runs through
        // bash -c) and valgrind.  Valgrind reads '%' in --log-file as a
        // format escape (only %p, %q, %% are valid) and aborts at startup
        // on anything else; bash word-splits on spaces and expands '$', so
        // a checkout under a path like '/home/u/my work' hands valgrind a
        // truncated log path and the remainder as the program to run.
        // Double every '%' in the path portion so only the trailing %p
        // stays a live escape, then bash-quote the result.
        tempstr logfile = algo::DirFileJoin(algo::GetCurDir(), logbase);
        algo::Replace(logfile, "%", "%%");
        // Child tracing reaches every program the script runs, and a
        // comptest that compiles a file runs the whole C++ toolchain: the
        // driver, the compiler proper, the assembler, the linker.  Valgrind
        // finds uninitialised reads in GCC's own register allocator, so a
        // comptest whose script happens to invoke a compiler fails on errors
        // in a binary this repo does not build.  Valgrind's verdict on such
        // a binary is not a verdict about this repo, so tracing stops at the
        // toolchain.  Skipping the driver is what does the work, because the
        // stages it spawns then run outside valgrind altogether; the stages
        // are named as well, for a build that invokes one of them directly.
        // The patterns are bash-quoted: '*' is a glob character, and the
        // whole valgrind wrap is spliced into a command bash parses.
        strptr trace_child = shell_cmd ? " --trace-children=yes --trace-children-skip='*/cc,*/c++,*/cc1,*/cc1plus,*/gcc,*/gcc-*,*/g++,*/g++-*,*/clang,*/clang++,*/as,*/ld,*/collect2'" : "";
        cmd_eff = tempstr() << "valgrind --tool=memcheck" << trace_child << " --log-file=" << algo::strptr_ToBash(logfile) << ".%p.log " << cmd_eff;
    } else if (mode == command_atf_comp_mode_valgrind && proc_N() == 1) {
        cmd_eff = tempstr() << "valgrind " << cmd_eff;
    }
    proc.proc = name;
    proc.status = -1;
    // run the (shell) command through bash; FProc's cloexec (default) keeps the
    // pipe fds out of other procs' children, where an inherited copy would hold
    // the pipe open and block EOF.
    ary_Alloc(proc.subproc.args) = "bash";
    ary_Alloc(proc.subproc.args) = "-c";
    ary_Alloc(proc.subproc.args) = cmd_eff;
    proc.subproc.fstdin  = "|"; // write end exposed as to_stdin
    proc.subproc.fstdout = "|"; // read end exposed as from_stdout
    proc.subproc.fstderr = ">&1"; // fold stderr into the stdout pipe
    algo_lib::ProcStart(proc.subproc);
    // take ownership of the parent-side pipe ends; the harness (and the fbuf
    // reader) close them, so detach from subproc to avoid a double close.
    proc.stdin_fd = proc.subproc.to_stdin;
    algo::Fildes read_fd = proc.subproc.from_stdout;
    proc.subproc.to_stdin = algo::Fildes();
    proc.subproc.from_stdout = algo::Fildes();
    algo::SetBlockingMode(read_fd, false);
    in_BeginRead(proc, read_fd);
    proc_XrefMaybe(proc);
    return proc;
}

// Write a line to process stdin. If proc.subst, apply $-substitution.
// A tool that rejects its command line exits before it reads fd 0, so the
// write can reach a pipe whose read end is already gone. What the tool did
// with the line reaches the golden through the tool's own output and exit
// code, and the line itself is recorded above either way, so a broken pipe
// is the tool's verdict rather than a harness failure and is not diagnosed
// as one. Any other write error still aborts the test.
// SIGPIPE is ignored only for the duration of the write: the disposition is
// process-global and survives exec into every child atf_comp spawns later,
// so leaving SIG_IGN set would silently change how the tools under test
// react to their own broken pipes.
void atf_comp::ProcWrite(atf_comp::FProc &proc, strptr msg) {
    Log(tempstr() << proc.proc << " <- " << msg);
    tempstr line;
    line << Trimmed(Subst(msg)) << "\n";
    sighandler_t prior = signal(SIGPIPE, SIG_IGN);
    ssize_t nwrite = write(proc.stdin_fd.value, line.ch_elems, line.ch_n);
    int err = errno;
    signal(SIGPIPE, prior);
    errno = err;
    errno_vrfy_(nwrite == (ssize_t)line.ch_n || (nwrite == -1 && err == EPIPE));
}

// Send signal to process
void atf_comp::ProcKill(atf_comp::FProc &proc, int signal) {
    Log(tempstr() << "# kill " << proc.proc << " signal:" << signal);
    if (proc.subproc.pid > 0) {
        kill(proc.subproc.pid, signal);
        proc.killed = true;
    }
}

// Close process stdin (signal EOF)
void atf_comp::ProcWriteEof(atf_comp::FProc &proc) {
    if (ValidQ(proc.stdin_fd)) {
        Log(tempstr() << "# eof " << proc.proc);
        close(proc.stdin_fd.value);
        proc.stdin_fd = algo::Fildes();
    }
}

// Multiplier on a comptest's wall-clock budget for the build under test.
//
// A comptest that starts a cluster and drives a client through it takes tens of
// seconds on the release build, and atfdb.comptest.timeout is the budget that
// work is given.  gcov instrumentation and valgrind each run the same work
// several times slower, so on those builds the budget expires while the test is
// still doing what it was asked to do, and the run is reported as a timeout of
// whichever process the harness happened to be reading.  The number states the
// time the test needs, so the harness scales it by what this build costs rather
// than every comptest carrying a figure tuned for the slowest one.
static double TimeoutScale() {
    bool instrumented = atf_comp::_db.cmdline.cfg == dev_Cfg_cfg_coverage
        || atf_comp::_db.cmdline.mode == command_atf_comp_mode_memcheck;
    return instrumented ? 4.0 : 1.0;
}

// Check if test timeout has been exceeded
static bool TimedOutQ() {
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    double elapsed = algo::ElapsedSecs(atf_comp::_db.t0, algo::CurrSchedTime());
    return ct && elapsed >= ct->timeout * TimeoutScale();
}

// Read process stdout until UNTIL pattern is found (or EOF).
// Empty UNTIL means read until EOF (process exit).
// Emit output lines to log as "proc -> line".
// Use Fbuf Linebuf for line reassembly; poll with 1-second timeout
// for test timeout enforcement.  Without KILL_ON_TIMEOUT a timeout aborts
// the read (vrfy; the run loop catches it and fails the test).  With it --
// the cleanup drain -- the proc gets SIGTERM, then SIGKILL after a 5s
// grace, and reading continues to the EOF the kill produces: cleanup must
// end the test's procs, never the sweep.
tempstr atf_comp::ProcRead(atf_comp::FProc &proc, strptr until, bool kill_on_timeout DFLTVAL(false)) {
    tempstr result;
    bool matched = false;
    bool term_sent = false;
    bool kill_sent = false;
    algo::SchedTime term_time;
    while (!matched && !proc.in_eof) {
        algo::aryptr<char> msg = in_GetMsg(proc);
        if (msg.elems) {
            strptr line(msg.elems, msg.n_elems);
            result << line << "\n";
            LogOutput(proc, line);
            in_SkipMsg(proc);
            matched = ch_N(until) > 0 && algo::FindStr(result, until) != -1;
        } else if (proc.in_eof) {
            // Flush remaining partial line (no trailing newline before EOF)
            i32 remaining = in_N(proc);
            if (remaining > 0) {
                strptr tail((char*)(proc.in_elems + proc.in_start), remaining);
                result << tail << "\n";
                LogOutput(proc, tail);
                in_RemoveAll(proc);
            }
        } else {
            // No complete line yet — poll for more data or timeout
            struct pollfd pfd;
            pfd.fd = proc.in_iohook.fildes.value;
            pfd.events = POLLIN;
            int rc = poll(&pfd, 1, 1000);
            int err = errno;
            if (rc == 0) {
                if (TimedOutQ()) {
                    if (!kill_on_timeout) {
                        vrfy(false, tempstr() << "atf_comp.timeout"
                             << Keyval("proc", proc.proc)
                             << Keyval("until", until));
                    } else if (!term_sent) {
                        ProcKill(proc, SIGTERM);
                        term_time = algo::CurrSchedTime();
                        term_sent = true;
                    } else if (!kill_sent && algo::ElapsedSecs(term_time, algo::CurrSchedTime()) > 5.0) {
                        ProcKill(proc, SIGKILL);
                        kill_sent = true;
                    }
                }
            } else if (rc < 0 && err == EINTR) {
                // A signal interrupted the wait, and an interrupted wait is not
                // an end of output.  Reading it as one returns whatever the child
                // had written so far as its whole transcript, which the golden
                // comparison then reports as a mismatch -- the same verdict a
                // real behavior change produces, and one that appears only when
                // the machine is loaded enough to deliver a signal mid-poll.  So
                // the wait is retried and only end of file ends the read.
            } else if (rc < 0 || !(pfd.revents & (POLLIN | POLLHUP))) {
                vrfy(false, tempstr() << "atf_comp.pollerror"
                     << Keyval("proc", proc.proc)
                     << Keyval("until", until)
                     << Keyval("revents", pfd.revents)
                     << Keyval("comment", rc < 0 ? strerror(err) : "pipe reported an error"));
            }
        }
    }
    return result;
}

// Wait for the cluster under PROC to print READY_FOR_TEST, then take the
// variables that cluster wrote about itself into the replacement scope.
//
// A cluster picks its own ports and addresses as it starts, so a test script
// cannot name them ahead of time; the cluster writes what it chose to
// data/atfdb/var.ssim under its instance directory, and from there each becomes
// a $-substitution the script can use.  Which directory that is comes from the
// atfdb.testenv row marked vardir -- the same row that told the cluster where
// to put its instance in the first place, so the two cannot disagree.  A tree
// with no such row waits for readiness and takes no variables.
void atf_comp::WaitReadyForTest(atf_comp::FProc &proc) {
    tempstr out = ProcRead(proc, "READY_FOR_TEST");
    vrfy(algo::FindStr(out, "READY_FOR_TEST") != -1, tempstr() << "atf_comp.not_ready"
         << Keyval("proc", proc.proc)
         << Keyval("comment", "the cluster ended its output before announcing readiness"));
    ind_beg(atf_comp::_db_testenv_curs,testenv,atf_comp::_db) if (testenv.vardir) {
        char *initdir = getenv(Zeroterm(tempstr()<<testenv.testenv));
        if (initdir != NULL) {
            ind_beg(algo::FileLine_curs, line, algo::DirFileJoin(initdir, "data/atfdb/var.ssim")) {
                atfdb::Var var;
                if (atfdb::Var_ReadStrptrMaybe(var, line)) {
                    SetVar(var.var, var.value);
                }
            }ind_end;
        }
    }ind_end;
}

// Wait for process to exit, drain remaining stdout into log.
// Internal helper: does the work without enforcing an expected exit code.
static void ProcWaitImpl(atf_comp::FProc &proc) {
    if (proc.subproc.pid != 0) {
        ProcWriteEof(proc);
        ProcRead(proc, "", true /*kill_on_timeout*/);
        if (ValidQ(proc.in_iohook.fildes)) {
            close(proc.in_iohook.fildes.value);
            proc.in_iohook.fildes = algo::Fildes();
        }
        algo_lib::ProcWait(proc.subproc);
        proc.status = algo_lib::ProcExitCode(proc.subproc);
        if (proc.killed) {
            proc.status = -1;
        }
        Log(tempstr() << "# exit " << proc.proc << " code:" << proc.status);
        // check the valgrind memcheck logs for errors -- one log per traced
        // pid (the wrap runs valgrind with --trace-children and a %p log).
        // An error goes into the test log, the record CheckOutput diffs
        // against the golden, so the comptest carrying the memory error
        // fails and the run's exit code becomes nonzero. No golden can be
        // taught to expect the line: capture and memcheck are two values of
        // the same -mode argument, and only a capture run writes a golden.
        //
        // Valgrind writes ERROR SUMMARY when the client shuts down, so a
        // process killed by a signal -- the drain's SIGTERM and SIGKILL, a
        // test's own ProcKill -- leaves a log with no summary, and so does
        // a log that is empty or unreadable. Such a log carries no count,
        // and reading its absence as zero would make a process valgrind
        // never finished judging indistinguishable from one it cleared. It
        // is named on the harness's own stdout instead: under
        // --trace-children a killed descendant is routine, so the notice
        // stays out of the test log, where it would fail the comptest.
        if (ch_N(proc.memcheck_log) > 0) {
            ind_beg(algo::Dir_curs, entry, tempstr() << proc.memcheck_log << ".*.log") {
                i32 nerror = 0;
                bool summary_found = false;
                ind_beg(algo::FileLine_curs, line, entry.pathname) {
                    algo::StringIter it(line);
                    GetWordCharf(it);
                    it.Ws();
                    if (SkipStrptr(it, "ERROR SUMMARY:") && TryParseI32(it, nerror)) {
                        summary_found = true;
                        break;
                    }
                }ind_end;
                if (!summary_found) {
                    prlog("# memcheck log has no ERROR SUMMARY (see " << entry.pathname << ")");
                } else if (nerror > 0) {
                    Log(tempstr() << "# memcheck errors: " << nerror << " (see " << entry.pathname << ")");
                    SysCmd(tempstr() << "cat " << entry.pathname);
                }
            }ind_end;
        }
    }
}

// Wait for process to exit; verify it exited with EXPECTED_EXIT (default 0).
// -1 means the process was killed (via ProcKill).  Procs that are never
// passed to ProcWait are not checked — ProcWaitAll drains them silently.
void atf_comp::ProcWait(atf_comp::FProc &proc, int expected_exit DFLTVAL(0)) {
    ProcWaitImpl(proc);
    vrfy(proc.status == expected_exit, tempstr()
         <<"atf_comp: "<<proc.proc<<" exited with code "<<proc.status
         <<", expected "<<expected_exit);
}

// Get process exit code
int atf_comp::ProcStatus(atf_comp::FProc &proc) {
    return proc.status;
}

// Wait for all procs in reverse order; do not enforce exit codes here.
// Tests assert exit codes with explicit ProcWait; this is the cleanup sweep.
void atf_comp::ProcWaitAll() {
    for (int i = proc_N() - 1; i >= 0; i--) {
        ProcWaitImpl(proc_qFind(i));
    }
}

// Compose the deny-write scaffold: a bash line that prepares the test's
// tempdir with a read-only directory ro/ and runs TOOL against it, so the
// tool's attempt to rewrite a file under ro/ fails with EACCES and the test
// can pin the failure report. SEED creates the fixture file(s) under ro/
// before the directory is locked.
//
// The mode must be restored after the run: a directory left at 0555 cannot
// be removed by the next run's tempdir cleanup, so one interrupted run would
// turn every following run of the test red. The EXIT trap restores the mode
// on normal exit and, via the TERM trap, when the harness timeout kill sends
// SIGTERM; the SIGKILL that follows 5s later still leaves the directory
// locked -- unfixable, the shell gets no chance to run anything.
//
// chmod-based write denial assumes an unprivileged runner: under
// CAP_DAC_OVERRIDE (e.g. a root container) the write succeeds and every
// deny-write golden mismatches. CI uses shell executors today, which run
// unprivileged.
tempstr atf_comp::DenyWriteCmd(strptr seed, strptr tool) {
    tempstr script;
    script << "cd $tempdir && mkdir ro && " << seed
           << " && trap \"chmod 755 ro\" EXIT && trap \"exit 1\" TERM && chmod 555 ro && " << tool;
    return tempstr() << "bash -c '" << script << "'"; // ignore:hand_quote -- quotes wrap the bash -c script, target syntax
}
