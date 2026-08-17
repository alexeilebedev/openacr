## dev.readmefile - File containing documentation


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Readmefile

* file:[data/dev/readmefile.ssim](/data/dev/readmefile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|inl|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|sandbox|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|filter|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [abt_md -readmefile](/txt/exe/abt_md/README.md) - Regx of readme to process/show (empty=all)
