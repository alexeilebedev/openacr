## dmmeta.tracerec - Trace record


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tracerec

* file:[data/dmmeta/tracerec.ssim](/data/dmmeta/tracerec.ssim)

italicised fields: *name, ns* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tracerec|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||e.g. rl_lib.trace|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*rl_lib.trace<br>.LR of tracerec*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*e.g. rl_lib<br>.LL of tracerec*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.tracerec

* [dmmeta.tracefld via tracerec](/txt/ssimdb/dmmeta/tracefld.md) - Struct of trace

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTracerec](/txt/gen/amc/amc.md#amc-ftracerec)
