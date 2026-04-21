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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
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

// Append line to test log; with -v, also print to stdout
static void Log(strptr line) {
    atf_comp::_db.log << line << "\n";
    verblog(line);
}

// Scan text for atf.var lines, set variables in Replscope
static void ScanVars(strptr text) {
    ind_beg(algo::Line_curs, line, text) {
        if (StartsWithQ(line, "atf.var")) {
            ind_beg(algo::Attr_curs, attr, RestFrom(line, 7)) {
                atf_comp::SetVar(attr.name, attr.value);
            }ind_end;
        }
    }ind_end;
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
// Skip atf.var lines: these are consumed by ScanVars for variable substitution
// but excluded from the log because their order is non-deterministic when
// multiple nodes publish ports concurrently.
static void LogOutput(atf_comp::FProc &proc, strptr text) {
    ind_beg(algo::Line_curs, line, text) {
        if (ch_N(line) > 0
            && !StartsWithQ(line, "profiling:")
            && !StartsWithQ(line, "atf.var")
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

// Spawn subprocess with $-substitution, return reference.
atf_comp::FProc &atf_comp::ProcStart(strptr cmd) {
    tempstr name = DeriveProcName(cmd);
    Log(tempstr() << "# start " << name << " cmd:" << strptr_ToSsim(Trimmed(cmd)));
    tempstr cmd_eff = Subst(cmd);
    cmd_eff = tempstr() << algo::TrimmedRight(cmd_eff);
    atf_comp::FProc &proc = proc_Alloc();
    // wrap command based on mode
    u8 mode = atf_comp::_db.cmdline.mode;
    tempstr bindir;
    bindir << "build/" << atf_comp::_db.cmdline.cfg;
    if ((mode == command_atf_comp_mode_covcheck || mode == command_atf_comp_mode_covcapture)
        && StartsWithQ(cmd_eff, bindir)) {
        tempstr covdir;
        covdir << "temp/atf_comp/cov.d";
        atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
        tempstr logfile;
        logfile << covdir << "/" << (ct ? strptr(ct->comptest) : strptr("unknown")) << ".atf_cov.log";
        command::atf_cov atf_cov;
        atf_cov.covdir = covdir;
        atf_cov.incremental = true;
        atf_cov.logfile = logfile;
        atf_cov.runcmd = cmd_eff;
        cmd_eff = tempstr() << command::atf_cov_ToCmdline(atf_cov);
    } else if (mode == command_atf_comp_mode_memcheck && proc_N() == 1) { // first process only; rest are auxiliary
        atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
        tempstr logpath;
        logpath << "temp/atf_comp/" << (ct ? strptr(ct->comptest) : strptr("unknown")) << "/" << name << ".memcheck.log";
        proc.memcheck_log = logpath;
        algo::DeleteFile(logpath);
        cmd_eff = tempstr() << "valgrind --tool=memcheck --log-file=" << logpath << " " << cmd_eff;
    } else if (mode == command_atf_comp_mode_valgrind && proc_N() == 1) {
        cmd_eff = tempstr() << "valgrind " << cmd_eff;
    }
    proc.proc = name;
    proc.status = -1;
    int stdin_pipe[2];
    int stdout_pipe[2];
    vrfy_(pipe2(stdin_pipe, O_CLOEXEC) == 0);
    vrfy_(pipe2(stdout_pipe, O_CLOEXEC) == 0);
    proc.bash.cmd.c = cmd_eff;
    proc.bash.fstdin << "<&" << stdin_pipe[0];
    proc.bash.fstdout << ">&" << stdout_pipe[1];
    proc.bash.fstderr << ">&" << stdout_pipe[1];
    bash_Start(proc.bash);
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    proc.stdin_fd.value = stdin_pipe[1];
    algo::Fildes read_fd(stdout_pipe[0]);
    algo::SetBlockingMode(read_fd, false);
    in_BeginRead(proc, read_fd);
    proc_XrefMaybe(proc);
    return proc;
}

// Write a line to process stdin. If proc.subst, apply $-substitution.
void atf_comp::ProcWrite(atf_comp::FProc &proc, strptr msg) {
    Log(tempstr() << proc.proc << " <- " << msg);
    tempstr line;
    line << Trimmed(Subst(msg)) << "\n";
    errno_vrfy_(write(proc.stdin_fd.value, line.ch_elems, line.ch_n) == (ssize_t)line.ch_n);
}

// Send signal to process
void atf_comp::ProcKill(atf_comp::FProc &proc, int signal) {
    Log(tempstr() << "# kill " << proc.proc << " signal:" << signal);
    if (proc.bash.pid > 0) {
        kill(proc.bash.pid, signal);
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

// Check if test timeout has been exceeded
static bool TimedOutQ() {
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    double elapsed = algo::ElapsedSecs(atf_comp::_db.t0, algo::CurrSchedTime());
    return ct && elapsed >= ct->timeout;
}

// Read process stdout until UNTIL pattern is found (or EOF).
// Empty UNTIL means read until EOF (process exit).
// Detect atf.var lines and set variables.
// Emit output lines to log as "proc -> line".
// Use Fbuf Linebuf for line reassembly; poll with 1-second timeout
// for test timeout enforcement.
tempstr atf_comp::ProcRead(atf_comp::FProc &proc, strptr until) {
    tempstr result;
    bool matched = false;
    while (!matched && !proc.in_eof) {
        algo::aryptr<char> msg = in_GetMsg(proc);
        if (msg.elems) {
            strptr line(msg.elems, msg.n_elems);
            result << line << "\n";
            ScanVars(line);
            LogOutput(proc, line);
            in_SkipMsg(proc);
            matched = ch_N(until) > 0 && algo::FindStr(result, until) != -1;
        } else if (proc.in_eof) {
            // Flush remaining partial line (no trailing newline before EOF)
            i32 remaining = in_N(proc);
            if (remaining > 0) {
                strptr tail((char*)(proc.in_elems + proc.in_start), remaining);
                result << tail << "\n";
                ScanVars(tail);
                LogOutput(proc, tail);
                in_RemoveAll(proc);
            }
        } else {
            // No complete line yet — poll for more data or timeout
            struct pollfd pfd;
            pfd.fd = proc.in_iohook.fildes.value;
            pfd.events = POLLIN;
            int rc = poll(&pfd, 1, 1000);
            if (rc == 0) {
                if (TimedOutQ()) {
                    vrfy(false, tempstr() << "atf_comp.timeout"
                         << Keyval("proc", proc.proc)
                         << Keyval("until", until));
                }
            } else if (rc <= 0 || !(pfd.revents & (POLLIN | POLLHUP))) {
                break;
            }
        }
    }
    return result;
}

// Wait for process to exit, drain remaining stdout into log.
void atf_comp::ProcWait(atf_comp::FProc &proc) {
    if (proc.bash.pid != 0) {
        ProcWriteEof(proc);
        ProcRead(proc, "");
        if (ValidQ(proc.in_iohook.fildes)) {
            close(proc.in_iohook.fildes.value);
            proc.in_iohook.fildes = algo::Fildes();
        }
        bash_Wait(proc.bash);
        proc.status = WIFEXITED(proc.bash.status) ? WEXITSTATUS(proc.bash.status) : -1;
        if (proc.killed) {
            proc.status = -1;
        }
        Log(tempstr() << "# exit " << proc.proc << " code:" << proc.status);
        // check valgrind memcheck log for errors
        if (ch_N(proc.memcheck_log) > 0) {
            i32 num_errors = 0;
            ind_beg(algo::FileLine_curs, line, proc.memcheck_log) {
                algo::StringIter it(line);
                GetWordCharf(it);
                it.Ws();
                if (SkipStrptr(it, "ERROR SUMMARY:") && TryParseI32(it, num_errors)) {
                    break;
                }
            }ind_end;
            if (num_errors > 0) {
                prlog("# memcheck errors: " << num_errors << " (see " << proc.memcheck_log << ")");
                SysCmd(tempstr() << "cat " << proc.memcheck_log);
            }
        }
    }
}

// Get process exit code
int atf_comp::ProcStatus(atf_comp::FProc &proc) {
    return proc.status;
}

// Wait for all procs in reverse order
void atf_comp::ProcWaitAll() {
    for (int i = proc_N() - 1; i >= 0; i--) {
        ProcWait(proc_qFind(i));
    }
}
