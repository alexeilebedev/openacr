## dmmeta.nsjs - Generate JavaScript code for this namespace


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Nsjs

* file:[data/dmmeta/nsjs.ssim](/data/dmmeta/nsjs.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|typescript|bool|[Val](/txt/exe/amc/reftype/Val.md)||Generate typescript|
|ifaceonly|bool|[Val](/txt/exe/amc/reftype/Val.md)||Project ctypes as interfaces named after the ctype; emit no runtime class|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FNsjs](/txt/gen/amc/amc.md#amc-fnsjs)
