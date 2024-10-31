## fmdb.alm_code -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)fmdb.AlmCode

* file:[data/fmdb/alm_code.ssim](/data/fmdb/alm_code.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|alm_code|[fm.Code](/txt/protocol/fm/README.md#fm-code)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm code|
|severity|[fm.Severity](/txt/protocol/fm/README.md#fm-severity)|[Val](/txt/exe/amc/reftypes.md#val)||Assigned severity|
|source|[fmdb.AlmSource](/txt/ssimdb/fmdb/alm_source.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|summary|[fm.Summary](/txt/protocol/fm/README.md#fm-summary)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm summary (slogan)|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [lib_fm](/txt/lib/lib_fm/README.md) as [lib_fm.FAlmCode](/txt/lib/lib_fm/README.md#lib_fm-falmcode)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

