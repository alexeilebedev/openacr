## dev.targdep - Dependency between targets


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targdep

* file:[data/dev/targdep.ssim](/data/dev/targdep.ssim)

italicised fields: *target, parent* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targdep|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key - target.parent|
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Child target<br>.RL of targdep*|
|*parent*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent target<br>.RR of targdep*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FTargdep](/txt/exe/abt/internals.md#abt-ftargdep)
* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FTargdep](/txt/exe/abt_md/internals.md#abt_md-ftargdep)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FTargdep](/txt/exe/acr_in/internals.md#acr_in-ftargdep)
* [amc](/txt/exe/amc/internals.md) as [amc.FTargdep](/txt/exe/amc/internals.md#amc-ftargdep)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

