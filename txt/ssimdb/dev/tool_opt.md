## dev.tool_opt - Compiler/linker options to use


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.ToolOpt

* file:[data/dev/tool_opt.ssim](/data/dev/tool_opt.ssim)

italicised fields: *uname, compiler, cfg, arch, target, opt_type, opt, sortfld* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tool_opt|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)||Primary key - uname.compiler-cfg.arch/target-opt_type:opt|
|*uname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*match value of `uname`<br>/LL.LL-LL of tool_opt*|
|*compiler*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*match compiler name<br>/LL.LL-LR of tool_opt*|
|*cfg*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*match configuration name<br>/LL.LR-LL of tool_opt*|
|*arch*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*match value of `arch` <br>/LL.LR-LR of tool_opt*|
|*target*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*match name of target<br>/LR:LL-LL of tool_opt*|
|*opt_type*|*[dev.OptType](/txt/ssimdb/dev/opt_type.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*option type<br>/LR:LL-LR of tool_opt*|
|*opt*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftype.md#val)*||*option value<br>/LR:LR of tool_opt*|
|*sortfld*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*uname-compiler - sort field<br>.LL of tool_opt*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FToolOpt](/txt/gen/abt/abt.md#abt-ftoolopt)
