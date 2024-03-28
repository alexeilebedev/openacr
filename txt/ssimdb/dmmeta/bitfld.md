## dmmeta.bitfld - Specify offset/width of a bitfield within another field
<a href="#dmmeta-bitfld"></a>

Specify that the field is a computed field extracted from certain bits of the value of a source field.
In the example below, the field `bits5` is defined as bits 5..10 of the value of the field called `value`.
```
    dmmeta.field  field:atf_amc.BitfldType1.value  arg:u64  reftype:Val     dflt:""  comment:""
    dmmeta.field  field:atf_amc.BitfldType1.bits5   arg:u64  reftype:Bitfld  dflt:""  comment:""
      dmmeta.bitfld  field:atf_amc.BitfldType1.bits5   offset:5  width:10  srcfield:atf_amc.BitfldType1.value  comment:""
```

Note that the AMC bitfld is endian-agnostic; it specifies a calculation to perform on the source field,
which is the same on all architectures. In contrast, C bitfields depend on machine endianness, because
they are not specified in relation to a source field.

Bitfld requires `reftype:Bitfld` in the parent field record. A Bitfld field can be of any type that
can be created from an integer, such as bool, any signed or unsigned integer type, or a wrapped enum type.

The following functions are generated:
```
inline-command: amc atf_amc.BitfldType1.bits5.% -proto -report:N
// Retrieve bitfield from value of field value
//    10 bits starting at bit 5.
// func:atf_amc.BitfldType1.bits5.Get
u64                  bits5_Get(const atf_amc::BitfldType1& parent) __attribute__((__warn_unused_result__, nothrow));
// Set bitfield in value of field 'value'
//    10 bits starting at bit 5.
// func:atf_amc.BitfldType1.bits5.Set
void                 bits5_Set(atf_amc::BitfldType1& parent, u64 rhs) __attribute__((nothrow));
// func:atf_amc.BitfldType1.bits5.ReadStrptrMaybe
static bool          bits5_ReadStrptrMaybe(atf_amc::BitfldType1 &parent, algo::strptr in_str) __attribute__((nothrow));

```

Bitfields can be read and printed from string tuples just like other fields.
When printing a ctype containing bitfields to an ssim tuple, amc does not print the source field;
instead, all the bitfields are printed instead.

When printing a ctype containing bitfields using the `Raw` format, only the source field is printed,
and bitfields are skipped.

It is an error to specify a combination of offset and width that is out of bounds for the source type.
Amc will flag this. It is also an error to have two bitfields overlap. All of the `offset+width`
ranges must be disjoint.

In order to facilitate flexible handling of named bits within bitsets,
special enum constants are generated for bitfld when its width is `1` and the field type is `bool`.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Bitfld

* file:[data/dmmeta/bitfld.ssim](/data/dmmeta/bitfld.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|name|algo.Smallstr50|Val||<br>.RR of field|
|offset|i32|Val||Offset, in bits, within parent field|
|width|i32|Val||Width, in bits, within parent field.|
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FBitfld

