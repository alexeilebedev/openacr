## httpdb.status -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)httpdb.Status

* file:[data/httpdb/status.ssim](/data/httpdb/status.ssim)

italicised fields: *code, reason* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|status|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*code*|*u16*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br> LL of status*|
|*reason*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br> LR of status*|

### Used In Executables
<a href="#used-in-executables"></a>
* [lib_http](/txt/gen/lib_http/lib_http.md) as [lib_http.FStatus](/txt/gen/lib_http/lib_http.md#lib_http-fstatus)
