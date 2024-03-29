## dmmeta.pmaskfld - Specify which field holds presence mask bits (one for each field in struct)
<a href="#dmmeta-pmaskfld"></a>

Indicate that the field holds a presence mask.
then amc allocates 1 bit in the pmask for each field in the struct. The bits are initially zero.
The field can be of any integer type, or an Inlary.

Amc then generates a Set, PresentQ, and SetPresent functions for each field, unless one already exists.
Whenever the Set function is called on a field, the corresponding pmask bit is set to 1.
When reading from a string, the pmask bits are populated for each scanned attribute.
When printing, fields with zero pmask bit are not printed.

Regardless of pmask status, all fields of a struct are initialized to default values upon construction,
so there is no speed to be gained from using pmask. Access to fields is not protected in any way --
they can be accessed as if the pmask didn't exist. The generated PresentQ function can be used to check
if a field was previously assigned a value with the `Set` function.

When reading a record from a string, `amc` uses the `Set` function if one exists. So, one can read
a tuple and know which attributes were present, by examining the pmask 

It is possible to have multiple `pmaskfld` fields in a struct, each for its own purpose.
In this case, use [pmaskfld_member](pmaskfld_member.md) to manually assign which fields
a given presence mask covers. If none are specified, then all fields are added to the presence mask.
If the `pmaskfld` field is named `present` or `pmask`, then `PresentQ`, `SetPresent` functions are generated.
If it's named `blah`, then the functions become `BlahQ`, `SetBlah`.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Pmaskfld

* file:[data/dmmeta/pmaskfld.ssim](/data/dmmeta/pmaskfld.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>.RL of field|
|filter_print|bool|Val|true|Omit non-present fields when printing|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.pmaskfld

* [dmmeta.pmaskfld_member via pmaskfld](/txt/ssimdb/dmmeta/pmaskfld_member.md) - Explicit field-to-pmask association

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FPmaskfld

