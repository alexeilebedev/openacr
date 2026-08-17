## atfdb.testenv -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Testenv

* file:[data/atfdb/testenv.ssim](/data/atfdb/testenv.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|testenv|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Value template;  substitutions from the test scope apply|
|slowonly|bool|[Val](/txt/exe/amc/reftype.md#val)||Set only when the run is instrumented and therefore slow|
|vardir|bool|[Val](/txt/exe/amc/reftype.md#val)||This directory holds data/atfdb/var.ssim, read once the test signals readiness|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_comp](/txt/gen/atf_comp/atf_comp.md) as [atf_comp.FTestenv](/txt/gen/atf_comp/atf_comp.md#atf_comp-ftestenv)
