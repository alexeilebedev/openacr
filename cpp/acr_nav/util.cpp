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
// Source: cpp/acr_nav/util.cpp -- Shared utility functions
//

#include "include/algo.h"
#include "include/acr_nav.h"

// -----------------------------------------------------------------------------

void acr_nav::WriteStdout(const char *buf, int len) {
    ssize_t rc = write(STDOUT_FILENO, buf, len);
    (void)rc;
}

// -----------------------------------------------------------------------------

void acr_nav::DetectTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    acr_nav::_db.term_wid = w.ws_col;
    acr_nav::_db.term_hei = w.ws_row;
}

void acr_nav::SwitchToBrowse() {
    acr_nav::_db.p_cur_mode = acr_nav::ind_navmode_Find("browse");
    vrfy(acr_nav::_db.p_cur_mode, "navmode 'browse' not found");
}

// -----------------------------------------------------------------------------

int acr_nav::DecimalDigits(int n) {
    int d = 1;
    while (n >= 10) { n /= 10; d++; }
    return d;
}

// True if field matches regex under the current filtertarget's boolean flags.
bool acr_nav::FieldMatchesFilter(acr_nav::FField &fld, algo_lib::Regx &regx, acr_nav::FFiltertarget &ft) {
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
bool acr_nav::CtypeMatchesFilter(acr_nav::FCtype &ct, algo_lib::Regx &regx, acr_nav::FFiltertarget &ft) {
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

acr_nav::FCtype* acr_nav::SelectedCtype(acr_nav::FPanel &left) {
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
acr_nav::FNs* acr_nav::SelectedNs() {
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
bool acr_nav::IsHelpMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_help_viewmode;
}

bool acr_nav::IsDetailMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_detail_viewmode;
}

bool acr_nav::IsNsDepMode() {
    return acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_nsdep_viewmode;
}

// -----------------------------------------------------------------------------

int acr_nav::RightPanelLineCount() {
    return acr_nav::line_N(*acr_nav::_db.p_cur_viewmode);
}

algo::strptr acr_nav::RightPanelLineFind(int idx) {
    return acr_nav::line_qFind(*acr_nav::_db.p_cur_viewmode, idx);
}

algo::strptr acr_nav::RightPanelLineHeader() {
    return acr_nav::_db.p_cur_viewmode->header;
}

// Find the ssimfile backing a ctype. If the ctype itself has no ssimfile,
// follow the Base field to the underlying type (e.g. abt.FTarget -> dev.Target).
acr_nav::FSsimfile* acr_nav::FindSsimfile(acr_nav::FCtype &ctype) {
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
int acr_nav::PrintRecordCount(cstring &out, acr_nav::FCtype &ctype) {
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
void acr_nav::ClearViewmodeLines(acr_nav::FViewmode &vm) {
    acr_nav::line_RemoveAll(vm);
    acr_nav::cspan_RemoveAll(vm);
    acr_nav::preview_nav_RemoveAll(vm);
}

// Reset a viewmode to empty: clear lines, spans, nav columns, header, and h-scroll.
void acr_nav::ResetViewmodeContent(acr_nav::FViewmode &vm) {
    ClearViewmodeLines(vm);
    vm.header = "";
    vm.preview_h_scroll = 0;
}

// Invalidate cached content for content-loading viewmodes (preview, codegen, graph).
// Called when the selected ctype becomes NULL (namespace header row), so stale
// content from the previous ctype is not displayed.
// nsdep excluded: caches by namespace, handles NULL selection in RightPanelItemCount.
void acr_nav::InvalidateContentCaches() {
    ResetViewmodeContent(*acr_nav::_db.p_preview_viewmode);
    ResetViewmodeContent(*acr_nav::_db.p_codegen_viewmode);
    ResetViewmodeContent(*acr_nav::_db.p_graph_viewmode);
    acr_nav::_db.p_preview_ctype = NULL;
    acr_nav::_db.p_codegen_ctype = NULL;
    acr_nav::_db.p_graph_ctype = NULL;
}

// True if the byte range [byte_start, byte_end) in line contains at least one non-space character.
bool acr_nav::RegionHasContent(algo::strptr line, int byte_start, int byte_end) {
    bool ret = false;
    int end = i32_Min(byte_end, elems_N(line));
    for (int i = i32_Max(byte_start, 0); i < end && !ret; i++) {
        ret = line.elems[i] != ' ';
    }
    return ret;
}

// Add a color span to a viewmode. Positions are 0-based relative to stored line text.
// Spans must be emitted in line_idx then col_start order. No overlapping spans.
// Caller must ensure line_idx < line_N(vm).
// Whitespace-only regions are silently skipped.
void acr_nav::AddSpan(acr_nav::FViewmode &vm, int line_idx, int col_start, int col_end, acr_nav::FNavstyle *p_style) {
    if (col_start < col_end && p_style
        && RegionHasContent(acr_nav::line_qFind(vm, line_idx), col_start, col_end)) {
        acr_nav::LineColorSpan &span = acr_nav::cspan_Alloc(vm);
        span.line_idx = line_idx;
        span.col_start = col_start;
        span.col_end = col_end;
        span.p_navstyle = p_style;
    }
}

// Replace control characters (< 0x20) and DEL (0x7F) with '.' for safe terminal display.
// 1:1 byte replacement preserves string length for column alignment.
void acr_nav::SanitizeForDisplay(cstring &str) {
    for (int i = 0; i < ch_N(str); i++) {
        unsigned char c = (unsigned char)str.ch_elems[i];
        if (c < 0x20 || c == 0x7F) {
            str.ch_elems[i] = '.';
        }
    }
}

// Count extra bytes from UTF-8 multibyte characters (byte_length - display_width).
int acr_nav::Utf8ExtraBytes(algo::strptr s) {
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
int acr_nav::DisplayToByte(algo::strptr s, int display_col) {
    int dcol = 0, i = 0;
    while (i < s.n_elems && dcol < display_col) {
        unsigned char c = s.elems[i];
        int char_bytes = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        dcol++;
        i = i32_Min(i + char_bytes, s.n_elems);
    }
    return i;
}

// -----------------------------------------------------------------------------

algo::strptr acr_nav::NsDisplayName(acr_nav::FNs &ns) {
    return ch_N(ns.ns) > 0 ? algo::strptr(ns.ns) : algo::strptr("other");
}

// -----------------------------------------------------------------------------

acr_nav::FField* acr_nav::RightPanelFieldFind(acr_nav::FCtype *sel_ct, int idx) {
    acr_nav::FField *ret = NULL;
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        ret = reverse ? c_field_arg_Find(*sel_ct, idx) : c_field_Find(*sel_ct, idx);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Number of content rows available for the dual-panel display.
// Subtracts title bar (1) and status bar (1), plus breadcrumb bar (1) when navstack is non-empty.
int acr_nav::VisibleRows() {
    int chrome = 2 + (acr_nav::navstack_N() > 0 ? 1 : 0);
    return i32_Max(1, acr_nav::_db.term_hei - chrome);
}

// Number of data rows visible in each panel (VisibleRows minus the column header row).
int acr_nav::DataRows() {
    return VisibleRows() - 1;
}

void acr_nav::AdjustScroll(acr_nav::FPanel &panel, int n_items) {
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
