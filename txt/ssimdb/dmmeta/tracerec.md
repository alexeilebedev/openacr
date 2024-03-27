## dmmeta.tracerec - Trace record


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tracerec

* file:[data/dmmeta/tracerec.ssim](/data/dmmeta/tracerec.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tracerec|algo.Smallstr50|Val||e.g. rl_lib.trace|
|name|algo.Smallstr50|Val||rl_lib.trace<br>.LR of tracerec|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||e.g. rl_lib<br>.LL of tracerec|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.tracerec

* [dmmeta.tracefld via tracerec](/txt/ssimdb/dmmeta/tracefld.md) - Struct of trace

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FTracerec

