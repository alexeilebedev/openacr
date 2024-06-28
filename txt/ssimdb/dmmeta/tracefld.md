## dmmeta.tracefld - Trace fields


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tracefld

* file:[data/dmmeta/tracefld.ssim](/data/dmmeta/tracefld.ssim)

italicised fields: *tracerec, name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tracefld|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*tracerec*|*[dmmeta.Tracerec](/txt/ssimdb/dmmeta/tracerec.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Struct of trace<br>.RL of tracefld*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Field of trace<br>.RR of tracefld*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.tracefld

* [dmmeta.Usertracefld](/txt/ssimdb/dmmeta/usertracefld.md) - Add custom user trace fields to process's trace struct 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FTracefld](/txt/exe/amc/internals.md#amc-ftracefld)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

