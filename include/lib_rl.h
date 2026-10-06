// Copyright (C) 2025-2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: lib_rl (lib) -- Line editor for interactive tools: history, search and completion
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

    // Load history from the history file, keeping its last max_history lines.
    // A file that holds more is rewritten with the lines kept, so it never grows
    // past the limit by more than one session's lines.
    void InitHistory();

    // get last history line, empty if none
    strptr LastHistory();

    // Add LINE to history and append it to the history file, unless it is empty or
    // repeats the last line.
    void AddHistory(strptr line);

    // prlog handler: erase the prompt and line, print STR through the handler the
    // app had, and draw them again below it, so a log line never lands in the middle
    // of what the user is typing.  LOGCAT and TIME pass through.
    void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime time, strptr str);

    // setup own prlog function
    void RedirectPrlog();

    // restore prlog function
    void RestorePrlog();

    // Feed byte C, read from the terminal, to the key decoder, and apply the key it
    // completes in the current state.  A byte that leaves the key unfinished is
    // kept in _db.keystate and _db.keyarg.  A key that ends the line sets
    // LineValidQ.
    void ApplyByte(u8 c);

    // whether the line is valid
    bool LineValidQ();

    // whether EOF
    bool EofQ();

    // stream error
    int Error();

    // True while a key means something other than itself: a search is on, or the
    // decoder is inside a multi-byte key.
    bool SubstateQ();

    // End a search, restoring the line it began with, and drop a half-read key.
    void CancelSubstate();

    // True when the editor holds the terminal
    bool ReadlineQ();

    // Show PROMPT before the line from now on, redrawing it when the editor holds
    // the terminal.
    void SetPrompt(algo::strptr prompt);

    // switch  mode 0 - normal, 1 - readline
    void SwitchMode(int mode);

    // Read every byte that is waiting on standard input, and return the line once
    // one is complete, or an empty strptr while it is not.  On a terminal the bytes
    // are keys of the editor, and the line is redrawn once after them.  Otherwise a
    // line ends at a newline or at the end of input.  ^D on an empty line, and the
    // end of input, set EofQ.
    strptr GetLine();

    // Skip current line
    void SkipLine();

    // Abandon the line being typed.  On the screen it stays, marked ^C, and the
    // prompt starts again on the row below with an empty line.
    void KillBuffer();

    // The line as it stands in the editor, which an app reads to work ahead of a
    // completion it has not been asked for yet.
    strptr CurLine();

    // True when the word starting at BEG is LINE's first, which is the only
    // position that names a command.
    bool FirstWordQ(strptr line, int beg);

    // The last blank-separated word of LINE, which is the one a completion acts on.
    strptr LastWord(strptr line);

    // The directory part of WORD -- everything before its last '/', with the root
    // spelled "/" -- and empty when the word names no directory.
    strptr WordDir(strptr word);

    // The name part of WORD -- everything after its last '/'.
    strptr WordName(strptr word);

    // Offer WORD as a candidate for the completion query in flight.  Call only
    // from the h_compl hook, once per word that matches what the user typed.
    void AddCandidate(strptr word);

    // The longest prefix every offered candidate shares, empty when none was
    // offered.  Against `part`, `passwd` and `path` that is `pa`; against one
    // candidate it is that word, which is how a unique match completes outright.
    tempstr CommonPrefix();

    // Begin reading lines for application APP with prompt PROMPT: load the
    // history, register standard input with the event loop, and, on a terminal,
    // draw the prompt.  The app sets _db.iohook.callback to learn that input is
    // waiting, and calls GetLine from a step to read it.
    void BeginReadline(strptr app, strptr prompt);

    // Stop reading lines, and give the terminal back the modes it had.
    void EndReadline();

    // restore terminal settings
    //     (user-implemented function, prototype is in amc-generated header)
    // void iohook_Cleanup(); // dmmeta.ffunc:lib_rl.FDb.iohook.Cleanup

    // flush unread input, so it will not go to shell
    void FlushInput();
}
