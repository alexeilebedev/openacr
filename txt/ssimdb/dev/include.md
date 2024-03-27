## dev.include - A site where one file includes another


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Include

* file:[data/dev/include.ssim](/data/dev/include.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|include|algo.Smallstr200|Val|
|srcfile|[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)|Pkey||<br>:LL of include|
|filename|[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)|Pkey||<br>:LR of include|
|sys|bool|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FInclude
* [src_lim](/txt/exe/src_lim/README.md) as src_lim.FInclude

