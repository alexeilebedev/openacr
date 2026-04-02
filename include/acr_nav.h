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
// Header: include/acr_nav.h
//

#include "include/gen/acr_nav_gen.h"
#include "include/gen/acr_nav_gen.inl.h"

namespace acr_nav { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr_nav/content.cpp
    //

    // Extract namespace pointer from a nsdep viewmode line by finding the line_key
    // color span (which highlights the namespace name) and looking up the namespace.
    // Returns NULL for header, separator, or "(none)" lines.
    acr_nav::FNs* NsDepNsAtLine(acr_nav::FViewmode &vm, int line_idx);

    // Compute and display cross-namespace field dependencies for a given namespace.
    // Upstream: namespaces this ns imports from (via field arg references).
    // Downstream: namespaces that import from this ns (via field_arg back-references).
    void LoadNsDep(acr_nav::FNs &ns);

    // Ensure-content wrappers for hook dispatch.
    // Each normalizes the lazy-load check to the ensure_content hook signature.
    void PreviewEnsureContent(void *, acr_nav::FCtype &ct);
    void CodegenEnsureContent(void *, acr_nav::FCtype &ct);
    void NsDepEnsureContent(void *, acr_nav::FCtype &ct);

    // Load metadata records for a single field from detailsrc ssimfiles.
    // Re-serializes the dmmeta.field record as the first card, then scans each
    // detailsrc file for matching records (first attribute value == field name).
    void LoadDetail(acr_nav::FField &field);
    int RightPanelItemCount(acr_nav::FCtype *sel_ct);

    // Panel item count dispatches on position (0=ctype list, 1=field list).
    // The two panels have structurally different roles (parent/child),
    // not N instances of one concept -- factoring into step hooks
    // would move the if elsewhere without reducing complexity.
    int PanelItemCount(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct);

    // Build preformatted help lines from keybind/navaction data.
    // Single-column layout with section headers styled like the detail view.
    // Directional pairs (up/down, left/right) are merged into single lines.
    // Arrow keys display as Unicode symbols (↑↓←→).
    void BuildHelpLines();

    // -------------------------------------------------------------------
    // cpp/acr_nav/graph.cpp
    //

    // Map a graph line index to the neighbor ctype and (optionally) the field on that line.
    // Returns neighbor ctype via p_node_out, field via p_field_out (both nullable).
    // Center open/close lines return NULL for both. Neighbor open/close lines return node but NULL field.
    void GraphInfoAtLine(acr_nav::FCtype &center, int line_idx, acr_nav::FCtype **p_node_out, acr_nav::FField **p_field_out);

    // Reverse of GraphInfoAtLine: given a neighbor ctype, return its first line in the graph.
    // Returns -1 if not found.
    int GraphFindCtypeLine(acr_nav::FCtype &center, acr_nav::FCtype *target);
    void GraphEnsureContent(void *, acr_nav::FCtype &ct);

    // -------------------------------------------------------------------
    // cpp/acr_nav/main.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:acr_nav

    // -------------------------------------------------------------------
    // cpp/acr_nav/nav.cpp
    //
    void BuildLeftItems();
    void BuildLeftItemsReset();

    // Used by show_help toggle, show_detail toggle, and startup help dismiss.
    void PopViewmode();

    // Push current viewmode + right-panel state onto overlay stack, switch to target.
    void PushOverlay(acr_nav::FViewmode *target);

    // Dismiss startup help on any non-passive action (not movement/panel switch).
    // Escape/? pop the overlay directly; detected here via !IsHelpMode().
    void DismissStartupHelp(acr_nav::FKeybind *keybind);

    // If an overlay viewmode is active and the selected ctype changed, pop all
    // overlays and restore the base viewmode.  During startup help, preserve the
    // overlay so movement doesn't dismiss it.
    // nsdep is a context-sensitive view: auto-activates on namespace headers,
    // restores the previous viewmode when leaving.
    bool PopOverlayOnCtypeChange(acr_nav::FCtype *prev_sel_ct, acr_nav::FCtype *sel_ct);

