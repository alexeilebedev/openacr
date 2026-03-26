# Plan: acr_nav M7a — Component Tests + Documentation

## Context

acr_nav M1-M6b are complete. M7 is the final milestone — but it's too large for one session (5 items spanning 3 knowledge domains). Split like M6 was:

- **M7a** (this plan): Component tests + docs — "full development loop" closure
- **M7b** (next): Naventry filter state + `dmmeta.substr` keybind validation — "schema completeness"
- **Deferred**: Data-driven panel dispatch (N=2 with different iteration patterns isn't a missing noun — revisit when a third panel is needed)
- **Dropped**: SelectedCtype optimization (already effectively cached), SESE cleanup (already compliant)

## What M7a Does

Write 3-4 `atfdb.comptest` tests for acr_nav's non-interactive batch output path (`BatchOutput()` at line 96 of `acr_nav.cpp`, triggered when stdout is not a TTY). Write usage documentation.

**Concept learned:** Component test infrastructure — `comptest.ssim`, `tmsg.ssim`, expected output files in `test/atf_comp/`.

**Pattern to study:** Existing `acr.Select*` tests in `data/atfdb/comptest.ssim` and `test/atf_comp/`.

## Steps

### 1. Study test infrastructure

Read existing component test patterns:
- `data/atfdb/comptest.ssim` — how tests are declared
- `data/atfdb/tmsg.ssim` — how stdin input is provided (if needed)
- `test/atf_comp/` — where expected outputs live
- Study a simple test like `acr.Select` end to end

### 2. Design test cases

Target 3-4 tests covering:
1. **Basic output** — `acr_nav -ns:acr_nav` piped to non-TTY, verify ctype/field listing
2. **Namespace filter** — `acr_nav -ns:dmmeta` filters to dmmeta ctypes only
3. **Empty result** — `acr_nav -ns:nonexistent` produces report with n_sel_ctype:0
4. **Default behavior** — `acr_nav` with no args (all namespaces) — verify report line format

### 3. Add test records + expected outputs

- Add comptest records to `data/atfdb/comptest.ssim`
- Create expected output files in `test/atf_comp/`
- Run `normalize comp` to verify

### 4. Write documentation

- Update `txt/exe/acr_nav/README.md` with:
  - Purpose and usage (`acr_nav -ns:<regex>`)
  - Key bindings (browse mode, filter mode)
  - Design notes (schema-driven keybinds, gstatic Hook dispatch)
  - Batch vs TUI mode

### 5. Update ideas.md

Document M7 decomposition decisions:
- Panel dispatch deferred (N=2, premature)
- SelectedCtype optimization dropped (phantom)
- SESE cleanup dropped (already compliant)

## Files to Modify

- `data/atfdb/comptest.ssim` — add test records
- `test/atf_comp/` — expected output files (new)
- `txt/exe/acr_nav/README.md` — usage documentation
- `txt/ideas.md` — update factorization debt notes

## Files to Study

- `data/atfdb/comptest.ssim` — existing test patterns
- `test/atf_comp/` — expected output format
- `cpp/acr_nav/acr_nav.cpp:96` — `BatchOutput()` function (the test subject)

## Verification

- `acr -check` passes
- `amc` runs clean
- `abt -build acr_nav` compiles
- `normalize comp` — new tests pass
- `amc_vis acr_nav.%` shows clean diagram
