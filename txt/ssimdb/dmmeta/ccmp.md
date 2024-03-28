## dmmeta.ccmp - Generate hash function


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ccmp

* file:[data/dmmeta/ccmp.ssim](/data/dmmeta/ccmp.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Target ctype|
|extrn|bool|Val||Whether implementation is external|
|genop|bool|Val||Generate C++ comparison operators (<,>, etc)|
|order|bool|Val|
|minmax|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FCcmp

