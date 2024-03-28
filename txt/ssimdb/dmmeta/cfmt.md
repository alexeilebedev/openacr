## dmmeta.cfmt - Specify options for printing/reading ctypes into multiple formats


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cfmt

* file:[data/dmmeta/cfmt.ssim](/data/dmmeta/cfmt.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cfmt|algo.Smallstr100|Val|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>.RL of cfmt|
|strfmt|[dmmeta.Strfmt](/txt/ssimdb/dmmeta/strfmt.md)|Pkey||<br>.RR of cfmt|
|printfmt|[dmmeta.Printfmt](/txt/ssimdb/dmmeta/printfmt.md)|Pkey|
|read|bool|Val|
|print|bool|Val|
|sep|algo.Smallstr20|Val|
|genop|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FCfmt
* [amc](/txt/exe/amc/README.md) as amc.FCfmt
* [lib_ctype](/txt/lib/lib_ctype/README.md) as lib_ctype.FCfmt

