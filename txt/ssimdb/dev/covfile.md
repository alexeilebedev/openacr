## dev.covfile -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covfile

* file:[data/dev/covfile.ssim](/data/dev/covfile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Source file|
|total|u32|[Val](/txt/exe/amc/reftype/Val.md)||Total lines|
|nonexe|u32|[Val](/txt/exe/amc/reftype/Val.md)||Non-executable lines|
|exe|u32|[Val](/txt/exe/amc/reftype/Val.md)||Executable lines|
|exer|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftype/Val.md)||Percentage of executable lines|
|hit|u32|[Val](/txt/exe/amc/reftype/Val.md)||Exercised lines|
|cov|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftype/Val.md)||Line coverage|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FCovfile](/txt/gen/atf_cov/atf_cov.md#atf_cov-fcovfile)
