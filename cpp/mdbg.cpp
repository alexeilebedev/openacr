// (C) AlgoEngineering LLC 2008-2013
// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: mdbg (exe) -- My debugger
// Exceptions: NO
// Source: cpp/mdbg.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev shreejith.lokegowda
//

#include "include/gen/mdbg_gen.h"
#include "include/gen/mdbg_gen.inl.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

// -----------------------------------------------------------------------------

// Set up breakpoints
static void GenBreakpoints() {
    StringIter breaklist(mdbg::_db.cmdline.b);
    cstring &out = mdbg::_db.gdbscript;
    while (!breaklist.EofQ()) {
        out << "break " << GetTokenChar(breaklist, ',') << eol;
    }
}

// -----------------------------------------------------------------------------

// Find pid of existing target process in case we want to debug it
static u32 Find_Pid(algo_lib::Replscope &R) {
    tempstr pid_str;
    pid_str << Trimmed(SysEval(Subst(R, "pidof $tgtfname"), FailokQ(true), 1024));
    u32 pid;
    StringIter iter(pid_str);
    if (!TryParseU32(iter, pid)) { // if pid is a valid value
        prlog(Subst(R, "Could not find the pid of the process:$tgtfname to attach!"));
        prlog("Make sure process is running and -root option is enabled(if remote)...");
        pid = 0;
    }
    return pid;
}

// -----------------------------------------------------------------------------

// Commands to enable debugging through emacs
// Static Emacs functions
static void AddEmacsFunctions(algo_lib::Replscope &R) {
    // helper function
    // works in rhel7, where most of the integration between gdb and emacs is broken.
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
    Ins(&R, mdbg::_db.script, "  (gdb-send-input-ui \"!abt $target -cfg $cfg\")");
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
    Ins(&R, mdbg::_db.script, "; (C) Intercontinental Exchange | NYSE 2014-2016");
    Ins(&R, mdbg::_db.script, "; This file was generated by mdbg");
    Ins(&R, mdbg::_db.script, "(defun gdb-restore-windows-full () (interactive)");
    if (!mdbg::_db.rhel7) {
        Ins(&R, mdbg::_db.script, "  (gdb-display-assembler-buffer)");
    }
    Ins(&R, mdbg::_db.script, "  (gdb-restore-windows)");

    if (mdbg::_db.cmdline.disas) {
        Ins(&R, mdbg::_db.script, "  (split-window-horizontally)"); // make another window to the right
        Ins(&R, mdbg::_db.script, "  (switch-to-buffer \"*disassembly of $target*\")");
        Ins(&R, mdbg::_db.script, "  (other-window -1)"); // back in source
    }
    Ins(&R, mdbg::_db.script, "  )");

    AddEmacsFunctions(R);

    bool can_debug = true;

    if (can_debug) {
        Ins(&R, mdbg::_db.script, "");
        if (mdbg::_db.cmdline.manywin) {
            Ins(&R, mdbg::_db.script, "(setq gdb-many-windows t)");
        }
        Set(R, "$gdbopt", (mdbg::_db.rhel7 ? "-i=mi" : "--annotate=3"));
        Ins(&R, mdbg::_db.script, "(gdb \"gdb $gdbopt --init-command=temp/mdbg.gdb\")");

        StringToFile(mdbg::_db.script, "temp/mdbg.el");

        DisplayEmacsNotes();

        SysCmd("emacs -nw --load temp/mdbg.el", FailokQ(true), DryrunQ(false));
    }
}

// -----------------------------------------------------------------------------
// Commands to enable debugging through gdb command line
//
static void Main_NoEmacs(algo_lib::Replscope &R) {
    // another helper
    Ins(&R, mdbg::_db.script, "# ");
    Ins(&R, mdbg::_db.script, "# (C) Intercontinental Exchange | NYSE 2014-2016");
    Ins(&R, mdbg::_db.script, "# This file was generated by mdbg");
    Ins(&R, mdbg::_db.script, "gdb $gdbopt --init-command=temp/mdbg.gdb");
    StringToFile(mdbg::_db.script, "temp/mdbg.nel");
    SysCmd(". temp/mdbg.nel", FailokQ(true), DryrunQ(false));
}

