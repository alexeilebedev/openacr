## amc Reftype: Llist
<a href="#amc-reftype-llist"></a>

`Llist` is a **cross-reference** index implemented as an
in-place linked list.  The list does not own its elements — it
threads them through `next` / `prev` pointers that amc adds to
the element's own ctype.  Insertion and removal are O(1) once
you have a pointer to the row; iteration is O(n) over the
chain.  Because the links live inside the element, a single
element can be on as many independent Llists as you declare.

```ssim
dmmeta.field  field:abt.FDb.zs_srcfile_read  arg:abt.FSrcfile  reftype:Llist  dflt:""  comment:""
  dmmeta.llist  field:abt.FDb.zs_srcfile_read  havetail:Y  havecount:N  listtype:zs  comment:""
```

### Listtype - eight flavors of linked list
<a href="#listtype"></a>

Three orthogonal booleans in `dmmeta.listtype` produce eight
named listtypes:

| listtype | circular | haveprev | instail | shape                              |
|----------|----------|----------|---------|------------------------------------|
| `zs`     | N        | N        | Y       | Zero-terminated singly-linked queue (head→…→NULL, insert at tail) |
| `zsl`    | N        | N        | N       | Zero-terminated singly-linked LIFO (head→…→NULL, push at head) |
| `zd`     | N        | Y        | Y       | Zero-terminated doubly-linked queue |
| `zdl`    | N        | Y        | N       | Zero-terminated doubly-linked LIFO |
| `cs`     | Y        | N        | Y       | Circular singly-linked queue (tail→head) |
| `csl`    | Y        | N        | N       | Circular singly-linked LIFO        |
| `cd`     | Y        | Y        | Y       | Circular doubly-linked queue       |
| `cdl`    | Y        | Y        | N       | Circular doubly-linked LIFO (stack) |

Reading the flags:

- **circular** — last element points back to the first.  Lets
  `Last()` and `RotateFirst()` work without a tail pointer.
- **haveprev** — each element has a `prev` link.  Required for
  O(1) `Remove(elem)`; without it, removal is only O(1) via
  `RemoveFirst()`.
- **instail** — `Insert()` appends at the tail (FIFO/queue
  behavior).  When false, `Insert()` pushes at the head
  (LIFO/stack).

The listtype is picked **by the field's name prefix** via the
`dmmeta.fprefix` map.  `zs_srcfile_read` → `zs` → singly-linked
zero-terminated queue.  This is why an Llist field's name
matters: rename a field from `zs_…` to `cd_…` and you have
changed it into a circular doubly-linked queue.

### What it generates
<a href="#what-it-generates"></a>

The list's own state goes into the **parent** ctype
(see `tclass_Llist` in `cpp/amc/llist.cpp`):

| Field (parent) | Type      | Always | Notes |
|----------------|-----------|--------|-------|
| `<name>_head`  | `T*`      | yes    | first element, NULL if empty |
| `<name>_tail`  | `T*`      | when `dmmeta.llist.havetail:Y` | last element, kept in sync by Insert/Remove |
| `<name>_n`     | `i32`     | when `dmmeta.llist.havecount:Y` | element count |

The link fields go into the **element** ctype (`field.p_arg`),
named after the `xfname` (cross-reference field name):

| Field (element)  | Type | Always       | Notes |
|------------------|------|--------------|-------|
| `<xfname>_next`  | `T*` | yes          | initialized to `(T*)-1` to mark "not in list"; NULL/head when in list |
| `<xfname>_prev`  | `T*` | if `haveprev` | back link |

The sentinel `(T*)-1` in `next` distinguishes "not currently
in this list" from "in the list at the tail" (where `next` is
NULL on a zero-terminated list).  `InLlistQ` tests this
sentinel.

Validity rules amc enforces at code-gen time:

- `instail:Y` on a non-circular list **requires** `havetail:Y`
  (`amc.notail` error).
- A circular list must have **exactly one** of `havetail` or
  `haveprev` — never both, never neither (`amc.badcirc`).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Llist`.  `arg:` is the element
  ctype; the **field name prefix** picks the listtype via
  `dmmeta.fprefix`.

- `dmmeta.llist` record with:
  - `havetail:Y/N` — keep an explicit `_tail` pointer (default Y)
  - `havecount:Y/N` — keep an explicit `_n` counter (default N)
  - `listtype:<key>` — usually omitted; defaulted from the
    field's prefix.

- `dmmeta.xref` if you want the list maintained automatically
  on row insertion / deletion.  Without an xref the list is
  manual: you call `Insert` / `Remove` yourself.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/llist.cpp`.  Some tfuncs are emitted
conditionally — gated by `listtype.haveprev`, `havetail`, etc.

