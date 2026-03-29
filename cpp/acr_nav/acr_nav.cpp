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

static void SwitchToBrowse() {
    acr_nav::_db.p_cur_mode = acr_nav::ind_navmode_Find("browse");
    vrfy(acr_nav::_db.p_cur_mode, "navmode 'browse' not found");
}

// -----------------------------------------------------------------------------

static void ApplyFilter() {
    acr_nav::zd_sel_ctype_RemoveAll();
    algo_lib::Regx filter_regx;
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        tempstr pattern;
        pattern << "%" << acr_nav::_db.filter << "%";
        algo::MakeLower(pattern);
        algo_lib::Regx_ReadSql(filter_regx, pattern, false);
    }
    ind_beg(acr_nav::_db_ctype_curs, ctype, acr_nav::_db) {
        tempstr lower_name(ctype.ctype);
        algo::MakeLower(lower_name);
        bool filter_match = !has_filter || algo_lib::Regx_Match(filter_regx, lower_name);
        if (ch_N(ctype.ctype) > 0 && filter_match) {
            acr_nav::zd_sel_ctype_Insert(ctype);
        }
    } ind_end;
}

// -----------------------------------------------------------------------------

static void ApplyFilterReset() {
    ApplyFilter();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
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

static acr_nav::FCtype* SelectedCtype(acr_nav::FPanel &left) {
    acr_nav::FCtype *ret = NULL;
    int idx = 0;
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        if (idx == left.sel_row) {
            ret = &ctype;
        }
        idx++;
    } ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// 5 call sites dispatch on IsXrefMode() within the has_fields:Y path.
// Future: field-source properties on FViewmode would eliminate all 5.
static bool IsXrefMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_xref_viewmode;
}

// IsHelpMode: identity check used only for the ? toggle guard in navaction_show_help.
// IsDetailMode: identity check for d toggle guard and title bar display.
// 2 overlay viewmodes, each with distinct entry guards (help: unconditional;
// detail: requires has_fields + selected field) -- an is_overlay data property
// would not unify the navaction functions at this count. Revisit at 3 overlays.
static bool IsHelpMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_help_viewmode;
}

static bool IsDetailMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_detail_viewmode;
}

// Pop the topmost overlay viewmode from viewmode_stack, restoring p_cur_viewmode.
// Used by show_help toggle, show_detail toggle, and startup help dismiss.
static void PopViewmode() {
    if (!acr_nav::viewmode_stack_EmptyQ()) {
        acr_nav::FViewmode *prev = acr_nav::ind_viewmode_Find(acr_nav::viewmode_stack_qLast());
        acr_nav::viewmode_stack_RemoveLast();
        if (prev) {
            acr_nav::_db.p_cur_viewmode = prev;
        }
    }
}

static int RightPanelLineCount() {
    return acr_nav::line_N(*acr_nav::_db.p_cur_viewmode);
}

static algo::strptr RightPanelLineFind(int idx) {
    return acr_nav::line_qFind(*acr_nav::_db.p_cur_viewmode, idx);
}

static algo::strptr RightPanelLineHeader() {
    return acr_nav::_db.p_cur_viewmode->header;
}

// Find the ssimfile backing a ctype. If the ctype itself has no ssimfile,
// follow the Base field to the underlying type (e.g. abt.FTarget -> dev.Target).
static acr_nav::FSsimfile* FindSsimfile(acr_nav::FCtype &ctype) {
    acr_nav::FSsimfile *ret = ctype.c_ssimfile;
    if (!ret) {
        for (int i = 0; i < c_field_N(ctype) && !ret; i++) {
            acr_nav::FField *fld = c_field_Find(ctype, i);
            if (fld && algo::strptr(fld->p_reftype->reftype) == "Base") {
                ret = fld->p_arg->c_ssimfile;
            }
        }
    }
    return ret;
}

// Load ssimfile content into the preview viewmode's line Tary, stripping the tuple head from each line.
// Format a single row of attr values into an aligned column string.
static void FormatPreviewRow(cstring &out, algo::Tuple &tuple, int *col_wid, int n_col) {
    int ci = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        if (ci < n_col) {
            if (ci > 0) {
                out << "  ";
            }
            out << attr.value;
            char_PrintNTimes(' ', out, col_wid[ci] - ch_N(attr.value));
            ci++;
        }
    } ind_end;
    // Fill missing columns
    while (ci < n_col) {
        if (ci > 0) {
            out << "  ";
        }
        char_PrintNTimes(' ', out, col_wid[ci]);
        ci++;
    }
}

