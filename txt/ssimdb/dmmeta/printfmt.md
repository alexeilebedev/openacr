## dmmeta.printfmt - Print format
<a href="#dmmeta-printfmt"></a>
List of supported print formats

```
inline-command: acr printfmt | ssimfilt ^ -t
PRINTFMT    COMMENT
Auto        Decide between Raw and Tuple based on # of fields
Bitset      Comma-separated list of names of set bits
CompactSep  Separator, omit default values.
Extern      User-provided function
Raw         Print all fields one after another
Sep         Separator
Tuple       Ssim tuple

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Printfmt

* file:[data/dmmeta/printfmt.ssim](/data/dmmeta/printfmt.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|printfmt|algo.Smallstr50|Val|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.printfmt

* [dmmeta.cfmt via printfmt](/txt/ssimdb/dmmeta/cfmt.md) - Specify options for printing/reading ctypes into multiple formats

