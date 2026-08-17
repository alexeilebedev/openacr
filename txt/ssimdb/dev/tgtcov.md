## dev.tgtcov - Captured line coverate information by target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Tgtcov

* file:[data/dev/tgtcov.ssim](/data/dev/tgtcov.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Target|
|cov_min|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftype.md#val)||Minimal coverage limit|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FTgtcov](/txt/gen/atf_cov/atf_cov.md#atf_cov-ftgtcov)
