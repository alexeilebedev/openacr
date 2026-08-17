## dev.sandbox - Registered sandbox: a named, resettable copy of the checkout


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Sandbox

* file:[data/dev/sandbox.ssim](/data/dev/sandbox.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|sandbox|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|cow|bool|[Val](/txt/exe/amc/reftype.md#val)||Materialize as cow farm when the host has cowdancer; worktree otherwise|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [wt -name](/txt/exe/wt/README.md) - Sandbox name

### Used In Executables
<a href="#used-in-executables"></a>
* [wt](/txt/gen/wt/wt.md) as [wt.FSandbox](/txt/gen/wt/wt.md#wt-fsandbox)
