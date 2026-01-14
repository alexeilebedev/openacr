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
// Header: include/lib_rl.h
//

#include "include/gen/lib_rl_gen.h"
#include "include/gen/lib_rl_gen.inl.h"

namespace lib_rl { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_rl/lib_rl.cpp
    //

    // compose history file name
    tempstr HistoryFile(strptr app);

    // initialize history from file
    // TODO lock
    void InitHistory();

    // get last history line, empty if none
    strptr LastHistory();

    // add line to history
    void AddHistory(strptr line);

    // prlog handler - safe print when redline is active
    void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime time, strptr str);

    // setup own prlog function
    void RedirectPrlog();

    // restore prlog function
    void RestorePrlog();

    // whether the line is valid
    bool LineValidQ();

    // whether EOF
    bool EofQ();

    // stream error
    int Error();

    // whether realine in substate - not bare chars
    bool SubstateQ();

    // whether mode is readline or normal
    bool ReadlineQ();
    void SetPrompt(algo::strptr prompt);

    // switch  mode 0 - normal, 1 - readline
    void SwitchMode(int mode);

    // get line
    strptr GetLine();

    // called inside rl_callback_read_char() when new input line entered
    void OnLine(char *zline);

    // Skip current line
    void SkipLine();

    // Kill entire buffer
    void KillBuffer();

    // begin readline, user shall add own iohook function
    void BeginReadline(strptr app, strptr prompt);

    // stop reading readline
    void EndReadline();

    // restore terminal settings
    //     (user-implemented function, prototype is in amc-generated header)
    // void iohook_Cleanup(); // fcleanup:lib_rl.FDb.iohook
}
