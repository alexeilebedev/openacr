# Plan: Runtime State Dump — amc Generator MVP

## Context

OpenACR programs are in-memory databases. amc knows every pool via `gen_detectinst()` (`cpp/amc/gen.cpp:460`) which checks `reftype.inst` and populates `zd_inst` on each FCtype. A new amc generator emits a state dump function for any opt-in program — census (pool counts) + filtered record dump (ssim tuples). Primary consumer: Claude Code as AI agent inspecting runtime state.

## Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Consumer | AI agent | Structured output for programmatic consumption |
| Scope | General, any amc program | amc generator, opt-in via schema record |
| Opt-in | `dmmeta.nsdump ns:<ns>` | Namespace-level, follows `dmmeta.main` / `dmmeta.nsx` pattern |
| Output | Census (all pools) + record dump (cfmt-only) | Census for visibility, dump for inspection |
| Format | Ssim tuples using `report.PoolCensus` ctype | Follows existing `report.*` protocol (`nstype:protocol`) |
| CLI trigger | `-dump` field on `command.<ns>` | Developer adds field manually; dump call in user-written Main() |
| Stdin trigger | Program-specific | Programs with stdin loops add dispatch manually (follows acr_nav headless pattern) |
| Reftype coverage | Lary + Inlary (MVP) | Covers ~117 of ~120 pools. Tpool has no count variable and no cursor — deferred. |

## Schema Additions

### 1. New ssimfile: `dmmeta.nsdump`

```ssim
dmmeta.ctype     ctype:dmmeta.Nsdump  comment:"Enable runtime state dump for namespace"
dmmeta.field     field:dmmeta.Nsdump.ns  arg:dmmeta.Ns  reftype:Pkey  dflt:""  comment:""
dmmeta.ssimfile  ssimfile:dmmeta.nsdump  ctype:dmmeta.Nsdump
```

Pkey field is `ns` (not `nsdump`) — `acr_ed -create -ssimfile -subset:dmmeta.Ns` borrows the pkey name from the subset's first field. Consistent with `dmmeta.Main.ns`, `dmmeta.Nsx.ns`, `dmmeta.Nscpp.ns`.

### 2. Census output ctype: `report.PoolCensus`

```ssim
dmmeta.ctype  ctype:report.PoolCensus  comment:"Pool census line: ctype name and live record count"
dmmeta.field  field:report.PoolCensus.ctype  arg:algo.Smallstr100  reftype:Val  dflt:""  comment:"Fully qualified ctype name"
dmmeta.field  field:report.PoolCensus.n_record  arg:i32  reftype:Val  dflt:""  comment:"Live record count"
dmmeta.cfmt   cfmt:report.PoolCensus.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
```

cfmt record is required — without it, `PoolCensus_Print()` won't be generated. Format matches all existing report cfmt records exactly (`report.abt.String`, `report.acr.String`, etc.).

### 3. amc internal: load nsdump records

```ssim
dmmeta.ctype   ctype:amc.FNsdump  comment:""
dmmeta.field   field:amc.FNsdump.msghdr  arg:dmmeta.Nsdump  reftype:Base  dflt:""  comment:""
dmmeta.field   field:amc.FDb.nsdump  arg:amc.FNsdump  reftype:Lary  dflt:""  comment:""
dmmeta.finput  field:amc.FDb.nsdump  extrn:N  update:N  strict:Y  comment:""
dmmeta.field   field:amc.FNs.c_nsdump  arg:amc.FNsdump  reftype:Ptr  dflt:""  comment:""
dmmeta.xref    field:amc.FNs.c_nsdump  inscond:true  via:amc.FDb.ind_ns/dmmeta.Nsdump.ns
```

Follows `amc.FMain` / `amc.FNsx` pattern. Xref `via` goes through hash index `ind_ns` (not the Lary pool), matching exact pattern of `amc.FNs.c_main` (`via:amc.FDb.ind_ns/dmmeta.Main.ns`) and `amc.FNs.c_nsx` (`via:amc.FDb.ind_ns/dmmeta.Nsx.ns`).

### 4. Generator record

```ssim
amcdb.gen  gen:ns_state_dump  perns:Y  comment:"Generate FDb state dump function"
```

**Name**: `ns_state_dump` — all `perns:Y` generators use `ns_` prefix (verified: 20/20).

**Ordering**: `amcdb.gen` has NO ssimsort record — file order IS execution order. `acr -insert -write` appends to end of file. After inserting, manually reposition the record before `gen:ns_funcindex` (the generator creates function objects that must exist before indexing and printing). Alternatively, hand-edit the file position.

### 5. Build system

```ssim
dev.targsrc  targsrc:amc/cpp/amc/state_dump.cpp  comment:""
```

## Generated Code

For each namespace `ns` with an `nsdump` record, the generator emits a namespace-level function:

