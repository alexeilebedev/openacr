## amcdb.tcurs - Cursor template


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tcurs

* file:[data/amcdb/tcurs.ssim](/data/amcdb/tcurs.ssim)

italicised fields: *curstype* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|dflt|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate cursor by default? (if not, require fcurs)|
|*curstype*|*[amcdb.Curstype](/txt/ssimdb/amcdb/curstype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of tfunc*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FTcurs](/txt/exe/amc/internals.md#amc-ftcurs)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

