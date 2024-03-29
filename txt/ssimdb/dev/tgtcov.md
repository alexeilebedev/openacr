## dev.tgtcov - Captured line coverate information by target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Tgtcov

* file:[data/dev/tgtcov.ssim](/data/dev/tgtcov.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||Target|
|cov_min|algo.U32Dec2|Val||Minimal coverage limit|
|maxerr|algo.U32Dec2|Val||Tolerable error|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FTgtcov

