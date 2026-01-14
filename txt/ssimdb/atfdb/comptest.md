## atfdb.comptest -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Comptest

* file:[data/atfdb/comptest.ssim](/data/atfdb/comptest.ssim)

italicised fields: *target, testname* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|comptest|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*target*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of comptest*|
|*testname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.LR of comptest*|
|timeout|i32|[Val](/txt/exe/amc/reftypes.md#val)|10||
|memcheck|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|coverage|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|exit_code|u8|[Val](/txt/exe/amc/reftypes.md#val)||Exit code to check|
|ncore|i32|[Val](/txt/exe/amc/reftypes.md#val)|1||
|repeat|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Number of times to repeat the test|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of atfdb.comptest

* [atfdb.Targs](/txt/ssimdb/atfdb/targs.md) -  
* [atfdb.Tfilt](/txt/ssimdb/atfdb/tfilt.md) -  

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference atfdb.comptest

* [atfdb.tmsg via comptest](/txt/ssimdb/atfdb/tmsg.md) -  

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [atf_comp -comptest](/txt/exe/atf_comp/README.md) - Select comptest (SQL regex) 
* [atf_exp -comptest](/txt/exe/atf_exp/README.md) - Component test name 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FComptest](/txt/exe/abt_md/internals.md#abt_md-fcomptest)
* [atf_comp](/txt/exe/atf_comp/internals.md) as [atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

