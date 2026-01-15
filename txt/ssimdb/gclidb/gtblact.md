## gclidb.gtblact - static combination of gcli tables and actions


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblact

* file:[data/gclidb/gtblact.ssim](/data/gclidb/gtblact.ssim)

italicised fields: *gtbl, gact* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblact|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gtbl*|*[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>_LL of gtblact*|
|*gact*|*[gclidb.Gact](/txt/ssimdb/gclidb/gact.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>_LR of gtblact*|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|auth|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference gclidb.gtblact

* [gclidb.gtblactfld via gtblact](/txt/ssimdb/gclidb/gtblactfld.md) - List of available for each table & action combination 
* [gclidb.gtblacttst via gtblact](/txt/ssimdb/gclidb/gtblacttst.md) -  

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_gcli](/txt/exe/atf_gcli/internals.md) as [atf_gcli.FGtblact](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblact)
* [gcli](/txt/exe/gcli/internals.md) as [gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

