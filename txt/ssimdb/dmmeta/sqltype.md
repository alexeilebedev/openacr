## dmmeta.sqltype - Mapping of ctype -> SQL expression
<a href="#dmmeta-sqltype"></a>
Mapping of ctype to SQL expression.
This table is used when converting OpenACR schema to MariaDB schema.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Sqltype

* file:[data/dmmeta/sqltype.ssim](/data/dmmeta/sqltype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|expr|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FSqltype](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-fsqltype)
* [lib_sqlite](/txt/gen/lib_sqlite/lib_sqlite.md) as [lib_sqlite.FSqltype](/txt/gen/lib_sqlite/lib_sqlite.md#lib_sqlite-fsqltype)
* [ssim2mysql](/txt/gen/ssim2mysql/ssim2mysql.md) as [ssim2mysql.FSqltype](/txt/gen/ssim2mysql/ssim2mysql.md#ssim2mysql-fsqltype)
