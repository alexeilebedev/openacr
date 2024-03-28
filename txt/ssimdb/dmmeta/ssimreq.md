## dmmeta.ssimreq - Extended constraints for ssim records


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimreq

* file:[data/dmmeta/ssimreq.ssim](/data/dmmeta/ssimreq.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimreq|algo.Smallstr100|Val|
|parent|algo.Smallstr100|Val|
|parent_field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>:LL of parent|
|parent_ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>:LL.RL of parent|
|value|algo.Smallstr100|Val||<br>:LR of parent|
|child_ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Pkey||<br>:LL of ssimreq|
|child_key|algo.Smallstr100|Val||<br>:RR of ssimreq|
|reqchild|bool|Val|
|bidir|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/exe/acr/README.md) as acr.FSsimreq
* [apm](/txt/exe/apm/README.md) as apm.FSsimreq

