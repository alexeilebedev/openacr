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

The openacr namespaces ship as a package, `openacr`, and that
package is read by people who have never seen the tree it was published from.
A name belonging to a downstream project is noise to them at best, and a
dangling reference at worst.

The rule therefore binds every document the package carries, this one
included, and every source file and test as well; ask `apm -package:openacr -l
-showfile` for the list.

A published document about an openacr tool illustrates with a table that same
package carries.  `dmmeta.ns`, `dmmeta.ctype`, `dmmeta.field`, `dev.target` and
`dev.gitfile` are the standing examples, they are small, and every reader
already has them.  `apm -package:openacr -l -showrec` answers what else the
package holds.

`apm -check` enforces it, as the `apm.keyword` finding.  It takes each package and gathers
the words the package must not contain: the `dev.pkgkeyword` words of every
package that extends or requires it, and the `forbid:Y` words of the package and
of every package that carries it.  Each package declares its own words, so the
list ships with the package it identifies and never with the one it must stay
out of.  The words are matched against every record, every file name, and every
line of every file the package publishes, so a downstream table named in an
openacr document fails `normalize` in the commit that writes it.

A record is checked whole, so a forbidden word in a `comment:`, a path regx or
any other attribute fails the package exactly as one in the key does.  A line
the destination regenerates from its own data is skipped: a copyright notice, a
header's `update-hdr` block, a README's usage block, and the fenced output of an
inline command.  A line that has to keep a word, such as an editor key sequence,
carries `ignore:pkgkeyword`.

Two consequences for a change that gives an openacr tool a new input.  Its
`dev.pkgkey` rows belong in the same commit, since a row that lands in the
openacr package by default and names the downstream tree fails the check.  And a row openacr must
keep is written without the word: the `Syntax` mdsection's path is
`(txt/exe/%/README.md|txt/protocol/%.md)` for that reason, which stays precise
because the generator gates on whether the page has a command or an executable
behind it.

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

**A structured key carries only components that vary independently.**  A key
names an element of a set, so it must carry exactly those components whose
variation reaches another element of that set.  Run the test by holding every
other component fixed and varying one: if that direction cannot reach an element
the set is allowed to contain, the component is functionally determined by the
rest, which makes it an attribute of the row.  Call this cardinality
analysis, and do it when the key is designed, because nothing downstream catches
the failure.  Too few components collide on insert and announce themselves; too
many are silent, since every extra spelling is a well-formed string that
`acr -check` accepts.

`dev.targsrc` is the standing example of getting it wrong, and it is in the tree.
Its key is `target/src`, but a source file belongs to exactly one target, so
`src` alone identifies the row -- every one of its ~1700 rows carries a distinct
source path.  The declared key is a superkey and not a candidate key: it admits
the product of ~150 targets and ~5500 tracked files, for a set that cannot
exceed the files alone.  Every extra spelling is a source assigned to two
targets.  The key should have been `src` alone, making the table a subset of
`dev.gitfile` with `target` as an attribute.  Do not copy its shape.

`dev.targdep` is the same construction done right: `target.parent`, both
components `dev.Target` keys, and neither determined by the other, since a target
has many parents and a parent serves many targets.  `dmmeta.ctype` is the other
correct shape, a parent key and a name unique within it, which is the commonest
key in the schema.

A composite key shows its arity in the column list, where a reviewer asks whether
it is minimal.  A structured key shows one column and keeps its arity in
`dmmeta.substr` one table away, so the question never gets asked on its own.
Ask it deliberately.

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

**A deadline heap's xref is `inscond:false`.**  A `Bheap` whose `fstep` is
`TimeHookOnce` fires its step when the heap head's sort field passes the clock,
and an xref with `inscond:true` puts every new row on the heap at `XrefMaybe`,
with the sort field still at its zero default.  A deadline of zero is already
past, so the step fires on the next pass for a row that was meant to wait for an
event -- a lost-fill backstop escalating the moment its conversation was created,
before the first ask had gone out.  Declare the xref `inscond:false` and let the
code that stamps the deadline be the one that inserts.

**Tpool recycles memory and has no `RemoveAll`.**  Use `Lary` when the whole
pool has to be cleared between iterations.

**An environment variable is a row of `dev.envvar`, and a credential name a row
of `dev.cred`.**  Both tables hold names and never values: `dev.envvar` carries
the variable's name and whether it is this tree's own, and `dev.cred` carries a
credential's name, the `creddb.kind` it is stored under, and the variable that
delivers it when one does.  A secret's value lives in credd, in a protected CI
variable, or in the person's own store, and in none of these rows.

Both are projected to symbols in `algo_lib`, so code spells a name once:
`getenv(algo_lib::dev_envvar_CREDD_PASSWORD)` rather than a string literal, and
`algo_lib::dev_cred_gli_mr` for a credential a tool asks credd for.  A typo is
then a compile error where it used to be a variable that silently read empty.
The symbol reaches everywhere C++ names a variable -- `getenv`, `setenv`,
`unsetenv` and `putenv`, `atf_comp::SetEnv` in a test, and the `NAME=` that
prefixes an assignment this tree writes into a file it parses back.  The one
place a literal stays is shell program text emitted for bash to run, where the
name belongs to the program being written.

A variable a packaged library reads is named for that library, not for the tree
the library happens to be used by.  `apm -check` forbids the openacr
package any record naming the tree it is published from, so a variable named
after that tree and read by `algo_lib` can neither ride in the package nor be spelled as
a symbol there -- the row is excluded and the symbol does not exist, and the
packaged library stops compiling.  The name is what is wrong in that situation,
so `ALGOFATALDIR` is what `algo_lib` reads.

So a change that introduces an environment variable adds its row to
`data/dev/envvar.ssim` in the same commit, and one that introduces a credential
name adds its row to `data/dev/cred.ssim`.  The names form one global set --
two programs that each pick `PORT` do not collide until one execs the other --
and a table is what makes the set visible and lets `acr` cross-reference it:
`acr dev.envvar` lists the variables a tree reads today.

**acr orders a file by a sort key of four parts, compared in order: the ctype,
a number, a string and an arrival counter.**  The string is the table's
`dmmeta.ssimsort` sort field evaluated on the record, and it is empty for a
table that declares no `ssimsort` row.  When that field's type is a builtin
numeric, acr reads the value into the number and clears the string, so `10`
follows `9` in such a table and precedes it in one whose sort field is a string.
The arrival counter is issued per distinct key, so rows with equal sort fields
keep the order they were read in.  Two consequences follow.  A table with no
`ssimsort` row sorts by arrival alone, so its file order is its read order and a
new row lands at the end, which reads as acr not sorting it.  And a query prints
in the ctype's rank, then this key, which is the order `bin/normalize` holds a
file to; `amc`, `acr -check % -x` and `abt_md -check` never look at it.

