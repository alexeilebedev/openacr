## dmmeta.fcmdline - Annotate field that holds process command line


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmdline

* file:[data/dmmeta/fcmdline.ssim](/data/dmmeta/fcmdline.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Parent field|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Parent namespace<br>.RL.RL of field|
|read|bool|Val|true|Read the command line automatically from main?|
|basecmdline|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Optional command line to merge with this one|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FFcmdline
* [amc](/txt/exe/amc/README.md) as amc.FFcmdline

