## dmmeta.dispatch - Generate code for a multi-way branch


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispatch

* file:[data/dmmeta/dispatch.ssim](/data/dmmeta/dispatch.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispatch|algo.Smallstr50|Val||Primary key (ns.name)|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Parent namespace<br>.RL of dispatch|
|name|algo.Smallstr50|Val||Dispatch name<br>.RR of dispatch|
|unk|bool|Val||Want default case?|
|read|bool|Val||Generate read function|
|print|bool|Val||Generate print function|
|haslen|bool|Val||Include length in dispatch function|
|call|bool|Val||Generate call to user-defined function|
|strict|bool|Val||Only dispatch if length matches exactly|
|comment|algo.Comment|Val|

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
* [amc](/txt/exe/amc/README.md) as amc.FDispatch
* [src_func](/txt/exe/src_func/README.md) as src_func.FDispatch