static void LoadPreview(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_preview_viewmode;
    acr_nav::line_RemoveAll(vm);
    vm.header = "";
    acr_nav::_db.p_preview_ctype = &ctype;
    acr_nav::FSsimfile *ssimfile = FindSsimfile(ctype);
    if (ssimfile) {
        tempstr path;
        path << "data/" << ssimns_Get(*ssimfile) << "/"
             << name_Get(*ssimfile) << ".ssim";
        algo_lib::MmapFile file;
        if (algo_lib::MmapFile_Load(file, path)) {
            // First pass: determine columns and widths
            int n_col = 0;
            int col_wid[64];
            algo::cstring col_name[64];
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    if (n_col == 0) {
                        ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                            if (n_col < 64) {
                                col_name[n_col] = attr.name;
                                col_wid[n_col] = ch_N(attr.name);
                                n_col++;
                            }
                        } ind_end;
                    }
                    int ci = 0;
                    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                        if (ci < n_col) {
                            col_wid[ci] = i32_Max(col_wid[ci], ch_N(attr.value));
                            ci++;
                        }
                    } ind_end;
                }
            } ind_end;
            // Build header from column names
            if (n_col > 0) {
                tempstr hdr;
                for (int c = 0; c < n_col; c++) {
                    if (c > 0) {
                        hdr << "  ";
                    }
                    hdr << col_name[c];
                    char_PrintNTimes(' ', hdr, col_wid[c] - ch_N(col_name[c]));
                }
                vm.header = hdr;
            }
            // Second pass: format data rows directly into preview_line
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    tempstr row;
                    FormatPreviewRow(row, tuple, col_wid, n_col);
                    acr_nav::line_Alloc(vm) = row;
                }
            } ind_end;
        }
    }
}

static void EnsurePreviewLoaded(acr_nav::FCtype *sel_ct) {
    if (sel_ct && acr_nav::_db.p_preview_ctype != sel_ct) {
        LoadPreview(*sel_ct);
    }
}

// Load metadata records for a single field from detailsrc ssimfiles.
// Re-serializes the dmmeta.field record as the first line, then scans each
// detailsrc file for matching records (first attribute value == field name).
static void LoadDetail(acr_nav::FField &field) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_detail_viewmode;
    acr_nav::line_RemoveAll(vm);
    acr_nav::_db.p_detail_field = &field;
    // First line: the dmmeta.field record itself
    {
        dmmeta::Field base;
        acr_nav::field_CopyOut(field, base);
        tempstr fld_line;
        dmmeta::Field_Print(base, fld_line);
        acr_nav::line_Alloc(vm) = fld_line;
    }
    // Scan each detailsrc file for matching records.
    // Path derived via Pathcomp rather than Ssimfile accessors: detailsrc uses
    // a Smallstr50 key instead of a Pkey to dmmeta.Ssimfile, avoiding ~5 schema
    // records (finput, Upptr, xref) for display-only file scanning.
    algo::strptr field_name(field.field);
    ind_beg(acr_nav::_db_detailsrc_curs, ds, acr_nav::_db) {
        algo::strptr dskey(ds.detailsrc);
        algo::strptr ns = algo::Pathcomp(dskey, ".LL");
        algo::strptr name = algo::Pathcomp(dskey, ".LR");
        tempstr path;
        path << "data/" << ns << "/" << name << ".ssim";
        algo_lib::MmapFile file;
        if (algo_lib::MmapFile_Load(file, path)) {
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    if (attrs_N(tuple) > 0 && attrs_qFind(tuple, 0).value == field_name) {
                        acr_nav::line_Alloc(vm) = line;
                    }
                }
            } ind_end;
        }
    } ind_end;
    // Set header
    tempstr hdr;
    hdr << field.field << " (" << acr_nav::line_N(vm) << " records)";
    vm.header = hdr;
}

static int RightPanelItemCount(acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (!acr_nav::_db.p_cur_viewmode->has_fields) {
        if (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode && sel_ct) {
            EnsurePreviewLoaded(sel_ct);
        }
        ret = RightPanelLineCount();
    } else if (sel_ct) {
        ret = IsXrefMode() ? c_field_arg_N(*sel_ct) : c_field_N(*sel_ct);
    }
    return ret;
}

