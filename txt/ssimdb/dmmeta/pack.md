## dmmeta.pack - Request byte-packing of structure fields
<a href="#dmmeta-pack"></a>

Using the `dmmeta.pack` record you can request that the generated struct be wrapped
with the `#pragma pack`:

```
dmmeta.pack  ctype:algw1.AccountEnableMsg       comment:""
```

Here is the generated code:

```
// --- algw1.AccountEnableMsg
#pragma pack(push,1)
struct AccountEnableMsg { // algw1.AccountEnableMsg
    explicit AccountEnableMsg(bool                           in_enable
        ,i32                            in_account);
    AccountEnableMsg();
};
#pragma pack(pop)
```

Another way to make sure that ALL structs in a namespace are 1-byte packed
is to set the attribute `pack:Y` in the `nsx` (namespace extra) record:

```
dmmeta.nsx  ns:algw1  genthrow:N  correct_getorcreate:Y  pool:algo_lib.FDb.lpool  sortxref:N  pack:Y  fldoffset_asserts:Y  comment:""
```

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Pack

* file:[data/dmmeta/pack.ssim](/data/dmmeta/pack.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Target ctype|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FPack](/txt/exe/acr_ed/internals.md#acr_ed-fpack)
* [amc](/txt/exe/amc/internals.md) as [amc.FPack](/txt/exe/amc/internals.md#amc-fpack)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

