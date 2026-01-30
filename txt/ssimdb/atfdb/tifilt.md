## atfdb.tifilt - Input filter for component test


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Tifilt

* file:[data/atfdb/tifilt.ssim](/data/atfdb/tifilt.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|ifilter|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Command to preprocess input|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_comp](/txt/exe/atf_comp/internals.md) as [atf_comp.FTifilt](/txt/exe/atf_comp/internals.md#atf_comp-ftifilt)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

