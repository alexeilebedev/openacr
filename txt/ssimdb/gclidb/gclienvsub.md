## gclidb.gclienvsub -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclienvsub

* file:[data/gclidb/gclienvsub.ssim](/data/gclidb/gclienvsub.ssim)

italicised fields: *gclienv, sub* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclienvsub|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gclienv*|*[gclidb.Gclienv](/txt/ssimdb/gclidb/gclienv.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of gclienvsub*|
|*sub*|*[gclidb.Gclisub](/txt/ssimdb/gclidb/gclisub.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LR of gclienvsub*|
|fwd|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|rev|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|uval|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_gcli](/txt/exe/atf_gcli/internals.md) as [atf_gcli.FGclienvsub](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienvsub)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

