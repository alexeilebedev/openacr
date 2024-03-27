## dev.tool_opt - Compiler/linker options to use


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.ToolOpt

* file:[data/dev/tool_opt.ssim](/data/dev/tool_opt.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tool_opt|algo.Smallstr100|Val||Primary key - uname.compiler-cfg.arch/target-opt_type:opt|
|uname|algo.Smallstr50|Val||match value of `uname`<br>/LL.LL-LL of tool_opt|
|compiler|algo.Smallstr50|Val||match compiler name<br>/LL.LL-LR of tool_opt|
|cfg|algo.Smallstr50|Val||match configuration name<br>/LL.LR-LL of tool_opt|
|arch|algo.Smallstr50|Val||match value of `arch` <br>/LL.LR-LR of tool_opt|
|target|algo.Smallstr50|Val||match name of target<br>/LR:LL-LL of tool_opt|
|opt_type|[dev.OptType](/txt/ssimdb/dev/opt_type.md)|Pkey||option type<br>/LR:LL-LR of tool_opt|
|opt|algo.Smallstr100|Val||option value<br>/LR:LR of tool_opt|
|sortfld|algo.Smallstr50|Val||uname-compiler - sort field<br>.LL of tool_opt|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FToolOpt

