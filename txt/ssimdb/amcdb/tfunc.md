## amcdb.tfunc - AMC template function


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tfunc

* file:[data/amcdb/tfunc.ssim](/data/amcdb/tfunc.ssim)

italicised fields: *tclass, name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*tclass*|*[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of tfunc*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of tfunc*|
|hasthrow|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Important defaults for new tfuncs to work|
|leaf|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Important defaults for new tfuncs to work|
|poolfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|inl|bool|[Val](/txt/exe/amc/reftypes.md#val)||Function is typically inline|
|wur|bool|[Val](/txt/exe/amc/reftypes.md#val)||Warn unused result|
|pure|bool|[Val](/txt/exe/amc/reftypes.md#val)||OK to call fewer times than specified in text|
|ismacro|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of amcdb.tfunc

* [amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md) - Cursor template 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FTfunc](/txt/exe/amc/internals.md#amc-ftfunc)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

