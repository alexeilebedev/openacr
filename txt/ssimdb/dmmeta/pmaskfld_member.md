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
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.PmaskfldMember

* file:[data/dmmeta/pmaskfld_member.ssim](/data/dmmeta/pmaskfld_member.ssim)

italicised fields: *pmaskfld, field* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pmaskfld_member|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*pmaskfld*|*[dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/RL of pmaskfld_member*|
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/RR of pmaskfld_member*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FPmaskfldMember](/txt/exe/amc/internals.md#amc-fpmaskfldmember)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

