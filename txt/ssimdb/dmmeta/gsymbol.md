## dmmeta.gsymbol - Create C++ symbols from entries in ssim table


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Gsymbol

* file:[data/dmmeta/gsymbol.ssim](/data/dmmeta/gsymbol.ssim)

italicised fields: *ns, ssimfile* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gsymbol|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Namespace where new symbols will be placed<br>/RL of gsymbol*|
|*ssimfile*|*[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Source ssimfile<br>/RR of gsymbol*|
|inc|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Regex to filter rows|
|symboltype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FGsymbol](/txt/exe/amc/internals.md#amc-fgsymbol)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

