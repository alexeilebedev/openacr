## gclidb.mrjob - Mr job


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mrjob

* file:[data/gclidb/mrjob.ssim](/data/gclidb/mrjob.ssim)

italicised fields: *mr, job* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mrjob|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)||Identifier. must be in form mr.id|
|*mr*|*[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Mr portion of mr job<br>/RL of mrjob*|
|*job*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Id portion of mr job<br>/RR of mrjob*|
|ref|[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Mr portion of mr job|
|id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Job ID|
|name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Job name|
|pipeline_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Pipeline id|
|status|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|runner|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FMrjob](/txt/exe/gcli/internals.md#gcli-fmrjob)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

