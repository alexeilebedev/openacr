## dmmeta.svtype - Table for determining ctype from separated value file
<a href="#dmmeta-svtype"></a>

This table helps `sv2ssim` map CSV fields to ctypes.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Svtype

* file:[data/dmmeta/svtype.ssim](/data/dmmeta/svtype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Type to choose|
|maxwid|i32|Val||Maximum width in chars of input field|
|fixedwid1|i32|Val||Max chars before decimal point|
|fixedwid2|i32|Val||Max chars after decimal point|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [sv2ssim](/txt/exe/sv2ssim/README.md) as sv2ssim.FSvtype

