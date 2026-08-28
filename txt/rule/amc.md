## amc: Rules
<a href="#amc-rules"></a>

`amc` reads the ssim database and writes C++ under `cpp/gen` and `include/gen`.
Its output is committed, so a rebuilt `amc` regenerating the tree byte-for-byte
is the check that closes over every rule below: any of them broken shows up as a
diff.  What follows is the part of `amc`'s design that its source does not state.

### Invariants
<a href="#invariants"></a>

**The access list above a struct names only same-namespace accesses, so Base is left out
of it.**  A Base relation crosses namespaces as a matter of course: every message of a
protocol embeds the header, and each is written in whichever namespace its own author
worked in.  A header's list therefore named the handful of messages beside it and none of
the hundred elsewhere, which reads as the whole answer and is not one.  `doc msg:<ctype>`
answers that relation instead, from `dmmeta.typefld` and the Base fields.

**A varlen tail is representable whatever it holds, so a message carrying one still gets
its `dmmeta.msg` and `dmmeta.msgfield` rows.**  The walk that derives them refuses a field
it cannot place, because a wrong offset decodes garbage -- but a varlen is the last field
of its message, so nothing follows it to be misplaced.  What the row says is where the tail
begins, and its arg says what the tail is made of: `char` for text, a message ctype for a
run of framed messages.  Twenty-two messages under `ams.MsgHeader` were skipped entirely
before this held, `x2.PubMsg` among them.

### Naming a record in generated code
<a href="#naming-a-record-in-generated-code"></a>

A generated function is handed records, and it has to call them something.  Two
roles exist and each has exactly one word.  **`parent`** is the container the
function reaches a record through.  **`row`** is the record the function acts on.
A signature carries at most one of each, which is what
`c_ssimfile_InsertMaybe(FCtype& parent, FSsimfile& row)` reads as: reach into
this container, act on that record.

Three functions produce these names, and each answers a different question.

| function | question | value |
|---|---|---|
| `Instname(ctype)` | what does this ctype contribute to an identifier? | its own name in lower_under, leading `F` dropped |
| `Varname(ctype)` | what variable holds a record of it? | `_db` for a global, the instname otherwise |
| `Refname(ctype)` | what does a body call the container? | the Varname for a global, `parent` otherwise |

`Instname` is stored on `amc.FCtype.instname` and derived from the ctype's name
alone -- `strptr_PrintInstname`, a leading-`F` strip in front of
`algo::strptr_PrintLowerUnder`.  Deriving it from the name is the point.  It used
to be taken from whichever field instantiated the ctype first, in the order
fields appear in `field.ssim`, and ten ctypes have more than one instance; for
those, moving two unrelated field rows renamed generated identifiers.  A field
row's position decides struct layout and nothing else.

#### Why the parameter is a fixed word
<a href="#why-the-parameter-is-a-fixed-word"></a>

Naming the parameter after its ctype looks tidier and does not work.  Every
generated body that aliases its argument calls it `row`, so a ctype whose derived
name is `row` shadows that alias -- which is why `lib_sqlite.FRow`'s parameter was
once spelled `trow`, a dodge nobody recorded the reason for.  Thirty-three ctypes
in the tree would collide the same way against a local `amc` emits fifty or more
times: `ctype` (thirteen of them), `msg` (six), `cmd` (seven), and `err`, `row`,
`child`, `base`.

A single reserved word cannot collide, and it buys a second thing.  Two
generators must agree on the name of one function's argument -- the Atree child
`Init` writes into the same function the ctype's own `Init` declares -- and with a
fixed word they agree by construction instead of by both deriving the same
string.

The reservation has to be enforced rather than assumed.  `amc` emitted `parent`
itself, in the Atree rebalance helpers, and `InsertImpl` carried the record
argument beside it, so the signature came out
`InsertImpl(FCascdel& parent, FCascdel* parent, FCascdel& row)`.  The Atree
generator's tree-parent identifier is `up` for that reason.

#### An identifier that names a variable keeps the underscore
<a href="#an-identifier-that-names-a-variable-keeps-the-underscore"></a>

A trace counter is `_db.trace.alloc__db_malloc`, with the double underscore, and
that is not an accident to be tidied.  The counter names the *variable* holding
the pool, and for a global that variable is `_db`, so the name comes from
`Varname` rather than from `Instname`.  `lib_ams` recovers a pool's metric name by
stripping the three leading components that spelling gives it
(`Pathcomp(attr.name,"_LR_LR_LR")`), so building the counter from the instname
instead produces `alloc_db_malloc` and silently renames every metric.  It
compiles, it passes, and the metric names are wrong.

The same rule picks `Varname` for a hook's generated ctype
(`<ns>.<varname>_<field>_hook`) and `Instname` everywhere the identifier names a
type rather than a variable: static hook functions, ptrary membership fields,
`CopyIn`/`CopyOut`, and the `$xfname` prefix a child record carries.

#### Init and Uninit name their record `parent`
<a href="#init-and-uninit-name-their-record-parent-"></a>

Every other ctype-level function names its one argument `row`, because that
argument is the subject.  `Init` and `Uninit` do not, and the reason is
structural rather than a matter of taste.

Their bodies are not written in one place.  Around a hundred field-level
generator sites splice text into them -- every reftype contributes its own field
initializer and its own teardown -- and in *those* generators' scope the enclosing
record is the container, which is `$parname`.  So the argument has to be named
whatever those hundred sites already call it.

Two fixes suggest themselves and neither works.  Rewriting the splice sites to a
row-shaped variable cannot be complete, because the record also arrives through
replvars whose *values* carry `$parname` -- `$Root`, `$NElem`, `$parelems`,
`$lenexpr`, `$_`, `$a_val` -- and those resolve at the point of use, where no
textual pass over the generator sources reaches them.  Rebinding `$parname` for
the duration of an `Init` or `Uninit` tfunc fails differently: several of those
tfuncs also declare standalone functions that take the container in the same
breath (`$name_EmptyQ($Parent)`, `$name_Last($Parent)`,
`$name_ElemLt($Parent, ...)`), so one rebinding serves one and corrupts the other.

Naming the argument `row` therefore requires the emission layer to know each
target function's argument name, which `Ins` does not -- it takes a buffer, not a
function.  Until that changes, `Init` and `Uninit` say `parent`, and they say it
consistently: the alias that used to bind a second name for the same record in
those bodies is gone.
