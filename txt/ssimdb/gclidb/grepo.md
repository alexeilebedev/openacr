## gclidb.grepo - Repo table (acr grepo -in ~/.ssim)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Grepo

* file:[data/gclidb/grepo.ssim](/data/gclidb/grepo.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grepo|algo.Smallstr250|Val||gcli host/full name|
|host|algo.cstring|Val||gcli host<br>@LL of grepo|
|fname|algo.cstring|Val||gcli full name<br>@LR of grepo|
|name|algo.cstring|Val||gcli full name|
|token|algo.cstring|Val||gitlab project or repo access token|
|default_branch|algo.cstring|Val|"origin"|gcli remote used in the workflow|
|keyid|algo.cstring|Val||gcli project id or repo|
|ssh_repo|algo.cstring|Val|
|web_url|algo.cstring|Val|
|descr|algo.cstring|Val|
|active|bool|Val|true|set to false to ignore the line but keep it in the file|
|priv|algo.cstring|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FGrepo

