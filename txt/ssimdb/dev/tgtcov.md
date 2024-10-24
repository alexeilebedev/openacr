## dev.tgtcov - Captured line coverate information by target


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Tgtcov

* file:[data/dev/tgtcov.ssim](/data/dev/tgtcov.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Target|
|cov_min|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftypes.md#val)||Minimal coverage limit|
|maxerr|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftypes.md#val)||Tolerable error|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FTgtcov](/txt/exe/atf_cov/internals.md#atf_cov-ftgtcov)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

