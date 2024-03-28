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

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/exe/acr/README.md) as acr.FAnonfld
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FAnonfld
* [amc](/txt/exe/amc/README.md) as amc.FAnonfld

