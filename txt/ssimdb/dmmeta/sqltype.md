## dmmeta.sqltype - Mapping of ctype -> SQL expression
<a href="#dmmeta-sqltype"></a>
Mapping of ctype to SQL expression.
This table is used when converting OpenACR schema to MariaDB schema.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Sqltype

* file:[data/dmmeta/sqltype.ssim](/data/dmmeta/sqltype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|expr|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FSqltype](/txt/lib/lib_ctype/README.md#lib_ctype-fsqltype)
* [ssim2mysql](/txt/exe/ssim2mysql/internals.md) as [ssim2mysql.FSqltype](/txt/exe/ssim2mysql/internals.md#ssim2mysql-fsqltype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

