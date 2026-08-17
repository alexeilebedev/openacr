## dmmeta.disptrace - Generate trace fields (cycles, counts) for all dispatch branches


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Disptrace

* file:[data/dmmeta/disptrace.ssim](/data/dmmeta/disptrace.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispatch|[dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|cycle|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FDisptrace](/txt/gen/amc/amc.md#amc-fdisptrace)
