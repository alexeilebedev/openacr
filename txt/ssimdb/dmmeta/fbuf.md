## dmmeta.fbuf - Buffer for reading/writing messages, works with Iohook


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fbuf

* file:[data/dmmeta/fbuf.ssim](/data/dmmeta/fbuf.ssim)

italicised fields: *fbufdir* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*fbufdir*|*[dmmeta.Fbufdir](/txt/ssimdb/dmmeta/fbufdir.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR_LL of field*|
|max|u32|[Val](/txt/exe/amc/reftypes.md#val)||Size of buffer in bytes|
|fbuftype|[dmmeta.Fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Type of buffer|
|insready|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|inseof|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|iotype|[dmmeta.Fbufiotype](/txt/ssimdb/dmmeta/fbufiotype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"standard"||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFbuf](/txt/exe/amc/internals.md#amc-ffbuf)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

