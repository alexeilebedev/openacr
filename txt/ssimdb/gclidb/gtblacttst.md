## gclidb.gtblacttst -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtblacttst

* file:[data/gclidb/gtblacttst.ssim](/data/gclidb/gtblacttst.ssim)

italicised fields: *gtblact, gclienv, t* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtblacttst|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gtblact*|*[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of gtblacttst*|
|*gclienv*|*[gclidb.Gclienv](/txt/ssimdb/gclidb/gclienv.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LR.LL of gtblacttst*|
|working|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|*t*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of gtblacttst*|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference gclidb.gtblacttst

* [gclidb.gtblacttstout via gtblacttst](/txt/ssimdb/gclidb/gtblacttstout.md) -  

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [atf_gcli -gtblacttst](/txt/exe/atf_gcli/README.md) - gcli tests regx 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_gcli](/txt/exe/atf_gcli/internals.md) as [atf_gcli.FGtblacttst](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttst)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

