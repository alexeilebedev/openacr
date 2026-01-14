## dev.targsyslib - Use of system library by target


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targsyslib

* file:[data/dev/targsyslib.ssim](/data/dev/targsyslib.ssim)

italicised fields: *target, syslib, uname, prefix* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsyslib|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LR.LL of targsyslib*|
|*syslib*|*[dev.Syslib](/txt/ssimdb/dev/syslib.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LR.LR of targsyslib*|
|*uname*|*[dev.Uname](/txt/ssimdb/dev/uname.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LL of targsyslib*|
|*prefix*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RL of targsyslib*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FTargsyslib](/txt/exe/abt/internals.md#abt-ftargsyslib)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

