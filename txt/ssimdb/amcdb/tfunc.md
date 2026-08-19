## amcdb.tfunc - AMC template function


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tfunc

* file:[data/amcdb/tfunc.ssim](/data/amcdb/tfunc.ssim)

italicised fields: *tclass, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*tclass*|*[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of tfunc*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of tfunc*|
|hasthrow|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Important defaults for new tfuncs to work|
|leaf|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Important defaults for new tfuncs to work|
|poolfunc|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|inl|bool|[Val](/txt/exe/amc/reftype.md#val)||Function is typically inline|
|wur|bool|[Val](/txt/exe/amc/reftype.md#val)||Warn unused result|
|pure|bool|[Val](/txt/exe/amc/reftype.md#val)||OK to call fewer times than specified in text|
|ismacro|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of amcdb.tfunc

* [amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md) - Cursor template

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)
