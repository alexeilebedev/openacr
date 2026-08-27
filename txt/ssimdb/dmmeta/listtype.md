## dmmeta.listtype - Specify structure of linked list based on field prefix


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Listtype

* file:[data/dmmeta/listtype.ssim](/data/dmmeta/listtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|listtype|[algo.Smallstr5](/txt/protocol/algo/README.md#algo-smallstr5)|[Val](/txt/exe/amc/reftype/Val.md)||Field-name prefix naming this list shape|
|circular|bool|[Val](/txt/exe/amc/reftype/Val.md)||Circular list|
|haveprev|bool|[Val](/txt/exe/amc/reftype/Val.md)||Previous link|
|instail|bool|[Val](/txt/exe/amc/reftype/Val.md)||Queue|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.listtype

* [dmmeta.llist via listtype](/txt/ssimdb/dmmeta/llist.md) - Options for Llist field

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FListtype](/txt/gen/acr_ed/acr_ed.md#acr_ed-flisttype)
* [amc](/txt/gen/amc/amc.md) as [amc.FListtype](/txt/gen/amc/amc.md#amc-flisttype)
* [atf_amc](/txt/gen/atf_amc/atf_amc.md) as [atf_amc.FListtype](/txt/gen/atf_amc/atf_amc.md#atf_amc-flisttype)
