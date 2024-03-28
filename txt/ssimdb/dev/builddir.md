## dev.builddir - Directory where object files/executables go. Determines compile/link options


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Builddir

* file:[data/dev/builddir.ssim](/data/dev/builddir.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|builddir|algo.Smallstr50|Val||Primary key - uname.compiler.cfg-arch|
|uname|[dev.Uname](/txt/ssimdb/dev/uname.md)|Pkey||OS name (e.g. Linux,Darwin)<br>.LL-LL of builddir|
|compiler|[dev.Compiler](/txt/ssimdb/dev/compiler.md)|Pkey||Compiler name (e.g. g++,clang)<br>.LL-LR of builddir|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|Pkey||build config name (e.g. debug,release)<br>.LR-LL of builddir|
|arch|[dev.Arch](/txt/ssimdb/dev/arch.md)|Pkey||architecture name (e.g. x86)<br>.LR-LR of builddir|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FBuilddir
* [mdbg](/txt/exe/mdbg/README.md) as mdbg.FBuilddir

