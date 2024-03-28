## gclidb.gclicmdf2j - Mapping of internal fields to JSON fields for gitlab/github


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclicmdf2j

* file:[data/gclidb/gclicmdf2j.ssim](/data/gclidb/gclicmdf2j.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclicmdf2j|algo.Smallstr250|Val|
|gclicmdf|algo.cstring|Val||<br>@LL of gclicmdf2j|
|gclicmd|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|Pkey||<br>/LL of gclicmdf2j|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>/LR@LL of gclicmdf2j|
|jkey|algo.cstring|Val||<br>/LR@LR of gclicmdf2j|
|dup|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FGclicmdf2j

