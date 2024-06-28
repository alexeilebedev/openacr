## dev.tool_opt - Compiler/linker options to use


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.ToolOpt

* file:[data/dev/tool_opt.ssim](/data/dev/tool_opt.ssim)

italicised fields: *uname, compiler, cfg, arch, target, opt_type, opt, sortfld* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tool_opt|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key - uname.compiler-cfg.arch/target-opt_type:opt|
|*uname*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*match value of `uname`<br>/LL.LL-LL of tool_opt*|
|*compiler*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*match compiler name<br>/LL.LL-LR of tool_opt*|
|*cfg*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*match configuration name<br>/LL.LR-LL of tool_opt*|
|*arch*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*match value of `arch` <br>/LL.LR-LR of tool_opt*|
|*target*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*match name of target<br>/LR:LL-LL of tool_opt*|
|*opt_type*|*[dev.OptType](/txt/ssimdb/dev/opt_type.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*option type<br>/LR:LL-LR of tool_opt*|
|*opt*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*option value<br>/LR:LR of tool_opt*|
|*sortfld*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*uname-compiler - sort field<br>.LL of tool_opt*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FToolOpt](/txt/exe/abt/internals.md#abt-ftoolopt)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

