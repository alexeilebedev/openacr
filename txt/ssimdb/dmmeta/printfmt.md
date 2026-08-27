## dmmeta.printfmt - Print format
<a href="#dmmeta-printfmt"></a>
List of supported print formats

```
inline-command: acr printfmt | ssimfilt ^ -t
PRINTFMT    COMMENT
Auto        Neutral value: the strfmt's own shape; under strfmt:String that is Tuple
Bitset      Comma-separated list of names of set bits
CompactSep  Separator, omit default values. Declared only: amc generates no printer and no reader
Extern      User-provided function
Raw         Print all fields one after another
Sep         Separator
Tuple       Ssim tuple

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Printfmt

* file:[data/dmmeta/printfmt.ssim](/data/dmmeta/printfmt.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|printfmt|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.printfmt

* [dmmeta.cfmt via printfmt](/txt/ssimdb/dmmeta/cfmt.md) - Specify options for printing/reading ctypes into multiple formats
