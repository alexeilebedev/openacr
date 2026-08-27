## dmmeta.fpbufcase -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fpbufcase

* file:[data/dmmeta/fpbufcase.ssim](/data/dmmeta/fpbufcase.ssim)

italicised fields: *fpbuf, oneof, ctype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fpbufcase|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*fpbuf*|*[dmmeta.Fpbuf](/txt/ssimdb/dmmeta/fpbuf.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>/LL of fpbufcase*|
|*oneof*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>/LR of fpbufcase*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>/LL.RL of fpbufcase*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFpbufcase](/txt/gen/amc/amc.md#amc-ffpbufcase)
