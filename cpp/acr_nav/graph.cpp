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
// Source: cpp/acr_nav/graph.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

static const char* G_DBL_VERT  = "\xe2\x95\x91";  // ║  double vertical (spine)
static const char* G_DBL_TL    = "\xe2\x95\x94";  // ╔  double top-left (spine start)
static const char* G_DBL_BL    = "\xe2\x95\x9a";  // ╚  double bottom-left (spine end)
static const char* G_DBL_TEE_R = "\xe2\x95\x9f";  // ╟  double vert + single right
static const char* G_DBL_TEE_L = "\xe2\x95\xa2";  // ╢  double vert + single left
static const char* G_ROUND_TL  = "\xe2\x95\xad";  // ╭  rounded top-left (neighbor open)
static const char* G_ROUND_BL  = "\xe2\x95\xb0";  // ╰  rounded bottom-left (neighbor close)
static const char* G_ARR_R     = "\xe2\x96\xb6";  // ▶  right arrow
static const char* G_ARR_L     = "\xe2\x97\x80";  // ◀  left arrow
static const char* G_VERT      = "\xe2\x94\x82";  // │  single vertical
static const char* G_HORIZ     = "\xe2\x94\x80";  // ─  single horizontal

// Line builder that tracks byte position vs display column for Unicode output.
// Spaces are 1 byte = 1 column; box-drawing chars are 3 bytes = 1 column.
struct GLine {
    cstring str;
    int extra;  // accumulated extra bytes (byte_count - display_width)
    GLine() : extra(0) {}
    void PadTo(int col) {
        int need = col - (ch_N(str) - extra);
        if (need > 0) {
            char_PrintNTimes(' ', str, need);
        }
    }
    void Ascii(const char *s) { str << s; }
    void Ascii(algo::strptr s) { str << s; }
    // Single-display-column UTF-8 character (box-drawing, arrows).
    void Utf8(const char *s) { int n = strlen(s); str << s; extra += (n - 1); }
    void RepeatUtf8(const char *s, int cnt) {
        int n = strlen(s);
        for (int i = 0; i < cnt; i++) {
            str << s;
        }
        extra += (n - 1) * cnt;
    }
    int DisplayCol() { return ch_N(str) - extra; }
    int BytePos() { return ch_N(str); }
};

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
void acr_nav::GraphInfoAtLine(acr_nav::FCtype &center, int line_idx, acr_nav::FCtype **p_node_out, acr_nav::FField **p_field_out) {
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
        int block_lines = g.n_field + 2;
        if (li == n_left - 1 && line_idx == cur_line + 1 + g.n_field) {
            // combined close -- node stays NULL
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
int acr_nav::GraphFindCtypeLine(acr_nav::FCtype &center, acr_nav::FCtype *target) {
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
        cur_line += g.n_field + 2;
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
            GLine gl;
            gl.PadTo(center_x);
            gl.Utf8(G_DBL_TL);
            gl.Ascii(" ");
            int name_start = gl.BytePos();
            gl.Ascii(ctype.ctype);
            int name_end = gl.BytePos();
            acr_nav::line_Alloc(vm) = gl.str;
            AddSpan(vm, acr_nav::line_N(vm) - 1, name_start, name_end,
                    acr_nav::_db.p_graph_ctype_style);
        }
        // Right-column blocks
        for (int ri = 0; ri < n_right; ri++) {
            GraphEdgeGroup &g = groups[right_groups[ri]];
            for (int fi = 0; fi < g.n_field; fi++) {
                acr_nav::FField &fld = *g.fields[fi];
                tempstr label;
                label << fld.reftype << " " << name_Get(fld);
                GLine gl;
                gl.PadTo(center_x);
                gl.Utf8(G_DBL_TEE_R);
                gl.Ascii(" ");
                int label_start = gl.BytePos();
                gl.Ascii(label);
                int label_end = gl.BytePos();
                gl.Ascii(" ");
                int arrow_start = gl.BytePos();
                int arrow_pad = max_right_label - ch_N(label);
                gl.RepeatUtf8(G_HORIZ, arrow_pad);
                gl.Utf8(G_ARR_R);
                int arrow_end = gl.BytePos();
                int name_start = 0, name_end = 0;
                if (fi == 0) {
                    gl.Ascii(" ");
                    name_start = gl.BytePos();
                    gl.Ascii(g.p_neighbor->ctype);
                    name_end = gl.BytePos();
                    PrintRecordCount(gl.str, *g.p_neighbor);
                } else {
                    gl.Utf8(G_VERT);
                }
                acr_nav::line_Alloc(vm) = gl.str;
                int line_idx = acr_nav::line_N(vm) - 1;
                if (fld.p_reftype->c_reftypestyle) {
                    AddSpan(vm, line_idx, label_start, label_end,
                            fld.p_reftype->c_reftypestyle->p_navstyle);
                }
                AddSpan(vm, line_idx, arrow_start, arrow_end,
                        acr_nav::_db.p_graph_arrow);
                if (fi == 0) {
                    AddSpan(vm, line_idx, name_start, name_end,
                            acr_nav::_db.p_graph_neighbor);
                }
            }
            // Close line: just spine (clean separator)
            {
                GLine gl;
                gl.PadTo(center_x);
                gl.Utf8(G_DBL_VERT);
                acr_nav::line_Alloc(vm) = gl.str;
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
                GLine gl;
                gl.PadTo(name_x);
                gl.Utf8(G_ROUND_TL);
                gl.Ascii(" ");
                int nb_start = gl.BytePos();
                gl.Ascii(g.p_neighbor->ctype);
                int nb_end = gl.BytePos();
                PrintRecordCount(gl.str, *g.p_neighbor);
                gl.PadTo(center_x);
                gl.Utf8(G_DBL_TEE_L);
                acr_nav::line_Alloc(vm) = gl.str;
                AddSpan(vm, acr_nav::line_N(vm) - 1, nb_start, nb_end,
                        acr_nav::_db.p_graph_neighbor);
            }
            // Edge lines
            for (int fi = 0; fi < g.n_field; fi++) {
                acr_nav::FField &fld = *g.fields[fi];
                tempstr label;
                label << fld.reftype << " " << name_Get(fld);
                GLine gl;
                gl.PadTo(name_x);
                gl.Utf8(G_VERT);
                int arrow_start = gl.BytePos();
                gl.Utf8(G_ARR_L);
                int dash_len = center_x - name_x - 2;
                gl.RepeatUtf8(G_HORIZ, dash_len);
                int arrow_end = gl.BytePos();
                gl.Utf8(G_DBL_TEE_L);
                gl.Ascii(" ");
                int label_start = gl.BytePos();
                gl.Ascii(label);
                int label_end = gl.BytePos();
                acr_nav::line_Alloc(vm) = gl.str;
                int line_idx = acr_nav::line_N(vm) - 1;
                AddSpan(vm, line_idx, arrow_start, arrow_end,
                        acr_nav::_db.p_graph_arrow);
                if (fld.p_reftype->c_reftypestyle) {
                    AddSpan(vm, line_idx, label_start, label_end,
                            fld.p_reftype->c_reftypestyle->p_navstyle);
                }
            }
            // Close lines
            if (li == n_left - 1) {
                // Combined close: neighbor ends + spine ends on same line
                {
                    GLine gl;
                    gl.PadTo(name_x);
                    gl.Utf8(G_ROUND_BL);
                    gl.PadTo(center_x);
                    gl.Utf8(G_DBL_BL);
                    acr_nav::line_Alloc(vm) = gl.str;
                }
            } else {
                // Non-last left close: neighbor ends, spine continues
                {
                    GLine gl;
                    gl.PadTo(name_x);
                    gl.Utf8(G_ROUND_BL);
                    gl.PadTo(center_x);
                    gl.Utf8(G_DBL_VERT);
                    acr_nav::line_Alloc(vm) = gl.str;
                }
            }
        }
        // If no left blocks, close center with standalone line
        if (n_left == 0) {
            GLine gl;
            gl.PadTo(center_x);
            gl.Utf8(G_DBL_BL);
            acr_nav::line_Alloc(vm) = gl.str;
        }
    }
}

void acr_nav::GraphEnsureContent(void *, acr_nav::FCtype &ct) {
    if (acr_nav::_db.p_graph_ctype != &ct) {
        LoadGraph(ct);
    }
}
