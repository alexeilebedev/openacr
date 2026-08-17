## atfdb.citest -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Citest

* file:[data/atfdb/citest.ssim](/data/atfdb/citest.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|citest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)||Primary key|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"test"|CI job in which this test runs|
|sandbox|bool|[Val](/txt/exe/amc/reftype.md#val)||Run test in sandbox|
|timeout|i32|[Val](/txt/exe/amc/reftype.md#val)|600|Max runtime budget for this citest, seconds (>= observed runtime doubled, floored at 10min)|
|failfast|bool|[Val](/txt/exe/amc/reftype.md#val)|false|A failure ends the run: remaining citests are skipped|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_ci -citest](/txt/exe/atf_ci/README.md) - Regx of tests to run
