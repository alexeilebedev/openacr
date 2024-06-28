## dmmeta.listtype - Specify structure of linked list based on field prefix


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Listtype

* file:[data/dmmeta/listtype.ssim](/data/dmmeta/listtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|listtype|[dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|circular|bool|[Val](/txt/exe/amc/reftypes.md#val)||Circular list|
|haveprev|bool|[Val](/txt/exe/amc/reftypes.md#val)||Previous link|
|instail|bool|[Val](/txt/exe/amc/reftypes.md#val)||Queue|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.listtype

* [dmmeta.llist via listtype](/txt/ssimdb/dmmeta/llist.md) - Options for Llist field 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FListtype](/txt/exe/acr_ed/internals.md#acr_ed-flisttype)
* [amc](/txt/exe/amc/internals.md) as [amc.FListtype](/txt/exe/amc/internals.md#amc-flisttype)
* [atf_amc](/txt/exe/atf_amc/internals.md) as [atf_amc.FListtype](/txt/exe/atf_amc/internals.md#atf_amc-flisttype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