    // Navigate directly to a ctype by key. Used by headless Navigate command.
    // If sel_ct is non-null (already viewing a ctype), pushes navstack via NavigateToTarget.
    // If sel_ct is null (initial state on namespace header), navigates without pushing navstack.
    // Returns the target FCtype, or nullptr if not found.
    acr_nav::FCtype* GoToCtype(algo::strptr ctype_key, acr_nav::FViewmode *dest_viewmode);
    //     (user-implemented function, prototype is in amc-generated header)
    // void navaction_move_up(); // gstatic/acr_navdb.navaction:move_up
    // void navaction_move_down(); // gstatic/acr_navdb.navaction:move_down
    // void navaction_page_up(); // gstatic/acr_navdb.navaction:page_up
    // void navaction_page_down(); // gstatic/acr_navdb.navaction:page_down
    // void navaction_switch_panel_left(); // gstatic/acr_navdb.navaction:switch_panel_left
    // void navaction_switch_panel_right(); // gstatic/acr_navdb.navaction:switch_panel_right
    // void navaction_follow_ref(); // gstatic/acr_navdb.navaction:follow_ref
    // void navaction_go_top(); // gstatic/acr_navdb.navaction:go_top
    // void navaction_go_bottom(); // gstatic/acr_navdb.navaction:go_bottom
    // void navaction_go_back(); // gstatic/acr_navdb.navaction:go_back
    // void navaction_quit(); // gstatic/acr_navdb.navaction:quit
    // void navaction_cycle_viewmode(); // gstatic/acr_navdb.navaction:cycle_viewmode
    // void navaction_filter_accept(); // gstatic/acr_navdb.navaction:filter_accept
    // void navaction_filter_start(); // gstatic/acr_navdb.navaction:filter_start
    // void navaction_filter_cancel(); // gstatic/acr_navdb.navaction:filter_cancel
    // void navaction_filter_append_space(); // gstatic/acr_navdb.navaction:filter_append_space
    // void navaction_filter_clear(); // gstatic/acr_navdb.navaction:filter_clear
    // void navaction_dismiss_or_clear(); // gstatic/acr_navdb.navaction:dismiss_or_clear
    // void navaction_filter_backspace(); // gstatic/acr_navdb.navaction:filter_backspace
    // void navaction_filter_cycle_target(); // gstatic/acr_navdb.navaction:filter_cycle_target
    // void navaction_show_help(); // gstatic/acr_navdb.navaction:show_help
    // void navaction_show_detail(); // gstatic/acr_navdb.navaction:show_detail

    // -------------------------------------------------------------------
    // cpp/acr_nav/render.cpp
    //

    // Build breadcrumb trail from navigation stack.
    // Returns empty string at depth 0, or "A > B > C" showing the path of ctypes visited.
    tempstr BuildBreadcrumb(acr_nav::FCtype *sel_ct);

    // Build status bar hint from explicit strings on navmode/viewmode.
    // Filter: navmode.status_hint. Overlay: viewmode.status_hint.
    // Non-overlay browse: arrow indicators + optional "Backspace:back" +
    // viewmode.status_hint (right panel) or navmode.status_hint (left panel).
    void BuildStatusHint(cstring &out);
    void Render(cstring &buf, acr_nav::FCtype *sel_ct);

    // -------------------------------------------------------------------
    // cpp/acr_nav/util.cpp
    //
    void WriteStdout(const char *buf, int len);
    void DetectTerminal();
    void SwitchToBrowse();
    int DecimalDigits(int n);

    // True if field matches regex under the current filtertarget's boolean flags.
    bool FieldMatchesFilter(acr_nav::FField &fld, algo_lib::Regx &regx, acr_nav::FFiltertarget &ft);

