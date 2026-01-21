## dmmeta.fbitset - Generate bitset functions over integer field or array
<a href="#dmmeta-fbitset"></a>

The fbitset record (not to be confused with `bitfld`) can be attached to any
numeric field, an inline array (`Inlary`) or a `Tary`. It generates standard bitset functions
Here is an example:

```
inline-command: acr -t field:atf_amc.Bitset.fld8 | egrep Bitset
    dmmeta.ctype  ctype:atf_amc.Bitset  comment:"Test bitset"
      dmmeta.field  field:atf_amc.Bitset.fld8  arg:u8  reftype:Val  dflt:""  comment:"Bitset field"
        dmmeta.fbitset  field:atf_amc.Bitset.fld8  comment:""
        dmmeta.fcurs  fcurs:atf_amc.Bitset.fld8/bitcurs  comment:""
```

The generated functions are as follows:

```
inline-command: amc -report:N -proto atf_amc.Bitset.fld8.%
// Return constant 1
// func:atf_amc.Bitset.fld8.N
inline int           fld8_N(atf_amc::Bitset& parent) __attribute__((__warn_unused_result__, nothrow, pure));
// Access value
// func:atf_amc.Bitset.fld8.qFind
inline u8&           fld8_qFind(atf_amc::Bitset& parent, int) __attribute__((__warn_unused_result__, nothrow));
// Get max # of bits in the bitset
// Return max. number of bits supported by array
// func:atf_amc.Bitset.fld8.NBits
inline int           fld8_Nbits(atf_amc::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
// Retrieve value of bit #BIT_IDX in bit set. No bounds checking
// func:atf_amc.Bitset.fld8.qGetBit
inline bool          fld8_qGetBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((__warn_unused_result__, nothrow));
// Retrieve value of bit #BIT_IDX in bit set. If bit index is out of bounds, return 0.
// func:atf_amc.Bitset.fld8.GetBit
inline bool          fld8_GetBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((__warn_unused_result__, nothrow));
// Check if all the bits in the bitset are equal to zero
// func:atf_amc.Bitset.fld8.BitsEmptyQ
inline bool          fld8_BitsEmptyQ(atf_amc::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
// func:atf_amc.Bitset.fld8.Sum1s
inline u64           fld8_Sum1s(atf_amc::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
// Clear bit # BIT_IDX in bit set. No bounds checking
// func:atf_amc.Bitset.fld8.qClearBit
inline void          fld8_qClearBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Clear bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing
// func:atf_amc.Bitset.fld8.ClearBit
inline void          fld8_ClearBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. No bounds checking
// func:atf_amc.Bitset.fld8.qSetBit
inline void          fld8_qSetBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing.
// func:atf_amc.Bitset.fld8.SetBit
inline void          fld8_SetBit(atf_amc::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. No bounds checking
// func:atf_amc.Bitset.fld8.qSetBitVal
inline void          fld8_qSetBitVal(atf_amc::Bitset& parent, u32 bit_idx, bool val) __attribute__((nothrow));
// Or bit # BIT_IDX in bit set. No bounds checking
// func:atf_amc.Bitset.fld8.qOrBitVal
inline void          fld8_qOrBitVal(atf_amc::Bitset& parent, u32 bit_idx, bool val) __attribute__((nothrow));
// Set all bits of array to zero.
// Note: this does not change what NBits will return.
// func:atf_amc.Bitset.fld8.ClearBitsAll
inline void          fld8_ClearBitsAll(atf_amc::Bitset& parent) __attribute__((nothrow));
// Zero in PARENT any bits that are set in RHS.
// func:atf_amc.Bitset.fld8.ClearBits
inline void          fld8_ClearBits(atf_amc::Bitset& parent, atf_amc::Bitset &rhs) __attribute__((nothrow));
// Set PARENT to union of two bitsets.
// (This function is not named Set.. to avoid triple entendre).
// func:atf_amc.Bitset.fld8.OrBits
inline void          fld8_OrBits(atf_amc::Bitset& parent, atf_amc::Bitset &rhs) __attribute__((nothrow));
// Return smallest number N such that indexes of all 1 bits are below N
// func:atf_amc.Bitset.fld8.Sup
inline i32           fld8_Sup(atf_amc::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));

```

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fbitset

* file:[data/dmmeta/fbitset.ssim](/data/dmmeta/fbitset.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFbitset](/txt/exe/amc/internals.md#amc-ffbitset)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

