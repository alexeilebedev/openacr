## dev.msgfile - File in repo containing messages that should be normalized during CI


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Msgfile

* file:[data/dev/msgfile.ssim](/data/dev/msgfile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Name of file containing messages|
|strict|bool|[Val](/txt/exe/amc/reftypes.md#val)||Delete unrecognizable lines (except comments)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FMsgfile](/txt/exe/atf_ci/internals.md#atf_ci-fmsgfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

