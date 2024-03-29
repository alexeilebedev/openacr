## dmmeta.smallstr - Generated fixed-length padded or length-delimited string field
<a href="#dmmeta-smallstr"></a>

Smallstr is a fixed-length character field. Memory is reserved inline in the parent struct.
Strings can be length-suffixed (Rpascal), left-padded or right-padded.
For padded strings, the string value is calculated by discarding the pad characters from the edge.
Any smallstr is castable to strptr.
The following small string types are supported:
```
inline-command: acr strtype | ssimfilt ^ -t
STRTYPE   COMMENT
rpascal   String of length N+2, last byte is the count
rightpad  String of length N, padded on the right
leftpad   String of length N, padded on the left

```
See [Strings](/txt/exe/amc/strings.md) for more information

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Smallstr

* file:[data/dmmeta/smallstr.ssim](/data/dmmeta/smallstr.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>.RL of field|
|length|i32|Val||Maximum characters in the string|
|strtype|[dmmeta.Strtype](/txt/ssimdb/dmmeta/strtype.md)|Pkey||Data format for string|
|pad|algo.CppExpr|Val||Pad character (if applicable)|
|strict|bool|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.smallstr

* [dmmeta.Numstr](/txt/ssimdb/dmmeta/numstr.md) - Add functions to read numbers out of a string field

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/exe/acr/README.md) as acr.FSmallstr
* [amc](/txt/exe/amc/README.md) as amc.FSmallstr

