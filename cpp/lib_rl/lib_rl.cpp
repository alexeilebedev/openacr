// Copyright (C) 2025-2026 AlgoRND
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
// Target: lib_rl (lib) -- GNU readline support library
// Exceptions: yes
// Source: cpp/lib_rl/lib_rl.cpp
//

#include "include/algo.h"
#include "include/lib_rl.h"
#include <readline/readline.h>
#include <readline/history.h>

////////////////////////////////////////////////////////////////////////////////
//
//                                HISTORY
//
////////////////////////////////////////////////////////////////////////////////

// compose history file name
tempstr lib_rl::HistoryFile(strptr app) {
    tempstr ret;
    char *home = getenv("HOME");
    if (home && home[0]) {
        ret << home;
    } else {
        ret << algo::GetCurDir();
    }
    MaybeDirSep(ret);
    ret << ".";
    if (ch_N(app)) {
        ret << app << "_";
    }
    ret << "history";
    return ret;
}

//------------------------------------------------------------------------------

// initialize history from file
// TODO lock
void lib_rl::InitHistory() {
    if (_db.isatty) {
        _db.history_file = HistoryFile(_db.cmdline.app);
        history_truncate_file(Zeroterm(_db.history_file),_db.cmdline.max_history);
        read_history(Zeroterm(_db.history_file));
        stifle_history(_db.cmdline.max_history);
    }
}

//------------------------------------------------------------------------------

