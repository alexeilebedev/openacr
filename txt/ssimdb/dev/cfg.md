## dev.cfg - Compiler configuration


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Cfg

* file:[data/dev/cfg.ssim](/data/dev/cfg.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cfg|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|suffix|[algo.Smallstr5](/txt/protocol/algo/README.md#algo-smallstr5)|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.cfg

* [dev.builddir via cfg](/txt/ssimdb/dev/builddir.md) - build config name (e.g. debug,release) 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [abt -cfg](/txt/exe/abt/README.md) - Set config 
* [atf_comp -cfg](/txt/exe/atf_comp/README.md) - Set config 
* [mdbg -cfg](/txt/exe/mdbg/README.md) - Configuration to use 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FCfg](/txt/exe/abt/internals.md#abt-fcfg)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FCfg](/txt/exe/atf_ci/internals.md#atf_ci-fcfg)
* [mdbg](/txt/exe/mdbg/internals.md) as [mdbg.FCfg](/txt/exe/mdbg/internals.md#mdbg-fcfg)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

