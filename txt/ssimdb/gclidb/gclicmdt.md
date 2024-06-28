## gclidb.gclicmdt - Internal test


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclicmdt

* file:[data/gclidb/gclicmdt.ssim](/data/gclidb/gclicmdt.ssim)

italicised fields: *gclicmd, gtype* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclicmdt|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gmethod|[gclidb.Gmethod](/txt/ssimdb/gclidb/gmethod.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"GET"||
|*gclicmd*|*[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of gclicmdt*|
|*gtype*|*[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of gclicmdt*|
|uri|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FGclicmdt](/txt/exe/gcli/internals.md#gcli-fgclicmdt)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

