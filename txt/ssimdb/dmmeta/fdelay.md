## dmmeta.fdelay - Control elapsed time between execution of a step


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fdelay

* file:[data/dmmeta/fdelay.ssim](/data/dmmeta/fdelay.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fstep|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|Pkey|
|delay|algo.I64Dec9|Val||Delay between steps in seconds|
|scale|bool|Val||Does delay apply to the whole list?|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFdelay

