## atfdb.citest -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Citest

* file:[data/atfdb/citest.ssim](/data/atfdb/citest.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|citest|algo.Smallstr50|Val||Primary key|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|Pkey|"test"|CI job in which this test runs|
|sandbox|bool|Val||Run test in sandbox|
|comment|algo.Comment|Val|

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_ci -citest](/txt/exe/atf_ci/README.md) - Regx of tests to run

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FCitest

