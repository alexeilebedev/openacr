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
#include <signal.h>

static struct termios acr_nav_orig_termios;
static bool acr_nav_raw_mode = false;
static volatile sig_atomic_t acr_nav_sigwinch = 0;

static void SigwinchHandler(int) {
    acr_nav_sigwinch = 1;
}


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

static int DecimalDigits(int n) {
    int d = 1;
    while (n >= 10) { n /= 10; d++; }
    return d;
}

// True if field matches regex under the current filtertarget's boolean flags.
static bool FieldMatchesFilter(acr_nav::FField &fld, algo_lib::Regx &regx,
                               acr_nav::FFiltertarget &ft) {
    bool match = false;
    if (!match && ft.match_field_name) {
        tempstr lower(name_Get(fld));
        algo::MakeLower(lower);
        match = algo_lib::Regx_Match(regx, lower);
    }
    if (!match && ft.match_comment) {
        tempstr lower(fld.comment);
        algo::MakeLower(lower);
        match = algo_lib::Regx_Match(regx, lower);
    }
    if (!match && ft.match_arg) {
        tempstr lower(fld.p_arg->ctype);
        algo::MakeLower(lower);
        match = algo_lib::Regx_Match(regx, lower);
    }
    if (!match && ft.match_reftype) {
        tempstr lower(fld.p_reftype->reftype);
        algo::MakeLower(lower);
        match = algo_lib::Regx_Match(regx, lower);
    }
    return match;
}

// -----------------------------------------------------------------------------

// True if ctype passes the current filter.  Dispatches on filtertarget's
// boolean flags: match_ctype_name checks the ctype name directly,
// has_field_criteria iterates fields via FieldMatchesFilter.
static bool CtypeMatchesFilter(acr_nav::FCtype &ct, algo_lib::Regx &regx,
                               acr_nav::FFiltertarget &ft) {
    bool match = false;
    if (!match && ft.match_ctype_name) {
        tempstr lower(ct.ctype);
        algo::MakeLower(lower);
        match = algo_lib::Regx_Match(regx, lower);
    }
    if (!match && ft.has_field_criteria) {
        for (int f = 0; f < acr_nav::c_field_N(ct) && !match; f++) {
            acr_nav::FField *fld = acr_nav::c_field_Find(ct, f);
            if (fld) {
                match = FieldMatchesFilter(*fld, regx, ft);
            }
        }
    }
    return match;
}

// -----------------------------------------------------------------------------

static void BuildLeftItems() {
    acr_nav::left_item_RemoveAll();
    acr_nav::_db.n_visible_ctype = 0;
    algo_lib::Regx filter_regx;
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        tempstr pattern;
        pattern << "%" << acr_nav::_db.filter << "%";
        algo::MakeLower(pattern);
        algo_lib::Regx_ReadSql(filter_regx, pattern, false);
    }
    acr_nav::_db.filter_regx = filter_regx;
    acr_nav::FFiltertarget &ft = *acr_nav::_db.p_cur_filtertarget;
    // Collect namespaces with matching ctypes, sorted alphabetically.
    // FNs records are loaded from data/ in file order; explicit sort guarantees
    // stable display regardless of load order.
    acr_nav::FNs *ns_arr[256];
    int n_ns = 0;
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        int n_match = 0;
        for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
            acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
            if (ct && ch_N(ct->ctype) > 0) {
                bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, ft);
                n_match += match;
            }
        }
        ns.n_match = n_match;
        if (n_match > 0 && n_ns < 256) {
            ns_arr[n_ns++] = &ns;
        }
    } ind_end;
    // Insertion sort by namespace name
    for (int i = 1; i < n_ns; i++) {
        acr_nav::FNs *tmp = ns_arr[i];
        int j = i;
        while (j > 0 && algo::strptr_Cmp(ns_arr[j - 1]->ns, tmp->ns) > 0) {
            ns_arr[j] = ns_arr[j - 1];
            j--;
        }
        ns_arr[j] = tmp;
    }
    // Build display list
    for (int ni = 0; ni < n_ns; ni++) {
        acr_nav::FNs &ns = *ns_arr[ni];
        // Namespace header
        acr_nav::LeftItem &hdr = acr_nav::left_item_Alloc();
        hdr.ctype = "";
        hdr.ns = ns.ns;
        acr_nav::_db.n_visible_ctype += ns.n_match;
        // Ctype rows (if expanded)
        if (!ns.collapsed) {
            for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
                acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
                if (ct && ch_N(ct->ctype) > 0) {
                    bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, ft);
                    if (match) {
                        acr_nav::LeftItem &item = acr_nav::left_item_Alloc();
                        item.ctype = ct->ctype;
                        item.ns = "";
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

static void BuildLeftItemsReset() {
    BuildLeftItems();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

static void RecollapseAutoExpanded() {
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        if (ns.auto_expanded) {
            ns.collapsed = true;
            ns.auto_expanded = false;
        }
    } ind_end;
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

static acr_nav::FCtype* SelectedCtype(acr_nav::FPanel &left) {
    acr_nav::FCtype *ret = NULL;
    if (left.sel_row >= 0 && left.sel_row < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(left.sel_row);
        if (ch_N(item.ctype) > 0) {
            ret = acr_nav::ind_ctype_Find(item.ctype);
        }
    }
    return ret;
}

// Extract the namespace from the current left panel selection.
// Works for both ctype rows (via ctype's p_ns) and namespace headers (via LeftItem.ns).
static acr_nav::FNs* SelectedNs() {
    acr_nav::FPanel &left = *acr_nav::_db.p_left_panel;
    acr_nav::FCtype *ct = SelectedCtype(left);
    acr_nav::FNs *ret = NULL;
    if (ct) {
        ret = ct->p_ns;
    } else if (left.sel_row >= 0 && left.sel_row < acr_nav::left_item_N()) {
        ret = acr_nav::ind_ns_Find(acr_nav::left_item_qFind(left.sel_row).ns);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// is_reverse on FViewmode drives forward/reverse dispatch — no identity checks needed.

// IsHelpMode: identity check used only for the ? toggle guard in navaction_show_help.
// IsDetailMode: identity check for d toggle guard and title bar display.
// 2 overlay viewmodes, each with distinct entry guards (help: unconditional;
// detail: requires has_fields + selected field). The is_overlay property on
// viewmode records captures overlay state for the hint system; the navaction
// functions are not unified because their entry guards differ.
static bool IsHelpMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_help_viewmode;
}

static bool IsDetailMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_detail_viewmode;
}

static bool IsNsDepMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_nsdep_viewmode;
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

// Dismiss startup help on any non-passive action (not movement/panel switch).
// Escape/? pop the overlay directly; detected here via !IsHelpMode().
static void DismissStartupHelp(acr_nav::FKeybind *keybind) {
    if (acr_nav::_db.startup_help) {
        bool is_action = keybind && !keybind->p_navaction->passive;
        if (is_action || !IsHelpMode()) {
            acr_nav::_db.startup_help = false;
            if (IsHelpMode()) {
                PopViewmode();
            }
        }
    }
}

static void LoadNsDep(acr_nav::FNs &ns);

// If an overlay viewmode is active and the selected ctype changed, pop all
// overlays and restore the base viewmode.  During startup help, preserve the
// overlay so movement doesn't dismiss it.
// nsdep overlay is special: it stays open and reloads on namespace change.
static bool PopOverlayOnCtypeChange(acr_nav::FCtype *prev_sel_ct, acr_nav::FCtype *sel_ct) {
    bool keep_nsdep = false;
    bool nsdep_ns_changed = false;
    if (IsNsDepMode()) {
        // nsdep stays open across all navigation; reload on namespace change
        acr_nav::FNs *cur_ns = sel_ct ? sel_ct->p_ns : SelectedNs();
        keep_nsdep = (cur_ns != NULL);
        nsdep_ns_changed = keep_nsdep && cur_ns != acr_nav::_db.p_nsdep_ns;
        if (nsdep_ns_changed) {
            LoadNsDep(*cur_ns);
        }
    }
    bool changed = sel_ct != prev_sel_ct || nsdep_ns_changed;
    if (changed && !acr_nav::viewmode_stack_EmptyQ() && !acr_nav::_db.startup_help && !keep_nsdep) {
        acr_nav::FViewmode *base = acr_nav::ind_viewmode_Find(acr_nav::viewmode_stack_qFind(0));
        acr_nav::viewmode_stack_RemoveAll();
        if (base) {
            acr_nav::_db.p_cur_viewmode = base;
        }
        acr_nav::_db.p_detail_field = NULL;
    }
    return nsdep_ns_changed;
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
            if (fld && fld->p_reftype->reftype == "Base") {
                ret = fld->p_arg->c_ssimfile;
            }
        }
    }
    return ret;
}

// Append " (N)" record count suffix for the ctype's ssimfile.
// Returns number of characters appended (0 if no ssimfile or no records).
static int PrintRecordCount(cstring &out, acr_nav::FCtype &ctype) {
    int ret = 0;
    acr_nav::FSsimfile *ssf = FindSsimfile(ctype);
    if (ssf && ssf->n_record > 0) {
        int before = ch_N(out);
        out << " (" << ssf->n_record << ")";
        ret = ch_N(out) - before;
    }
    return ret;
}

// Clear all line content and color spans from a viewmode.
static void ClearViewmodeLines(acr_nav::FViewmode &vm) {
    acr_nav::line_RemoveAll(vm);
    acr_nav::cspan_RemoveAll(vm);
    acr_nav::preview_nav_RemoveAll(vm);
}

// Add a color span to a viewmode. Positions are 0-based relative to stored line text.
// Spans must be emitted in line_idx then col_start order. No overlapping spans.
static void AddSpan(acr_nav::FViewmode &vm, int line_idx, int col_start, int col_end, acr_nav::FNavstyle *p_style) {
    if (col_start < col_end && p_style) {
        acr_nav::LineColorSpan &span = acr_nav::cspan_Alloc(vm);
        span.line_idx = line_idx;
        span.col_start = col_start;
        span.col_end = col_end;
        span.p_navstyle = p_style;
    }
}

// Replace control characters (< 0x20) and DEL (0x7F) with '.' for safe terminal display.
// 1:1 byte replacement preserves string length for column alignment.
static void SanitizeForDisplay(cstring &str) {
    for (int i = 0; i < ch_N(str); i++) {
        unsigned char c = (unsigned char)str.ch_elems[i];
        if (c < 0x20 || c == 0x7F) {
            str.ch_elems[i] = '.';
        }
    }
}

// Count extra bytes from UTF-8 multibyte characters (byte_length - display_width).
static int Utf8ExtraBytes(algo::strptr s) {
    int extra = 0;
    for (int i = 0; i < s.n_elems; i++) {
        unsigned char c = s.elems[i];
        if (c >= 0xF0)      { extra += 3; }
        else if (c >= 0xE0) { extra += 2; }
        else if (c >= 0xC0) { extra += 1; }
    }
    return extra;
}

// Convert a display-column offset to a byte offset within a UTF-8 string.
// Each multi-byte lead byte counts as 1 display column; continuation bytes are skipped.
static int DisplayToByte(algo::strptr s, int display_col) {
    int dcol = 0, i = 0;
    while (i < s.n_elems && dcol < display_col) {
        unsigned char c = s.elems[i];
        int char_bytes = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        dcol++;
        i = i32_Min(i + char_bytes, s.n_elems);
    }
    return i;
}

// Load ssimfile content into the preview viewmode's line Tary, stripping the tuple head from each line.
// Format a single row of attr values into an aligned column string.
static void FormatPreviewRow(cstring &out, algo::Tuple &tuple, int *display_wid, int n_col, int *col_byte_pos) {
    int ci = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        if (ci < n_col) {
            if (ci > 0) {
                out << "  ";
            }
            if (col_byte_pos) col_byte_pos[ci] = ch_N(out);
            tempstr safe(attr.value);
            SanitizeForDisplay(safe);
            out << safe;
            int disp_len = i32_Max(0, ch_N(safe) - Utf8ExtraBytes(strptr(safe)));
            char_PrintNTimes(' ', out, display_wid[ci] - disp_len);
            ci++;
        }
    } ind_end;
    // Fill missing columns
    while (ci < n_col) {
        if (ci > 0) {
            out << "  ";
        }
        if (col_byte_pos) col_byte_pos[ci] = ch_N(out);
        char_PrintNTimes(' ', out, display_wid[ci]);
        ci++;
    }
}

static void LoadPreview(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_preview_viewmode;
    tempstr pending(acr_nav::_db.preview_nav_pending);
    acr_nav::_db.preview_nav_pending = "";
    ClearViewmodeLines(vm);
    vm.header = "";
    acr_nav::_db.p_preview_ctype = &ctype;
    acr_nav::FSsimfile *ssimfile = FindSsimfile(ctype);
    if (ssimfile) {
        tempstr path;
        path << "data/" << ssimns_Get(*ssimfile) << "/"
             << name_Get(*ssimfile) << ".ssim";
        algo_lib::MmapFile file;
        if (algo_lib::MmapFile_Load(file, path)) {
            // First pass: determine columns and display widths
            int n_col = 0;
            int display_wid[64];
            algo::cstring col_name[64];
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    if (n_col == 0) {
                        ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                            if (n_col < 64) {
                                col_name[n_col] = attr.name;
                                display_wid[n_col] = ch_N(attr.name);
                                n_col++;
                            }
                        } ind_end;
                    }
                    int ci = 0;
                    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                        if (ci < n_col) {
                            display_wid[ci] = i32_Max(display_wid[ci], ch_N(attr.value) - Utf8ExtraBytes(strptr(attr.value)));
                            ci++;
                        }
                    } ind_end;
                }
            } ind_end;
            // Detect navigable columns (already cleared by ClearViewmodeLines above)
            acr_nav::FCtype *field_base = ssimfile->p_ctype;
            {
                int col_pos = 0;
                for (int c = 0; c < n_col; c++) {
                    if (c > 0) {
                        col_pos += 2; // separator
                    }
                    tempstr qname;
                    qname << field_base->ctype << "." << col_name[c];
                    acr_nav::FField *fld = acr_nav::ind_field_Find(qname);
                    if (fld && fld->p_reftype->up && FindSsimfile(*fld->p_arg)) {
                        acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_Alloc(vm);
                        nc.col_start = col_pos;
                        nc.col_wid = display_wid[c];
                        nc.name_len = ch_N(col_name[c]);
                        nc.col_name = col_name[c];
                        nc.target_ctype = fld->p_arg->ctype;
                    }
                    col_pos += display_wid[c];
                }
            }
            vm.pkey_wid = (n_col > 0) ? display_wid[0] : 0;
            // Find comment column
            int comment_col = -1;
            for (int c = 0; c < n_col; c++) {
                if (algo::strptr_Eq(strptr(col_name[c]), "comment")) {
                    comment_col = c;
                    break;
                }
            }
            // Build header from column names
            if (n_col > 0) {
                tempstr hdr;
                for (int c = 0; c < n_col; c++) {
                    if (c > 0) {
                        hdr << "  ";
                    }
                    hdr << col_name[c];
                    char_PrintNTimes(' ', hdr, display_wid[c] - ch_N(col_name[c]));
                }
                vm.header = hdr;
            }
            // Second pass: format data rows directly into preview_line
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    tempstr row;
                    int col_byte_pos[64];
                    FormatPreviewRow(row, tuple, display_wid, n_col, col_byte_pos);
                    acr_nav::line_Alloc(vm) = row;
                    int li = acr_nav::line_N(vm) - 1;
                    // Dim pkey column (column 0)
                    if (n_col > 0) {
                        int pkey_end = (n_col > 1) ? col_byte_pos[1] - 2 : ch_N(row);
                        AddSpan(vm, li, 0, pkey_end, acr_nav::_db.p_line_key);
                    }
                    // Highlight comment column (only if non-empty)
                    if (comment_col >= 0) {
                        int cci = 0;
                        bool has_comment = false;
                        ind_beg(algo::Tuple_attrs_curs, cattr, tuple) {
                            if (cci == comment_col) {
                                has_comment = ch_N(cattr.value) > 0;
                                break;
                            }
                            cci++;
                        } ind_end;
                        if (has_comment) {
                            AddSpan(vm, li, col_byte_pos[comment_col], ch_N(row), acr_nav::_db.p_line_comment);
                        }
                    }
                }
            } ind_end;
            // Apply deferred follow-ref match
            if (ch_N(pending) > 0 && vm.pkey_wid > 0) {
                int n_lines = acr_nav::line_N(vm);
                for (int i = 0; i < n_lines; i++) {
                    algo::strptr row = acr_nav::line_qFind(vm, i);
                    int end = i32_Min(DisplayToByte(row, vm.pkey_wid), elems_N(row));
                    algo::strptr pkey_raw(row.elems, end);
                    tempstr pkey;
                    pkey << algo::TrimmedRight(pkey_raw);
                    if (algo::strptr_Eq(strptr(pkey), algo::TrimmedRight(strptr(pending)))) {
                        acr_nav::_db.p_right_panel->sel_row = i;
                        break;
                    }
                }
            }
        }
    }
    acr_nav::_db.sel_nav_col = 0;
}

