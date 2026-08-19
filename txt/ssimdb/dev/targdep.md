## dev.targdep - Dependency between targets


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targdep

* file:[data/dev/targdep.ssim](/data/dev/targdep.ssim)

italicised fields: *target, parent* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targdep|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)||Primary key - target.parent|
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Child target<br>.RL of targdep*|
|*parent*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Parent target<br>.RR of targdep*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FTargdep](/txt/gen/abt/abt.md#abt-ftargdep)
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FTargdep](/txt/gen/abt_md/abt_md.md#abt_md-ftargdep)
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FTargdep](/txt/gen/acr_in/acr_in.md#acr_in-ftargdep)
* [amc](/txt/gen/amc/amc.md) as [amc.FTargdep](/txt/gen/amc/amc.md#amc-ftargdep)
