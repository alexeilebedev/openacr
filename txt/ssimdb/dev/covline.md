## dev.covline -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covline

* file:[data/dev/covline.ssim](/data/dev/covline.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covline|algo.Smallstr200|Val||file:line|
|src|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey||Source file<br>:RL of covline|
|line|u32|Val||Source line<br>:RR of covline|
|flag|char|Val|'N'|Flag|
|hit|u32|Val||Number of hits|
|text|algo.cstring|Val||Line text|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FCovline

