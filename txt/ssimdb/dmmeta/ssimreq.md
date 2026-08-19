## dmmeta.ssimreq - Extended constraints for ssim records


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimreq

* file:[data/dmmeta/ssimreq.ssim](/data/dmmeta/ssimreq.ssim)

italicised fields: *parent_field, parent_ctype, value, child_ssimfile, child_key* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimreq|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|parent|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|*parent_field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>:LL of parent*|
|*parent_ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>:LL.RL of parent*|
|*value*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>:LR of parent*|
|*child_ssimfile*|*[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>:LL of ssimreq*|
|*child_key*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>:RR of ssimreq*|
|reqchild|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|bidir|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/gen/acr/acr.md) as [acr.FSsimreq](/txt/gen/acr/acr.md#acr-fssimreq)
* [apm](/txt/gen/apm/apm.md) as [apm.FSsimreq](/txt/gen/apm/apm.md#apm-fssimreq)
