## dmmeta.fdelay - Control elapsed time between execution of a step


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fdelay

* file:[data/dmmeta/fdelay.ssim](/data/dmmeta/fdelay.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fstep|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|delay|[algo.I64Dec9](/txt/protocol/algo/README.md#algo-i64dec9)|[Val](/txt/exe/amc/reftype/Val.md)||Delay between steps in seconds|
|scale|bool|[Val](/txt/exe/amc/reftype/Val.md)||Does delay apply to the whole list?|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFdelay](/txt/gen/amc/amc.md#amc-ffdelay)
