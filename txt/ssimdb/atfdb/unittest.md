## atfdb.unittest -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Unittest

* file:[data/atfdb/unittest.ssim](/data/atfdb/unittest.ssim)

italicised fields: *ns, testname* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|unittest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of unittest*|
|*testname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of unittest*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [atf_unit -unittest](/txt/exe/atf_unit/README.md) - SQL regex, selecting test to run 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_unit](/txt/exe/atf_unit/internals.md) as [atf_unit.FUnittest](/txt/exe/atf_unit/internals.md#atf_unit-funittest)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

