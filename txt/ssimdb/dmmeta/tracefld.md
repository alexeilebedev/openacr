## dmmeta.tracefld - Trace fields


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tracefld

* file:[data/dmmeta/tracefld.ssim](/data/dmmeta/tracefld.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tracefld|algo.Smallstr100|Val|
|tracerec|[dmmeta.Tracerec](/txt/ssimdb/dmmeta/tracerec.md)|Pkey||Struct of trace<br>.RL of tracefld|
|name|algo.Smallstr50|Val||Field of trace<br>.RR of tracefld|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.tracefld

* [dmmeta.Usertracefld](/txt/ssimdb/dmmeta/usertracefld.md) - Add custom user trace fields to process's trace struct

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FTracefld

