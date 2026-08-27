## atfdb.citest -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Citest

* file:[data/atfdb/citest.ssim](/data/atfdb/citest.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|citest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Primary key|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|"normalize"|CI job in which this test runs|
|sandbox|bool|[Val](/txt/exe/amc/reftype/Val.md)||Run test in sandbox|
|timeout|i32|[Val](/txt/exe/amc/reftype/Val.md)|600|Max runtime budget for this citest, seconds (>= observed runtime doubled, floored at 10min)|
|failfast|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|A failure ends the run: remaining citests are skipped|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_ci -citest](/txt/exe/atf_ci/README.md) - Regx of tests to run

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FCitest](/txt/gen/atf_ci/atf_ci.md#atf_ci-fcitest)
