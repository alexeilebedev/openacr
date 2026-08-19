## atfdb.age -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Age

* file:[data/atfdb/age.ssim](/data/atfdb/age.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|age|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|matchorder|u32|[Val](/txt/exe/amc/reftype.md#val)|0|Order the axes are applied in|
|read|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Command printing the axis's current value as a bare number|
|burn|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Command advancing the axis by AGEN, which reaches it in the environment|
|cold|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Burn needs the cluster stopped, so a deploy applies it rather than a run|
|timeout|u32|[Val](/txt/exe/amc/reftype.md#val)|300|Seconds either command may take|
|costmb|u32|[Val](/txt/exe/amc/reftype.md#val)|0|Megabytes one unit of this axis costs a node; 0 = costs nothing|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||
