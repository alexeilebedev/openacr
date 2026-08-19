## dev.srcfile - Source file or header (key is pathname)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Srcfile

* file:[data/dev/srcfile.ssim](/data/dev/srcfile.ssim)

italicised fields: *ext* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|*ext*|*[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of srcfile*|

### Related
<a href="#related"></a>
These ssimfiles reference dev.srcfile

* [dev.include via srcfile](/txt/ssimdb/dev/include.md) - A site where one file includes another 
* [dev.include via filename](/txt/ssimdb/dev/include.md) - A site where one file includes another

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FSrcfile](/txt/gen/abt/abt.md#abt-fsrcfile)
