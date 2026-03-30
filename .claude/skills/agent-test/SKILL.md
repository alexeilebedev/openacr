---
name: agent-test
description: Exploratory testing of acr_nav via headless mode. Launches parallel subagents to stress-test the app using judgment, not scripts. Use after significant changes or when asked to test acr_nav.
---

# Agent-Driven Exploratory Testing

Exploratory testing of acr_nav through its headless protocol. The value is unconstrained judgment finding novel issues that scripted regression tests miss: state combinations, protocol gaps, edge cases no one thought to script.

This is an orchestrator's playbook. Subagents receive `references/protocol.md` — they never see this file.

## Prerequisite

Verify headless mode works:
```bash
printf 'acr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null | head -1
```
Should produce an `acr_nav.Screen` line. If not, build first: `abt acr_nav`.

## Step 1: Study Existing Coverage

Before launching exploratory agents, understand what regression tests already cover:

```bash
atf_comp acr_nav.% -print:Y 2>/dev/null | head -50   # list tests
```

Read a few test output files in `test/atf_comp/acr_nav.*` to see what states and transitions are already verified. Also read `.claude/plans/acr_nav_headless_test_report.md` for prior exploratory findings (some bugs noted there have been fixed — verify, don't re-report).

The purpose of exploratory testing is to find what these DON'T cover.

## Step 2: Dispatch Subagents

Launch parallel subagents based on your judgment. Context determines count and focus:
- **Testing the whole app**: more agents, broader areas
- **Testing a new feature**: 1-2 focused agents on that feature + its interactions
- **Investigating a specific bug**: 1 agent, deep and targeted

Each subagent gets:
1. The full `references/protocol.md` (paste it into the prompt)
2. An area assignment (a few sentences describing what to explore)
3. The report template (below)

## The Orthogonal Axes

acr_nav's state is the product of these independent dimensions:

| Axis | Values | Source |
|------|--------|--------|
| navmode | browse, filter | `acr_navdb.navmode` |
| viewmode | fields, xref, preview, codegen, help, detail | `acr_navdb.viewmode` |
| panel focus | ctype_list, content | `acr_navdb.panel` |
| navstack depth | 0..N | filter/selection/viewmode saved per level |
| filter state | empty, active text | filtertarget: ctype or field |
| left panel | per-namespace collapsed/expanded, selection, scroll | |

The interesting bugs live in **combinations across axes** — e.g., filter active + xref viewmode + navstack depth 2 + content focused. Does backtrack restore all state correctly?

## Exploration Starting Points

Suggestions — combine, split, skip, or invent areas based on what regression tests already cover:

- **Navigation & boundaries** — cursor extremes, SetTermSize + pagination, rapid keys, scroll_offset tracking
- **Filter interactions** — filter + follow ref, filter + collapse, special chars, zero matches, filtertarget cycling
- **Namespace tree** — collapse/expand, VisibleLeftItem accuracy, filter + collapse interaction, n_match/n_record correctness
- **Viewmodes & overlays** — Tab cycle, help/detail stacking, overlay dismiss, VisibleField vs VisibleLine content verification
- **Xref & deep navigation** — deep navstack, breadcrumb accuracy, xref count cross-validation with `acr`
- **Protocol robustness** — malformed input, InputError output, SetTermSize edge values, unknown keys, EOF behavior
- **State combinations** — the most valuable territory. Pick 3+ axes, set them to non-default values, then perform an operation. Does all state remain consistent?

## Subagent Guidelines

- **Don't hardcode counts.** Read initial state (auto-screenshot on launch) as baseline. Verify relative changes, not absolute values.
- **Don't repeat regression tests.** If `acr_nav.Filter` already tests basic filtering, explore filter + other features instead.
- **Run `acr acr_navdb.%`** to discover the full capability surface (keybinds, viewmodes, navactions, navstyles).
- **Follow surprises.** When something unexpected happens, send more commands and investigate. The most valuable findings come from following anomalies.
- **Cross-validate with `acr`.** Verify field counts, xref counts, and record counts against `acr dmmeta.field -where:...` queries.

## Synthesis

After all subagents complete:

1. Run `atf_comp acr_nav.%` to confirm existing regression tests still pass.
2. Compile findings into a report at `.claude/plans/`.
3. If bugs are found, optionally launch follow-up agents to trace root cause in `cpp/acr_nav/acr_nav.cpp`.

## Report Template

Each subagent produces:

```
## [Area Name]

### Tests Run: N

### Bugs Found
- **Bug N: [title]** — [description]. Location: [file:line]. Reproduction: [command sequence].

### Protocol Gaps Found
- **Gap N: [title]** — [what agent cannot observe or control]. Impact: [severity].

### Verified Working
- [bullet list of tested behaviors that work correctly]

### Observations
- [anything notable: surprising behavior, performance, design insights]
```

The orchestrator's final report follows the same structure, consolidated across all subagent areas, with a summary table at the top.
