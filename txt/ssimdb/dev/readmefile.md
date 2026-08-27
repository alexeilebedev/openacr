## dev.readmefile - File containing documentation


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Readmefile

* file:[data/dev/readmefile.ssim](/data/dev/readmefile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|inl|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|sandbox|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|filter|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [abt_md -readmefile](/txt/exe/abt_md/README.md) - Regx of readme to process/show (empty=all)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FReadmefile](/txt/gen/abt_md/abt_md.md#abt_md-freadmefile)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FReadmefile](/txt/gen/atf_ci/atf_ci.md#atf_ci-freadmefile)
