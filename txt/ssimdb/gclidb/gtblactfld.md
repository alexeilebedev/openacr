## gclidb.gtblactfld - List of available for each table & action combination


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblactfld

* file:[data/gclidb/gtblactfld.ssim](/data/gclidb/gtblactfld.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblactfld|algo.Smallstr50|Val|
|gtblact|[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)|Pkey||<br>.RL of gtblactfld|
|field|[gclidb.Gfld](/txt/ssimdb/gclidb/gfld.md)|Pkey||<br>.RR of gtblactfld|
|dflt|algo.cstring|Val|
|field_name_dflt|bool|Val|
|optional|bool|Val|
|regx|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FGtblactfld

