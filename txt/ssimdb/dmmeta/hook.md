## dmmeta.hook - Required on Hook fields
<a href="#dmmeta-hook"></a>

A Hook field is a pointer to a function.

When a record with Hook is created at process startup time using [gstatic](gstatic.md),
amc forward-declares one function for each record in the table; the name of the function is based
on the primary key of the record.
These are user-implemented functions, and their pointers are automatically assigned to hook fields
as part of process startup initialization.
Thus, one can associate one user-implemented function with each row of a ssim table.
There are no restrictions on the target table -- it can be indexed and cross-referenced in any way.
This is a very commonly used trick to structure processes.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Hook

* file:[data/dmmeta/hook.ssim](/data/dmmeta/hook.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||The field|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FHook

