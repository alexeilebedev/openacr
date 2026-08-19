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
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Anonfld

* file:[data/dmmeta/anonfld.ssim](/data/dmmeta/anonfld.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/gen/acr/acr.md) as [acr.FAnonfld](/txt/gen/acr/acr.md#acr-fanonfld)
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FAnonfld](/txt/gen/acr_compl/acr_compl.md#acr_compl-fanonfld)
* [amc](/txt/gen/amc/amc.md) as [amc.FAnonfld](/txt/gen/amc/amc.md#amc-fanonfld)
