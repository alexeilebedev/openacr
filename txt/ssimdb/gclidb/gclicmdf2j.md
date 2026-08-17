## gclidb.gclicmdf2j - Mapping of internal fields to JSON fields for gitlab/github


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclicmdf2j

* file:[data/gclidb/gclicmdf2j.ssim](/data/gclidb/gclicmdf2j.ssim)

italicised fields: *gclicmdf, gclicmd, field, jkey* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclicmdf2j|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)|||
|*gclicmdf*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>@LL of gclicmdf2j*|
|*gclicmd*|*[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LL of gclicmdf2j*|
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LR@LL of gclicmdf2j*|
|*jkey*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>/LR@LR of gclicmdf2j*|
|dup|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGclicmdf2j](/txt/gen/gcli/gcli.md#gcli-fgclicmdf2j)
