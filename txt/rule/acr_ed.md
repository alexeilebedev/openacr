## acr_ed: the rules
<a href="#acr_ed-the-rules"></a>

`acr_ed` edits the schema.  Read
[/txt/rule/openacr.md](/txt/rule/openacr.md) beside this file,
[/txt/rule/acr.md](/txt/rule/acr.md) for the behavior `acr_ed` inherits from
the tool it drives, and [/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md)
for the option-by-option reference and the worked examples.

### acr_ed writes a script; it does not edit anything itself
<a href="#acr_ed-writes-a-script-it-does-not-edit-anything-itself"></a>

Run `acr_ed` without `-write` and it prints a shell script to stdout.  Add
`-write` and it runs that same script, which is the whole of the difference.
The script is a `bash` program built from three parts, in this order: one `acr
-replace -check -write -t` fed the proposed records on a heredoc, then whatever
extra commands the action accumulated, then `bin/amc` when the action declares
it needs one.

Two properties fall out of that shape and both are worth keeping.

`acr` has no dependency on `acr_ed`.  Everything `acr_ed` does is expressible
as `acr` commands, and printing them is what lets a reader check the edit
before it happens.  An action that cannot be written as a script -- one that
reaches into the database in C++ and writes the file itself -- breaks the
property, and with it the ability to inspect an edit.

The script is a transaction only in the weak sense that `set -e` stops it at
the first failure.  Nothing rolls back, so an action whose second command fails
leaves the tree in whatever state its first command produced.  `-sandbox` is
the way to find out what a new action does before it does it anywhere real.

### One action per invocation
<a href="#one-action-per-invocation"></a>

`ProcessAction` counts the selected `dev.edaction` rows and refuses both zero
and more than one, printing the table so the caller can see what was on offer.
An action is selected by the combination of `-create` / `-del` / `-rename` and
whichever entity option carries a value, and the selection logic lives in one
place per verb.

Adding an action is therefore three things: a `dev.edaction` row carrying
`needamc` and a one-line usage `comment`, a case in the matching `Select*`
function, and the handler itself.  `needamc:Y` is the declaration that the
edit changes what `amc` would generate; `Create_Srcfile`, `Delete_Srcfile` and
`Rename_Srcfile` are the three that set it to `N`, because a source file's
existence is not schema.

### A schema edit carries its data consequence
<a href="#a-schema-edit-carries-its-data-consequence"></a>

`acr` reads a data row against the current ctype and ignores an attribute
naming no field of it, so removing a field from the schema leaves that field's
values sitting in the ssimfile with nothing to object.  The full account of why
is in [/txt/rule/acr.md](/txt/rule/acr.md); what matters here is whose job it
is to finish the edit.

It is `acr_ed`'s.  An action that changes what a row looks like on disk emits
the command that rewrites the rows, in the same script, after the schema
change.  `Delete_Field` rewrites every ssimfile whose ctype -- or whose ctype's
base chain -- carries the deleted field.  `Rename_Ssimfile` passes `-g -x` so
that the rows' attribute key is rewritten and the physical file moves under
git.  A new action is judged the same way: name the files whose contents the
edit changes, and make the script rewrite them.

The test for whether an action is finished is that `bin/normalize` has nothing
left to do.  A rewrite the action skips is not invisible -- it is deferred to
the pre-merge gate, where it costs a full pipeline and reads as an unrelated
`normalize_acr` diff.

### A field rename carries its column, and `acr` is what carries it
<a href="#a-field-rename-carries-its-column-and-acr-is-what-carries-it"></a>

`Rename_Field` emits one command -- `acr field:<old> -rename:<new> -write` --
with no rewrite beside it.  That reads like the omission `Delete_Field` was
fixed for, and it is not one: `acr` renames the column itself.  When the record
whose pkey is changing is a `dmmeta.field`, `ScheduleCascadeUpdate` in
`cpp/acr/query.cpp` schedules a second pass over the ssimfile of the field's
ctype which renames the matching attribute in every row, so the schema row, the
data column and every Pkey reference to the field move in one `acr` run.
Renaming `x2db.Gwproto.offset` to `portoffset` rewrites `data/x2db/gwproto.ssim`
alongside `field.ssim`, `funique.ssim` and `dispsig.ssim`, and leaves
`acr -check % -x` at `n_err:0` with the file already in normal form -- a
re-run of `acr 'x2db.gwproto:%' -write` reports `n_file_mod:0`.

The pass is skipped for a fldfunc, which has no presence in any ssimfile, and
for a ctype with no ssimfile of its own.  It reaches one file only, the ssimfile
of the field's ctype, where `Delete_Field` walks the ctype's `Base` chain as
well.  Today no ssimfile ctype has a `Base`, so the two cover the same ground;
the first one to gain a `Base` splits them.

