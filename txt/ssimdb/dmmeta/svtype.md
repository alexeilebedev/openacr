## dmmeta.svtype - Table for determining ctype from separated value file
<a href="#dmmeta-svtype"></a>

This table helps `sv2ssim` map CSV fields to ctypes.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Svtype

* file:[data/dmmeta/svtype.ssim](/data/dmmeta/svtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Type to choose|
|maxwid|i32|[Val](/txt/exe/amc/reftype/Val.md)||Maximum width in chars of input field|
|fixedwid1|i32|[Val](/txt/exe/amc/reftype/Val.md)||Max chars before decimal point|
|fixedwid2|i32|[Val](/txt/exe/amc/reftype/Val.md)||Max chars after decimal point|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [sv2ssim](/txt/gen/sv2ssim/sv2ssim.md) as [sv2ssim.FSvtype](/txt/gen/sv2ssim/sv2ssim.md#sv2ssim-fsvtype)
