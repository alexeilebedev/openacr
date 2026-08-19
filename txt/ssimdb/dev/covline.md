## dev.covline -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Covline

* file:[data/dev/covline.ssim](/data/dev/covline.ssim)

italicised fields: *src, line* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|covline|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Key: file:line|
|*src*|*[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Source file<br>:RL of covline*|
|*line*|*u32*|*[Val](/txt/exe/amc/reftype.md#val)*||*Source line<br>:RR of covline*|
|flag|char|[Val](/txt/exe/amc/reftype.md#val)|'N'|Flag|
|hit|u32|[Val](/txt/exe/amc/reftype.md#val)||Number of hits|
|text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Line text|

### Constants
<a href="#constants"></a>
|Field|Fconst|Value|Comment|
|---|---|---|---|
|flag|
||N|'N'|Non-executable|
||E|'E'|Executable|
||P|'P'|Executable, partially executed|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FCovline](/txt/gen/atf_cov/atf_cov.md#atf_cov-fcovline)
