## dmmeta.dispatch - Generate code for a multi-way branch


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispatch

* file:[data/dmmeta/dispatch.ssim](/data/dmmeta/dispatch.ssim)

italicised fields: *ns, name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispatch|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key (ns.name)|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent namespace<br>.RL of dispatch*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Dispatch name<br>.RR of dispatch*|
|unk|bool|[Val](/txt/exe/amc/reftypes.md#val)||Want default case?|
|read|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate read function|
|print|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate print function|
|haslen|bool|[Val](/txt/exe/amc/reftypes.md#val)||Include length in dispatch function|
|call|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate call to user-defined function|
|strict|bool|[Val](/txt/exe/amc/reftypes.md#val)||Only dispatch if length matches exactly|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.dispatch

* [dmmeta.Dispctx](/txt/ssimdb/dmmeta/dispctx.md) - Use context with dispatch 
* [dmmeta.Dispfilter](/txt/ssimdb/dmmeta/dispfilter.md) - Generate filter function on dispatch 
* [dmmeta.Disptrace](/txt/ssimdb/dmmeta/disptrace.md) - Generate trace fields (cycles, counts) for all dispatch branches 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.dispatch

* [dmmeta.dispatch_msg via dispatch](/txt/ssimdb/dmmeta/dispatch_msg.md) - Add message to a dispatch 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)
* [src_func](/txt/exe/src_func/internals.md) as [src_func.FDispatch](/txt/exe/src_func/internals.md#src_func-fdispatch)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

