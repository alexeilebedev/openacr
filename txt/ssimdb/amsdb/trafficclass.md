## amsdb.trafficclass -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amsdb.Trafficclass

* file:[data/amsdb/trafficclass.ssim](/data/amsdb/trafficclass.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|trafficclass|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|id|u8|[Val](/txt/exe/amc/reftype.md#val)|0|Class index: the per-class unicast port offset (data 0, gapfill 1, control 2)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference amsdb.trafficclass

* [amsdb.mcgrptype via trafficclass](/txt/ssimdb/amsdb/mcgrptype.md) - Backplane traffic class of every queue this fabric grptype names
