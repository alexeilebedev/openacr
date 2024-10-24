## atfdb.citest -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Citest

* file:[data/atfdb/citest.ssim](/data/atfdb/citest.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|citest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"test"|CI job in which this test runs|
|sandbox|bool|[Val](/txt/exe/amc/reftypes.md#val)||Run test in sandbox|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [atf_ci -citest](/txt/exe/atf_ci/README.md) - Regx of tests to run 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FCitest](/txt/exe/atf_ci/internals.md#atf_ci-fcitest)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

