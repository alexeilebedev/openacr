## amc Feature: Bitsets — fbitset over integers or arrays
<a href="#amc-feature-bitsets-fbitset-over-integers-or-arrays"></a>

`dmmeta.fbitset` turns any integer field, inline array of
integers, or Tary of integers into a **bit vector** with O(1)
get/set/clear/scan accessors.  amc divides the bit index by
the element bit-width to find the right element, then
manipulates a single bit within it.  No extra storage is added
— the underlying integer storage *is* the bit vector.

```ssim
dmmeta.field      field:atf_amc.Bitset.fld64  arg:u64  reftype:Val  dflt:""  comment:""
  dmmeta.fbitset  field:atf_amc.Bitset.fld64
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [What it generates](#what-it-generates)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Single-value vs array bitsets](#single-value-vs-array-bitsets)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### What it generates
<a href="#what-it-generates"></a>

amc looks at `field.p_arg->c_csize` to determine the element
width (`u8` → 8, `u16` → 16, `u32` → 32, `u64` → 64), then
emits all bit-manipulation functions using
`bit_idx >> idxshift` and `bit_idx & shiftmask`.  For a `u64`
host: `idxshift = 6`, `shiftmask = 63`.

The accessor family is the same whether the underlying storage
is one integer, an `Inlary<N>` of integers, or a `Tary<u8>`.
amc just changes how it locates the element.

### Single-value vs array bitsets
<a href="#single-value-vs-array-bitsets"></a>

| Storage         | `_N` returns | `_qFind(i)` |
|-----------------|--------------|-------------|
| Single int (`Val u32` etc.) | 1            | the integer itself |
| `Inlary<N>` of int          | `N` (compile-time) | element `i` |
| `Tary` of int               | `<name>_n`   | element `i` |

For single-value bitsets, amc also emits a trivial `_N` and
`_qFind` so the rest of the bit-access functions can be
shared.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` of an integer type (`u8`/`u16`/`u32`/`u64`) —
  as `Val`, `Inlary`, or `Tary`.
- `dmmeta.fbitset  field:<full>` — the marker record.

