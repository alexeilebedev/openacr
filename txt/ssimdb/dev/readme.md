## dev.readme - File containing documentation


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Readme

* file:[data/dev/readme.ssim](/data/dev/readme.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey|
|readmecat|[dev.Readmecat](/txt/ssimdb/dev/readmecat.md)|Pkey||<br>/LR/LL of gitfile|
|inl|bool|Val|
|sandbox|bool|Val|
|filter|algo.Smallstr100|Val|
|comment|algo.Comment|Val|

### Used In Commands
<a href="#used-in-commands"></a>
* [abt_md -readme](/txt/exe/abt_md/README.md) - Regx of readme to process/show (empty=all)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FReadme
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FReadme

