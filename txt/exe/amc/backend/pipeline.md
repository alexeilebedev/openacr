## amc Backend: the gen-table pipeline
<a href="#amc-backend-the-gen-table-pipeline"></a>

amc has no special control flow.  At top level it walks the
`amcdb.gen` table once and calls each phase in declared order.
A few phases run once globally; most run once per selected
namespace.  This page documents every phase.

The table lives in `ssimfile:amcdb.gen`.  Each row binds a phase
name (e.g. `amcdb.gen:ns_dispatch`) to a C++ function pointer
(`amc::gen_ns_dispatch`).  See
[backend/tclass-tfunc.md](/txt/exe/amc/backend/tclass-tfunc.md) for
the registration mechanism.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [How phases run](#how-phases-run)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Phase categories](#phase-categories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Phase reference](#phase-reference)<br/>

### How phases run
<a href="#how-phases-run"></a>

*To be written.*  Describe the loop in `cpp/amc/main.cpp`
(`ind_beg(_db_gen_curs, gen, _db) { RunGen(gen) }`),
how `perns:Y` toggles per-namespace iteration, and how phase
failure short-circuits subsequent phases via `prev_err`.

### Phase categories
<a href="#phase-categories"></a>

*To be written.*  Group the 96 phases into broad buckets:
- **Prep** — signature, select_ns, sort, lookuppkey
- **Schema rewrites** — clonefconst, rewrite_regx, gconst
- **Synthetic field creation** — newfield_charset, newfield_count,
  newfield_hook, newfield_exec, newfield_sortfld, newfield_cbase,
  newfield_ptrary, newfield_cfmt, newfield_dispatch
- **Validation** — check_basefield, check_ssimsort, checkxref
- **Per-namespace code emission** — ns_dispatch, ns_func, etc.
- **Output** — ns_write

### Phase reference
<a href="#phase-reference"></a>

*To be written.*  Table of every phase: name, `perns` flag,
one-paragraph "what it does" sourced from `ssimfile:amcdb.gen`
comments and the matching `gen_*` function in `cpp/amc/`.
