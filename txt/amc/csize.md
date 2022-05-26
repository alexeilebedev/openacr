## Amc: Size and field offset assertions

`amc` offers several mechanisms that help cross-reference a protocol description
that has been converted to ssim records with the intended original.

### dmmeta.csize: declare intended struct size

The `dmmeta.csize` table allows user to declare the intended size, in bytes, of each ctype,
and its alignment. For example:

```
dmmeta.csize  ctype:algw1.AccountEnableMsg         size:8   alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountLimitsMsg         size:88  alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountNoLoadMsg         size:8   alignment:1  comment:""
```

This declaration has no effect on the code generation. It is an assertion.
For each csize record, `amc` generates a static (compile-time) assertion in the `StaticCheck`
function in the target namespace. 
E.g.

```
    algo_assert(sizeof(algw1::AccountEnableMsg) == 8); // csize:algw1.AccountEnableMsg
```

### dmmeta.ctypelen: computed struct size (output)

For each ctype, `amc` creates a `ctypelen` record and saves the table to disk
upon completion. This allows querying the actual struct size, alignment, and number of
invisible pad bytes (inserted due to alignment) for any generated struct:

```
dmmeta.ctypelen  ctype:algw1.AccountEnableMsg  len:8  alignment:1  padbytes:0
...
dmmeta.ctypelen  ctype:algo_lib.FDb  len:9904  alignment:16  padbytes:70
```

### Fldoffset: declare intended field offset

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

### dmmeta.pack: Request 1-byte packing

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
