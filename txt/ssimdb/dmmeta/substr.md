## dmmeta.substr - Specify that the field value is computed from a substring of another field
<a href="#dmmeta-substr"></a>

This record indicates that the field value is calculated by taking a substring of another
field. The substring field is not necessarily itself a string. It can be any type, in which case
accessing the field (with the generated `_Get` function) involves reading the field value from
the string.

Substrings are used pervasively by acr to construct structured keys.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Substr

* file:[data/dmmeta/substr.ssim](/data/dmmeta/substr.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|expr|algo.CppExpr|Val|
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -substr](/txt/exe/acr_ed/README.md) - New field is a substring

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FSubstr
* [acr](/txt/exe/acr/README.md) as acr.FSubstr
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FSubstr
* [amc](/txt/exe/amc/README.md) as amc.FSubstr
* [apm](/txt/exe/apm/README.md) as apm.FSubstr
* [lib_ctype](/txt/lib/lib_ctype/README.md) as lib_ctype.FSubstr
* [lib_sqlite](/txt/lib/lib_sqlite/README.md) as lib_sqlite.FSubstr
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) as ssim2mysql.FSubstr

