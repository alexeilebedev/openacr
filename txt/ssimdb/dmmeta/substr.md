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

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|expr|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -substr](/txt/exe/acr_ed/README.md) - New field is a substring

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FSubstr](/txt/gen/abt_md/abt_md.md#abt_md-fsubstr)
* [acr](/txt/gen/acr/acr.md) as [acr.FSubstr](/txt/gen/acr/acr.md#acr-fsubstr)
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FSubstr](/txt/gen/acr_compl/acr_compl.md#acr_compl-fsubstr)
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FSubstr](/txt/gen/acr_in/acr_in.md#acr_in-fsubstr)
* [amc](/txt/gen/amc/amc.md) as [amc.FSubstr](/txt/gen/amc/amc.md#amc-fsubstr)
* [apm](/txt/gen/apm/apm.md) as [apm.FSubstr](/txt/gen/apm/apm.md#apm-fsubstr)
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FSubstr](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-fsubstr)
* [lib_sqlite](/txt/gen/lib_sqlite/lib_sqlite.md) as [lib_sqlite.FSubstr](/txt/gen/lib_sqlite/lib_sqlite.md#lib_sqlite-fsubstr)
* [ssim2mysql](/txt/gen/ssim2mysql/ssim2mysql.md) as [ssim2mysql.FSubstr](/txt/gen/ssim2mysql/ssim2mysql.md#ssim2mysql-fsubstr)
