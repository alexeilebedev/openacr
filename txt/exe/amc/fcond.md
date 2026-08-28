## fcond: list membership tied to a condition on a field
<a href="#fcond-list-membership-tied-to-a-condition-on-a-field"></a>
`dmmeta.fcond` ties membership in an index to a condition on a field.
One row declares: while this condition holds, this record (or a
related record) belongs on this list.  Nothing else needs to
remember — the code that knows when the condition flips performs the
membership move.

Which conditions a field has depends on its reftype.  A `Val` field's
conditions are its values, so a row on a `Val` field names a constant
and the field's generated `_Set` maintains the membership on every
value transition.  Every other reftype has a condition vocabulary of
its own — see [Conditions of a reftype](#conditions-of-a-reftype).

The mechanism exists to compile the *temporal* half of a record's
lifecycle into data.  A record that lives through a state machine —
a publish request, a read subscription, an I/O buffer — spends its
life waiting on conditions, and each condition is registered by
membership in a list that some step drains or some event walks.
Writing the registration by hand scatters it: the insert lives in
one module, the remove in another, and nothing checks the pairing.
`fcond` puts the pairing in the schema.  What becomes data is only
the waiting: which condition a record waits on, and which list that
membership registers it in.  What the machine decides, and in what
order it moves through its states, stays a hand-written function,
because that is the part a reader has to be able to follow.

### The record
<a href="#the-record"></a>

```ssim
dmmeta.fcond  fcond:atf_amc.FCondtest.state/run  ins:atf_amc.FDb.zd_condtest_run  via:""  rem:Y  comment:""
```

- **fcond** — primary key: `<field>/<condition>`.  The field names an
  existing `dmmeta.field`.  On a `Val` field the condition is a
  constant of that field's type: when the field's arg carries
  `fconst` records, one of the fconst names; a `bool` field takes
  `true` or `false`; an integer field takes a literal.  On a field of
  any other reftype the condition is the name of an `amcdb.tcond` of
  that reftype's tclass.
- **ins** — the index the record is inserted into when the condition
  *begins to hold* (a no-op if already present).  The index lives
  on the namespace global (`FDb`), and its element type must match
  the record that gets inserted — the fcond field's ctype, or the
  `via` target.  The reftype of the index is the user's choice of
  scheduling discipline: an `Llist` drained by an `fstep` is a work
  queue or a rotation; a plain `Llist` walked by an event handler is
  a wait list; a `Bheap` keyed by a deadline gives timed wakeup.
- **via** — optional pointer hop.  When empty, the record carrying
  the field is itself inserted.  When set, it names an `Upptr` field
  on the same ctype, and the *pointed-to* record is inserted: a
  child whose state parks its parent queue declares
  `via:<ns>.FChild.p_parent`.  A NULL pointer skips the operation.
- **rem** — `Y`: when the condition *stops holding*, remove from the
  index (a no-op if absent).

Every row inserts — a row that never inserted would tie nothing to
the condition and might as well not exist — so `rem` alone
distinguishes the two kinds, level versus edge.  `rem:Y` ties
membership to the condition — a mirror, checkable record-by-record.
`rem:N` ties only insertion to the condition — a work queue armed by
the rising edge and drained by its consumer (`RemoveFirst` in a step,
typically).

### Generated code for a Val field
<a href="#generated-code-for-a-val-field"></a>

For a `Val` field carrying fcond rows, amc generates (or extends)
`<name>_Set(parent, rhs)`:

- the assignment happens only on change; a same-value `Set` performs
  no list operations (conditions are level-triggered, and the level
  did not change);
- for the old value, each matching row with `rem:Y` removes;
- for the new value, each matching row inserts;
- rows with `via` apply the operation to the pointed-to record when
  the pointer is non-NULL.

Several rows may name the same ins index (several values arm one
queue), and several fields may feed indices on the same record; one
(field, value) pair carries at most one row — that is its primary
key.  Direct assignment to the field bypasses maintenance — write
through `_Set`.  The enum convenience setters compose: `_SetEnum` and
the string-parsing `_SetStrptrMaybe` route through `_Set`, so no
generated path skips the dispatch.  Record deletion needs no special
handling for the record's own membership: the pool's delete removes
it from every index it occupies.  A `via` row is the exception —
deleting a record whose field still holds the watched value leaves
the *pointed-to* record on the list, so a record that parks its
parent leaves the watched value (or the caller adjusts the parent)
before deletion.

The initial value decides the initial membership of a `Val` field's
rows: the record's
generated `XrefMaybe` — the function that enters a new record into
its indices — carries one guarded insert per fcond row, applied
when the watched field holds the row's value at xref time.  A record
born in a registered state (the field's default, or a value the
creator sets before `XrefMaybe`) is on its list without any manual
arm; a value set through `_Set` before `XrefMaybe` counts the same
way, since the xref-time insert is an idempotent no-op.  Insertion
therefore has exactly one owner — the fcond declaration — and the
ins index's own `dmmeta.xref` must carry `inscond:false`: an
unconditional xref-time insert would be a second writer, entering
every record regardless of its value (and, for a `via` row, firing
at the *element* ctype's creation, unrelated to the watched
predicate).  amc refuses a row whose ins index xref says anything
but `inscond:false`.

### Conditions of a reftype
<a href="#conditions-of-a-reftype"></a>

A value is not the only thing a record can be waiting on.  An output
buffer's producer waits for the buffer to drain; the buffer's
occupancy is a continuous quantity crossing a mark, and no assignment
to a field expresses the moment it crosses.  The generator that owns
the buffer is the one piece of code that observes the crossing.

Generalize: for every reftype other than `Val`, the events worth
waiting on are the ones its own generated code produces, and the
generator is where the edge is known.  Each tclass therefore declares
the conditions it can report, as `amcdb.tcond` records keyed
`<tclass>.<name>` — the same shape as the `amcdb.tfunc` records that
declare the functions a tclass generates:

```ssim
amcdb.tcond  tcond:Fbuf.ready  comment:"the buffer has work: bytes to write out, or a message to read"
amcdb.tcond  tcond:Fbuf.eof    comment:"the buffer reached end of life: no more data will ever pass through it"
amcdb.tcond  tcond:Fbuf.space  comment:"a congested out buffer drained past its low-water mark"
```

An fcond row on such a field names one of them, and the split of
responsibility is: fcond owns the membership — which index, through
which pointer, removed on the falling edge or not — while the tclass
owns when the condition flips.

```ssim
dmmeta.fcond  fcond:<ns>.FConn.out/ready  ins:<ns>.FDb.cd_conn_outflow  via:""  rem:Y  comment:""
dmmeta.fcond  fcond:<ns>.FConn.out/space  ins:<ns>.FDb.cd_session_space  via:<ns>.FConn.p_session  rem:N  comment:""
```

A condition with no fcond row is not reported at all: the generator
looks the row up and emits nothing where it is absent, so declaring
the row is what turns the condition on.  The generated symbols
(`amc::amcdb_tcond_Fbuf_ready` and friends) are how a generator names
the condition it is about to report, and asking for a condition of
another tclass fails at generation time rather than silently emitting
nothing.

There is no xref-time insert for these rows.  A value has an initial
value to read; a condition of a reftype does not — a buffer that
starts empty simply never flipped — so membership begins empty and
the first edge arms it.

### Constraints checked
<a href="#constraints-checked"></a>

- on a `Val` field, the condition must be an fconst of the watched
  field (its own fconsts, or its arg ctype's when the arg is an
  enum-wrapper ctype), `true`/`false` for a bool, or an integer
  literal; and the field must be a plain `Val` — not a computed field
  (`substr`/`cppfunc`), not big-endian, not a pmask member;
- on a field of any other reftype, the condition must be an
  `amcdb.tcond` of that reftype's tclass;
- `ins` must name an `Llist` or `Bheap` on the namespace global
  (`FDb`) of the watched field's namespace, and its element type
  must match the inserted record's type;
- `via`, when set, must be a `Ptr` or `Upptr` field on the watched
  ctype, and its target type must match the ins index's element
  type;
- the ins index's `dmmeta.xref` (when one exists) must carry
  `inscond:false` — fcond owns insertion, birth included.

### Relation to the xref insert condition
<a href="#relation-to-the-xref-insert-condition"></a>

`dmmeta.xref` carries an insert condition of its own: `inscond`, an
arbitrary C++ expression evaluated when the record is x-referenced,
gating the insert into that index.  It is the same concept as fcond —
index membership tied to a predicate — with the predicate left
anonymous.  Because the predicate is an expression rather than a
declared field and value, no setter can maintain it: for most
reftypes it is evaluated once, at xref time, and a later change in
its truth silently strands the membership.  (`Bheap` is the
exception: the generated sort-key `Set` re-evaluates the condition
and reheaps or removes, so the same attribute is one-shot on one
reftype and maintained on another.)  In exchange, `inscond` covers
index kinds fcond does not yet reach — `Thash`, `Ptr`, indexes on
ctypes other than the namespace global.  Where the predicate has the
form field == value over a declared field and the index is one fcond
supports, prefer the fcond row: the condition becomes a fact the
schema checks, the setter maintains, and a command can display.

### Example: a two-list lifecycle
<a href="#example-a-two-list-lifecycle"></a>

The unit-test fixture (`amctest:Fcond%`, cpp/atf_amc/fcond.cpp): a
record with a state field (fconsts `idle`, `run`, `done`) that must
sit on a rotation while running, a bool `ready` that arms a work
queue, and a `done` state that parks the record's parent:

```ssim
dmmeta.fcond  fcond:atf_amc.FCondtest.state/idle  ins:atf_amc.FDb.zd_condtest_idle   via:""  rem:Y  comment:"mirror of the default value: membership starts at birth"
dmmeta.fcond  fcond:atf_amc.FCondtest.state/run   ins:atf_amc.FDb.zd_condtest_run    via:""  rem:Y  comment:"mirror: on the list iff running"
dmmeta.fcond  fcond:atf_amc.FCondtest.state/done  ins:atf_amc.FDb.zd_condpar_done    via:atf_amc.FCondtest.p_parent  rem:Y  comment:"via mirror: parent listed while child done"
dmmeta.fcond  fcond:atf_amc.FCondtest.ready/true  ins:atf_amc.FDb.cd_condtest_ready  via:""  rem:N  comment:"queue: armed on rising edge only"
```

`state_Set(rec, atf_amc_FCondtest_state_run)` inserts into the
mirror; any later `state_Set` away from `run` removes.
`ready_Set(rec, true)` arms the queue; `ready_Set(rec, false)`
leaves the queue untouched — the step that drains it is the only
remover.
