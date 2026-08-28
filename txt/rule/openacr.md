## openacr: Rules
<a href="#openacr-rules"></a>

This file carries the rules the openacr tools obey.  Read it before changing `acr`, `acr_ed`, `amc`, `abt`, `abt_md`,
`src_func`, `apm`, or any other namespace that belongs to the openacr
distribution, and read it beside that namespace's own rule file where one
exists.  It carries the invariants the whole toolchain obeys, the vocabulary
its names come from, and the traps that a change to any one tool can walk
into.

The audience is an agent.  A rule here is stated with the detail needed to act
on it -- the exact command, the exact error text, the reason the obvious
alternative fails -- because that is what a reader has to have before touching
the code.  The pages under [/txt/openacr](/txt/openacr/README.md) are the human
introduction to the same system, and they stay short on purpose.

The file has two halves and they are read differently.  The rules are read
through, before a change.  The `####` sections under [Reading and editing the
database](#reading-and-editing-the-database), [Rebasing and
regenerating](#rebasing-and-regenerating), [Gates that pass without checking
anything](#gates-that-pass-without-checking-anything) and [Publishing a
package](#publishing-a-package) are recipes, searched on the symptom the tool
printed at you, the way any recipe file is searched:

```bash
grep -n "can't find build/gitinfo.h" txt/rule/openacr.md
grep -n "acr_dm.mergefail\|unrecognized attr" txt/rule/openacr.md
```

### Everything is data
<a href="#everything-is-data"></a>

Every fact openacr knows about a program is a record in an ssimfile, and the
code is derived from the records.  A ctype is a row, a field is a row, a build
target is a row, and so is a command-line option, a step, a hash index, a unit
test and a source file.  `amc` reads the rows and writes the C++; `abt` reads
the rows and links the binaries; `abt_md` reads the rows and writes the
documentation.

Three rules follow, and they are the ones a change is judged against.

**Generated files are never edited.**  Anything under `cpp/gen`, `include/gen`
or `ts/gen` is amc's output, and an edit there survives exactly until the next
`amc` run.  The way to change generated code is to change the record it was
generated from, or to change the generator.

**A new capability is a new table or a new field.**  When a tool grows a
behavior that depends on which case it is in, the case is data and the switch
that selects it is a table nobody has written down yet.  `dmmeta.fprefix` is
that rule applied to field-name prefixes: the mapping from `ind_` to `Thash`
could have been a chain of string compares inside `acr_ed`, and instead it is
twenty rows anyone can query and extend.

**A fact has one home.**  The same value stored in two rows is one update away
from disagreeing with itself.  Where a second copy is genuinely needed it is
derived -- a substr field, a cppfunc field, a Count xref -- so that the copy
cannot drift from its source.

### An openacr document illustrates with openacr's own schema
<a href="#an-openacr-document-illustrates-with-openacr-s-own-schema"></a>

The acr page used to explain what happens after a field is deleted, and it
explained it like this: run `acr_ed -del -field amsdb.Proctype.hbtimeout
-write`, and afterwards `ssimfile:amsdb.proctype` still reads `amsdb.proctype
proctype:samp_meng  id:23  ns:samp_meng  ...  hbtimeout:30`.  A reader who came
to learn what a field delete does now has to work out what a proctype is, what
its `id` is for, and why one of them is called `samp_meng`.  None of that is
about `acr`.  Worse, there is no proctype in the openacr distribution at all,
so the reader cannot look the table up even if they want to.

The openacr namespaces ship as a package -- `dev.package:openacr` -- and that
package is read by people who have never seen the tree it was published from.
A name belonging to a downstream project is noise to them at best, and a
dangling reference at worst.

The rule therefore binds every document the package carries, which is
`txt/openacr`, `txt/exe`, `txt/ssimdb` and `txt/tut`; ask `apm
-package:openacr -l -showfile` for the list.  This file is not among them, so
it illustrates with whatever example is clearest, downstream tables included.

A published document about an openacr tool illustrates with a table that same
package carries.  `dmmeta.ns`, `dmmeta.ctype`, `dmmeta.field`, `dev.target` and
`dev.gitfile` are the standing examples, they are small, and every reader
already has them.  `apm -package:openacr -l -showrec` answers what else the
package holds.

One check enforces part of this and it is worth knowing its limit.  The
`apm_nodownstream` citest evaluates each package that declares a
`dev.package.nomention` regx and refuses the package if a *record key* or a
*file name* in it matches.  It does not read the contents of the files.  A
downstream table named in the prose of an openacr document therefore passes
every gate in the tree, which is why this rule is on the writer.

The rule reaches examples in code as well as in prose.  A comptest of an
openacr tool, a tutorial under `txt/tut`, a sample invocation in a function
comment -- each is read by the same audience and picks its table by the same
test.

### Who each document is written for
<a href="#who-each-document-is-written-for"></a>

Four document sets cover openacr, and each answers a different question.
Putting a fact in the wrong one is how the same fact ends up in three of them.

| Read for | Document |
|---|---|
| what openacr is, and why it is shaped this way | [/txt/openacr](/txt/openacr/README.md) -- short, human, principles |
| the rule a change is written against | `txt/rule/<ns>.md`, plus this file |
| every option of one tool, and examples of that tool | `txt/exe/<tool>/README.md` |
| the columns of one ssim table | `txt/ssimdb/<ns>/<table>.md` |

Two consequences.  An example of a tool belongs in that tool's README, so a
page about `acr` shows `acr` and names `acr_ed` with a link rather than
demonstrating it.  And a page under `/txt/openacr` that has grown a section of
traps has grown the wrong thing: the traps belong here, and the page keeps the
principle that made them worth knowing.

### The schema rules
<a href="#the-schema-rules"></a>

These govern hand-editing or extending the ssim schema and the in-memory
database `amc` generates from it.

**String defaults need inner quotes.**  A `dmmeta.field` `dflt` for a string is
`'""'` for empty and `'"release"'` for a value.

**An integer reader clamps; it does not refuse.**  `u32_ReadStrptrMaybe`,
`i32_ReadStrptrMaybe` and their siblings cap a number at the type's limit and
return `true`, so `8589934592` into a `u32` field stores `4294967295` and reads
as success.  `<Ctype>_ReadFieldMaybe` passes that straight through.  A command
that sets a field from a user's number therefore echoes the field rather than
the number it was handed, or the caller reads their own input back and never
learns the value was clamped.

**Thash cursors are opt-in.**  They are not generated by default.  Insert
`dmmeta.fcurs  fcurs:<ns>.<Ctype>.<field>/curs  comment:""` and run `amc`.

**Fieldwise constructors are opt-in too.**  Merge `dmmeta.cpptype
ctype:<ctype>  ctor:Y` and run `amc`.

**Never add xrefs or indexes to an ssimdb ctype.**  An ssimdb namespace --
`dmmeta`, `dev`, `atfdb` and the rest of `acr ns -where nstype:ssimdb` --
defines schema and wire format only, and has no in-memory pool for an index to
live in.  The index belongs on the wrapping `<lib_or_exe>.F<Ctype>`, the
per-tool struct whose first field is `Base <ssimdb-ctype>`.  So
`field:dmmeta.Field.zs_fcb` is nonsense and `field:amc.FField.zs_fcb` is the
same idea in its right home.

**No fconst-on-`u8` enum field in an ssimdb table.**  Such a field prints its
symbolic names in the ssimfile, and the MariaDB round trip
(`normalize_acr_my`) maps `u8` to an integer column and rejects the symbol with
`Incorrect integer value: 'all'`.  An enum has two supported homes instead.  It
can be a small ssimfile of its own, referenced by `Pkey`, with `dmmeta.gconst`
rows for the compile-time constants.  Or, when the field lives on a non-ssimdb
ctype, it can be a value ctype in a protocol namespace with a `printfmt:Raw`
cfmt.  The second form is unavailable inside an ssimdb namespace, where every
ctype must be an ssimfile (`amc.need_ssimfile`).

**A `RegxSql` field on an ssimdb table takes `arg:algo.cstring`.**  A field
typed by a ctype inherits that ctype's `dmmeta.sqltype` column, and a pattern
is longer than any single key it matches, so the MariaDB round trip refuses the
row with `ssim2mysql.error Data too long for column ... at row 1`.
`algo.cstring` maps to `text` and has no such ceiling.  Name the table the
pattern ranges over in the field's `comment`, since the arg no longer records
it and the referencing table drops off the target's generated doc page.  A
ctype-typed `RegxSql` is fine on an in-memory ctype, which never reaches a
column.

**`via` in `dmmeta.xref` is a pointer field or a `<hash>/<key>` pair.**  The
hash is the parent's Thash field, and the key uses the **ssimfile** field name
on the child rather than the in-memory ctype field, because Base-imported
fields have no field records of their own.

**A varlen field goes last in a packed protocol message.**  To add a Val field
after one, use `acr_ed -create -field ... -before <existing>`.  Row order in
`field.ssim` is struct layout, so this is a real constraint rather than a
formatting preference.

**No `algo.cstring` in a packed protocol message.**  A wire ctype is a packed
struct and `algo.cstring` is an in-memory pointer-plus-length view, which
cannot go on the wire.  Use `reftype:Varlen arg:char` and reach the value
through the generated getter.

**Base a message on `ams.MsgHeader` only in a namespace compiled into
`lib_prot`.**  A message header is a ctype carrying a `dmmeta.typefld` row, and
`amc` collects every message whose ultimate base is that header and generates
one read/print dispatch over the whole set, in the header's own namespace.  For
`ams.MsgHeader` that dispatch is `ams::MsgHeaderMsgs`, compiled into
`lib_prot`.  A message based on it from an app-private namespace joins that
dispatch, so `lib_prot` comes to reference a symbol only that one app links,
and every other target fails at link time.  Check membership with `acr
targsrc:lib_prot/%`.  An app-private message gets a private header ctype
instead: a `u32` carrying `dmmeta.typefld` and a `u32` carrying `dmmeta.lenfld`
reproduce `ams.MsgHeader`'s eight-byte layout, so the wire bytes are unchanged
and `amc` generates the app's own dispatch.  When the messages are packed, pack
the header too, or `amc` refuses with `amc.back_pack` and prints the
`dmmeta.pack` row to insert.

**Message formatting needs a `dmmeta.pnew` row.**  `MsgType_FmtByteAry` and
`MsgType_FmtShm` are generated per target and per message, keyed
`<target>/<ctype>.<allocator>`, and without the row the function does not
exist.

**Always call `XrefMaybe` after allocating.**  `pool_Alloc` produces the
record, the fields are filled, and `pool_XrefMaybe(rec)` is what inserts it
into every index.  The one exception is a record deleted again immediately
without being used.

**Tpool recycles memory and has no `RemoveAll`.**  Use `Lary` when the whole
pool has to be cleared between iterations.

### Reading and editing the database
<a href="#reading-and-editing-the-database"></a>

#### acr -in:run/x2inv -check reports Invalid value on rows that are fine
<a href="#acr-inrun-x2inv-check-reports-invalid-value-on-rows-that-are-fine"></a>

Checking the attached dataset on its own reports hundreds of bad references, one
per row, in the shape

```bash
run/x2inv/awsdb/awspecacctkey.ssim:1: Invalid value awspec:aws1.ami-builder
run/x2inv/omdb/omhost.ssim:1: Invalid value net:ext-0
run/x2inv/x2rdb/nodeacct.ssim:1: Invalid value user:x2admin
```

Nothing is wrong with the dataset.  `acr` resolves exactly one input root and has
no `-invdir`, so `-in:run/x2inv` makes the dataset the whole database and every
foreign key whose parent stayed in `data/` then points outside the world acr can
see.  The straddling keys are the ones the split cut across: `omdb.omhost` to
`x2rdb.net` and `x2rdb.env`, `x2rdb.nodeacct` to `x2rdb.user`, `x2rdb.nodecname`
to `x2rdb.intfname`, `x2rdb.nodehost` to `x2rdb.devintf`, and
`awsdb.awspecacctkey` to `awsdb.awspec`.  This is why omcli's own sort of
`omdb.omhost` runs without `-check`.

Check both layers by handing acr one stream that holds them:

```bash
(acr %; acr % -in:run/x2inv) | acr -in:- -check %      # report.acr_check  n_err:0
```

Each root is dumped by acr and the two dumps are read back as a single stream, so
the check sees one database and the straddling keys resolve.  A `cat` of
`data/*/*.ssim run/x2inv/*/*.ssim` into a temp file answers identically today and
is the older form of this recipe.  The pipeline needs no temp file, and it asks
acr for each root's rows rather than a shell glob, which assumes every ssimfile
sits exactly two directories down — true now, and a property of the names rather
than a rule.

Run it against `run/x2inv` alone before trusting a clean answer, since that is the
form that must fail: it reports `n_err:6154` and exits 1, while the union reports
0 and the two record counts sum to the union's.

`acr` will not grow a flag for the second root.  A layer here is the *inventory*,
which is a concept of the tree that carries one, and acr is domain-neutral — it
knows ssimfiles, roots and ctypes, and nothing about what the rows describe.  The
two dozen tools carrying `-invdir` are that tree's own, and the flag means
something to each of them.  Widening acr's `-in`
to a list was tried instead and abandoned: `-in` names both where rows are read
and where they are written, so a second root reopens where an inserted row lands,
for a gain the pipeline above already delivers.

A per-layer check cannot be made to pass, so a clean answer from the concatenated
form is what says the dataset is consistent.

#### acr.bad_dflt Default names a key that does not exist
<a href="#acr-bad_dflt-default-names-a-key-that-does-not-exist"></a>

`acr -check` reports a `dmmeta.field` row, not a data row:

```bash
data/dmmeta/field.ssim:5294: acr.bad_dflt  field:command.x2ca.certtype  dflt:nosuch-type-99  arg:x2certdb.Certtype  comment:"Default names a key that does not exist"
```

The field declares `reftype:Pkey` with an `arg` naming an ssimdb ctype, so its
`dflt` is the key the tool resolves when the operator passes no flag.  That key is
a foreign key and it names no row.  Fix the default, or create the row it names:

```bash
acr certtype:%                                   # what the table actually holds
echo 'dmmeta.field  field:command.x2ca.certtype  dflt:'"'"'"serverClient"'"'"'' | acr -update -write
```

Read `n_file_mod:1` to know the write landed.  `acr -update -write -check` on a row
that is *currently* bad checks before it writes, so the check fails on the old value,
the write is refused, and the `acr.update` line still echoes the row you meant —
`n_file_mod:0` is the only part that tells you nothing happened.

Two defaults will not report even when they name nothing.  A default that is not a
string literal is not a key: `dflt:0` initializes a pointer and `dflt:'""'` says the
operator must supply the value.  And a target table that loaded zero rows is skipped,
because the inventory tables are tracked here as empty files whose rows arrive from
the x2inv dataset, and a checkout with nothing attached cannot tell a key that was
never created from one it was not given.  Four live defaults sit in that state —
`command.x2ca.certcanode`, `command.omcli.x2cert_certcanode`,
`command.x2dist.vtpath` and `x2rdb.K8host.k8cluster`.  Check them by handing acr both
layers as one stream, the form the recipe above gives:

```bash
(acr %; acr % -in:run/x2inv) | acr -in:- -check %
```

#### acr -insert drops a misspelled attribute and reports n_err:0
<a href="#acr-insert-drops-a-misspelled-attribute-and-reports-n_err0"></a>

A tuple whose attribute name is misspelled inserts anyway.  `acr` recognizes
the attributes it knows, silently ignores the one it does not, and the row
lands with that field at its declared default.  The insert reports success at
every level: `report.acr_check  records:13  n_err:0`, then
`n_insert:13  n_file_mod:3`.

Typing `atg:bool` for `arg:bool` on a new `dmmeta.field` row produced

```ssim
dmmeta.field  field:report.x2aws_verify.success  arg:""  reftype:Val  dflt:""  ...
```

which is a field with no type.  Nothing objects until `amc` generates against
it, and what surfaces there names the generated artifact rather than the typo
that caused it.

The cause is that the check validates *values*, not attribute names: an
unknown attribute is not a bad value, so no rule fires.  This is the same
family as a moved table arriving without a field — the default is what hides
it — and it is why `n_err:0` on an insert says nothing about whether the row
you meant is the row you got.

So grep the file for the key after any insert that matters, and read the
attributes rather than the report line:

```bash
grep -n 'field:report.x2aws_verify' data/dmmeta/field.ssim
```

Repair with `acr -replace -check -write`, passing the whole corrected tuple;
`-replace` updates the row in place and reports `n_update:1`.

#### A moved ssim table arrives missing a field, and the default hides it
<a href="#a-moved-ssim-table-arrives-missing-a-field-and-the-default-hides-it"></a>

Rows exported to the x2inv dataset by a binary built against an older schema
arrive without the fields that binary did not know about.  Nothing reports it.
The load succeeds, `acr -check % -x` passes at `n_err:0`, and every absent
field reads as its declared default, so the tables look complete and the
database is quietly wrong.

`x2rdb.nodecname` is the case that showed it.  All 86 rows reached x2inv with
no `sshalias` field.  That field says a cname is also an ssh entrance to its
node, and it defaults to false, so 85 rows holding `sshalias:N` lost nothing.
One row held `sshalias:Y`, and its loss took that cname off the Host line in the
generated script for the node it named.

The symptom is therefore not an error but a diff: regenerating a committed
artifact stops reproducing the committed bytes.  That makes byte-identical
regeneration the check that a migration is honest, and the reason to run it
before believing any table has moved:

```bash
bin/x2admin -generate
git -C ~/x2inv status --short -- x2admin/gen/script   # silence means identical
```

To find the loss directly, diff each moved file against the revision that
still holds its rows — the field set is what matters, not the row count, since
a row with a field missing is still one row:

```bash
for f in ~/x2inv/x2rdb/*.ssim; do b=$(basename $f)
  diff <(git show master:data/x2rdb/$b | sort) <(sort $f) | grep -q . && echo "DRIFT $b"
done
```

Repair by re-exporting from the revision that still carries the rows, with a
binary built from the current schema.

#### The closure of a table to move is wider than dmmeta.field shows
<a href="#the-closure-of-a-table-to-move-is-wider-than-dmmeta-field-shows"></a>

Moving a table means moving everything keyed on it, because each loader inserts
parents first and resolves references as it goes — a child left behind whose parent
arrives with a later layer cannot load, whatever the layer order:

```bash
x2admin.bad_xref  index:x2admin.FDb.ind_device  key:awin1.ctrl-1
data/x2rdb/devintf.ssim:1: x2rdb.devintf  devintf:awin1.ctrl-1/ctrl  subnet:awin1.ctrl  ip:""
```

The obvious way to enumerate that closure is to grep the field graph:

```bash
grep -n "arg:x2rdb.Device" data/dmmeta/field.ssim     # names 7 of 14 tables
```

It is wrong, and quietly.  A key can be computed rather than declared:
`awsdb.Awnlb.device` is a cppfunc returning `algo.Smallstr50`, so its `dmmeta.field`
row records no reference to a device at all, and the edge exists only as a
`dmmeta.xref`.  Four more tables hang below `awnlb` and never appear.

Read the closure off the xref table instead, which resolves computed keys and
declared ones the same way:

```bash
grep -n "ind_device/" data/dmmeta/xref.ssim | sed 's|.*via:||'
```

Confirm it by measurement rather than by reading, since a tool indexes only some of
what it loads.  Copy `data/` aside, empty the table there, hand the rows back as the
layer, and re-run each reader until it loads clean — every file the loader names on
the way is one more table in the closure:

```bash
J=temp/split; rm -rf $J; cp -r data $J/base; mkdir -p $J/layer/x2rdb
cp data/x2rdb/device.ssim $J/layer/x2rdb/; : > $J/base/x2rdb/device.ssim
bin/x2aws -in:$J/base -invdir:$J/layer -device:nosuchdevice-zz
```

The mirror case is worth knowing so a clean run is not over-read: a declared Pkey
that no tool hash-indexes is never checked at load, so its table moves alone without
complaint.  `awsdb.Awspec.user` is one.  Enforcement follows the xref, not the schema.

### The generated tree goes stale in ways that read as success
<a href="#the-generated-tree-goes-stale-in-ways-that-read-as-success"></a>

`amc` runs with no arguments and regenerates globally; there is no partial
mode.  Its query forms -- `amc <ctype>` prints a struct, `amc <func>` prints a
function -- are for reading, not for generating.

The failure this section is about is always the same shape.  A generator or an
input is older than the schema it is being run against, every command exits
zero, and the damage surfaces somewhere unrelated.

**A stale `bin/amc` deletes the branch's generated code.**  On a branch that
modifies `amc` itself, rebuild it with `abt amc -build -install` before running
it.  The installed binary exits clean and emits the previous generator's
output, and the deletion surfaces later as compile errors in consumers of
`cpp/gen`.  The same staleness follows any move across commits that touched
`cpp/amc`, and [Rebasing and regenerating](#rebasing-and-regenerating) below is
the whole of that case.

**A stale generator can also fail outright.**  It reports a schema violation
against a ctype the branch never touched -- `amc.dangling_pointer` naming an
upstream field, then `amc.no_output` -- because the old generator is judging
the merged schema by its own rules.  That pair reads like a schema mistake and
is not one.  Confirm the ssim inputs match upstream with `git diff
origin/master -- data/`, rebuild `amc`, and rerun.

**`amc` reaching `n_filemod:0` is not evidence the generated tree is
current.**  Several artifacts are derived by citests rather than by `amc`, and
`apm/gen/<package>.ssim` is the one that catches people out: the `apm_gen`
citest owns it, runs near the end of `normalize`, and rewrites an input that
`pbapi_gen` has already read and passed against.  Drive the generation to a
fixpoint before committing, then check the gates as a group:

```bash
atf_ci -citest:'pbapi_gen|apm_gen|apm_check|fast_gen|normalize_acr|checkclean'
```

A citest reporting `atf_ci.modified_files` is naming a file it regenerated, so
the answer is always to commit that file.  And `atf_ci -check_clean` refuses to
run on a dirty tree, so the commit has to come before the gate can confirm it.

**Renaming a doc file moves its hash in the package manifest.**  Regenerate
with `apm -package:<package> -generate`, which is exactly what `apm_gen`
checks; without it the rename looks complete and fails the pre-merge gate.

The codestyle skill carries the full cascade table -- which edit stales which
artifact, from a renamed citest function through a `dmmeta.ns` comment to a
deleted `dev.gitfile` row.  The order that converges is the same in every case:
make every edit first, then run `amc`, `update-hdr` and `abt_md` as a group,
then commit, then `bin/normalize`.

### Rebasing and regenerating
<a href="#rebasing-and-regenerating"></a>

#### Regenerate before you build, after a rebase
<a href="#regenerate-before-you-build-after-a-rebase"></a>

The `amc` you run after a rebase is the binary you built before it, and when
the commits you rebased across changed the generator, that stale binary
rewrites their generated output back to what it used to be.  The symptom is an
`amc` run reporting a nonzero `n_filemod` naming files your branch never
touched, whose diff reverts a change master just made.  Nothing in the run says
it is wrong: `n_filemod` is how a legitimate regen reports itself, and the
reverted files are generated ones you are expected not to read.  So after a
rebase, build before you generate — `abt amc -build -install`, then `amc`,
which reports `n_filemod:0` if the tree was already consistent.  The same
applies to any tool whose output you are about to commit: `acr`, `update-hdr`,
`abt_md`.

The trigger is not the rebase.  It is the working tree moving across commits
that touched `cpp/amc`, so a `git reset --hard` onto a diverged remote branch
does it, and so does a plain checkout or pull.  In each case `bin/amc` is a
symlink into `build/release`, still holding the binary the previous tree built,
and that binary knows the schema the previous tree had.  What it rewrites is not
confined to `cpp/gen` and `include/gen` either: the amc-owned ssim tables go
with it, so rows in `ssimfile:dmmeta.msgfield` come back with
`strtype:rightpad  pad:0` flattened to `strtype:""  pad:""` — which reads as a
schema edit nobody made.

Two symptoms, and the one that reaches you first is a build error in
hand-written code that calls a generated accessor the old generator spelled
differently:

```c++
cpp/lib_x2net/sock.cpp: error: cannot convert 'algo::aryptr<ams::MsgHeader>' to 'ams::MsgHeader*' in initialization
```

The tell is that it names a file the change never opened.  The second symptom is
`git status` listing generated files and amc-owned tables modified in namespaces
the work has nothing to do with — easy to skim past, because a regen is expected
to touch generated files.

Recovering costs one rebuild.  Put the generated tree and the amc-owned tables
back, build the generator, then regenerate:

```bash
git checkout HEAD -- cpp/gen include/gen data/dmmeta/msgfield.ssim data/dmmeta/ctypelen.ssim
abt amc -build -install
amc
```

A second `amc` then reports `n_filemod:0`, and what it did modify is scoped to
the change.  Rebuild the whole tree afterwards with `abt % -build -install`: the
objects from the bad generation are still in `build/`, and the targets that
failed to compile were never installed.

A branch that adds a unit test conflicts, when rebased across a master commit
that also added one, in the rows that register the test and in the two
generated `atf_unit` files amc derives from them.  Which files actually
conflict depends on how close the two sorted keys land, and every one of those
conflicts is a union — both rows belong — so keep both arms in sorted order and
resolve the generated pair by taking your arm and letting amc re-derive it.
The confusing part is what happens if you build before rerunning amc: your arm
of the generated header predates master's new test, so its prototype is absent
and the build fails on an undeclared `unittest_<ns>_<Name>`.  That is the
expected intermediate state of an unfinished resolution, not a signal about
either branch.

A driver that gives up without writing leaves a file that carries no conflict
markers, so an unresolved path looks like a resolved one.  The driver runs with
`-write_ours`, and a non-zero exit that wrote nothing leaves our arm in the
working file while git marks the path `UU`.  During a rebase "ours" is master,
so what lands is master's copy without any of the rows the branch adds, and
searching the file for `<<<<<<<` finds nothing.  Staging it therefore drops
those rows without saying so, and the loss surfaces much later as a generation
citest — `apm_gen`, for the file under `apm/gen/` that a `dev.pkggen` row names —
reporting that the file no longer matches what the generator produces.

Two changes to `acr_dm` mean a current binary does not do this.  A line it
cannot key — a generated package file carries dozens of them, each a
`# SHA1 = <base64>` above the `dev.gitfile` row it checksums — is held as the
text of the row below it and merged as that row's own attribute, so
`acr_dm.error  Missing key attribute` is not a thing the driver prints any
more.  Seeing that string at all means `bin/acr_dm` predates the fix, and one
`abt acr_dm -build -install` is the whole repair.  And a failure that does
happen writes both arms whole between markers and prints `acr_dm.mergefail` on
stderr, which is unusable on purpose: nothing can mistake it for a merged
result.

Either way, resolve such a path by merging the three stages explicitly, then
check that the result still contains your rows:

```bash
for s in 1 2 3; do git show :$s:<path> >/tmp/stage$s; done
git merge-file -L mine -L base -L master /tmp/stage3 /tmp/stage1 /tmp/stage2
cp /tmp/stage3 <path> && git add <path>
```

Stage 3 is the branch's arm, stage 1 the merge base and stage 2 master's arm;
`git merge-file` writes the merged text back into the first file it is given.

The same unmarked `UU` appears for a second reason, and it is the one to rule
out first because it costs a rebase rather than a file.  `bin/acr_dm` is a
symlink into `build/release`, so in a worktree that has never been built the
driver is not there at all.  Git then reports every ssim path as conflicted,
each holding master's copy with no markers, and the printout says nothing about
a missing binary — `.gitattributes` names `merge=acr_dm`, the config names its
command line, and a command that does not exist simply fails.  The tell is that
*every* ssim file the two arms both touched comes out `UU` while `grep -c
'^<<<<<<<'` returns 0 on all of them, where a genuine driver refusal hits one
or two paths and prints `acr_dm.error`.  A worktree is set up for a rebase by
building it first: run `ai`, which bootstraps `abt` and then builds every
target, and redo the rebase with the driver present.  Twelve unmarked
conflicts became four real ones — all of them `dispsig` hashes amc recomputes —
once it was.

A merge can also move a row while announcing nothing, and an order-dependent
table is where that matters.  `dmmeta.ssimsort` sorts `dmmeta.field` by
`dmmeta.Field.ctype` and by nothing else, so field order within a ctype is
whatever the file says, and that order is the struct layout amc generates.  The
next `acr` write then hides the evidence, regrouping rows under their ctype and
leaving a moved field last in its ctype rather than first.  `acr_dm.FieldOrder`
and `acr_dm.Reorder` pin the two ways this used to happen; what outlives them is
how to check such a merge, since nothing in the output says a row moved:

```bash
acr_dm <base> <ours> <theirs> -anchor
```

Each row is printed with the row it was placed after, so a row that jumped
reads as one whose anchor is nowhere near it in the file.  A table is
order-dependent exactly when `acr` prints an `acr.rowid` attribute for it,
which one query answers:

```bash
acr <ssimfile>:% -rowid | head -3
```

`dmmeta.field` shows `acr.rowid:0`, `acr.rowid:1`, … and is at risk;
`dmmeta.ctype`, sorted by its own primary key, shows no such attribute and
cannot be reordered by a merge.

A position is merged by the rule the attributes are merged by, so a deliberate
move survives a rebase and a contested one is reported.  `acr_dm.moveconflict` on
stderr, and a `# acr_dm.moveconflict` line above a marker block whose two sides
are the same row byte-for-byte, says both arms moved that row and to different
places: the line names both positions, and resolving it means picking one and
deleting the row from the other side.  The same message with `cycle:Y` says the
two moves ask for a loop — one arm puts `a` after `b` while the other puts `b`
after `a` — which is refused rather than merged, because a row on a cycle is a
row the output walk never reaches and it would come out of the merge missing.

`abt_md` has a second trap in the same situation, because checking one file is
weaker than the gate.  `bin/normalize`'s `quickreadme` step runs `abt_md`
tree-wide in write mode, and that pass normalizes markdown a narrowed
`abt_md <path> -check` leaves alone — a heading with no blank line above it,
for one.  A rebase produces exactly that shape, because resolving a conflict
by deleting the `=======` marker removes the line that had been separating the
previous section from the next heading: the file reads correctly to a human,
passes the narrowed check, and then fails `quickreadme` on a one-line
whitespace diff.  Run `abt_md -evalcmd:N` and commit what it rewrites before
pushing a doc edit that came out of a conflict resolution.

A narrowed `-check` no longer pretends to check links at all -- it refuses:

```ssim
abt_md.narrow_check  nselect:16  nreadmefile:1284  comment:"-check reads every readme; drop -readmefile and -ns, or use -update to regenerate a selection"
```

A link leaves the document it is written in, so the link check reads every readme
at once and a selection could only skip it.  Run `abt_md -check` with no selection
to check links, and `abt_md -ns:<ns>` to regenerate one namespace's documents,
which checks no link and is not asked to.

`abt_md -check` resolves a link against `dev.gitfile`, so a target that exists on
disk but is not a file the repo tracks -- a build artifact, a gitignored path,
anything reached through a symlinked directory such as `.claude/skills/` -- is
reported as `target is not a file the repo tracks`.

Nothing checks that a document is reachable, which the site build used to do: it
rejected a page no toctree held.  `doc` reaches every document through
`dev.readmefile`, so an unreferenced one is still findable, and an unreachable
document is no longer an error anyone is told about.

A conflict in an ssimfile that a tool generates is resolved by regenerating the
file, never by merging it.  Consider a branch that adds a derived column to
`x2db.x2node` while master adds a topology.  `acr` aligns the columns of every
file it writes, so the new column reformats all of the rows, and master's new
topology inserts rows of its own; git reports the whole file as a single
conflict whose two arms differ on every line.  That reads like a far larger
divergence than the one column and one row set that actually changed.  The
insight is that the row set is not the authority — the `x2db.topo` specs are,
and they merged cleanly — so the file has no content of its own to lose.  Take
either arm, then run `x2topo -topo:% -rebuild -write`, which rewrites the file
from every spec including the one master just added.  A tree-wide rebuild is
safe to reach for: over all topologies it changes only what the specs derive,
leaving proc, core, shm, grp, gwport, storage, nodeintf and mcgrp untouched.
The tell that a file is in this class is that both arms carry the same keys.

The same "ours drops your rows" trap has a worse form in hand-written C++,
because there the compiler can stay silent about it.  Consider a branch that
added two functions to `lib_x2net` and one line elsewhere in the same file that
sets the pointer they read.  Master meanwhile refactored that file, so both arms
differ heavily and taking master's arm is the tempting resolution.  It drops all
three additions.  The two functions have callers in other targets, so the build
names them at once — but the assignment has no caller: the pointer keeps its
generated `NULL` initializer, the branch's own reader compiles and runs, and it
silently takes its fallback path forever.  Here that meant every gapfill fill
addressed to the wrong receive role, which the build, `amc`, `acr -check` and
`abt_md` all passed and only `x2test:refill` caught.

So a resolution is not finished when the tree builds.  Check it against the
branch's own pre-rebase delta, which is still reachable by sha in the reflog:

```bash
git merge-base <old-tip> origin/master              # the branch's original base
git diff <base> <old-tip> -- cpp/x2rxn/foo.cpp      # every line the branch added
```

Read the added lines and confirm each is either present in the rebased tree or
deliberately superseded by something master now does instead.  The distinction
matters and only reading tells them apart: a signature master replaced with a
better mechanism is superseded, while an assignment nobody calls is lost.  Do
this for every hand-written file resolved with `--ours`, and note that
`git checkout --ours <path>` writes the file without marking it resolved, so
`git diff --name-only --diff-filter=U` still lists it — read that list rather
than the rebase's own conflict banner, which scrolls.

Run `abt_md -check` bare, never narrowed and never piped.  Narrowing does select
the file — the positional argument is a regx over readme names and a readme is
named by its path, so `abt_md txt/openacr/recipes.md -check` matches — but it then
prints `abt_md: disable link checking, not all files being loaded` and exits 0
whatever the links do, because a link is validated against the anchors of the
files loaded alongside it.  A broken link therefore reads as a clean bill of
health under every narrowed form, and only the bare tree-wide run names it, with
`target:` and `comment:"target file doesn't exist"`.  Piping hides what is left:
`abt_md -check | tail` reports the exit status of `tail`, so a failing check
reads as a passing one whenever its complaint scrolled past the last few lines —
and the complaint it hides is often that the binary itself is stale, which a
rebase across a schema change produces routinely.

#### A rebase leaves an amc count constant stale when both sides added the same number of rows
<a href="#a-rebase-leaves-an-amc-count-constant-stale-when-both-sides-added-the-same-number-of-rows"></a>

`bin/normalize` fails with `atf_ci.modified_files  during:pbapi_gen
files:include/gen/command_gen.h`, and the diff the citest prints is one line:

```bash
-enum { command_FieldIdEnum_N = 915 };
+enum { command_FieldIdEnum_N = 917 };
```

The citest name is a red herring — `pbapi_gen` runs `pbapi -write` per registered
proto, which re-runs the generator, so it is merely the first citest in the job
that regenerates this header.  Plain `amc` produces the same one-line diff and
reports `n_filemod:1`.

The cause is git's own text merge of a checked-in generated file.  A count
constant such as `command_FieldIdEnum_N` lives hundreds of lines below the list
it counts, so a rebase treats the two as unrelated regions.  Here the branch had
added the options `timespeed` and `mingap` while master had added `access_key`
and `slot`.  Each side added two entries at a different offset in the enum, so
those merged additively to 917.  Both sides had also rewritten the count line
from `913` to `915` — the *same* text on both sides — so the three-way merge saw
no conflict, took the shared value, and left the file claiming 915 over 917
entries.

Nothing local objects.  The tree compiles, the enum entries are all present and
correctly valued, `acr -check % -x` is clean, and only running the generator
surfaces it.  The stale constant is what indexes the generated tables, so
believing it is a real defect and not a cosmetic one.

Confirm the mechanism before treating it as this case, because the neighbouring
recipe above describes a different rebase artifact with an overlapping symptom.
Count the list against the constant on each ref:

```bash
f=include/gen/command_gen.h
for ref in origin/master HEAD; do
  n=$(git show $ref:$f | grep -m1 command_FieldIdEnum_N)
  c=$(git show $ref:$f | sed -n '/^enum command_FieldIdEnum {/,/^};/p' | grep -c command_FieldId_)
  echo "$ref  $n  entries:$c"
done
```

A ref whose count disagrees with its entries is the stale one.  The repair is to
run `amc` and fold the result into the commit, which is the standing rule for
`atf_ci.modified_files` and is correct here: the regeneration reflects the
branch's own two options.

Two things follow.  `atf_ci` refuses a dirty tree with `atf_ci.dirty_tree`, so
the regenerated file has to be committed before `bin/normalize` will run at all —
the first run after the fix otherwise exits immediately having tested nothing.
And this is what the pre-push sequence in the `commit` skill is defending
against: fetch, rebase, `ai`, then `bin/normalize` in full, because a rebase that
reported no conflict can still leave a generated file internally inconsistent.

#### A rebase moves a field row, and only a generated doc shows it
<a href="#a-rebase-moves-a-field-row-and-only-a-generated-doc-shows-it"></a>

A branch renamed one cmdline option and gave the freed name to a new one,
declared elsewhere in the block.  After rebasing onto master, everything built
and the only symptom was a `(resource)` option printed outside the resource
listing in three regenerated docs.

`ssimfile:dmmeta.field` is ordered by declaration rather than sorted, and that
order is what the help text and the generated docs follow.  The `acr_dm` merge
driver matches a row by its key and keeps the *incoming* side's position for it,
so a row whose key existed on master lands where master had it, not where the
branch declared it.

Move the row back to its authored position by editing the file, and re-run `amc`:
when the generated C++ comes out byte-identical to what the branch committed, the
authored order was the right one.  `acr -insert` is not the tool for this — it
sorts, and this file is not sorted.

Extend that byte-identical test to the whole generated tree, because one file
agreeing proves only one row.  Every generated file upstream did not touch must
regenerate to exactly its pre-rebase blob, and `git rev-parse <old-tip>:<path>`
hands you that blob without a checkout.  Twenty of twenty matching is what says
the authored order was restored everywhere rather than in the block you looked at.

The same reversion also happens with no generated doc showing it, and that case is
harder because the doc goes the other way.  A branch that moves a field row also
regenerates the readmes following that order, so those readme edits sit in its own
diff.  The rebase reverts the row and keeps the branch's readme, and `quickreadme`
then rewrites the readme back to master's ordering — the one artifact that could
have named the loss absorbs it instead.  It reads as ordinary post-rebase drift,
and the standing rule of committing whatever a citest regenerated is exactly what
bakes the reversion in.

So measure the branch's delta against itself, which the reflog still reaches:

```bash
old=<pre-rebase tip>
base=$(git merge-base $old origin/master)
git diff $base $old -- data/dmmeta/field.ssim | wc -l          # intended
git diff origin/master HEAD -- data/dmmeta/field.ssim | wc -l   # actual
```

An actual delta much shorter than the intended one means rows the branch moved
have come back.  Measured on a lease branch it was 59 lines against 268, thirteen
`command.<tool>` blocks whose `invdir` row had returned to where master keeps it,
while `acr -check % -x`, the full build and the branch's own comptest all passed.
Row order inside a ctype block is help-text order, and no gate tests it.

#### amc.varlen_last after a rebase, and the cascade that hides it
<a href="#amc-varlen_last-after-a-rebase-and-the-cascade-that-hides-it"></a>

```ssim
amc.varlen_last  field:<ns>.<Msg>.signature  varlen:<ns>.<Msg>.clustername  comment:"fixed field follows a varlen field; varlen fields must be last"
amc.no_output  comment:"no files were modified"
algo_lib.exec  cmd:"bin/amc  -query:'' -report:N"  comment:"exit code 2"
```

The recipe above ends by saying row order inside a ctype block is help-text order
and no gate tests it.  For a message ctype that is not so.  The order is the wire
layout, a varlen field has to come last, and `amc` refuses the whole tree when one
does not — so the same silent reversion that costs an ordinary branch its help-text
order costs this one every generated file, since amc exits 2 and writes nothing.

The cascade is what makes it hard to read.  Five citests depend on output amc never
produced, so `quickreadme`, `pbapi_gen`, `readme`, `indent_srcfile` and `fast_gen`
all fail behind it, and `fast_gen`'s report is both the loudest and the most
misleading: 490 pure deletions of `emdi12`, `fast` and `fasttest` rows out of
`ctypelen.ssim`, `msg.ssim` and `msgfield.ssim`, with no additions anywhere.  That
is the signature of a missing build, in three namespaces the branch never mentions,
and looking for one there finds nothing — `bin/fast` was built and working the whole
time.  Eight failures, one cause, and the cause names the only namespace whose
report was a single quiet line.

So read the `varlen_last` line and go to `ssimfile:dmmeta.field`.  Master had moved
the varlen field last in one message ctype and moved two signature fields ahead of
an id in another; the branch carried the older order for both ctypes, and the rebase
kept the branch's while reporting no conflict at all.  Take master's order verbatim
for any ctype the branch has no business reordering.  `amc` then answering
`n_filemod:0` is the confirmation: it says the generated headers had merged correctly
all along and only the ssim source order was stale, so nothing outside `field.ssim`
needed touching.

Find every such reversion at once by diffing against master and dropping the rows
that are legitimately yours:

```bash
git diff origin/master HEAD -- data/dmmeta/field.ssim | grep '^[+-]dmmeta' \
  | grep -v 'field:awsdb\.\|field:x2aws\.'      # your branch's own namespaces
```

A permutation shows up as a matched `+`/`-` pair carrying identical text, which is
the tell: a row that is added and removed with nothing changed but its position was
moved by the merge and not by you.  Anything left after the filter is a row to put
back.

### Documentation is generated too
<a href="#documentation-is-generated-too"></a>

`abt_md` is to `txt/` what `amc` is to `cpp/gen`, and it has its own set of
things that read as success.

A generated section is one whose heading matches a pattern in `acr mdsection`.
`abt_md <mdfile regx>` regenerates it; `abt_md -ns:<ns regx>` refreshes a whole
namespace.  An inline command appears as `inline-command: ...` inside a
preformatted block, and its output replaces the rest of that block, which is
why `abt_md` with the commands on is schema-mutating: the tutorials under
`txt/tut` run `acr_ed -create ... -write` to produce their output.  Two
concurrent runs blank each other's blocks, so never start one beside a comptest
sweep or a normalize.  `abt_md -evalcmd:N` skips the evaluation.

**An option is explained in its own section, not in the prose above it.**  A tool
README carries a `####` heading per command-line option, and that heading is where a
reader looking up one flag lands.  The explanation goes under it: what the flag does,
what it is for, what turning it the other way asks instead.  The prose sections above
may name a flag while describing what the tool is for -- that is how a reader learns
the flag exists -- but the depth belongs in the flag's own section, where it is found
by anyone who arrives knowing only the flag's name.

**`abt_md -check` never regenerates.**  It validates links and exits
zero over a section that has drifted from its ssim rows, so the drift fails
`quickreadme` under `bin/normalize` instead.  Run plain `bin/abt_md` first,
then `-check`.

**Plain `abt_md` does not refresh every generated section.**  The cmdline field
tables under `txt/protocol/command/README.md`, and the `Syntax` block of each
`txt/exe/<tool>/README.md`, are regenerated by the `quickreadme` citest.  So a
`dmmeta.field` row you added still reads as the old value after both `abt_md`
and `abt_md -check` pass.  This bites hardest after a rebase whose conflicts
were resolved by taking the upstream side of those files.

**Run `abt_md` twice after inserting a command-line field.**  The first run
emits the new `#### -<flag>` block without the blank line every other block
ends with, and the second run adds it.  A single run leaves a diff that the
next person's `abt_md` produces and `checkclean` rejects.

**An anchor under a heading is generated.**  Write the heading; `abt_md` writes
the `<a href="#...">` line beneath it.

**A rebase conflict in a generated section is never resolved by hand.**  Take
the upstream side of every conflicting file, finish the rebase, then run
`abt_md` and commit what it regenerates.  Merging the two sides by eye means
reproducing the generator's output by hand, which is slower and wrong.

**`abt_md` refreshes the sections a file already carries and never adds one.**
So a README's prose and its `Description` are somebody's writing, and deleting
the file does not bring them back -- there is no wholly derived document left in
`txt/` for that trick to work on.  `Functions` has a second way of going
missing, because it forks `src_func`: `-evalcmd:N` leaves whatever function list
the file already held.

Prose in `txt/` is imperative and present tense: "update key; write file", not
"updates key, writes file".  A new file is created with `acr_ed -create
-srcfile txt/.../xyz.md` and removed with `acr_ed -del -srcfile ...`, because
those are the commands that keep `dev.gitfile` and `dev.readmefile` in step
with the tree.

#### Renaming a command option across the tree
<a href="#renaming-a-command-option-across-the-tree"></a>

An option's name is a short string that other tools spell the same way.  A
tree-wide substitution of `-set:` while renaming `acl -set` to `-addperm`
rewrote gli's own `-set:` in `cpp/gli/`, eleven examples in
`txt/exe/gli/README.md`, and a phrase containing "-set:" in the recipes
— none of which the rename was about.  The tell is that a rename is a rename
of one command's option, while a substitution matches text.

Two checks catch it, and neither alone is enough.  `grep` the new name back and
read every hit that does not belong to the command being renamed.  Then run
`abt_md -check`: it evaluates the command lines the docs show through
`acr_compl`, so an option spliced into the wrong tool prints as
`acr_compl.check error:"unknown option" value:<newname> command:<tool>` with
the file and line.

Beware the exit code when running it in a pipeline — `abt_md -check | tail`
reports `tail`'s status, so a run that printed those errors still reads as
`0`.  Redirect to a file and check the status of `abt_md` itself.

`abt_md -check` reads markdown, so it is blind to a command line assembled as
a string in code.  Renaming `userproc -exec` to `-cmd` left
`ts/x2ui/src/db/tty.ts` composing the retired spelling into the line that spawns
the pty bridge, and the x2ui tests mock `RunCommand` rather than inspecting
the line, so nothing failed: the Console terminal and the bench launcher were
broken features on an otherwise green tree.  A doc that keeps the old spelling
is cosmetic, and code that keeps it is not.

The scope that misses it is the natural one.  A sweep over `cpp/ txt/ test/
data/` covers where command options are declared and documented, and that is
the scope the `-exec` rename used; `ts/` holds no ssim and no handler, so there
is no obvious reason to include it.  The reason is that the SPA composes x2cli
command lines as strings and sends them over the gateway, which makes `ts/` a
caller of the option catalog exactly as `cpp/` is.  Sweep it too.

So the sweep to run is for the *old* name, over code as well as docs, with
`*/gen/*` and the gitignored `wt/` excluded.  What survives is the set of
sites the rename missed:

```bash
grep -rInE -- '-oldname' --include='*.cpp' --include='*.h' --include='*.ts' \
    --include='*.tsx' --include='*.md' --include='*.ssim' . \
  | grep -vE '/gen/|/node_modules/|^\./wt/'
```

Read every survivor instead of substituting over it.  A hit under `cpp/gen/`
is regenerated from ssim and needs no edit, and a hit in another namespace
belongs to another tool — `command.atf_cmdline.exec` is the standing example,
that tool's own `-exec` flag, which must keep its name while the x2cmd one
changes.

### The runtime a generated process runs
<a href="#the-runtime-a-generated-process-runs"></a>

Every openacr executable is a single-threaded cooperative loop over steps.  A
step is a function bound to a list, a heap or a timer, and the loop calls it
when there is something to do.

**A step's collection is non-empty when it is called.**  Inside
`ns::<field>_Step()` the first-element accesses on a `zd_` or `bh_` field need
no NULL check.

**A step that cannot act removes the element.**  Leaving an element whose
condition will never resolve turns the step into a hot poll.  Take it off the
list so the list drains, and say in one line why the removal is safe, because
the reasoning is rarely obvious from the code.

**A recurring step with no delay stops the loop from ever sleeping.**
`MainLoop` repeats while `next_loop < limit`, and the generated `Call` for an
`InlineRecur` step pins `next_loop` to `clock + <field>_delay`.  A delay left
at its zero default pins `next_loop` to the current clock on every pass, the
loop's condition never goes false, and the process spins at a full core with
nothing to report: the program is correct, only hot.  So a recurring step sets
its cadence with `<field>_SetDelay(...)`, and its enable flag defaults to false
when it has nothing to do until some setup runs -- the pin happens whenever the
flag is on, whether or not the body does any work.

The cost lands on whoever links the code, which makes this sharper in a library
than in an executable.  A spinning executable wastes one core; the same step in
a library, enabled by default, spins every client that links it.  What surfaces
is not a CPU report but an unrelated test failing, because its publisher can no
longer keep up with its own budget.  A test that fails only when run alongside
others, and passes alone, is the signature.

**A server exits naturally or not at all.**  `algo_lib::MainLoop` ends when the
next scheduling cycle is at infinity: no FIohook registered, no non-empty fstep
list, no scheduled timehook.  A server that reaches for
`algo_lib::ReqExitMainLoop()` is papering over a state bug; the shutdown is
performed by draining whatever is keeping the loop alive.

| what holds the loop open | how it is drained |
|---|---|
| inbound shm | set `c_shmhdr->eof`, or `cd_poll_read_Remove(shm)` when the peer signalled end-of-stream |
| outbound shm | nothing -- a write-only shm is not polled and holds nothing open |
| signaled mode | `lib_ams::SetSignaledMode(false)`, which removes the signalfd FIohook |
| stdin | `fdin_RemoveAll()` on EOF, and drop the stdio-mode loopback shm if one is in `cd_poll_read` |
| forked children | reap in a recurring timehook, and `bh_timehook_Remove(th)` once `waitpid` returns ECHILD |
| an FCmd's in-fbuf | in-fbuf EOF fires `cd_cmd_eof_Step`; remove the FCmd and any shm reads left without a producer |

Signaled mode is the one worth reading twice.  `lib_ams::SetSignaledMode(true)`
arms a signalfd and registers it as an FIohook so a peer's SIGRTMIN wakes the
loop, and that hook stays registered as long as the process stays in signaled
mode.  A registered FIohook is one of the three things natural exit requires
the absence of, so a process in signaled mode has no natural exit at all: it
runs to `algo_lib::_db.limit` whatever else it has drained.  `lib_ams::Uninit`
leaves signaled mode as its first act, but `Uninit` runs after `MainLoop`, so a
server that must exit on its own leaves signaled mode as part of the drain.

Install the SIGCHLD handler and schedule the reaping timehook **before**
forking, or a fast-exiting child races the default ignore-handler.  The handler
itself does nothing but `algo_lib::ThScheduleIn(th, 0)` to wake the loop.

`ReqExit` is fine in a fixed-task CLI tool, where the work is bounded and
exiting is the normal terminus.

**A subprocess is invoked through its generated `command::<target>_proc`.**
`<target>_Exec` returns the exit code and `<target>_ExecX` throws
`algo_lib::ErrorX` on a non-zero one.  Setting `_fstdout="|"` makes `_Start`
create a pipe and expose the read end as `_from_stdout`, which `_Wait` closes.

**State lives on `FDb`.**  A file-scope `static` is invisible to `acr`, does
not travel with copy and print, and takes part in no xref.  The codestyle skill
states this rule and its consequences in full.

### Building and testing
<a href="#building-and-testing"></a>

`abt` takes one regex and builds what it matches; `ai` bootstraps `abt` first
and then builds everything.  The rules below are the ones that turn a build or
a test run into a wrong answer rather than an error.

**Start with `ai` after pulling work that changed the build tools.**  Suppose
`abt <target>` compiles nothing, prints `abt can't find build/gitinfo.h`, and
dies.  Neither `PATH` nor the checkout is at fault.  The library every
executable links includes a build-identity header that `abt` writes at the
start of a run, and the `abt` in `build/` was compiled before that header
existed, so its dependency scan demands a file it does not know how to write.
A tool cannot bootstrap a feature it predates.  `ai` plants an empty
`build/gitinfo.h`, the build succeeds unversioned, and the fresh `abt` it
installs writes the real stamp on the next run.

**A rebase leaves the built tools a version behind, and each of the two
failures is mistaken for something else.**  The silent one comes from `amc`.
Some of `amc`'s inputs are also its outputs -- `ssimfile:dmmeta.ctypelen`
records every ctype's computed length, `ssimfile:dmmeta.dispsig` records every
dispatch's signature -- and a generator built before a new kind of row was
declared does not know to emit it, so running it *deletes* that row along with
the generated line that loaded it at boot.  The run exits zero and `acr -check
% -x` passes, because a missing row is not an inconsistency but a smaller
database.  The only tell is the diff: a generated row that disappears right
after a rebase means the generator is older than the schema.  Rebuild and
regenerate, and never accept the deletion as the new truth.

The loud one comes from `abt`, which scans sources for the headers they
include.  A master commit can add an include of a header `abt` itself writes
into `build/`, and an `abt` predating that stamp cannot resolve it, so it fails
and segfaults -- `cpp/lib/algo/arg.cpp:36: abt can't find build/gitinfo.h`.
That reads like a broken checkout and is a stale binary.  The order after a
rebase that moved the toolchain is `ai`, then `amc`, then
`abt % -build -install`.

**One build or documentation tool at a time per checkout.**  A generated
`txt/ssimdb/` page once reached a commit with an `abt.config builddir:...` line
spliced into the middle of a C++ prototype, the rest of the prototype gone.
Nothing emits that line but a running `abt` and nothing writes that page but a
readme regeneration, so the two ran at once in one checkout and one tool's
output landed inside the other's capture.  The damage is silent: the
regeneration reports success, the corrupt page is committed, and the next
regeneration is what finally reports the file as modified.  Concurrent work
goes in its own worktree -- see [/txt/exe/wt/README.md](/txt/exe/wt/README.md).

**Edit `amc` in a sandbox, not in the live tree.**  A broken generator pollutes
`cpp/gen` and the next build cannot bootstrap out of it.  Regenerate inside a
fresh copy with `wt amc -reset -- amc`, compile it there with `wt amc -- ai`,
tighten the generator until the sandbox build is clean, and only then run `amc
&& ai` in the live tree.

**A name-derivation change is not covered by the generated tree's diff.**  Some
ctypes live only inside a comptest: `amc.PoolInsertScale` hands `amc` a
synthetic universe and diffs the generator's stdout against a stored baseline,
and none of those ctypes appears in `data/`, so none contributes a line to
`cpp/gen`.  A rule that renames them leaves the committed generated tree
byte-identical.  Such a change can therefore reach `n_filemod:0`, build, pass
`atf_unit`, `atf_amc` and `bin/normalize` in full, and still fail the `comp`
and `coverage` jobs on a golden diff -- `bin/normalize` does not run
`atf_comp`.  Run `atf_comp -comptest:'amc.%'` before pushing.

**A comptest diff is read, not blessed.**  `atf_comp` prints a coloured diff
between `temp/atf_comp/<name>` and `test/atf_comp/<name>`.  A diff consistent
across reruns is a regression and the code is what changes.  A diff that varies
between runs, or names a timestamp, port, pid or generated id, is
non-determinism, and it is fixed by adding the field to
`ssimfile:atfdb.unstableattr` or by writing a `dmmeta.tfilt` rule -- never by
recapturing.

Masking reaches ssim tuple output only: a line is parsed as a tuple and the
matching attrs are replaced with `***`.  A human table is not a tuple, so no
rule can mask a column of one, and the volatile value has to be kept out of the
assertion instead -- often by adding `-ssim` to the command, which turns the
same data into something the rules can reach.  Scope an `unstableattr` entry to
one tuple head (`report.abt.time`) rather than to all of them (`%.time`): the
wildcard reaches every comptest in the tree and can turn another baseline's
deliberate assertion into `***` with nobody noticing.

**`-mode:print` does not run the test.**  It prints
`test/atf_comp/<name>` as it stands on disk, so its output matches that file by
construction whatever the code does.  "A fresh `-mode:print` capture matches
the committed baseline" therefore says nothing about whether the test passes,
and it is not evidence that a failure was a flake.  Plain `atf_comp
<ns>.<name>` is the question; it exits non-zero and prints the diff.

**The command a comptest runs is in `cpp/atf_comp/<ns>.cpp`, not in the
reference.**  The reference under `test/atf_comp/<name>` opens with a
`# start bash cmd:` line that reads like the script, and it is captured output
like every line beneath it.  Editing it changes what the test expects and not
what it runs, so the run fails on line one and the next `-capture` writes the
real command back over the edit.  Change the `ProcStart` call in the source,
rebuild `atf_comp`, then recapture.

**A test shortened for speed no longer covers what it was written for.**  A
four-minute test is narrowed -- sixteen streams to one, three nodes to one,
five repetitions to one -- wall time falls, the suite stays green, and the diff
reads as a pure speedup.  What left with the wall time is the breadth: a
publish test that fanned out across three nodes exercised the path where two
partitions accept writes at once, and the one-node version cannot reach the
defect class it was built to catch.  The loss is invisible because a test
reports only on what it ran, and `npass` goes up by one either way -- the same
evidence the suite would give if the test had been deleted.

So name the failure the test is supposed to turn red on, and keep the dimension
that carries it.  Cut the dimensions that only cost time: a warmup, a message
count, a settle wait budgeted generously.  Where the expensive dimension *is*
the distinguishing one, split the test rather than shrink it.  Then confirm the
shortened test can still fail, by reintroducing the defect it was written for
and watching it go red.

**The first `bin/normalize` in a fresh worktree can time out at the `readme`
gate.**  Every gate before it reports `success:Y`, and running that one gate
again immediately afterwards passes in about four minutes, with nothing in the
branch changed between the two runs.  The gate regenerates the markdown under
`txt/`, and the tutorials hold inline commands that create a target, generate
its code, and compile and link it, inside the worktree's own copy-on-write
`abt_md` sandbox.  A fresh sandbox holds no object files, so the first run
compiles the whole dependency tree before the tutorial produces any output, and
that exceeds the 600-second budget `acr citest:readme` records.

Two things follow.  The timeout is a property of the sandbox's state rather
than of the branch, so it happens on `master` as readily as anywhere else.  And
a gate that exceeds its budget ends the job, leaving every later gate unrun:
the run exits 124 and its verdict is incomplete, so it cannot be read as one
gate failing and the rest passing.  Run `bin/normalize` again.  To tell this
apart from a gate that is genuinely stuck, watch the sandbox's build directory
-- a cold first run fills it steadily and a stuck gate does not.

**The `apm_gen` diff is accepted, not reviewed.**  The gate regenerates
`apm/gen/<package>.ssim` for every `dev.pkggen` entry, and that file restates
the package's whole ssim schema in one place, so any branch that adds a field
or gives a command a new flag leaves it out of date.  The diff carries nothing
the branch's own ssim diff does not, and reading it is the same review twice.
Run `apm -package:<pkg> -generate`, `git add`, amend.  Do not re-run
`bin/normalize` afterwards: the regeneration is confined to `apm/gen`, no other
gate reads that directory, and the gates that already reported `success:Y` in
that run still hold.

**A missing `node_modules` is a missing build.**  `abt_ts` is to `ts/` what
`ai` and `abt` are to `cpp/`, and it is run on demand rather than once per
machine.  `abt_ts -normalize` installs and typechecks, `-build` implies that
and then bundles, `-clean` drops the caches.  A test that shells out to a
package-local binary fails with `No such file or directory` and `exit ... code:127`,
which is the build step having been skipped and never an environment fault.

**macOS keeps its adaptation layer in one file.**  A call that Linux provides and
macOS does not needs a function with a Darwin body, and those bodies belong in
`cpp/lib/algo/macos.cpp`, inside one `#if defined(__MACH__)` spanning the whole
file.  `dev.targsrc` names a target and a source with no uname between them, so
every source in a target compiles on every platform, and the file-wide `#if` is
what leaves this one empty on Linux.  A conditional that picks between two bodies
of the same function stays where it is, and so does one in a header, since
neither is a function that is missing.

### Gates that pass without checking anything
<a href="#gates-that-pass-without-checking-anything"></a>

#### A citest that reports success in eleven milliseconds
<a href="#a-citest-that-reports-success-in-eleven-milliseconds"></a>

A citest row carries a comment saying what it checks and a timeout saying how
long that may take, and the job log reports it green:

```ssim
atf_ci.citest  citest:<name>  runtime:00:00:00.011441083  success:Y  comment:"...checks every link"
```

Eleven milliseconds against a six-hundred-second timeout is the whole finding.
A handler whose body sits inside `#if 0` builds nothing and returns, and a
citest that does no work cannot fail; one that cannot fail reports the same
`success:Y` as one that ran, so the row in the table, the comment on the
handler and the line in the job log all describe a gate that is not there.  One
such gate stood for long enough that the thing behind it accumulated 144
warnings.

The runtime column is what separates the two cases, and it is the only thing
in the job log that does.  When a citest is meant to build, run a cluster or
walk the tree, a sub-second runtime is the symptom — scan the column before
trusting a green `comp` job, and grep the handler for `#if 0` when a number
looks too small for the work its comment describes:

```bash
grep -n "#if 0" cpp/atf_ci/*.cpp
```

The general shape is the one AGENTS.md states for greps: a check that finds
nothing has not passed until you know it can fail.  A disabled test is that
rule's worst case, because the harness reports it as a pass rather than as an
empty result, and nobody re-reads a green line.

#### A set difference computed with grep -v fails open
<a href="#a-set-difference-computed-with-grep-v-fails-open"></a>

A containment check reported that every line of one file was present in
another, thirty times in one run, and it was wrong every time.  The check was
`grep -Fxv -f <pattern file> <input>` — print the lines of the input that are
absent from the pattern file — over a pattern file of tens of thousands of
lines.  It printed nothing, and nothing is exactly what full containment looks
like.

`grep` in this repo's shell is not the system tool.  It is a shell function
that routes to `ugrep`, and `ugrep` refuses a pattern set past a size it
considers too complex: it writes `ugrep: error: ... exceeds complexity limits`
to stderr and then matches nothing at all.  A `-v` invocation that matches
nothing prints nothing, so the error path and the clean path produce the same
visible result — an empty stdout and a zero exit status.  The distinction
lives only on stderr, which a check written as one stage of a larger pipeline
routinely discards.

The general shape is worth carrying away past this one command.  A set
difference computed by negated matching fails open, because "no output" is
simultaneously the answer "the sets are contained" and the answer "the matcher
gave up", and no caller can tell the two apart.

Use `comm -23` over sorted input instead:

```bash
LC_ALL=C comm -23 <(LC_ALL=C sort -u <input>) <(LC_ALL=C sort -u <patterns>)
```

`comm` has no complexity ceiling, and it fails loudly rather than silently — on
unsorted input it prints `comm: file N is not in sorted order` and exits
nonzero, so a mistake in the pipeline announces itself instead of being
reported as a clean result.  The `LC_ALL=C` is not optional: `comm` compares
against the collation its input was sorted under, so the sort and the
comparison have to be pinned to the same one.

#### abt exits 144 and prints only its config line
<a href="#abt-exits-144-and-prints-only-its-config-line"></a>

A build that should take a minute returns at once, having printed one line:

```ssim
abt.config  builddir:Linux-g++.release-x86_64  ood_src:1  ood_target:15  cache:gcache
```

There is no `abt.exec` line, no compiler diagnostic, and no `report.abt`.  The
exit status is 144, which no message anywhere explains.

The cause is a stale `build/<cfg>/abt.lock`.  The file holds one number, the pid
of the abt that took it, and an abt killed before it could release the lock
leaves the file behind naming a pid that no longer exists.  Read the owner and
ask whether it is alive:

```bash
cat build/release/abt.lock
ps -p $(cat build/release/abt.lock) -o pid,cmd
```

An empty `ps` answer means the lock is abandoned, and `rm -f
build/release/abt.lock` lets the next build run.  Do this only when the owner is
genuinely gone -- another worktree's build is a different lock file, but a second
abt in *this* build directory is a live owner and removing its lock corrupts both
runs.

The way the lock is orphaned in the first place is worth knowing, because it
looks like a different failure.  `ai` at its default parallelism starts one
compiler per core, and 128 concurrent g++ processes exhaust memory on a box that
has plenty for a normal build.  The kernel kills them, abt reports each as
`status 9`, then `status 15` for the ones it terminates itself, and the run ends
with a large `n_err` and no compiler error text:

```ssim
report.abt  n_target:148  time:00:01:41  hitrate:0%  pch_hitrate:98%  n_warn:0  n_err:88  n_install:144
```

`n_err` in the dozens with no diagnostic under it is memory, not code.  Cap the
parallelism -- `abt % -build -install -maxjobs:24` -- and the same tree builds
clean.

#### A normalize pass that times out leaves the tree dirty
<a href="#a-normalize-pass-that-times-out-leaves-the-tree-dirty"></a>

`bin/normalize` regenerates files as it goes, so a pass killed by a runtime cap
leaves its partial output in the working tree instead of rolling it back.  The
next pass then tests nothing: its `checkclean` step refuses to run on a dirty
tree, and the run exits at once with `atf_ci.dirty_tree` naming the leftovers
rather than anything about the branch.  Read those modified files as the
previous pass's product — most often a regenerated table or an indent fix,
which is the fix already written for you — commit or amend them, and only then
rerun.  The builds inside are incremental, so a rerun from a clean tree resumes
where the capped pass stopped.

One of those leftovers can be a partial rewrite rather than a finished one, so
"the fix already written for you" needs checking before it is committed.  A pass
killed during `bin/normalize` left two `fast.FDb.fs_%` rows of
`ssimfile:dmmeta.field` reordered against master, which reads exactly like
ordinary sort drift and would have been committed as such; a later pass that ran
to completion did not touch the file at all, and the reorder was the interrupted
write and nothing more.  So revert the leftovers you cannot attribute to a citest
that finished, and let a complete pass say what the tree actually owes.  What the
capped pass produced is a hypothesis; only a pass that reached the end of that
citest is evidence.

#### Leaks a leak check does not report (pool memory)
<a href="#leaks-a-leak-check-does-not-report-pool-memory-"></a>

`Lpool` and `Tpool` mark each block they hand out, so a row allocated from an
amc pool and never deleted is reported by `valgrind --leak-check=full` with the
stack that allocated it.  Without the marks the checker sees only the 2MB
mapping the lpool took from `algo_lib.FDb.sbrk`, and nothing about the rows
carved out of it.

The marks live in `cfg:memcheck` only, because a valgrind client request costs
its instructions whether or not a checker is attached.  A memcheck run against
any other configuration therefore reports invalid reads and writes and reports
no leak inside a pool -- and a leak report that names nothing reads exactly like
a clean run.  `atf_comp` prints `atf_comp.memcheck_cfg` when it notices, and the
fix is `-cfg:memcheck`:

```bash
atf_ci -cijob:memcheck                                    # mem_prep builds it, then runs
abt % -cfg:memcheck -build                                # by hand: build it
atf_comp -mode:memcheck -cfg:memcheck <comptest regx>     # then drive it
grep -l "definitely lost" temp/atf_comp/*/*.memcheck.*.log
```

That configuration is release plus the client requests and `-g`, nothing else,
so it costs 20% under valgrind where `cfg:debug` costs 190% -- debug is
unoptimized, and 2.8x of its cost is there before valgrind is involved at all.
The numbers move accordingly: `acr ns:algo_lib` reports about 23,700 blocks in
use at exit built as memcheck against about 10,800 as release, and its stacks
name file and line.

The wrap asks for `--leak-check=full --errors-for-leak-kinds=definite`, so a
leaked record fails the comptest that leaked it.  Read `still reachable` and
`possibly lost` in a log as normal rather than as findings: a module holds its
tables until it exits, and a pool's free list is reached by interior pointers.
Only `definitely lost` counts, which is why only it is an error.

Valgrind traces children, so leaks inside `bash`, `sed` and `cp` would land
under the name of whichever comptest ran them.  `conf/memcheck.supp` drops
those, scoped to leaks so an invalid read or write in one of them still
reports.

`Blkpool` is the exception worth knowing.  It returns memory a buffer at a
time, so a leaked element pins its buffer and the report names the buffer's
`ReserveBuffers` stack rather than the element's own.

### Glossary
<a href="#glossary"></a>

A noun here names a key -- a value identifying one element of a set -- and the
same word names the whole set, read as the query `noun:%`.  Plurals are never
used, and an attribute of one element is a two-word `noun noun` phrase: `ctype
name`, `field reftype`, `target license`.

| term | meaning |
|---|---|
| ssim tuple | one record: a type tag, then whitespace-separated `key:value` attributes, the first of which is the primary key |
| ssimfile | a sorted text file of tuples of one type, under `data/<ns>/<name>.ssim`, and the table it holds |
| ssimdb | a namespace that defines tables only, with no in-memory pool and no build target |
| ctype | a compound type -- a record, a struct, a message -- named `<ns>.<Name>` |
| field | one attribute of a ctype, named `<ns>.<Ctype>.<name>`, and the row that declares it |
| reftype | what a field's declared type constructs: `Val`, `Pkey`, `Base`, `Thash`, `Llist`, `Ptrary`, `Upptr`, `Varlen` and the rest of `acr reftype` |
| pkey | the first field of a ctype, the value that identifies one row |
| base | a field whose reftype is `Base`: the ctype it names contributes all of its fields to this one |
| xref | an in-memory cross-reference: the row that says one record is reachable from another, and by which access path |
| via | the access path an xref takes, either a pointer field on the child or a `<hash>/<key>` pair |
| ns | a namespace, and the unit a target is built from |
| target | a thing `abt` builds: an executable, a library, an ssimdb, a protocol |
| finput | a declaration that a target loads an ssimfile into an in-memory pool at startup |
| gstatic | the same, except the rows are compiled in rather than read at startup |
| fstep | a declaration that a field's collection is driven by a step function |
| fldfunc | a field whose value is computed rather than stored: a substr, a cppfunc, a Count |
| cursor | the generated way to walk a pool, list, heap or index: `ind_beg(<curs>, var, root) ... ind_end` |
| in-memory database | the pools, indexes and cross-references `amc` generates for one namespace, rooted at its `FDb` |
| FDb | the singleton that holds every pool and every global index of a namespace |
| package | a named subset of the tree that `apm` can evaluate and publish |
| citest | one check of a cijob, driven by `atf_ci` |

**A document names a table by its short name, and `abt_md -check` resolves it.**  A span
`ssimfile:dmmeta.ctype` names that table; the qualified `dmmeta.ssimfile:dmmeta.ctype`
names the row of the catalog holding it and reads sideways in a sentence.  Both are
checked against the database, so a table that moves to another namespace stops being a
silent staleness and becomes a failing check.

An unqualified span is otherwise not read as a key, because the bare leaf form is how an
attribute appears inside a tuple: `cascdel:Y`, `cfmt:Argv` and `sandbox:Y` are values
rather than keys, and reading them as keys reports every one of them.  What admits
`ssimfile:` and refuses those is the leaf's own table -- a table keyed by a table name is
a table about tables, so a span naming one is a reference to a table.  Nothing else in the
tree is.
