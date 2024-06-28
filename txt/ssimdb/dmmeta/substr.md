## dmmeta.substr - Specify that the field value is computed from a substring of another field
<a href="#dmmeta-substr"></a>

This record indicates that the field value is calculated by taking a substring of another
field. The substring field is not necessarily itself a string. It can be any type, in which case
accessing the field (with the generated `_Get` function) involves reading the field value from
the string.

Substrings are used pervasively by acr to construct structured keys.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Substr

* file:[data/dmmeta/substr.ssim](/data/dmmeta/substr.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|expr|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [acr_ed -substr](/txt/exe/acr_ed/README.md) - New field is a substring 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FSubstr](/txt/exe/abt_md/internals.md#abt_md-fsubstr)
* [acr](/txt/exe/acr/internals.md) as [acr.FSubstr](/txt/exe/acr/internals.md#acr-fsubstr)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FSubstr](/txt/exe/acr_in/internals.md#acr_in-fsubstr)
* [amc](/txt/exe/amc/internals.md) as [amc.FSubstr](/txt/exe/amc/internals.md#amc-fsubstr)
* [apm](/txt/exe/apm/internals.md) as [apm.FSubstr](/txt/exe/apm/internals.md#apm-fsubstr)
* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FSubstr](/txt/lib/lib_ctype/README.md#lib_ctype-fsubstr)
* [lib_sqlite](/txt/lib/lib_sqlite/README.md) as [lib_sqlite.FSubstr](/txt/lib/lib_sqlite/README.md#lib_sqlite-fsubstr)
* [ssim2mysql](/txt/exe/ssim2mysql/internals.md) as [ssim2mysql.FSubstr](/txt/exe/ssim2mysql/internals.md#ssim2mysql-fsubstr)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

