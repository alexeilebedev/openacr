## dmmeta.finsertwhen - Switch -<flag> to insert-completion mode when <when> flag is also set


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Finsertwhen

* file:[data/dmmeta/finsertwhen.ssim](/data/dmmeta/finsertwhen.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|finsertwhen|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Field that gets insert-completion|
|when|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||When this flag is also set on the cmdline, use insert-completion for finsertwhen.field|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FFinsertwhen](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffinsertwhen)
