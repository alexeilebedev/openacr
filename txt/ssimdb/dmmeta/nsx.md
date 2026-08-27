## dmmeta.nsx - Control code-generation and exception handling options for process/library


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Nsx

* file:[data/dmmeta/nsx.ssim](/data/dmmeta/nsx.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|genthrow|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|correct_getorcreate|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|pool|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|sortxref|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|pack|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FNsx](/txt/gen/amc/amc.md#amc-fnsx)
* [src_hdr](/txt/gen/src_hdr/src_hdr.md) as [src_hdr.FNsx](/txt/gen/src_hdr/src_hdr.md#src_hdr-fnsx)