The integer type's `dmmeta.csize` must be set (it always is for
the built-in ints) so amc knows how many bits per element.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/bitset.cpp`.

#### Sizing
<a href="#sizing"></a>

| Tfunc                | Generated function                          | Effect |
|----------------------|---------------------------------------------|--------|
| `Bitset.N`           | `int <name>_N(P&)`                          | Element count.  For Val: 1. |
| `Bitset.NBits`       | `int <name>_Nbits(P&)`                      | `_N() * elem_bits` — total capacity. |
| `Bitset.Sup`         | `int <name>_Sup(P&)`                        | One past the highest set bit (i.e., enough to enumerate all 1s).  Useful for tight loops. |

#### Bit-level get / set
<a href="#bit-level-get-set"></a>

| Tfunc                | Generated function                              | Effect |
|----------------------|-------------------------------------------------|--------|
| `Bitset.qGetBit`     | `bool <name>_qGetBit(P&, u32 bit_idx)`          | Unchecked get. |
| `Bitset.GetBit`      | `bool <name>_GetBit(P&, u32 bit_idx)`           | Bounds-checked get; out-of-range returns false. |
| `Bitset.qSetBit`     | `void <name>_qSetBit(P&, u32 bit_idx)`          | Unchecked set-to-1. |
| `Bitset.SetBit`      | `void <name>_SetBit(P&, u32 bit_idx)`           | Bounds-checked set; out-of-range is a no-op (or auto-extends — see `ExpandBits`). |
| `Bitset.qClearBit`   | `void <name>_qClearBit(P&, u32 bit_idx)`        | Unchecked set-to-0. |
| `Bitset.ClearBit`    | `void <name>_ClearBit(P&, u32 bit_idx)`         | Bounds-checked clear. |
| `Bitset.qSetBitVal`  | `void <name>_qSetBitVal(P&, u32 bit_idx, bool v)` | Conditional set; same shift+mask logic as qSetBit. |
| `Bitset.qOrBitVal`   | `void <name>_qOrBitVal(P&, u32 bit_idx, bool v)` | OR a value in (sets if `v`, leaves alone otherwise). |

#### Whole-bitset ops
<a href="#whole-bitset-ops"></a>

| Tfunc                | Generated function                          | Effect |
|----------------------|---------------------------------------------|--------|
| `Bitset.BitsEmptyQ`  | `bool <name>_BitsEmptyQ(P&)`                | All bits are 0. |
| `Bitset.Sum1s`       | `int <name>_Sum1s(P&)`                      | Population count (`__builtin_popcountll` per element). |
| `Bitset.ClearBitsAll`| `void <name>_ClearBitsAll(P&)`              | Zero every element. |
| `Bitset.ClearBits`   | `void <name>_ClearBits(P&, P& rhs)`         | `parent &= ~rhs` element-wise. |
| `Bitset.OrBits`      | `void <name>_OrBits(P&, P& rhs)`            | `parent |= rhs` element-wise. |

#### Capacity management (Tary backings only)
<a href="#capacity-management-tary-backings-only-"></a>

| Tfunc                | Generated function                          | Effect |
|----------------------|---------------------------------------------|--------|
| `Bitset.ExpandBits`  | `void <name>_ExpandBits(P&, u32 bit_idx)`   | Grow the Tary so the indicated bit is in range; fills new elements with zeros. |
| `Bitset.AllocBit`    | `int <name>_AllocBit(P&)`                   | Find the lowest 0 bit, set it, return its index.  Extends the array if no 0 bit exists. |

#### Iteration
<a href="#iteration"></a>

| Tfunc                | Generated function                          | Effect |
|----------------------|---------------------------------------------|--------|
| `Bitset.bitcurs`     | `<P>_<name>_bitcurs` cursor + `_Reset/_ValidQ/_Next/_Access` | Walk the 1-bits in order.  Uses `BitScanForward` on each non-zero word — skips runs of zeros without testing each bit. |

### Pitfalls
<a href="#pitfalls"></a>

- **Underlying integer width is detected from `dmmeta.csize`.**
  The element must be an unsigned integer builtin (rejected with
  `amc.bitset_elem` otherwise); a builtin element whose `csize` row
  is absent from the universe is rejected with `amc.bitset_elem_size`.
- **Tary backing's destruction**.  `ClearBitsAll` zeros bytes
  but does not shrink the Tary.  Use `_RemoveAll` on the Tary
  if you also want to free capacity.
- **Bit ordering is LSB-first within each element.**  Bit 0 is
  the LSB of element 0; bit 8 is the LSB of element 1 (for `u8`
  storage).  Watch this when sharing a bitset with code that
  uses a different convention.
- **Single-value bitsets cap at `elem_bits` bits.**  Use Inlary
  / Tary for anything larger.
- **`_SetBit` vs `_qSetBit`.**  `_SetBit` silently no-ops on
  out-of-range indexes for fixed-size storage; use
  `_ExpandBits` first on Tary storage if you want auto-grow.
- **`bitcurs` returns indexes in order**, including for Tary
  storage — but each grow leaves bits in element-major order,
  so a 1-bit at idx 0 is visited before one at idx 64 even if
  the latter was set first.

### See also
<a href="#see-also"></a>

- [Charsets](/txt/exe/amc/charset.md) — same bitset machinery for character classes
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — sub-byte field carved from another field
- [Presence masks](/txt/exe/amc/pmask.md) — bitsets specialized for struct-field presence
- Source: `cpp/amc/bitset.cpp`
- Spec table: `acr 'dmmeta.fbitset:%'`
- Tfunc records: `acr 'tfunc:Bitset.%'`

### Example
<a href="#example"></a>

A 64-bit bool array packed into a single `u64`:

```ssim
dmmeta.ctype  ctype:net.IfaceFlags
  dmmeta.field  field:net.IfaceFlags.bits  arg:u64  reftype:Val
    dmmeta.fbitset  field:net.IfaceFlags.bits
```

```c++
net::IfaceFlags fl;
bits_SetBit(fl, 3);                   // mark interface #3 up
if (bits_GetBit(fl, 3)) { ... }
ind_beg(net::IfaceFlags_bits_bitcurs, idx, fl) {
    process_iface(idx);                // visits 3 (and any others)
} ind_end;
```

A growable bitset on a `Tary<u8>`:

```ssim
dmmeta.ctype  ctype:algo_lib.Bitset
  dmmeta.field  field:algo_lib.Bitset.ary  arg:u8  reftype:Tary
    dmmeta.fbitset  field:algo_lib.Bitset.ary
```

```c++
algo_lib::Bitset bs;
int idx = ary_AllocBit(bs);           // grows the Tary if needed
int n   = ary_Sum1s(bs);              // population count
ary_ClearBitsAll(bs);                 // wipe
```
