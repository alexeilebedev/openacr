## dmmeta.dispatch - Generate code for a multi-way branch


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispatch

* file:[data/dmmeta/dispatch.ssim](/data/dmmeta/dispatch.ssim)

italicised fields: *ns, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispatch|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)||Primary key (ns.name)|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Parent namespace<br>.RL of dispatch*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Dispatch name<br>.RR of dispatch*|
|unk|bool|[Val](/txt/exe/amc/reftype.md#val)||Want default case?|
|read|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate read function|
|print|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate print function|
|haslen|bool|[Val](/txt/exe/amc/reftype.md#val)||Include length in dispatch function|
|call|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate call to user-defined function|
|strict|bool|[Val](/txt/exe/amc/reftype.md#val)||Only dispatch if length matches exactly|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.dispatch

* [dmmeta.Dispctx](/txt/ssimdb/dmmeta/dispctx.md) - Use context with dispatch 
* [dmmeta.Dispfilter](/txt/ssimdb/dmmeta/dispfilter.md) - Generate filter function on dispatch 
* [dmmeta.Disptrace](/txt/ssimdb/dmmeta/disptrace.md) - Generate trace fields (cycles, counts) for all dispatch branches

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.dispatch

* [dmmeta.dispatch_msg via dispatch](/txt/ssimdb/dmmeta/dispatch_msg.md) - Add message to a dispatch

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FDispatch](/txt/gen/abt_md/abt_md.md#abt_md-fdispatch)
* [amc](/txt/gen/amc/amc.md) as [amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)
