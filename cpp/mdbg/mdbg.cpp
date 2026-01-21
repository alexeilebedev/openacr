// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2024 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Target: mdbg (exe) -- Gdb front-end
// Exceptions: NO
// Source: cpp/mdbg/mdbg.cpp
//

#include "include/mdbg.h"
#include "include/gen/mdbg_gen.h"
#include "include/gen/mdbg_gen.inl.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

// -----------------------------------------------------------------------------

// Set up breakpoints
// Breakpoint list is comma-separated
// See
// https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_28.html
// Special syntax FUNCTION#N breaks on Nth crossing of the function
static void GenBreakpoints() {
    ind_beg(command::mdbg_b_curs,b,mdbg::_db.cmdline) {
        cstring &out = mdbg::_db.gdbscript;
        ind_beg(algo::Sep_curs,tok,b,',') {
            algo::strptr where = Pathcomp(tok,"#LL");
            algo::strptr count = Pathcomp(tok,"#LR");
            mdbg::_db.break_main=where=="main";
            out << "break " << where << eol;
            if (ch_N(count)) {
                out << "ignore "<<mdbg::_db.bnum<<" "<<count << eol;
            }
            mdbg::_db.bnum++;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Find pid of existing target process in case we want to debug it
static u32 Find_Pid(algo_lib::Replscope &R) {
    u32 pid = mdbg::_db.cmdline.pid;
    if (pid==0) {
        tempstr pid_str;
        pid_str << Trimmed(SysEval(Subst(R, "pidof $tgtfname"), FailokQ(true), 1024));
        algo::StringIter iter(pid_str);
        if (!TryParseU32(iter, pid)) { // if pid is a valid value
            prlog(Subst(R, "Could not find the pid of the process:$tgtfname to attach!"));
            prlog("Make sure process is running and use sudo option is enabled...");
            pid = 0;
        }
    }
    return pid;
}

// -----------------------------------------------------------------------------

// Commands to enable debugging through emacs
// Static Emacs functions
static void AddEmacsFunctions(algo_lib::Replscope &R) {
    Ins(&R, mdbg::_db.script, "(defun gdb-send-input (string) (interactive)");
    Ins(&R, mdbg::_db.script, "  (let ((old-buffer (current-buffer)))");
    Ins(&R, mdbg::_db.script, "    (switch-to-buffer \"*gud*\")");
    Ins(&R, mdbg::_db.script, "    (insert string)");
    Ins(&R, mdbg::_db.script, "    (comint-send-input)");
    Ins(&R, mdbg::_db.script, "    (switch-to-buffer old-buffer)");
    Ins(&R, mdbg::_db.script, "    ))");

    Ins(&R, mdbg::_db.script, "(defun gdb-send-input-ui (string) (interactive)");
    Ins(&R, mdbg::_db.script, "  (gdb-restore-windows-full)");
    Ins(&R, mdbg::_db.script, "  (insert string)");
    Ins(&R, mdbg::_db.script, "  (comint-send-input)");
    Ins(&R, mdbg::_db.script, "  )");

    Ins(&R, mdbg::_db.script, "(defun gdb-remote-output (process output)");
    Ins(&R, mdbg::_db.script, "  (let ((old-buffer (current-buffer)))");
    Ins(&R, mdbg::_db.script, "    (switch-to-buffer \"*gud*\")");
    Ins(&R, mdbg::_db.script, "    (insert output)");
    Ins(&R, mdbg::_db.script, "    (comint-set-process-mark)");
    Ins(&R, mdbg::_db.script, "    (switch-to-buffer old-buffer)");
    Ins(&R, mdbg::_db.script, "    ))");

    Ins(&R, mdbg::_db.script, "(defun mdbg-recompile   () (interactive)");
    Ins(&R, mdbg::_db.script, "  (gdb-send-input-ui \"!abt $buildtarget -cfg $cfg\")");
    Ins(&R, mdbg::_db.script, "  (gdb-send-input-ui \"run\"))");

    Ins(&R, mdbg::_db.script, "(defun mdbg-step   () (interactive) (gdb-send-input-ui \"step\"))");
    Ins(&R, mdbg::_db.script, "(defun mdbg-next   () (interactive) (gdb-send-input-ui \"next\"))");
    Ins(&R, mdbg::_db.script, "(defun mdbg-finish () (interactive) (gdb-send-input-ui \"finish\"))");
    Ins(&R, mdbg::_db.script, "(defun mdbg-until  () (interactive) (gdb-send-input-ui \"until\"))");
    Ins(&R, mdbg::_db.script, "(defun mdbg-cont   () (interactive) (gdb-send-input-ui \"cont\"))");

    // this doesn't work when focus is in the gdb window
    if (true) {
        Ins(&R, mdbg::_db.script, "(defun mdbg-break  () (interactive) (gdb-send-input (concat \"break "
            "\" (buffer-name) \":\" (number-to-string (line-number-at-pos)))))");
        Ins(&R, mdbg::_db.script, "(defun mdbg-clear  () (interactive) (gdb-send-input (concat \"clear "
            "\" (buffer-name) \":\" (number-to-string (line-number-at-pos)))))");
    } else {
        Ins(&R, mdbg::_db.script, "(defun mdbg-break  () (interactive) (gud-break))");
        Ins(&R, mdbg::_db.script, "(defun mdbg-clear  () (interactive) (gdb-remove))");
    }
    Ins(&R, mdbg::_db.script, "(defun mdbg-up     () (interactive) (gdb-send-input-ui \"up\"))");
    Ins(&R, mdbg::_db.script, "(defun mdbg-down   () (interactive) (gdb-send-input-ui \"down\"))");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f7>\")      'mdbg-recompile)   ; rebuild program "
        "being debugged");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f11>\")     'mdbg-step)   ;step into");
    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"S-<f11>\")   'mdbg-finish) ;step out of function");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f10>\")     'mdbg-next)   ;step over");
    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"C-<f10>\")   'mdbg-until)  ;go until current line");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f5>\")      'mdbg-cont) ;run");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f9>\")      'mdbg-break) ;set breakpoint");
    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"C-<f9>\")    'mdbg-clear) ;clear breakpoint");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"M-<down>\")  'mdbg-down)");
    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"M-<up>\")  'mdbg-up)");

    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f6>\")      'other-window)");
    Ins(&R, mdbg::_db.script, "(global-set-key (kbd \"<f12>\")     'gdb-restore-windows-full)");

    Ins(&R, mdbg::_db.script, "(setq gdb-debuginfod-enable-setting \"set debuginfod enabled off\")");
}