### Reading and editing the database
<a href="#reading-and-editing-the-database"></a>

#### A query joining ssimfile names with `|` selects nothing and exits 0
<a href="#a-query-joining-ssimfile-names-with-selects-nothing-and-exits-0"></a>

One table checks, and the two of them together check nothing:

```bash
acr -check 'dev.target'                  # report.acr_check  records:171  n_err:0
acr -check 'dev.target|dev.targdep'      # report.acr_check  records:0    n_err:0
acr -check '(dev.target|dev.targdep)'    # report.acr_check  records:0    n_err:0
```

A query names one ssimfile, and an alternation has to sit inside the namespace or
inside the file name.  Spanning the dot names no table at all, and acr reports
that as an empty selection rather than as an error, so the exit code is 0.

| could be | discriminant |
|---|---|
| the alternation spans the dot | run one of the branches alone -- a count where the joined form gave 0 is this case |
| the tables really hold nothing | `acr '<ns>.%' -in:<root>` counts the namespace, and 0 there means the root is empty |

Write the alternation inside a component:

```bash
acr -check 'dev.(target|targdep)'        # report.acr_check  records:692  n_err:0
acr -check '(dev|dmmeta).%'              # two namespaces whole
```

A script that builds its selection by pasting table names is the shape to look
for, because it passes on every tree it has ever run against.  One query per
namespace is the form that keeps the selection exact -- group the names by their
namespace, then loop, and fail the run if any query does.

#### acr.bad_dflt Default names a key that does not exist
<a href="#acr-bad_dflt-default-names-a-key-that-does-not-exist"></a>

`acr -check` reports a `dmmeta.field` row, not a data row:

```bash
data/dmmeta/field.ssim:<line>: acr.bad_dflt  field:command.atf_comp.cfg  dflt:nosuch-cfg  arg:dev.Cfg  comment:"Default names a key that does not exist"
```

The field declares `reftype:Pkey` with an `arg` naming an ssimdb ctype, so its
`dflt` is the key the tool resolves when the operator passes no flag.  That key is
a foreign key and it names no row.  Fix the default, or create the row it names:

```bash
acr cfg:%                                        # what the table actually holds
echo 'dmmeta.field  field:command.atf_comp.cfg  dflt:'"'"'"release"'"'"'' | acr -update -write
```

Read `n_file_mod:1` to know the write landed.  `acr -update -write -check` on a row
that is *currently* bad checks before it writes, so the check fails on the old value,
the write is refused, and the `acr.update` line still echoes the row you meant —
`n_file_mod:0` is the only part that tells you nothing happened.

Two defaults will not report even when they name nothing.  A default that is not a
string literal is not a key: `dflt:0` initializes a pointer and `dflt:'""'` says the
operator must supply the value.  And a target table that loaded zero rows is skipped,
because a tree can track a table as an empty file whose rows arrive from a dataset
layer, and a checkout with nothing attached cannot tell a key that was never created
from one it was not given.  Check such a default by handing acr both layers as one
stream:

```bash
(acr %; acr % -in:<dataset>) | acr -in:- -check %
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
dmmeta.field  field:report.acr.n_skip  arg:""  reftype:Val  dflt:""  ...
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
grep -n 'field:report.acr.n_skip' data/dmmeta/field.ssim
```

Repair with `acr -replace -check -write`, passing the whole corrected tuple;
`-replace` updates the row in place and reports `n_update:1`.

#### A data row carrying `'""'` holds two characters, and the program reading it never matches empty
<a href="#a-data-row-carrying-holds-two-characters-and-the-program-reading-it-never-matches-empty"></a>

`dflt:'""'` on a `dmmeta.field` row is a C++ initializer and the right spelling
there.  Copied into a *data* row it is no longer an initializer, it is a value,
and acr stores the two characters `""`.  A program that tests the field for
empty then never matches, and nothing reports anything.

acr's own quoting is what tells the two apart:

```
jsonattr:""      the empty string
jsonattr:'""'    the two-character string ""
```

`omdb.ommetric` carried the quoted form on all seventeen of its log-parsed rows.
omparse admits a metric to its token index when `jsonattr` is empty, so the index
was built with zero entries, every metric in every benchmark log was read and
discarded, and the tool exited 0 while printing a table of dashes.  Put the
literal back on a single row and only that row's column goes to `-`, which is
the discriminant when one column is empty and its neighbours are not.

Find the shape across the database.  It belongs in `dmmeta.field.dflt`, and in a
data row it is nearly always the mistake:

```bash
grep -rn ":'\"\"'" data/ | grep -v '^data/dmmeta/field.ssim'
```

`acr -insert` will not clear such a value, and it reports nothing to say so.
`-insert` ignores any tuple whose key already exists, whatever its attributes:
passing `jsonattr:""` explicitly is ignored, and so is an insert that rewrites
the comment as well.  Each reports `n_ignore:1` and `n_file_mod:0` while the
literal stays in the file.  Repair it with `-replace`, as the section above
prescribes, passing the whole corrected tuple:

```bash
echo 'omdb.ommetric  ommetric:pb_msg  parsenum:2  jsonattr:""  comment:"Pub rate msg/s"' \
  | acr -replace -write      # acr.update ... n_update:1  n_file_mod:1
```

The canonical form is `jsonattr:""` rather than an omitted attribute, because a
field whose `dflt` is `'""'` has a default that the empty string differs from.
Deleting the attribute by hand parses correctly and is undone by the next
rewrite, so let acr spell it.

#### ssim2mysql.error Incorrect integer value for a u8 fconst column
<a href="#ssim2mysql-error-incorrect-integer-value-for-a-u8-fconst-column"></a>

`normalize_acr_my` fails with a MySQL error naming an enum's symbol, not its number:

```
ssim2mysql.error  Incorrect integer value: 'steady' for column `atfdb`.`benchprofile`.`shape` at row 1
```

A `u8` field with a matching `dmmeta.fconst` table is an enum, and `acr` and `amc`
both accept either form of it — the generated `_SetStrptr`/`_ToCstr` accessors exist
so code and ssim data can carry the symbol (`shape:steady`) instead of the number,
and `acr -check` passes a data row written either way.  `ssim2mysql` does not resolve
that symbol: it copies the field's raw ssim text into the `INSERT` it sends to
MySQL, so a `u8` column declared `TINYINT` gets the literal word `steady`, which a
strict-mode server refuses to parse as an integer.  No existing table hit this
before, because every other persisted `u8`+`fconst` field already stores its value
as the plain number the column expects.

