## dev.edaction - acr_ed editing action (only one can exist at a time)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Edaction

* file:[data/dev/edaction.ssim](/data/dev/edaction.ssim)

italicised fields: *edacttype, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|edaction|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*edacttype*|*[dev.Edacttype](/txt/ssimdb/dev/edacttype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>_LL of edaction*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>_LR of edaction*|
|needamc|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FEdaction](/txt/gen/acr_ed/acr_ed.md#acr_ed-fedaction)
