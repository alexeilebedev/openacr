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

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Field with which to associate constants|
|namefld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Field which gives the name of the constant|
|idfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Optional attribute of namefld that gives numeric ID for gconst|
|wantenum|bool|Val|true|
|namefldctyp|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||The ctype for this gconst's name field<br>.RL of namefld|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FGconst
* [amc](/txt/exe/amc/README.md) as amc.FGconst

