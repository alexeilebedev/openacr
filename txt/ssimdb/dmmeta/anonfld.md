## dmmeta.anonfld - Omit field name where possible (command line, enums, constants)
<a href="#dmmeta-anonfld"></a>

The `anonfld` record indicates that a field has an optional name.
It is used in several contexts. When used in a command line, it creates a positional argument.
For enums, it causes the name of the field to be stripped from the generated enum constant.
If there were no anonfld,
the enum's symbols would be named `algo_Bool_value_true`, etc. With anonfld, it becomes `algo_Bool_true`,
etc.

```
    dmmeta.anonfld  field:algo.Bool.value  comment:""
```

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Anonfld

* file:[data/dmmeta/anonfld.ssim](/data/dmmeta/anonfld.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr](/txt/exe/acr/internals.md) as [acr.FAnonfld](/txt/exe/acr/internals.md#acr-fanonfld)
* [acr_compl](/txt/exe/acr_compl/internals.md) as [acr_compl.FAnonfld](/txt/exe/acr_compl/internals.md#acr_compl-fanonfld)
* [amc](/txt/exe/amc/internals.md) as [amc.FAnonfld](/txt/exe/amc/internals.md#amc-fanonfld)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