// True if the identifier matches a C++ keyword commonly found in amc-generated output.
// Linear scan of ~26 entries; adequate for per-line highlighting.
static bool IsKw(algo::strptr word) {
    static const algo::strptr kw[] = {
        "bool", "const", "delete", "enum", "explicit", "extern",
        "friend", "inline", "namespace", "operator", "static",
        "struct", "template", "typename", "using", "void", "virtual",
        "__attribute__",
        "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"
    };
    bool ret = false;
    for (int i = 0; i < (int)(sizeof(kw)/sizeof(kw[0])); i++) {
        if (algo::strptr_Eq(word, kw[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}

// Tokenize one line of C++ code and emit highlight spans for keywords, strings, comments, and preprocessor directives.
// Single-pass left-to-right scan. Spans are emitted in col_start order (required by renderer).
// Handles: // comments (not inside strings), "..." strings (with \" escapes),
// #include/#pragma/#ifdef/#endif at line start, ~18 C++ keywords with word-boundary checks.
// Defers: /* */ multi-line comments, raw string literals (not in amc output).
static void HighlightCppLine(acr_nav::FViewmode &vm, int line_idx, algo::strptr line) {
    int len = line.n_elems;
    int pos = 0;
    // Check for preprocessor directive: # at start of line (after optional whitespace)
    int pp = 0;
    while (pp < len && (line.elems[pp] == ' ' || line.elems[pp] == '\t')) {
        pp++;
    }
    if (pp < len && line.elems[pp] == '#') {
        AddSpan(vm, line_idx, 0, len, acr_nav::_db.p_line_preproc);
        pos = len; // done with this line
    }
    while (pos < len) {
        char c = line.elems[pos];
        // Check for // line comment
        bool is_comment = (c == '/' && pos + 1 < len && line.elems[pos + 1] == '/');
        if (is_comment) {
            AddSpan(vm, line_idx, pos, len, acr_nav::_db.p_line_comment);
            pos = len; // done with this line
        }
        // Check for string literal
        if (pos < len && line.elems[pos] == '"') {
            int start = pos;
            pos++; // skip opening quote
            bool closed = false;
            while (pos < len && !closed) {
                char sc = line.elems[pos];
                if (sc == '\\' && pos + 1 < len) {
                    pos += 2; // skip escaped character
                } else if (sc == '"') {
                    pos++; // skip closing quote
                    closed = true;
                } else {
                    pos++;
                }
            }
            AddSpan(vm, line_idx, start, pos, acr_nav::_db.p_line_string);
        }
        // Check for identifier (potential keyword)
        if (pos < len) {
            char ic = line.elems[pos];
            bool is_alpha = (ic >= 'A' && ic <= 'Z') || (ic >= 'a' && ic <= 'z') || ic == '_';
            if (is_alpha) {
                int start = pos;
                while (pos < len) {
                    char wc = line.elems[pos];
                    bool is_word = (wc >= 'A' && wc <= 'Z') || (wc >= 'a' && wc <= 'z')
                        || (wc >= '0' && wc <= '9') || wc == '_';
                    if (!is_word) {
                        break;
                    }
                    pos++;
                }
                // Word boundary check: char before start must be non-word or start of line
                bool before_ok = (start == 0);
                if (!before_ok) {
                    char bc = line.elems[start - 1];
                    before_ok = !((bc >= 'A' && bc <= 'Z') || (bc >= 'a' && bc <= 'z')
                        || (bc >= '0' && bc <= '9') || bc == '_');
                }
                // Char after end must be non-word or end of line
                bool after_ok = (pos == len);
                if (!after_ok) {
                    char ac = line.elems[pos];
                    after_ok = !((ac >= 'A' && ac <= 'Z') || (ac >= 'a' && ac <= 'z')
                        || (ac >= '0' && ac <= '9') || ac == '_');
                }
                if (before_ok && after_ok) {
                    algo::strptr word(line.elems + start, pos - start);
                    if (IsKw(word)) {
                        AddSpan(vm, line_idx, start, pos, acr_nav::_db.p_line_keyword);
                    }
                }
            } else {
                pos++; // advance past non-interesting character
            }
        }
    }
}

// Load amc-generated C++ struct definition for a ctype into the codegen viewmode.
// Ctype names come from trusted ssimfile data loaded at startup.
static void LoadCodegen(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_codegen_viewmode;
    ClearViewmodeLines(vm);
    acr_nav::_db.p_codegen_ctype = &ctype;
    tempstr cmd;
    cmd << "amc '" << ctype.ctype << "'";
    vm.header = cmd;
    tempstr output = SysEval(cmd, FailokQ(true), 64*1024);
    ind_beg(Line_curs, line, output) {
        if (!StartsWithQ(line, "report.")) {
            acr_nav::line_Alloc(vm) = line;
            HighlightCppLine(vm, acr_nav::line_N(vm) - 1, line);
        }
    } ind_end;
}

static algo::strptr NsDisplayName(acr_nav::FNs &ns) {
    return ch_N(ns.ns) > 0 ? algo::strptr(ns.ns) : algo::strptr("other");
}

// Per-namespace dependency count for LoadNsDep accumulation.
struct NsDep { acr_nav::FNs *ns; int count; };

// Format a sorted section of namespace dependency counts as highlighted text lines.
// Sorts deps[0..n) by count descending, emits a header line and one row per dep.
static void FormatNsDepSection(acr_nav::FViewmode &vm, algo::strptr header,
                               NsDep *deps, int n) {
    // Sort by count descending (insertion sort)
    for (int i = 1; i < n; i++) {
        NsDep tmp = deps[i];
        int j = i - 1;
        while (j >= 0 && deps[j].count < tmp.count) {
            deps[j + 1] = deps[j];
            j--;
        }
        deps[j + 1] = tmp;
    }
    // Section header
    {
        tempstr hdr;
        hdr << header;
        acr_nav::line_Alloc(vm) = hdr;
        AddSpan(vm, acr_nav::line_N(vm) - 1, 0, ch_N(hdr), acr_nav::_db.p_line_section);
    }
    if (n == 0) {
        acr_nav::line_Alloc(vm) = "  (none)";
    }
    // Find max ns name width for alignment
    int max_wid = 0;
    for (int i = 0; i < n; i++) {
        int name_wid = ch_N(NsDisplayName(*deps[i].ns));
        max_wid = i32_Max(max_wid, name_wid);
    }
    // Format each row
    for (int i = 0; i < n; i++) {
        algo::strptr name = NsDisplayName(*deps[i].ns);
        tempstr row;
        row << "  ";
        char_PrintNTimes(' ', row, max_wid - ch_N(name));
        int ns_start = ch_N(row);
        row << name;
        int ns_end = ch_N(row);
        row << "  ";
        int cnt = deps[i].count;
        int digs = DecimalDigits(cnt);
        char_PrintNTimes(' ', row, i32_Max(0, 5 - digs));
        row << cnt;
        row << (cnt == 1 ? " field" : " fields");
        acr_nav::line_Alloc(vm) = row;
        AddSpan(vm, acr_nav::line_N(vm) - 1, ns_start, ns_end, acr_nav::_db.p_line_key);
    }
}

// Find-or-insert namespace in accumulator array, increment count.
static void AccumNsDep(NsDep *deps, int &n, int max_n, acr_nav::FNs *ns) {
    bool found = false;
    for (int i = 0; i < n && !found; i++) {
        if (deps[i].ns == ns) {
            deps[i].count++;
            found = true;
        }
    }
    if (!found && n < max_n) {
        deps[n].ns = ns;
        deps[n].count = 1;
        n++;
    }
}

// Compute and display cross-namespace field dependencies for a given namespace.
// Upstream: namespaces this ns imports from (via field arg references).
// Downstream: namespaces that import from this ns (via field_arg back-references).
static void LoadNsDep(acr_nav::FNs &ns) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_nsdep_viewmode;
    ClearViewmodeLines(vm);
    acr_nav::_db.p_nsdep_ns = &ns;
    // Fixed-size accumulator for per-namespace counts
    NsDep deps[256];
    if (acr_nav::ns_N() > 256) {
        acr_nav::line_Alloc(vm) = "(too many namespaces)";
    } else {
        // --- Upstream: fields in this ns whose arg is in another ns ---
        int n_up = 0;
        ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
            ind_beg(acr_nav::ctype_c_field_curs, fld, ct) {
                if (fld.p_arg && fld.p_arg->p_ns != &ns) {
                    AccumNsDep(deps, n_up, 256, fld.p_arg->p_ns);
                }
            } ind_end;
        } ind_end;
        algo::strptr display_name = NsDisplayName(ns);
        // Format upstream section
        {
            tempstr hdr;
            hdr << "Upstream (" << display_name << " imports from):";
            FormatNsDepSection(vm, hdr, deps, n_up);
        }
        // --- Downstream: fields from OTHER ns whose arg points to ctypes in this ns ---
        NsDep down[256];
        int n_down = 0;
        ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
            ind_beg(acr_nav::ctype_c_field_arg_curs, fld, ct) {
                if (fld.p_ctype->p_ns != &ns) {
                    AccumNsDep(down, n_down, 256, fld.p_ctype->p_ns);
                }
            } ind_end;
        } ind_end;
        // Blank separator
        acr_nav::line_Alloc(vm) = "";
        // Format downstream section
        {
            tempstr hdr;
            hdr << "Downstream (imports from " << display_name << "):";
            FormatNsDepSection(vm, hdr, down, n_down);
        }
    }
    vm.header = NsDisplayName(ns);
}

// Ensure-content wrappers for hook dispatch.
// Each normalizes the lazy-load check to the ensure_content hook signature.
static void PreviewEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_preview_ctype != &ct) {
        LoadPreview(ct);
    }
}

