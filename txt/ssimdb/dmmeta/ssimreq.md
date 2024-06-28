## dmmeta.ssimreq - Extended constraints for ssim records


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimreq

* file:[data/dmmeta/ssimreq.ssim](/data/dmmeta/ssimreq.ssim)

italicised fields: *parent_field, parent_ctype, value, child_ssimfile, child_key* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimreq|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|parent|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*parent_field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>:LL of parent*|
|*parent_ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>:LL.RL of parent*|
|*value*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>:LR of parent*|
|*child_ssimfile*|*[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>:LL of ssimreq*|
|*child_key*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>:RR of ssimreq*|
|reqchild|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|bidir|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr](/txt/exe/acr/internals.md) as [acr.FSsimreq](/txt/exe/acr/internals.md#acr-fssimreq)
* [apm](/txt/exe/apm/internals.md) as [apm.FSsimreq](/txt/exe/apm/internals.md#apm-fssimreq)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

