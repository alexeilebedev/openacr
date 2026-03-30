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

static int DecimalDigits(int n) {
    int d = 1;
    while (n >= 10) { n /= 10; d++; }
    return d;
}

// True if field name or comment matches regex.
static bool FieldMatchesFilter(acr_nav::FField &fld, algo_lib::Regx &regx) {
    tempstr lower_name(name_Get(fld));
    algo::MakeLower(lower_name);
    bool match = algo_lib::Regx_Match(regx, lower_name);
    if (!match) {
        tempstr lower_comment(algo::strptr(fld.comment));
        algo::MakeLower(lower_comment);
        match = algo_lib::Regx_Match(regx, lower_comment);
    }
    return match;
}

// -----------------------------------------------------------------------------

// True if ctype passes the current filter.  When is_field_target is false,
// matches against the ctype name; when true, matches against any field's
// name or comment via FieldMatchesFilter.
static bool CtypeMatchesFilter(acr_nav::FCtype &ct, algo_lib::Regx &regx, bool is_field_target) {
    bool match = false;
    if (!is_field_target) {
        tempstr lower_name(ct.ctype);
        algo::MakeLower(lower_name);
        match = algo_lib::Regx_Match(regx, lower_name);
    } else {
        for (int f = 0; f < acr_nav::c_field_N(ct) && !match; f++) {
            acr_nav::FField *fld = acr_nav::c_field_Find(ct, f);
            if (fld) {
                match = FieldMatchesFilter(*fld, regx);
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
    bool is_field_target = (acr_nav::_db.p_cur_filtertarget != acr_nav::_db.p_default_filtertarget);
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
                bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, is_field_target);
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
                    bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, is_field_target);
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
    if (left.sel_row >= 0 && left.sel_row < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(left.sel_row);
        if (ch_N(item.ctype) > 0) {
            ret = acr_nav::ind_ctype_Find(item.ctype);
        }
    }
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
// detail: requires has_fields + selected field). The is_overlay property on
// viewmode records captures overlay state for the hint system; the navaction
// functions are not unified because their entry guards differ.
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

// If an overlay viewmode is active and the selected ctype changed, pop all
// overlays and restore the base viewmode.  During startup help, preserve the
// overlay so movement doesn't dismiss it.
static void PopOverlayOnCtypeChange(acr_nav::FCtype *prev_sel_ct, acr_nav::FCtype *sel_ct) {
    if (sel_ct != prev_sel_ct && !acr_nav::viewmode_stack_EmptyQ() && !acr_nav::_db.startup_help) {
        acr_nav::FViewmode *base = acr_nav::ind_viewmode_Find(acr_nav::viewmode_stack_qFind(0));
        acr_nav::viewmode_stack_RemoveAll();
        if (base) {
            acr_nav::_db.p_cur_viewmode = base;
        }
        acr_nav::_db.p_detail_field = NULL;
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

// Load ssimfile content into the preview viewmode's line Tary, stripping the tuple head from each line.
// Format a single row of attr values into an aligned column string.
static void FormatPreviewRow(cstring &out, algo::Tuple &tuple, int *col_wid, int n_col) {
    int ci = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        if (ci < n_col) {
            if (ci > 0) {
                out << "  ";
            }
            tempstr safe(attr.value);
            SanitizeForDisplay(safe);
            out << safe;
            char_PrintNTimes(' ', out, col_wid[ci] - ch_N(safe));
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

// Load amc-generated C++ struct definition for a ctype into the codegen viewmode.
// Ctype names come from trusted ssimfile data loaded at startup.
static void LoadCodegen(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_codegen_viewmode;
    acr_nav::line_RemoveAll(vm);
    acr_nav::_db.p_codegen_ctype = &ctype;
    tempstr cmd;
    cmd << "amc '" << ctype.ctype << "'";
    vm.header = cmd;
    tempstr output = SysEval(cmd, FailokQ(true), 64*1024);
    ind_beg(Line_curs, line, output) {
        if (!StartsWithQ(line, "report.")) {
            acr_nav::line_Alloc(vm) = line;
        }
    } ind_end;
}

static void EnsureCodegenLoaded(acr_nav::FCtype *sel_ct) {
    if (sel_ct && acr_nav::_db.p_codegen_ctype != sel_ct) {
        LoadCodegen(*sel_ct);
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
    acr_nav::line_RemoveAll(vm);
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
        if (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode && sel_ct) {
            EnsurePreviewLoaded(sel_ct);
        }
        // Future: an ensure-content hook on FViewmode would eliminate per-viewmode checks here
        if (acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_codegen_viewmode && sel_ct) {
            EnsureCodegenLoaded(sel_ct);
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
        int sel = left->sel_row;
        if (sel >= 0 && sel < acr_nav::left_item_N()) {
            acr_nav::LeftItem &item = acr_nav::left_item_qFind(sel);
            if (ch_N(item.ctype) == 0) {
                // Namespace header: toggle collapse
                acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
                if (ns) {
                    ns->collapsed = !ns->collapsed;
                    BuildLeftItems();
                }
            } else {
                acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
            }
        }
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
            entry.right_sel_row = acr_nav::_db.p_right_panel->sel_row;
            entry.right_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
            entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
            entry.ctype = sel_ct->ctype;
            entry.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
            entry.focus_panel = acr_nav::_db.p_cur_panel->panel;
            // Reset to forward view on navigation -- new ctype starts in its natural view
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_default_viewmode;
            // Switch to browse mode with full ctype list so target is reachable
            acr_nav::_db.filter = "";
            acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
            SwitchToBrowse();
            target->p_ns->collapsed = false;  // ensure target namespace is expanded
            BuildLeftItems();
            for (int i = 0; i < acr_nav::left_item_N(); i++) {
                if (algo::strptr(acr_nav::left_item_qFind(i).ctype) == algo::strptr(target->ctype)) {
                    left->sel_row = i;
                }
            }
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
            if (algo::strptr(acr_nav::left_item_qFind(i).ctype) == algo::strptr(entry->ctype)) {
                acr_nav::_db.p_left_panel->sel_row = i;
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

void acr_nav::navaction_filter_accept() {
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        // Expand all namespaces that have matching ctypes so results are visible
        ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
            if (ns.n_match > 0) {
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
                || algo::strptr(na.dismiss_viewmode) == cur_vm);
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
        // Build a line for each navaction
        for (int a = 0; a < n_actions; a++) {
            tempstr std_keys;
            tempstr short_keys;
            // First pass: browse-mode keybinds
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
            // Second pass: filter-mode keybinds only if no browse keys found
            if (ch_N(std_keys) == 0 && ch_N(short_keys) == 0) {
                ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
                    if (kb.p_navaction == actions[a] && acr_nav::navmode_Get(kb) == "filter") {
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
            }
            tempstr line;
            line << std_keys;
            char_PrintNTimes(' ', line, i32_Max(1, 18 - ch_N(line)));
            line << short_keys;
            char_PrintNTimes(' ', line, i32_Max(1, 28 - ch_N(line)));
            line << actions[a]->comment;
            acr_nav::line_Alloc(vm) = line;
        }
    }
    acr_nav::line_Alloc(vm) = "";
    acr_nav::line_Alloc(vm) = "  \xe2\x86\x91\xe2\x86\x93 Browse    Enter Start    ? Help anytime";
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

// Render the content area: dual-panel data rows.
static void RenderContentArea(RenderCtx &ctx) {
    int n_left = acr_nav::left_item_N();
    int scroll = acr_nav::_db.p_left_panel->scroll_offset;
    int n_right = RightPanelItemCount(ctx.sel_ct);
    bool has_fields = acr_nav::_db.p_cur_viewmode->has_fields;
    bool in_xref = IsXrefMode();
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
        EmitStyle(ctx.buf, !ctx.left_focused ? *acr_nav::_db.p_title_focus : *acr_nav::_db.p_title_nofocus);
        ctx.buf << hdr << "\x1b[0m\x1b[K\r\n";
        visible--;
    }

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
                left_cell << (ch_N(item.ns) > 0 ? algo::strptr(item.ns) : algo::strptr("extern"));
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
                acr_nav::FSsimfile *ssf = ct ? FindSsimfile(*ct) : nullptr;
                if (ssf && ssf->n_record > 0) {
                    left_cell << " (" << ssf->n_record << ")";
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
            if (right_sel) {
                EmitStyle(ctx.buf, !ctx.left_focused ? *acr_nav::_db.p_sel_focus : *acr_nav::_db.p_sel_nofocus);
            }
            if (fld && fld->p_reftype->c_reftypestyle) {
                EmitStyle(ctx.buf, *fld->p_reftype->c_reftypestyle->p_navstyle);
            }
            bool field_match = false;
            if (fld && !in_xref
                && acr_nav::_db.p_cur_filtertarget != acr_nav::_db.p_default_filtertarget
                && ch_N(acr_nav::_db.filter) > 0) {
                field_match = FieldMatchesFilter(*fld, acr_nav::_db.filter_regx);
            }
            if (field_match) {
                EmitStyle(ctx.buf, *acr_nav::_db.p_filter_match);
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
        status << "/";
        if (ch_N(acr_nav::_db.p_cur_filtertarget->label) > 0) {
            status << acr_nav::_db.p_cur_filtertarget->label;
        }
        status << acr_nav::_db.filter;
    }
    if (in_filter || has_filter) {
        status << "  ";
    }
    BuildStatusHint(status);
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
    // Left panel width: fits longest row across all matching namespaces
    // (regardless of collapse state, so width is stable on expand/collapse)
    int max_name = 0;
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        if (ns.n_match > 0) {
            // Namespace header: " X label (count)"
            int label_len = ch_N(ns.ns) > 0 ? ch_N(ns.ns) : 6;  // "extern" = 6
            int count = ns.n_match;
            int hdr_wid = 4 + label_len + 3 + DecimalDigits(count);
            max_name = i32_Max(max_name, hdr_wid);
            // Ctype rows: "    TypeName (N)" = 4 + stripped name + count width
            for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
                acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
                if (ct && ch_N(ct->ctype) > 0) {
                    algo::strptr stripped = algo::Pathcomp(algo::strptr(ct->ctype), ".LR");
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
    acr_nav::_db.p_xref_viewmode = acr_nav::ind_viewmode_Find("xref");
    acr_nav::_db.p_help_viewmode = acr_nav::ind_viewmode_Find("help");
    acr_nav::_db.p_detail_viewmode = acr_nav::ind_viewmode_Find("detail");
    acr_nav::_db.p_codegen_viewmode = acr_nav::ind_viewmode_Find("codegen");
    vrfy(acr_nav::_db.p_default_viewmode, "viewmode 'fields' not found");
    vrfy(acr_nav::_db.p_preview_viewmode, "viewmode 'preview' not found");
    vrfy(acr_nav::_db.p_xref_viewmode, "viewmode 'xref' not found");
    vrfy(acr_nav::_db.p_help_viewmode, "viewmode 'help' not found");
    vrfy(acr_nav::_db.p_detail_viewmode, "viewmode 'detail' not found");
    vrfy(acr_nav::_db.p_codegen_viewmode, "viewmode 'codegen' not found");
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
    // Resolve well-known filtertarget pointers
    acr_nav::_db.p_default_filtertarget = acr_nav::ind_filtertarget_Find("ctype");
    vrfy(acr_nav::_db.p_default_filtertarget, "filtertarget 'ctype' not found");
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    acr_nav::_db.p_filter_match = acr_nav::ind_navstyle_Find("filter_match");
    vrfy(acr_nav::_db.p_filter_match, "navstyle 'filter_match' not found");
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
                && algo::strptr(na.dismiss_viewmode) == algo::strptr(acr_nav::_db.p_cur_viewmode->viewmode)) {
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
            PopOverlayOnCtypeChange(prev_sel_ct, sel_ct);
            bool forward = (acr_nav::navstack_N() >= prev_depth);
            bool vm_changed = (acr_nav::_db.p_cur_viewmode != prev_viewmode);
            bool ct_changed = (sel_ct != prev_sel_ct);
            if (forward && (vm_changed || ct_changed)) {
                right->sel_row = 0;
                right->scroll_offset = 0;
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
    int last_left = i32_Min(first_left + VisibleRows(), acr_nav::left_item_N());
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
        int n_vis = RightPanelItemCount(sel_ct);
        int first_right = right->scroll_offset;
        int last_right = i32_Min(first_right + VisibleRows(), n_vis);
        for (int i = first_right; i < last_right; i++) {
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
                vf.navigable = navigable;
                bool field_match = false;
                if (!IsXrefMode()
                    && acr_nav::_db.p_cur_filtertarget != acr_nav::_db.p_default_filtertarget
                    && ch_N(acr_nav::_db.filter) > 0) {
                    field_match = FieldMatchesFilter(*field, acr_nav::_db.filter_regx);
                }
                vf.match = field_match;
                prlog(vf);
            }
        }
    } else {
        // Visible lines (text-based modes: help, preview, detail, clipped to viewport)
        int n_lines = RightPanelItemCount(sel_ct);
        int first_line = right->scroll_offset;
        int last_line = i32_Min(first_line + VisibleRows(), n_lines);
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