| Tfunc                  | Generated function                                    | When emitted | Effect |
|------------------------|-------------------------------------------------------|--------------|--------|
| `Llist.Init`           | `<name>_Init(P&)` (macro)                             | always       | NULL out head/tail/n; mark every element's `next` as not-in-list (via the element ctype's own `Init`). |
| `Llist.EmptyQ`         | `bool <name>_EmptyQ(P&)`                              | always       | `head == NULL`. |
| `Llist.N`              | `i32 <name>_N(const P&)`                              | `havecount:Y`| Stored counter. |
| `Llist.First`          | `T* <name>_First(P&)`                                 | always       | Pointer to head; NULL if empty. |
| `Llist.Last`           | `T* <name>_Last(P&)`                                  | `havetail:Y` OR circular+`haveprev` | Pointer to last element; NULL if empty. |
| `Llist.qLast`          | `T& <name>_qLast(P&)`                                 | always       | Reference to last — no bounds check. |
| `Llist.Next`           | `T* <xfname>_Next(T& row)`                            | always       | Follow `next` link. |
| `Llist.Prev`           | `T* <xfname>_Prev(T& row)`                            | `haveprev:Y` | Follow `prev` link. |
| `Llist.InLlistQ`       | `bool <xfname>_InLlistQ(T& row)`                      | always       | `row.<xfname>_next != (T*)-1`. |
| `Llist.Insert`         | `void <name>_Insert(P&, T& row)`                      | always       | Add `row`.  `instail:Y` appends; `instail:N` pushes at head.  No-op if already in list. |
| `Llist.InsertBefore`   | `void <name>_InsertBefore(P&, T& row, T* before)`      | doubly-linked | Insert `row` ahead of `before`; a NULL `before` appends at the tail. |
| `Llist.Remove`         | `void <name>_Remove(P&, T& row)`                      | always       | Remove `row` from the list. O(1) if `haveprev:Y`; otherwise O(n). No-op if not in list. |
| `Llist.RemoveFirst`    | `T* <name>_RemoveFirst(P&)`                           | always       | Unlink and return head; NULL if empty. |
| `Llist.RemoveAll`      | `void <name>_RemoveAll(P&)`                           | always       | Unlink every element (clear `next`/`prev`), reset head/tail/n. |
| `Llist.RotateFirst`    | `T* <name>_RotateFirst(P&)`                           | circular only| Return head, then advance head to `head->next` — round-robin. |
| `Llist.Cascdel`        | (private)                                             | `cascdel:Y` on the xref | Called by row's `Delete`; pops first, deletes it, repeats. |
| `Llist.DestructiveFirst`| Reserved                                             | always       | Marker tfunc (no body) — reserved for atomic-pop variants. |
| `Llist.curs`           | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access`    | always       | Forward cursor used by `ind_beg`. |
| `Llist.delcurs`        | `<P>_<name>_delcurs` + ...                            | `dmmeta.fcurs` | "Delete cursor" — safe to call `_Remove` on the current row inside the loop.  Opt-in, see [cursors](/txt/exe/amc/reftype.md#cursors). |

### Memory model
<a href="#memory-model"></a>

- The list itself allocates nothing.  All storage is in the
  parent's two/three header fields and in each element's
  `next`/`prev` link fields.
- The element type's `Init` is amended by amc to set
  `next = (T*)-1` (the sentinel for "not in list").  Code that
  bypasses `Init` and zeroes elements with `memset` will leave
  them marked "in list with `next == NULL`" — a recipe for
  corruption.  Use `_Init` (or allocate via a pool, which calls
  `_Init` for you).
- An element can be on multiple Llists simultaneously — one
  pair of `next`/`prev` per list.
- A list cannot be "moved" — its head/tail pointers reference
  elements that must themselves live in stable memory (e.g.,
  [Lary](/txt/exe/amc/reftype/Lary.md)).  Holding an Llist
  over rows allocated via [Tary](/txt/exe/amc/reftype/Tary.md)
  is unsafe (and rejected when an xref is declared).

### Pitfalls
<a href="#pitfalls"></a>

- **Element's `_Init` must run** before the row can be touched
  by `Insert`.  If you `memset(row, 0, sizeof(row))` or
  otherwise bypass `_Init`, `next` will be `NULL` instead of
  the "not in list" sentinel `(T*)-1`, and `InLlistQ` will
  lie.  Always allocate via a pool (which calls `_Init`).
- **`Remove` without `haveprev` is O(n).**  Singly-linked
  removal has to walk from the head.  Use `RemoveFirst` for
  queues, or pick a `haveprev:Y` listtype if random removal
  matters.
- **No automatic list maintenance without xref.**  An Llist
  field is only auto-populated when a matching `dmmeta.xref`
  is defined.  Without an xref the list is manual.
- **One next/prev per list.**  A row can belong to multiple
  Llists, but each list needs its own dedicated `next`/`prev`
  field pair on the row — driven by the field-name prefix /
  `xfname`.
- **`instail` requires a tail.**  Zero-terminated tail-insert
  lists must set `havetail:Y` — amc will refuse otherwise.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [ZSListMT](/txt/exe/amc/reftype/ZSListMT.md) — lock-free zero-terminated queue
- [Cross-references](/txt/exe/amc/xref.md) — auto-insertion model
- [Bheap](/txt/exe/amc/reftype/Bheap.md) — when ordering matters
- Source: `cpp/amc/llist.cpp`
- Tfunc records: `acr 'tfunc:Llist.%'`
- Listtype catalog: `acr listtype:%`

### Example
<a href="#example"></a>

The `abt` build tool keeps a worklist of source files waiting
to be parsed.  The list lives in `FDb` as a zero-terminated
singly-linked queue:

```ssim
dmmeta.ctype  ctype:abt.FSrcfile
  dmmeta.field  field:abt.FSrcfile.zs_srcfile_read_next  arg:abt.FSrcfile  reftype:Upptr ...

dmmeta.field  field:abt.FDb.zs_srcfile_read  arg:abt.FSrcfile  reftype:Llist
  dmmeta.llist  field:abt.FDb.zs_srcfile_read  havetail:Y  havecount:N  listtype:zs
```

Usage:

```c++
zs_srcfile_read_Insert(srcfile);          // enqueue at tail
while (abt::FSrcfile *cur = zs_srcfile_read_RemoveFirst()) {
    // process cur
}
```

For a round-robin pattern (circular list), `RotateFirst` is
the idiom:

```c++
ind_beg(...) {
    abt::FCmd *cmd = cd_cmd_RotateFirst(parent); // pop+push back
    if (!cmd) break;
    ...
} ind_end;
```
