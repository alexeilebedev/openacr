## atfdb.comptest -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Comptest

* file:[data/atfdb/comptest.ssim](/data/atfdb/comptest.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|comptest|algo.Smallstr50|Val|
|target|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.LL of comptest|
|testname|algo.Smallstr50|Val||<br>.LR of comptest|
|timeout|i32|Val|10|
|memcheck|bool|Val|true|
|exit_code|u8|Val||Exit code to check|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of atfdb.comptest

* [atfdb.Targs](/txt/ssimdb/atfdb/targs.md) -  
* [atfdb.Tfilt](/txt/ssimdb/atfdb/tfilt.md) -

### Related
<a href="#related"></a>
These ssimfiles reference atfdb.comptest

* [atfdb.tmsg via comptest](/txt/ssimdb/atfdb/tmsg.md) -

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_comp -comptest](/txt/exe/atf_comp/README.md) - Select comptest (SQL regex)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FComptest
* [atf_comp](/txt/exe/atf_comp/README.md) as atf_comp.FComptest

