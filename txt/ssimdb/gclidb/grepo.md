## gclidb.grepo - Repo table (acr grepo -in ~/.ssim)


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Grepo

* file:[data/gclidb/grepo.ssim](/data/gclidb/grepo.ssim)

italicised fields: *host, fname* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grepo|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)||gcli host/full name|
|*host*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*gcli host<br>@LL of grepo*|
|*fname*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*gcli full name<br>@LR of grepo*|
|name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||gcli full name|
|token|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||gitlab project or repo access token|
|default_branch|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"origin"|gcli remote used in the workflow|
|keyid|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||gcli project id or repo|
|ssh_repo|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|web_url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|descr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|active|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|set to false to ignore the line but keep it in the file|
|priv|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FGrepo](/txt/exe/gcli/internals.md#gcli-fgrepo)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

