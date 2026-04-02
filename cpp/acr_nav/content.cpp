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
// Source: cpp/acr_nav/content.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

static const char* G_HORIZ = "\xe2\x94\x80";  // ─  single horizontal

using acr_nav::AddSpan;
using acr_nav::ClearViewmodeLines;
using acr_nav::FindSsimfile;
using acr_nav::SanitizeForDisplay;
using acr_nav::Utf8ExtraBytes;
using acr_nav::DisplayToByte;
using acr_nav::NsDisplayName;
using acr_nav::IsNsDepMode;
using acr_nav::SelectedNs;
using acr_nav::RightPanelLineCount;
using acr_nav::DecimalDigits;

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
    vm.preview_h_scroll = 0;
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
                    acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_Alloc(vm);
                    nc.col_start = col_pos;
                    nc.col_wid = display_wid[c];
                    nc.name_len = ch_N(col_name[c]);
                    nc.col_name = col_name[c];
                    tempstr qname;
                    qname << field_base->ctype << "." << col_name[c];
                    acr_nav::FField *fld = acr_nav::ind_field_Find(qname);
                    if (fld && fld->p_reftype->up && FindSsimfile(*fld->p_arg)) {
                        nc.target_ctype = fld->p_arg->ctype;
                    }
                    col_pos += display_wid[c];
                }
                vm.total_content_wid = col_pos;
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
                    // Highlight comment column
                    if (comment_col >= 0) {
                        AddSpan(vm, li, col_byte_pos[comment_col], ch_N(row), acr_nav::_db.p_line_comment);
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
    if (acr_nav::_db.sel_nav_col_pending >= 0) {
        acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col_pending;
        acr_nav::_db.sel_nav_col_pending = -1;
    } else {
        acr_nav::_db.sel_nav_col = 0;
    }
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

// Extract namespace pointer from a nsdep viewmode line by finding the line_key
// color span (which highlights the namespace name) and looking up the namespace.
// Returns NULL for header, separator, or "(none)" lines.
acr_nav::FNs* acr_nav::NsDepNsAtLine(acr_nav::FViewmode &vm, int line_idx) {
    acr_nav::FNs *ret = NULL;
    if (line_idx >= 0 && line_idx < acr_nav::line_N(vm)) {
        algo::strptr line_text = acr_nav::line_qFind(vm, line_idx);
        bool found = false;
        for (int si = 0; si < acr_nav::cspan_N(vm) && !found; si++) {
            acr_nav::LineColorSpan &span = acr_nav::cspan_qFind(vm, si);
            if (span.line_idx == line_idx && span.p_navstyle == acr_nav::_db.p_line_key) {
                int end = i32_Min(span.col_end, elems_N(line_text));
                if (span.col_start < end) {
                    algo::strptr name(line_text.elems + span.col_start, end - span.col_start);
                    ret = acr_nav::ind_ns_Find(name);
                    if (!ret && algo::strptr_Cmp(name, "other") == 0) {
                        ret = acr_nav::ind_ns_Find("");
                    }
                }
                found = true;
            }
        }
    }
    return ret;
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
void acr_nav::LoadNsDep(acr_nav::FNs &ns) {
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
void acr_nav::PreviewEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_preview_ctype != &ct) {
        LoadPreview(ct);
    }
}

void acr_nav::CodegenEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_codegen_ctype != &ct) {
        LoadCodegen(ct);
    }
}

void acr_nav::NsDepEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_nsdep_ns != ct.p_ns) {
        LoadNsDep(*ct.p_ns);
    }
}

// Emit a section header line: "── title ──────..." with line_section highlight.
static void EmitSectionHeader(acr_nav::FViewmode &vm, algo::strptr title) {
    tempstr hdr;
    hdr << G_HORIZ << G_HORIZ << " " << title << " ";
    int display_width = ch_N(hdr) - Utf8ExtraBytes(strptr(hdr));
    int fill = i32_Max(0, 36 - display_width);
    for (int i = 0; i < fill; i++) hdr << G_HORIZ;
    acr_nav::line_Alloc(vm) = hdr;
    AddSpan(vm, acr_nav::line_N(vm) - 1, 0, ch_N(hdr), acr_nav::_db.p_line_section);
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
    tempstr head_str;
    head_str << tuple.head;
    EmitSectionHeader(vm, strptr(head_str));
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
void acr_nav::LoadDetail(acr_nav::FField &field) {
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

int acr_nav::RightPanelItemCount(acr_nav::FCtype *sel_ct) {
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

// -----------------------------------------------------------------------------

// Panel item count dispatches on position (0=ctype list, 1=field list).
// The two panels have structurally different roles (parent/child),
// not N instances of one concept -- factoring into step hooks
// would move the if elsewhere without reducing complexity.
int acr_nav::PanelItemCount(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (panel.position == 0) {
        ret = acr_nav::left_item_N();
    } else if (panel.position == 1) {
        ret = RightPanelItemCount(sel_ct);
    }
    return ret;
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
    bool ret = false;
    algo::strptr na(a->navaction);
    algo::strptr nb(b->navaction);
    int prefix = 0;
    while (prefix < na.n_elems && prefix < nb.n_elems && na.elems[prefix] == nb.elems[prefix]) {
        prefix++;
    }
    if (prefix > 0 && prefix < na.n_elems && prefix < nb.n_elems) {
        algo::strptr sa(na.elems + prefix, na.n_elems - prefix);
        algo::strptr sb(nb.elems + prefix, nb.n_elems - prefix);
        ret = (sa == "up" && sb == "down")
            || (sa == "left" && sb == "right")
            || (sa == "top" && sb == "bottom");
    }
    return ret;
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
void acr_nav::BuildHelpLines() {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_help_viewmode;
    ClearViewmodeLines(vm);
    vm.header = "Keyboard Shortcuts";
    // Collect helpgroups sorted by sort_order
    acr_nav::FHelpgroup *groups[16]; // fixed capacity; silently truncates if exceeded
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
        EmitSectionHeader(vm, groups[g]->comment);
        // Collect navactions in this group, sorted by sort_order
        acr_nav::FNavaction *actions[32]; // fixed capacity; silently truncates if exceeded
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