static acr_nav::FField* RightPanelFieldFind(acr_nav::FCtype *sel_ct, int idx) {
    acr_nav::FField *ret = NULL;
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        ret = IsXrefMode() ? c_field_arg_Find(*sel_ct, idx) : c_field_Find(*sel_ct, idx);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Panel item count dispatches on position (0=ctype list, 1=field list).
// The two panels have structurally different roles (parent/child),
// not N instances of one concept -- factoring into step hooks
// would move the if elsewhere without reducing complexity.
static int PanelItemCount(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (panel.position == 0) {
        ret = acr_nav::zd_sel_ctype_N();
    } else if (panel.position == 1) {
        ret = RightPanelItemCount(sel_ct);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Number of content rows available for the dual-panel display.
// Subtracts title bar (1) and status bar (1), plus breadcrumb bar (1) when navstack is non-empty.
static int VisibleRows() {
    int chrome = 2 + (acr_nav::navstack_N() > 0 ? 1 : 0);
    return i32_Max(1, acr_nav::_db.term_hei - chrome);
}

static void AdjustScroll(acr_nav::FPanel &panel, int n_items) {
    int visible = VisibleRows();
    int last = i32_Max(0, n_items - 1);
    panel.sel_row = i32_Max(0, i32_Min(panel.sel_row, last));
    if (panel.sel_row >= panel.scroll_offset + visible) {
        panel.scroll_offset = panel.sel_row - visible + 1;
    }
    if (panel.sel_row < panel.scroll_offset) {
        panel.scroll_offset = panel.sel_row;
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_move_up() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    panel.sel_row = i32_Max(0, panel.sel_row - 1);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_move_down() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    panel.sel_row = i32_Min(last, panel.sel_row + 1);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_page_up() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int page = VisibleRows();
    panel.sel_row = i32_Max(0, panel.sel_row - page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_page_down() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    int page = VisibleRows();
    panel.sel_row = i32_Min(last, panel.sel_row + page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_switch_panel_left() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    if (panel.position > acr_nav::_db.p_left_panel->position) {
        acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_switch_panel_right() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    if (panel.position < acr_nav::_db.p_right_panel->position) {
        // Filter is a left-panel concern; leaving left panel accepts the filter
        if (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode) {
            SwitchToBrowse();
        }
        acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_follow_ref() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    if (panel.position == 0) {
        acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
    } else if (panel.position == 1 && acr_nav::_db.p_cur_viewmode->has_fields
               && sel_ct && panel.sel_row < RightPanelItemCount(sel_ct)) {
        acr_nav::FField *fld = RightPanelFieldFind(sel_ct, panel.sel_row);
        acr_nav::FCtype *target = NULL;
        if (fld) {
            target = IsXrefMode() ? fld->p_ctype : fld->p_arg;
        }
        if (fld && target != sel_ct) {
            acr_nav::Naventry &entry = acr_nav::navstack_Alloc();
            entry.filter = acr_nav::_db.filter;
            entry.navmode = acr_nav::_db.p_cur_mode->navmode;
            entry.scroll_offset = left->scroll_offset;
            entry.sel_row = left->sel_row;
            entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
            entry.ctype = sel_ct->ctype;
            // Reset to forward view on navigation -- new ctype starts in its natural view
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_default_viewmode;
            // Switch to browse mode with full ctype list so target is reachable
            acr_nav::_db.filter = "";
            SwitchToBrowse();
            ApplyFilter();
            int target_idx = 0;
            ind_beg(acr_nav::_db_zd_sel_ctype_curs, ct, acr_nav::_db) {
                if (&ct == target) {
                    left->sel_row = target_idx;
                }
                target_idx++;
            } ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_top() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    panel.sel_row = 0;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_bottom() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    panel.sel_row = last;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_back() {
    if (!acr_nav::navstack_EmptyQ()) {
        acr_nav::Naventry *entry = acr_nav::navstack_Last();
        acr_nav::_db.filter = entry->filter;
        acr_nav::FViewmode *vm = acr_nav::ind_viewmode_Find(entry->viewmode);
        if (vm) {
            acr_nav::_db.p_cur_viewmode = vm;
        }
        acr_nav::FNavmode *mode = acr_nav::ind_navmode_Find(entry->navmode);
        if (mode) {
            acr_nav::_db.p_cur_mode = mode;
        }
        ApplyFilter();
        acr_nav::_db.p_left_panel->scroll_offset = entry->scroll_offset;
        acr_nav::_db.p_left_panel->sel_row = entry->sel_row;
        acr_nav::navstack_RemoveLast();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_quit() {
    acr_nav::_db.running = false;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_xref() {
    acr_nav::FViewmode *next = acr_nav::ind_viewmode_Find(acr_nav::_db.p_cur_viewmode->next);
    if (next) {
        acr_nav::_db.p_cur_viewmode = next;
    }
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_preview() {
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    bool in_preview = (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode);
    bool go_preview = !in_preview && sel_ct && FindSsimfile(*sel_ct);
    acr_nav::_db.p_cur_viewmode = go_preview ? acr_nav::_db.p_preview_viewmode : acr_nav::_db.p_default_viewmode;
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_accept() {
    SwitchToBrowse();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_start() {
    acr_nav::_db.p_cur_mode = acr_nav::_db.p_filter_mode;
    ch_RemoveAll(acr_nav::_db.filter);
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_cancel() {
    SwitchToBrowse();
    ch_RemoveAll(acr_nav::_db.filter);
    ApplyFilterReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_append_space() {
    acr_nav::_db.filter << " ";
    ApplyFilterReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_clear() {
    ch_RemoveAll(acr_nav::_db.filter);
    ApplyFilterReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_backspace() {
    if (ch_N(acr_nav::_db.filter) > 0) {
        acr_nav::_db.filter.ch_n = ch_N(acr_nav::_db.filter) - 1;
        ApplyFilterReset();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_help() {
    if (IsHelpMode()) {
        PopViewmode();
    } else {
        acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_cur_viewmode->viewmode;
        acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_help_viewmode;
    }
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_detail() {
    if (IsDetailMode()) {
        PopViewmode();
        acr_nav::_db.p_detail_field = NULL;
    } else if (acr_nav::_db.p_cur_viewmode->has_fields) {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        acr_nav::FField *fld = RightPanelFieldFind(sel_ct, acr_nav::_db.p_right_panel->sel_row);
        if (fld) {
            acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_cur_viewmode->viewmode;
            LoadDetail(*fld);
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_detail_viewmode;
        }
    }
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

// Emit ANSI escape sequences for a named terminal style.
// Attributes compose: caller may emit selection style then field color.
static void EmitStyle(cstring &out, acr_nav::FNavstyle &style) {
    if (style.bold) out << "\x1b[1m";
    if (style.dim) out << "\x1b[2m";
    if (style.reverse) out << "\x1b[7m";
    if (style.fg_color != algo_TermColor_default) {
        out << "\x1b[" << (90 + u32(style.fg_color)) << "m";
    }
}

// Truncate str to max_width characters, then right-pad with spaces to max_width.
static void TruncPad(cstring &str, int max_width) {
    if (ch_N(str) > max_width) {
        str.ch_n = max_width;
    }
    char_PrintNTimes(' ', str, i32_Max(0, max_width - ch_N(str)));
}

// -----------------------------------------------------------------------------

// Per-frame rendering context.  Populated once by Render, read by phase functions.
struct RenderCtx {
    cstring            &buf;
    acr_nav::FCtype    *sel_ct;
    int                 wid;
    int                 left_wid;
    int                 right_wid;
    bool                left_focused;
    bool                show_breadcrumb;
    int                 visible;
    RenderCtx(cstring &buf_
              , acr_nav::FCtype *sel_ct_
              , int wid_
              , int left_wid_
              , int right_wid_
              , bool left_focused_
              , bool show_breadcrumb_
              , int visible_)
        : buf(buf_), sel_ct(sel_ct_)
        , wid(wid_), left_wid(left_wid_), right_wid(right_wid_)
        , left_focused(left_focused_), show_breadcrumb(show_breadcrumb_), visible(visible_)
    {}
};

// -----------------------------------------------------------------------------

// Build breadcrumb trail from navigation stack.
// Returns empty string at depth 0, or "A > B > C" showing the path of ctypes visited.
static tempstr BuildBreadcrumb(acr_nav::FCtype *sel_ct) {
    tempstr bc;
    for (int i = 0; i < acr_nav::navstack_N(); i++) {
        if (i > 0) {
            bc << " > ";
        }
        bc << acr_nav::navstack_qFind(i).ctype;
    }
    if (acr_nav::navstack_N() > 0 && sel_ct) {
        bc << " > " << sel_ct->ctype;
    }
    return bc;
}

// Build status bar hint string from keybind records.
// Groups keys by navaction hint label, ordered by hint_order.
static void BuildStatusHint(cstring &out) {
    acr_nav::FKeybind *entries[64];
    int n_entries = 0;
    algo::strptr cur_mode = acr_nav::_db.p_cur_mode->navmode;
    ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
        if (kb.hint_order > 0 && acr_nav::navmode_Get(kb) == cur_mode && n_entries < 64) {
            entries[n_entries++] = &kb;
        }
    } ind_end;
    // Sort by hint_order (insertion sort -- small array)
    for (int i = 1; i < n_entries; i++) {
        acr_nav::FKeybind *tmp = entries[i];
        int j = i;
        while (j > 0 && entries[j - 1]->hint_order > tmp->hint_order) {
            entries[j] = entries[j - 1];
            j--;
        }
        entries[j] = tmp;
    }
    // Group by hint label, format as "key1/key2:hint"
    int i = 0;
    while (i < n_entries) {
        algo::Smallstr50 hint = entries[i]->p_navaction->hint;
        if (ch_N(out) > 0) {
            out << "  ";
        }
        int j = i;
        while (j < n_entries && entries[j]->p_navaction->hint == hint) {
            if (j > i) {
                out << "/";
            }
            out << acr_nav::key_Get(*entries[j]);
            j++;
        }
        out << ":" << hint;
        i = j;
    }
}

// -----------------------------------------------------------------------------

// Build preformatted help lines from keybind/navaction data.
// Iterates helpgroup records by sort_order, then navactions within each group.
// Keys split into standard (multi-char names) and shortcut (single letters).
static void BuildHelpLines() {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_help_viewmode;
    acr_nav::line_RemoveAll(vm);
    // Collect helpgroups sorted by sort_order
    acr_nav::FHelpgroup *groups[16];
    int n_groups = 0;
    ind_beg(acr_nav::_db_helpgroup_curs, hg, acr_nav::_db) {
        if (n_groups < 16) {
            groups[n_groups++] = &hg;
        }
    } ind_end;
    for (int i = 1; i < n_groups; i++) {
        acr_nav::FHelpgroup *tmp = groups[i];
        int j = i;
        while (j > 0 && groups[j - 1]->sort_order > tmp->sort_order) {
            groups[j] = groups[j - 1];
            j--;
        }
        groups[j] = tmp;
    }
    // Build header
    {
        tempstr hdr;
        hdr << "Standard";
        char_PrintNTimes(' ', hdr, i32_Max(1, 18 - ch_N(hdr)));
        hdr << "Shortcut";
        char_PrintNTimes(' ', hdr, i32_Max(1, 28 - ch_N(hdr)));
        hdr << "Action";
        vm.header = hdr;
    }
    // For each group, collect navactions and build lines
    for (int g = 0; g < n_groups; g++) {
        if (g > 0) {
            acr_nav::line_Alloc(vm) = "";
        }
        // Collect navactions in this group, sorted by help_sort
        acr_nav::FNavaction *actions[32];
        int n_actions = 0;
        ind_beg(acr_nav::_db_navaction_curs, na, acr_nav::_db) {
            if (ch_N(na.helpgroup) > 0 && na.helpgroup == groups[g]->helpgroup && n_actions < 32) {
                actions[n_actions++] = &na;
            }
        } ind_end;
        for (int i = 1; i < n_actions; i++) {
            acr_nav::FNavaction *tmp = actions[i];
            int j = i;
            while (j > 0 && actions[j - 1]->help_sort > tmp->help_sort) {
                actions[j] = actions[j - 1];
                j--;
            }
            actions[j] = tmp;
        }
        // Build a line for each navaction
        for (int a = 0; a < n_actions; a++) {
            tempstr std_keys;
            tempstr short_keys;
            ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
                if (kb.p_navaction == actions[a] && acr_nav::navmode_Get(kb) == "browse") {
                    algo::Smallstr50 key = acr_nav::key_Get(kb);
                    algo::strptr keystr(key);
                    bool is_shortcut = (elems_N(keystr) == 1 && ((keystr[0] >= 'a' && keystr[0] <= 'z') || (keystr[0] >= 'A' && keystr[0] <= 'Z')));
                    cstring &target = is_shortcut ? short_keys : std_keys;
                    if (ch_N(target) > 0) {
                        target << "/";
                    }
                    target << key;
                }
            } ind_end;
            tempstr line;
            line << std_keys;
            char_PrintNTimes(' ', line, i32_Max(1, 18 - ch_N(line)));
            line << short_keys;
            char_PrintNTimes(' ', line, i32_Max(1, 28 - ch_N(line)));
            line << actions[a]->comment;
            acr_nav::line_Alloc(vm) = line;
        }
    }
}

// -----------------------------------------------------------------------------

// Render the title bar: left panel title | right panel title.
static void RenderTitleBar(RenderCtx &ctx) {
    // Left panel
    {
        tempstr ltitle;
        ltitle << " " << acr_nav::_db.p_left_panel->title << " (" << acr_nav::zd_sel_ctype_N() << ")";
        TruncPad(ltitle, ctx.left_wid - 1);
        EmitStyle(ctx.buf, ctx.left_focused ? *acr_nav::_db.p_title_focus : *acr_nav::_db.p_title_nofocus);
        ctx.buf << ltitle << "\x1b[0m";
    }
    ctx.buf << "|";
    // Right panel title: detail mode shows field name instead of ctype name.
    // Title branches: detail mode, has_fields+selected, has_fields+empty, line-mode.
    // has_fields on FViewmode is the structural axis separating field/line rendering.
    {
        tempstr rtitle;
        if (IsDetailMode() && acr_nav::_db.p_detail_field) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << ": " << acr_nav::_db.p_detail_field->field
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        } else if (acr_nav::_db.p_cur_viewmode->has_fields && ctx.sel_ct) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << ": " << ctx.sel_ct->ctype
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        } else if (acr_nav::_db.p_cur_viewmode->has_fields) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title << " (empty)";
        } else {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        }
        TruncPad(rtitle, ctx.right_wid);
        EmitStyle(ctx.buf, !ctx.left_focused ? *acr_nav::_db.p_title_focus : *acr_nav::_db.p_title_nofocus);
        ctx.buf << rtitle << "\x1b[0m";
    }
    ctx.buf << "\r\n";
}

// -----------------------------------------------------------------------------

// Render the content area: dual-panel data rows.
static void RenderContentArea(RenderCtx &ctx) {
    // Pre-walk left panel linked list to scroll_offset
    acr_nav::FCtype *left_ptr = acr_nav::zd_sel_ctype_First();
    {
        int skip = 0;
        while (left_ptr && skip < acr_nav::_db.p_left_panel->scroll_offset) {
            left_ptr = acr_nav::zd_sel_ctype_Next(*left_ptr);
            skip++;
        }
    }
    int n_right = RightPanelItemCount(ctx.sel_ct);
    bool has_fields = acr_nav::_db.p_cur_viewmode->has_fields;
    bool in_xref = IsXrefMode();
    int visible = ctx.visible;

    // Column header row (all modes)
    if (ctx.sel_ct || !has_fields) {
        tempstr left_cell;
        char_PrintNTimes(' ', left_cell, ctx.left_wid - 1);
        ctx.buf << left_cell << "\x1b[0m|";
        tempstr hdr;
        if (!has_fields) {
            algo::strptr line_header = RightPanelLineHeader();
            if (ch_N(line_header) > 0) {
                hdr << " " << line_header;
            }
        } else {
            hdr << " field";
            char_PrintNTimes(' ', hdr, i32_Max(1, 24 - ch_N(hdr)));
            hdr << (in_xref ? "ctype" : "arg");
            char_PrintNTimes(' ', hdr, i32_Max(1, 48 - ch_N(hdr)));
            hdr << "reftype";
        }
        TruncPad(hdr, ctx.right_wid);
        EmitStyle(ctx.buf, !ctx.left_focused ? *acr_nav::_db.p_title_focus : *acr_nav::_db.p_title_nofocus);
        ctx.buf << hdr << "\x1b[0m\x1b[K\r\n";
        visible--;
    }

    for (int row = 0; row < visible; row++) {
        // Left cell
        tempstr left_cell;
        bool left_sel = false;
        if (left_ptr) {
            left_sel = (acr_nav::_db.p_left_panel->scroll_offset + row == acr_nav::_db.p_left_panel->sel_row);
            left_cell << " " << left_ptr->ctype;
            left_ptr = acr_nav::zd_sel_ctype_Next(*left_ptr);
        }
        TruncPad(left_cell, ctx.left_wid - 1);
        if (left_sel) {
            EmitStyle(ctx.buf, ctx.left_focused ? *acr_nav::_db.p_sel_focus : *acr_nav::_db.p_sel_nofocus);
        }
        ctx.buf << left_cell << "\x1b[0m|";

        // Right cell
        tempstr right_cell;
        bool right_sel = false;
        int right_data_idx = acr_nav::_db.p_right_panel->scroll_offset + row;
        acr_nav::FField *fld = nullptr;
        if (ctx.sel_ct && right_data_idx < n_right) {
            right_sel = (right_data_idx == acr_nav::_db.p_right_panel->sel_row);
            if (!has_fields) {
                right_cell << " " << RightPanelLineFind(right_data_idx);
            } else {
                    fld = RightPanelFieldFind(ctx.sel_ct, right_data_idx);
                    if (fld) {
                        right_cell << " " << name_Get(*fld);
                        char_PrintNTimes(' ', right_cell, i32_Max(1, 24 - ch_N(right_cell)));
                        right_cell << (in_xref ? fld->p_ctype->ctype : fld->p_arg->ctype);
                        char_PrintNTimes(' ', right_cell, i32_Max(1, 48 - ch_N(right_cell)));
                        right_cell << fld->p_reftype->reftype;
                    }
                }
            } else if (ctx.sel_ct && n_right == 0 && right_data_idx == 0) {
                right_cell << " (" << acr_nav::_db.p_cur_viewmode->empty_msg << ")";
            }
            TruncPad(right_cell, ctx.right_wid);
            if (right_sel) {
                EmitStyle(ctx.buf, !ctx.left_focused ? *acr_nav::_db.p_sel_focus : *acr_nav::_db.p_sel_nofocus);
            }
            if (fld && fld->p_reftype->c_reftypestyle) {
                EmitStyle(ctx.buf, *fld->p_reftype->c_reftypestyle->p_navstyle);
            }
            ctx.buf << right_cell << "\x1b[0m\x1b[K\r\n";
        }
}

// -----------------------------------------------------------------------------

// Render the breadcrumb bar (shown only when navstack is non-empty).
static void RenderBreadcrumbBar(RenderCtx &ctx) {
    if (ctx.show_breadcrumb) {
        tempstr bcline;
        bcline << " " << BuildBreadcrumb(ctx.sel_ct);
        TruncPad(bcline, ctx.wid);
        EmitStyle(ctx.buf, *acr_nav::_db.p_statusbar);
        ctx.buf << bcline << "\x1b[0m\r\n";
    }
}

// -----------------------------------------------------------------------------

// Render the status bar: filter text + hints + position indicator.
static void RenderStatusBar(RenderCtx &ctx) {
    EmitStyle(ctx.buf, *acr_nav::_db.p_statusbar);
    tempstr status;
    bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    status << " ";
    if (in_filter || has_filter) {
        status << "/" << acr_nav::_db.filter;
    }
    if (!in_filter) {
        if (has_filter) {
            status << "  ";
        }
        BuildStatusHint(status);
    }
    acr_nav::FPanel &cur = *acr_nav::_db.p_cur_panel;
    int cur_items = PanelItemCount(cur, ctx.sel_ct);
    tempstr pos;
    if (cur_items > 0) {
        pos << (cur.sel_row + 1) << "/" << cur_items;
    } else {
        pos << "0/0";
    }
    char_PrintNTimes(' ', status, i32_Max(1, ctx.wid - ch_N(status) - ch_N(pos)));
    status << pos;
    ctx.buf << status << "\x1b[0m";
}

// -----------------------------------------------------------------------------

static void Render(acr_nav::FCtype *sel_ct) {
    DetectTerminal();
    tempstr buf;
    buf << "\x1b[H";
    int wid = acr_nav::_db.term_wid;
    // Left panel width: fits longest visible name, capped at 40% of terminal
    int max_name = 0;
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ct, acr_nav::_db) {
        max_name = i32_Max(max_name, ch_N(ct.ctype));
    } ind_end;
    int min_left = i32_Min(acr_nav::_db.p_left_panel->min_width, wid / 2);
    int left_wid = i32_Max(min_left, i32_Min(max_name + 2, wid * 40 / 100));
    int right_wid = i32_Max(1, wid - left_wid);
    RenderCtx ctx(buf, sel_ct
                  , wid, left_wid, right_wid
                  , /*left_focused=*/(acr_nav::_db.p_cur_panel == acr_nav::_db.p_left_panel)
                  , /*show_breadcrumb=*/(acr_nav::navstack_N() > 0)
                  , /*visible=*/VisibleRows());
    RenderTitleBar(ctx);
    RenderContentArea(ctx);
    RenderBreadcrumbBar(ctx);
    RenderStatusBar(ctx);
    WriteStdout(buf.ch_elems, ch_N(buf));
}

// -----------------------------------------------------------------------------

static void InitPanels() {
    acr_nav::_db.p_left_panel = acr_nav::ind_panel_Find("ctype_list");
    acr_nav::_db.p_right_panel = acr_nav::ind_panel_Find("field_list");
    vrfy(acr_nav::_db.p_left_panel, "panel 'ctype_list' not found");
    vrfy(acr_nav::_db.p_right_panel, "panel 'field_list' not found");
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
    acr_nav::_db.p_filter_mode = acr_nav::ind_navmode_Find("filter");
    vrfy(acr_nav::_db.p_filter_mode, "navmode 'filter' not found");
    // Resolve well-known viewmode pointers by name
    acr_nav::_db.p_default_viewmode = acr_nav::ind_viewmode_Find("fields");
    acr_nav::_db.p_preview_viewmode = acr_nav::ind_viewmode_Find("preview");
    acr_nav::_db.p_xref_viewmode = acr_nav::ind_viewmode_Find("xref");
    acr_nav::_db.p_help_viewmode = acr_nav::ind_viewmode_Find("help");
    acr_nav::_db.p_detail_viewmode = acr_nav::ind_viewmode_Find("detail");
    vrfy(acr_nav::_db.p_default_viewmode, "viewmode 'fields' not found");
    vrfy(acr_nav::_db.p_preview_viewmode, "viewmode 'preview' not found");
    vrfy(acr_nav::_db.p_xref_viewmode, "viewmode 'xref' not found");
    vrfy(acr_nav::_db.p_help_viewmode, "viewmode 'help' not found");
    vrfy(acr_nav::_db.p_detail_viewmode, "viewmode 'detail' not found");
    // Start in help mode so new users see keybindings
    acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_default_viewmode->viewmode;
    acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_help_viewmode;
    acr_nav::_db.startup_help = true;
    BuildHelpLines();
    // Resolve well-known navstyle pointers by name
    acr_nav::_db.p_title_focus = acr_nav::ind_navstyle_Find("title_focus");
    acr_nav::_db.p_title_nofocus = acr_nav::ind_navstyle_Find("title_nofocus");
    acr_nav::_db.p_sel_focus = acr_nav::ind_navstyle_Find("sel_focus");
    acr_nav::_db.p_sel_nofocus = acr_nav::ind_navstyle_Find("sel_nofocus");
    acr_nav::_db.p_statusbar = acr_nav::ind_navstyle_Find("statusbar");
    vrfy(acr_nav::_db.p_title_focus && acr_nav::_db.p_title_nofocus
         && acr_nav::_db.p_sel_focus && acr_nav::_db.p_sel_nofocus
         && acr_nav::_db.p_statusbar, "required navstyle records missing");
    SwitchToBrowse();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

static bool ProcessKey(algo::strptr key_name) {
    bool did_something = false;
    // Startup help: first keypress dismisses and falls through to dispatch
    if (acr_nav::_db.startup_help) {
        acr_nav::_db.startup_help = false;
        PopViewmode();
    }
    {
        acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
        acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
        acr_nav::FCtype *prev_sel_ct = SelectedCtype(*left);
        tempstr composite;
        composite << acr_nav::_db.p_cur_mode->navmode << "." << key_name;
        acr_nav::FKeybind *keybind = acr_nav::ind_keybind_Find(composite);
        if (keybind) {
            step_Call(*keybind->p_navaction);
            did_something = true;
        }
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (!keybind && in_filter && elems_N(key_name) == 1 && key_name[0] > 32) {
            acr_nav::_db.filter << key_name;
            ApplyFilterReset();
            did_something = true;
        }
        if (did_something) {
            acr_nav::FCtype *sel_ct = SelectedCtype(*left);
            if (sel_ct != prev_sel_ct) {
                // If an overlay viewmode is active, pop all and restore base
                if (!acr_nav::viewmode_stack_EmptyQ()) {
                    acr_nav::FViewmode *base = acr_nav::ind_viewmode_Find(acr_nav::viewmode_stack_qFind(0));
                    acr_nav::viewmode_stack_RemoveAll();
                    if (base) {
                        acr_nav::_db.p_cur_viewmode = base;
                    }
                    acr_nav::_db.p_detail_field = NULL;
                }
                right->sel_row = 0;
                right->scroll_offset = 0;
            }
            AdjustScroll(*left, acr_nav::zd_sel_ctype_N());
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
    screen.n_sel_ctype = acr_nav::zd_sel_ctype_N();
    screen.n_ctype = acr_nav::ctype_N();
    screen.n_field = acr_nav::field_N();
    screen.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    screen.breadcrumb = BuildBreadcrumb(sel_ct);
    prlog(screen);
    // Left panel state
    acr_nav::PanelState left_state;
    left_state.panel = left->panel;
    left_state.sel_row = left->sel_row;
    left_state.scroll_offset = left->scroll_offset;
    left_state.n_items = acr_nav::zd_sel_ctype_N();
    left_state.sel_value = "";
    if (sel_ct) {
        left_state.sel_value = sel_ct->ctype;
    }
    prlog(left_state);
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
    // Visible fields (field-based modes only)
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        int n_vis = RightPanelItemCount(sel_ct);
        for (int i = 0; i < n_vis; i++) {
            acr_nav::FField *field = RightPanelFieldFind(sel_ct, i);
            if (field) {
                acr_nav::VisibleField vf;
                vf.row = i;
                vf.field = field->field;
                vf.arg = field->p_arg->ctype;
                vf.reftype = field->p_reftype->reftype;
                if (field->p_reftype->c_reftypestyle) {
                    vf.style = field->p_reftype->c_reftypestyle->p_navstyle->navstyle;
                }
                bool navigable = IsXrefMode()
                    ? (field->p_ctype != sel_ct)
                    : (field->p_arg != sel_ct);
                vf.navigable = navigable ? "Y" : "N";
                prlog(vf);
            }
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
    while (acr_nav::_db.running && !eof) {
        char buf[4096];
        ssize_t nr = read(STDIN_FILENO, buf, sizeof(buf));
        eof = (nr <= 0);
        algo::LinebufBegin(linebuf, algo::memptr((u8*)buf, eof ? 0 : nr), eof);
        algo::strptr line;
        while (algo::LinebufNext(linebuf, line)) {
            acr_nav::SendKey send_key;
            acr_nav::Screenshot screenshot;
            if (acr_nav::SendKey_ReadStrptrMaybe(send_key, line)) {
                ProcessKey(send_key.key);
            } else if (acr_nav::Screenshot_ReadStrptrMaybe(screenshot, line)) {
                HeadlessOutput();
            }
        }
    }
    HeadlessOutput();
}

// -----------------------------------------------------------------------------

void acr_nav::Main() {
    ApplyFilter();
    bool headless = _db.cmdline.headless || !isatty(STDOUT_FILENO);
    if (headless) {
        HeadlessMain();
    } else {
        InitPanels();
        DetectTerminal();
        EnterRawMode();
        acr_nav::FCtype *sel_ct = SelectedCtype(*_db.p_left_panel);
        Render(sel_ct);
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            if (ProcessKey(key_name)) {
                sel_ct = SelectedCtype(*_db.p_left_panel);
                Render(sel_ct);
            }
        }
        ExitRawMode();
    }
}
