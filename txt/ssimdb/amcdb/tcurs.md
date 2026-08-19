## amcdb.tcurs - Cursor template


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tcurs

* file:[data/amcdb/tcurs.ssim](/data/amcdb/tcurs.ssim)

italicised fields: *curstype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|dflt|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate cursor by default? (if not, require fcurs)|
|*curstype*|*[amcdb.Curstype](/txt/ssimdb/amcdb/curstype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RR of tfunc*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTcurs](/txt/gen/amc/amc.md#amc-ftcurs)