```cpp
// Generated in cpp/gen/<ns>_gen.cpp
void ns::StateDump(algo::cstring& out, algo_lib::Regx& filter) {
    report::PoolCensus census;

    // -- For each Lary/Inlary pool field in ns.FDb --
    // (one block per pool, generated at amc time)

    // Example: pool "ctype" of type ns.FCtype
    census.ctype = "ns.FCtype";
    census.n_record = ns::ctype_N();           // generated _N() accessor
    report::PoolCensus_Print(census, out);
    out << '\n';
    // Record dump block only emitted if FCtype has cfmt with print:Y
    // (checked at generation time via zs_cfmt on the FCtype)
    if (Regx_Match(filter, strptr("ns.FCtype"))) {
        ind_beg(ns::_db_ctype_curs, rec, ns::_db) {
            ns::FCtype_Print(rec, out);
            out << '\n';
        }ind_end;
    }

    // -- repeat for each pool --
}
```

**Function name**: `ns::StateDump` — namespace-level, no `FDb_` prefix. Created via `amc::ind_func_GetOrCreate(key)` with key `"$ns...StateDump"` (three dots = namespace scope). Set `func.glob = true`. Emit body via `Ins(&R, func.body, ...)` using Replscope R for substitutions.

### Count accessor per reftype

| Reftype | Generated accessor | Coverage |
|---------|-------------------|----------|
| Lary | `ns::pool_N()` → reads `_db.pool_n` | ~116 pools |
| Inlary | `ns::pool_N()` → reads `_db.pool_n` or returns `$max` (fixed) | ~1 pool |
| Tpool | **Skipped** — no count variable, no cursor | ~1 pool |
| Lpool | **Skipped** — no cursor | ~1 pool |
| Sbrk | **Skipped** — no count, no cursor | ~1 pool |

### cfmt detection (generation-time check)

For each pool's arg ctype, the generator checks `zs_cfmt` (Llist on `amc::FCtype`):
- Has cfmt with `print:Y` → emit both census line AND record dump block
- No cfmt or print:N → emit census line only (count is always available)

### CLI trigger integration

The developer adds a `-dump` field to `command.<ns>` and a few lines to their `Main()`:

```cpp
// In ns::Main() (hand-written, e.g., cpp/acr_nav/main.cpp)
// Integrates with existing branch structure (headless vs TUI):
bool do_dump = ch_N(_db.cmdline.dump) > 0;
if (do_dump) {
    algo_lib::Regx filter;
    Regx_ReadSql(filter, _db.cmdline.dump, true);
    algo::cstring out;
    StateDump(out, filter);
    prlog(out);
} else if (_db.cmdline.headless) {
    HeadlessMain();
} else {
    // ... normal TUI / batch logic ...
}
```

No early return — follows SESE by extending the existing `if/else` branch structure in Main(). The `do_dump` branch is just another mode alongside headless/TUI.

## Implementation Steps

### Step 1: Schema setup

```bash
# 1a. Create nsdump ssimfile (generates ctype + pkey field + ssimfile record)
acr_ed -create -ssimfile:dmmeta.nsdump -subset:dmmeta.Ns -write

# 1b. Create report.PoolCensus ctype + fields + cfmt
acr_ed -create -ctype:report.PoolCensus -comment:"Pool census line: ctype name and live record count" -write
acr_ed -create -field:report.PoolCensus.ctype -arg:algo.Smallstr100 -comment:"Fully qualified ctype name" -write
acr_ed -create -field:report.PoolCensus.n_record -arg:i32 -comment:"Live record count" -write
echo 'dmmeta.cfmt  cfmt:report.PoolCensus.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""' | acr -insert -write

# 1c. Create amc.FNsdump + pool + finput + xref
acr_ed -create -ctype:amc.FNsdump -write
acr_ed -create -field:amc.FNsdump.msghdr -arg:dmmeta.Nsdump -reftype:Base -write
acr_ed -create -field:amc.FDb.nsdump -arg:amc.FNsdump -reftype:Lary -write
echo 'dmmeta.finput  field:amc.FDb.nsdump  extrn:N  update:N  strict:Y  comment:""' | acr -insert -write
acr_ed -create -field:amc.FNs.c_nsdump -arg:amc.FNsdump -reftype:Ptr -via:amc.FDb.ind_ns/dmmeta.Nsdump.ns -write

# 1d. Add generator record (appends to end; reposition before ns_funcindex after)
echo 'amcdb.gen  gen:ns_state_dump  perns:Y  comment:"Generate FDb state dump function"' | acr -insert -write
# NOTE: manually move the line in data/amcdb/gen.ssim to before gen:ns_funcindex

# 1e. Register source file
echo 'dev.targsrc  targsrc:amc/cpp/amc/state_dump.cpp  comment:""' | acr -insert -write

# 1f. Run amc in sandbox (modifying amc itself — safety first)
sandbox amc -reset -diff -- amc
```

