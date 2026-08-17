## amc Reftype: ZSListMT
<a href="#amc-reftype-zslistmt"></a>

`ZSListMT` is a **lock-free single-producer / multi-consumer
zero-terminated queue**.  Writers push elements onto a
thread-shared singly-linked list via an atomic compare-and-
swap; readers drain the list with an atomic swap that reverses
the chain into a private read list.  No mutex, no
ring-buffer — just two pointer fields per queue and one
`next` pointer per element.

Use ZSListMT when one thread produces work items consumed by
another (the canonical case: a worker thread offloading
results onto an event-loop thread).

```
dmmeta.field  field:lib_kafka.FDb.zs_msg  arg:lib_kafka.FMsg  reftype:ZSListMT  dflt:""  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype
(see `tclass_ZSListMT` in `cpp/amc/llist.cpp`):

| Field          | Type   | Meaning                                                 |
|----------------|--------|---------------------------------------------------------|
| `<name>_head`  | `T*`   | reader-side list head (no atomic — owned by the reader) |
| `<name>_mt`    | `T*`   | writer-side list head (atomic CAS for inserts)          |

State in the **element** ctype:

| Field             | Type | Meaning                                          |
|-------------------|------|--------------------------------------------------|
| `<xfname>_next`   | `T*` | next element; `(T*)-1` while not in any list     |

### How it works
<a href="#how-it-works"></a>

- **Insert (any thread)**: prepend the new element to `_mt`
  using a CAS loop:
  ```
  do {
      tmp = _mt;
      row.next = tmp;
  } while (!CAS(&_mt, tmp, &row));
  ```

- **DestructiveFirst (reader thread only)**: if `_head` is
  empty, atomically grab `_mt` (`__sync_lock_test_and_set`),
  reverse the chain into `_head`, and return the new head.
  The reverse step transforms the writer-LIFO into reader-FIFO
  order.

- **RemoveFirst**: `DestructiveFirst`, then pop the head.

- **Remove(row)**: walk the `_head` chain to splice out `row`.
  Only valid from the reader; writer-side never removes.

### When to use it
<a href="#when-to-use-it"></a>

| Use ZSListMT when… | Use plain Llist (`zs`) when… |
|--------------------|------------------------------|
| Producer and consumer are different threads | Single-threaded |
| Producer is willing to do an atomic CAS per insert | Need O(1) removal of arbitrary elements |
| FIFO order is acceptable (reversed at drain time) | Need head/tail access without draining |

ZSListMT does **not** support: `Last`, `Prev`, `RotateFirst`,
`fstep` ("ZSListMT doesn't support fstep" enforced in
`gen.cpp`), or `havecount`.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:ZSListMT`.  `arg:` is the element
  ctype.  The field name must start with `zs_`, a prefix shared with
  Llist — which means `acr_ed` infers Llist from the name and ZSListMT
  has to be named explicitly
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).

No `dmmeta.llist` record applies — ZSListMT is its own
reftype with a fixed shape.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/llist.cpp` (search for `ZSListMT`).

| Tfunc                            | Generated function                              | Effect |
|----------------------------------|-------------------------------------------------|--------|
| `ZSListMT.Init`                  | (folded)                                        | NULL `_head` and `_mt`. |
| `ZSListMT.InLlistQ`              | `bool <xfname>_InLlistQ(T& row)`                | `row.<xfname>_next != (T*)-1`. |
| `ZSListMT.Insert`                | `void <name>_Insert(P&, T& row)`                | CAS-prepend `row` onto `_mt`.  Row must not already be in the list (UB if it is). |
| `ZSListMT.DestructiveFirst`      | `T* <name>_DestructiveFirst(P&)`                | If `_head` is empty, drain `_mt` (atomic swap + reverse) into `_head`; return current head. **Reader-thread only.** |
| `ZSListMT.RemoveFirst`           | `T* <name>_RemoveFirst(P&)`                     | Pop the head; calls `DestructiveFirst` first. |
| `ZSListMT.Remove`                | `void <name>_Remove(P&, T& row)`                | Reader-side: walk `_head` and unlink. |

There is no `_First` (without draining you can't see writer-
side entries), no `_Last`, no `_N`.

### Memory model
<a href="#memory-model"></a>

- Writers issue exactly one CAS per insert.
- Readers do exactly one atomic swap per drain.
- The list is **single-producer / multi-consumer** in name
  ("MT" = multi-thread), but the read side is **not**
  thread-safe with itself — only one thread should call
  `DestructiveFirst` / `RemoveFirst` / `Remove`.  Multiple
  writers are fine (the CAS handles contention).
- Element storage must live in stable memory
  ([Lary](/txt/exe/amc/reftype/Lary.md) or similar).

### Pitfalls
<a href="#pitfalls"></a>

- **Inserting an already-in-list row is UB.**  ZSListMT does
  not check the sentinel before inserting (unlike Llist).
  Confirm `_InLlistQ(row) == false` first if you can't
  guarantee single insertion by construction.
- **Reader is single-threaded.**  Multiple readers calling
  `DestructiveFirst` / `RemoveFirst` race on `_head`.
- **No `fstep`.**  amc rejects fstep on ZSListMT fields.
- **Order is FIFO** because `DestructiveFirst` reverses the
  writer LIFO into reader order.  Don't rely on insertion
  order being preserved before drain.
- **Element's `_Init` must run.**  Sets `next = (T*)-1`.
- **`Remove(row)` walks the chain.**  O(n) cost; usually
  `RemoveFirst` is what you want.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Llist](/txt/exe/amc/reftype/Llist.md) — single-threaded sibling
- [Runtime](/txt/exe/amc/runtime.md) — typical drain pattern from a step
- Source: `cpp/amc/llist.cpp` (search for `ZSListMT`)
- Tfunc records: `acr 'tfunc:ZSListMT.%'`

### Example
<a href="#example"></a>

Worker thread pushes completed jobs; the event loop drains
them on its scheduler step:

```c++
// worker thread:
lib_kafka::FMsg &m = ...;
zs_msg_Insert(_db, m);            // CAS

// event-loop thread (drain everything on each step):
while (lib_kafka::FMsg *m = zs_msg_RemoveFirst(_db)) {
    process(*m);
}
```
