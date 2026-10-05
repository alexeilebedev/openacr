## amc: Rules
<a href="#amc-rules"></a>

`amc` reads the ssim database and writes C++ under `cpp/gen` and `include/gen`.
Its output is committed, so a rebuilt `amc` regenerating the tree byte-for-byte
is the check that closes over every rule below: any of them broken shows up as a
diff.  What follows is the part of `amc`'s design that its source does not state.

### Invariants
<a href="#invariants"></a>

**Every table amc writes back lives in `gendb`, and amc reads none of them.**  A
`gendb` row is computed from the rest of the schema on each run, so an edit to one
lasts until the next `amc`.  A table that amc both reads and writes would make a
run's output depend on the previous run's, and a table amc writes outside `gendb`
would read to everyone else as input.

**A generator that emits a symbol for a record registers the pair in
`gendb.cppsym`, at the point where it emits it.**  The table is how tools that
read hand-written code learn which record a name stands for, so a symbol that
was emitted and not registered is a dependency nobody can see.  Today that is
ctype structs, `fconst` constants (a `gconst` constant names its value-table
row, a message type its `dmmeta.msgtype` row), `gsymbol` strings, the named
references of a `gstatic` table, and the user functions amc declares for a
record to bind (`extrn:Y`: handlers, steps, citests), which `apm.mixedfile` and
`src_func -createmissing` read.  A ctype or constant amc makes up has no
record, and so no row.

**No block the memory checker holds contains another it holds.**  A pool marks
each block or element it hands out, and a pool carving from a marking pool
(`MemcheckedPoolQ`) unmarks the block it receives first.  A malloc block cannot be
unmarked, so a Tpool on malloc marks no element (`MarkElemQ` in
`cpp/amc/tpool.cpp`): the element at the block's first byte shares the heap
chunk's address, and freeing it reads to valgrind as a mismatched free once a run
uses a whole block.

**An exec wrapper's `memlimitmb` limits the child's private memory.**  The generated
`Start` applies it as `RLIMIT_DATA`, soft and hard, in the forked child before exec.
On Linux 4.7 and later that limit counts private writable mappings -- the heap,
malloc's arenas once written, thread stacks -- and none of the shared segments the
child maps, so the rings and boards of a node cost the child nothing.  An older kernel
caps only the `brk` heap, and every mmap'd allocation escapes it.  `RLIMIT_AS` would
count each mapping in full, and a child that maps a segment larger than its limit
could not map it at all.

**The access list above a struct names only same-namespace accesses, so Base is left out
of it.**  A Base relation crosses namespaces as a matter of course: every message of a
protocol embeds the header, and each is written in whichever namespace its own author
worked in.  A header's list therefore named the handful of messages beside it and none of
the hundred elsewhere, which reads as the whole answer and is not one.  `doc msg:<ctype>`
answers that relation instead, from `dmmeta.typefld` and the Base fields.

**A finput's update path puts the row back on each access path it left through the
call that path has.**  A hash refuses a duplicate key and says so, so it is rejoined
through `InsertMaybe`; a heap and a tree take any row and have no Maybe, so they are
rejoined through `Insert`.  Generating a Maybe for an Atree left the generated code
naming a function that does not exist, which only the first Atree on a finput with
`update:Y` found.

**A finput's update path copies every field it can and refuses the ones it cannot.**  A
Val field is assigned; a Regx field is re-read by the reader for its style, which clears
the old states before it parses the new expression.  A field of any other reftype stops
`amc` rather than being skipped.  An update that leaves a field behind reports success and
changes nothing, and the process that answered the command is the one place the new value
shows -- it mutates its own row before publishing, so every other process goes on
enforcing the old one.

