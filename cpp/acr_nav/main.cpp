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
// Source: cpp/acr_nav/main.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"
#include <termios.h>
#include <poll.h>
#include <signal.h>

using acr_nav::WriteStdout;
using acr_nav::DetectTerminal;
using acr_nav::SelectedCtype;
using acr_nav::InvalidateContentCaches;
using acr_nav::AdjustScroll;
using acr_nav::RightPanelItemCount;
using acr_nav::DataRows;
using acr_nav::BuildLeftItemsReset;
using acr_nav::BuildLeftItems;
using acr_nav::PopViewmode;
using acr_nav::DismissStartupHelp;
using acr_nav::PopOverlayOnCtypeChange;
using acr_nav::PushOverlay;
using acr_nav::GraphFindCtypeLine;
using acr_nav::SwitchToBrowse;
using acr_nav::BuildHelpLines;
using acr_nav::PreviewEnsureContent;
using acr_nav::CodegenEnsureContent;
using acr_nav::NsDepEnsureContent;
using acr_nav::GraphEnsureContent;
using acr_nav::Render;
using acr_nav::RightPanelFieldFind;
using acr_nav::FieldMatchesFilter;
using acr_nav::RightPanelLineFind;
using acr_nav::BuildBreadcrumb;
using acr_nav::BuildStatusHint;

static struct termios acr_nav_orig_termios;
static bool acr_nav_raw_mode = false;
static volatile sig_atomic_t acr_nav_sigwinch = 0;