static void CodegenEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_codegen_ctype != &ct) {
        LoadCodegen(ct);
    }
}

static void NsDepEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_nsdep_ns != ct.p_ns) {
        LoadNsDep(*ct.p_ns);
    }
}

// --- Graph viewmode: Interactive access path diagram ---

// Return true if a field should be excluded from the graph diagram.
// Matches amc_vis::DepRefQ exclusions: self-ref, Base, Regx, RegxSql, Hook.
static bool GraphSkipQ(acr_nav::FField &field) {
    return field.p_arg == field.p_ctype
        || field.reftype == dmmeta_Reftype_reftype_Base
        || field.reftype == dmmeta_Reftype_reftype_Regx
        || field.reftype == dmmeta_Reftype_reftype_RegxSql
        || field.reftype == dmmeta_Reftype_reftype_Hook;
}

// Edge group: one neighbor ctype with one or more connecting fields.
struct GraphEdgeGroup {
    acr_nav::FCtype *p_neighbor;
    acr_nav::FField *fields[64];
    int n_field;
    bool is_left;  // true = left column (up:Y dep), false = right column
};

// Add a field to the dep-path edge group matching (neighbor, is_left).
// Creates a new group if none exists and capacity allows.
static void AddDepEdge(GraphEdgeGroup *groups, int &n_group, int max_groups,
                       acr_nav::FCtype *neighbor, bool is_left, acr_nav::FField &field) {
    int gi = -1;
    for (int i = 0; i < n_group; i++) {
        if (groups[i].p_neighbor == neighbor && groups[i].is_left == is_left) {
            gi = i;
            break;
        }
    }
    if (gi < 0 && n_group < max_groups) {
        gi = n_group++;
        groups[gi].p_neighbor = neighbor;
        groups[gi].n_field = 0;
        groups[gi].is_left = is_left;
    }
    if (gi >= 0 && groups[gi].n_field < 64) {
        groups[gi].fields[groups[gi].n_field++] = &field;
    }
}

// Collect edge groups from c_field (forward) and c_field_arg (reverse).
// Returns the number of groups written into 'groups' (max 64).
static int CollectGraphEdges(acr_nav::FCtype &center, GraphEdgeGroup *groups, int max_groups) {
    int n_group = 0;
    // Track Val creation targets for first-wins dedup
    acr_nav::FCtype *val_targets[64];
    int n_val_target = 0;
    ind_beg(acr_nav::ctype_c_field_curs, field, center) {
        if (GraphSkipQ(field)) {
            // skip
        } else if (field.p_reftype->isval) {
            // Val creation path: right column, first-wins dedup per target
            bool seen = false;
            for (int i = 0; i < n_val_target; i++) {
                if (val_targets[i] == field.p_arg) {
                    seen = true;
                    break;
                }
            }
            if (!seen && n_val_target < 64 && n_group < max_groups) {
                val_targets[n_val_target++] = field.p_arg;
                // Val creation path: always a new group (one edge per target, matching amc_vis)
                int gi = n_group++;
                groups[gi].p_neighbor = field.p_arg;
                groups[gi].n_field = 1;
                groups[gi].fields[0] = &field;
                groups[gi].is_left = false;
            }
        } else {
            AddDepEdge(groups, n_group, max_groups, field.p_arg, field.p_reftype->up, field);
        }
    } ind_end;
    // Reverse edges: fields from other ctypes whose arg is center.
    // Skip value embeddings (isval && !hasalloc) to avoid flooding from
    // primitive types -- but keep allocators (Lary, Tpool, Lpool).
    ind_beg(acr_nav::ctype_c_field_arg_curs, field, center) {
        if (GraphSkipQ(field) || (field.p_reftype->isval && !field.p_reftype->hasalloc)) {
            // skip
        } else {
            AddDepEdge(groups, n_group, max_groups, field.p_ctype, !field.p_reftype->up, field);
        }
    } ind_end;
    return n_group;
}