**A generated reader skips an attribute that names no field of the ctype.**  A row on
disk or on a durable log outlives the build that wrote it, and a field removed from the
schema leaves its attribute on every row written before the removal; a reader that refused
the row would drop it silently, since a finput with `strict:N` skips a row it cannot read.
So `<Ctype>_ReadFieldMaybe` treats the attribute as extra information, the way `acr`
reads a row against the current ctype.  A parser of typed input sets
`algo_lib::_db.strict_attr` around its read, and the attribute is then an error naming
itself (`unrecognized attr`, `attr:<name>`).  `amc` loads the source tree that way, so a
typo in `data/` still stops the generator at the row that carries it, and so do
the `gli` `-filter` and `-set` attributes and the `acr_compl` request line.  `acr.rowid` rides on rows `acr -rowid`
prints and is skipped under either setting, and a value with no attribute name, a
positional past the last anonymous field, is refused under either.  The amctest
`ReadTupleUnknownAttr` holds the shape.

**A varlen tail is representable whatever it holds, so a message carrying one still gets
its `gendb.msg` and `gendb.msgfield` rows.**  The walk that derives them refuses a field
it cannot place, because a wrong offset decodes garbage -- but a varlen is the last field
of its message, so nothing follows it to be misplaced.  What the row says is where the tail
begins, and its arg says what the tail is made of: `char` for text, a message ctype for a
run of framed messages.

**A message with several varlen fields is refused before a handler reads it when the
end offsets it carries are not monotone and inside the varlen area.**  A message
carries the end of each varlen field but the last, and every accessor of a later
field subtracts one end from another, unsigned.  The ends come off the wire, so a
client built against another layout puts payload bytes where they sit, the
subtraction wraps to gigabytes, and the first read walks off the buffer.  Castdown
and every dispatch case test the ends (`VarlenBoundExpr`) as they test the length
against the fixed size, and the amctest `CastDownVarlenEnd` holds the shape.

**A connection ends after the last message its peer sent.**  An fbuf pair shares
one descriptor, and the kernel reports a hangup or an error on it together with
the bytes that arrived before it.  Say a NATS client writes 95KB of publishes and
closes.  The gateway reads 64KB of messages per pass, so it has read the whole
burst into its buffer before it has processed a third of it.  The pair's
readiness callback (`PairReady`) therefore arms the read side on a hangup or an
error, exactly as on a read event.  The read reaches the end of the stream after
the bytes, and the read buffer queues the end once no message is left in it.  A
pair whose read buffer has no ready list is ended at once, since nothing else
would wake it.

**A list has at most one step per process.**  Two steps draining one list would race for
its rows, and which one saw a row would depend on the order of `Steps()`.  So an alias step
on a list that has a step of its own replaces that step in the executable's `Steps()`, and
amc refuses two alias steps on one list inside one executable's closure.

**The order of a pass is a latency hint.**  `Steps()` runs the bands of `amcdb.stepband`
in rank order and, within a band, the namespaces in dependency order.  Each step is a
transaction, so no step relies on another having run earlier in the same pass; moving a
step to another band changes how soon a reaction happens and never whether it happens.
A step in `output` moves bytes that are already buffered out of the process; a step that
composes a message, such as a heartbeat or a serve, is `work`, so the output band flushes
what it wrote in the same pass.  A time hook step fires from the `algo_lib` heap of its band,
and only `work` and `idle` have one.

**A message prints by its schema, and no library names a message to print it.**
For every message header with a length field, amc generates
`<Header>Msgs_PrintFmt`, one case per message carrying a varlen field.  A field of
messages framed by the same header makes the message a layer, which
a byte field prints inline, or on a line of its own when it is the last; a field
of rows prints a row per line.  Two `dmmeta.msgtype` flags carry what the layout
cannot.  `strip` names the messages header stripping (`algo.MsgFmt.strip`, `-hdr`)
removes: a layer yields the messages it nests, a leaf the bytes it carries, and
every other message prints whole, since its fixed fields are data -- a file
chunk's offset, a ssim record's verb.  The schema cannot tell those apart from a
header, so the flag says it.  `heartbeat` marks a liveness message a trace leaves
out.  The one extension point is
`algo.MsgFmt.h_convert`, which the printer calls on a byte field when a reader
installed it.  The cases live in the schema rather than in `lib_ams`, so a
generic library stays tied to no one protocol, and a new message prints correctly
by being declared.

