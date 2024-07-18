## dev.builddir - Directory where object files/executables go. Determines compile/link options


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Builddir

* file:[data/dev/builddir.ssim](/data/dev/builddir.ssim)

italicised fields: *uname, compiler, cfg, arch* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|builddir|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key - uname.compiler.cfg-arch|
|*uname*|*[dev.Uname](/txt/ssimdb/dev/uname.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*OS name (e.g. Linux,Darwin)<br>.LL-LL of builddir*|
|*compiler*|*[dev.Compiler](/txt/ssimdb/dev/compiler.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Compiler name (e.g. g++,clang)<br>.LL-LR of builddir*|
|*cfg*|*[dev.Cfg](/txt/ssimdb/dev/cfg.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*build config name (e.g. debug,release)<br>.LR-LL of builddir*|
|*arch*|*[dev.Arch](/txt/ssimdb/dev/arch.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*architecture name (e.g. x86)<br>.LR-LR of builddir*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FBuilddir](/txt/exe/abt/internals.md#abt-fbuilddir)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FBuilddir](/txt/exe/atf_ci/internals.md#atf_ci-fbuilddir)
* [mdbg](/txt/exe/mdbg/internals.md) as [mdbg.FBuilddir](/txt/exe/mdbg/internals.md#mdbg-fbuilddir)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

