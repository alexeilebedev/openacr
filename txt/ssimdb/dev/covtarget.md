## dev.covtarget -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covtarget

* file:[data/dev/covtarget.ssim](/data/dev/covtarget.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covtarget|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||Target|
|total|u32|Val||Total lines|
|nonexe|u32|Val||Non-executable lines|
|exe|u32|Val||Executable lines|
|exer|algo.U32Dec2|Val||Percentage of executable lines|
|hit|u32|Val||Exercised lines|
|cov|algo.U32Dec2|Val||Line coverage|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FCovtarget

