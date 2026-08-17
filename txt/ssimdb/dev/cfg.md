## dev.cfg - Compiler configuration


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Cfg

* file:[data/dev/cfg.ssim](/data/dev/cfg.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cfg|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|suffix|[algo.Smallstr5](/txt/protocol/algo/README.md#algo-smallstr5)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference dev.cfg

* [dev.builddir via cfg](/txt/ssimdb/dev/builddir.md) - build config name (e.g. debug,release)

### Used In Commands
<a href="#used-in-commands"></a>
* [abt -cfg](/txt/exe/abt/README.md) - Set config 
* [atf_comp -cfg](/txt/exe/atf_comp/README.md) - Configuration (determines bindir) 
* [mdbg -cfg](/txt/exe/mdbg/README.md) - Configuration to use

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FCfg](/txt/gen/abt/abt.md#abt-fcfg)
* [mdbg](/txt/gen/mdbg/mdbg.md) as [mdbg.FCfg](/txt/gen/mdbg/mdbg.md#mdbg-fcfg)
