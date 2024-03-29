## gclidb.issuenote - Issue note


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issuenote

* file:[data/gclidb/issuenote.ssim](/data/gclidb/issuenote.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issuenote|algo.Smallstr250|Val||Identifier. must be in form issue.id|
|issue|[gclidb.Issue](/txt/ssimdb/gclidb/issue.md)|Pkey||Issue portion of issue note<br>.RL of issuenote|
|nid|algo.cstring|Val||Id portion of issue note<br>.RR of issuenote|
|note|algo.cstring|Val||Body of issue note|
|system|algo.cstring|Val||System or user note (glab only)|
|author|algo.cstring|Val||Authtor of issue note|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FIssuenote

