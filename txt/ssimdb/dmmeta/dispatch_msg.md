## dmmeta.dispatch_msg - Add message to a dispatch


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.DispatchMsg

* file:[data/dmmeta/dispatch_msg.ssim](/data/dmmeta/dispatch_msg.ssim)

italicised fields: *dispatch, ctype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispatch_msg|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|*dispatch*|*[dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/RL of dispatch_msg*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/RR of dispatch_msg*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -dispatch_msg](/txt/exe/acr_ed/README.md) - Add dispatch_msg record (-create); pkey is <dispatch>/<msgtype>

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FDispatchmsg](/txt/gen/amc/amc.md#amc-fdispatchmsg)
