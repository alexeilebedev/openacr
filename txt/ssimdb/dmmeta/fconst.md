## dmmeta.fconst - Specify enum value (integer + string constant) for a field
<a href="#dmmeta-fconst"></a>

The `fconst` record associates a name/value pair with a field. When reading from string,
the name (or the numeric value) can be used. When printing, the appropriate name is printed
Here is an example:
``` 
inline-command: acr field:lib_json.FParser.state -l -tree -ndown 1
dmmeta.field  field:lib_json.FParser.state  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/ws          value:0   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/value       value:1   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/number      value:2   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str         value:3   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_esc     value:4   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_uesc    value:5   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_usurr1  value:6   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_usurr2  value:7   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/token       value:8   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/sec_line    value:9   comment:"JSON security line"
  dmmeta.fconst  fconst:lib_json.FParser.state/err         value:99  comment:"parser error"
report.acr  n_select:12  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

NOTE: The mapping from strings to integers is done using an efficient `static hash`.
The following functions are generated:
```
inline-command: amc lib_json.FParser.state.% -proto -report:N
// Get value of field as enum type
// func:lib_json.FParser.state.GetEnum
lib_json_FParser_state_Enum state_GetEnum(const lib_json::FParser& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:lib_json.FParser.state.SetEnum
void                 state_SetEnum(lib_json::FParser& parent, lib_json_FParser_state_Enum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:lib_json.FParser.state.ToCstr
const char*          state_ToCstr(const lib_json::FParser& parent) __attribute__((nothrow));
// Convert state to a string. First, attempt conversion to a known string.
// If no string matches, print state as a numeric value.
// func:lib_json.FParser.state.Print
void                 state_Print(const lib_json::FParser& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:lib_json.FParser.state.SetStrptrMaybe
bool                 state_SetStrptrMaybe(lib_json::FParser& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:lib_json.FParser.state.SetStrptr
void                 state_SetStrptr(lib_json::FParser& parent, algo::strptr rhs, lib_json_FParser_state_Enum dflt) __attribute__((nothrow));

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fconst

* file:[data/dmmeta/fconst.ssim](/data/dmmeta/fconst.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fconst|algo.Smallstr100|Val|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>/LL of fconst|
|value|algo.CppExpr|Val|
|name|algo.Smallstr100|Val||<br>/LR of fconst|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FFconst
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FFconst
* [amc](/txt/exe/amc/README.md) as amc.FFconst
* [lib_ctype](/txt/lib/lib_ctype/README.md) as lib_ctype.FFconst