// -----------------------------------------------------------------------------

// Display additional information in the Emacs window
static void DisplayEmacsNotes() {
    prlog("mdbg.note  Debug script saved to temp/mdbg.el");
    prlog("mdbg.note  Invoking gdb under emacs. Make sure to link ~/.emacs -> conf/emacs.el, ~/elisp -> conf/elisp");
    prlog("mdbg.note  Standard shortcuts:");
    prlog("mdbg.note               F7  recompile and restart executable");
    prlog("mdbg.note              F11  step into");
    prlog("mdbg.note        Shift-F11  step out");
    prlog("mdbg.note              F10  step over");
    prlog("mdbg.note         Ctrl-F10  run to cursor");
    prlog("mdbg.note              F12  restore windows");
    prlog("mdbg.note              F9   set breakpoint");
    prlog("mdbg.note         Ctrl-F9   clear breakpoint");
    prlog("mdbg.note              F6   other window");
    prlog("mdbg.note    Alt-Up Arrow   Go up the call stack");
    prlog("mdbg.note  Alt-Down Arrow   Go down the call stack");
}

// -----------------------------------------------------------------------------

static void Main_Emacs(algo_lib::Replscope &R) {
    // another helper
    Ins(&R, mdbg::_db.script, "; ");
    Ins(&R, mdbg::_db.script, "; This file was generated by mdbg");
    Ins(&R, mdbg::_db.script, "(defun gdb-restore-windows-full () (interactive)");
    Ins(&R, mdbg::_db.script, "  (gdb-restore-windows)");

    if (mdbg::_db.cmdline.disas) {
        Ins(&R, mdbg::_db.script, "  (split-window-horizontally)"); // make another window to the right
        Ins(&R, mdbg::_db.script, "  (switch-to-buffer \"*disassembly of $target*\")");
        Ins(&R, mdbg::_db.script, "  (other-window -1)"); // back in source
    }
    Ins(&R, mdbg::_db.script, "  )");

    AddEmacsFunctions(R);

    Ins(&R, mdbg::_db.script, "");
    if (mdbg::_db.cmdline.manywin) {
        Ins(&R, mdbg::_db.script, "(setq gdb-many-windows t)");
    }
#if __FreeBSD__ > 0
    // Emacs current directory is truly unpredictable
    Ins(&R, mdbg::_db.script, "(gdb \"gdb -i=mi -x temp/mdbg.gdb\")");
#else
    Ins(&R, mdbg::_db.script, "(gdb \"gdb -i=mi --init-command=temp/mdbg.gdb\")");
#endif

    StringToFile(mdbg::_db.script, "temp/mdbg.el");

    DisplayEmacsNotes();

    if (mdbg::_db.cmdline.dry_run) {
        prlog("# Emacs script is shown below");
        prlog(mdbg::_db.script);
    } else {
        SysCmd("emacs -nw --load temp/mdbg.el", FailokQ(true), DryrunQ(false));
    }
}

