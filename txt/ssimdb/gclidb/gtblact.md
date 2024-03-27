## gclidb.gtblact -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblact

* file:[data/gclidb/gtblact.ssim](/data/gclidb/gtblact.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblact|algo.Smallstr50|Val|
|gtbl|[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)|Pkey||<br>_LL of gtblact|
|gact|[gclidb.Gact](/txt/ssimdb/gclidb/gact.md)|Pkey||<br>_LR of gtblact|
|t|bool|Val|
|e|bool|Val|
|auth|bool|Val|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.gtblact

* [gclidb.gtblactfld via gtblact](/txt/ssimdb/gclidb/gtblactfld.md) - List of available for each table & action combination 
* [gclidb.gtblacttst via gtblact](/txt/ssimdb/gclidb/gtblacttst.md) -

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_gcli](/txt/exe/atf_gcli/README.md) as atf_gcli.FGtblact
* [gcli](/txt/exe/gcli/README.md) as gcli.FGtblact