// -----------------------------------------------------------------------------
// Generate gdb script
//
static void Main_Gdb(algo_lib::Replscope &R) {
    Ins(&R, mdbg::_db.gdbscript, "# (C) Intercontinental Exchange | NYSE 2014-2016");
    Ins(&R, mdbg::_db.gdbscript, "# This file was generated by mdbg");

    //Ins(&R, mdbg::_db.gdbscript, "set trace-commands on");
    //Ins(&R, mdbg::_db.gdbscript, "set logging on"); - really unnecessary
    Ins(&R, mdbg::_db.gdbscript, "file $localexe");
    Ins(&R, mdbg::_db.gdbscript, "set args $args");
    if (mdbg::_db.cmdline.attach) {
        u32 pid = Find_Pid(R);
        if (pid != 0) {
            Set(R, "$pid", tempstr() << pid);
            Ins(&R, mdbg::_db.gdbscript, "attach $pid");
        }
    } else {
        Ins(&R, mdbg::_db.gdbscript, "start");
    }
    Ins(&R, mdbg::_db.gdbscript, "set history filename temp/gdb.history");
    Ins(&R, mdbg::_db.gdbscript, "set history save on");
    Ins(&R, mdbg::_db.gdbscript, "set record instruction-history-size 1000000");// for record btrace - 1M
    Ins(&R, mdbg::_db.gdbscript, "set record function-call-history-size 100000");// for record btrace -- 100k
    Ins(&R, mdbg::_db.gdbscript, "set record function-call-history 1000");// show this many at a time
    Ins(&R, mdbg::_db.gdbscript, "set confirm off");
    Ins(&R, mdbg::_db.gdbscript, "handle SIGPIPE nostop");
    Ins(&R, mdbg::_db.gdbscript, "set pagination off");
    if (mdbg::_db.cmdline.catchthrow) {
        Ins(&R, mdbg::_db.gdbscript, "catch throw");
    }
    if (mdbg::_db.cmdline.follow_child) {
        Ins(&R, mdbg::_db.gdbscript, "set follow-fork-mode child");
    }
    if (mdbg::_db.cmdline.py) {
        Ins(&R, mdbg::_db.gdbscript, "python");
        Ins(&R, mdbg::_db.gdbscript, "sys.path.insert(0, 'py')");
        Ins(&R, mdbg::_db.gdbscript, "end");
    }

    // set up main breakpoint
    GenBreakpoints();

    if (ch_N(mdbg::_db.cmdline.bcmd) > 0) {
        Ins(&R, mdbg::_db.gdbscript, "commands");
        Ins(&R, mdbg::_db.gdbscript, "silent");
        Ins(&R, mdbg::_db.gdbscript, mdbg::_db.cmdline.bcmd);
        Ins(&R, mdbg::_db.gdbscript, "end");
    }

    Ins(&R, mdbg::_db.gdbscript, "break algo::FatalErrorExit");
    if (!mdbg::_db.cmdline.attach && mdbg::_db.cmdline.b != "main") {
        Ins(&R, mdbg::_db.gdbscript, "cont");
    }

    StringToFile(mdbg::_db.gdbscript, "temp/mdbg.gdb");
}

// -----------------------------------------------------------------------------

static tempstr FindBuildDir() {
    mdbg::FCfg *cfg = mdbg::ind_cfg_Find(mdbg::_db.cmdline.cfg);
    vrfy(cfg
         , tempstr()<<"mdbg.nocfg"
         <<Keyval("cfg",mdbg::_db.cmdline.cfg)
         <<Keyval("comment","configuration not found"));

    vrfy(c_builddir_N(*cfg)>0
         , tempstr()<<"mdbg.nobuilddir"
         <<Keyval("cfg",cfg->cfg)
         <<Keyval("comment","no build directories associated with cfg"));
    return tempstr()<<c_builddir_Find(*cfg,0)->builddir;
}

// -----------------------------------------------------------------------------

void mdbg::Main() {
    mdbg::_db.rhel7 = true;

    algo_lib::Replscope R;
    Set(R, "$target", mdbg::_db.cmdline.target, false);
    Set(R, "$tgtfname", mdbg::_db.cmdline.target, false);
    Set(R, "$cfg", mdbg::_db.cmdline.cfg, false);
    Set(R, "$localexe", DirFileJoin(FindBuildDir(),mdbg::_db.cmdline.target));

    // compile executable first to avoid embarrassment
    command::abt_proc abt;
    abt.cmd.cfg = mdbg::_db.cmdline.cfg;
    abt.cmd.target.expr = mdbg::_db.cmdline.target;
    abt_ExecX(abt);

    tempstr local_exe(Subst(R,"$localexe"));
    if (!FileQ(local_exe)) {
        vrfy(0, tempstr()<<"mdbg.nofile  file:"<<local_exe<<"  comment:\"File doesn't exist\"");
    }

    Set(R, "$args", mdbg::_db.cmdline.args, false);
    Main_Gdb(R);

    if (mdbg::_db.cmdline.tui) {
        SysCmd(Subst(R,"gdb -tui  --init-command=temp/mdbg.gdb"), FailokQ(true), DryrunQ(false));
    } else if (mdbg::_db.cmdline.emacs) {
        Main_Emacs(R);
    } else {
        Main_NoEmacs(R);
    }
}
