## amc Reftype: Tary
<a href="#amc-reftype-tary"></a>

`Tary` is a dynamic, contiguous, resizable array of values —
roughly the equivalent of C++'s `std::vector<T>`.  All elements
live in a single heap block; growth is by powers of two
(amortized O(1) `Alloc`).  Records held in a `Tary` **cannot be
cross-referenced** because growing the array moves elements to a
new buffer; amc enforces this at code-generation time.

```ssim
dmmeta.field  field:algo.LineBuf.buf  arg:char  reftype:Tary  dflt:""  comment:""
  dmmeta.tary  field:algo.LineBuf.buf  aliased:Y  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

Three storage fields are inserted into the parent ctype
(see `tclass_Tary` in `cpp/amc/tary.cpp`):

| Field            | C++ type    | Meaning                                       |
|------------------|-------------|-----------------------------------------------|
| `<name>_elems`   | `T*`        | pointer to the heap block holding the values  |
| `<name>_n`       | `u64`       | number of constructed elements                |
| `<name>_max`     | `u64`       | capacity in elements before the next realloc  |

The first allocation grows `max` to 4, after which each grow
doubles the capacity (see `tfunc_Tary_AbsReserve` in
`cpp/amc/tary.cpp`).  Reallocation is done via the field's
**base pool** (`dmmeta.basepool`) using
`<basepool>_ReallocMem(...)`; if that returns NULL, the process
dies with `FatalErrorExit("$ns.tary_nomem ...")`.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Tary`.  `arg:` is the element
  type (built-in like `char`, `u8`, `i32`, or any other ctype).
- `dmmeta.basepool` if the namespace does not have a default
  pool — Tary allocates through this pool.  Most projects let
  the per-namespace default in `dmmeta.nsx.pool` drive this.

Optional:

- `dmmeta.tary  field:<name>  aliased:Y` — generates the
  `aryptr`-flavored variants (`Addary`, `Setary` (`aryptr` rhs),
  `Insary`).  These accept `algo::aryptr<T>` and check at runtime
  that the caller did not pass a sub-range of the same array
  (which would be invalidated by the implicit `Reserve`).  An
  alias violation is fatal — there is no automatic adjustment.

Constraint: the element type **must have `n_xref == 0`** (no
cross-references point into rows of this type).  Violations
abort amc with a clear message suggesting `Lary` instead — see
`tclass_Tary` lines 38–48.

### Memory model
<a href="#memory-model"></a>

- Single contiguous block, owned by the parent struct.
- `Init` zeros `elems`/`n`/`max` (`tfunc_Tary_Init` at line 260).
- `Uninit` destructs each live element (when `T` has a non-
  trivial destructor) and frees the block back to the base pool.
- **Reserve** grows the capacity to at least
  `current_n + n`; **AbsReserve** grows to at least `n`.  Both
  may move memory — any pointer obtained earlier from
  `_elems`, `_qFind`, `_Last`, `_Getary`, or any cursor is
  invalidated after a grow.
- Element constructors run in-place via placement `new`; element
  destructors are invoked at `Remove` / `RemoveLast` /
  `RemRegion` / `Uninit` time (only when the element type has a
  destructor, see `HasDtorQ`).

### Generated functions
<a href="#generated-functions"></a>

Every tfunc whose tclass is `Tary`, with the prototype amc emits
for a field declared as `T <name>` in parent `P`.  Function
templates live in `cpp/amc/tary.cpp`; tfunc records live in
`ssimfile:amcdb.tfunc`.

#### Lifecycle
<a href="#lifecycle"></a>

| Tfunc          | Generated function                                | Effect |
|----------------|---------------------------------------------------|--------|
| `Tary.Init`    | `<name>_Init(P&)` (macro, inline)                 | Zero elems/n/max. |
| `Tary.Uninit`  | `<name>_Uninit(P&)` (macro)                       | Destruct + free buffer. |

#### Capacity
<a href="#capacity"></a>

| Tfunc             | Generated function                          | Effect |
|-------------------|---------------------------------------------|--------|
| `Tary.N`          | `i64 <name>_N(const P&)`                    | Number of elements. |
| `Tary.Max`        | `i64 <name>_Max(P&)`                        | Capacity (without reallocating). |
| `Tary.EmptyQ`     | `bool <name>_EmptyQ(P&)`                    | True if `N() == 0`. |
| `Tary.Reserve`    | `void <name>_Reserve(P&, i64 n)`            | Ensure room for `n` **more** elements. |
| `Tary.AbsReserve` | `void <name>_AbsReserve(P&, i64 n)`         | Ensure capacity ≥ `n` absolute. Doubles, never shrinks. |

