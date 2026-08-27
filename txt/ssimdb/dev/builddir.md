## dev.builddir - Directory where object files/executables go. Determines compile/link options


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Builddir

* file:[data/dev/builddir.ssim](/data/dev/builddir.ssim)

italicised fields: *uname, compiler, cfg, arch* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|builddir|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Primary key - uname.compiler.cfg-arch|
|*uname*|*[dev.Uname](/txt/ssimdb/dev/uname.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*OS name (e.g. Linux,Darwin)<br>.LL-LL of builddir*|
|*compiler*|*[dev.Compiler](/txt/ssimdb/dev/compiler.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Compiler name (e.g. g++,clang)<br>.LL-LR of builddir*|
|*cfg*|*[dev.Cfg](/txt/ssimdb/dev/cfg.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*build config name (e.g. debug,release)<br>.LR-LL of builddir*|
|*arch*|*[dev.Arch](/txt/ssimdb/dev/arch.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*architecture name (e.g. x86)<br>.LR-LR of builddir*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FBuilddir](/txt/gen/abt/abt.md#abt-fbuilddir)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FBuilddir](/txt/gen/atf_ci/atf_ci.md#atf_ci-fbuilddir)
* [mdbg](/txt/gen/mdbg/mdbg.md) as [mdbg.FBuilddir](/txt/gen/mdbg/mdbg.md#mdbg-fbuilddir)
