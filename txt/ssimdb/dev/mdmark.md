## dev.mdmark - invisible markers for markdown files


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Mdmark

* file:[data/dev/mdmark.ssim](/data/dev/mdmark.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mdmark|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|state|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|param|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||parameter of mdmark - command or mdsection etc...|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Constants
<a href="#constants"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Constants -->
|Field|Fconst|Value|Comment|
|---|---|---|---|
|mdmark|
||MDSECTION|MDSECTION|mdsection marker|
||CMD|CMD|hidden inline-command marker|
||TOC|TOC|MYst Table of Contents directive|
|state|
||BEG|BEG|opening marker|
||END|END|closing marker|
||BEG_AUTO|BEG_AUTO|opening marker inserted by abt_md|
||END_AUTO|END_AUTO|closing marker inserted by abt_md|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Constants -->

