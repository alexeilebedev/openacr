## gclidb.grepo - Repo table (acr grepo -in ~/.ssim)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Grepo

* file:[data/gclidb/grepo.ssim](/data/gclidb/grepo.ssim)

italicised fields: *host, fname* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grepo|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||gcli host/full name|
|*host*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*gcli host<br>@LL of grepo*|
|*fname*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*gcli full name<br>@LR of grepo*|
|name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||gcli full name|
|token|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||gitlab project or repo access token|
|default_branch|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"origin"|gcli remote used in the workflow|
|keyid|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||gcli project id or repo|
|ssh_repo|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|web_url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|descr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|active|bool|[Val](/txt/exe/amc/reftype.md#val)|true|set to false to ignore the line but keep it in the file|
|priv|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGrepo](/txt/gen/gcli/gcli.md#gcli-fgrepo)
