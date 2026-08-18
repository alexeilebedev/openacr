## atfdb.unittest -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Unittest

* file:[data/atfdb/unittest.ssim](/data/atfdb/unittest.ssim)

italicised fields: *ns, testname* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|unittest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of unittest*|
|*testname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of unittest*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_unit -unittest](/txt/exe/atf_unit/README.md) - SQL regex, selecting test to run

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_unit](/txt/gen/atf_unit/atf_unit.md) as [atf_unit.FUnittest](/txt/gen/atf_unit/atf_unit.md#atf_unit-funittest)
