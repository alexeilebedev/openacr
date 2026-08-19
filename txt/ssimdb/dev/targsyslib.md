## dev.targsyslib - Use of system library by target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targsyslib

* file:[data/dev/targsyslib.ssim](/data/dev/targsyslib.ssim)

italicised fields: *target, syslib, uname, prefix* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsyslib|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LR.LL of targsyslib*|
|*syslib*|*[dev.Syslib](/txt/ssimdb/dev/syslib.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LR.LR of targsyslib*|
|*uname*|*[dev.Uname](/txt/ssimdb/dev/uname.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LL of targsyslib*|
|*prefix*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RL of targsyslib*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FTargsyslib](/txt/gen/abt/abt.md#abt-ftargsyslib)
