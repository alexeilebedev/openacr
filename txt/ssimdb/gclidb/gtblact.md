## gclidb.gtblact - static combination of gcli tables and actions


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblact

* file:[data/gclidb/gtblact.ssim](/data/gclidb/gtblact.ssim)

italicised fields: *gtbl, gact* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblact|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*gtbl*|*[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>_LL of gtblact*|
|*gact*|*[gclidb.Gact](/txt/ssimdb/gclidb/gact.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>_LR of gtblact*|
|t|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|auth|bool|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.gtblact

* [gclidb.gtblactfld via gtblact](/txt/ssimdb/gclidb/gtblactfld.md) - List of available for each table & action combination

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGtblact](/txt/gen/gcli/gcli.md#gcli-fgtblact)
