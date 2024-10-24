## dmmeta.fbigend - Annotate field as having big-endian storage
<a href="#dmmeta-fbigend"></a>

This record indicates that the memory byte order of a field is big-endian.
Here is an example:

```
dmmeta.field  field:atf_amc.TypeBE16.value  arg:u16  reftype:Val  dflt:""  comment:""
    dmmeta.fbigend  field:atf_amc.TypeBE16.value  comment:""
```

Amc generates values `value_Get` and `value_Set` to access the value.
```
    u16                  value_Get(const atf_amc::TypeBE16& parent) __attribute__((__warn_unused_result__, nothrow));
    void                 value_Set(atf_amc::TypeBE16& parent, u16 rhs) __attribute__((nothrow));
```

When generating the C++ struct, amc adds `_be` to the field name so that the programmer
becomes aware, should they choose to access it manually, that the actual field *value* is different
from what's stored in memory:
```
    $ amc atf_amc.TypeBE16%
    // --- atf_amc.TypeBE16
    struct TypeBE16 { // atf_amc.TypeBE16
        u16   value_be;   //   0
        TypeBE16();
    };
```

Amc reads/writes big-endian fields just like all other fields.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fbigend

* file:[data/dmmeta/fbigend.ssim](/data/dmmeta/fbigend.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFbigend](/txt/exe/amc/internals.md#amc-ffbigend)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

