## dmmeta.fbuf - Buffer for reading/writing messages, works with Iohook


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fbuf

* file:[data/dmmeta/fbuf.ssim](/data/dmmeta/fbuf.ssim)

italicised fields: *fbufdir* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*fbufdir*|*[dmmeta.Fbufdir](/txt/ssimdb/dmmeta/fbufdir.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RR_LL of field*|
|max|u32|[Val](/txt/exe/amc/reftype/Val.md)||Size of buffer in bytes|
|fbuftype|[dmmeta.Fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Type of buffer|
|iotype|[dmmeta.Fbufiotype](/txt/ssimdb/dmmeta/fbufiotype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|"standard"||
|pair|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|""|Read buffer this write buffer shares a descriptor with|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFbuf](/txt/gen/amc/amc.md#amc-ffbuf)
