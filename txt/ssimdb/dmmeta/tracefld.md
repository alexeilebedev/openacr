## dmmeta.tracefld - Trace fields


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tracefld

* file:[data/dmmeta/tracefld.ssim](/data/dmmeta/tracefld.ssim)

italicised fields: *tracerec, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tracefld|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*tracerec*|*[dmmeta.Tracerec](/txt/ssimdb/dmmeta/tracerec.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Struct of trace<br>.RL of tracefld*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*Field of trace<br>.RR of tracefld*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.tracefld

* [dmmeta.Usertracefld](/txt/ssimdb/dmmeta/usertracefld.md) - Add custom user trace fields to process's trace struct

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTracefld](/txt/gen/amc/amc.md#amc-ftracefld)