static void SigwinchHandler(int) {
    acr_nav_sigwinch = 1;
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

static bool ByteAvailable() {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    return poll(&pfd, 1, 50) > 0;
}

// -----------------------------------------------------------------------------

// VT100 byte-to-keyname mapping. Looks like a missing noun but
// these are terminal protocol constants, not application data.
// The data-driven boundary is one layer up: acr_navdb.keybind.
static tempstr ReadKeyName() {
    tempstr ret;
    char c = 0;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0 && errno == EINTR) {
        // interrupted by signal (e.g. SIGWINCH) — return empty key, caller repaints
    } else if (n <= 0) {
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
                } else if (seq[1] == 'H') {
                    ret = "Home";
                } else if (seq[1] == 'F') {
                    ret = "End";
                }
            }
        }
    } else if (c == '\r' || c == '\n') {
        ret = "Enter";
    } else if (c == 127 || c == 8) {
        ret = "Backspace";
    } else if (c == 9) {
        ret = "Tab";
    } else if (c == ' ') {
        ret = "Space";
    } else if (c >= 1 && c <= 26 && c != 3 && c != 8 && c != 9 && c != 10 && c != 13) {
        ret << "Ctrl-";
        ret << char('A' + c - 1);
    } else if (c > 32 && c < 127) {
        ret << c;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void InitPanels() {
    acr_nav::_db.p_left_panel = acr_nav::ind_panel_Find("ctype_list");
    acr_nav::_db.p_right_panel = acr_nav::ind_panel_Find("content");
    vrfy(acr_nav::_db.p_left_panel, "panel 'ctype_list' not found");
    vrfy(acr_nav::_db.p_right_panel, "panel 'content' not found");
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
    acr_nav::_db.p_filter_mode = acr_nav::ind_navmode_Find("filter");
    vrfy(acr_nav::_db.p_filter_mode, "navmode 'filter' not found");
    // Resolve well-known viewmode pointers by name
    acr_nav::_db.p_default_viewmode = acr_nav::ind_viewmode_Find("fields");
    acr_nav::_db.p_preview_viewmode = acr_nav::ind_viewmode_Find("preview");
    acr_nav::_db.p_help_viewmode = acr_nav::ind_viewmode_Find("help");
    acr_nav::_db.p_detail_viewmode = acr_nav::ind_viewmode_Find("detail");
    acr_nav::_db.p_codegen_viewmode = acr_nav::ind_viewmode_Find("codegen");
    acr_nav::_db.p_nsdep_viewmode = acr_nav::ind_viewmode_Find("nsdep");
    acr_nav::_db.p_xref_viewmode = acr_nav::ind_viewmode_Find("xref");
    acr_nav::_db.p_graph_viewmode = acr_nav::ind_viewmode_Find("graph");
    vrfy(acr_nav::_db.p_default_viewmode, "viewmode 'fields' not found");
    vrfy(acr_nav::_db.p_preview_viewmode, "viewmode 'preview' not found");
    vrfy(acr_nav::_db.p_help_viewmode, "viewmode 'help' not found");
    vrfy(acr_nav::_db.p_detail_viewmode, "viewmode 'detail' not found");
    vrfy(acr_nav::_db.p_codegen_viewmode, "viewmode 'codegen' not found");
    vrfy(acr_nav::_db.p_nsdep_viewmode, "viewmode 'nsdep' not found");
    vrfy(acr_nav::_db.p_xref_viewmode, "viewmode 'xref' not found");
    vrfy(acr_nav::_db.p_graph_viewmode, "viewmode 'graph' not found");
    // Set ensure-content hooks for lazy-loading viewmodes
    acr_nav::_db.p_preview_viewmode->ensure_content = PreviewEnsureContent;
    acr_nav::_db.p_codegen_viewmode->ensure_content = CodegenEnsureContent;
    acr_nav::_db.p_nsdep_viewmode->ensure_content = NsDepEnsureContent;
    acr_nav::_db.p_graph_viewmode->ensure_content = GraphEnsureContent;
    // Resolve navaction -> helpgroup pointers (Ptr, not Upptr: 6 navactions have empty helpgroup)
    ind_beg(acr_nav::_db_navaction_curs, na, acr_nav::_db) {
        if (ch_N(na.helpgroup) > 0) {
            na.p_helpgroup = acr_nav::ind_helpgroup_Find(na.helpgroup);
        }
    } ind_end;
    // Start in help mode so new users see keybindings
    acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_default_viewmode;
    PushOverlay(acr_nav::_db.p_help_viewmode);
    acr_nav::_db.startup_help = true;
    // Resolve well-known navstyle pointers by name
    acr_nav::_db.p_title_focus = acr_nav::ind_navstyle_Find("title_focus");
    acr_nav::_db.p_title_nofocus = acr_nav::ind_navstyle_Find("title_nofocus");
    acr_nav::_db.p_sel_focus = acr_nav::ind_navstyle_Find("sel_focus");
    acr_nav::_db.p_sel_nofocus = acr_nav::ind_navstyle_Find("sel_nofocus");
    acr_nav::_db.p_statusbar = acr_nav::ind_navstyle_Find("statusbar");
    vrfy(acr_nav::_db.p_title_focus && acr_nav::_db.p_title_nofocus
         && acr_nav::_db.p_sel_focus && acr_nav::_db.p_sel_nofocus
         && acr_nav::_db.p_statusbar, "required navstyle records missing");
    // Resolve well-known filtertarget pointers
    acr_nav::_db.p_default_filtertarget = acr_nav::ind_filtertarget_Find("ctype");
    vrfy(acr_nav::_db.p_default_filtertarget, "filtertarget 'ctype' not found");
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    acr_nav::_db.p_filter_match = acr_nav::ind_navstyle_Find("filter_match");
    vrfy(acr_nav::_db.p_filter_match, "navstyle 'filter_match' not found");
    // Syntax highlight styles (optional -- null degrades to no highlighting via AddSpan guard)
    acr_nav::_db.p_line_comment = acr_nav::ind_navstyle_Find("line_comment");
    acr_nav::_db.p_line_keyword = acr_nav::ind_navstyle_Find("line_keyword");
    acr_nav::_db.p_line_string = acr_nav::ind_navstyle_Find("line_string");
    acr_nav::_db.p_line_preproc = acr_nav::ind_navstyle_Find("line_preproc");
    acr_nav::_db.p_line_section = acr_nav::ind_navstyle_Find("line_section");
    acr_nav::_db.p_line_key = acr_nav::ind_navstyle_Find("line_key");
    // Graph view styles (optional)
    acr_nav::_db.p_graph_ctype_style = acr_nav::ind_navstyle_Find("graph_ctype");
    acr_nav::_db.p_graph_neighbor = acr_nav::ind_navstyle_Find("graph_neighbor");
    acr_nav::_db.p_graph_arrow = acr_nav::ind_navstyle_Find("graph_arrow");
    // Preview follow-ref styles (optional)
    acr_nav::_db.p_line_nav_header = acr_nav::ind_navstyle_Find("line_nav_header");
    acr_nav::_db.p_line_nav_cell = acr_nav::ind_navstyle_Find("line_nav_cell");
    acr_nav::_db.p_line_nav_cell_nofk = acr_nav::ind_navstyle_Find("line_nav_cell_nofk");
    BuildHelpLines();
    SwitchToBrowse();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

static bool ProcessKey(algo::strptr key_name) {
    bool did_something = false;
    {
        acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
        acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
        acr_nav::FCtype *prev_sel_ct = SelectedCtype(*left);
        acr_nav::FViewmode *prev_viewmode = acr_nav::_db.p_cur_viewmode;
        int prev_depth = acr_nav::navstack_N();
        int prev_overlay_depth = acr_nav::viewmode_stack_N();
        tempstr composite;
        composite << acr_nav::_db.p_cur_mode->navmode << "." << key_name;
        acr_nav::FKeybind *keybind = acr_nav::ind_keybind_Find(composite);
        if (keybind) {
            acr_nav::FNavaction &na = *keybind->p_navaction;
            bool is_overlay = acr_nav::_db.p_cur_viewmode->is_overlay;
            bool blocked = na.need_no_overlay && is_overlay;
            if (blocked && acr_nav::_db.startup_help) {
                acr_nav::_db.startup_help = false;
                PopViewmode();
                blocked = false;
            }
            if (blocked && ch_N(na.dismiss_viewmode) > 0
                && na.dismiss_viewmode == acr_nav::_db.p_cur_viewmode->viewmode) {
                blocked = false;
            }
            if (!blocked) {
                step_Call(na);
                did_something = true;
            }
        }
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (!keybind && in_filter && elems_N(key_name) == 1 && key_name[0] > 32) {
            acr_nav::_db.filter << key_name;
            BuildLeftItemsReset();
            did_something = true;
        }
        if (did_something) {
            DismissStartupHelp(keybind);
            acr_nav::FCtype *sel_ct = SelectedCtype(*left);
            bool nsdep_ns_changed = PopOverlayOnCtypeChange(prev_sel_ct, sel_ct);
            bool forward = (acr_nav::navstack_N() >= prev_depth);
            bool vm_changed = (acr_nav::_db.p_cur_viewmode != prev_viewmode);
            bool ct_changed = (sel_ct != prev_sel_ct) || nsdep_ns_changed;
            if (ct_changed && !sel_ct) {
                InvalidateContentCaches();
            }
            bool overlay_pop = vm_changed && (acr_nav::viewmode_stack_N() < prev_overlay_depth);
            if (forward && ((vm_changed && !overlay_pop) || ct_changed)) {
                right->sel_row = 0;
                right->scroll_offset = 0;
            }
            if (forward && ct_changed
                && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_graph_viewmode
                && sel_ct && prev_sel_ct && sel_ct != prev_sel_ct) {
                int line = GraphFindCtypeLine(*sel_ct, prev_sel_ct);
                if (line >= 0) {
                    right->sel_row = line;
                }
            }
            AdjustScroll(*left, acr_nav::left_item_N());
            AdjustScroll(*right, RightPanelItemCount(sel_ct));
        }
    }
    return did_something;
}

// -----------------------------------------------------------------------------

static void HeadlessOutput() {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    // Screen record
    acr_nav::Screen screen;
    screen.mode = acr_nav::_db.p_cur_mode->navmode;
    screen.focus = acr_nav::_db.p_cur_panel->panel;
    screen.filter = acr_nav::_db.filter;
    screen.navstack_depth = acr_nav::navstack_N();
    screen.n_sel_ctype = acr_nav::_db.n_visible_ctype;
    screen.n_ctype = acr_nav::ctype_N();
    screen.n_field = acr_nav::field_N();
    screen.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    screen.sel_nav_col = acr_nav::_db.sel_nav_col;
    screen.breadcrumb = BuildBreadcrumb(sel_ct);
    screen.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
    BuildStatusHint(screen.hints);
    prlog(screen);
    // Left panel state
    acr_nav::PanelState left_state;
    left_state.panel = left->panel;
    left_state.sel_row = left->sel_row;
    left_state.scroll_offset = left->scroll_offset;
    left_state.n_items = acr_nav::left_item_N();
    left_state.sel_value = "";
    if (left->sel_row >= 0 && left->sel_row < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(left->sel_row);
        if (ch_N(item.ctype) > 0) {
            left_state.sel_value = item.ctype;
        } else {
            left_state.sel_value = item.ns;
        }
    }
    prlog(left_state);
    // Left panel items (clipped to viewport)
    int first_left = left->scroll_offset;
    int last_left = i32_Min(first_left + DataRows(), acr_nav::left_item_N());
    for (int i = first_left; i < last_left; i++) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(i);
        acr_nav::VisibleLeftItem vli;
        vli.row = i;
        if (ch_N(item.ctype) > 0) {
            vli.value = item.ctype;
            vli.kind = "ctype";
            vli.collapsed = false;
            vli.n_match = 0;
            acr_nav::FCtype *ct = acr_nav::ind_ctype_Find(item.ctype);
            vli.n_record = (ct && ct->c_ssimfile) ? ct->c_ssimfile->n_record : 0;
        } else {
            vli.value = item.ns;
            vli.kind = "ns";
            acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
            vli.collapsed = ns ? ns->collapsed : false;
            vli.n_match = ns ? ns->n_match : 0;
            vli.n_record = 0;
        }
        prlog(vli);
    }
    // Right panel state
    acr_nav::PanelState right_state;
    right_state.panel = right->panel;
    right_state.sel_row = right->sel_row;
    right_state.scroll_offset = right->scroll_offset;
    right_state.n_items = RightPanelItemCount(sel_ct);
    right_state.sel_value = "";
    if (right->sel_row < RightPanelItemCount(sel_ct)) {
        if (!acr_nav::_db.p_cur_viewmode->has_fields) {
            right_state.sel_value = RightPanelLineFind(right->sel_row);
        } else if (sel_ct) {
            acr_nav::FField *fld = RightPanelFieldFind(sel_ct, right->sel_row);
            if (fld) {
                right_state.sel_value = fld->field;
            }
        }
    }
    prlog(right_state);
    // Visible fields (field-based modes only, clipped to viewport)
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        int n_vis = RightPanelItemCount(sel_ct);
        int first_right = right->scroll_offset;
        int last_right = i32_Min(first_right + DataRows(), n_vis);
        for (int i = first_right; i < last_right; i++) {
            acr_nav::FField *field = RightPanelFieldFind(sel_ct, i);
            if (field) {
                acr_nav::VisibleField vf;
                vf.row = i;
                vf.field = field->field;
                vf.arg = reverse ? field->p_ctype->ctype : field->p_arg->ctype;
                vf.reftype = field->p_reftype->reftype;
                if (field->p_reftype->c_reftypestyle) {
                    vf.style = field->p_reftype->c_reftypestyle->p_navstyle->navstyle;
                }
                bool navigable = reverse
                    ? (field->p_ctype != sel_ct)
                    : (field->p_arg != sel_ct);
                vf.navigable = navigable;
                bool field_match = false;
                if (!reverse
                    && acr_nav::_db.p_cur_filtertarget->has_field_criteria
                    && ch_N(acr_nav::_db.filter) > 0) {
                    field_match = FieldMatchesFilter(*field, acr_nav::_db.filter_regx, *acr_nav::_db.p_cur_filtertarget);
                }
                vf.match = field_match;
                prlog(vf);
            }
        }
    } else {
        // Visible lines (text-based modes: help, preview, detail, clipped to viewport)
        int n_lines = RightPanelItemCount(sel_ct);
        int first_line = right->scroll_offset;
        int last_line = i32_Min(first_line + DataRows(), n_lines);
        for (int i = first_line; i < last_line; i++) {
            acr_nav::VisibleLine vl;
            vl.row = i;
            vl.value = RightPanelLineFind(i);
            prlog(vl);
        }
    }
    // Blank line terminates screenshot block
    prlog("");
}