    // True if ctype passes the current filter.  Dispatches on filtertarget's
    // boolean flags: match_ctype_name checks the ctype name directly,
    // has_field_criteria iterates fields via FieldMatchesFilter.
    bool CtypeMatchesFilter(acr_nav::FCtype &ct, algo_lib::Regx &regx, acr_nav::FFiltertarget &ft);
    acr_nav::FCtype* SelectedCtype(acr_nav::FPanel &left);

    // Extract the namespace from the current left panel selection.
    // Works for both ctype rows (via ctype's p_ns) and namespace headers (via LeftItem.ns).
    acr_nav::FNs* SelectedNs();

    // is_reverse on FViewmode drives forward/reverse dispatch — no identity checks needed.
    // IsHelpMode: identity check used only for the ? toggle guard in navaction_show_help.
    // IsDetailMode: identity check for d toggle guard and title bar display.
    // 2 overlay viewmodes, each with distinct entry guards (help: unconditional;
    // detail: requires has_fields + selected field). The is_overlay property on
    // viewmode records captures overlay state for the hint system; the navaction
    // functions are not unified because their entry guards differ.
    bool IsHelpMode();
    bool IsDetailMode();
    bool IsNsDepMode();
    int RightPanelLineCount();
    algo::strptr RightPanelLineFind(int idx);
    algo::strptr RightPanelLineHeader();

    // Find the ssimfile backing a ctype. If the ctype itself has no ssimfile,
    // follow the Base field to the underlying type (e.g. abt.FTarget -> dev.Target).
    acr_nav::FSsimfile* FindSsimfile(acr_nav::FCtype &ctype);

    // Append " (N)" record count suffix for the ctype's ssimfile.
    // Returns number of characters appended (0 if no ssimfile or no records).
    int PrintRecordCount(cstring &out, acr_nav::FCtype &ctype);

    // Clear all line content and color spans from a viewmode.
    void ClearViewmodeLines(acr_nav::FViewmode &vm);

    // Reset a viewmode to empty: clear lines, spans, nav columns, header, and h-scroll.
    void ResetViewmodeContent(acr_nav::FViewmode &vm);

    // Invalidate cached content for content-loading viewmodes (preview, codegen, graph).
    // Called when the selected ctype becomes NULL (namespace header row), so stale
    // content from the previous ctype is not displayed.
    // nsdep excluded: caches by namespace, handles NULL selection in RightPanelItemCount.
    void InvalidateContentCaches();

    // True if the byte range [byte_start, byte_end) in line contains at least one non-space character.
    bool RegionHasContent(algo::strptr line, int byte_start, int byte_end);

    // Add a color span to a viewmode. Positions are 0-based relative to stored line text.
    // Spans must be emitted in line_idx then col_start order. No overlapping spans.
    // Caller must ensure line_idx < line_N(vm).
    // Whitespace-only regions are silently skipped.
    void AddSpan(acr_nav::FViewmode &vm, int line_idx, int col_start, int col_end, acr_nav::FNavstyle *p_style);

    // Replace control characters (< 0x20) and DEL (0x7F) with '.' for safe terminal display.
    // 1:1 byte replacement preserves string length for column alignment.
    void SanitizeForDisplay(cstring &str);

    // Count extra bytes from UTF-8 multibyte characters (byte_length - display_width).
    int Utf8ExtraBytes(algo::strptr s);

    // Convert a display-column offset to a byte offset within a UTF-8 string.
    // Each multi-byte lead byte counts as 1 display column; continuation bytes are skipped.
    int DisplayToByte(algo::strptr s, int display_col);
    algo::strptr NsDisplayName(acr_nav::FNs &ns);
    acr_nav::FField* RightPanelFieldFind(acr_nav::FCtype *sel_ct, int idx);

    // Number of content rows available for the dual-panel display.
    // Subtracts title bar (1) and status bar (1), plus breadcrumb bar (1) when navstack is non-empty.
    int VisibleRows();

    // Number of data rows visible in each panel (VisibleRows minus the column header row).
    int DataRows();
    void AdjustScroll(acr_nav::FPanel &panel, int n_items);
}
