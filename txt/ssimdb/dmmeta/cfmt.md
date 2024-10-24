## dmmeta.cfmt - Specify options for printing/reading ctypes into multiple formats


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cfmt

* file:[data/dmmeta/cfmt.ssim](/data/dmmeta/cfmt.ssim)

italicised fields: *ctype, strfmt* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cfmt|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of cfmt*|
|*strfmt*|*[dmmeta.Strfmt](/txt/ssimdb/dmmeta/strfmt.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of cfmt*|
|printfmt|[dmmeta.Printfmt](/txt/ssimdb/dmmeta/printfmt.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|read|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|print|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|sep|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)|||
|genop|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FCfmt](/txt/exe/acr_ed/internals.md#acr_ed-fcfmt)
* [amc](/txt/exe/amc/internals.md) as [amc.FCfmt](/txt/exe/amc/internals.md#amc-fcfmt)
* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FCfmt](/txt/lib/lib_ctype/README.md#lib_ctype-fcfmt)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