// -----------------------------------------------------------------------------

static void HeadlessMain() {
    InitPanels();
    acr_nav::_db.term_hei = 100000;
    acr_nav::_db.running = true;
    algo::LineBuf linebuf;
    bool eof = false;
    int lineno = 0;
    while (acr_nav::_db.running && !eof) {
        char buf[4096];
        ssize_t nr = read(STDIN_FILENO, buf, sizeof(buf));
        eof = (nr <= 0);
        algo::LinebufBegin(linebuf, algo::memptr((u8*)buf, eof ? 0 : nr), eof);
        algo::strptr line;
        while (algo::LinebufNext(linebuf, line)) {
            ++lineno;
            acr_nav::SendKey send_key;
            acr_nav::Screenshot screenshot;
            acr_nav::SetTermSize set_term_size;
            if (elems_N(line) == 0) {
                // empty lines are ssim separators, not errors
            } else if (acr_nav::SendKey_ReadStrptrMaybe(send_key, line)) {
                ProcessKey(send_key.key);
            } else if (acr_nav::Screenshot_ReadStrptrMaybe(screenshot, line)) {
                HeadlessOutput();
            } else if (acr_nav::SetTermSize_ReadStrptrMaybe(set_term_size, line)) {
                acr_nav::_db.term_hei = i32_Max(1, set_term_size.term_hei);
                acr_nav::_db.term_wid = i32_Max(1, set_term_size.term_wid);
                acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
                acr_nav::FCtype *sel_ct = SelectedCtype(*left);
                AdjustScroll(*left, acr_nav::left_item_N());
                AdjustScroll(*acr_nav::_db.p_right_panel, RightPanelItemCount(sel_ct));
            } else {
                acr_nav::InputError err;
                err.lineno = lineno;
                err.msg << "unrecognized input: " << line;
                prlog(err);
            }
        }
    }
    HeadlessOutput();
}

