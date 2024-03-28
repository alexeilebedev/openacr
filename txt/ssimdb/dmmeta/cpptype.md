## dmmeta.cpptype - Specify whether a ctype can be passed by value, and other c++ options


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cpptype

* file:[data/dmmeta/cpptype.ssim](/data/dmmeta/cpptype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|
|ctor|bool|Val|false|if true, generate non-default constructor from all fields|
|dtor|bool|Val|true|generate non-default destructor|
|cheap_copy|bool|Val|false|Pass by value whenever possible|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FCpptype
* [amc](/txt/exe/amc/README.md) as amc.FCpptype