#### Single-element insertion
<a href="#single-element-insertion"></a>

| Tfunc              | Generated function                                   | Effect |
|--------------------|------------------------------------------------------|--------|
| `Tary.Alloc`       | `T& <name>_Alloc(P&)`                                | Append a default-constructed element, return reference. |
| `Tary.AllocAt`     | `T& <name>_AllocAt(P&, i64 at)`                      | Insert default-constructed element at index `at`; fatal if `at` out of range. |

#### Bulk insertion
<a href="#bulk-insertion"></a>

| Tfunc                | Generated function                                                  | Effect |
|----------------------|---------------------------------------------------------------------|--------|
| `Tary.AllocN`        | `aryptr<T> <name>_AllocN(P&, i64 n)`                                | Append `n` default-constructed elements. |
| `Tary.AllocNAt`      | `aryptr<T> <name>_AllocNAt(P&, i64 n, i64 at)`                      | Insert `n` default-constructed elements at index `at`. |
| `Tary.AllocNVal`     | `aryptr<T> <name>_AllocNVal(P&, i64 n, const T& val)`               | Append `n` copies of `val`. |
| `Tary.Addary`        | `aryptr<T> <name>_Addary(P&, aryptr<T> rhs)` *(if `aliased:Y`)*     | Append every element of `rhs`. Fatal if `rhs` aliases the array. |
| `Tary.Insary`        | `void <name>_Insary(P&, aryptr<T> rhs, i64 at)` *(if `aliased:Y`)*  | Insert `rhs` at index `at`. Fatal on alias. |

#### Replacement
<a href="#replacement"></a>

| Tfunc            | Generated function                                          | Effect |
|------------------|-------------------------------------------------------------|--------|
| `Tary.Setary`    | `void <name>_Setary(P&, P& rhs)`                            | Copy elements from another struct of the same type. |
| `Tary.Setary2`   | `void <name>_Setary(P&, const aryptr<T>& rhs)` *(aliased:Y)* | Replace contents from `aryptr`. Fatal on alias. |
| `Tary.AssignAryptr` | `void operator=(const strptr&)` etc. *(generated as method)* | Assignment from `aryptr<T>` / `strptr` for char/u8 cases. |
| `Tary.CtorAryptr`   | Constructor from `aryptr<T>` / `strptr` (char/u8)         | Ctor variant for char/u8 arrays. |

#### Removal
<a href="#removal"></a>

| Tfunc              | Generated function                          | Effect |
|--------------------|---------------------------------------------|--------|
| `Tary.Remove`      | `void <name>_Remove(P&, u64 i)`             | Remove element at `i`; shifts tail left. No-op if `i` out of range. |
| `Tary.RemoveLast`  | `void <name>_RemoveLast(P&)`                | Pop. No-op if empty. |
| `Tary.RemoveAll`   | `void <name>_RemoveAll(P&)`                 | Set `n=0`, destruct each element. |
| `Tary.RemRegion`   | `void <name>_RemRegion(P&, i64 beg, i64 n)` | Remove `[beg, beg+n)`; clipped to valid range. |

#### Access
<a href="#access"></a>

| Tfunc            | Generated function                          | Effect |
|------------------|---------------------------------------------|--------|
| `Tary.Find`      | `T* <name>_Find(P&, u64 t)`                 | Pointer to element `t`, or NULL if out of range. |
| `Tary.qFind`     | `T& <name>_qFind(P&, u64 t)`                | "Quick" find — no bounds check. |
| `Tary.Last`      | `T* <name>_Last(P&)`                        | Pointer to last element, or NULL if empty. |
| `Tary.qLast`     | `T& <name>_qLast(P&)`                       | Reference to last element — no bounds check. |
| `Tary.Getary`    | `aryptr<T> <name>_Getary(const P&)`         | Return `(elems, n)` as an `aryptr`. |
| `Tary.rowid_Get` | `u64 <name>_rowid_Get(P&, T& elem)`         | Index of `elem` (must be a member of the array). |
| `Tary.RowidFind` | Internal `ImrowPtr` for reflection          | Only if `field.reflect` set. |
| `Tary.curs`      | `<P>_<name>_curs` cursor + `_Reset/_ValidQ/_Next/_Access` | `ind_beg`-compatible iterator. |

