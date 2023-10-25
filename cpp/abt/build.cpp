// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/build.cpp -- Build dag execution
//

#include "include/abt.h"

static tempstr PrintDoubleWithCommas(double d, int n) {
    tempstr ret;
    double_PrintWithCommas(d, ret, n);
    return ret;
}

static tempstr PrintDoublePercent(double d) {
    tempstr ret;
    double_PrintPercent(d, ret, 0);
    ret<<"%";
    return ret;
}

// -----------------------------------------------------------------------------

static void Build_Forkit(abt::FSyscmd *next_cmd) {
    if (next_cmd->redirect) {
        next_cmd->fstdout = tempstr() << "temp/abt-XXXXXX";
        next_cmd->fstderr = tempstr() << "temp/abt-XXXXXX";
        mode_t mask = umask(S_IXUSR | S_IRWXG | S_IWOTH | S_IXOTH);
        next_cmd->fd_stdout.fd = algo::Fildes(mkstemp((char*)Zeroterm(next_cmd->fstdout)));
        next_cmd->fd_stderr.fd = algo::Fildes(mkstemp((char*)Zeroterm(next_cmd->fstderr)));
        umask(mask);
    }
    int pid = fork();
    errno_vrfy(pid!=-1, "fork()");

    if (pid == 0) {
        algo_lib::DieWithParent();
        if (next_cmd->redirect) {
            next_cmd->command << " 2> "<<next_cmd->fstderr;
            next_cmd->command << " 1> "<<next_cmd->fstdout;
        }
        if (abt::_db.cmdline.printcmd) {
            // just print command that would have executed
            prlog(next_cmd->command);
            _exit(0);
        } else {
            verblog(next_cmd->command);
            int ret = execl("/bin/sh", "/bin/sh", "-c", Zeroterm(next_cmd->command), (char*)NULL);
            errno_vrfy(ret==0, "can't exec");
            _exit(1);
        }
    }
    // parent
    if (pid > 0) {
        next_cmd->pid = pid;
        abt::ind_running_InsertMaybe(*next_cmd);
    }
}

// -----------------------------------------------------------------------------