Store the enum numerically in the ssim data (`shape:0`, matching the `fconst`
table's `value:`), not symbolically, and let the comment on each `fconst` row carry
the name for a reader.  This is also the convention `dflt` already follows on this
class of field — `algo_lib.RegxOp.op` and `atfdb.Benchprofile.shape` both declare
`dflt:0`, never a symbol.

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
current.**  Several artifacts are derived by citests rather than by `amc`:
`pbapi_gen`, `fast_gen` and `doc_catalog` each regenerate their files and fail when
the tree differs, and a tree that extends openacr adds generators of its own.  Drive `amc` to a fixpoint before
committing, then check the gates as a group:

```bash
atf_ci -citest:'pbapi_gen|fast_gen|doc_catalog|apm_check|normalize_acr|checkclean'
```

A citest reporting `atf_ci.modified_files` is naming a file it regenerated, so
the answer is always to commit that file.  And `atf_ci -check_clean` refuses to
run on a dirty tree, so the commit has to come before the gate can confirm it.

**A mixed cached build can parse one command schema and execute another.**  The
process then reports `algo_lib.signal text:Segmentation fault`, and its backtrace
may end in a generated command destructor or `algo_lib::lpool_FreeMem`.  Compare
the installed program's `-h` output with the fields in
`include/gen/command_gen.h`.  A field present in the header and absent from help
means the executable contains stale objects, even when `abt` reports
`ood_src:0`.  Rebuild the affected targets without the cache:

```bash
abt '<target-regx>' -build -install -force -cache:none
```

Matching help output sends the diagnosis elsewhere.  In the stale-object case,
repeat the exact command after the uncached build before investigating its
runtime code.

**Taking a generated tree from another ref can leave objects built from it.**
After `git checkout origin/master -- cpp/gen include/gen` and an `amc` run that
rewrote those files, `abt` and `ai` can report a clean build, and touching the
files still gets a full cache hit, while an object holds the checked-out text.
Check a constant the change moved in the object itself, with `objdump -dr
build/release/cpp.gen.<ns>_gen.o`.  The uncached build above repairs it.

**Renaming a doc file moves its hash in the package manifest.**  Regenerate
with `apm -package:<package> -generate`, which is exactly what `apm_gen`
checks; without it the rename looks complete and fails the pre-merge gate.

The codestyle skill carries the full cascade table -- which edit stales which
artifact, from a renamed citest function through a `dmmeta.ns` comment to a
deleted `dev.gitfile` row.  The order that converges is the same in every case:
make every edit first, then run `amc`, `update-hdr` and `abt_md` as a group,
then commit, then `bin/normalize`.

### A cross-commit A/B disagrees with the same commit built elsewhere
<a href="#a-cross-commit-a-b-disagrees-with-the-same-commit-built-elsewhere"></a>

The generated tree is not the only thing that goes stale reading as success: so
can the compiled libraries a build reused rather than rebuilt.  `abt <exe>
-build` builds the executable's whole dependency closure, so naming the
executable is safe.  The trap is the timestamp heuristic underneath: `abt`
rebuilds a dependency only when its own sources are out of date against the build
directory's timestamps, and a `git checkout` of another commit sets source mtimes
that need not trip that test.  A shared library like `algo_lib` can then be left
as the build directory last built it -- not the checked-out commit's.  An A/B
that steps across commits this way can compare mismatched libraries, and the
result is not merely noisy, it is fabricated: a runtime bug appears to bisect to a
commit whose own diff cannot explain it.

| symptom | discriminant |
|---|---|
| the same commit behaves differently built here than in another checkout | build the executable's whole dependency closure: `abt <exe> -build -install -force`, and compare again |
| a bisect lands on a commit whose diff cannot explain the behavior | re-run each point with `-force`; a result that moves is a stale-link artifact, not a real bisect |

`-force` treats every file in the closure as out of date, so `algo_lib` and every
other dependency relink from their own commit's sources; with gcache warm this
costs seconds, not a full rebuild.  For a cross-commit A/B, build the executable
by name (`abt ams_sendtest -build -install -force`), never a target list that
stops at the library you happened to edit.  And confirm the baseline is
`origin/master`, not the local `master` ref, which trails it after a fetch of
other branches only — `git rev-list --count master..origin/master`.

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
confined to `cpp/gen` and `include/gen` either: the tables under `data/gendb`
go with it, so rows in `ssimfile:gendb.msgfield` come back with
`strtype:rightpad  pad:0` flattened to `strtype:""  pad:""` — which reads as a
schema edit nobody made.

Two symptoms, and the one that reaches you first is a build error in
hand-written code that calls a generated accessor the old generator spelled
differently:

```c++
cpp/lib_x2net/sock.cpp: error: cannot convert 'algo::aryptr<ams::MsgHeader>' to 'ams::MsgHeader*' in initialization
```

The tell is that it names a file the change never opened.  The second symptom is
`git status` listing generated files and `data/gendb` tables modified in namespaces
the work has nothing to do with — easy to skim past, because a regen is expected
to touch generated files.

Recovering costs one rebuild.  Put the generated tree and `data/gendb`
back, build the generator, then regenerate:

```bash
git checkout HEAD -- cpp/gen include/gen data/gendb
abt amc -build -install
amc
```

A second `amc` then reports `n_filemod:0`, and what it did modify is scoped to
the change.  Rebuild the whole tree afterwards with `abt % -build -install`: the
objects from the bad generation are still in `build/`, and the targets that
failed to compile were never installed.

Then run the citest-derived generators, because this is the one place the
generated tree arrives from somewhere other than `amc`.  Taking `ts/gen` from
upstream brings back upstream's catalogs, such as `doc_cat.ts`, and `amc` writes
none of them, so a field the branch added is missing from each while `amc` sits at
its fixpoint reporting nothing.
The gate group above is what catches it, and a rebase repair ends with that
group rather than with the fixpoint.

A rebase across a commit that moved a directory leaves the files your branch
created behind, and nothing objects.  Git's rename detection carries your edits
into the moved file, so a change to `ts/<old>/empty.tsx` arrives in
`ts/<new>/empty.tsx` intact and never conflicts.  A file your branch
added has no counterpart on master to be renamed from, so its imports still name
the old directory, and a path that resolves to nothing is a compile error rather
than a conflict.  `tsc --noEmit` is what finds them, which is `abt_ts -normalize`
for the `ts/` tree; a TypeScript branch rebased across a move is not resolved
until that has run.

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
those rows without saying so, and the loss surfaces much later, as a build
that misses a row's generated code or a test that misses its fixture.

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

A third cause gives the same unmarked `UU` and the cure above does nothing for it.
`.gitattributes` names `merge=acr_dm`, but what that name resolves to is local git
config that `gitconfig-setup` installs, not something the repo carries.  A clone
that never ran it has the attribute and no driver, so git falls back to its own
text merge: ssim files merge as lines, no conflict is reported for a row order it
silently picks, and order inside a ctype's group in `dmmeta.field` is struct member
layout.  One query separates this from a missing binary, and it is worth running
before trusting any ssim merge:

```bash
git config --get merge.acr_dm.driver   # empty means every .ssim merged as text
```

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
whitespace diff.  Run `abt_md` and commit what it rewrites before
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
file, never by merging it.  Consider a branch that widens a ctype while master
adds new ones, and both touch `gendb.ctypelen`, which `amc` writes.  `acr` aligns
the columns of every file it writes, so a wider value reformats all of the rows,
and master's ctypes insert rows of their own; git reports the whole file as a
single conflict whose two arms differ on every line.  That reads like a far
larger divergence than the one row and one row set that actually changed.  The
insight is that the row set is not the authority — the schema is, and it merged
cleanly — so the file has no content of its own to lose.  Take either arm, then
run the generator, which rewrites the file from the schema including what master
just added.  The tell that a file is in this class is that both arms carry the
same keys.

The same "ours drops your rows" trap has a worse form in hand-written C++,
because there the compiler can stay silent about it.  Consider a branch that
added two functions to a library file and one line elsewhere in the same file
that sets the pointer they read.  Master meanwhile refactored that file, so both arms
differ heavily and taking master's arm is the tempting resolution.  It drops all
three additions.  The two functions have callers in other targets, so the build
names them at once — but the assignment has no caller: the pointer keeps its
generated `NULL` initializer, the branch's own reader compiles and runs, and it
silently takes its fallback path forever.  The build, `amc`, `acr -check` and
`abt_md` all pass such a tree, and only a test of the behavior catches it.

So a resolution is not finished when the tree builds.  Check it against the
branch's own pre-rebase delta, which is still reachable by sha in the reflog:

```bash
git merge-base <old-tip> origin/master              # the branch's original base
git diff <base> <old-tip> -- cpp/<ns>/foo.cpp       # every line the branch added
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

A rebase across a commit that deletes a source file leaves that file's residue in
the two artifacts amc does not own.  `update-hdr` maintains the block inside a
hand-written header and `quickreadme` maintains the usage block inside a tool's
readme, and the merge driver keeps the branch's arm of each while taking master's
deletion of the `.cpp`.  The header goes on declaring functions nothing defines,
the readme goes on documenting options the tool no longer has, and nothing
objects: an unused declaration links, and `amc` reporting `n_filemod:0` says only
that amc's own files are current.  So run `update-hdr` and `abt_md` after the
rebase as well, and read what they rewrite — a hunk that removes a verb the
branch never worked on is master's deletion arriving late.

#### abt reports nothing out of date, and the binary lacks a field the generated source carries
<a href="#abt-reports-nothing-out-of-date-and-the-binary-lacks-a-field-the-generated-source-carries"></a>

After a rebase that replaced the generated tree wholesale -- `git checkout
origin/master -- cpp/gen include/gen ts/gen`, then a fresh `amc` -- the tree
builds clean and `abt % -build -install` ends with `n_err:0`, and the installed
binaries are still the ones the previous generated tree produced: a command
refuses an option its own help text in `cpp/gen/command_gen.cpp` lists, and a
client crashes on a message layout it was not compiled for.

```
acr: unknown option value:-newopt
echo -> algo_lib.signal  text:Segmentation fault
abt.config  builddir:Linux-g++.release-x86_64  ood_src:0  ood_target:0  cache:gcache
```

| could be | discriminant |
|---|---|
| the object cache answered the regenerated sources from before the checkout | `strings build/release/<tool> \| grep "<a comment from the regenerated header>"` finds nothing while `include/gen` carries it; `abt <tool> -build -install` says `ood_src:0` |

`abt % -build -install -force -cache:none` rebuilds every object from the
sources as they stand, and the binaries then match the generated tree.

#### abt reports ood_src:0 after a dev.tool_opt change
<a href="#abt-reports-ood_src0-after-a-dev-tool_opt-change"></a>

A row added to `dev.tool_opt` changes the command line of every compile it
matches, and the next `abt` run reports nothing to do:

```
abt.config  builddir:Linux-g++.profile-x86_64  ood_src:0  ood_target:0  cache:gcache
```

| could be | discriminant |
|---|---|
| abt dates an object against its sources and not against the flags that produced it | `abt <target> -cfg:<cfg> -printcmd` shows the new flag while the object predates the row |

`abt <target> -cfg:<cfg> -build -force` recompiles with the flags as they stand.

#### The readme_tut citest rewrites txt/tut/tut01.md with gcache.warning lines in a captured build
<a href="#the-readme_tut-citest-rewrites-txt-tut-tut01-md-with-gcache-warning-lines-in-a-captured-build"></a>

The `comp` job on a CI runner fails with `atf_ci.modified_files  during:readme_tut
files:txt/tut/tut01.md`, and the diff it prints adds lines of this shape to the
tutorial's captured `amc && ai samp_tut1` output:

```
+abt.exec  gcache  -- g++ -x c++ -Wno-invalid-offsetof ... -c cpp/gen/dev_gen.cpp -o build/release/cpp.gen.dev_gen.o ...
+gcache.warning  from:build/release/cpp.gen.dev_gen.o  to:/tmp/gcache/cd/00/cd00d74a2f9884d93b789cf613272ba7bceb5d99  comment:"cache entry could not be published"
```

| could be | discriminant |
|---|---|
| the runner's compiler cache disk is full, so gcache builds the object and cannot store it, and says so on the build's stdout | `df` of the `to:` path's filesystem on that runner reads full; the same job retried on another runner passes with no diff |

The tree is clean and the tutorial is right: the lines are the runner's, and the
fix is its disk.  Retry the job so it lands elsewhere, and free the cache on the
runner that printed them.

#### A job fails in get_sources with `Directory not empty` on temp/cov
<a href="#a-job-fails-in-get_sources-with-directory-not-empty-on-temp-cov"></a>

A merge-request job on a CI runner ends before its script starts: the
`get_sources` section prints its `Removing temp/...` lines and closes with

```
warning: failed to remove temp/cov/atf_x2_cov.d: Directory not empty
ERROR: Job failed: exit status 1
```

| could be | discriminant |
|---|---|
| a coverage job the reaper canceled on that runner left instrumented processes alive, and they write `.gcda` files into the checkout while `git clean` empties it, so the clean fails and the runner stops there | the `Removing` lines name `temp/cov/atf_x2_cov.d/...gcda` files; `ps` on the runner shows processes of the canceled job; the same job retried lands elsewhere and runs |

Nothing of the branch ran, so the commit is not in question.  Retry the job.

#### amc run before the toolchain is rebuilt rewrites every generated file in the tree
<a href="#amc-run-before-the-toolchain-is-rebuilt-rewrites-every-generated-file-in-the-tree"></a>

One schema row was added, `amc` reports a filemod count in the dozens or
hundreds, and `git status` names every `cpp/gen/%_gen.cpp` in the tree:

```bash
report.amc  n_cppfile:611  n_ctype:4922  n_filemod:74
```

A second `amc` run then reports `n_filemod:0`, which reads as a fixpoint and is
not one -- it is the stale generator agreeing with its own output.  The damage
surfaces at the commit, where a branch that touches twenty files produces a diff
touching a hundred and the extra ones are generated files nobody edited.

| could be | discriminant |
|---|---|
| `bin/amc` is built from a different commit than the schema it is reading | `git diff --stat origin/master HEAD` right after the commit -- a file count far above what the branch touched, made up of `%_gen.cpp`, is this |
| the schema change genuinely reaches that many targets | read one of the surprising diffs: a hunk adding or removing a line unrelated to the change (an `ApplyTrace` call, a field the branch never named) is the stale generator |

The cause is that `amc` is itself built from the tree, so every rebase and every
pull can move it.  Running it before the build regenerates the whole tree in the
older generator's format, and the result compiles and passes its checks, because
the tree is internally consistent with the generator that wrote it.

Rebuild the generator, restore the generated files, and regenerate:

```bash
abt amc -build -install
git checkout origin/master -- cpp/gen include/gen
amc
```

The filemod count is then the handful the change actually reaches.  The order in
the pre-push sequence exists for this reason: `ai` comes before anything that
regenerates, so a rebase never leaves a generator behind its input.

#### amc.error unrecognized attr, naming a field you just added to a table amc reads
<a href="#amc-error-unrecognized-attr-naming-a-field-you-just-added-to-a-table-amc-reads"></a>

A field was added with `acr_ed` to a table `amc` itself loads, such as
`dmmeta.msgtype`, and a row was given a value for it.  Every later `amc` run
stops before writing anything:

```
amc.error    comment:"unrecognized attr"  attr:heartbeat
```

`amc` reads its input strictly, and the installed binary was compiled before the
field existed.  Rebuilding it needs the generated code that knows the field, and
producing that code needs a working `amc`.  The cycle only exists while some row
carries a value for the new field.

| could be | discriminant |
|---|---|
| the installed `amc` predates the field | `grep <field> include/gen/dmmeta_gen.h` -- no hit is this |
| the attribute is misspelled | `acr dmmeta.field:<ns>.<Ctype>.%` -- the field is not listed under that name |

Run the installed `amc` once on a copy of the data with the field's values
removed, which keeps the field's own definition, then rebuild it and regenerate:

```bash
cp -r data /tmp/bootdata
sed -i 's/  heartbeat:[YN]//' /tmp/bootdata/dmmeta/msgtype.ssim
amc -in_dir:/tmp/bootdata
abt amc -build -install
amc
```

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
  | grep -v 'field:<ns>\.'                      # your branch's own namespaces
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
why `abt_md -tut` is schema-mutating: the tutorials under `txt/tut` run
`acr_ed -create ... -write` to produce their output, and a plain run leaves
their blocks alone.  Two concurrent runs blank each other's blocks, so never
start one beside a comptest sweep or a normalize.  `abt_md -evalcmd:N` skips
every evaluation.

**An inline command captures whatever the database held when it ran.**  Its
output reads as a fact about the tree and is a fact about one session.
A command that reports whether a record already existed answers one way where
the record exists and another where it does not, so a full `abt_md` on a tree
somebody had created the record in writes that answer into the readme and
commits it.
Local checks stay quiet about that: `-evalcmd:N` never runs the command and
`-check` only validates links, so the `readme` citest under `bin/normalize` is
the first thing to regenerate the line, and it fails on the dirty tree it just
made.  An inline-command hunk in a diff you did not set out to produce is that,
and the fix is to revert it rather than to commit what the run emitted.

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

**Plain `abt_md` refreshes every generated section, the `Syntax` block included.**
The `quickreadme` citest under `bin/normalize` is `abt_md -evalcmd:N`, so a tree
on which plain `abt_md` leaves no diff passes it.

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

**`abt_md` adds a missing generated section, and never restores prose.**  When
a page lacks a section that a `dev.mdsection` row generates for its path,
`abt_md` writes the heading and what the row generates.  A README's prose and its
`Description` text are somebody's writing, so deleting the file loses them.

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
a string in code.  Renaming a command's `-exec` to `-cmd` left a TypeScript
file composing the retired spelling into the line that spawns a pty bridge, and
the web UI's tests mock the command runner rather than inspecting the line, so
nothing failed: two features were broken on an otherwise green tree.  A doc that keeps the old spelling
is cosmetic, and code that keeps it is not.

The scope that misses it is the natural one.  A sweep over `cpp/ txt/ test/
data/` covers where command options are declared and documented, and that is
the scope the `-exec` rename used; `ts/` holds no ssim and no handler, so there
is no obvious reason to include it.  The reason is that a single-page app
composes command lines as strings and sends them to a server, which makes `ts/` a
caller of the option table exactly as `cpp/` is.  Sweep it too.

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
that tool's own `-exec` flag, which must keep its name while the renamed one
changes.

#### quickreadme rewrites a README that abt_md just regenerated
<a href="#quickreadme-rewrites-a-readme-that-abt_md-just-regenerated"></a>

A command gained an option, `abt_md` ran, the README was committed, and
`bin/normalize` stops at `quickreadme`:

```
atf_ci.modified_files  during:quickreadme  files:txt/exe/samp_agg/README.md  success:N  comment:"Please resolve modified files and try again"
```

| could be | discriminant |
|---|---|
| the first `abt_md` pass after a new option does not reach its fixpoint | `git diff` shows one added blank line after the new option's `<a href>` anchor, and a second `abt_md` leaves the file unchanged |

The first pass writes the new option's section without the blank line that
separates it from the next one, and the second pass adds it.  Run `abt_md`
twice after adding a `command.<tool>` field, and commit what the second pass
leaves.

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
amc writes tables as well as code -- `ssimfile:gendb.ctypelen` records every
ctype's computed length, `ssimfile:gendb.dispsig` records every dispatch's
signature -- and a generator built before a new kind of row was declared does
not know to emit it, so running it *deletes* that row along with the generated
line that loaded it at boot.  The run exits zero and `acr -check
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

**A new ctype field drifts every comptest golden that prints that ctype's
rows.**  Add a field to `dev.Target` and any golden that dumped a target row now
shows the new column and its comptest fails.  The change touches no line of that test's source, so it
reaches `n_filemod:0`, builds, and passes `bin/normalize` in full while the
`comp` and `coverage` jobs go red, exactly as a name-derivation change does.
After adding a field, grep the goldens for a row of the ctype
(`grep -rl "<ns>.<ctype>  <key>:" test/atf_comp`) and recapture each, or run the
`comp` cijob, before pushing.

**A comptest diff is read, not blessed.**  `atf_comp` prints a coloured diff
between `temp/atf_comp/<name>` and `test/atf_comp/<name>`.  A diff consistent
across reruns is a regression and the code is what changes.  A diff that varies
between runs, or names a timestamp, port, pid or generated id, is
non-determinism, and it is fixed by adding the field to
`ssimfile:atfdb.unstableattr` or by writing an `atfdb.tfilt` rule -- never by
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

**A missing `node_modules` is a missing build.**  `abt_ts` is to `ts/` what
`ai` and `abt` are to `cpp/`, and it is run on demand rather than once per
machine.  `abt_ts -normalize` installs and typechecks, `-build` implies that
and then bundles, `-clean` drops the caches.  A test that shells out to a
package-local binary fails with `No such file or directory` and `exit ... code:127`,
which is the build step having been skipped and never an environment fault.

**`abt -jcdb` writes a `compile_commands.json` for editor tooling.**  This
project has no `cmake` or `make`, so `clangd` finds no compilation database on
its own and falls back to guessed flags.  `abt "%" -cfg:release
-jcdb:compile_commands.json` writes the exact command line `abt` would run for
every selected source, without building anything.  The file lands at the repo
root and is already listed in `.gitignore`, because it holds one checkout's
own paths and configuration.  Regenerate it after a target's source list or
its flags change, because `clangd` reads whatever is on disk and gives no
signal that it has gone stale.

**macOS keeps its adaptation layer in one file.**  A call that Linux provides and
macOS does not needs a function with a Darwin body, and those bodies belong in
`cpp/lib/algo/macos.cpp`, inside one `#if defined(__MACH__)` spanning the whole
file.  `dev.targsrc` names a target and a source with no uname between them, so
every source in a target compiles on every platform, and the file-wide `#if` is
what leaves this one empty on Linux.  A conditional that picks between two bodies
of the same function stays where it is, and so does one in a header, since
neither is a function that is missing.

**`atf_comp -capture` rewrites `dev.gitfile` from `git ls-files`, and a row for
a file git does not track yet goes with it.**  The capture ends by running
`update-gitfile`, which replaces the table with one row per tracked file.  A new
test fixture, a new comptest driver source and the `dev.targsrc` row that named
that source are all inserted before the first capture, and none of them is in
git yet, so the capture deletes the gitfile rows and the targsrc row that
referenced one of them.  The report line says so -- `n_delete:9` on a run that
should delete nothing -- and the next `abt atf_comp` fails to link the drivers
whose source has no targsrc row.  `git add` the new files before the capture, or
re-insert the rows after it and read the capture's `report.acr` line either way.

### Gates that pass without checking anything
<a href="#gates-that-pass-without-checking-anything"></a>

#### A gate's printout is a sample, not an inventory
<a href="#a-gate-s-printout-is-a-sample-not-an-inventory"></a>

A check that names what it objects to usually names only the first few, and it
need not say so.  Ten of ten and ten of a hundred read identically, so a package
carrying twenty-four forbidden rows reports ten, and a fix addressing exactly
those ten leaves fourteen that surface next run looking like a fresh failure.
The shape that produces this is one counter serving two purposes: the variable
that caps the printout is the variable the summary reports, so it stops counting
at the cap.  `apm.keyword` keeps them apart -- `n_bad` counts every hit and
`n_shown` caps the printout, and its summary line carries both -- and a gate that
prints only one number has not been separated yet.

There is no signal in the printout to be suspicious of, so the habit has to be
unconditional: reproduce the gate's own query and count, before writing the fix.
Every such check is a command with a filter over it, and running that command
directly is what turns the sample into the list:

```bash
apm -package:openacr -l -showrec | grep -iE '<word>|<word>'
```

This is the companion to proving a grep can report a hit before believing its
silence.  That rule says an empty result is not evidence of absence; this one
says a bounded result is not evidence of extent, and neither failure announces
itself.

#### A test reports on code the tree no longer holds
<a href="#a-test-reports-on-code-the-tree-no-longer-holds"></a>

Proving that a new test can fail means stubbing the thing it tests, building,
watching it go red, then restoring the file and building again.  The last build
is the one to distrust.  Restoring a file by copying a backup over it can leave
`abt` believing the target is current, so it installs nothing, and the test then
reports the behavior of the stub while the source on disk reads correctly.  What
you see is a test that fails with the message you wrote for the stub, against a
function that is plainly right in the editor.

```
report.abt  n_target:22  time:00:00:00.036165061  hitrate:0%  pch_hitrate:0%  n_warn:0  n_err:0  n_install:22
```

The runtime is the discriminant, the same as in the section below: a build that
recompiled a translation unit does not finish in thirty milliseconds.  `touch`
the source and build again, and the test agrees with the source.  The habit that
avoids it is to restore the file with `git checkout -- <path>` or an edit rather
than a copy, and to read the build's runtime before believing the test that
follows it.

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

#### A comp job that names three failing goldens has stopped counting
<a href="#a-comp-job-that-names-three-failing-goldens-has-stopped-counting"></a>

The `comp` job runs `atf_comp` at its default `-maxerr:3`, so its log names the
first three goldens that differ and ends there:

```
atf_comp.end  comptest:samp_exch.Addorder  success:N  nlines:52  duration:0.71
atf_comp.end  comptest:acr_ed.CreateCtype  success:N  nlines:147  duration:0.75
atf_comp.end  comptest:acr_in.Reverse  success:N  nlines:78  duration:0.70
atf_ci.citest  citest:atf_comp  runtime:00:04:51  success:N
```

Three is the cap, not the count.  A change that moves one table's layout moves
every golden holding that table, and a formatting row is such a change: a tool
that right-aligns a column only when its field has a format entry re-spaces every
golden that lists the table the moment one field gets one, nineteen of them in
the case that taught this.  Recapturing the three the log named and pushing found the next
three a pipeline later, and the sixteen after that a pipeline after that.

Run the whole sweep locally with the cap lifted before the push, and read the
count it reports:

```bash
atf_comp % -maxerr:200 2>&1 | grep -E 'success:N|report.atf_comp'
```

A recapture of a table whose values did not move is whitespace, so check it
with `git diff -w -- test/atf_comp` before amending: a line that survives that
diff is the capture picking up something else, most often the exit lines of
the processes a test spawned changing order, and belongs restored from the old golden.

#### A comptest golden passes locally and reorders its sorted lines on CI
<a href="#a-comptest-golden-passes-locally-and-reorders-its-sorted-lines-on-ci"></a>

```
14,15d13
< acr -> dmmeta.ctype  ctype:amc.FCtype  comment:""
18a17,18
> acr -> dmmeta.ctype  ctype:amc.FCtype  comment:""
```

| could be | discriminant |
|---|---|
| a `sort` in the comptest's command follows the locale | the command sorts without `LC_ALL=C`, and `LC_ALL=C atf_comp <test>` fails locally the way CI does |

A locale collation such as `en_US.UTF-8` skips spaces and punctuation, so an
indented `  "dmmeta.field` line sorts after `dmmeta.ctype` on a
developer box.  A runner that sorts byte-wise puts it first.  Pin every `sort`,
`uniq` and `comm` a comptest runs to `LC_ALL=C sort`, then recapture.

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
rather than anything about the branch.  That refusal exits zero, so anything
reading the status code alone records the skipped run as a pass.  Read those
modified files as the previous pass's product — most often a regenerated table
or an indent fix, which is the fix already written for you — commit or amend
them, and only then rerun.  The builds inside are incremental, so a rerun from
a clean tree resumes where the capped pass stopped.

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

#### Every array of the algo namespace is backed by algo_lib's lpool
<a href="#every-array-of-the-algo-namespace-is-backed-by-algo_lib-s-lpool"></a>

`dmmeta.basepool` names the pool a `Tary` grows from, and every `Tary` under
`algo` -- `cstring`, `ByteAry`, `Tuple.attrs`, `StringAry`, the numeric arrays,
`LineBuf` -- names `algo_lib.FDb.lpool`, as do the header arrays of the `http`
protocol namespace, which has no pool of its own.  An array with no base pool grows
through `algo_lib.FDb.malloc`, and its first growth is one counted malloc per
instance, so a temporary built on a hot path costs a system allocation each
time.  A gateway REST request parses its URL into a `Tuple` and formats through
a few arrays, and cost a dozen mallocs a request until the arrays joined the
lpool; a WebSocket frame built in a `ByteAry` cost one per frame.  A new `Tary`
in `algo` gets its basepool row in the same change.  One consequence for code
that appends to an array: the lpool moves a block that outgrows its size class,
where the system realloc often grew it in place, so a pointer taken into an
array before an append is not the array afterwards.

#### Leaks a leak check does not report (pool memory)
<a href="#leaks-a-leak-check-does-not-report-pool-memory-"></a>

`Lpool` and `Tpool` mark each block they hand out, so a row allocated from an
amc pool and never deleted is reported by `valgrind --leak-check=full` with the
stack that allocated it.  Without the marks the checker sees only the 2MB
mapping the lpool took from `algo_lib.FDb.sbrk`, and nothing about the rows
carved out of it.

The marks are sound only over a base valgrind does not track.  A `Tpool` whose
base resolves to `algo_lib.FDb.malloc` takes each block from the real malloc, so
the checker already holds that block as a heap chunk, and the records the pool
marks inside it share its address range.  Nothing goes wrong until a block is
used up: the record at offset 0 has the block's own address, and its free reads

```
Mismatched free() / delete / delete []
   at MemcheckFree (algo.inl.h)
   by pdep_FreeMem (acr_gen.cpp)
```

because the chunk valgrind finds at that address is the malloc'd block.  The
records that block still holds then report as `definitely lost`.  Which pools
reach that state depends on how many records a run allocates, so a comptest
that passed for a year fails when a few more rows land in the table it prints.
The base a `Tpool` uses is the field's `dmmeta.basepool` row, else the
namespace's `nsx.pool`, else malloc, and every namespace whose records matter
under memcheck names an `Lpool` over `algo_lib.FDb.sbrk` there, as `amc`, `abt`,
`acr` and `lib_x2` do: the lpool marks the block, the tpool unmarks it before
carving, and only the records stay marked.

The marks live in `cfg:memcheck` only, because a valgrind client request costs
its instructions whether or not a checker is attached.  A memcheck run against
any other configuration therefore reports invalid reads and writes and reports
no leak inside a pool -- and a leak report that names nothing reads exactly like
a clean run.  `atf_comp` prints `atf_comp.memcheck_cfg` when it notices, and the
fix is `-cfg:memcheck`:

```bash
atf_ci -cijob:'memcheck%'                                 # each shard's mem_prep builds it, then runs
abt % -cfg:memcheck -build                                # by hand: build it
atf_comp -mode:memcheck -cfg:memcheck <comptest regx>     # then drive it
grep -l "definitely lost" temp/atf_comp/*/*.memcheck.*.log
```

In a worktree the same command answers `abt.builddir builddir:-.memcheck-`,
because `wt` plants `build/release`, `debug`, `coverage` and `profile` and no
`build/memcheck`; create the `Linux-g++.memcheck-x86_64` directory beside them
and the `memcheck` symlink to it, then build `%`, since a test that starts a
supervisor spawns every binary from its bindir, and a partial build ends the
cluster before readiness (`atf_comp.not_ready`).  `atf_comp -mode:memcheck` on a comptest
whose row was just switched to `memcheck:Y` answers `atf_comp.nomatch` until
`amc` and a rebuild of `atf_comp` carry the row into the driver.  The install
repoints `bin/` at the memcheck builds; `git checkout -- bin` puts release
back.

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

#### The coverage cijob fails with atf_cov.coverage_lost
<a href="#the-coverage-cijob-fails-with-atf_cov-coverage_lost"></a>

Every citest before `cov_finalize` reports success, and then the merge says the
run lost data:

```ssim
atf_cov.merge  covdir:temp/cov/atf_comp_cov.d  n_gcda:812  n_gcov:0  n_fail:0  success:N  comment:"gcov read nothing here; every target this citest alone exercises is lost"
atf_cov.coverage_lost  n_covdir_empty:1  n_covtarget:18  n_unmeasured:56  success:N  target:"abt_md acr acr_compl amc ..."  comment:"run lost coverage data; no target is judged against its floor"
```

The verdict is about the run, so the named targets are evidence rather than
findings: they are the targets the missing data would have measured, and the
diff under test has nothing to do with which ones they are.  Read the
`atf_cov.merge` line above it -- it names the directory, and the directory names
the citest whose data went missing.

| could be | discriminant |
|---|---|
| the instrumented binary died instead of exiting | a binary writes its coverage database as it exits, so a crash or a SIGKILL leaves nothing behind; read the citest's own output for the process that ended badly |
| the coverage build produced no program graphs | `atf_ci.cov_prep` reports `n_obj` and `n_gcno` at the end of the build, and fails there when they differ; a passing `cov_prep` rules this out |
| gcov could not read one of the profiles | an `atf_cov.gcov_fail` line above the merge line names the `.gcda` or `.gcno` gcov refused and why, in gcov's own words; `n_fail` on the merge line counts such commands, and a directory whose other files still yielded coverage is kept and judged per target |
| the covdir was cleared under the run | `cov_prep` empties `temp/cov`, so a second pipeline sharing the checkout removes a directory the first one is still filling; compare job start times on the runner |

`n_gcda:0` says the citest wrote no coverage database at all, and a non-zero
`n_gcda` with `n_gcov:0` says gcov could not read what it wrote.  The two have
different causes: the first is about the binary that ran, the second about the
program graphs beside its objects.  A non-zero `n_fail` with a non-zero
`n_gcov` is neither: gcov read the directory and refused one file in it, the
`atf_cov.gcov_fail` line says which, and the run goes on to judge each target
against its floor.

The whole class stays diagnosable only because the merge reports per directory.
A run that reported nothing but per-target floor breaches sent its reader into
the diff, which is the one place the cause has never been.

#### gli refuses to load with lib_gli.duplicate_key after a rename
<a href="#gli-refuses-to-load-with-lib_gli-duplicate_key-after-a-rename"></a>

```
gli.load_input  lib_gli.duplicate_key  xref:lib_gli.FDb.ind_label
data/glidb/label.ssim:<line>: glidb.label  label:<new>  ...
```

| could be | discriminant |
|---|---|
| a `sed` rename turned one row into a copy of another | `awk '{print $2}' data/<ns>/<file>.ssim \| sort \| uniq -d` names the repeated key; the earlier copy sits where the old name sorted |
| the file held the pair before the rename | the same pipe over `git show origin/master:<path>` names it too |

`acr -check % -x` reports `n_err:0` with two rows under one key, so a clean check
proves nothing here.  The tool that indexes the table, `gli` for `glidb.label`, is
the one that objects.  Renaming a namespace by `sed` to a name that already had
a row duplicated rows in both `glidb.label` and `dev.pkgkey`.  Run the pipe over every ssimfile after a
rename, then drop the repeats and let `acr '<ssimfile>:%' -write` restore the
order.

#### A wait loop on pgrep -f never ends
<a href="#a-wait-loop-on-pgrep-f-never-ends"></a>

```
until ! pgrep -f "atf_x2 stress_msgsize" >/dev/null; do sleep 20; done
```

`pgrep -f` matches whole command lines, and the loop's own `bash -c` carries
the pattern, so the loop always finds itself.  One such loop outlived its test by
nineteen hours.  Manage the pid explicitly instead: run the command in the
foreground under `timeout`, or keep `$!` when it starts and wait on that pid
(`while kill -0 <pid>`).

#### A regex-scoped comptest run is not the comptest gate
<a href="#a-regex-scoped-comptest-run-is-not-the-comptest-gate"></a>

Adding a field to a ctype whose rows a test prints changes every golden that
prints one, and the count is larger than it looks: one field on a response
ctype moved nine comptest goldens, an inline command's output in the command's
protocol page, and the TypeScript interface, whose typecheck then rejected three
test fixtures that build the ctype by hand.

The trap is in how the change is checked.  `atf_comp '<pattern>'` runs the tests
the pattern names, and a pattern is written from the tests the author has in
mind, so the ones they did not think of pass by not running.  Four of those nine
goldens were recaptured that way and the run reported 16 of 16; `comp` then
failed on three more, and two rounds of `atf_ci -citest:atf_comp` -- the citest
the job actually runs -- turned up the rest, two or three at a time, because a
loaded box flakes a different pair of timing-sensitive tests on every pass.

So the gate is the citest, never a pattern:

```bash
atf_ci -citest:atf_comp        # what the comp job runs
```

And for this class of change there is a check that does not run a test at all,
which is what makes it complete.  The goldens are text, so the field's absence
is greppable:

```bash
# an ssim-form row of the ctype that predates the new field
grep -l '<ns>.<Ctype>' test/atf_comp/* | while read f; do
  grep -H '<ns>.<Ctype>' "$f" | grep '<oldfield>:' | grep -v '<newfield>:'
done
# a table-form header without the new column
grep -H '<column>  *<column>' test/atf_comp/* | grep -v '<newfield>'
```

Run both against `origin/master`'s goldens before believing an empty result: they
report 22 rows and 13 headers there, which is what says the greps can fail.  An
empty result on a tree where the pre-change copy answers is a complete pass over
a class a test run samples.

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
names the row of the table holding it and reads sideways in a sentence.  Both are
checked against the database, so a table that moves to another namespace stops being a
silent staleness and becomes a failing check.

An unqualified span is otherwise not read as a key, because the bare leaf form is how an
attribute appears inside a tuple: `cascdel:Y`, `cfmt:Argv` and `sandbox:Y` are values
rather than keys, and reading them as keys reports every one of them.  What admits
`ssimfile:` and refuses those is the leaf's own table -- a table keyed by a table name is
a table about tables, so a span naming one is a reference to a table.  Nothing else in the
tree is.
