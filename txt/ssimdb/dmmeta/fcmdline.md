## dmmeta.fcmdline - Annotate field that holds process command line


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmdline

* file:[data/dmmeta/fcmdline.ssim](/data/dmmeta/fcmdline.ssim)

italicised fields: *ns* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Parent field|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent namespace<br>.RL.RL of field*|
|read|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Read the command line automatically from main?|
|basecmdline|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Optional command line to merge with this one|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_compl](/txt/exe/acr_compl/internals.md) as [acr_compl.FFcmdline](/txt/exe/acr_compl/internals.md#acr_compl-ffcmdline)
* [amc](/txt/exe/amc/internals.md) as [amc.FFcmdline](/txt/exe/amc/internals.md#amc-ffcmdline)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