// -----------------------------------------------------------------------------
// Commands to enable debugging through gdb command line
//
static void Main_NoEmacs(algo_lib::Replscope &R) {
    if (mdbg::_db.cmdline.tui) {
        Set(R,"$gdbopt","$gdbopt -tui");
    }
    Ins(&R, mdbg::_db.script, "# This file was generated by mdbg");
    Ins(&R, mdbg::_db.script, "gdb $gdbopt --init-command=temp/mdbg.gdb");
    StringToFile(mdbg::_db.script, "temp/mdbg.sh");
    if (mdbg::_db.cmdline.dry_run) {
        prlog(mdbg::_db.script);
    } else {
        SysCmd(". temp/mdbg.sh", FailokQ(true), DryrunQ(false));
    }
}

// -----------------------------------------------------------------------------
// Generate gdb script
//
void mdbg::Main_Gdb(algo_lib::Replscope &R) {
    Ins(&R, mdbg::_db.gdbscript, "# This file was generated by mdbg");

    //Ins(&R, mdbg::_db.gdbscript, "set trace-commands on");
    //Ins(&R, mdbg::_db.gdbscript, "set logging on"); - really unnecessary
    // on FreeBSD, emacs spawning gdb ends up in temp/ directory,
    // from where it can't find the executable
    if (!mdbg::_db.cmdline.dry_run) {
        // in dry run, GetCurDir() is a full path that differs in CI
        Ins(&R, mdbg::_db.gdbscript, tempstr()<<"cd "<< algo::GetCurDir());
    }
    Ins(&R, mdbg::_db.gdbscript, "set history filename temp/gdb.history");
    Ins(&R, mdbg::_db.gdbscript, "set history save on");
    // disable debug info daemon, we don't use it
    Ins(&R, mdbg::_db.gdbscript, "set debuginfod enabled off");

    // it's not running until record btrace is executed
    Ins(&R, mdbg::_db.gdbscript, "set record instruction-history-size 1000000");// for record btrace - 1M
    Ins(&R, mdbg::_db.gdbscript, "set record function-call-history-size 100000");// for record btrace -- 100k
    Ins(&R, mdbg::_db.gdbscript, "set record function-call-history 1000");// show this many at a time

    Ins(&R, mdbg::_db.gdbscript, "set confirm off");
    Ins(&R, mdbg::_db.gdbscript, "handle all nostop noprint pass");
    Ins(&R, mdbg::_db.gdbscript, "handle SIGSEGV SIGBUS stop print nopass");
    Ins(&R, mdbg::_db.gdbscript, "set pagination off");
    if (mdbg::_db.cmdline.catchthrow) {
        Ins(&R, mdbg::_db.gdbscript, "catch throw");
        mdbg::_db.bnum++;
    }
    if (mdbg::_db.cmdline.follow_child) {
        Ins(&R, mdbg::_db.gdbscript, "set follow-fork-mode child");
    }
    if (mdbg::_db.cmdline.py) {
        Ins(&R, mdbg::_db.gdbscript, "python");
        Ins(&R, mdbg::_db.gdbscript, "sys.path.insert(0, 'py')");
        Ins(&R, mdbg::_db.gdbscript, "end");
        Ins(&R, mdbg::_db.gdbscript, "source conf/gdb.py");
    }
    // Configure convenient multi-process debugging
    // We must keep all spawned processes under debugging, and allow them to run as much as possible.
    if (mdbg::_db.cmdline.mp) {
        Ins(&R, mdbg::_db.gdbscript, "set schedule-multiple on");
        Ins(&R, mdbg::_db.gdbscript, "set detach-on-fork off");
        Ins(&R, mdbg::_db.gdbscript, "set non-stop on");
        Ins(&R, mdbg::_db.gdbscript, "set mi-async"); // alias for 'target-async on', but newer
    }

    // load symbols
    Ins(&R, mdbg::_db.gdbscript, "file $localexe");
    if (mdbg::_db.cmdline.attach) {
        u32 pid = Find_Pid(R);
        if (pid != 0) {
            Set(R, "$pid", tempstr() << pid);
            Ins(&R, mdbg::_db.gdbscript, "attach $pid");
        }
    } else {
        // join argrs and show final command line
        mdbg::_db.gdbscript << "set args";
        ind_beg(command::mdbg_args_curs,arg,mdbg::_db.cmdline) {
            mdbg::_db.gdbscript << " " << strptr_ToBash(arg);
        }ind_end;
        mdbg::_db.gdbscript << eol;
        Ins(&R, mdbg::_db.gdbscript, "start");
    }

    // set up main breakpoint
    GenBreakpoints();
    if (ch_N(mdbg::_db.cmdline.bcmd) > 0) {
        Ins(&R, mdbg::_db.gdbscript, "commands");
        Ins(&R, mdbg::_db.gdbscript, "silent");
        Ins(&R, mdbg::_db.gdbscript, mdbg::_db.cmdline.bcmd);
        Ins(&R, mdbg::_db.gdbscript, "end");
    }
    // break right before our panic function
    Ins(&R, mdbg::_db.gdbscript, "break algo::FatalErrorExit");

    mdbg::_db.bnum++;

    if (!mdbg::_db.cmdline.attach && !mdbg::_db.break_main) {
        Ins(&R, mdbg::_db.gdbscript, "cont");
    }

    verblog(mdbg::_db.gdbscript);
    if (mdbg::_db.cmdline.dry_run) {
        prlog("#Gdb script is shown below");
        prlog(mdbg::_db.gdbscript);
    }
    StringToFile(mdbg::_db.gdbscript, "temp/mdbg.gdb");
}