### Step 2: Generator (`cpp/amc/state_dump.cpp`)

New file: `cpp/amc/state_dump.cpp`

Function: `void amc::gen_ns_state_dump()`

```
Logic:
1. amc::FNs& ns = *amc::_db.c_ns;
2. if (ns.c_nsdump) {  // only emit for namespaces with opt-in
3. Get FDb ctype: ns.c_globfld->p_arg  (the FDb ctype for this namespace)
4. Create function: amc::ind_func_GetOrCreate(key) with key "$ns...StateDump"
   - Set func.glob = true, func.extrn = false
   - Set func.ret = "void"
   - Set func.proto = "StateDump(algo::cstring& out, algo_lib::Regx& filter)"
5. Set up Replscope R with $ns substitution
6. Emit: Ins(&R, func.body, "report::PoolCensus census;")
7. Iterate fields of FDb ctype: ind_beg(amc::ctype_c_field_curs, field, *fdb)
8. For each field where field.reftype == dmmeta_Reftype_reftype_Lary
   or field.reftype == dmmeta_Reftype_reftype_Inlary:
   a. Set R substitutions: $name (field name), $Cpptype (arg ctype C++ name)
   b. Emit census: census.ctype = "$ns.$Cpptype"; census.n_record = $ns::$name_N();
   c. Check field.p_arg->zs_cfmt_First() for print:Y
   d. If printable: emit Regx_Match check + cursor loop + Print call
```

### Step 3: Enable for acr_nav

```bash
# Opt in
echo 'dmmeta.nsdump  ns:acr_nav' | acr -insert -write

# Add CLI flag
acr_ed -create -field:command.acr_nav.dump -arg:algo.cstring -dflt:'""' -comment:"Dump state matching regex and exit" -write

# Rebuild
sandbox amc -reset -diff -- amc
amc && abt -build -install acr_nav
```

Edit `cpp/acr_nav/main.cpp` — add `do_dump` branch to Main() alongside existing headless/TUI branches.

### Step 4: Headless stdin dispatch (acr_nav only)

```bash
acr_ed -create -ctype:acr_nav.StateDump -comment:"Request runtime state dump" -write
acr_ed -create -field:acr_nav.StateDump.filter -arg:algo.cstring -comment:"Regex filter on ctype name" -write
```

Add dispatch case in `HeadlessMain()` (`cpp/acr_nav/main.cpp:670+`):
```cpp
acr_nav::StateDump statedump_cmd;
if (acr_nav::StateDump_ReadStrptrMaybe(statedump_cmd, line)) {
    algo_lib::Regx filter;
    Regx_ReadSql(filter, statedump_cmd.filter, true);
    algo::cstring out;
    StateDump(out, filter);
    prlog(out);
}
```

### Step 5: Test + capture

```bash
acr_nav -dump:".*"                    # all pools
acr_nav -dump:"acr_nav.FPanel%"       # filtered
echo 'acr_nav.StateDump  filter:"acr_nav.%"' | acr_nav -headless
atf_comp acr_nav.% -capture
atf_comp acr_nav.%
normalize comp
```

## Key Files

| File | Action |
|------|--------|
| `cpp/amc/state_dump.cpp` | **NEW** — generator function |
| `data/dmmeta/nsdump.ssim` | **NEW** — opt-in records |
| `data/dmmeta/ctype.ssim` | ADD dmmeta.Nsdump, amc.FNsdump, report.PoolCensus, acr_nav.StateDump |
| `data/dmmeta/field.ssim` | ADD fields for all new ctypes |
| `data/dmmeta/cfmt.ssim` | ADD report.PoolCensus.String |
| `data/dmmeta/finput.ssim` | ADD amc.FDb.nsdump |
| `data/dmmeta/xref.ssim` | ADD amc.FNs.c_nsdump |
| `data/amcdb/gen.ssim` | ADD ns_state_dump (before ns_funcindex) |
| `data/dev/targsrc.ssim` | ADD amc/cpp/amc/state_dump.cpp |
| `cpp/acr_nav/main.cpp` | EDIT — dump branch in Main(), dispatch in HeadlessMain() |

## Verification

1. `acr -check` — referential integrity of all new schema records
2. `sandbox amc -reset -diff -- amc` — generator produces valid C++, amc rebuilds itself
3. `ai` — full build succeeds
4. `acr_nav -dump:".*"` — prints census for all pools + records for cfmt-enabled ctypes
5. `acr_nav -dump:"acr_nav.FPanel%"` — filtered output
6. `echo 'acr_nav.StateDump filter:"acr_nav.%"' | acr_nav -headless` — stdin trigger
7. `atf_comp acr_nav.% -capture && atf_comp acr_nav.%` — component tests pass
8. `normalize comp` — full component test suite
9. Enable for a second program (e.g., `acr`) to verify generality
