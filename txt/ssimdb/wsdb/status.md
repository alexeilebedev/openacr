## wsdb.status -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)wsdb.Status

* file:[data/wsdb/status.ssim](/data/wsdb/status.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|code|u32|[Val](/txt/exe/amc/reftypes.md#val)||Status code|
|reason|[algo.Smallstr32](/txt/protocol/algo/README.md#algo-smallstr32)|[Val](/txt/exe/amc/reftypes.md#val)||Status reason|
|local_only|bool|[Val](/txt/exe/amc/reftypes.md#val)||Never sent on close frame, use locally|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

