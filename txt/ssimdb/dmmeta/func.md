## dmmeta.func -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Func

* file:[data/dmmeta/func.ssim](/data/dmmeta/func.ssim)

italicised fields: *field, name, ctype, ns* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|func|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key|
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*** doesn't always work<br>.RL of func*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*** doesn't always work<br>.RR of func*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*** doesn't always work<br>.RL.RL of func*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL.RL.RL of func*|
|extrn|bool|[Val](/txt/exe/amc/reftypes.md#val)||Implemented externally by user|
|comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Description|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

