## dmmeta.pmaskfld_member - Explicit field-to-pmask association
<a href="#dmmeta-pmaskfld_member"></a>

Manually assign a field to a presence mask.
Here is an example:
```
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value1                                comment:""
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value3                                comment:""
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value5                                comment:""
```
See [pmaskfld](pmaskfld.md) for more information.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.PmaskfldMember

* file:[data/dmmeta/pmaskfld_member.ssim](/data/dmmeta/pmaskfld_member.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pmaskfld_member|algo.Smallstr100|Val|
|pmaskfld|[dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|Pkey||<br>/RL of pmaskfld_member|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>/RR of pmaskfld_member|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FPmaskfldMember

