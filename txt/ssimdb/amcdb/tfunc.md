## amcdb.tfunc - AMC template function


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tfunc

* file:[data/amcdb/tfunc.ssim](/data/amcdb/tfunc.ssim)

italicised fields: *tclass, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*tclass*|*[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of tfunc*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.RR of tfunc*|
|hasthrow|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Important defaults for new tfuncs to work|
|leaf|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Important defaults for new tfuncs to work|
|poolfunc|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|inl|bool|[Val](/txt/exe/amc/reftype/Val.md)||Function is typically inline|
|wur|bool|[Val](/txt/exe/amc/reftype/Val.md)||Warn unused result|
|pure|bool|[Val](/txt/exe/amc/reftype/Val.md)||OK to call fewer times than specified in text|
|ismacro|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of amcdb.tfunc

* [amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md) - Cursor template

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)