Because a rename stays inside one ctype, the ctype is already known and the new
name may be given bare: `acr_ed -field a.B.c -rename d` and
`... -rename a.B.d` are the same edit.  `Rename_Field` refuses the spellings
that cannot mean what they look like, rather than guessing.

A `:` anywhere in the new name is refused as `acr_ed.rename_prefix`.  The
spelling that produces it is the query one, `-rename field:a.B.d`, where `acr`
would read `field:a.B` as the ctype and write a record whose pkey no query
finds.  The prefix is refused rather than stripped, so `-rename` keeps one input
language.

A new name carrying a *different* ctype is refused as `acr_ed.rename_ctype`
whenever either ctype has an ssimfile, and the reason is the scheduled pass
above: it runs over the ssimfile of the ctype in the **new** pkey, so the old
table's rows would keep a column no field claims -- the state `Delete_Field`
exists to avoid, and one `acr -check` cannot see, since tolerating an unclaimed
attribute is what it is built to do.  Moving a field between ssim-backed tables
is a delete and a create.  Between two in-memory ctypes there is no column to
strand, so that form goes through.  A new name whose ctype does not exist at all
is refused as `acr_ed.no_ctype`, which is what a partially qualified name such
as `-rename Gwproto.portoffset` resolves to.  `acr_ed.RenameField` pins all four
outcomes.

What a rename cannot carry is the C++ that reads the field.  `amc` renames the
struct member and generates no alias, so `gwproto.offset` in a hand-written file
becomes `FGwproto has no member named 'offset'` -- the same property a ctype or
a target rename has, and the caller's to fix.

### The reftype comes from the field name
<a href="#the-reftype-comes-from-the-field-name"></a>

A field-name prefix implies its reftype, and the mapping is a table rather than
a chain of compares in the code: `acr fprefix` lists it.  `ind_` is a `Thash`,
`cd_` a circular doubly-linked `Llist`, `p_` an `Upptr`, `c_` a `Ptrary`, `bh_`
a `Bheap`, `tr_` an `Atree`.  With no prefix, `acr_ed` picks `Pkey` when the
field's ctype and its arg both have ssimfiles, and `Val` otherwise.

`c_` is claimed by both `Ptr` and `Ptrary`, and the choice is settled later:
an xref that goes through the child's primary key means at most one child, so
the field becomes a `Ptr`.

`acr_ed` will also guess the arg from the name.  `<ns>.FCtype.p_ns` finds
`<ns>.FDb.ns`, takes that pool's arg, and turns the xref on.  A guess is a
convenience and never a requirement, so any of it can be overridden by naming
`-arg`, `-reftype` or `-via` explicitly.

### An xref needs a path from the parent to the record
<a href="#an-xref-needs-a-path-from-the-parent-to-the-record"></a>

An index in table A over records of type B is possible when a function exists
that finds the A given the B.  `FDb` is always reachable, so a global index
always works.  A partitioned index -- one that lives on some other ctype --
makes `acr_ed` search B's fields and the global hashes for a key that reaches
A.

Zero paths and more than one path are both errors, and both are reported with
the candidates so the caller can pick.  `-via` names the choice: either a
pointer field on the child, or a `<hash>/<key>` pair where the key is the
**ssimfile** field name, since a Base-imported field has no field record of its
own.

### The visualization is part of the output
<a href="#the-visualization-is-part-of-the-output"></a>

When an action touches the shape of an in-memory database it registers the
ctypes involved, and `acr_ed` runs `amc_vis` over the existing dataset plus the
proposed records.  The drawing is appended to the record block as comment
lines, so it arrives inside the heredoc alongside the rows it describes.  The
reader then sees the structure the edit produces before agreeing to it, which
is a different question from whether each record is well formed.

The proposed records go through `acr %` rather than `cat` on the way to
`amc_vis`, because `acr.rowid` attributes would break it.

### `-sandbox` answers "what would this do to the tree"
<a href="#-sandbox-answers-what-would-this-do-to-the-tree-"></a>

`-sandbox` resets the `acr_ed` sandbox worktree, runs the whole transaction
inside it with `-write` forced on, and then shows the diff.  Inside a sandbox
`acr_ed` also builds and runs `amc` from source, because the sandbox is where a
change to the generator itself gets tested.

This is the answer to "I want to see the generated code this schema change
produces", which the printed script cannot show.

`-showcpp` is declared on the command line and read nowhere: `acr_ed` shows the
sandbox diff whenever `-sandbox` is given, and the flag selects nothing.
