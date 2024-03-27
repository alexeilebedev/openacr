## dmmeta.listtype - Specify structure of linked list based on field prefix


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Listtype

* file:[data/dmmeta/listtype.ssim](/data/dmmeta/listtype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|listtype|[dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Pkey|
|circular|bool|Val||Circular list|
|haveprev|bool|Val||Previous link|
|instail|bool|Val||Queue|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.listtype

* [dmmeta.llist via listtype](/txt/ssimdb/dmmeta/llist.md) - Options for Llist field

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FListtype
* [amc](/txt/exe/amc/README.md) as amc.FListtype
* [atf_amc](/txt/exe/atf_amc/README.md) as atf_amc.FListtype

