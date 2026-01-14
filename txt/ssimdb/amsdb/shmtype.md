## amsdb.shmtype -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amsdb.Shmtype

* file:[data/amsdb/shmtype.ssim](/data/amsdb/shmtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|shmtype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|id|[ams.Shmtype](/txt/protocol/ams/README.md#ams-shmtype)|[Val](/txt/exe/amc/reftypes.md#val)|||
|nonblock|bool|[Val](/txt/exe/amc/reftypes.md#val)||Non-blocking (lossy) stream|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [lib_ams](/txt/lib/lib_ams/README.md) as [lib_ams.FShmtype](/txt/lib/lib_ams/README.md#lib_ams-fshmtype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