// Map a graph line index to the neighbor ctype and (optionally) the field on that line.
// Returns neighbor ctype via p_node_out, field via p_field_out (both nullable).
// Center open/close lines return NULL for both. Neighbor open/close lines return node but NULL field.
static void GraphInfoAtLine(acr_nav::FCtype &center, int line_idx,
                            acr_nav::FCtype **p_node_out, acr_nav::FField **p_field_out) {
    acr_nav::FCtype *node = NULL;
    acr_nav::FField *field = NULL;
    GraphEdgeGroup groups[64];
    int n_group = CollectGraphEdges(center, groups, 64);
    int right_groups[64], n_right = 0;
    int left_groups[64], n_left = 0;
    for (int i = 0; i < n_group; i++) {
        if (groups[i].is_left) {
            left_groups[n_left++] = i;
        } else {
            right_groups[n_right++] = i;
        }
    }
    int cur_line = 1;  // line 0 is center open
    // Right-column blocks
    for (int ri = 0; ri < n_right; ri++) {
        GraphEdgeGroup &g = groups[right_groups[ri]];
        int block_lines = g.n_field + 1;
        if (line_idx >= cur_line && line_idx < cur_line + block_lines) {
            node = g.p_neighbor;
            int fi = line_idx - cur_line;
            if (fi < g.n_field) {
                field = g.fields[fi];
            }
        }
        cur_line += block_lines;
    }
    // Left-column blocks
    for (int li = 0; li < n_left; li++) {
        GraphEdgeGroup &g = groups[left_groups[li]];
        int block_lines = g.n_field + 2 + (li == n_left - 1 ? 1 : 0);
        if (li == n_left - 1 && line_idx == cur_line + 1 + g.n_field) {
            // center close -- node stays NULL
        } else if (line_idx >= cur_line && line_idx < cur_line + block_lines) {
            node = g.p_neighbor;
            int fi = line_idx - cur_line - 1;
            if (fi >= 0 && fi < g.n_field) {
                field = g.fields[fi];
            }
        }
        cur_line += block_lines;
    }
    if (p_node_out) *p_node_out = node;
    if (p_field_out) *p_field_out = field;
}

// Reverse of GraphInfoAtLine: given a neighbor ctype, return its first line in the graph.
// Returns -1 if not found.
static int GraphFindCtypeLine(acr_nav::FCtype &center, acr_nav::FCtype *target) {
    int result = -1;
    GraphEdgeGroup groups[64];
    int n_group = CollectGraphEdges(center, groups, 64);
    int right_groups[64], n_right = 0;
    int left_groups[64], n_left = 0;
    for (int i = 0; i < n_group; i++) {
        if (groups[i].is_left) {
            left_groups[n_left++] = i;
        } else {
            right_groups[n_right++] = i;
        }
    }
    int cur_line = 1; // line 0 is center open
    for (int ri = 0; ri < n_right && result < 0; ri++) {
        GraphEdgeGroup &g = groups[right_groups[ri]];
        if (g.p_neighbor == target) {
            result = cur_line;
        }
        cur_line += g.n_field + 1;
    }
    for (int li = 0; li < n_left && result < 0; li++) {
        GraphEdgeGroup &g = groups[left_groups[li]];
        if (g.p_neighbor == target) {
            result = cur_line;
        }
        cur_line += g.n_field + 2 + (li == n_left - 1 ? 1 : 0);
    }
    return result;
}

// Build amc_vis-style graph lines for the given ctype.
// Populates the graph viewmode's line_elems array.
static void LoadGraph(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_graph_viewmode;
    ClearViewmodeLines(vm);
    vm.header = ctype.ctype;
    acr_nav::_db.p_graph_ctype = &ctype;
    // Collect edge groups
    GraphEdgeGroup groups[64];
    int n_group = CollectGraphEdges(ctype, groups, 64);
    // Zero groups: emit nothing, let empty_msg render "no access paths"
    if (n_group > 0) {
        // Separate right and left groups
        int right_groups[64], n_right = 0;
        int left_groups[64], n_left = 0;
        for (int i = 0; i < n_group; i++) {
            if (groups[i].is_left) {
                left_groups[n_left++] = i;
            } else {
                right_groups[n_right++] = i;
            }
        }
        // Measure widths for column positioning
        int max_left_name = 0;
        int max_left_label = 0;
        int max_right_label = 0;
        for (int i = 0; i < n_group; i++) {
            GraphEdgeGroup &g = groups[i];
            for (int fi = 0; fi < g.n_field; fi++) {
                acr_nav::FField &fld = *g.fields[fi];
                int label_len = ch_N(fld.reftype) + 1 + ch_N(name_Get(fld));
                if (g.is_left) {
                    int name_len = ch_N(g.p_neighbor->ctype);
                    {
                        tempstr tmp;
                        name_len += PrintRecordCount(tmp, *g.p_neighbor);
                    }
                    max_left_name = i32_Max(max_left_name, name_len);
                    max_left_label = i32_Max(max_left_label, label_len);
                } else {
                    max_right_label = i32_Max(max_right_label, label_len);
                }
            }
        }
        // Column positions
        int center_x = 0;
        if (n_left > 0) {
            center_x = max_left_name + 2 + max_left_label + 3;
        }
        // Line 0: center open
        {
            tempstr line;
            char_PrintNTimes(' ', line, center_x);
            line << "/ " << ctype.ctype;
            acr_nav::line_Alloc(vm) = line;
            AddSpan(vm, acr_nav::line_N(vm) - 1, center_x + 2, center_x + 2 + ch_N(ctype.ctype),
                    acr_nav::_db.p_graph_ctype_style);
        }
        // Right-column blocks
        for (int ri = 0; ri < n_right; ri++) {
            GraphEdgeGroup &g = groups[right_groups[ri]];
            for (int fi = 0; fi < g.n_field; fi++) {
                acr_nav::FField &fld = *g.fields[fi];
                tempstr label;
                label << fld.reftype << " " << name_Get(fld);
                tempstr line;
                char_PrintNTimes(' ', line, center_x);
                line << "|" << label;
                int arrow_start = ch_N(line);
                int arrow_pad = max_right_label - ch_N(label) + 1;
                char_PrintNTimes('-', line, arrow_pad);
                line << ">";
                int arrow_end = ch_N(line);
                int name_start = 0, name_end = 0;
                if (fi == 0) {
                    line << "/ ";
                    name_start = ch_N(line);
                    line << g.p_neighbor->ctype;
                    name_end = ch_N(line);
                    PrintRecordCount(line, *g.p_neighbor);
                } else {
                    line << "|";
                }
                acr_nav::line_Alloc(vm) = line;
                int line_idx = acr_nav::line_N(vm) - 1;
                // Spans in col_start order: label, arrow, neighbor name
                if (fld.p_reftype->c_reftypestyle) {
                    AddSpan(vm, line_idx, center_x + 1, center_x + 1 + ch_N(label),
                            fld.p_reftype->c_reftypestyle->p_navstyle);
                }
                AddSpan(vm, line_idx, arrow_start, arrow_end,
                        acr_nav::_db.p_graph_arrow);
                if (fi == 0) {
                    AddSpan(vm, line_idx, name_start, name_end,
                            acr_nav::_db.p_graph_neighbor);
                }
            }
            // Close line for right neighbor
            {
                tempstr line;
                char_PrintNTimes(' ', line, center_x);
                line << "|";
                int right_x = center_x + max_right_label + 2;
                char_PrintNTimes(' ', line, right_x - center_x - 1);
                line << "-";
                acr_nav::line_Alloc(vm) = line;
            }
        }
        // Left-column blocks
        for (int li = 0; li < n_left; li++) {
            GraphEdgeGroup &g = groups[left_groups[li]];
            int left_x = center_x - max_left_label - 3;
            int neighbor_display_len = ch_N(g.p_neighbor->ctype);
            {
                tempstr tmp;
                neighbor_display_len += PrintRecordCount(tmp, *g.p_neighbor);
            }
            int name_x = left_x - neighbor_display_len - 2;
            if (name_x < 0) {
                name_x = 0;
            }
            // Open line: neighbor name
            {
                tempstr line;
                char_PrintNTimes(' ', line, name_x);
                line << "/ ";
                int nb_start = ch_N(line);
                line << g.p_neighbor->ctype;
                int nb_end = ch_N(line);
                PrintRecordCount(line, *g.p_neighbor);
                int cur_len = ch_N(line);
                if (cur_len < center_x) {
                    char_PrintNTimes(' ', line, center_x - cur_len);
                }
                line << "|";
                acr_nav::line_Alloc(vm) = line;
                AddSpan(vm, acr_nav::line_N(vm) - 1, nb_start, nb_end,
                        acr_nav::_db.p_graph_neighbor);
            }
            // Edge lines
            for (int fi = 0; fi < g.n_field; fi++) {
                acr_nav::FField &fld = *g.fields[fi];
                tempstr label;
                label << fld.reftype << " " << name_Get(fld);
                tempstr line;
                char_PrintNTimes(' ', line, name_x);
                line << "|";
                int arrow_start = ch_N(line);
                line << "<";
                int dash_len = center_x - name_x - 2;
                char_PrintNTimes('-', line, dash_len);
                int arrow_end = ch_N(line);
                line << "|" << label;
                acr_nav::line_Alloc(vm) = line;
                int line_idx = acr_nav::line_N(vm) - 1;
                // Spans in col_start order: arrow, label
                AddSpan(vm, line_idx, arrow_start, arrow_end,
                        acr_nav::_db.p_graph_arrow);
                if (fld.p_reftype->c_reftypestyle) {
                    int label_start = center_x + 1;
                    AddSpan(vm, line_idx, label_start, label_start + ch_N(label),
                            fld.p_reftype->c_reftypestyle->p_navstyle);
                }
            }
            // Center close: inside last left block; otherwise left-close + center bar
            if (li == n_left - 1) {
                tempstr line;
                char_PrintNTimes(' ', line, name_x);
                line << "|";
                int cur_len = ch_N(line);
                if (cur_len < center_x) {
                    char_PrintNTimes(' ', line, center_x - cur_len);
                }
                line << "-";
                acr_nav::line_Alloc(vm) = line;
            } else {
                tempstr line;
                char_PrintNTimes(' ', line, name_x);
                line << "-";
                int cur_len = ch_N(line);
                if (cur_len < center_x) {
                    char_PrintNTimes(' ', line, center_x - cur_len);
                }
                line << "|";
                acr_nav::line_Alloc(vm) = line;
            }
            // Left neighbor close after center close (last block only)
            if (li == n_left - 1) {
                tempstr line;
                char_PrintNTimes(' ', line, name_x);
                line << "-";
                acr_nav::line_Alloc(vm) = line;
            }
        }
        // If no left blocks, close center with standalone line
        if (n_left == 0) {
            tempstr line;
            char_PrintNTimes(' ', line, center_x);
            line << "-";
            acr_nav::line_Alloc(vm) = line;
        }
    }
}

static void GraphEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_graph_ctype != &ct) {
        LoadGraph(ct);
    }
}

