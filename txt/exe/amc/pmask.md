## amc Presence Masks
<a href="#amc-presence-masks"></a>
AMC supports creation of presence masks. A presence mask is a bitset field
which has one bit allocated for each field in the parent struct. A `<field>_Set` function
is created for each field, which assigns the field and flips the corresponding bit in the
presence mask to `1`. A `<field>_PresentQ` function is create, which tests the corresponding bit.
When reading the struct from a tuple, only the fields that were present in the tuple
will have the corresponding bits set (the rest of the fields will have default values).
When printing a struct to a tuple, only the fields that are marked `present` are written out.

AMC pmask creation is controlled with record
[pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md).

Optionally you can use [pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)
if you don't want the default behavior of including all fields in the presence mask.

Let's start with an example:

```
dmmeta.ctype  ctype:atf_amc.PmaskU32  comment:"A field controlled by a bitmask."
  dmmeta.pack  ctype:atf_amc.PmaskU32  comment:""
  dmmeta.field  field:atf_amc.PmaskU32.pmask  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.pmaskfld  field:atf_amc.PmaskU32.pmask  filter_print:N  comment:""

  dmmeta.field  field:atf_amc.PmaskU32.value   arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:atf_amc.PmaskU32.value2  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:atf_amc.PmaskU32.value3  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:atf_amc.PmaskU32.value4  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:atf_amc.PmaskU32.value5  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.cfmt  cfmt:atf_amc.PmaskU32.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:N  comment:""
```

This creates a number of functions for each field in the struct, which look like this:

```
// Return true if the field is marked in the presence mask
// func:atf_amc.PmaskU32.value5.PresentQ
bool                 value5_PresentQ(atf_amc::PmaskU32& parent) __attribute__((nothrow));
// Set presence bit for this field in the pmask
// func:atf_amc.PmaskU32.value5.SetPresent
void                 value5_SetPresent(atf_amc::PmaskU32& parent) __attribute__((nothrow));
// Return field's bit number in the pmask
// func:atf_amc.PmaskU32.value5.Present_GetBit
int                  value5_Present_GetBit(atf_amc::PmaskU32& parent) __attribute__((nothrow));
// func:atf_amc.PmaskU32.value5.Set
void                 value5_Set(atf_amc::PmaskU32& parent, u32 rhs) __attribute__((nothrow));
// func:atf_amc.PmaskU32.value5.ReadStrptrMaybe
static bool          value5_ReadStrptrMaybe(atf_amc::PmaskU32 &parent, algo::strptr in_str) __attribute__((nothrow));
```

### Multiple Presence Masks
<a href="#multiple-presence-masks"></a>
Using the example above, we could create an additional presence mask with only 3 fields
as members:

```
  dmmeta.field  field:atf_amc.PmaskU32.nullable  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.pmaskfld  field:atf_amc.PmaskU32.nullable  filter_print:N  comment:""
    dmmeta.pmaskfld_member pmaskfld_member:atf_amc.PmaskU32.nullable/atf_amc.PmaskU32.value
    dmmeta.pmaskfld_member pmaskfld_member:atf_amc.PmaskU32.nullable/atf_amc.PmaskU32.value2
    dmmeta.pmaskfld_member pmaskfld_member:atf_amc.PmaskU32.nullable/atf_amc.PmaskU32.value3
```

The functions for created as a result will be called `value_NullableQ`, `value_SetNullable`,
`value_Nullable_GetBit`.

The function `<field>_Set` sets the presence bit in all presence masks in which the field is a member.

It may be useful to ignore an auxiliary presence mask for the purpose of printing a struct.
Set `filter_print` to 'N' in the `pmaskfld` record if you don't want that presence
mask to omit fields when printing.

