## amsdb.proctype -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amsdb.Proctype

* file:[data/amsdb/proctype.ssim](/data/amsdb/proctype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proctype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|id|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|ns|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||dmmeta.ns can be missing if the module is external, don't set it as xref|
|overheadmb|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Measured base memory overhead MB (10^6) beyond topo budgets; 0=no derived proc limit|
|hugemb|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Huge-page heap ceiling MB (10^6); 0 = maps none, as a forking supervisor must|
|hbtimeout|i32|[Val](/txt/exe/amc/reftype/Val.md)|30|Heartbeat timeout sec: the supervisor kills a module silent this long|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [lib_ams](/txt/gen/lib_ams/lib_ams.md) as [lib_ams.FProctype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproctype)
