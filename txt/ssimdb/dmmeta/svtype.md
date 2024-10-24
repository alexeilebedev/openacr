## dmmeta.svtype - Table for determining ctype from separated value file
<a href="#dmmeta-svtype"></a>

This table helps `sv2ssim` map CSV fields to ctypes.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Svtype

* file:[data/dmmeta/svtype.ssim](/data/dmmeta/svtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Type to choose|
|maxwid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Maximum width in chars of input field|
|fixedwid1|i32|[Val](/txt/exe/amc/reftypes.md#val)||Max chars before decimal point|
|fixedwid2|i32|[Val](/txt/exe/amc/reftypes.md#val)||Max chars after decimal point|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [sv2ssim](/txt/exe/sv2ssim/internals.md) as [sv2ssim.FSvtype](/txt/exe/sv2ssim/internals.md#sv2ssim-fsvtype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