**Only an executable has a step function.**  Its `Steps()` names every step of every
namespace it links, so the whole main loop of a process reads in one generated function,
and a library step is a slot an executable may fill with its own.

**A projected ctype takes one of three forms, and its own schema decides which.**  A
packed ctype gets a codec, an unpacked plain-data ctype whose members are scalars or
nested ctypes gets its memory layout, and anything else gets the type alone.  No row
asks for a form, so a ctype cannot be projected as a wire form it does not have.  The
form decides the checks too.  A codec in Go, Python or Rust answers to `gen_check_proj`'s
byte-exact wire form.  A TypeScript codec answers to `gen_check_ts`, which admits more
field forms (an Opt, a Varlen of records, a Smallstr read as a string), so a packed
ctype projected into TypeScript alone is held to that check.  A type answers only to
its members having a type in every language.

**A projected layout states the offsets sizing computed, and C++ is held to the same
numbers.**  `CountStructField` records each member's offset on `amc.FField.offset` as it
builds the `algo_assert(_offset_of(...))` line for that member.  Those lines go into the
namespace's `SizeCheck` for every ctype projected into Go, Python or Rust with a layout,
whether or not the namespace packs.  A projected layout therefore cannot disagree with
the compiler without the C++ build failing first, which is what lets a Go, Python or
Rust process share a shm ring with a C++ one.

**A dispatch is projected by a `dmmeta.displang` row, never derived.**  Deriving it from
"every message is projected" selects every executable's `In`, `Shm` and `Syscmd`
dispatch the moment a protocol namespace is projected, about thirty of them in a large
tree.  A client's dispatch also lives in a library namespace, which an `nslang` row
cannot project.  So `displang`
names the dispatch and seeds nothing, and `amc` refuses one whose messages the
projection lacks.

**Each language is written by one step per namespace, `lang_<lang>`.**  In Go, Python
and TypeScript a namespace's text reads only that namespace's records, its imports
included, so no language needs a pass over every namespace before any file is written.
Rust is the exception, because a crate has one root.  The root is the namespace that
holds a projected dispatch, so `lang_rs` walks every namespace to find it and to list
the modules under it, and `amc.rs_root` refuses a second namespace holding one.

**A refused schema reaches no language emitter.**  The emitters walk every field the
checks admit, and a field the checks refused has no form for an emitter to write, so
walking it can crash `amc`.  `gen_check_proj` runs after every check of the schema's
fields and records `amc.FDb.proj_refuse` when any of them refused, and each
`lang_<lang>` then writes nothing.  The `ns_check_*` steps that run later judge the C++
output and nothing an emitter walks.  An emitter may therefore assume that every field
it sees has a form in its language.

**A Bheap pays at run time only for the head it has.**  `nhead`, from `dmmeta.bheap`, is
known when amc runs, so a Bheap without a row, or with `nhead:1`, generates the plain binary
heap byte for byte, and the run's fields and helpers exist only for `nhead` of 2 or more.
A change to the head's code leaves every generated file of an `nhead:1` heap unchanged, and
the diff of the generated tree shows it.

**A head sorts the rows below the root, and the root bounds the heap.**  The run holds up to
`nhead` rows in ascending order and ends at the root, and every row of the heap below the
root is at least the root.  Each mutation keeps both halves: an insert below the root goes
into the run, demoting the root when the run is full, and anything else goes to the heap.
The callbacks match those of a heap without a head.  Insert fires OnXref, Remove fires
OnUnref, and Insert, Remove, RemoveFirst, RemoveAll, Reheap and ReheapFirst fire
FirstChanged when the first row changes.  Reheap of a new row, RemoveFirst, RemoveAll and
Cascdel fire neither OnXref nor OnUnref.  The internal `InsertImpl` and `RemoveImpl` fire
nothing.  The sorted cursor `curs` reads the run in place and never writes to the heap.
The opt-in `fillcurs` promotes heap rows into the run as it walks, so it writes to the
heap, and nothing else may walk a Bheap while a `fillcurs` walk of it is active.

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
