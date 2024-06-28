## gclidb.gtblactfld - List of available for each table & action combination


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblactfld

* file:[data/gclidb/gtblactfld.ssim](/data/gclidb/gtblactfld.ssim)

italicised fields: *gtblact, field* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblactfld|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gtblact*|*[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of gtblactfld*|
|*field*|*[gclidb.Gfld](/txt/ssimdb/gclidb/gfld.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of gtblactfld*|
|dflt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|field_name_dflt|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|optional|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|regx|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FGtblactfld](/txt/exe/gcli/internals.md#gcli-fgtblactfld)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