// -----------------------------------------------------------------------------

static void CountSsimfileRecords() {
    ind_beg(acr_nav::_db_ssimfile_curs, ssimfile, acr_nav::_db) {
        tempstr fname(SsimFname(acr_nav::_db.cmdline.in, ssimfile.ssimfile));
        ind_beg(algo::FileLine_curs, line, fname) {
            (void)line;
            ssimfile.n_record++;
        } ind_end;
    } ind_end;
}

// -----------------------------------------------------------------------------

void acr_nav::Main() {
    CountSsimfileRecords();
    BuildLeftItems();
    bool headless = _db.cmdline.headless || !isatty(STDOUT_FILENO);
    if (headless) {
        HeadlessMain();
    } else {
        InitPanels();
        DetectTerminal();
        EnterRawMode();
        struct sigaction sa;
        sa.sa_handler = SigwinchHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0; // no SA_RESTART — read() must return EINTR
        sigaction(SIGWINCH, &sa, NULL);
        acr_nav::FCtype *sel_ct = SelectedCtype(*_db.p_left_panel);
        DetectTerminal();
        {
            cstring render_buf;
            Render(render_buf, sel_ct);
            WriteStdout(render_buf.ch_elems, ch_N(render_buf));
        }
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            bool repaint = acr_nav_sigwinch;
            acr_nav_sigwinch = 0;
            repaint = ProcessKey(key_name) || repaint;
            if (repaint) {
                sel_ct = SelectedCtype(*_db.p_left_panel);
                DetectTerminal();
                cstring render_buf;
                Render(render_buf, sel_ct);
                WriteStdout(render_buf.ch_elems, ch_N(render_buf));
            }
        }
        sa.sa_handler = SIG_DFL;
        sigaction(SIGWINCH, &sa, NULL);
        ExitRawMode();
    }
}
