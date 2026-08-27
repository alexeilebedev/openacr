## atfdb.comptest - A component test


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Comptest

* file:[data/atfdb/comptest.ssim](/data/atfdb/comptest.ssim)

italicised fields: *target, testname* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|comptest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*target*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LL of comptest*|
|*testname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.LR of comptest*|
|timeout|i32|[Val](/txt/exe/amc/reftype/Val.md)|10||
|memcheck|bool|[Val](/txt/exe/amc/reftype/Val.md)|true||
|coverage|bool|[Val](/txt/exe/amc/reftype/Val.md)|true||
|stablefld|bool|[Val](/txt/exe/amc/reftype/Val.md)||Replace unstable fields with ***|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of atfdb.comptest

* [atfdb.Tfilt](/txt/ssimdb/atfdb/tfilt.md) -  
* [atfdb.Tifilt](/txt/ssimdb/atfdb/tifilt.md) - Input filter for component test

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_comp -comptest](/txt/exe/atf_comp/README.md) - Select comptest (SQL regex)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FComptest](/txt/gen/abt_md/abt_md.md#abt_md-fcomptest)
* [atf_comp](/txt/gen/atf_comp/atf_comp.md) as [atf_comp.FComptest](/txt/gen/atf_comp/atf_comp.md#atf_comp-fcomptest)