#### Comparison
<a href="#comparison"></a>

| Tfunc        | Generated function              | Effect |
|--------------|---------------------------------|--------|
| `Tary.Eq`    | `bool <name>_Eq(P&, P&)`        | Element-wise equality. Requires elements to be comparable. |
| `Tary.Cmp`   | `i32 <name>_Cmp(P&, P&)`        | Lexicographic compare. |

#### Serialization
<a href="#serialization"></a>

| Tfunc                  | Generated function                                          | Effect |
|------------------------|-------------------------------------------------------------|--------|
| `Tary.Print`           | `void <name>_Print(P&, cstring& out)`                       | For `char`/`u8` arrays — print as string. Otherwise prints separator-joined elements. |
| `Tary.ReadStrptrMaybe` | `bool <name>_ReadStrptrMaybe(P&, strptr in)`                | For `char`/`u8` — assign from string.  For other types with a separator — split and parse. |

#### Streaming append (char/u8 only)
<a href="#streaming-append-char-u8-only-"></a>

| Tfunc                       | Generated function                                       | Effect |
|-----------------------------|----------------------------------------------------------|--------|
| `Tary.GetAlloc`             | `algo::Alloc <name>_GetAlloc(P&)`                        | Returns an `Alloc` handle that **clears then fills** the buffer. |
| `Tary.BeginAlloc`           | `void* <name>_BeginAlloc(P&, i64 len)`                   | Internal helper used by `GetAlloc`. |
| `Tary.GetAllocAppend`       | `algo::Alloc <name>_GetAllocAppend(P&)`                  | Handle that **appends** rather than clearing first. |
| `Tary.BeginAllocAppend`     | `void* <name>_BeginAllocAppend(P&, i64 len)`             | Internal helper used by `GetAllocAppend`. |

### Pitfalls
<a href="#pitfalls"></a>

- **Pointer invalidation on grow.** Any pointer obtained from
  `_qFind`, `_Last`, `_Getary`, or a cursor is invalid after the
  next `_Alloc`/`_Reserve`-class call that grows the buffer.  If
  you need stable pointers, use `Lary` instead.
- **No cross-references.** Defining an `xref` whose parent type
  is allocated via `Tary` is a hard amc error.  This is by
  design — `xref` requires permanent pointers, which `Tary`
  cannot offer.
- **Aliasing.** `Addary`/`Setary`/`Insary` will abort the process
  if `rhs` overlaps the parent array.  If you really want to copy
  a sub-range to the end, first copy it to a separate buffer.
- **Doubling capacity.** Capacity only grows; it never shrinks
  automatically.  `RemoveAll` keeps the existing buffer (just
  resets `n` to 0) — call `Uninit` then `Init` to actually free.
- **Default construction on `_Alloc`.** New elements are
  default-constructed with the field's `dflt` value.  If you
  need a different initializer, use `_AllocNVal` or write the
  fields after `_Alloc`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Lary](/txt/exe/amc/reftype/Lary.md) — when you need stable pointers
- [Inlary](/txt/exe/amc/reftype/Inlary.md) — when bounds are small and known
- [Memory pools](/txt/exe/amc/pool.md) — for the basepool that backs Tary
- Source: `cpp/amc/tary.cpp`
- Tfunc records: `acr 'tfunc:Tary.%'`

### Example
<a href="#example"></a>

A `LineBuf` whose `buf` field is `Tary<char>` is just a string
buffer with `Getary()` exposing the underlying chars:

```c++
algo::LineBuf lb;
buf_Addary(lb, algo::strptr("hello "));
buf_Addary(lb, algo::strptr("world"));
ind_beg(algo::LineBuf_buf_curs, ch, lb) {
    out << ch;        // 'h','e','l','l','o',' ','w','o','r','l','d'
} ind_end;
prlog(buf_Getary(lb));
```

A `Tary<T>` over a non-string type works the same — `_Alloc()`
returns a reference to a default-constructed element, and you can
write the fields directly:

```c++
acr::FQuery q;
acr::AttrRegx &w = where_Alloc(q);
w.attr  = "ctype";
w.regx  = "amc.%";
```