// get last history line, empty if none
strptr lib_rl::LastHistory() {
    strptr ret;
    if (history_length > 0) {
        if (HIST_ENTRY *last = history_get(::history_length)) {
            ret = last->line;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------

// add line to history
void lib_rl::AddHistory(strptr line) {
    if (ch_N(line) && line != LastHistory() && _db.isatty) {
        tempstr tmp(line);
        add_history(Zeroterm(tmp));
        // todo lock
        append_history(1,Zeroterm(_db.history_file));
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                SAFE PRLOG
//
////////////////////////////////////////////////////////////////////////////////

// prlog handler - safe print when redline is active
void lib_rl::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime time, strptr str) {
    rl_clear_visible_line();
    fflush(rl_outstream);
    _db.Prlog(logcat,time,str);
    rl_forced_update_display();
}

//------------------------------------------------------------------------------

// setup own prlog function
void lib_rl::RedirectPrlog() {
    if (algo_lib::_db.Prlog != lib_rl::Prlog) {
        lib_rl::_db.Prlog = algo_lib::_db.Prlog;
        algo_lib::_db.Prlog = lib_rl::Prlog;
    }
}

//------------------------------------------------------------------------------

// restore prlog function
void lib_rl::RestorePrlog() {
    if (algo_lib::_db.Prlog == lib_rl::Prlog) {
        algo_lib::_db.Prlog = lib_rl::_db.Prlog;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                               ENGINE
//
////////////////////////////////////////////////////////////////////////////////

// whether the line is valid
bool lib_rl::LineValidQ() {
    return _db.line_valid;
}

//------------------------------------------------------------------------------

// whether EOF
bool lib_rl::EofQ() {
    return !LineValidQ() && _db.eof;
}

//------------------------------------------------------------------------------

// stream error
int lib_rl::Error() {
    return !LineValidQ() ? _db.err : 0;
}

//------------------------------------------------------------------------------

// whether realine in substate - not bare chars
bool lib_rl::SubstateQ() {
    constexpr int substate_mask = RL_STATE_ISEARCH        //
        | RL_STATE_NSEARCH      //
        | RL_STATE_SEARCH       // search modes
        | RL_STATE_COMPLETING   // a completion is running
        | RL_STATE_METANEXT     // after ESC; building a key sequence
        | RL_STATE_NUMERICARG   // collecting numeric arg
        | RL_STATE_MACROINPUT   //
        | RL_STATE_MACRODEF     // keybord macros
        | RL_STATE_VIMOTION     //
        | RL_STATE_MULTIKEY     // vi/multi-key commands
        | RL_STATE_DISPATCHING  //
        | RL_STATE_MOREINPUT;   // command in progress
    return RL_ISSTATE(substate_mask);
}

//------------------------------------------------------------------------------

// whether mode is readline or normal
bool lib_rl::ReadlineQ() {
    return RL_ISSTATE(RL_STATE_CALLBACK);
}

// -----------------------------------------------------------------------------

void lib_rl::SetPrompt(algo::strptr prompt) {
    _db.cmdline.prompt=prompt;
    if (ReadlineQ()) {
        rl_callback_handler_install(Zeroterm(_db.cmdline.prompt),OnLine);
    }
}

//------------------------------------------------------------------------------

// switch  mode 0 - normal, 1 - readline
void lib_rl::SwitchMode(int mode) {
    if (_db.isatty) {
        if (mode) {
            if (!ReadlineQ()) {
                rl_callback_handler_install(Zeroterm(_db.cmdline.prompt),OnLine);
                RedirectPrlog();
            }
        } else {
            if (ReadlineQ()) {
                rl_clear_visible_line();
                rl_callback_handler_remove();
                RestorePrlog();
            }
        }
    }
}

//------------------------------------------------------------------------------

// get line
strptr lib_rl::GetLine() {
    if (!LineValidQ() && !EofQ()) {
        SwitchMode(1);
    }
    while (!LineValidQ() && !EofQ()) {
        // read input stream
        int c = getc(rl_instream);
        // check for eof and errors
        if (c==EOF) {
            _db.err = errno != EAGAIN && errno != EINTR ? errno : 0;
            _db.eof = feof(rl_instream) || _db.err;
            break;
        }
        // catch ^D for EOF
        if (_db.isatty && c == CTRL('D') && !rl_point && !rl_end && !SubstateQ()) {
            _db.eof = true;
            break;
        }
        if (_db.isatty) {
            rl_stuff_char(c);
            rl_callback_read_char(); // calls lib_rl::OnLine()
        } else if (c == '\n') {
            _db.line_valid = true;
        } else {
            ch_Alloc(_db.line) = c;
        }
    }
    if (LineValidQ() || EofQ()) {
        SwitchMode(0);
    }
    return LineValidQ() ? _db.line : strptr();
}

//------------------------------------------------------------------------------

// called inside rl_callback_read_char() when new input line entered
void lib_rl::OnLine(char *zline) {
    strptr line(zline);
    _db.line = line;
    _db.line_valid = true;
    if (_db.cmdline.add_history) {
        AddHistory(line);
    }
}

//------------------------------------------------------------------------------

// Skip current line
void lib_rl::SkipLine() {
    _db.line_valid = false;
    ch_RemoveAll(_db.line);
}

//------------------------------------------------------------------------------

// Kill entire buffer
void lib_rl::KillBuffer() {
    SkipLine();
    rl_replace_line("", 0);
    if (ReadlineQ()) {
        rl_clear_visible_line();
        rl_forced_update_display();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                SETUP/TEARDOWN
//
////////////////////////////////////////////////////////////////////////////////

// begin readline, user shall add own iohook function
void lib_rl::BeginReadline(strptr app, strptr prompt) {
    if (!ValidQ(_db.iohook.fildes)) {
        _db.cmdline.app = app;
        rl_readline_name = Zeroterm(_db.cmdline.app); // ponter - do not modify
        rl_initialize();
        _db.isatty = isatty(fileno(rl_instream));
        InitHistory();
        _db.cmdline.prompt = prompt;
        _db.iohook.fildes = algo::Fildes(fileno(rl_instream));
        algo::SetBlockingMode(_db.iohook.fildes,false);
        IOEvtFlags flags;
        read_Set(flags, true);
        algo_lib::IohookAdd(_db.iohook, flags);
        SwitchMode(1);
    }
}

//------------------------------------------------------------------------------

// stop reading readline
void lib_rl::EndReadline() {
    if (ValidQ(_db.iohook.fildes)) {
        SwitchMode(0);
        algo::Refurbish(_db.iohook);
    }
}

//------------------------------------------------------------------------------

// restore terminal settings
void lib_rl::iohook_Cleanup() {
    EndReadline();
}
