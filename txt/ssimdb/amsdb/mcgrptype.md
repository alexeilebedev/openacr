## amsdb.mcgrptype -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amsdb.Mcgrptype

* file:[data/amsdb/mcgrptype.ssim](/data/amsdb/mcgrptype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grptype|[amsdb.Grptype](/txt/ssimdb/amsdb/grptype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|trafficclass|[amsdb.Trafficclass](/txt/ssimdb/amsdb/trafficclass.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Backplane traffic class of every queue this fabric grptype names|
|fanout|bool|[Val](/txt/exe/amc/reftype.md#val)|false|TRUE when several processes read this class, which is what multicast is for|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||
