## gclidb.gtblacttst -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblacttst

* file:[data/gclidb/gtblacttst.ssim](/data/gclidb/gtblacttst.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblacttst|algo.Smallstr250|Val|
|gtblact|[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)|Pkey||<br>.LL of gtblacttst|
|gclienv|[gclidb.Gclienv](/txt/ssimdb/gclidb/gclienv.md)|Pkey||<br>.LR.LL of gtblacttst|
|working|bool|Val|true|
|t|algo.cstring|Val||<br>.RR of gtblacttst|
|cmd|algo.cstring|Val|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.gtblacttst

* [gclidb.gtblacttstout via gtblacttst](/txt/ssimdb/gclidb/gtblacttstout.md) -

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_gcli -gtblacttst](/txt/exe/atf_gcli/README.md) - gcli tests regx

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_gcli](/txt/exe/atf_gcli/README.md) as atf_gcli.FGtblacttst

