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
// Target: acr_nav (exe) -- TUI schema explorer for browsing ctypes, fields, and cross-references
// Exceptions: yes
// Source: cpp/acr_nav/acr_nav.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"
#include <termios.h>
#include <poll.h>

static struct termios acr_nav_orig_termios;
static bool acr_nav_raw_mode = false;

static void WriteStdout(const char *buf, int len) {
    ssize_t rc = write(STDOUT_FILENO, buf, len);
    (void)rc;
}

// -----------------------------------------------------------------------------

static void ExitRawMode() {
    if (acr_nav_raw_mode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &acr_nav_orig_termios);
        acr_nav_raw_mode = false;
        WriteStdout("\x1b[2J\x1b[H\x1b[?25h\x1b[0m", 17);
    }
}

// -----------------------------------------------------------------------------

static void EnterRawMode() {
    tcgetattr(STDIN_FILENO, &acr_nav_orig_termios);
    atexit(ExitRawMode);
    struct termios raw = acr_nav_orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    acr_nav_raw_mode = true;
    WriteStdout("\x1b[2J\x1b[H\x1b[?25l", 13);
}

// -----------------------------------------------------------------------------

static void DetectTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    acr_nav::_db.term_wid = w.ws_col;
    acr_nav::_db.term_hei = w.ws_row;
}

// -----------------------------------------------------------------------------

static void PopulateSelCtype() {
    ind_beg(acr_nav::_db_ctype_curs, ctype, acr_nav::_db) {
        bool match = Regx_Match(acr_nav::_db.cmdline.ns, ns_Get(ctype));
        if (match) {
            zd_sel_ctype_Insert(ctype);
        }
    } ind_end;
}

// -----------------------------------------------------------------------------

static void BatchOutput() {
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        prlog(ctype.ctype << "  (" << c_field_N(ctype) << " fields)");
        ind_beg(acr_nav::ctype_c_field_curs, field, ctype) {
            tempstr out;
            out << "  " << name_Get(field);
            char_PrintNTimes(' ', out, i32_Max(1, 24 - ch_N(out)));
            out << field.p_arg->ctype;
            char_PrintNTimes(' ', out, i32_Max(1, 52 - ch_N(out)));
            out << field.p_reftype->reftype;
            prlog(out);
        } ind_end;
    } ind_end;
    prlog("acr_nav.report"
          << Keyval("n_ctype", acr_nav::ctype_N())
          << Keyval("n_field", acr_nav::field_N())
          << Keyval("n_sel_ctype", acr_nav::zd_sel_ctype_N())
          << Keyval("n_keybind", acr_nav::keybind_N())
          << Keyval("n_panel", acr_nav::panel_N())
          << Keyval("n_navaction", acr_nav::navaction_N())
          << Keyval("n_navmode", acr_nav::navmode_N()));
}

// -----------------------------------------------------------------------------

static bool ByteAvailable() {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    return poll(&pfd, 1, 50) > 0;
}

// -----------------------------------------------------------------------------

