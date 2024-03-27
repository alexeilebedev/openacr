## dev.covfile -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covfile

* file:[data/dev/covfile.ssim](/data/dev/covfile.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey||Source file|
|total|u32|Val||Total lines|
|nonexe|u32|Val||Non-executable lines|
|exe|u32|Val||Executable lines|
|exer|algo.U32Dec2|Val||Percentage of executable lines|
|hit|u32|Val||Exercised lines|
|cov|algo.U32Dec2|Val||Line coverage|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FCovfile