// -----------------------------------------------------------------------------

static tempstr FindBuildDir() {
    return DirFileJoin("build",mdbg::_db.cmdline.cfg);
}

// -----------------------------------------------------------------------------

void mdbg::Main() {
    algo_lib::Replscope R;
    Set(R, "$target", mdbg::_db.cmdline.target, false);
    Set(R, "$tgtfname", mdbg::_db.cmdline.target, false);
    Set(R, "$cfg", mdbg::_db.cmdline.cfg, false);
    Set(R, "$builddir", FindBuildDir(), false);
    Set(R, "$localexe", DirFileJoin(FindBuildDir(),mdbg::_db.cmdline.target));

    tempstr buildtarget(_db.cmdline.target);

    if (_db.cmdline.tui) {
        _db.cmdline.emacs=false;
    }
    if (b_N(_db.cmdline)==0 && !_db.cmdline.mp) {
        b_Alloc(_db.cmdline) = "main";
    }
    if (_db.cmdline.mp) {
        // setup buildtarget as appropriate
    }
    Set(R, "$buildtarget", buildtarget, false);// what to rebuild

    // compile executable first to avoid embarrassment
    command::abt_proc abt;
    abt.cmd.cfg.expr = mdbg::_db.cmdline.cfg;
    abt.cmd.target.expr = buildtarget;
    if (_db.cmdline.dry_run) {
        prlog(abt_ToCmdline(abt.cmd));
    } else {
        abt_ExecX(abt);
    }

    tempstr local_exe(Subst(R,"$localexe"));
    if (!_db.cmdline.dry_run) {
        if (!FileQ(local_exe)) {
            vrfy(0, tempstr()<<"mdbg.nofile  file:"<<local_exe<<"  comment:\"File doesn't exist\"");
        }
    }

    Main_Gdb(R);

    if (mdbg::_db.cmdline.emacs) {
        Main_Emacs(R);
    } else {
        Main_NoEmacs(R);
    }
}
