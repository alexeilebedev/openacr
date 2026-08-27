## dmmeta.strtype - Type of small string
<a href="#dmmeta-strtype"></a>

This table describes small string types
```
inline-command: acr strtype | ssimfilt ^ -t
STRTYPE   COMMENT
rpascal   String of length N+2, last byte is the count
rightpad  String of length N, padded on the right
leftpad   String of length N, padded on the left

```
For more information, see [Strings](/txt/exe/amc/string.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Strtype

* file:[data/dmmeta/strtype.ssim](/data/dmmeta/strtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|strtype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.strtype

* [dmmeta.smallstr via strtype](/txt/ssimdb/dmmeta/smallstr.md) - Data format for string
