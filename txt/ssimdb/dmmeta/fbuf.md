## dmmeta.fbuf - Buffer for reading/writing messages, works with Iohook


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fbuf

* file:[data/dmmeta/fbuf.ssim](/data/dmmeta/fbuf.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|fbufdir|[dmmeta.Fbufdir](/txt/ssimdb/dmmeta/fbufdir.md)|Pkey||<br>.RR_LL of field|
|max|u32|Val||Size of buffer in bytes|
|fbuftype|[dmmeta.Fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|Pkey||Type of buffer|
|insready|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|inseof|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFbuf

