## dev.covtarget -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covtarget

* file:[data/dev/covtarget.ssim](/data/dev/covtarget.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covtarget|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Target|
|total|u32|[Val](/txt/exe/amc/reftype.md#val)||Total lines|
|nonexe|u32|[Val](/txt/exe/amc/reftype.md#val)||Non-executable lines|
|exe|u32|[Val](/txt/exe/amc/reftype.md#val)||Executable lines|
|exer|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftype.md#val)||Percentage of executable lines|
|hit|u32|[Val](/txt/exe/amc/reftype.md#val)||Exercised lines|
|cov|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftype.md#val)||Line coverage|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FCovtarget](/txt/gen/atf_cov/atf_cov.md#atf_cov-fcovtarget)
