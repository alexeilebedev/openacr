## dmmeta.fldoffset - Assert field offset - will result in compile-time error if violated
<a href="#dmmeta-fldoffset"></a>

The `dmmeta.fldoffset` table is similar to csize, but declares field ofset within a struct.
This, too, has no effect on code generation and results in a static assert being added
to a function `StaticCheck` in the target namespace.

Example:

```
dmmeta.fldoffset  field:algw1.AccountEnableMsg.account  offset:4
dmmeta.fldoffset  field:algw1.AccountEnableMsg.enable   offset:2
dmmeta.fldoffset  field:algw1.AccountEnableMsg.pad      offset:3
```

Generated code:

```
+    algo_assert(_offset_of(algw1::AccountEnableMsg, enable) == 2);
+    algo_assert(_offset_of(algw1::AccountEnableMsg, pad) == 3);
+    algo_assert(_offset_of(algw1::AccountEnableMsg, account) == 4);
```

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fldoffset

* file:[data/dmmeta/fldoffset.ssim](/data/dmmeta/fldoffset.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFldoffset](/txt/exe/amc/internals.md#amc-ffldoffset)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

