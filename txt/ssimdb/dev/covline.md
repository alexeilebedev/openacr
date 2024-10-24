## dev.covline -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covline

* file:[data/dev/covline.ssim](/data/dev/covline.ssim)

italicised fields: *src, line* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covline|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)||file:line|
|*src*|*[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Source file<br>:RL of covline*|
|*line*|*u32*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Source line<br>:RR of covline*|
|flag|char|[Val](/txt/exe/amc/reftypes.md#val)|'N'|Flag|
|hit|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of hits|
|text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Line text|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Constants
<a href="#constants"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Constants -->
|Field|Fconst|Value|Comment|
|---|---|---|---|
|flag|
||N|'N'|Non-executable|
||E|'E'|Executable|
||P|'P'|Executable, partially executed|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Constants -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FCovline](/txt/exe/atf_cov/internals.md#atf_cov-fcovline)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

