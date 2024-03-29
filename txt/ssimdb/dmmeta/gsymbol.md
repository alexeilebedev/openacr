## dmmeta.gsymbol - Create C++ symbols from entries in ssim table


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Gsymbol

* file:[data/dmmeta/gsymbol.ssim](/data/dmmeta/gsymbol.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gsymbol|algo.Smallstr50|Val|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Namespace where new symbols will be placed<br>/RL of gsymbol|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Pkey||Source ssimfile<br>/RR of gsymbol|
|inc|algo.Smallstr100|Val||Regex to filter rows|
|symboltype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FGsymbol

