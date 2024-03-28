## gclidb.mrnote - Issue note


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mrnote

* file:[data/gclidb/mrnote.ssim](/data/gclidb/mrnote.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|system|algo.cstring|Val||System or user note (glab only)|
|mrnote|algo.Smallstr250|Val||Identifier. must be in form issue.id|
|nid|algo.cstring|Val||Id portion of mr note<br>.RR of mrnote|
|note|algo.cstring|Val||Body of mr note|
|author|algo.cstring|Val||Authtor of mr note|
|mr|[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)|Pkey||Mr portion of mr note<br>.RL of mrnote|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FMrnote

