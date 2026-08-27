## dmmeta.gsymbol - Create C++ symbols from entries in ssim table


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Gsymbol

* file:[data/dmmeta/gsymbol.ssim](/data/dmmeta/gsymbol.ssim)

italicised fields: *ns, ssimfile* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gsymbol|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Namespace where new symbols will be placed<br>/RL of gsymbol*|
|*ssimfile*|*[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Source ssimfile<br>/RR of gsymbol*|
|inc|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)||Full regex (sql % wildcard) matched against the row key value|
|symboltype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FGsymbol](/txt/gen/amc/amc.md#amc-fgsymbol)
