## dev.covfile -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covfile

* file:[data/dev/covfile.ssim](/data/dev/covfile.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Source file|
|total|u32|[Val](/txt/exe/amc/reftypes.md#val)||Total lines|
|nonexe|u32|[Val](/txt/exe/amc/reftypes.md#val)||Non-executable lines|
|exe|u32|[Val](/txt/exe/amc/reftypes.md#val)||Executable lines|
|exer|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftypes.md#val)||Percentage of executable lines|
|hit|u32|[Val](/txt/exe/amc/reftypes.md#val)||Exercised lines|
|cov|[algo.U32Dec2](/txt/protocol/algo/README.md#algo-u32dec2)|[Val](/txt/exe/amc/reftypes.md#val)||Line coverage|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FCovfile](/txt/exe/atf_cov/internals.md#atf_cov-fcovfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

