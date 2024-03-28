## dmmeta.csize - Specify size/alignment for built-in C++ types
<a href="#dmmeta-csize"></a>

The `dmmeta.csize` table allows user to declare the intended size, in bytes, of each ctype,
and its alignment. For example:

```
dmmeta.csize  ctype:algw1.AccountEnableMsg         size:8   alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountLimitsMsg         size:88  alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountNoLoadMsg         size:8   alignment:1  comment:""
```

This declaration has no effect on the code generation. 
`amc` generates a static (compile-time) assertion in the `StaticCheck`
function in the target namespace. 
E.g.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Csize

* file:[data/dmmeta/csize.ssim](/data/dmmeta/csize.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|
|size|u32|Val|
|alignment|u32|Val|1|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FCsize

