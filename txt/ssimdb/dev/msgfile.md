## dev.msgfile - File in repo containing messages that should be normalized during CI


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Msgfile

* file:[data/dev/msgfile.ssim](/data/dev/msgfile.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey||Name of file containing messages|
|strict|bool|Val||Delete unrecognizable lines (except comments)|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FMsgfile

