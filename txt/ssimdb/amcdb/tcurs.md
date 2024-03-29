## amcdb.tcurs - Cursor template


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tcurs

* file:[data/amcdb/tcurs.ssim](/data/amcdb/tcurs.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tfunc|[amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md)|Pkey|
|dflt|bool|Val||Generate cursor by default? (if not, require fcurs)|
|curstype|[amcdb.Curstype](/txt/ssimdb/amcdb/curstype.md)|Pkey||<br>.RR of tfunc|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FTcurs

