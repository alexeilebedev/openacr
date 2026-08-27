## dmmeta.ffunc - User-selected field function; pkey is <field>.<name> where name is a tfunc


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ffunc

* file:[data/dmmeta/ffunc.ssim](/data/dmmeta/ffunc.ssim)

italicised fields: *field, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ffunc|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Field that this function applies to<br>.RL of ffunc*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*Name of the tfunc to generate or externalize<br>.RR of ffunc*|
|extrn|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Y: user supplies the function body (extern); N: amc generates it|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFfunc](/txt/gen/amc/amc.md#amc-fffunc)
