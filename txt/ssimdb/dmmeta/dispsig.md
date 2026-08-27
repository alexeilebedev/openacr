## dmmeta.dispsig - Cryptographic signature of all dispatches


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispsig

* file:[data/dmmeta/dispsig.ssim](/data/dmmeta/dispsig.ssim)

italicised fields: *ns, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispsig|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of dispsig*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.RR of dispsig*|
|signature|[algo.Signature](/txt/protocol/algo/Signature.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.dispsig

* [dmmeta.Dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md) - Check signature of input data against executable's version

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FDispsig](/txt/gen/acr_in/acr_in.md#acr_in-fdispsig)
* [amc](/txt/gen/amc/amc.md) as [amc.FDispsig](/txt/gen/amc/amc.md#amc-fdispsig)
