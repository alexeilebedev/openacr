## dmmeta.gconst - Import ssim table columns as fconst for a field
<a href="#dmmeta-gconst"></a>

The `gconst` record creates an enum type from two columns of an ssim file.
One column becomes the name, another becomes the value. If the target field is of string type,
then compile-time string constants are generated instead.
See [fconst](/txt/ssimdb/dmmeta/fconst.md) for more documentation.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Gconst

* file:[data/dmmeta/gconst.ssim](/data/dmmeta/gconst.ssim)

italicised fields: *namefldctyp* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Field with which to associate constants|
|namefld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Field which gives the name of the constant|
|idfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Optional attribute of namefld that gives numeric ID for gconst|
|wantenum|bool|[Val](/txt/exe/amc/reftype.md#val)|true||
|*namefldctyp*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*The ctype for this gconst's name field<br>.RL of namefld*|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FGconst](/txt/gen/abt_md/abt_md.md#abt_md-fgconst)
* [amc](/txt/gen/amc/amc.md) as [amc.FGconst](/txt/gen/amc/amc.md#amc-fgconst)
