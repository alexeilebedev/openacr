## dmmeta.fcmp - Request versionsort or case-insensitive sort for field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmp

* file:[data/dmmeta/fcmp.ssim](/data/dmmeta/fcmp.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Target field|
|versionsort|bool|Val||Use version sort (detect embedded integers in string)|
|casesens|bool|Val|true|Comparison is case-sensitive|
|extrn|bool|Val||Whether implementation is external|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFcmp

