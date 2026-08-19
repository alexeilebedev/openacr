## dmmeta.dispsigcheck - Check signature of input data against executable's version


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispsigcheck

* file:[data/dmmeta/dispsigcheck.ssim](/data/dmmeta/dispsigcheck.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispsig|[dmmeta.Dispsig](/txt/ssimdb/dmmeta/dispsig.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [algo_lib](/txt/gen/algo_lib/algo_lib.md) as [algo_lib.FDispsigcheck](/txt/gen/algo_lib/algo_lib.md#algo_lib-fdispsigcheck)
