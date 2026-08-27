## dmmeta.logcat - Log category


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Logcat

* file:[data/dmmeta/logcat.ssim](/data/dmmeta/logcat.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|logcat|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|enabled|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|builtin|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|stdout|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|maxmsg|i32|[Val](/txt/exe/amc/reftype/Val.md)|0||
|window|i32|[Val](/txt/exe/amc/reftype/Val.md)|0||
|suppress|bool|[Val](/txt/exe/amc/reftype/Val.md)||Throttle is suppressing this category for the rest of its window; enabled is the operator's switch|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [algo_lib](/txt/gen/algo_lib/algo_lib.md) as [algo_lib.FLogcat](/txt/gen/algo_lib/algo_lib.md#algo_lib-flogcat)
