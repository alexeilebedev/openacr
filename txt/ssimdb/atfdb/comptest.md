## atfdb.comptest - A component test


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Comptest

* file:[data/atfdb/comptest.ssim](/data/atfdb/comptest.ssim)

italicised fields: *target, testname* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|comptest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*target*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.LL of comptest*|
|*testname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.LR of comptest*|
|timeout|i32|[Val](/txt/exe/amc/reftype.md#val)|10||
|memcheck|bool|[Val](/txt/exe/amc/reftype.md#val)|true||
|coverage|bool|[Val](/txt/exe/amc/reftype.md#val)|true||
|stablefld|bool|[Val](/txt/exe/amc/reftype.md#val)||Replace unstable fields with ***|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

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
* [atf_comp](/txt/gen/atf_comp/atf_comp.md) as [atf_comp.FComptest](/txt/gen/atf_comp/atf_comp.md#atf_comp-fcomptest)
