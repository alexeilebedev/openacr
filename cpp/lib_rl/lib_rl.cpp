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
// Source: cpp/lib_rl/lib_rl.cpp
//
// The editor is a state machine over the line in _db.edit.  In state idle the
// terminal belongs to the app.  In state edit the terminal is in raw mode, the
// prompt and the line are drawn, and each key edits the line.  In state search
// each key extends a reverse search of history, and the line shows the match.
// A decoder underneath turns the bytes of a key into one lib_rl.Key, and
// _db.keystate says how far into a multi-byte key it is.  GetLine reads every
// byte that is waiting, then redraws once.

#include "include/algo.h"
#include "include/lib_rl.h"
#include <sys/ioctl.h>
#include <termios.h>

////////////////////////////////////////////////////////////////////////////////
//
//                                HISTORY
//
////////////////////////////////////////////////////////////////////////////////

// compose history file name
tempstr lib_rl::HistoryFile(strptr app) {
    tempstr ret;
    char *home = getenv(algo_lib::dev_envvar_HOME);
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

// Load history from the history file, keeping its last max_history lines.
// A file that holds more is rewritten with the lines kept, so it never grows
// past the limit by more than one session's lines.
void lib_rl::InitHistory() {
    hist_RemoveAll();
    if (_db.isatty) {
        _db.history_file = HistoryFile(_db.cmdline.app);
        tempstr text(algo::FileToString(_db.history_file, algo::FileFlags()));
        int nline = 0;
        ind_beg(algo::Line_curs, line, text) {
            nline += ch_N(line) > 0;
        }ind_end;
        int skip = i32_Max(nline - _db.cmdline.max_history, 0);
        ind_beg(algo::Line_curs, line, text) {
            if (ch_N(line) > 0 && skip > 0) {
                skip--;
            } else if (ch_N(line) > 0) {
                hist_Alloc() = line;
            }
        }ind_end;
        if (nline > hist_N()) {
            tempstr kept;
            ind_beg(_db_hist_curs, hist, _db) {
                kept << hist << eol;
            }ind_end;
            (void)algo::SafeStringToFile(kept, _db.history_file);
        }
    }
    _db.hist_pos = hist_N();
}

//------------------------------------------------------------------------------

// get last history line, empty if none
strptr lib_rl::LastHistory() {
    strptr ret;
    if (hist_N() > 0) {
        ret = hist_qFind(hist_N() - 1);
    }
    return ret;
}

//------------------------------------------------------------------------------

// Add LINE to history and append it to the history file, unless it is empty or
// repeats the last line.
void lib_rl::AddHistory(strptr line) {
    if (ch_N(line) && line != LastHistory() && _db.isatty) {
        hist_Alloc() = line;
        tempstr text;
        text << line << eol;
        (void)algo::StringToFile(text, _db.history_file, algo_FileFlags_append);
    }
    _db.hist_pos = hist_N();
}

////////////////////////////////////////////////////////////////////////////////
//
//                                TERMINAL
//
////////////////////////////////////////////////////////////////////////////////

// Write TEXT to the terminal.  Off a terminal nothing is drawn, which is also
// what lets a test drive the editor through ApplyByte.
static void Out(strptr text) {
    if (lib_rl::_db.isatty) {
        algo::WriteFile(algo::Fildes(1), (u8*)text.elems, text.n_elems);
    }
}

//------------------------------------------------------------------------------

// Return the width of the terminal in columns, 80 when it does not say.
static int GetCols() {
    struct winsize winsize;
    winsize.ws_col = 0;
    (void)ioctl(1, TIOCGWINSZ, &winsize);
    return winsize.ws_col > 0 ? winsize.ws_col : 80;
}

//------------------------------------------------------------------------------

// Return the number of columns TEXT takes on the terminal: one per character,
// and none for a color or other CSI escape sequence, which a prompt may carry.
static int GetWidth(strptr text) {
    int ret = 0;
    int i = 0;
    while (i < ch_N(text)) {
        u8 c = text.elems[i];
        if (c == 27 && i + 1 < ch_N(text) && text.elems[i + 1] == '[') {
            i += 2;
            while (i < ch_N(text) && !(u8(text.elems[i]) >= 0x40 && u8(text.elems[i]) <= 0x7e)) {
                i++;
            }
            i++;
        } else {
            ret += (c & 0xc0) != 0x80 && c >= 0x20;
            i++;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------

// Put the terminal in raw mode: a key arrives as it is typed, and the editor
// echoes it.  Signals stay on, so ^C still reaches the app as SIGINT.  What the
// terminal held before is kept on _db for RawEnd.
static void RawBegin() {
    struct termios tio;
    if (tcgetattr(0, &tio) == 0) {
        lib_rl::_db.tio_lflag = tio.c_lflag;
        lib_rl::_db.tio_vmin = tio.c_cc[VMIN];
        lib_rl::_db.tio_vtime = tio.c_cc[VTIME];
        tio.c_lflag &= ~tcflag_t(ICANON | ECHO);
        tio.c_cc[VMIN] = 1;
        tio.c_cc[VTIME] = 0;
        (void)tcsetattr(0, TCSADRAIN, &tio);
    }
}

//------------------------------------------------------------------------------

// Give the terminal back the modes RawBegin found.
static void RawEnd() {
    struct termios tio;
    if (tcgetattr(0, &tio) == 0) {
        tio.c_lflag = tcflag_t(lib_rl::_db.tio_lflag);
        tio.c_cc[VMIN] = lib_rl::_db.tio_vmin;
        tio.c_cc[VTIME] = lib_rl::_db.tio_vtime;
        (void)tcsetattr(0, TCSADRAIN, &tio);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                DISPLAY
//
////////////////////////////////////////////////////////////////////////////////

// Append to OUT what erases the drawn prompt and line: the cursor climbs to the
// first row they took, and everything below it is cleared.
static void Erase(cstring &out) {
    if (lib_rl::_db.cursor_row > 0) {
        out << "\033[" << lib_rl::_db.cursor_row << "A";
    }
    out << "\r\033[J";
    lib_rl::_db.cursor_row = 0;
}

//------------------------------------------------------------------------------

// Append to OUT the prompt and the line, with the cursor left at byte POS of
// the line.  In search the prompt shows what is searched for, and says when
// nothing in history matches it.
//
// A line longer than the terminal wraps, so the cursor may stand rows below the
// prompt.  The rows are counted from the widths, and _db.cursor_row records
// where the cursor ends, so that Erase can find the first row again.  A line
// that ends exactly at the right margin leaves the terminal waiting to wrap, so
// the cursor is moved to the next row by hand.
static void Draw(cstring &out, int pos) {
    tempstr prompt;
    if (lib_rl::_db.state.value == lib_rl_State_search) {
        bool fail = lib_rl::_db.search_pos < 0 || algo::FindStr(lib_rl::_db.edit, lib_rl::_db.search_str) < 0;
        prompt << (fail ? "(failed reverse-i-search)`" : "(reverse-i-search)`")
               << lib_rl::_db.search_str << "': ";
    } else {
        prompt << lib_rl::_db.cmdline.prompt;
    }
    int cols = GetCols();
    int width = GetWidth(prompt);
    int endcol = width + GetWidth(lib_rl::_db.edit);
    int curcol = width + GetWidth(ch_FirstN(lib_rl::_db.edit, pos));
    out << prompt << lib_rl::_db.edit;
    if (endcol > 0 && endcol % cols == 0) {
        out << "\r\n";
    }
    int uprow = endcol / cols - curcol / cols;
    if (uprow > 0) {
        out << "\033[" << uprow << "A";
    }
    out << "\r";
    if (curcol % cols > 0) {
        out << "\033[" << curcol % cols << "C";
    }
    lib_rl::_db.cursor_row = curcol / cols;
}

//------------------------------------------------------------------------------

// Redraw the prompt and the line where they stand, cursor at the point.
static void Redisplay() {
    tempstr out;
    Erase(out);
    Draw(out, lib_rl::_db.point);
    Out(out);
}

//------------------------------------------------------------------------------

// Leave the drawn line on the screen as it stands, with the cursor after its
// end, and start a new row below it.  MARK is written at the end of the line
// first, as ^C marks an abandoned one.
static void Finish(strptr mark) {
    tempstr out;
    Erase(out);
    Draw(out, ch_N(lib_rl::_db.edit));
    out << mark << "\r\n";
    lib_rl::_db.cursor_row = 0;
    Out(out);
}

////////////////////////////////////////////////////////////////////////////////
//
//                                SAFE PRLOG
//
////////////////////////////////////////////////////////////////////////////////

// prlog handler: erase the prompt and line, print STR through the handler the
// app had, and draw them again below it, so a log line never lands in the middle
// of what the user is typing.  LOGCAT and TIME pass through.
void lib_rl::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime time, strptr str) {
    tempstr out;
    Erase(out);
    Out(out);
    _db.Prlog(logcat, time, str);
    ch_RemoveAll(out);
    Draw(out, _db.point);
    Out(out);
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
//                                EDITING
//
////////////////////////////////////////////////////////////////////////////////

// Return the byte offset of the character before byte POS of the line.
static int PrevPos(int pos) {
    int ret = i32_Max(pos - 1, 0);
    while (ret > 0 && (u8(lib_rl::_db.edit.ch_elems[ret]) & 0xc0) == 0x80) {
        ret--;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Return the byte offset of the character after byte POS of the line.
static int NextPos(int pos) {
    int ret = i32_Min(pos + 1, ch_N(lib_rl::_db.edit));
    while (ret < ch_N(lib_rl::_db.edit) && (u8(lib_rl::_db.edit.ch_elems[ret]) & 0xc0) == 0x80) {
        ret++;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Return the offset where the word before byte POS begins: blanks are skipped
// back, then the word.
static int WordBack(int pos) {
    int ret = pos;
    while (ret > 0 && lib_rl::_db.edit.ch_elems[ret - 1] == ' ') {
        ret--;
    }
    while (ret > 0 && lib_rl::_db.edit.ch_elems[ret - 1] != ' ') {
        ret--;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Return the offset where the word after byte POS ends: blanks are skipped,
// then the word.
static int WordFwd(int pos) {
    int ret = pos;
    while (ret < ch_N(lib_rl::_db.edit) && lib_rl::_db.edit.ch_elems[ret] == ' ') {
        ret++;
    }
    while (ret < ch_N(lib_rl::_db.edit) && lib_rl::_db.edit.ch_elems[ret] != ' ') {
        ret++;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Replace bytes BEG to END of the line with TEXT, and put the cursor after it.
// With KILL, the bytes replaced go to the kill buffer.  Every change to the
// line is made here.
static void Replace(int beg, int end, strptr text, bool kill) {
    tempstr edit;
    edit << ch_FirstN(lib_rl::_db.edit, beg) << text << ch_RestFrom(lib_rl::_db.edit, end);
    if (kill) {
        lib_rl::_db.killbuf = ch_GetRegion(lib_rl::_db.edit, beg, end - beg);
    }
    lib_rl::_db.edit = edit;
    lib_rl::_db.point = beg + ch_N(text);
}

//------------------------------------------------------------------------------

// Show history line POS, or the new line when POS is hist_N().  Stepping away
// from the new line keeps it, so stepping back finds it as it was.
static void HistMove(int pos) {
    if (pos >= 0 && pos <= lib_rl::hist_N() && pos != lib_rl::_db.hist_pos) {
        if (lib_rl::_db.hist_pos == lib_rl::hist_N()) {
            lib_rl::_db.hist_edit = lib_rl::_db.edit;
        }
        lib_rl::_db.hist_pos = pos;
        strptr text = pos == lib_rl::hist_N() ? strptr(lib_rl::_db.hist_edit) : strptr(lib_rl::hist_qFind(pos));
        Replace(0, ch_N(lib_rl::_db.edit), text, false);
    }
}

//------------------------------------------------------------------------------

// Print the candidates below the line in columns, then draw the line again
// under them.
static void ListCandidates() {
    int width = 0;
    ind_beg(lib_rl::_db_cand_curs, cand, lib_rl::_db) {
        width = i32_Max(width, GetWidth(cand) + 2);
    }ind_end;
    int ncol = i32_Max(GetCols() / width, 1);
    Finish("");
    tempstr out;
    int col = 0;
    ind_beg(lib_rl::_db_cand_curs, cand, lib_rl::_db) {
        col++;
        out << cand;
        if (col == ncol || ind_curs(cand).index == lib_rl::cand_N() - 1) {
            out << "\r\n";
            col = 0;
        } else {
            char_PrintNTimes(' ', out, width - GetWidth(cand));
        }
    }ind_end;
    Out(out);
}

//------------------------------------------------------------------------------

// Complete the word before the cursor.  The app's h_compl hook offers the
// candidates.  One candidate replaces the word, followed by a blank unless it
// ends in '/', since a directory is completed one component at a time.
// Several candidates put in what they all begin with, and when that adds
// nothing, a second TAB in a row lists them.
static void Complete() {
    int beg = lib_rl::_db.point;
    while (beg > 0 && lib_rl::_db.edit.ch_elems[beg - 1] != ' ') {
        beg--;
    }
    lib_rl::Compl compl_;
    compl_.line = lib_rl::_db.edit;
    compl_.word = ch_GetRegion(lib_rl::_db.edit, beg, lib_rl::_db.point - beg);
    compl_.wordbeg = beg;
    lib_rl::cand_RemoveAll();
    lib_rl::h_compl_Call(compl_);
    tempstr common(lib_rl::CommonPrefix());
    bool unique = lib_rl::cand_N() == 1;
    bool longer = ch_N(common) > ch_N(compl_.word);
    if (unique || longer) {
        bool dir = ch_N(common) > 0 && common.ch_elems[ch_N(common) - 1] == '/';
        if (unique && !dir) {
            common << " ";
        }
        Replace(beg, lib_rl::_db.point, common, false);
    } else if (lib_rl::cand_N() > 1 && lib_rl::_db.tab_last) {
        ListCandidates();
    }
    lib_rl::cand_RemoveAll();
}

//------------------------------------------------------------------------------

// Hand the line to the app: it stays on the screen, the cursor moves to a new
// row, and the history gets it.
static void Accept() {
    Finish("");
    lib_rl::_db.line = lib_rl::_db.edit;
    lib_rl::_db.line_valid = true;
    if (lib_rl::_db.cmdline.add_history) {
        lib_rl::AddHistory(lib_rl::_db.line);
    }
    lib_rl::_db.hist_pos = lib_rl::hist_N();
    ch_RemoveAll(lib_rl::_db.edit);
    lib_rl::_db.point = 0;
}

//------------------------------------------------------------------------------

// Look for the search string in history, from line START back to the oldest,
// and show the first line that holds it, with the cursor at the match.  When no
// line does, the line shown stays, and the prompt says the search failed.
static void SearchFrom(int start) {
    int found = -1;
    for (int i = i32_Min(start, lib_rl::hist_N() - 1); i >= 0 && found < 0; i--) {
        if (algo::FindStr(lib_rl::hist_qFind(i), lib_rl::_db.search_str) >= 0) {
            found = i;
        }
    }
    if (found >= 0) {
        lib_rl::_db.search_pos = found;
        Replace(0, ch_N(lib_rl::_db.edit), lib_rl::hist_qFind(found), false);
        lib_rl::_db.point = algo::FindStr(lib_rl::_db.edit, lib_rl::_db.search_str);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                STATE MACHINE
//
////////////////////////////////////////////////////////////////////////////////

// Move the editor to state STATE.  Every transition is made here, together
// with what entering or leaving a state does to the terminal and the line:
// - entering edit from idle puts the terminal in raw mode, routes prlog
//   through the editor, and draws the prompt;
// - entering idle erases the prompt and gives the terminal back;
// - entering search keeps the line, which ^G restores;
// - leaving search for edit keeps the match, and history moves to it.
static void SetState(lib_rl::State state) {
    lib_rl::State prev = lib_rl::_db.state;
    lib_rl::_db.state = state;
    if (prev.value == lib_rl_State_idle && state.value != lib_rl_State_idle) {
        RawBegin();
        lib_rl::RedirectPrlog();
        lib_rl::_db.cursor_row = 0;
        Redisplay();
    } else if (prev.value != lib_rl_State_idle && state.value == lib_rl_State_idle) {
        tempstr out;
        Erase(out);
        Out(out);
        RawEnd();
        lib_rl::RestorePrlog();
    }
    if (state.value == lib_rl_State_search && prev.value != lib_rl_State_search) {
        // the line being typed is kept as history browsing keeps it, so ^N
        // after the search comes back to it
        if (lib_rl::_db.hist_pos == lib_rl::hist_N()) {
            lib_rl::_db.hist_edit = lib_rl::_db.edit;
        }
        lib_rl::_db.search_edit = lib_rl::_db.edit;
        ch_RemoveAll(lib_rl::_db.search_str);
        lib_rl::_db.search_pos = -1;
    }
    if (prev.value == lib_rl_State_search && state.value == lib_rl_State_edit && lib_rl::_db.search_pos >= 0) {
        lib_rl::_db.hist_pos = lib_rl::_db.search_pos;
    }
}

//------------------------------------------------------------------------------

// Apply KEY in state edit.
static void OnKeyEdit(lib_rl::Key key) {
    int len = ch_N(lib_rl::_db.edit);
    int pos = lib_rl::_db.point;
    switch (key.value) {
    case lib_rl_Key_ctrl_a: case lib_rl_Key_home:
        lib_rl::_db.point = 0;
        break;
    case lib_rl_Key_ctrl_e: case lib_rl_Key_end:
        lib_rl::_db.point = len;
        break;
    case lib_rl_Key_ctrl_b: case lib_rl_Key_left:
        lib_rl::_db.point = PrevPos(pos);
        break;
    case lib_rl_Key_ctrl_f: case lib_rl_Key_right:
        lib_rl::_db.point = NextPos(pos);
        break;
    case lib_rl_Key_word_left:
        lib_rl::_db.point = WordBack(pos);
        break;
    case lib_rl_Key_word_right:
        lib_rl::_db.point = WordFwd(pos);
        break;
    case lib_rl_Key_ctrl_h: case lib_rl_Key_backspace:
        Replace(PrevPos(pos), pos, "", false);
        break;
    case lib_rl_Key_ctrl_d: case lib_rl_Key_delete:
        Replace(pos, NextPos(pos), "", false);
        break;
    case lib_rl_Key_ctrl_k:
        Replace(pos, len, "", true);
        break;
    case lib_rl_Key_ctrl_u:
        Replace(0, pos, "", true);
        break;
    case lib_rl_Key_ctrl_w:
        Replace(WordBack(pos), pos, "", true);
        break;
    case lib_rl_Key_kill_word:
        Replace(pos, WordFwd(pos), "", true);
        break;
    case lib_rl_Key_ctrl_y:
        Replace(pos, pos, lib_rl::_db.killbuf, false);
        break;
    case lib_rl_Key_ctrl_p: case lib_rl_Key_up:
        HistMove(lib_rl::_db.hist_pos - 1);
        break;
    case lib_rl_Key_ctrl_n: case lib_rl_Key_down:
        HistMove(lib_rl::_db.hist_pos + 1);
        break;
    case lib_rl_Key_ctrl_l:
        Out("\033[H\033[2J");
        lib_rl::_db.cursor_row = 0;
        break;
    case lib_rl_Key_ctrl_r:
        SetState(lib_rl::State(lib_rl_State_search));
        break;
    case lib_rl_Key_tab:
        Complete();
        break;
    case lib_rl_Key_ctrl_j: case lib_rl_Key_ctrl_m:
        Accept();
        break;
    default:
        if (key.value >= 0x20 && key.value < 0x100 && key.value != lib_rl_Key_backspace) {
            char c = char(key.value);
            Replace(pos, pos, strptr(&c, 1), false);
        }
        break;
    }
    lib_rl::_db.tab_last = key.value == lib_rl_Key_tab;
}

//------------------------------------------------------------------------------

// Apply KEY in state search.  A character extends the search string and ^R
// looks further back.  Backspace shortens the string and searches again from
// the newest line.  ^G restores the line search began with.  Enter accepts the
// match.  Any other key ends the search on the match and then acts as it does
// in state edit.
static void OnKeySearch(lib_rl::Key key) {
    bool text = key.value >= 0x20 && key.value < 0x100 && key.value != lib_rl_Key_backspace;
    if (text) {
        ch_Alloc(lib_rl::_db.search_str) = char(key.value);
        SearchFrom(lib_rl::_db.search_pos >= 0 ? lib_rl::_db.search_pos : lib_rl::hist_N() - 1);
    } else if (key.value == lib_rl_Key_ctrl_r) {
        SearchFrom(lib_rl::_db.search_pos - 1);
    } else if (key.value == lib_rl_Key_backspace || key.value == lib_rl_Key_ctrl_h) {
        int n = ch_N(lib_rl::_db.search_str);
        while (n > 0 && (u8(lib_rl::_db.search_str.ch_elems[n - 1]) & 0xc0) == 0x80) {
            n--;
        }
        lib_rl::_db.search_str.ch_n = i32_Max(n - 1, 0);
        lib_rl::_db.search_pos = -1;
        SearchFrom(lib_rl::hist_N() - 1);
    } else if (key.value == lib_rl_Key_ctrl_g) {
        Replace(0, ch_N(lib_rl::_db.edit), lib_rl::_db.search_edit, false);
        lib_rl::_db.search_pos = -1;
        SetState(lib_rl::State(lib_rl_State_edit));
    } else {
        SetState(lib_rl::State(lib_rl_State_edit));
        OnKeyEdit(key);
    }
}

//------------------------------------------------------------------------------

// Apply KEY in the current state.
static void OnKey(lib_rl::Key key) {
    if (lib_rl::_db.state.value == lib_rl_State_search) {
        OnKeySearch(key);
    } else if (lib_rl::_db.state.value == lib_rl_State_edit) {
        OnKeyEdit(key);
    }
}

//------------------------------------------------------------------------------

// Return the key that ends escape sequence ESC [ ARG FINAL or ESC O FINAL: the
// arrows, Home, End and Delete, and Ctrl with an arrow for a word.
static lib_rl::Key DecodeSeq(strptr arg, char final) {
    lib_rl::Key ret(lib_rl_Key_none);
    bool ctrl = arg == "1;5";
    switch (final) {
    case 'A':
        ret.value = lib_rl_Key_up;
        break;
    case 'B':
        ret.value = lib_rl_Key_down;
        break;
    case 'C':
        ret.value = ctrl ? lib_rl_Key_word_right : lib_rl_Key_right;
        break;
    case 'D':
        ret.value = ctrl ? lib_rl_Key_word_left : lib_rl_Key_left;
        break;
    case 'H':
        ret.value = lib_rl_Key_home;
        break;
    case 'F':
        ret.value = lib_rl_Key_end;
        break;
    case '~':
        if (arg == "1" || arg == "7") {
            ret.value = lib_rl_Key_home;
        } else if (arg == "4" || arg == "8") {
            ret.value = lib_rl_Key_end;
        } else if (arg == "3") {
            ret.value = lib_rl_Key_delete;
        }
        break;
    default:
        break;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Feed byte C, read from the terminal, to the key decoder, and apply the key it
// completes in the current state.  A byte that leaves the key unfinished is
// kept in _db.keystate and _db.keyarg.  A key that ends the line sets
// LineValidQ.
void lib_rl::ApplyByte(u8 c) {
    lib_rl::Key key(lib_rl_Key_none);
    bool done = false;
    switch (lib_rl::_db.keystate.value) {
    case lib_rl_Keystate_plain:
        if (c == lib_rl_Key_esc) {
            lib_rl::_db.keystate.value = lib_rl_Keystate_esc;
        } else {
            key.value = c;
            done = true;
        }
        break;
    case lib_rl_Keystate_esc:
        lib_rl::_db.keystate.value = lib_rl_Keystate_plain;
        if (c == '[') {
            lib_rl::_db.keystate.value = lib_rl_Keystate_csi;
            ch_RemoveAll(lib_rl::_db.keyarg);
        } else if (c == 'O') {
            lib_rl::_db.keystate.value = lib_rl_Keystate_ss3;
        } else {
            key.value = c == 'b' ? lib_rl_Key_word_left
                : c == 'f' ? lib_rl_Key_word_right
                : c == 'd' ? lib_rl_Key_kill_word
                : c == lib_rl_Key_backspace ? lib_rl_Key_ctrl_w
                : lib_rl_Key_none;
            done = true;
        }
        break;
    case lib_rl_Keystate_csi:
        if (c >= 0x40 && c <= 0x7e) {
            lib_rl::_db.keystate.value = lib_rl_Keystate_plain;
            key = DecodeSeq(lib_rl::_db.keyarg, c);
            done = true;
        } else if (ch_N(lib_rl::_db.keyarg) < 16) {
            ch_Alloc(lib_rl::_db.keyarg) = c;
        }
        break;
    case lib_rl_Keystate_ss3:
        lib_rl::_db.keystate.value = lib_rl_Keystate_plain;
        key = DecodeSeq("", c);
        done = true;
        break;
    default:
        break;
    }
    if (done) {
        OnKey(key);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                ENGINE
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

// True while a key means something other than itself: a search is on, or the
// decoder is inside a multi-byte key.
bool lib_rl::SubstateQ() {
    return _db.state.value == lib_rl_State_search || _db.keystate.value != lib_rl_Keystate_plain;
}

// End a search, restoring the line it began with, and drop a half-read key.
void lib_rl::CancelSubstate() {
    _db.keystate.value = lib_rl_Keystate_plain;
    if (_db.state.value == lib_rl_State_search) {
        OnKeySearch(lib_rl::Key(lib_rl_Key_ctrl_g));
        Redisplay();
    }
}

//------------------------------------------------------------------------------

// True when the editor holds the terminal
bool lib_rl::ReadlineQ() {
    return _db.state.value != lib_rl_State_idle;
}

// -----------------------------------------------------------------------------

// Show PROMPT before the line from now on, redrawing it when the editor holds
// the terminal.
void lib_rl::SetPrompt(algo::strptr prompt) {
    _db.cmdline.prompt = prompt;
    if (ReadlineQ()) {
        Redisplay();
    }
}

//------------------------------------------------------------------------------

// switch  mode 0 - normal, 1 - readline
void lib_rl::SwitchMode(int mode) {
    if (_db.isatty) {
        if (mode && !ReadlineQ()) {
            SetState(lib_rl::State(lib_rl_State_edit));
        } else if (!mode && ReadlineQ()) {
            SetState(lib_rl::State(lib_rl_State_idle));
        }
    }
}

//------------------------------------------------------------------------------

// Read every byte that is waiting on standard input, and return the line once
// one is complete, or an empty strptr while it is not.  On a terminal the bytes
// are keys of the editor, and the line is redrawn once after them.  Otherwise a
// line ends at a newline or at the end of input.  ^D on an empty line, and the
// end of input, set EofQ.
strptr lib_rl::GetLine() {
    if (!LineValidQ() && !EofQ()) {
        SwitchMode(1);
    }
    bool more = true;
    while (more && !LineValidQ() && !EofQ()) {
        int c = getc(stdin);
        bool empty = ch_N(_db.edit) == 0 && !SubstateQ();
        if (c == EOF) {
            _db.err = errno != EAGAIN && errno != EINTR ? errno : 0;
            _db.eof = feof(stdin) || _db.err;
            more = false;
        } else if (_db.isatty && c == lib_rl_Key_ctrl_d && empty) {
            // Enter echoes a newline and ^D does not, so end the line here;
            // otherwise the teardown clears the row the prompt stands on.  A
            // zero-width prompt has drawn nothing.
            if (ch_N(_db.cmdline.prompt)) {
                Finish("");
            }
            _db.eof = true;
        } else if (_db.isatty) {
            ApplyByte(u8(c));
        } else if (c == '\n') {
            _db.line_valid = true;
        } else {
            ch_Alloc(_db.line) = c;
        }
    }
    if (!_db.isatty && _db.eof && ch_N(_db.line)) {
        _db.line_valid = true;
    }
    if (ReadlineQ() && !LineValidQ() && !EofQ()) {
        Redisplay();
    }
    if (LineValidQ() || EofQ()) {
        SwitchMode(0);
    }
    return LineValidQ() ? strptr(_db.line) : strptr();
}

//------------------------------------------------------------------------------

// Skip current line
void lib_rl::SkipLine() {
    _db.line_valid = false;
    ch_RemoveAll(_db.line);
}

//------------------------------------------------------------------------------

// Abandon the line being typed.  On the screen it stays, marked ^C, and the
// prompt starts again on the row below with an empty line.
void lib_rl::KillBuffer() {
    SkipLine();
    if (ReadlineQ()) {
        CancelSubstate();
        Finish("^C");
    }
    ch_RemoveAll(_db.edit);
    _db.point = 0;
    _db.hist_pos = hist_N();
    if (ReadlineQ()) {
        Redisplay();
    }
}

//------------------------------------------------------------------------------

// The line as it stands in the editor, which an app reads to work ahead of a
// completion it has not been asked for yet.
strptr lib_rl::CurLine() {
    return _db.edit;
}

//------------------------------------------------------------------------------

// True when the word starting at BEG is LINE's first, which is the only
// position that names a command.
bool lib_rl::FirstWordQ(strptr line, int beg) {
    bool ret = true;
    for (int i = 0; i < beg && i < ch_N(line); i++) {
        if (line.elems[i] != ' ') {
            ret = false;
        }
    }
    return ret;
}

// The last blank-separated word of LINE, which is the one a completion acts on.
strptr lib_rl::LastWord(strptr line) {
    strptr ret = line;
    for (int i = 0; i < ch_N(line); i++) {
        if (line.elems[i] == ' ') {
            ret = ch_RestFrom(line, i + 1);
        }
    }
    return ret;
}

// The directory part of WORD -- everything before its last '/', with the root
// spelled "/" -- and empty when the word names no directory.
strptr lib_rl::WordDir(strptr word) {
    strptr ret;
    int slash = -1;
    for (int i = 0; i < ch_N(word); i++) {
        if (word.elems[i] == '/') {
            slash = i;
        }
    }
    if (slash == 0) {
        ret = ch_FirstN(word, 1);
    } else if (slash > 0) {
        ret = ch_FirstN(word, slash);
    }
    return ret;
}

// The name part of WORD -- everything after its last '/'.
strptr lib_rl::WordName(strptr word) {
    strptr ret = word;
    for (int i = 0; i < ch_N(word); i++) {
        if (word.elems[i] == '/') {
            ret = ch_RestFrom(word, i + 1);
        }
    }
    return ret;
}

//------------------------------------------------------------------------------

// Offer WORD as a candidate for the completion query in flight.  Call only
// from the h_compl hook, once per word that matches what the user typed.
void lib_rl::AddCandidate(strptr word) {
    cand_Alloc() = word;
}

//------------------------------------------------------------------------------

// The longest prefix every offered candidate shares, empty when none was
// offered.  Against `part`, `passwd` and `path` that is `pa`; against one
// candidate it is that word, which is how a unique match completes outright.
tempstr lib_rl::CommonPrefix() {
    tempstr ret;
    bool first = true;
    ind_beg(lib_rl::_db_cand_curs, cand, lib_rl::_db) {
        if (first) {
            ret = cand;
            first = false;
        } else {
            int n = 0;
            while (n < ch_N(ret) && n < ch_N(cand) && ret.ch_elems[n] == cand.ch_elems[n]) {
                n++;
            }
            ret.ch_n = n;
        }
    }ind_end;
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//
//                                SETUP/TEARDOWN
//
////////////////////////////////////////////////////////////////////////////////

// Begin reading lines for application APP with prompt PROMPT: load the
// history, register standard input with the event loop, and, on a terminal,
// draw the prompt.  The app sets _db.iohook.callback to learn that input is
// waiting, and calls GetLine from a step to read it.
void lib_rl::BeginReadline(strptr app, strptr prompt) {
    if (!ValidQ(_db.iohook.fildes)) {
        _db.cmdline.app = app;
        _db.cmdline.prompt = prompt;
        _db.isatty = isatty(0);
        InitHistory();
        _db.iohook.fildes = algo::Fildes(0);
        algo::SetBlockingMode(_db.iohook.fildes, false);
        IOEvtFlags flags;
        read_Set(flags, true);
        algo_lib::IohookAdd(_db.iohook, flags);
        SwitchMode(1);
    }
}

//------------------------------------------------------------------------------

// Stop reading lines, and give the terminal back the modes it had.
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

//------------------------------------------------------------------------------

// flush unread input, so it will not go to shell
void lib_rl::FlushInput() {
    while (getc(stdin) != EOF) {
    }
}
