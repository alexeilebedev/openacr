## amcdb.tfunc - AMC template function


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tfunc

* file:[data/amcdb/tfunc.ssim](/data/amcdb/tfunc.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|algo.Smallstr50|Val|
|tclass|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|Pkey||<br>.RL of tfunc|
|name|algo.Smallstr50|Val||<br>.RR of tfunc|
|hasthrow|bool|Val|false|Important defaults for new tfuncs to work|
|leaf|bool|Val|true|Important defaults for new tfuncs to work|
|poolfunc|bool|Val|
|inl|bool|Val||Function is typically inline|
|wur|bool|Val||Warn unused result|
|pure|bool|Val||OK to call fewer times than specified in text|
|ismacro|bool|Val|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of amcdb.tfunc

* [amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md) - Cursor template

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FTfunc

