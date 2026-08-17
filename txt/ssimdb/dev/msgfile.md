## dev.msgfile - File in repo containing messages that should be normalized during CI


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Msgfile

* file:[data/dev/msgfile.ssim](/data/dev/msgfile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Name of file containing messages|
|strict|bool|[Val](/txt/exe/amc/reftype.md#val)||Delete unrecognizable lines (except comments)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||
