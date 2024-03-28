## dmmeta.strfmt - Cfmt conversion type (misnomer, not necessarily a string)
<a href="#dmmeta-strfmt"></a>

This table describes various string-based formats for a ctype.

```
inline-command: acr strfmt | ssimfilt ^ -t
STRFMT   COMMENT
Argv     Convert to/from command line
ArgvGnu  Convert to/from command line (use --key value)
Json     Convert to/from JSON tree
String   Convert to/from string
Tuple    Convert to/from array of key-value pairs

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Strfmt

* file:[data/dmmeta/strfmt.ssim](/data/dmmeta/strfmt.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|strfmt|algo.Smallstr50|Val|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.strfmt

* [dmmeta.cfmt via strfmt](/txt/ssimdb/dmmeta/cfmt.md) - Specify options for printing/reading ctypes into multiple formats