// Format a single ssim record as a vertical card: section header + one key:value per line.
// Appends formatted lines to vm.line_elems. Skips primary key attr when its value
// matches field_name (already shown in the detail header bar).
static void FormatDetailCard(acr_nav::FViewmode &vm, algo::Tuple &tuple, algo::strptr field_name) {
    // Compute key column width (max attr name length, excluding redundant pkey)
    int key_wid = 0;
    int ai = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        bool is_redundant_pkey = (ai == 0) && (attr.value == field_name);
        if (!is_redundant_pkey) {
            key_wid = i32_Max(key_wid, ch_N(attr.name));
        }
        ai++;
    } ind_end;
    // Section header: "-- dmmeta.thash ------..."
    tempstr hdr;
    hdr << "-- " << tuple.head << " ";
    char_PrintNTimes('-', hdr, i32_Max(0, 36 - ch_N(hdr)));
    acr_nav::line_Alloc(vm) = hdr;
    AddSpan(vm, acr_nav::line_N(vm) - 1, 0, ch_N(hdr), acr_nav::_db.p_line_section);
    // Key:value rows
    ai = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        bool is_redundant_pkey = (ai == 0) && (attr.value == field_name);
        if (!is_redundant_pkey) {
            tempstr row;
            row << "  " << attr.name;
            char_PrintNTimes(' ', row, i32_Max(2, key_wid - ch_N(attr.name) + 4));
            tempstr safe(attr.value);
            SanitizeForDisplay(safe);
            row << safe;
            acr_nav::line_Alloc(vm) = row;
            AddSpan(vm, acr_nav::line_N(vm) - 1, 2, 2 + ch_N(attr.name), acr_nav::_db.p_line_key);
        }
        ai++;
    } ind_end;
    // Blank separator between cards
    acr_nav::line_Alloc(vm) = strptr();
}

// Load metadata records for a single field from detailsrc ssimfiles.
// Re-serializes the dmmeta.field record as the first card, then scans each
// detailsrc file for matching records (first attribute value == field name).
static void LoadDetail(acr_nav::FField &field) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_detail_viewmode;
    ClearViewmodeLines(vm);
    acr_nav::_db.p_detail_field = &field;
    algo::strptr field_name(field.field);
    int n_records = 0;
    // First card: the dmmeta.field record itself
    {
        dmmeta::Field base;
        acr_nav::field_CopyOut(field, base);
        tempstr fld_line;
        dmmeta::Field_Print(base, fld_line);
        algo::Tuple fld_tuple;
        if (algo::Tuple_ReadStrptr(fld_tuple, fld_line, false)) {
            FormatDetailCard(vm, fld_tuple, field_name);
            n_records++;
        }
    }
    // Second card: the dmmeta.reftype record for this field's reftype
    if (ch_N(field.p_reftype->comment) > 0) {
        dmmeta::Reftype base;
        acr_nav::reftype_CopyOut(*field.p_reftype, base);
        tempstr rt_line;
        dmmeta::Reftype_Print(base, rt_line);
        algo::Tuple rt_tuple;
        if (algo::Tuple_ReadStrptr(rt_tuple, rt_line, false)) {
            FormatDetailCard(vm, rt_tuple, field_name);
            n_records++;
        }
    }
    // Scan each detailsrc file for matching records.
    // Path derived via Pathcomp rather than Ssimfile accessors: detailsrc uses
    // a Smallstr50 key instead of a Pkey to dmmeta.Ssimfile, avoiding ~5 schema
    // records (finput, Upptr, xref) for display-only file scanning.
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
                        FormatDetailCard(vm, tuple, field_name);
                        n_records++;
                    }
                }
            } ind_end;
        }
    } ind_end;
    // Set header
    tempstr hdr;
    hdr << field.field << " (" << n_records << " records)";
    vm.header = hdr;
}

static int RightPanelItemCount(acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (!acr_nav::_db.p_cur_viewmode->has_fields) {
        if (sel_ct && acr_nav::_db.p_cur_viewmode->ensure_content) {
            acr_nav::ensure_content_Call(*acr_nav::_db.p_cur_viewmode, *sel_ct);
        }
        // Fallback for nsdep namespace-header rows where sel_ct is NULL --
        // the hook path above handles the ctype case via NsDepEnsureContent.
        if (!sel_ct && IsNsDepMode()) {
            acr_nav::FNs *ns = SelectedNs();
            if (ns && acr_nav::_db.p_nsdep_ns != ns) {
                LoadNsDep(*ns);
            }
        }
        ret = RightPanelLineCount();
    } else if (sel_ct) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        ret = reverse ? c_field_arg_N(*sel_ct) : c_field_N(*sel_ct);
    }
    return ret;
}

