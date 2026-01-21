// Copyright (C) 2024,2026 AlgoRND
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
// Target: atf_exp (exe) -- ATF expect
// Exceptions: yes
// Source: cpp/atf_exp/atf_exp.cpp
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/algo.h"
#include "include/atf_exp.h"

// Add variable NAME=VALUE to global replscope
// And append a line to $tempdir/vars
void atf_exp::SetVar(algo::strptr name, algo::strptr value) {
    Set(_db.R,tempstr()<<"$"<<name,value);
    tempstr str;
    str << name <<"="<<strptr_ToBash(value) << eol;
    str << "export "<<name << eol;
    WriteFile(_db.vars_file.fd, (u8*)str.ch_elems, str.ch_n);
}

// -----------------------------------------------------------------------------

// remove first N elements
static void ary_Shift(algo::ByteAry &parent, int n) {
    int rest = ary_N(parent) - n;
    if (rest > 0) {
        memmove(parent.ary_elems, parent.ary_elems + n, rest);
        parent.ary_n = rest;
    } else {
        ary_RemoveAll(parent);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                              STDIN MESSAGES
//
////////////////////////////////////////////////////////////////////////////////

// remove directory
void atf_exp::In_RemDirRecurseMsg(ams::RemDirRecurseMsg &msg) {
    algo::RemDirRecurse(Subst(_db.R,pathname_Getary(msg)),msg.remove_topmost);
}

//------------------------------------------------------------------------------

// start new subprocess
void atf_exp::In_ProcStartMsg(ams::ProcStartMsg &msg) {
    if (ind_proc_Find(msg.proc)) {
        prerr("atf_exp.badproc"
              <<Keyval("proc",msg.proc)
              <<Keyval("msg",msg)
              <<Keyval("comment","proc already defined"));
        algo_lib::_db.exit_code=1;
    } else {
        FProc &proc = ind_proc_GetOrCreate(msg.proc);
        int fd[2][2];
        if (msg.pty) {
            int fdc;
            int fdp;
            errno_vrfy_((fdp = posix_openpt(O_RDWR|O_NOCTTY|O_CLOEXEC))>=0);
            errno_vrfy_(grantpt(fdp)==0);
            errno_vrfy_(unlockpt(fdp)==0);
            errno_vrfy_((fdc = open(ptsname(fdp),O_RDWR|O_CLOEXEC|O_NOCTTY))>=0);
            fd[0][0] = fd[1][1] = fdc;
            fd[0][1] = fd[1][0] = fdp;
        } else {
            errno_vrfy_(pipe2(fd[0],O_CLOEXEC)==0);
            errno_vrfy_(pipe2(fd[1],O_CLOEXEC)==0);
        }
        algo::Fildes in(fd[1][0]);
        algo::Fildes out(fd[0][1]);
        SetBlockingMode(in,false);
        SetBlockingMode(out,false);
        in_BeginRead(proc,in);
        out_BeginWrite(proc,out,false);

        proc.bash.fstdin = tempstr() << "<&" << fd[0][0];
        proc.bash.fstdout = tempstr() << ">&" << fd[1][1];
        proc.bash.fstderr = proc.bash.fstdout;
        try {// failed substitution -> failed test
            proc.bash.cmd.c = Subst(_db.R,cmd_Getary(msg));
        } catch (algo_lib::ErrorX &x) {
            proc.bash.cmd.c = "false";
        }
        bash_Start(proc.bash);
        if (proc.bash.pid > 0) {
            proc.pid = proc.bash.pid;
            ind_proc_pid_InsertMaybe(proc);
        }
        close(fd[0][0]);
        close(fd[1][1]);
        ProcStatus(proc);
    }
}

//------------------------------------------------------------------------------

// Switch focus to process, start reading process output
// and waiting `until` string.
// Empty string means until process exit.
// Launch timer.
void atf_exp::In_ProcReadMsg(ams::ProcReadMsg &msg) {
    if ((_db.c_proc_read = ind_proc_Find(msg.proc))) {
        _db.proc_read_until = until_Getary(msg);
        _db.c_proc_read->wait = !ch_N(_db.proc_read_until);
        // suspend stdin
        while (FFdin *fdin = cd_fdin_read_RemoveFirst()) {
            cd_fdin_suspend_Insert(*fdin);
        }
        // launch timer
        hook_Set0(_db.timeout,Timeout);
        ThScheduleIn(_db.timeout, algo::ToSchedTime(_db.cmdline.timeout));
        // read shelved
        ProcRead(*_db.c_proc_read);
    } else {
        prerr("atf_exp.badproc"
              <<Keyval("proc",msg.proc)
              <<Keyval("msg",msg)
              <<Keyval("comment","undefined proc"));
        algo_lib::_db.exit_code=1;
    }
}

//------------------------------------------------------------------------------

// send message to process
void atf_exp::In_ProcMsg(ams::ProcMsg &msg) {
    if (FProc *proc = ind_proc_Find(msg.proc)) {
        cstring line = tempstr() << payload_Getary(msg) << eol;
        out_WriteAll(*proc,(u8*)line.ch_elems,line.ch_n);
    } else {
        prerr("atf_exp.badproc"
              <<Keyval("proc",msg.proc)
              <<Keyval("msg",msg)
              <<Keyval("comment","undefined proc"));
        algo_lib::_db.exit_code=1;
    }
}

//------------------------------------------------------------------------------

// Close process stdin from write side, delay close in case of non-empty buffer
void atf_exp::In_ProcEofMsg(ams::ProcEofMsg &msg) {
    if (FProc *proc = ind_proc_Find(msg.proc)) {
        if (!out_N(*proc)) {
            algo::Refurbish(proc->out_iohook);
        } else {
            proc->out_delay_close = true;
        }
    } else {
        prerr("atf_exp.badproc"
              <<Keyval("proc",msg.proc)
              <<Keyval("msg",msg)
              <<Keyval("comment","undefined proc"));
        algo_lib::_db.exit_code=1;
    }
}

//------------------------------------------------------------------------------

// kill process
void atf_exp::In_ProcKillMsg(ams::ProcKillMsg &msg) {
    if (FProc *proc = ind_proc_Find(msg.proc)) {
        if (proc->pid > 0) {
            kill(proc->pid,msg.signal);
        }
    } else {
        prerr("atf_exp.badproc"
              <<Keyval("proc",msg.proc)
              <<Keyval("msg",msg)
              <<Keyval("comment","undefined proc"));
        algo_lib::_db.exit_code=1;
    }
}

//------------------------------------------------------------------------------

// Terminate
void atf_exp::In_TerminateMsg(ams::TerminateMsg &) {
    algo_lib::ReqExitMainLoop();
}

//------------------------------------------------------------------------------

// read stdin
void atf_exp::cd_fdin_read_Step() {
    if (FFdin *fdin = cd_fdin_read_RotateFirst()) {
        if (!_db.c_proc_read) {
            strptr msgstr = in_GetMsg(*fdin);
            if (ch_N(msgstr)) {
                algo::ByteAry buf;
                if (ams::MsgHeaderMsgs_ReadStrptr(msgstr,buf)) {
                    ams::MsgHeader &msg = *(ams::MsgHeader*)buf.ary_elems;
                    cstring out;
                    ams::MsgHeaderMsgs_Print(out,msg,UINT_MAX);
                    prlog(out);
                    InDispatch(msg);
                }
                in_SkipMsg(*fdin);
            }
        } else {
            cd_fdin_read_Remove(*fdin);
        }
    }
}

//------------------------------------------------------------------------------

// stdin eof
void atf_exp::cd_fdin_eof_Step() {
    if (FFdin *fdin = cd_fdin_eof_RotateFirst()) {
        fdin_Delete(*fdin);
    }
    //algo_lib::ReqExitMainLoop();
}

////////////////////////////////////////////////////////////////////////////////
//
//                         SUBPROCESS COMMUNICATION
//
////////////////////////////////////////////////////////////////////////////////

// Read shelved process output, put to stdout.
// Search `until` string. When found, stop reading process output,
// keep the rest of data in the buffer and switch back to stdin.
// Check process exit, issue process status message, delete process.
void atf_exp::ProcRead(atf_exp::FProc &proc) {
    if (&proc == _db.c_proc_read) {
        bool del = !proc.pid; // not currently running
        del = del && proc.wait; // process has been waited with empty 'until' clause
        del = del && proc.in_eof && !proc.in_msgvalid; // eof
        del = del && (proc.in_err.value || !out_N(proc)); // no data to send or subprocess stdin closed
        if (del) {
            ProcStatus(proc);
        }
        // drain messages to stdout, stop when found
        bool found(false);
        u8 *first_pmsg = (u8*)proc.output.ary_elems;
        u8 *next_pmsg = first_pmsg;
        ind_beg(ams::MsgHeader_curs,pmsg,proc.output) if (!found) {
            next_pmsg = (u8*)pmsg + pmsg->length;
            cstring out;
            ams::MsgHeaderMsgs_Print(out,*pmsg,UINT_MAX);
            prlog(out);
            found = ch_N(_db.proc_read_until) && algo::FindStr(out,_db.proc_read_until) >= 0;
        }ind_end;
        ary_Shift(proc.output,next_pmsg-first_pmsg);
        found |= !ch_N(_db.proc_read_until) && del; // handle empty until case
        // if found - resume stdin, remove timer
        if (found) {
            _db.c_proc_read = NULL;
            while (FFdin *fdin = cd_fdin_suspend_RemoveFirst()) {
                cd_fdin_read_Insert(*fdin);
            }
            bh_timehook_Remove(_db.timeout);
        } else {
            // since this function is non-blocking,
            // not found doesn't mean timed out...
            // see Timeout function for that
        }
        // delete process
        if (del) {
            proc_Delete(proc);
        }
    }
}

//------------------------------------------------------------------------------

// timeout hook
void atf_exp::Timeout() {
    if (_db.c_proc_read) {
        FProc &proc = *_db.c_proc_read;
        prlog("atf_exp.timeout"
              << Keyval("proc",proc.proc)
              << Keyval("pid", proc.pid)
              << Keyval("wait",proc.wait)
              << Keyval("in_eof",proc.in_eof)
              << Keyval("in_msgvalid",proc.in_msgvalid)
              << Keyval("out_n",out_N(proc))
              << Keyval("output_n",ary_N(proc.output))
              << Keyval("until",_db.proc_read_until));
        if (_db.cmdline.dbgshell){
            // invoke live bash with current env
            TimeoutBash();
            // then continue with exit
            prlog("atf_exp.timeout: continue to MainLoop exit");
        }
        algo_lib::ReqExitMainLoop();
        algo_lib::_db.exit_code=1;
    }
}

//------------------------------------------------------------------------------

// issue process status message
void atf_exp::ProcStatus(atf_exp::FProc &proc) {
    cstring status;
    if (proc.pid) {
        status = "Running";
    } else {
        if (proc.bash.pid == 0) {
            status = "NotStarted";
        } else if (proc.bash.pid < 0) {
            status << "StartFailed: " << strerror(proc.bash.status);
        } else if (WIFEXITED(proc.bash.status)) {
            status << "Exited: " << WEXITSTATUS(proc.bash.status);
        } else if (WIFSIGNALED(proc.bash.status)) {
            status << strsignal(WTERMSIG(proc.bash.status));
            if (WCOREDUMP(proc.bash.status)) {
                status << " (core dumped)";
            }
        }
    }
    ProcStatusMsg_FmtAppend(proc.output,proc.proc,status);
}

//------------------------------------------------------------------------------

// process read hook:
// - capture process variables in form of `$name:value`,
// which could be utilized when launching next process using `$name` substitution
// in ams.ProcStartMsg  cmd:"... $name ..."
// - store process output as ams.ProcMsg in process output buffer (shelf)
void atf_exp::cd_proc_read_Step() {
    if (FProc *proc = cd_proc_read_RotateFirst()) {
        strptr line = in_GetMsg(*proc);
        if (ch_N(line)) {
            if (StartsWithQ(line,"atf.var")) {// filter these out
                ind_beg(algo::Attr_curs,attr,RestFrom(line,7)) {
                    SetVar(attr.name,attr.value);
                }ind_end;
            }
            ProcMsg_FmtAppend(proc->output,proc->proc,line);
        }
        in_SkipMsg(*proc);
        ProcRead(*proc); // handles eof
    }
}

//------------------------------------------------------------------------------

// write to subprocess
void atf_exp::cd_proc_outflow_Step() {
    if (FProc *proc = cd_proc_outflow_RotateFirst()) {
        out_Outflow(*proc);
        if (proc->out_delay_close && !out_N(*proc)) {
            // close proc stdin from sending side
            algo::Refurbish(proc->out_iohook);
        }
    }
}

//------------------------------------------------------------------------------

// wait subprocess
void atf_exp::ind_proc_pid_Step() {
    int status = 0;
    int pid = waitpid(-1,&status,WNOHANG);
    if (pid > 0) {
        // child exited or killed
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            if (FProc *proc = ind_proc_pid_Find(pid)) {
                ind_proc_pid_Remove(*proc);
                proc->pid = 0;
                ProcRead(*proc);// read remainder of output from the process
            } else {
                prerr("Unknown PID: " << pid);
            }
        }
    } else {
        // nothing interesting
        ind_proc_pid_SetDelay(algo::ToSchedTime(60.0));
    }
}

//------------------------------------------------------------------------------

// Handle SIGCHLD
void atf_exp::SigchldHandler(int sig) {
    (void)sig;
    atf_exp::ind_proc_pid_SetDelay(algo::SchedTime());
}

//------------------------------------------------------------------------------

// Setup signal handlers
void atf_exp::SetupSignalHandlers() {
    // SIGCHLD
    struct sigaction sigact;
    sigact.sa_handler = SigchldHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction(SIGCHLD, &sigact, 0);
    // SIGPIPE
    signal(SIGPIPE,SIG_IGN);
}

// -----------------------------------------------------------------------------

// invoke bash with interactive input
void atf_exp::TimeoutBash() {
    cstring cmd;
    Ins(&_db.R, cmd,"echo Interactive bash entered on timeout");
    Ins(&_db.R, cmd,"source $tempdir/vars");
    Ins(&_db.R, cmd,"exec </dev/tty");
    Ins(&_db.R, cmd,"bash -i");
    verblog("atf_exp.timeout_bash.cmd\n" << cmd);
    auto rc=SysCmd(cmd);
    prlog("Exited from SysCmd"<<Keyval("rc", rc));
}

//------------------------------------------------------------------------------

// main routine
void atf_exp::Main() {
    SetupSignalHandlers();
    // stdin
    FFdin &fdin = fdin_Alloc();
    algo::Fildes fd(0);
    algo::SetBlockingMode(fd,false);
    in_BeginRead(fdin,fd);
    if (_db.cmdline.tempdir == "") {//default value
        _db.cmdline.tempdir << "temp/atf_comp/" << _db.cmdline.comptest;
    }
    _db.R.strict=2;
    atf_exp::_db.vars_file.fd=OpenWrite(algo::DirFileJoin(_db.cmdline.tempdir,"vars"),algo_FileFlags_append);
    SetVar("comptest",_db.cmdline.comptest); // atf_exp.Cli
    SetVar("tempdir",_db.cmdline.tempdir); // temp/atf_comp/atf_exp.Cli
    SetVar("bindir",_db.cmdline.bindir); // build/release
    Set(_db.R,"$$","$",false);
    // dump vars
    verblog("atf_exp.vars"
            <<Keyval("comptest",_db.cmdline.comptest)
            <<Keyval("tempdir",_db.cmdline.tempdir)
            <<Keyval("bindir",_db.cmdline.bindir));

    MainLoop();
}