static tempstr ReadKeyName() {
    tempstr ret;
    char c = 0;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) {
        acr_nav::_db.running = false;
    } else if (c == 3) {
        acr_nav::_db.running = false;
    } else if (c == '\x1b') {
        if (!ByteAvailable()) {
            ret = "Escape";
        } else {
            char seq[4];
            ssize_t nr;
            seq[0] = 0;
            seq[1] = 0;
            nr = read(STDIN_FILENO, &seq[0], 1);
            (void)nr;
            if (seq[0] == '[') {
                nr = read(STDIN_FILENO, &seq[1], 1);
                (void)nr;
                if (seq[1] == 'A') {
                    ret = "Up";
                } else if (seq[1] == 'B') {
                    ret = "Down";
                } else if (seq[1] == 'C') {
                    ret = "Right";
                } else if (seq[1] == 'D') {
                    ret = "Left";
                } else if (seq[1] == '5') {
                    nr = read(STDIN_FILENO, &seq[2], 1);
                    (void)nr;
                    ret = "PgUp";
                } else if (seq[1] == '6') {
                    nr = read(STDIN_FILENO, &seq[2], 1);
                    (void)nr;
                    ret = "PgDown";
                }
            }
        }
    } else if (c == '\r' || c == '\n') {
        ret = "Enter";
    } else if (c == 127 || c == 8) {
        ret = "Backspace";
    } else if (c >= 32 && c < 127) {
        ret << c;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void DispatchAction(strptr action) {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int n_items = acr_nav::zd_sel_ctype_N();
    int last = i32_Max(0, n_items - 1);
    if (action == "move_up") {
        panel.sel_row = i32_Max(0, panel.sel_row - 1);
    } else if (action == "move_down") {
        panel.sel_row = i32_Min(last, panel.sel_row + 1);
    } else if (action == "page_up") {
        int page = acr_nav::_db.term_hei - 2;
        panel.sel_row = i32_Max(0, panel.sel_row - page);
    } else if (action == "page_down") {
        int page = acr_nav::_db.term_hei - 2;
        panel.sel_row = i32_Min(last, panel.sel_row + page);
    } else if (action == "quit") {
        acr_nav::_db.running = false;
    }
}

// -----------------------------------------------------------------------------

static void AdjustScroll() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int visible = i32_Max(1, acr_nav::_db.term_hei - 2);
    if (panel.sel_row >= panel.scroll_offset + visible) {
        panel.scroll_offset = panel.sel_row - visible + 1;
    }
    if (panel.sel_row < panel.scroll_offset) {
        panel.scroll_offset = panel.sel_row;
    }
}

// -----------------------------------------------------------------------------

static void Render() {
    tempstr buf;
    buf << "\x1b[H";
    int wid = acr_nav::_db.term_wid;
    int visible = acr_nav::_db.term_hei - 2;
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;

    // Title bar
    buf << "\x1b[7m";
    tempstr title;
    title << " " << panel.title << " (" << acr_nav::zd_sel_ctype_N() << ")";
    char_PrintNTimes(' ', title, i32_Max(0, wid - ch_N(title)));
    buf << title << "\x1b[0m\r\n";

    // Content rows
    int idx = 0;
    int row = 0;
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        if (idx >= panel.scroll_offset && row < visible) {
            bool selected = (idx == panel.sel_row);
            if (selected) {
                buf << "\x1b[7m";
            }
            tempstr line;
            line << " " << ctype.ctype;
            char_PrintNTimes(' ', line, i32_Max(0, wid - ch_N(line)));
            buf << line;
            if (selected) {
                buf << "\x1b[0m";
            }
            buf << "\x1b[K\r\n";
            row++;
        }
        idx++;
    } ind_end;

    // Fill empty rows
    while (row < visible) {
        buf << "\x1b[K\r\n";
        row++;
    }

    // Status bar
    buf << "\x1b[7m";
    tempstr status;
    status << " q:quit  Up/Dn:navigate  PgUp/PgDn:page";
    tempstr pos;
    pos << (panel.sel_row + 1) << "/" << acr_nav::zd_sel_ctype_N();
    char_PrintNTimes(' ', status, i32_Max(1, wid - ch_N(status) - ch_N(pos)));
    status << pos;
    buf << status << "\x1b[0m";

    WriteStdout(buf.ch_elems, ch_N(buf));
}

// -----------------------------------------------------------------------------

void acr_nav::Main() {
    PopulateSelCtype();
    bool is_tty = isatty(STDOUT_FILENO);
    if (!is_tty) {
        BatchOutput();
    } else {
        DetectTerminal();
        EnterRawMode();
        _db.p_cur_panel = ind_panel_Find("ctype_list");
        vrfy(_db.p_cur_panel, "panel 'ctype_list' not found");
        _db.p_cur_mode = ind_navmode_Find("browse");
        vrfy(_db.p_cur_mode, "navmode 'browse' not found");
        _db.p_cur_panel->sel_row = 0;
        _db.p_cur_panel->scroll_offset = 0;
        Render();
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            acr_nav::FKeybind *keybind = ind_keybind_Find(key_name);
            if (keybind) {
                DispatchAction(keybind->p_navaction->navaction);
                AdjustScroll();
                Render();
            }
        }
        ExitRawMode();
    }
}
