## amc Feature: Fcmap — bidirectional enum mapping
<a href="#amc-feature-fcmap-bidirectional-enum-mapping"></a>

`dmmeta.fcmap` generates **conversion functions between two
enum-typed fields**, where each row of the table maps one
left-side value to one right-side value.  Use Fcmap to bridge
two enumerations that mean the same thing in different
namespaces (a WebSocket status code → an internal disconnect
reason, a Kafka API key → an amc message type, a vendor's FIX
tag → your canonical message-type fconst).

```
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:1000
              rightField:lib_ws.Reason.code  rightVal:normal  bidir:Y
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:1006
              rightField:lib_ws.Reason.code  rightVal:abnormal  bidir:Y
```

After amc runs, you get:

```c++
void ws::Disconnect_To_lib_ws_Reason(ws::Disconnect& rhs, lib_ws::Reason& lhs);
void ws::Disconnect_From_lib_ws_Reason(const lib_ws::Reason& rhs, ws::Disconnect& lhs);
```

…each a `switch` over the right-side fconst values that
assigns the matching left-side fconst value.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Default / wildcard rows](#default-wildcard-rows)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Bool target](#bool-target)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Ssim inputs
<a href="#ssim-inputs"></a>

Each `dmmeta.fcmap` row defines one mapping:

| Attribute     | Meaning |
|---------------|---------|
| `fcmap`       | Map name (string).  Multiple rows with the same name form one map. |
| `leftField`   | Pkey of the left field (full `<ctype>.<field>` path). |
| `leftVal`     | The left-side fconst value (or `*` for default). |
| `rightField`  | Pkey of the right field. |
| `rightVal`    | The right-side fconst value (or empty if the case has no mapping). |
| `bidir`       | When `Y`, generate both `_To<right>` and `_From<right>`.  When `N`, only one direction. |

The `leftField`/`rightField` types must both be either:

- a `Val` with associated `fconst` entries, or
- a `Bitfld` carved out of an integer with fconsts on it
  (amc handles the underlying bit shift).

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/fcmap.cpp`.

For each fcmap (and per direction when `bidir:Y`), amc emits:

| Generated function                                                   | Effect |
|----------------------------------------------------------------------|--------|
| `void <NSleft>::<Lhs>_To<NSright>_<Rhs>(Lhs& rhs, Rhs& lhs)`         | `switch (rhs.<leftField>)` and assign the matching `<rightVal>` to `lhs.<rightField>`. |
| `void <NSleft>::<Lhs>_From<NSright>_<Rhs>(const Rhs& rhs, Lhs& lhs)` | Inverse direction (emitted when `bidir:Y`). |

The switch is exhaustive over the left field's fconsts and
falls through to a default case (a `leftVal:*` row, if any).
Missing cases yield amc warnings — fcmap nudges you to keep
the mappings complete.

When the right field's fconsts include the `bool`-style
`true` / `false`, amc validates that every left value has a
mapping for both (since omitting one leaves the bool
undefined).

### Default / wildcard rows
<a href="#default-wildcard-rows"></a>

Set `leftVal:*` to declare a default for any left value not
otherwise enumerated:

```
dmmeta.fcmap  fcmap:vendor_to_amc  leftField:vendor.Msg.type  leftVal:*
              rightField:amc.Msg.type  rightVal:Unknown  bidir:N
```

This becomes the `default:` case of the generated switch.

### Bool target
<a href="#bool-target"></a>

When `rightField`'s arg is `bool`, amc emits a special
`bool true` / `bool false` validation step — both must have
explicit mappings in the fcmap; an empty `rightVal` is a hint
to set the bool to false.

### Pitfalls
<a href="#pitfalls"></a>

- **One row per mapping.**  Many-to-one needs multiple rows
  sharing a `rightVal`.  amc generates a `case` per left
  value, so the switch may visit the same arm repeatedly.
- **Namespace dependency.**  amc walks
  `ns->c_cppincl/hdrincl/parentns` to confirm the left
  namespace can reach the right namespace's header.
  Cross-namespace fcmap without an include relationship is
  rejected (see `DependentQ` in `cpp/amc/fcmap.cpp`).
- **`bidir:Y`** generates two switches; if the mapping isn't
  actually injective, the inverse direction is lossy and amc
  warns.
- **No partial maps.**  Each fcmap's left field must be fully
  covered (every fconst value mapped, plus optional `*`).
- **`leftVal:*` only works as a default**; you can't have two
  wildcard rows in the same fcmap.
- **`fcmap` is a single string ID,** not a (left,right)
  pair — group rows by giving them the same `fcmap` value.

### See also
<a href="#see-also"></a>

- [Enums](/txt/exe/amc/enum.md) — the fconst foundation
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — fcmap supports bitfld-typed fields
- Source: `cpp/amc/fcmap.cpp`
- Spec table: `acr 'dmmeta.fcmap:%'`

### Example
<a href="#example"></a>

Mapping WebSocket close codes to internal reason enums:

```
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:1000
              rightField:lib_ws.Reason.code  rightVal:normal  bidir:Y
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:1001
              rightField:lib_ws.Reason.code  rightVal:going_away  bidir:Y
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:1006
              rightField:lib_ws.Reason.code  rightVal:abnormal  bidir:Y
dmmeta.fcmap  fcmap:ws_to_int  leftField:ws.Disconnect.code  leftVal:*
              rightField:lib_ws.Reason.code  rightVal:other  bidir:Y
```

Generates:

```c++
void ws::Disconnect_To_lib_ws_Reason(ws::Disconnect& rhs, lib_ws::Reason& lhs) {
    switch (rhs.code) {
        case ws_Disconnect_code_1000: lhs.code = lib_ws_Reason_code_normal;     break;
        case ws_Disconnect_code_1001: lhs.code = lib_ws_Reason_code_going_away; break;
        case ws_Disconnect_code_1006: lhs.code = lib_ws_Reason_code_abnormal;   break;
        default:                      lhs.code = lib_ws_Reason_code_other;      break;
    }
}
void ws::Disconnect_From_lib_ws_Reason(const lib_ws::Reason& rhs, ws::Disconnect& lhs) {
    // the same switch, read right to left
}
```

Calls in user code:

```c++
ws::Disconnect       wire;
lib_ws::Reason       canon;
wire.code = 1006;
ws::Disconnect_To_lib_ws_Reason(wire, canon);
// canon.code == lib_ws_Reason_code_abnormal
```
