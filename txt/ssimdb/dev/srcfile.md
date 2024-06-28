## dev.srcfile - Source file or header (key is pathname)


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Srcfile

* file:[data/dev/srcfile.ssim](/data/dev/srcfile.ssim)

italicised fields: *ext* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*ext*|*[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of srcfile*|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.srcfile

* [dev.include via srcfile](/txt/ssimdb/dev/include.md) - A site where one file includes another 
* [dev.include via filename](/txt/ssimdb/dev/include.md) - A site where one file includes another 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

