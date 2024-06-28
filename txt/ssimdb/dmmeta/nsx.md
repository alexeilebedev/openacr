## dmmeta.nsx - Control code-generation and exception handling options for process/library


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Nsx

* file:[data/dmmeta/nsx.ssim](/data/dmmeta/nsx.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|genthrow|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|correct_getorcreate|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|pool|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|sortxref|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|pack|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FNsx](/txt/exe/amc/internals.md#amc-fnsx)
* [src_hdr](/txt/exe/src_hdr/internals.md) as [src_hdr.FNsx](/txt/exe/src_hdr/internals.md#src_hdr-fnsx)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

