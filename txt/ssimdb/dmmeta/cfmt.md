## dmmeta.cfmt - Specify options for printing/reading ctypes into multiple formats


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cfmt

* file:[data/dmmeta/cfmt.ssim](/data/dmmeta/cfmt.ssim)

italicised fields: *ctype, strfmt* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cfmt|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of cfmt*|
|*strfmt*|*[dmmeta.Strfmt](/txt/ssimdb/dmmeta/strfmt.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RR of cfmt*|
|printfmt|[dmmeta.Printfmt](/txt/ssimdb/dmmeta/printfmt.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|read|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|print|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|sep|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)|||
|genop|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FCfmt](/txt/gen/acr_ed/acr_ed.md#acr_ed-fcfmt)
* [amc](/txt/gen/amc/amc.md) as [amc.FCfmt](/txt/gen/amc/amc.md#amc-fcfmt)
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FCfmt](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-fcfmt)
