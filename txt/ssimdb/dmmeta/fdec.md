## dmmeta.fdec - Specify that field has an implied # of decimal places and specify formatting options
<a href="#dmmeta-fdec"></a>

Fdec specifies that a field is implicitly scaled by some power of 10.
Example:
```
dmmeta.fdec  field:algo.I32Dec4.value  nplace:4  fixedfmt:N  comment:""
```

The following functions are generated

```
inline-command: amc -report:N algo.I32Dec4.value.% -proto
// Set value of field value.
// The value is rounded to the nearest integer.
// This ensures that truncation of a near-integer value does not occur.
// Example: 1.3 cannot be represented exactly as a double, the actual
// stored value will be 1.29999999. when we apply C truncation,
// we want to end up with 1.3, not 1.2.
// func:algo.I32Dec4.value.qSetDouble
void                 value_qSetDouble(algo::I32Dec4& parent, double val) __attribute__((nothrow));
// func:algo.I32Dec4.value.GetDouble
double               value_GetDouble(const algo::I32Dec4& parent) __attribute__((nothrow));
// Return integer portion (divide number by 10000)
// func:algo.I32Dec4.value.GetInt
i32                  value_GetInt(const algo::I32Dec4& parent) __attribute__((nothrow));
// Return constant 10000
// func:algo.I32Dec4.value.GetScale
i32                  I32Dec4_GetScale() __attribute__((nothrow));
// Set value of field value, using rounding.
// If value is out of range for the target type, return false.
// func:algo.I32Dec4.value.SetDoubleMaybe
bool                 value_SetDoubleMaybe(algo::I32Dec4& parent, double val) __attribute__((nothrow));
// func:algo.I32Dec4.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(algo::I32Dec4& parent, algo::strptr in) __attribute__((nothrow));
// func:algo.I32Dec4.value.Print
void                 value_Print(algo::I32Dec4& parent, cstring &outstr) __attribute__((nothrow));
// func:algo.I32Dec4.value.Cast
operator i32         () const __attribute__((nothrow));

```

The `fixedfmt` option controls whether scientific notation is allowed to be used when printing such field.
Set it to `Y` to disable scientific notation.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fdec

* file:[data/dmmeta/fdec.ssim](/data/dmmeta/fdec.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Target field|
|nplace|i32|Val||Number of implied decimal places|
|fixedfmt|bool|Val||Print exactly SCALE chars after decimal point|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFdec

