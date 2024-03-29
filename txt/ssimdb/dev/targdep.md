## dev.targdep - Dependency between targets


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targdep

* file:[data/dev/targdep.ssim](/data/dev/targdep.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targdep|algo.Smallstr50|Val||Primary key - target.parent|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||Child target<br>.RL of targdep|
|parent|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||Parent target<br>.RR of targdep|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FTargdep
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FTargdep
* [amc](/txt/exe/amc/README.md) as amc.FTargdep

