## amc Feature: Compile-time size & offset assertions
<a href="#amc-feature-compile-time-size-offset-assertions"></a>

A wire protocol's value is its byte layout — change a struct
field's size or move it by even one byte and every counterpart
that reads or writes the protocol starts producing garbage.
amc has three ssim tables that pin that layout at compile
time:

| Table             | What it asserts                                 |
|-------------------|--------------------------------------------------|
| `dmmeta.csize`    | The total size of a ctype in bytes.              |
| `dmmeta.fldoffset`| The offset of a named field within a ctype.     |
| `dmmeta.ctypelen` | The calculated length / alignment / pad-byte count of a ctype.  Auto-populated by amc, used by `Opt`/`Varlen` sizing. |

amc emits a `StaticCheck()` function per namespace that
materializes each of these assertions as an `algo_assert` —
a compile-time `static_assert` against `sizeof` and
`_offset_of`.  Anything that drifts breaks the build.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [csize](#csize)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [fldoffset](#fldoffset)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [ctypelen](#ctypelen)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated code](#generated-code)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### csize
<a href="#csize"></a>

`dmmeta.csize  ctype:<T>  size:<bytes>  alignment:<bytes>`
pins the size and alignment of any ctype — usually one
**not** declared by amc (an external `cextern`-typed alias, a
sub-struct from a C library).  amc compares `size` against
`sizeof(T)` in C++ and generates an `algo_assert`:

```c++
algo_assert(sizeof(<T>) == <size>);
algo_assert(_alignment_of(<T>) == <alignment>);
```

This is amc's only way to know how big a `LEVELDB` handle is
or what `MYSQL_RES` looks like — anything that doesn't itself
get parsed by amc must come with a csize record.

For ctypes amc *does* declare, csize is computed from
`gen_compute_size` (see `cpp/amc/size.cpp`); the
hand-written rows are for foreign types.

### fldoffset
<a href="#fldoffset"></a>

`dmmeta.fldoffset  field:<full>  offset:<bytes>` pins the
in-struct offset of a single field.  amc emits:

```c++
algo_assert(_offset_of(<Parent>, <field>) == <offset>);
```

(`<field>_be` for `fbigend` fields.)  Reordering a field, or
accidentally inserting a field higher up, will trip this check
and fail the build.

### ctypelen
<a href="#ctypelen"></a>

`dmmeta.ctypelen` is **calculated by amc**, not handwritten:

| Attribute   | Source |
|-------------|--------|
| `len`       | Sum of field sizes, with alignment padding. |
| `alignment` | Maximum alignment among fields. |
| `padbytes`  | Total bytes of padding inserted. |
| `plaindata` | True iff the struct's value is its bytes, so a copy may be a memcpy. |

amc inserts a `dmmeta.ctypelen` row for every ctype it sees;
downstream consumers (`Opt`, `Varlen`, the wire-format
codecs) use it to compute message sizes without including the
ctype's header.

### Generated code
<a href="#generated-code"></a>

`StaticCheck()` (one per namespace, declared `priv`) collects
every assertion in the namespace.  See
`tfunc_Ns_StaticCheck` in `cpp/amc/protocol.cpp`:

```c++
static void <ns>::StaticCheck() {
    // csize / ctypelen
    algo_assert(sizeof(prot::MsgHeader) == 4);
    algo_assert(_alignment_of(prot::MsgHeader) == 1);
    // fldoffset for prot::MsgHeader.type
    algo_assert(_offset_of(prot::MsgHeader, type) == 0);
    algo_assert(_offset_of(prot::MsgHeader, length) == 1);
    // ...
}
```

`StaticCheck` is called from each namespace's static
initializer — failing assertions fail the C++ build, not at
runtime.

For each `Opt` field, amc also emits a `SizeOf*` enum that
encodes the **fixed portion's size**:

```c++
enum { _SizeOf_prot_DataMsg = 4 };
```

These are placed in a per-namespace `_size_enums` block so
consumers can know the size of a message without including
its header.  Cross-namespace `Opt` fields are why `ctypelen`
must be set on the target type.

### Pitfalls
<a href="#pitfalls"></a>

- **csize is mandatory for cextern ctypes.**  amc has no way
  to know the size of types it didn't generate.  Forgetting
  the csize record yields `amc.nolength` errors when those
  types are used inside `Opt` / `Varlen`.
- **fldoffset accumulates.**  Removing a fldoffset row does
  not undo the prior assertion — you must remove both the row
  and the field, or the build still fails at the now-stale
  offset.
- **Pack affects offsets.**  Without `dmmeta.pack`, the
  compiler may insert padding bytes, breaking fldoffset
  assertions.  Pin layout with `pack` (or `nsx.pack:Y` for
  the whole namespace) before adding fldoffset rows.
- **`_offset_of` is the macro form.**  amc uses
  `_offset_of(struct, field)` rather than the C `offsetof`
  to avoid macro name clashes in some clang environments.
- **ctypelen is calculated, not declared by hand.**  Don't
  edit `data/dmmeta/ctypelen.ssim` directly — `amc` rewrites
  it on every run.
- **Sub-byte layout (Bitfld) is not validated** by fldoffset.
  Use `csize` on the host integer and trust amc's bit
  arithmetic.

### See also
<a href="#see-also"></a>

- [Protocols](/txt/exe/amc/proto.md) — typical context
- [Big-endian fields](/txt/exe/amc/bigendian.md) — pairs with offset assertions
- [Opt](/txt/exe/amc/reftype/Opt.md) / [Varlen](/txt/exe/amc/reftype/Varlen.md) — consumers of ctypelen
- [Backend / pipeline](/txt/exe/amc/backend/pipeline.md) — `gen_compute_size`, `gen_ns_size_enums`
- Source: `cpp/amc/size.cpp`, `cpp/amc/protocol.cpp`
- Spec tables: `acr 'dmmeta.csize:%'`, `acr 'dmmeta.fldoffset:%'`, `acr 'dmmeta.ctypelen:%'`

### Example
<a href="#example"></a>

Pinning a foreign struct's size:

```
dmmeta.ctype  ctype:MYSQL_RES  comment:"mysql_res_struct"
  dmmeta.cextern  ctype:MYSQL_RES
  dmmeta.csize    ctype:MYSQL_RES  size:120  alignment:8
```

Pinning fields of a wire-format message:

```
dmmeta.ctype     ctype:prot.MsgHeader
  dmmeta.pack    ctype:prot.MsgHeader
  dmmeta.field   field:prot.MsgHeader.type   arg:u8  reftype:Val
    dmmeta.fldoffset  field:prot.MsgHeader.type  offset:0
  dmmeta.field   field:prot.MsgHeader.length arg:u8  reftype:Val
    dmmeta.fldoffset  field:prot.MsgHeader.length  offset:1
```

After amc, the namespace's `StaticCheck()` includes
`algo_assert(_offset_of(prot::MsgHeader, type) == 0)` and
`algo_assert(_offset_of(prot::MsgHeader, length) == 1)`.
Reorder the fields and the build dies.
