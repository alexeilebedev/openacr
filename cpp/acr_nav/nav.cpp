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
// Source: cpp/acr_nav/nav.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

using acr_nav::CtypeMatchesFilter;
using acr_nav::SelectedCtype;
using acr_nav::SelectedNs;
using acr_nav::PanelItemCount;
using acr_nav::RightPanelItemCount;
using acr_nav::SwitchToBrowse;
using acr_nav::IsHelpMode;
using acr_nav::IsDetailMode;
using acr_nav::IsNsDepMode;
using acr_nav::NsDepNsAtLine;
using acr_nav::NsDisplayName;
using acr_nav::RightPanelLineFind;
using acr_nav::RightPanelFieldFind;
using acr_nav::DisplayToByte;
using acr_nav::GraphInfoAtLine;
using acr_nav::DataRows;
using acr_nav::FindSsimfile;
using acr_nav::LoadNsDep;
using acr_nav::LoadDetail;
using acr_nav::BuildLeftItems;
using acr_nav::PopViewmode;
using acr_nav::PushOverlay;
using acr_nav::PopOverlayOnCtypeChange;
using acr_nav::DismissStartupHelp;
using acr_nav::BuildLeftItemsReset;

// -----------------------------------------------------------------------------

void acr_nav::BuildLeftItems() {
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
    acr_nav::FNs *ns_arr[256]; // fixed capacity; silently truncates if exceeded
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

void acr_nav::BuildLeftItemsReset() {
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

// Used by show_help toggle, show_detail toggle, and startup help dismiss.
void acr_nav::PopViewmode() {
    if (!acr_nav::viewmode_stack_EmptyQ()) {
        acr_nav::OverlayEntry &entry = acr_nav::viewmode_stack_qLast();
        acr_nav::FViewmode *prev = acr_nav::ind_viewmode_Find(entry.viewmode);
        if (prev) {
            acr_nav::_db.p_cur_viewmode = prev;
            acr_nav::_db.p_right_panel->sel_row = entry.saved_sel_row;
            acr_nav::_db.p_right_panel->scroll_offset = entry.saved_scroll_offset;
        }
        acr_nav::viewmode_stack_RemoveLast();
    }
}

// Push current viewmode + right-panel state onto overlay stack, switch to target.
void acr_nav::PushOverlay(acr_nav::FViewmode *target) {
    acr_nav::OverlayEntry &entry = acr_nav::viewmode_stack_Alloc();
    entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    entry.saved_sel_row = acr_nav::_db.p_right_panel->sel_row;
    entry.saved_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
    acr_nav::_db.p_cur_viewmode = target;
}

// Dismiss startup help on any non-passive action (not movement/panel switch).
// Escape/? pop the overlay directly; detected here via !IsHelpMode().
void acr_nav::DismissStartupHelp(acr_nav::FKeybind *keybind) {
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
// nsdep is a context-sensitive view: auto-activates on namespace headers,
// restores the previous viewmode when leaving.
bool acr_nav::PopOverlayOnCtypeChange(acr_nav::FCtype *prev_sel_ct, acr_nav::FCtype *sel_ct) {
    bool nsdep_changed = false;
    // nsdep context: activate on ns headers, deactivate on ctype rows, reload on ns change
    if (IsNsDepMode()) {
        if (sel_ct) {
            // Leaving ns header for ctype row — restore saved viewmode
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_pre_nsdep_viewmode
                ? acr_nav::_db.p_pre_nsdep_viewmode : acr_nav::_db.p_default_viewmode;
            acr_nav::_db.p_pre_nsdep_viewmode = NULL;
            nsdep_changed = true;
        } else {
            acr_nav::FNs *ns = SelectedNs();
            if (ns && ns != acr_nav::_db.p_nsdep_ns) {
                LoadNsDep(*ns);
                nsdep_changed = true;
            }
        }
    }
    bool changed = sel_ct != prev_sel_ct || nsdep_changed;
    if (changed && !acr_nav::viewmode_stack_EmptyQ() && !acr_nav::_db.startup_help) {
        acr_nav::FViewmode *base = acr_nav::ind_viewmode_Find(acr_nav::viewmode_stack_qFind(0).viewmode);
        // RemoveAll intentionally bypasses PopViewmode — saved panel state is
        // discarded because ct_changed triggers a reset in ProcessKey.
        acr_nav::viewmode_stack_RemoveAll();
        if (base) {
            acr_nav::_db.p_cur_viewmode = base;
        }
        acr_nav::_db.p_detail_field = NULL;
        // If overlay-pop restored nsdep as base but we landed on a ctype row,
        // deactivate nsdep immediately (same logic as the IsNsDepMode block above).
        if (IsNsDepMode() && sel_ct) {
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_pre_nsdep_viewmode
                ? acr_nav::_db.p_pre_nsdep_viewmode : acr_nav::_db.p_default_viewmode;
            acr_nav::_db.p_pre_nsdep_viewmode = NULL;
            nsdep_changed = true;
        }
    }
    // Auto-activate nsdep when landing on a namespace-header row
    if (!IsNsDepMode() && !sel_ct && !acr_nav::_db.p_cur_viewmode->is_overlay) {
        acr_nav::FNs *ns = SelectedNs();
        if (ns) {
            if (acr_nav::_db.startup_help) {
                acr_nav::_db.startup_help = false;
                if (IsHelpMode()) {
                    PopViewmode();
                }
            }
            acr_nav::_db.p_pre_nsdep_viewmode = acr_nav::_db.p_cur_viewmode;
            LoadNsDep(*ns);
            acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_nsdep_viewmode;
            nsdep_changed = true;
        }
    }
    return nsdep_changed;
}

// -----------------------------------------------------------------------------

// Push a snapshot of the current navigation state onto the navstack.
// entry_ctype: the ctype key (or namespace display name for nsdep) to record.
static void PushNaventry(algo::strptr entry_ctype) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::Naventry &entry = acr_nav::navstack_Alloc();
    entry.filter = acr_nav::_db.filter;
    entry.navmode = acr_nav::_db.p_cur_mode->navmode;
    entry.scroll_offset = left->scroll_offset;
    entry.sel_row = left->sel_row;
    entry.right_sel_row = acr_nav::_db.p_right_panel->sel_row;
    entry.right_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
    entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    entry.ctype = entry_ctype;
    entry.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
    entry.focus_panel = acr_nav::_db.p_cur_panel->panel;
    entry.sel_nav_col = acr_nav::_db.sel_nav_col;
}

// -----------------------------------------------------------------------------

// Push navstack, navigate to target ctype, set dest_viewmode.
// Shared by field/xref follow and graph follow.
static void NavigateToTarget(acr_nav::FCtype *sel_ct, acr_nav::FCtype *target, acr_nav::FViewmode *dest_viewmode) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    PushNaventry(sel_ct->ctype);
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

// -----------------------------------------------------------------------------

// Navigate directly to a ctype by key. Used by headless Navigate command.
// If sel_ct is non-null (already viewing a ctype), pushes navstack via NavigateToTarget.
// If sel_ct is null (initial state on namespace header), navigates without pushing navstack.
// Returns the target FCtype, or nullptr if not found.
acr_nav::FCtype* acr_nav::GoToCtype(algo::strptr ctype_key, acr_nav::FViewmode *dest_viewmode) {
    acr_nav::FCtype *target = acr_nav::ind_ctype_Find(ctype_key);
    if (target) {
        acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
        acr_nav::FCtype *sel_ct = SelectedCtype(*left);
        if (sel_ct) {
            // Already viewing a ctype — push navstack and navigate
            NavigateToTarget(sel_ct, target, dest_viewmode);
        } else {
            // Initial state (cursor on namespace header) — navigate without pushing navstack
            target->p_ns->collapsed = false;
            acr_nav::_db.filter = "";
            acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
            SwitchToBrowse();
            BuildLeftItems();
            for (int i = 0; i < acr_nav::left_item_N(); i++) {
                if (acr_nav::left_item_qFind(i).ctype == target->ctype) {
                    left->sel_row = i;
                    break;
                }
            }
            acr_nav::_db.p_cur_viewmode = dest_viewmode;
        }
    }
    return target;
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
        if (n_nav > 0 && acr_nav::_db.sel_nav_col < n_nav - 1) {
            acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col + 1;
        }
    }
}

// -----------------------------------------------------------------------------

// Left panel Enter: toggle namespace collapse or switch to right panel.
static void FollowRefLeftPanel(acr_nav::FPanel *left) {
    int sel = left->sel_row;
    if (sel >= 0 && sel < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(sel);
        if (ch_N(item.ctype) == 0) {
            acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
            if (ns) {
                ns->collapsed = !ns->collapsed;
                if (ns->collapsed) {
                    ns->auto_expanded = false;
                }
                BuildLeftItems();
            }
        } else {
            // Leave nsdep context so right panel returns to fields view
            if (IsNsDepMode()) {
                acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_pre_nsdep_viewmode
                    ? acr_nav::_db.p_pre_nsdep_viewmode : acr_nav::_db.p_default_viewmode;
                acr_nav::_db.p_pre_nsdep_viewmode = NULL;
            }
            acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
        }
    }
}

// -----------------------------------------------------------------------------

// Graph mode Enter: navigate to the neighbor ctype on the selected line.
static void FollowRefGraph(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    acr_nav::FCtype *target = NULL;
    GraphInfoAtLine(*sel_ct, panel.sel_row, &target, NULL);
    if (target && target != sel_ct) {
        NavigateToTarget(sel_ct, target, acr_nav::_db.p_graph_viewmode);
    }
}

// -----------------------------------------------------------------------------

// NsDep mode Enter: jump to the namespace on the selected line.
// Keeps the target namespace collapsed and selects its header row.
static void FollowRefNsDep(acr_nav::FPanel &panel, acr_nav::FPanel *left, acr_nav::FCtype *sel_ct) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_nsdep_viewmode;
    acr_nav::FNs *target_ns = NsDepNsAtLine(vm, panel.sel_row);
    if (target_ns) {
        PushNaventry(sel_ct ? algo::strptr(sel_ct->ctype)
            : (acr_nav::_db.p_nsdep_ns ? NsDisplayName(*acr_nav::_db.p_nsdep_ns)
                                       : algo::strptr("")));
        acr_nav::_db.filter = "";
        acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
        SwitchToBrowse();
        BuildLeftItems();
        for (int i = 0; i < acr_nav::left_item_N(); i++) {
            if (acr_nav::left_item_qFind(i).ns == target_ns->ns
                && ch_N(acr_nav::left_item_qFind(i).ctype) == 0) {
                left->sel_row = i;
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Preview mode Enter: extract cell value from the selected navigable column
// and follow the foreign-key reference to the target ctype.
static void FollowRefPreview(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    acr_nav::FViewmode &pvm = *acr_nav::_db.p_preview_viewmode;
    int n_nav = acr_nav::preview_nav_N(pvm);
    if (n_nav > 0 && acr_nav::_db.sel_nav_col < n_nav
        && panel.sel_row < acr_nav::line_N(pvm)) {
        acr_nav::PreviewNavCol &nc = acr_nav::preview_nav_qFind(pvm, acr_nav::_db.sel_nav_col);
        algo::strptr row_text = RightPanelLineFind(panel.sel_row);
        int start = DisplayToByte(row_text, nc.col_start);
        int end = i32_Min(DisplayToByte(row_text, nc.col_start + nc.col_wid), elems_N(row_text));
        tempstr cell_value;
        if (start < elems_N(row_text)) {
            algo::strptr raw_cell(row_text.elems + start, end - start);
            cell_value << algo::TrimmedRight(raw_cell);
        }
        acr_nav::FCtype *target = ch_N(nc.target_ctype) > 0
            ? acr_nav::ind_ctype_Find(nc.target_ctype) : nullptr;
        if (target && target != sel_ct && ch_N(cell_value) > 0) {
            acr_nav::_db.preview_nav_pending = cell_value;
            NavigateToTarget(sel_ct, target, acr_nav::_db.p_preview_viewmode);
        }
    }
}

// -----------------------------------------------------------------------------

// Fields/xref mode Enter: follow the field reference (forward or reverse).
static void FollowRefFields(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
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

// -----------------------------------------------------------------------------

void acr_nav::navaction_follow_ref() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    if (panel.position == 0) {
        FollowRefLeftPanel(left);
    } else if (panel.position == 1 && sel_ct
               && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_graph_viewmode) {
        FollowRefGraph(panel, sel_ct);
    } else if (panel.position == 1 && IsNsDepMode()) {
        FollowRefNsDep(panel, left, sel_ct);
    } else if (panel.position == 1 && sel_ct
               && acr_nav::_db.p_cur_viewmode == acr_nav::_db.p_preview_viewmode) {
        FollowRefPreview(panel, sel_ct);
    } else if (panel.position == 1 && acr_nav::_db.p_cur_viewmode->has_fields
               && sel_ct && panel.sel_row < RightPanelItemCount(sel_ct)) {
        FollowRefFields(panel, sel_ct);
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
        if (target_ct) {
            for (int i = 0; i < acr_nav::left_item_N(); i++) {
                if (acr_nav::left_item_qFind(i).ctype == entry->ctype) {
                    acr_nav::_db.p_left_panel->sel_row = i;
                    break;
                }
            }
        } else {
            // No ctype match (nsdep namespace jump) — scan for namespace header
            // entry->ctype holds NsDisplayName: "other" for empty ns, else the ns key
            algo::strptr saved_name(entry->ctype);
            algo::strptr ns_key = (saved_name == "other") ? algo::strptr("") : saved_name;
            for (int i = 0; i < acr_nav::left_item_N(); i++) {
                if (acr_nav::left_item_qFind(i).ns == ns_key
                    && ch_N(acr_nav::left_item_qFind(i).ctype) == 0) {
                    acr_nav::_db.p_left_panel->sel_row = i;
                    break;
                }
            }
        }
        acr_nav::_db.p_left_panel->scroll_offset = entry->scroll_offset;
        acr_nav::_db.p_right_panel->sel_row = entry->right_sel_row;
        acr_nav::_db.p_right_panel->scroll_offset = entry->right_scroll_offset;
        acr_nav::_db.sel_nav_col_pending = entry->sel_nav_col;
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
        PushOverlay(acr_nav::_db.p_help_viewmode);
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
            LoadDetail(*fld);
            PushOverlay(acr_nav::_db.p_detail_viewmode);
        }
    }
}
