## dmmeta.sqltype - Mapping of ctype -> SQL expression
<a href="#dmmeta-sqltype"></a>
Mapping of ctype to SQL expression.
This table is used when converting OpenACR schema to MariaDB schema.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Sqltype

* file:[data/dmmeta/sqltype.ssim](/data/dmmeta/sqltype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|
|expr|algo.Smallstr100|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) as ssim2mysql.FSqltype

