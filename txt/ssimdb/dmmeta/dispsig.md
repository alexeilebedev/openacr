## dmmeta.dispsig - Cryptographic signature of all dispatches


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Dispsig

* file:[data/dmmeta/dispsig.ssim](/data/dmmeta/dispsig.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dispsig|algo.Smallstr50|Val|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.RL of dispsig|
|name|algo.Smallstr50|Val||<br>.RR of dispsig|
|signature|algo.Sha1sig|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.dispsig

* [dmmeta.Dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md) - Check signature of input data against executable's version

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FDispsig
* [amc](/txt/exe/amc/README.md) as amc.FDispsig