// Special handling of CL.EXE output:
// - single pathname on line by itself is skipped (CL echoes source filename to stdout)
// - windows paths are translated to cygwin paths
static void ClReveal(strptr text, algo::Fildes out) {
    int nprint=0;
    int nfilter=0;
    ind_beg(Line_curs,line,text) {
        if (nfilter==0 && (EndsWithQ(line,".cpp") || EndsWithQ(line,".c"))) {
            nfilter++;
            // filter echo of .cpp filename
        } else {
            // unixify path
            tempstr newline(line);
            if (StartsWithQ(Trimmed(line),"c:\\")) {
                Replace(newline,"c:\\","/cygdrive/c/");
                Replace(newline," : ",": ");
                Replace(newline,"\\","/");
            }
            if (nprint==0 && ch_N(line)>0) {
                WriteFile(out,(u8*)"\n",1);
            }
            newline << "\n";
            nprint++;
            WriteFile(out,(u8*)newline.ch_elems,newline.ch_n);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void RevealOutput(algo::Fildes a, algo::Fildes b, bool scan) {
    algo_lib::MmapFile mmap;
    if (MmapFile_LoadFd(mmap,a)) {
        if (abt::_db.cmdline.compiler == dev_Compiler_compiler_cl) {
            ClReveal(mmap.text,b);
        } else {
            WriteFile(b,(u8*)mmap.text.elems,mmap.text.n_elems);
        }
        mmap.fd.fd = algo::Fildes();// forget it -- avoid double close
    }
    if (scan) {
        ind_beg(algo::Line_curs,line,mmap.text) {
            abt::_db.report.n_err  += FindFrom(line, "error:", 0) >= 0;
            abt::_db.report.n_err  += FindFrom(line, "undefined reference", 0) >= 0;
            abt::_db.report.n_warn += FindFrom(line, "warning:", 0) >= 0;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static void Build_DumpOutput(abt::FSyscmd *compl_cmd) {
    if (ValidQ(compl_cmd->fd_stdout.fd)) {
        RevealOutput(compl_cmd->fd_stdout.fd, Fildes(1),false);
        fd_Cleanup(compl_cmd->fd_stdout);
    }
    if (ValidQ(compl_cmd->fd_stderr.fd)) {
        RevealOutput(compl_cmd->fd_stderr.fd, Fildes(2),true/*scan errors*/);
        fd_Cleanup(compl_cmd->fd_stderr);
        // increment error counter if command failed.
        // (prevents abt from reporting n_err:0 if a command silently failed)
        abt::_db.report.n_err += compl_cmd->status != 0;
    }
    // remove temp files
    if (ch_N(compl_cmd->fstdout)) {
        unlink(Zeroterm(compl_cmd->fstdout));
    }
    if (ch_N(compl_cmd->fstderr)) {
        unlink(Zeroterm(compl_cmd->fstderr));
    }
}

// -----------------------------------------------------------------------------

static void Build_ShowCompl(abt::FSyscmd *compl_cmd) {
    bool print_cmd   = compl_cmd->status != 0 && !compl_cmd->fail_prereq;
    // #AL# NOTE: do not use Filestat here as it may be out of date
    u64 stderr_len = ValidQ(compl_cmd->fd_stderr.fd) ? GetFileSize(compl_cmd->fstderr) : 0;

    print_cmd |= stderr_len > 0;            // reveal command name if any error output present
    print_cmd &= algo_lib::_db.last_signal == 0;// but not if exiting on signal
    print_cmd &= !compl_cmd->fail_prereq;// don't print message if the error is forced

    if (print_cmd) {            // print command name and status (if nonzero)
        tempstr msg;
        msg << "\n";
        msg << "abt.exec  "<<compl_cmd->command;
        if (compl_cmd->status != 0) {
            msg << ": status "<<compl_cmd->status;
        }
        prerr(msg);
    }
}

// -----------------------------------------------------------------------------

static void Build_Progress(int line_n, int built_n) {
    prlog_("\r"
           <<"abt.build"
           <<Keyval("line_n",PrintDoubleWithCommas(line_n,0))
           <<Keyval("built_n",PrintDoubleWithCommas(built_n,0))
           <<Keyval("job_n" , abt::ind_running_N())
           <<Keyval("complete",PrintDoublePercent(double(built_n)/u32_Max(line_n,1))));
}

// -----------------------------------------------------------------------------

static void Build_AdvanceDependent(abt::FSyscmd *compl_cmd) {
    ind_beg(abt::syscmd_c_next_curs, dep,*compl_cmd) {
        dep.p_child->fail_prereq |= compl_cmd->status != 0;
        dep.p_child->nprereq--;
        abt::bh_syscmd_Reheap(*dep.p_child);
    }ind_end;
}

// -----------------------------------------------------------------------------

static int LineN() {
    int line_n=0;
    ind_beg(abt::_db_syscmd_curs, syscmd,abt::_db) {
        line_n += syscmd.line_n;
    }ind_end;
    return line_n;
}

// -----------------------------------------------------------------------------

static void Build_RunDag() {
    // main loop
    int maxjobs = abt::_db.cmdline.maxjobs;
    u32 nfail = 0;
    // count # of lines attributable to all commands.
    u32 line_n = u32_Max(1,LineN());
    u32 last_progress=0;
    u32 built_n = 0;
    bool show_progress = isatty(1) && line_n > 10000;
    bool keepgoing = false;
    do {
        abt::FSyscmd *compl_cmd = NULL;
        abt::FSyscmd *next_cmd = abt::bh_syscmd_First();
        int compl_status = 0;

        // - either start a new process, or wait for one to terminate.
        // if nothing can be done, exit
        bool can_start = abt::ind_running_N() < maxjobs && next_cmd && next_cmd->nprereq==0;
        bool can_wait  = abt::ind_running_N() > 0;
        keepgoing      = can_start || can_wait;

        if (can_start) {
            abt::bh_syscmd_RemoveFirst();
            bool empty = !ch_N(next_cmd->command);
            next_cmd->fail_prereq |= algo_lib::_db.last_signal != 0; // detect signal, quietly finish
            next_cmd->fail_prereq |= nfail >= abt::_db.cmdline.maxerr;// if too many commands fail, fail quickly
            bool prefail = next_cmd->fail_prereq;
            bool forkit = !empty && !prefail;
            compl_cmd = empty || prefail ? next_cmd : NULL;
            compl_status = prefail ? -1 : 0;
            if (forkit) {
                Build_Forkit(next_cmd);
            }
        } else if (can_wait) {
            int compl_pid = waitpid(-1, &compl_status, 0);// we couldn't start a new process, so the wait is blocking.
            if (compl_pid > 0) {
                compl_cmd = abt::ind_running_Find(compl_pid);
                abt::ind_running_Remove(*compl_cmd);
            }
        }
        if (compl_cmd) {
            compl_cmd->status = compl_status;
            nfail += compl_status != 0;
            // do not increment lines-built counter if ctrl+c was pressed
            built_n += algo_lib::_db.last_signal != 0 ? 0 : compl_cmd->line_n;
            Build_ShowCompl(compl_cmd);
            Build_DumpOutput(compl_cmd);
            if (show_progress && u32_Update(last_progress,built_n*100/line_n)) {
                Build_Progress(line_n,built_n);
            }
            Build_AdvanceDependent(compl_cmd);
        }
    } while (keepgoing);

    if (show_progress) {
        prlog("");// newline
    }
    // check that all commands completed.
    vrfy(abt::ind_running_N() == 0, "something is still running");
}

// -----------------------------------------------------------------------------

void abt::Main_Build() {
    // run the DAG
    // first compute the execution heap
    ind_beg(abt::_db_syscmd_curs, syscmd,abt::_db) {
        if (abt::_db.cmdline.printcmd) {
            syscmd.redirect=false;
        }
        syscmd.nprereq = c_prior_N(syscmd);
        abt::bh_syscmd_Insert(syscmd);
    }ind_end;

    Build_RunDag();

    // check that we finished the script
    vrfy(abt::bh_syscmd_N() == 0, "dependency loop");

    // count failures
    int nfail = 0;
    ind_beg(abt::_db_syscmd_curs, syscmd,abt::_db) {
        nfail += syscmd.status != 0 && !syscmd.fail_prereq;
    }ind_end;
    if (nfail > 0) {
        abt::_db.report.n_err += nfail;
    }

    // complain if it's not our fault
    if (algo_lib::_db.last_signal) {
        prerr("abt.signal"
              <<Keyval("signal", algo_lib::_db.last_signal)
              <<Keyval("comment",strsignal(algo_lib::_db.last_signal)));
    }
}
