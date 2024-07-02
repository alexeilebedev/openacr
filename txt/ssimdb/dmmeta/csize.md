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
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Csize

* file:[data/dmmeta/csize.ssim](/data/dmmeta/csize.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|size|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|alignment|u32|[Val](/txt/exe/amc/reftypes.md#val)|1||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FCsize](/txt/exe/amc/internals.md#amc-fcsize)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

