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
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Smallstr

* file:[data/dmmeta/smallstr.ssim](/data/dmmeta/smallstr.ssim)

italicised fields: *ctype* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of field*|
|length|i32|[Val](/txt/exe/amc/reftypes.md#val)||Maximum characters in the string|
|strtype|[dmmeta.Strtype](/txt/ssimdb/dmmeta/strtype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Data format for string|
|pad|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Pad character (if applicable)|
|strict|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.smallstr

* [dmmeta.Numstr](/txt/ssimdb/dmmeta/numstr.md) - Add functions to read numbers out of a string field 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr](/txt/exe/acr/internals.md) as [acr.FSmallstr](/txt/exe/acr/internals.md#acr-fsmallstr)
* [amc](/txt/exe/amc/internals.md) as [amc.FSmallstr](/txt/exe/amc/internals.md#amc-fsmallstr)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