static acr_nav::FField* RightPanelFieldFind(acr_nav::FCtype *sel_ct, int idx) {
    acr_nav::FField *ret = NULL;
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        ret = reverse ? c_field_arg_Find(*sel_ct, idx) : c_field_Find(*sel_ct, idx);
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
        ret = acr_nav::left_item_N();
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

// Number of data rows visible in each panel (VisibleRows minus the column header row).
static int DataRows() {
    return VisibleRows() - 1;
}

static void AdjustScroll(acr_nav::FPanel &panel, int n_items) {
    int visible = DataRows();
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
    int page = DataRows();
    panel.sel_row = i32_Max(0, panel.sel_row - page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_page_down() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    int page = DataRows();
    panel.sel_row = i32_Min(last, panel.sel_row + page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_switch_panel_left() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    if (panel.position > acr_nav::_db.p_left_panel->position) {
        acr_nav::FViewmode &pvm = *acr_nav::_db.p_cur_viewmode;
        int n_nav = acr_nav::preview_nav_N(pvm);
        if (n_nav > 0 && acr_nav::_db.sel_nav_col > 0) {
            acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col - 1;
        } else {
            acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
        }
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
    } else if (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode) {
        acr_nav::FViewmode &pvm = *acr_nav::_db.p_preview_viewmode;
        int n_nav = acr_nav::preview_nav_N(pvm);
        if (n_nav > 0) {
            acr_nav::_db.sel_nav_col = (acr_nav::_db.sel_nav_col + 1) % n_nav;
        }
    }
}

// -----------------------------------------------------------------------------

// Push navstack, navigate to target ctype, set dest_viewmode.
// Shared by field/xref follow and graph follow.
static void NavigateToTarget(acr_nav::FCtype *sel_ct, acr_nav::FCtype *target, acr_nav::FViewmode *dest_viewmode) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::Naventry &entry = acr_nav::navstack_Alloc();
    entry.filter = acr_nav::_db.filter;
    entry.navmode = acr_nav::_db.p_cur_mode->navmode;
    entry.scroll_offset = left->scroll_offset;
    entry.sel_row = left->sel_row;
    entry.right_sel_row = acr_nav::_db.p_right_panel->sel_row;
    entry.right_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
    entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    entry.ctype = sel_ct->ctype;
    entry.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
    entry.focus_panel = acr_nav::_db.p_cur_panel->panel;
    acr_nav::_db.p_cur_viewmode = dest_viewmode;
    acr_nav::_db.filter = "";
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    SwitchToBrowse();
    target->p_ns->collapsed = false;
    BuildLeftItems();
    for (int i = 0; i < acr_nav::left_item_N(); i++) {
        if (acr_nav::left_item_qFind(i).ctype == target->ctype) {
            left->sel_row = i;
            break;
        }
    }
}

void acr_nav::navaction_follow_ref() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    if (panel.position == 0) {
        int sel = left->sel_row;
        if (sel >= 0 && sel < acr_nav::left_item_N()) {
            acr_nav::LeftItem &item = acr_nav::left_item_qFind(sel);
            if (ch_N(item.ctype) == 0) {
                // Namespace header: toggle collapse
                acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
                if (ns) {
                    ns->collapsed = !ns->collapsed;
                    if (ns->collapsed) {
                        ns->auto_expanded = false;
                    }
                    BuildLeftItems();
                }
            } else {
                // Dismiss nsdep overlay so right panel returns to fields view
                if (IsNsDepMode()) {
                    PopViewmode();
                }
                acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
            }
        }
    } else if (panel.position == 1 && sel_ct
               && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_graph_viewmode) {
        // Graph mode: navigate to the neighbor ctype on the selected line
        acr_nav::FCtype *target = NULL;
        GraphInfoAtLine(*sel_ct, panel.sel_row, &target, NULL);
        if (target && target != sel_ct) {
            NavigateToTarget(sel_ct, target, acr_nav::_db.p_graph_viewmode);
        }
    } else if (panel.position == 1 && sel_ct
               && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode) {
        acr_nav::FViewmode &pvm = *acr_nav::_db.p_preview_viewmode;
        int n_nav = acr_nav::preview_nav_N(pvm);
        if (n_nav > 0 && acr_nav::_db.sel_nav_col < n_nav
            && panel.sel_row < acr_nav::line_N(pvm)) {
            acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_qFind(pvm, acr_nav::_db.sel_nav_col);
            // Extract cell value from formatted preview line
            algo::strptr row_text = RightPanelLineFind(panel.sel_row);
            int start = DisplayToByte(row_text, nc.col_start);
            int end = i32_Min(DisplayToByte(row_text, nc.col_start + nc.col_wid), elems_N(row_text));
            tempstr cell_value;
            if (start < elems_N(row_text)) {
                algo::strptr raw_cell(row_text.elems + start, end - start);
                cell_value << algo::TrimmedRight(raw_cell);
            }
            acr_nav::FCtype *target = acr_nav::ind_ctype_Find(nc.target_ctype);
            if (target && target != sel_ct && ch_N(cell_value) > 0) {
                acr_nav::_db.preview_nav_pending = cell_value;
                NavigateToTarget(sel_ct, target, acr_nav::_db.p_preview_viewmode);
            }
        }
    } else if (panel.position == 1 && acr_nav::_db.p_cur_viewmode->has_fields
               && sel_ct && panel.sel_row < RightPanelItemCount(sel_ct)) {
        acr_nav::FField *fld = RightPanelFieldFind(sel_ct, panel.sel_row);
        acr_nav::FCtype *target = NULL;
        if (fld) {
            bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
            target = reverse ? fld->p_ctype : fld->p_arg;
        }
        if (fld && target != sel_ct) {
            NavigateToTarget(sel_ct, target, acr_nav::_db.p_default_viewmode);
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
        acr_nav::FFiltertarget *ft = acr_nav::ind_filtertarget_Find(entry->filtertarget);
        if (ft) {
            acr_nav::_db.p_cur_filtertarget = ft;
        }
        // Ensure target namespace is expanded so the ctype is findable
        acr_nav::FCtype *target_ct = acr_nav::ind_ctype_Find(entry->ctype);
        if (target_ct) {
            target_ct->p_ns->collapsed = false;
        }
        BuildLeftItems();
        // Scan for the saved ctype -- collapse state may have changed since push
        acr_nav::_db.p_left_panel->sel_row = 0;
        acr_nav::_db.p_left_panel->scroll_offset = 0;
        for (int i = 0; i < acr_nav::left_item_N(); i++) {
            if (acr_nav::left_item_qFind(i).ctype == entry->ctype) {
                acr_nav::_db.p_left_panel->sel_row = i;
                break;
            }
        }
        acr_nav::_db.p_left_panel->scroll_offset = entry->scroll_offset;
        acr_nav::_db.p_right_panel->sel_row = entry->right_sel_row;
        acr_nav::_db.p_right_panel->scroll_offset = entry->right_scroll_offset;
        acr_nav::FPanel *focus = acr_nav::ind_panel_Find(entry->focus_panel);
        if (focus) {
            acr_nav::_db.p_cur_panel = focus;
        } else {
            acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
        }
        acr_nav::navstack_RemoveLast();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_quit() {
    acr_nav::_db.running = false;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_cycle_viewmode() {
    acr_nav::FViewmode *next = acr_nav::ind_viewmode_Find(acr_nav::_db.p_cur_viewmode->next);
    if (next) {
        acr_nav::_db.p_cur_viewmode = next;
    }
}

// -----------------------------------------------------------------------------

static void ToggleViewmode(acr_nav::FViewmode *target) {
    bool in_target = (acr_nav::_db.p_cur_viewmode == target);
    bool can_enter = !in_target;
    if (can_enter && target->need_ssimfile) {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        can_enter = sel_ct && FindSsimfile(*sel_ct);
    }
    acr_nav::_db.p_cur_viewmode = can_enter ? target : acr_nav::_db.p_default_viewmode;
}

void acr_nav::navaction_toggle_preview() { ToggleViewmode(acr_nav::_db.p_preview_viewmode); }

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_codegen() { ToggleViewmode(acr_nav::_db.p_codegen_viewmode); }

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_fields() { ToggleViewmode(acr_nav::_db.p_default_viewmode); }

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_xref() { ToggleViewmode(acr_nav::_db.p_xref_viewmode); }

// -----------------------------------------------------------------------------

void acr_nav::navaction_toggle_graph() { ToggleViewmode(acr_nav::_db.p_graph_viewmode); }

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_accept() {
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        // Recollapse previous session's auto-expanded before setting new ones
        RecollapseAutoExpanded();
        // Expand all namespaces that have matching ctypes so results are visible
        ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
            if (ns.n_match > 0 && ns.collapsed) {
                ns.auto_expanded = true;
                ns.collapsed = false;
            }
        } ind_end;
        BuildLeftItems();
        // Navigate cursor to first matching ctype (skip namespace headers)
        acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
        left->sel_row = 0;
        left->scroll_offset = 0;
        for (int i = 0; i < acr_nav::left_item_N(); i++) {
            if (ch_N(acr_nav::left_item_qFind(i).ctype) > 0) {
                left->sel_row = i;
                break;
            }
        }
    }
    SwitchToBrowse();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_start() {
    acr_nav::_db.pre_filter_text = acr_nav::_db.filter;
    acr_nav::_db.pre_filter_target = acr_nav::_db.p_cur_filtertarget->filtertarget;
    acr_nav::_db.pre_filter_sel_row = acr_nav::_db.p_left_panel->sel_row;
    acr_nav::_db.pre_filter_scroll_offset = acr_nav::_db.p_left_panel->scroll_offset;
    acr_nav::_db.p_cur_mode = acr_nav::_db.p_filter_mode;
    ch_RemoveAll(acr_nav::_db.filter);
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    BuildLeftItemsReset();
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_cancel() {
    SwitchToBrowse();
    acr_nav::_db.filter = acr_nav::_db.pre_filter_text;
    acr_nav::FFiltertarget *ft = acr_nav::ind_filtertarget_Find(acr_nav::_db.pre_filter_target);
    if (ft) {
        acr_nav::_db.p_cur_filtertarget = ft;
    } else {
        acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    }
    BuildLeftItems();
    acr_nav::_db.p_left_panel->sel_row = acr_nav::_db.pre_filter_sel_row;
    acr_nav::_db.p_left_panel->scroll_offset = acr_nav::_db.pre_filter_scroll_offset;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_append_space() {
    acr_nav::_db.filter << " ";
    BuildLeftItemsReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_clear() {
    if (ch_N(acr_nav::_db.filter) > 0) {
        ch_RemoveAll(acr_nav::_db.filter);
        RecollapseAutoExpanded();
        BuildLeftItemsReset();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_dismiss_or_clear() {
    if (!acr_nav::viewmode_stack_EmptyQ()) {
        bool was_detail = IsDetailMode();
        PopViewmode();
        if (was_detail) {
            acr_nav::_db.p_detail_field = NULL;
        }
    } else {
        navaction_filter_clear();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_backspace() {
    if (ch_N(acr_nav::_db.filter) > 0) {
        acr_nav::_db.filter.ch_n = ch_N(acr_nav::_db.filter) - 1;
        BuildLeftItemsReset();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_cycle_target() {
    acr_nav::FFiltertarget *next = acr_nav::ind_filtertarget_Find(acr_nav::_db.p_cur_filtertarget->next);
    if (next) {
        acr_nav::_db.p_cur_filtertarget = next;
    }
    BuildLeftItemsReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_help() {
    if (IsHelpMode()) {
        PopViewmode();
    } else {
        acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_cur_viewmode->viewmode;
        acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_help_viewmode;
    }
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
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_nsdep() {
    if (IsNsDepMode()) {
        PopViewmode();
    } else {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        acr_nav::FNs *ns = sel_ct ? sel_ct->p_ns : SelectedNs();
        if (ns) {
            acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_cur_viewmode->viewmode;
            LoadNsDep(*ns);
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_nsdep_viewmode;
        }
    }
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

// Build context-aware status bar hint string from keybind records.
// Checks navaction visibility conditions (need_no_overlay, need_has_fields,
// need_navstack, need_right_panel, need_left_panel) and overlay dismiss_hint against runtime state.
// Groups keys by effective hint label, ordered by hint_order.
static void BuildStatusHint(cstring &out) {
    struct HintEntry {
        acr_nav::FKeybind *kb;
        algo::strptr hint;
    };
    HintEntry entries[64];
    int n_entries = 0;
    algo::strptr cur_mode = acr_nav::_db.p_cur_mode->navmode;
    bool is_overlay = acr_nav::_db.p_cur_viewmode->is_overlay;
    bool has_fld = acr_nav::_db.p_cur_viewmode->has_fields;
    bool has_nav = !acr_nav::navstack_EmptyQ();
    bool on_right = (acr_nav::_db.p_cur_panel->position == 1);
    algo::strptr cur_vm = acr_nav::_db.p_cur_viewmode->viewmode;
    ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
        if (kb.hint_order <= 0 || !(acr_nav::navmode_Get(kb) == cur_mode)) {
            continue;
        }
        acr_nav::FNavaction &na = *kb.p_navaction;
        algo::strptr hint;
        // When a keybind dismisses the current overlay, use dismiss_hint and
        // bypass need_* checks -- the dismiss action is always available.
        bool is_dismiss = is_overlay && ch_N(na.dismiss_hint) > 0
            && (ch_N(na.dismiss_viewmode) == 0
                || na.dismiss_viewmode == cur_vm);
        if (is_dismiss) {
            hint = na.dismiss_hint;
        } else {
            if (na.need_no_overlay && is_overlay) continue;
            // Symbol-only hints (arrow indicators) suppressed during overlays
            if (is_overlay && ch_N(na.hint) > 0 && (algo::strptr(na.hint)[0] & 0x80)) continue;
            if (na.need_has_fields && !has_fld) continue;
            if (na.need_navstack && !has_nav) continue;
            if (na.need_right_panel && !on_right) continue;
            if (na.need_left_panel && on_right) continue;
            hint = na.hint;
        }
        if (ch_N(hint) == 0) continue;
        if (n_entries < 64) {
            entries[n_entries].kb = &kb;
            entries[n_entries].hint = hint;
            n_entries++;
        }
    } ind_end;
    // Sort by hint_order (insertion sort -- small array)
    for (int i = 1; i < n_entries; i++) {
        HintEntry tmp = entries[i];
        int j = i;
        while (j > 0 && entries[j - 1].kb->hint_order > tmp.kb->hint_order) {
            entries[j] = entries[j - 1];
            j--;
        }
        entries[j] = tmp;
    }
    // Group by effective hint label, format as "key1/key2:hint".
    // Symbol-only hints (non-ASCII first byte, e.g. ↑↓←→) render without key prefix.
    int i = 0;
    while (i < n_entries) {
        algo::strptr hint = entries[i].hint;
        if (ch_N(out) > 0) {
            out << "  ";
        }
        int j = i;
        bool symbol_hint = ch_N(hint) > 0 && (hint[0] & 0x80);
        if (symbol_hint) {
            out << hint;
            while (j < n_entries && entries[j].hint == hint) j++;
        } else {
            while (j < n_entries && entries[j].hint == hint) {
                if (j > i) {
                    out << "/";
                }
                out << acr_nav::key_Get(*entries[j].kb);
                j++;
            }
            out << ":" << hint;
        }
        i = j;
    }
}

// -----------------------------------------------------------------------------

// Append key display name, mapping arrow keys to Unicode symbols.
static void AppendKeyDisplay(cstring &out, algo::strptr key) {
    if (key == "Up")         { out << "\xe2\x86\x91"; }
    else if (key == "Down")  { out << "\xe2\x86\x93"; }
    else if (key == "Left")  { out << "\xe2\x86\x90"; }
    else if (key == "Right") { out << "\xe2\x86\x92"; }
    else { out << key; }
}

// Collect browse-mode keybinds for one action into a single key string.
// Standard keys (arrows, Enter, etc.) come first, then letter aliases.
static void CollectActionKeys(acr_nav::FNavaction *action, cstring &keys) {
    // Two passes: first non-letter keys, then letter keys
    for (int pass = 0; pass < 2; pass++) {
        ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
            if (kb.p_navaction == action && acr_nav::navmode_Get(kb) == "browse") {
                algo::Smallstr50 key = acr_nav::key_Get(kb);
                algo::strptr keystr(key);
                bool is_letter = (elems_N(keystr) == 1 && ((keystr[0] >= 'a' && keystr[0] <= 'z') || (keystr[0] >= 'A' && keystr[0] <= 'Z')));
                if ((pass == 0 && !is_letter) || (pass == 1 && is_letter)) {
                    if (ch_N(keys) > 0) {
                        keys << "/";
                    }
                    AppendKeyDisplay(keys, keystr);
                }
            }
        } ind_end;
    }
}

// Check if two consecutive navactions form a directional pair (up/down, left/right, top/bottom).
static bool IsDirPair(acr_nav::FNavaction *a, acr_nav::FNavaction *b) {
    algo::strptr na(a->navaction);
    algo::strptr nb(b->navaction);
    int prefix = 0;
    while (prefix < na.n_elems && prefix < nb.n_elems && na.elems[prefix] == nb.elems[prefix]) {
        prefix++;
    }
    if (prefix <= 0 || prefix >= na.n_elems || prefix >= nb.n_elems) {
        return false;
    }
    algo::strptr sa(na.elems + prefix, na.n_elems - prefix);
    algo::strptr sb(nb.elems + prefix, nb.n_elems - prefix);
    return (sa == "up" && sb == "down")
        || (sa == "left" && sb == "right")
        || (sa == "top" && sb == "bottom");
}

// Merge comments of paired actions: combine first differing word with "/".
// "Move selection up" + "Move selection down" → "Move selection up/down"
static tempstr MergePairComments(algo::strptr c1, algo::strptr c2) {
    // Extract words from both comments
    tempstr words1[16], words2[16];
    int nw1 = 0, nw2 = 0;
    {
        int i = 0;
        while (i < c1.n_elems && nw1 < 16) {
            while (i < c1.n_elems && c1.elems[i] == ' ') i++;
            int start = i;
            while (i < c1.n_elems && c1.elems[i] != ' ') i++;
            if (i > start) { words1[nw1] << algo::strptr(c1.elems + start, i - start); nw1++; }
        }
    }
    {
        int i = 0;
        while (i < c2.n_elems && nw2 < 16) {
            while (i < c2.n_elems && c2.elems[i] == ' ') i++;
            int start = i;
            while (i < c2.n_elems && c2.elems[i] != ' ') i++;
            if (i > start) { words2[nw2] << algo::strptr(c2.elems + start, i - start); nw2++; }
        }
    }
    // Walk words, combine first differing pair with "/"
    tempstr result;
    int n = i32_Max(nw1, nw2);
    for (int w = 0; w < n; w++) {
        if (w > 0) { result << " "; }
        algo::strptr s1 = w < nw1 ? strptr(words1[w]) : strptr();
        algo::strptr s2 = w < nw2 ? strptr(words2[w]) : strptr();
        if (s1 == s2) {
            result << s1;
        } else if (s1.n_elems > 0 && s2.n_elems > 0) {
            result << s1 << "/" << s2;
        } else if (s1.n_elems > 0) {
            result << s1;
        } else {
            result << s2;
        }
    }
    return result;
}

// Build preformatted help lines from keybind/navaction data.
// Single-column layout with section headers styled like the detail view.
// Directional pairs (up/down, left/right) are merged into single lines.
// Arrow keys display as Unicode symbols (↑↓←→).
static void BuildHelpLines() {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_help_viewmode;
    ClearViewmodeLines(vm);
    vm.header = "Keyboard Shortcuts";
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
    // For each group, emit section header + action lines
    for (int g = 0; g < n_groups; g++) {
        // Section header: "-- Movement ------..."
        {
            tempstr hdr;
            hdr << "-- " << groups[g]->comment << " ";
            char_PrintNTimes('-', hdr, i32_Max(0, 36 - ch_N(hdr)));
            acr_nav::line_Alloc(vm) = hdr;
            AddSpan(vm, acr_nav::line_N(vm) - 1, 0, ch_N(hdr), acr_nav::_db.p_line_section);
        }
        // Collect navactions in this group, sorted by sort_order
        acr_nav::FNavaction *actions[32];
        int n_actions = 0;
        ind_beg(acr_nav::_db_navaction_curs, na, acr_nav::_db) {
            if (na.p_helpgroup == groups[g] && n_actions < 32) {
                actions[n_actions++] = &na;
            }
        } ind_end;
        for (int i = 1; i < n_actions; i++) {
            acr_nav::FNavaction *tmp = actions[i];
            int j = i;
            while (j > 0 && actions[j - 1]->sort_order > tmp->sort_order) {
                actions[j] = actions[j - 1];
                j--;
            }
            actions[j] = tmp;
        }
        // Build a line for each navaction, merging directional pairs
        for (int a = 0; a < n_actions; a++) {
            bool is_pair = (a + 1 < n_actions) && IsDirPair(actions[a], actions[a + 1]);
            // Collect keys into single string
            tempstr keys;
            CollectActionKeys(actions[a], keys);
            if (is_pair) {
                CollectActionKeys(actions[a + 1], keys);
            }
            // Build comment
            tempstr comment;
            if (is_pair) {
                comment = MergePairComments(strptr(actions[a]->comment), strptr(actions[a + 1]->comment));
                a++;
            } else {
                comment << actions[a]->comment;
            }
            // Build line: "  keys          comment"
            tempstr line;
            line << "  " << keys;
            int keys_end = ch_N(line);
            int extra = Utf8ExtraBytes(strptr(keys));
            char_PrintNTimes(' ', line, i32_Max(2, 22 - ch_N(line) + extra));
            int comment_start = ch_N(line);
            line << comment;
            acr_nav::line_Alloc(vm) = line;
            int li = acr_nav::line_N(vm) - 1;
            AddSpan(vm, li, 2, keys_end, acr_nav::_db.p_line_key);
            AddSpan(vm, li, comment_start, ch_N(line), acr_nav::_db.p_line_comment);
        }
    }
}

// -----------------------------------------------------------------------------

// Render the title bar: left panel title | right panel title.
static void RenderTitleBar(RenderCtx &ctx) {
    // Left panel
    {
        tempstr ltitle;
        ltitle << " " << acr_nav::_db.p_left_panel->title << " (" << acr_nav::_db.n_visible_ctype << ")";
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

// Emit preview header with cyan-highlighted navigable column names.
// nav_col entries provide column positions; non-nav regions get base_style.
static void EmitStyledPreviewHeader(cstring &buf, algo::strptr hdr, acr_nav::FViewmode &vm, acr_nav::FNavstyle &base_style) {
    int prev_end = 0;
    int hdr_n = elems_N(hdr);
    int n_nav = acr_nav::preview_nav_N(vm);
    for (int ni = 0; ni < n_nav; ni++) {
        acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_qFind(vm, ni);
        int adj_start = nc.col_start + 1;  // +1 for leading space in right_cell
        int adj_end = i32_Min(adj_start + nc.name_len, hdr_n);
        if (adj_start >= hdr_n) {
            continue;  // inside loop, not function return
        }
        // Plain run before this nav column
        if (adj_start > prev_end) {
            buf << algo::strptr(hdr.elems + prev_end, adj_start - prev_end);
        }
        // Cyan-styled nav column name
        if (acr_nav::_db.p_line_nav_header) {
            EmitStyle(buf, *acr_nav::_db.p_line_nav_header);
        }
        buf << algo::strptr(hdr.elems + adj_start, adj_end - adj_start);
        buf << "\x1b[0m";
        EmitStyle(buf, base_style);
        prev_end = adj_end;
    }
    // Trailing plain run
    if (prev_end < hdr_n) {
        buf << algo::strptr(hdr.elems + prev_end, hdr_n - prev_end);
    }
}

// Emit a text region [rs, re) from right_cell, splitting at overlay boundaries.
// base_style is applied to non-overlay portions (NULL for plain text).
// overlay_style replaces base_style inside [ov_start, ov_end).
static void EmitRegionWithOverlay(cstring &buf, algo::strptr right_cell,
                                   int rs, int re, bool right_sel,
                                   acr_nav::FNavstyle *sel_style,
                                   acr_nav::FNavstyle *base_style,
                                   int ov_start, int ov_end,
                                   acr_nav::FNavstyle *overlay_style) {
    // Before overlay
    if (rs < ov_start && rs < re) {
        int seg_end = i32_Min(ov_start, re);
        if (base_style) {
            EmitStyle(buf, *base_style);
        }
        buf << algo::strptr(right_cell.elems + rs, seg_end - rs);
        if (base_style) {
            buf << "\x1b[0m";
            if (right_sel && sel_style) {
                EmitStyle(buf, *sel_style);
            }
        }
        rs = seg_end;
    }
    // Inside overlay
    if (rs >= ov_start && rs < ov_end && rs < re) {
        int seg_end = i32_Min(ov_end, re);
        buf << "\x1b[0m";
        EmitStyle(buf, *overlay_style);
        buf << algo::strptr(right_cell.elems + rs, seg_end - rs);
        buf << "\x1b[0m";
        if (right_sel && sel_style) {
            EmitStyle(buf, *sel_style);
        }
        rs = seg_end;
    }
    // After overlay
    if (rs < re) {
        if (base_style) {
            EmitStyle(buf, *base_style);
        }
        buf << algo::strptr(right_cell.elems + rs, re - rs);
        if (base_style) {
            buf << "\x1b[0m";
            if (right_sel && sel_style) {
                EmitStyle(buf, *sel_style);
            }
        }
    }
}

// Emit a line with color spans interleaved. Spans reference positions in the stored line text
// (0-based). right_cell has a leading space prefix, so span positions are adjusted by +1.
// Uses span-boundary runs for clean SESE flow. span_cursor advances monotonically across
// visible lines for O(visible) total scan cost.
// overlay_start/overlay_end define a region where overlay_style replaces base styles.
static void EmitStyledLine(cstring &buf, algo::strptr right_cell, bool right_sel,
                           acr_nav::FViewmode &vm, int line_idx, int &span_cursor,
                           acr_nav::FNavstyle *sel_style,
                           int overlay_start, int overlay_end,
                           acr_nav::FNavstyle *overlay_style) {
    int cell_n = elems_N(right_cell);
    int prev_end = 0;
    int ov_start = (overlay_start >= 0 && overlay_style) ? overlay_start + 1 : cell_n + 1;
    int ov_end = (overlay_end >= 0 && overlay_style) ? i32_Min(overlay_end + 1, cell_n) : cell_n + 1;
    // Advance span_cursor to first span for this line
    int n_spans = acr_nav::cspan_N(vm);
    while (span_cursor < n_spans && acr_nav::cspan_qFind(vm, span_cursor).line_idx < line_idx) {
        span_cursor++;
    }
    // Process spans for this line
    int si = span_cursor;
    while (si < n_spans && acr_nav::cspan_qFind(vm, si).line_idx == line_idx) {
        acr_nav::LineColorSpan &span = acr_nav::cspan_qFind(vm, si);
        int adj_start = span.col_start + 1;
        int adj_end = i32_Min(span.col_end + 1, cell_n);
        if (adj_start >= cell_n) {
            si++;
            continue;  // span fully clipped -- inside while loop, not function-level return
        }
        // Emit plain run before this span
        if (adj_start > prev_end) {
            EmitRegionWithOverlay(buf, right_cell, prev_end, adj_start, right_sel, sel_style, nullptr, ov_start, ov_end, overlay_style);
        }
        // Emit styled span
        EmitRegionWithOverlay(buf, right_cell, adj_start, adj_end, right_sel, sel_style, span.p_navstyle, ov_start, ov_end, overlay_style);
        prev_end = adj_end;
        si++;
    }
    // Emit trailing plain run
    if (prev_end < cell_n) {
        EmitRegionWithOverlay(buf, right_cell, prev_end, cell_n, right_sel, sel_style, nullptr, ov_start, ov_end, overlay_style);
    }
}

// -----------------------------------------------------------------------------

// Render the content area: dual-panel data rows.
static void RenderContentArea(RenderCtx &ctx) {
    int n_left = acr_nav::left_item_N();
    int scroll = acr_nav::_db.p_left_panel->scroll_offset;
    int n_right = RightPanelItemCount(ctx.sel_ct);
    bool has_fields = acr_nav::_db.p_cur_viewmode->has_fields;
    bool in_xref = acr_nav::_db.p_cur_viewmode->is_reverse;
    int visible = ctx.visible;

    // Column header row -- always rendered for visual stability
    // (cursor on namespace header has no sel_ct, but layout must not shift)
    {
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
        acr_nav::FNavstyle &base_hdr_style = !ctx.left_focused ? *acr_nav::_db.p_title_focus : *acr_nav::_db.p_title_nofocus;
        EmitStyle(ctx.buf, base_hdr_style);
        if (!has_fields && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode
            && acr_nav::preview_nav_N(*acr_nav::_db.p_cur_viewmode) > 0) {
            EmitStyledPreviewHeader(ctx.buf, strptr(hdr), *acr_nav::_db.p_cur_viewmode, base_hdr_style);
        } else {
            ctx.buf << hdr;
        }
        ctx.buf << "\x1b[0m\x1b[K\r\n";
        visible--;
    }

    int span_cursor = 0;
    for (int row = 0; row < visible; row++) {
        // Left cell
        tempstr left_cell;
        bool left_sel = false;
        int left_idx = scroll + row;
        if (left_idx < n_left) {
            left_sel = (left_idx == acr_nav::_db.p_left_panel->sel_row);
            acr_nav::LeftItem &item = acr_nav::left_item_qFind(left_idx);
            if (ch_N(item.ctype) == 0) {
                // Namespace header row
                acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
                int count = ns ? ns->n_match : 0;
                left_cell << (ns && ns->collapsed ? " \xe2\x96\xb8 " : " \xe2\x96\xbe ");
                left_cell << (ns ? NsDisplayName(*ns) : algo::strptr("other"));
                left_cell << " (" << count << ")";
            } else {
                // Ctype row: indented, namespace prefix stripped
                algo::strptr full(item.ctype);
                algo::strptr stripped = algo::Pathcomp(full, ".LR");
                // Dot-less ctypes (extern types): use full key as name
                if (elems_N(stripped) == 0) {
                    stripped = full;
                }
                left_cell << "    " << stripped;
                acr_nav::FCtype *ct = acr_nav::ind_ctype_Find(item.ctype);
                if (ct) {
                    PrintRecordCount(left_cell, *ct);
                }
            }
        }
        // +2 for namespace headers: triangle is 3 UTF-8 bytes but 1 display column
        bool is_header = left_idx < n_left && ch_N(acr_nav::left_item_qFind(left_idx).ctype) == 0;
        TruncPad(left_cell, ctx.left_wid - 1 + (is_header ? 2 : 0));
        if (left_sel) {
            EmitStyle(ctx.buf, ctx.left_focused ? *acr_nav::_db.p_sel_focus : *acr_nav::_db.p_sel_nofocus);
        }
        ctx.buf << left_cell << "\x1b[0m|";

        // Right cell
        tempstr right_cell;
        bool right_sel = false;
        int right_data_idx = acr_nav::_db.p_right_panel->scroll_offset + row;
        acr_nav::FField *fld = nullptr;
        if ((ctx.sel_ct || !has_fields) && right_data_idx < n_right) {
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
        } else if ((ctx.sel_ct || !has_fields) && n_right == 0 && right_data_idx == 0) {
            right_cell << " (" << acr_nav::_db.p_cur_viewmode->empty_msg << ")";
        }
        TruncPad(right_cell, ctx.right_wid);
        if (right_sel && !ctx.left_focused) {
            EmitStyle(ctx.buf, *acr_nav::_db.p_sel_focus);
        }
        if (fld && fld->p_reftype->c_reftypestyle) {
            EmitStyle(ctx.buf, *fld->p_reftype->c_reftypestyle->p_navstyle);
        }
        bool field_match = false;
        if (fld && !in_xref
            && acr_nav::_db.p_cur_filtertarget->has_field_criteria
            && ch_N(acr_nav::_db.filter) > 0) {
            field_match = FieldMatchesFilter(*fld, acr_nav::_db.filter_regx, *acr_nav::_db.p_cur_filtertarget);
        }
        if (field_match) {
            EmitStyle(ctx.buf, *acr_nav::_db.p_filter_match);
        }
        if (!has_fields && acr_nav::cspan_N(*acr_nav::_db.p_cur_viewmode) > 0) {
            bool right_focused_sel = right_sel && !ctx.left_focused;
            int ov_start = -1;
            int ov_end = -1;
            acr_nav::FNavstyle *ov_style = nullptr;
            if (right_focused_sel && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode) {
                acr_nav::FViewmode &pvm = *acr_nav::_db.p_preview_viewmode;
                int n_nav = acr_nav::preview_nav_N(pvm);
                if (n_nav > 0 && acr_nav::_db.sel_nav_col < n_nav) {
                    acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_qFind(pvm, acr_nav::_db.sel_nav_col);
                    algo::strptr data_line = RightPanelLineFind(right_data_idx);
                    ov_start = DisplayToByte(data_line, nc.col_start);
                    ov_end = DisplayToByte(data_line, nc.col_start + nc.col_wid);
                    ov_style = acr_nav::_db.p_line_nav_cell;
                }
            }
            EmitStyledLine(ctx.buf, strptr(right_cell), right_focused_sel, *acr_nav::_db.p_cur_viewmode, right_data_idx, span_cursor, right_focused_sel ? acr_nav::_db.p_sel_focus : nullptr, ov_start, ov_end, ov_style);
            ctx.buf << "\x1b[0m\x1b[K\r\n";
        } else {
            ctx.buf << right_cell << "\x1b[0m\x1b[K\r\n";
        }
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
        status << "/";
        if (ch_N(acr_nav::_db.p_cur_filtertarget->label) > 0) {
            status << acr_nav::_db.p_cur_filtertarget->label;
        }
        status << acr_nav::_db.filter;
    }
    if (in_filter && ch_N(acr_nav::_db.p_cur_filtertarget->description) > 0) {
        status << "  (" << acr_nav::_db.p_cur_filtertarget->description << ")";
    }
    if (in_filter || has_filter) {
        status << "  ";
    }
    BuildStatusHint(status);
    acr_nav::FPanel &cur = *acr_nav::_db.p_cur_panel;
    if (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_graph_viewmode
        && acr_nav::_db.p_graph_ctype
        && cur.position == 1) {
        acr_nav::FField *fld = NULL;
        GraphInfoAtLine(*acr_nav::_db.p_graph_ctype, cur.sel_row, NULL, &fld);
        if (fld && ch_N(fld->p_reftype->comment) > 0) {
            status << "  " << fld->p_reftype->comment;
        }
    }
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
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    AdjustScroll(*left, acr_nav::left_item_N());
    AdjustScroll(*acr_nav::_db.p_right_panel, RightPanelItemCount(sel_ct));
    tempstr buf;
    buf << "\x1b[H";
    int wid = acr_nav::_db.term_wid;
    // Left panel width: fits longest row across all matching namespaces
    // (regardless of collapse state, so width is stable on expand/collapse)
    int max_name = 0;
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        if (ns.n_match > 0) {
            // Namespace header: " X label (count)"
            int label_len = ch_N(NsDisplayName(ns));
            int count = ns.n_match;
            int hdr_wid = 4 + label_len + 3 + DecimalDigits(count);
            max_name = i32_Max(max_name, hdr_wid);
            // Ctype rows: "    TypeName (N)" = 4 + stripped name + count width
            for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
                acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
                if (ct && ch_N(ct->ctype) > 0) {
                    algo::strptr stripped = algo::Pathcomp(ct->ctype, ".LR");
                    int count_wid = 0;
                    if (ct->c_ssimfile && ct->c_ssimfile->n_record > 0) {
                        count_wid = 3 + DecimalDigits(ct->c_ssimfile->n_record);
                    }
                    max_name = i32_Max(max_name, 4 + elems_N(stripped) + count_wid);
                }
            }
        }
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
    acr_nav::viewmode_stack_Alloc() = acr_nav::_db.p_default_viewmode->viewmode;
    acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_help_viewmode;
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
            if (forward && (vm_changed || ct_changed)) {
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
        Render(sel_ct);
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            bool repaint = acr_nav_sigwinch;
            acr_nav_sigwinch = 0;
            repaint = ProcessKey(key_name) || repaint;
            if (repaint) {
                sel_ct = SelectedCtype(*_db.p_left_panel);
                Render(sel_ct);
            }
        }
        sa.sa_handler = SIG_DFL;
        sigaction(SIGWINCH, &sa, NULL);
        ExitRawMode();
    }
}
