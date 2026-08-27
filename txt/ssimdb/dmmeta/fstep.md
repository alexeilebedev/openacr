## dmmeta.fstep - Generate a main loop step to be executed whenever a field is non-empty


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fstep

* file:[data/dmmeta/fstep.ssim](/data/dmmeta/fstep.ssim)

italicised fields: *field, ns, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fstep|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br> of fstep*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LL of fstep*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.RR of fstep*|
|steptype|[dmmeta.Steptype](/txt/ssimdb/dmmeta/steptype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.fstep

* [dmmeta.Fdelay](/txt/ssimdb/dmmeta/fdelay.md) - Control elapsed time between execution of a step

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFstep](/txt/gen/amc/amc.md#amc-ffstep)
