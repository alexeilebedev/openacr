## Code Generation
<a href="#code-generation"></a>

How OpenACR turns ssim records into C++ — the ssim → amc → C++
pipeline, the source-file/header workflow, and the doc-regen tools.

### The pipeline
<a href="#the-pipeline"></a>

ssimfile → amc → C++:

1. **Schema in `data/`** — ssimfiles are the source of truth.
2. **`amc` generates** `cpp/gen/*.cpp` and `include/gen/*.h` —
   never hand-edit those files.
3. **`acr_ed -create`** adds fields/ctypes (writes ssim + runs
   amc).  Since records are just text, you can also
   `echo ... | acr -insert -write` directly.  Validate with
   `acr -check %` (referential integrity) and `acr -check % -x`
   (cross-refs, gitfile presence).  `acr_ed` rejects bad changes;
   `amc` catches common bad patterns.

`amc` runs with no args and globally regenerates.  There is no
partial mode.  Query mode: `amc <ctype>` prints a struct, `amc
<func>` prints a function source — rarely needed.

On a branch that modifies amc itself, rebuild it (`abt amc -build
-install`) before running it.  A stale `bin/amc` exits clean but
emits the previous generator's output, silently deleting the
branch's generated code; the deletion surfaces later as unrelated
compile errors in `cpp/gen/` consumers.

The same staleness bites after a rebase or merge that brings in
upstream amc changes: the installed `bin/amc` still embodies the
pre-rebase generator, and running it rewrites every generated file
back to the old output (the symptom is a huge `cpp/gen/` diff far
beyond the branch's own schema changes).  When a rebase conflicts
on a generated file, do not resolve it by picking a side: restore
the generated tree to the merged index, take the upstream version
of any generated pair that amc's own build needs to compile,
rebuild amc, and only then rerun it — the regenerated files are
then the true union of both sides' schemas.

A stale generator can also fail outright instead of mis-generating.
It reports a schema violation against a ctype the branch never
touched — `amc.dangling_pointer` naming an upstream field, followed
by `amc.no_output` — because the old generator is judging the merged
schema by its own rules.  That pair reads like a schema mistake and
is not one: confirm the ssim inputs match upstream (`git diff
origin/master -- data/`), then rebuild amc and rerun.

`acr_ed` reference:
[/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md).
Per-executable docs: `txt/exe/<exe>/README.md` (see
`acr ns -where nstype:exe`).

### Source files & headers
<a href="#source-files-headers"></a>

- New source file: `acr_ed -create -srcfile <srcfile> -target
  <target> -write`.
- After adding a function, run `update-hdr` to refresh prototypes.
  Never write prototypes by hand.
- **Comment blocks belong to the function below them.** Move /
  delete the preceding `// …` lines with the function.
- Default arguments use `DFLTVAL`, e.g. `void F(int a, int b
  DFLTVAL(0))` — the macro expands only in the header (no-op in
  the source).  C++ disallows restating defaults.

### New script under `bin/`
<a href="#new-script-under-bin-"></a>

```bash
acr_ed -create -srcfile bin/<name> [-license <pkey>] [-comment "…"] -write
```

Creates the file (default `#!/usr/bin/env bash` shebang if it
doesn't exist), inserts `dev.gitfile` + `dev.scriptfile` + the
`txt/script/<name>.md` readme stub, runs `src_hdr -update_copyright`
to stamp the header.  `-license` defaults to `GPL`; pick from
`acr license` (e.g. `GPL`, `X2`, `ARND`).  `-comment` lands in the
scriptfile record.  Copyright text itself lives in the file header
(managed by `src_hdr`), not in any table.

### Adding a message handler
<a href="#adding-a-message-handler"></a>

1. Insert a `dmmeta.dispatch_msg` record.
2. Run `amc`.
3. Run `src_func -createmissing` to stub the new function.

For dispatch `<ns>.<disp>`, all handler functions must live in
`cpp/<ns>/<disp>.cpp` named `void ns::Disp_<MessageType>(...)`.
The file must contain *all* such functions and only them.

### AMC executables — common conventions
<a href="#amc-executables-common-conventions"></a>

All amc-generated exes accept:

- `-v` / `-verbose`, `-d` / `-debug` — repeatable to raise level.
- `-h` / `-help`, `-version`, `-signature` — diagnostic.

Per-tool syntax: `acr field:command.<ns>.%` (add `-t` for
transitive closure).

Tools that read ssimfiles support `-in` (default `"data"`).

```bash
acr_in <ns regx>             # files a tool loads
acr_in <ns regx> -data       # actual tuples loaded
acr_in -r <ssimfile regx>    # which tools read this ssimfile
# Custom inputs: run `acr_in -data`, edit the result, pass via -in.
```

### Documentation
<a href="#documentation"></a>

- Imperative tense everywhere: "update key; write file" not
  "updates key, writes file".
- Markdown lives in `txt/`.
- New md file: `acr_ed -create -srcfile txt/.../xyz.md`.  Delete:
  `acr_ed -del -srcfile ...`.
- Generated sections match patterns in the `mdsection` ssimfile
  (`acr mdsection`).  Regenerate: `abt_md <mdfile regx>`.
- Inline commands appear as `inline-command: ...` in a preformatted
  block; the output replaces the rest of the block.  Skip eval:
  `abt_md <mdfile> -evalcmd:N`.
- Refresh docs for a namespace: `abt_md -ns:<ns regx>`.
- Check links / syntax: `abt_md -check` (`-external` checks
  external URLs too — needs network).  `-check` only checks: it never
  regenerates, so a section that has drifted from its ssim rows passes it
  and fails `quickreadme` under `bin/normalize` instead.  Run plain
  `bin/abt_md` first, then `-check`.
- Plain `bin/abt_md` does not refresh every generated section.  The
  cmdline field tables under `txt/protocol/command/README.md`, and the
  `Syntax` block of each `txt/exe/<tool>/README.md`, are regenerated by
  the `quickreadme` citest and not by `abt_md` — so a `dmmeta.field` row
  you add or edit still reads as the old value after `abt_md` and
  `abt_md -check` both pass, and only `bin/normalize` catches it.  This
  bites hardest after a rebase whose conflicts you resolved by taking
  the upstream side of those files.  Run `bin/normalize`, or
  `atf_ci -cijob:normalize -citest:quickreadme`, before believing a
  cmdline change is fully regenerated.
- After inserting a new command-line field, run `bin/abt_md` **twice**.
  The first run emits the new `#### -<flag>` block without the blank line
  every other block ends with, and the second run adds it; a single run
  therefore leaves a diff that the next person's `abt_md` produces and
  `checkclean` rejects.  Confirmed on `command.glrunner.sweep_max`: run
  one, no blank line; run two, blank line; run three, no change.
- Everything under `txt/gen/` is generated, so a rebase conflict there is
  never resolved by hand.  Take the upstream side of every conflicting file
  (`git checkout --ours <file>` while the rebase is stopped), finish the
  rebase, then run `abt_md` and commit what it regenerates.  Merging the two
  sides by eye means reproducing the generator's output by hand, which is
  both slower and wrong.
- Renaming or moving a doc file also moves its hash in the package manifest
  `apm/gen/<package>.ssim`.  Regenerate it with `apm -package:<package> -generate`,
  which is what the `apm_gen` citest of the `normalize` cijob checks; the
  rename otherwise looks complete and fails the pre-merge gate.

### Reference
<a href="#reference"></a>

- Querying / editing the ssim db: [acr.md](acr.md).
- Schema rules and pitfalls: [schema.md](schema.md).
- Build and test: [build.md](build.md).
