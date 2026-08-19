## dev.uncovfunc - Functions with no test coverage -- every executable line unhit across the suite


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Uncovfunc

* file:[data/dev/uncovfunc.ssim](/data/dev/uncovfunc.ssim)

italicised fields: *name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|uncovfunc|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Key: ns.name(args)|
|*name*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Qualified function name (ns.name), substr of the pkey before the '('<br>(LL of uncovfunc*|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FUncovfunc](/txt/gen/atf_cov/atf_cov.md#atf_cov-funcovfunc)
