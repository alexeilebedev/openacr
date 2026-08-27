## dmmeta.func -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Func

* file:[data/dmmeta/func.ssim](/data/dmmeta/func.ssim)

italicised fields: *field, name, ctype, ns* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|func|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)||Primary key|
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*** doesn't always work<br>.RL of func*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*** doesn't always work<br>.RR of func*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*** doesn't always work<br>.RL.RL of func*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL.RL.RL of func*|
|extrn|bool|[Val](/txt/exe/amc/reftype/Val.md)||Implemented externally by user|
|comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Description|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)
