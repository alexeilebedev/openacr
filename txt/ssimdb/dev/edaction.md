## dev.edaction - acr_ed editing action (only one can exist at a time)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Edaction

* file:[data/dev/edaction.ssim](/data/dev/edaction.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|edaction|algo.Smallstr50|Val|
|edacttype|[dev.Edacttype](/txt/ssimdb/dev/edacttype.md)|Pkey||<br>_LL of edaction|
|name|algo.Smallstr50|Val||<br>_LR of edaction|
|needamc|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FEdaction

