## Code Generation
<a href="#code-generation"></a>

`amc` reads the ssim database and writes C++.  Over 95% of the code in this
project arrives that way, so understanding what `amc` produces from what is
most of understanding the codebase.

### The pipeline
<a href="#the-pipeline"></a>

```text
data/*.ssim  →  amc  →  cpp/gen/*.cpp, include/gen/*.h  →  abt  →  bin/
```

The schema in `data/` is the source of truth.  `amc` reads every ssimfile and
emits, for each namespace, a complete in-memory database: the pools that hold
records, the hash indexes and lists that reach them, the cursors that walk
them, the step dispatch, the command-line parser, and the readers and printers
for every ctype.  Hand-written code under `cpp/<ns>/` calls those generated
APIs and holds the logic that is genuinely particular to the program.

`amc` takes no arguments and regenerates the whole tree; there is no partial
mode.  Its output is committed, which is what makes the pipeline checkable: a
rebuilt `amc` regenerating the tree byte-for-byte is the proof that the schema
and the code still agree.

Nothing under `cpp/gen`, `include/gen` or `ts/gen` is ever edited by hand.  An
edit there survives until the next `amc` run, and the way to change generated
code is to change the record it came from, or to change the generator.

### Editing the schema
<a href="#editing-the-schema"></a>

```bash
acr_ed -create -field <ns>.FCtype.<name> -arg <ctype> -write
acr_ed -create -srcfile cpp/<ns>/<file>.cpp -target <ns> -write
acr_ed -create -srcfile txt/.../xyz.md -write
```

`acr_ed` writes the records and runs `amc` in one step, and it prints the
script it is about to run when `-write` is left off.  Records are just text, so
`echo ... | acr -insert -write` works too; what `acr_ed` adds is the set of
companion records an entity needs and the ordering between them.  Full
reference: [/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md).

After adding a function, run `update-hdr` to refresh its prototype.  Never
write a prototype by hand, and move a function's leading comment block with the
function -- the comment belongs to the declaration below it, and `update-hdr`
copies both.  A default argument is written in the source as `DFLTVAL(X)`,
because C++ disallows restating a default and `update-hdr` is what turns it
into `= X` in the header.

### Message handlers
<a href="#message-handlers"></a>

A handler is dispatched from a table rather than from a switch.  Insert a
`dmmeta.dispatch_msg` record, run `amc`, then `src_func -createmissing` to stub
the function.  For dispatch `<ns>.<disp>` every handler lives in
`cpp/<ns>/<disp>.cpp`, named `void ns::Disp_<MessageType>(...)`, and that file
holds all of them and nothing else.

### What every generated executable accepts
<a href="#what-every-generated-executable-accepts"></a>

The command line is generated from `dmmeta.field` rows on `command.<ns>`, so
every tool shares the same parser and the same conventions: `-v` / `-verbose`
and `-d` / `-debug` repeat to raise a level, and `-h`, `-version` and
`-signature` are diagnostic.  A tool that reads ssimfiles also takes `-in`,
defaulting to `data`.

```bash
acr field:command.<ns>.%     # the options of one tool, from the schema
acr_in <ns regx>             # the ssimfiles it loads
acr_in <ns regx> -data       # the tuples it loads
acr_in -r <ssimfile regx>    # which tools read this table
```

### Documentation is generated too
<a href="#documentation-is-generated-too"></a>

`abt_md` is to `txt/` what `amc` is to `cpp/gen`.  A heading matching a pattern
in `acr mdsection` owns a generated section beneath it; an `inline-command:`
line inside a preformatted block is executed and its output replaces the rest
of the block; the `<a href="#...">` anchor under every heading is written by
the tool rather than by hand.  Prose is imperative and present tense: "update
key; write file".

The ways a generated document can be stale while every local check passes are
in [/txt/rule/openacr.md](/txt/rule/openacr.md), along with the traps a stale
`amc` produces.
