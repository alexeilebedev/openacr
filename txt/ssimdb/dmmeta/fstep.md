## dmmeta.fstep - Generate a main loop step to be executed whenever a field is non-empty


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fstep

* file:[data/dmmeta/fstep.ssim](/data/dmmeta/fstep.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fstep|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br> of fstep|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.LL of fstep|
|name|algo.Smallstr50|Val||<br>.RR of fstep|
|steptype|[dmmeta.Steptype](/txt/ssimdb/dmmeta/steptype.md)|Pkey|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.fstep

* [dmmeta.Fdelay](/txt/ssimdb/dmmeta/fdelay.md) - Control elapsed time between execution of a step

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -fstep](/txt/exe/acr_ed/README.md) - Add fstep record

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFstep
* [src_func](/txt/exe/src_func/README.md) as src_func.FFstep

