## dmmeta.fdelay - Control elapsed time between execution of a step


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fdelay

* file:[data/dmmeta/fdelay.ssim](/data/dmmeta/fdelay.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fstep|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|delay|[algo.I64Dec9](/txt/protocol/algo/README.md#algo-i64dec9)|[Val](/txt/exe/amc/reftypes.md#val)||Delay between steps in seconds|
|scale|bool|[Val](/txt/exe/amc/reftypes.md#val)||Does delay apply to the whole list?|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFdelay](/txt/exe/amc/internals.md#amc-ffdelay)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

