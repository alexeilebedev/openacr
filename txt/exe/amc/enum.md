## amc Enumerated Types
<a href="#amc-enumerated-types"></a>

You can associate several key-value pairs with a numeric field using `fconst` records.
For these, `amc` will generate a custom enumerated type whose c++ symbols are derived from the
field name and the value (or, if the field is `anonfld`, the ctype name and the value,
omitting the field name).

If the field has a numeric type, then a c++ enum is generated. If it's a string type,
then the symbols are defined global strings (`extern const char *`) whose value is the
fconst value.

For numeric types, when generating a Print function, the field will be printed as a symbol
if it matches one of the known values. Otherwise, the numeric value will be printed.
When generating a Read function, amc will first attempt to look up the input string, and if it
matches one of the defined fconst values, map it to the associated underlying value. If this
fails, amc will read the value as an integer.

Here is an example defining an fconst:

```
inline-command: acr -report:N field:algo.Bool.% -t | grep Bool.
    dmmeta.ctype  ctype:algo.Bool  comment:"Bool type for converting booleans to string"
      dmmeta.field  field:algo.Bool.value  arg:u8  reftype:Val  dflt:false  comment:""
        dmmeta.anonfld  field:algo.Bool.value  comment:""
        dmmeta.fcast  field:algo.Bool.value  expr:""  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/N      value:0  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/Y      value:1  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/true   value:1  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/false  value:0  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/0      value:0  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/1      value:1  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/off    value:0  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/on     value:1  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/no     value:0  comment:""
        dmmeta.fconst  fconst:algo.Bool.value/yes    value:1  comment:""
```

This translates to the following c++ code:

```
inline-command: grep -A 15 'BoolEnum.*{' include/gen/algo_gen.h
enum algo_BoolEnum {        // algo.Bool.value
     algo_Bool_N       = 0
    ,algo_Bool_Y       = 1
    ,algo_Bool_true    = 1
    ,algo_Bool_false   = 0
    ,algo_Bool_0       = 0
    ,algo_Bool_1       = 1
    ,algo_Bool_off     = 0
    ,algo_Bool_on      = 1
    ,algo_Bool_no      = 0
    ,algo_Bool_yes     = 1
};

enum { algo_BoolEnum_N = 10 };


```

And here is the generated struct definition:

```
// --- algo.Bool
struct Bool { // algo.Bool
    u8   value;   //   false
    explicit Bool(u8                             in_value);
    Bool(algo_BoolEnum arg);
    Bool();
};
```

### Constructing Enums from a table
<a href="#constructing-enums-from-a-table"></a>
Instead of defining enum symbols/values in the `fconst` table, you can take them
from any ssimfile, by adding a `gconst` record and specifying the ssimfile and two fields correspond
to the symbol and to the numeric value.

Here is an example. Let's consider a struct called `ws.StatusCode` (ws=WebSockets, an ad-hoc example)
with a single numeric field `value`:

```
dmmeta.ctype  ctype:ws.StatusCode  comment:""
  dmmeta.field  field:ws.StatusCode.value  arg:u16  reftype:Val  dflt:""  comment:""
    dmmeta.anonfld  field:ws.StatusCode.value  comment:""
    dmmeta.fcast  field:ws.StatusCode.value  expr:""  comment:""
    dmmeta.gconst  field:ws.StatusCode.value  namefld:wsdb.Status.reason  idfld:wsdb.Status.code  wantenum:N
```

Our status codes are in a table called `wsdb.Status`. Presumably this table was obtained
from the specification:

```
$ acr wsdb.status
wsdb.status  code:1000  reason:"Normal closure"                 local_only:N  comment:""
wsdb.status  code:1001  reason:"Going away"                     local_only:N  comment:"Server going down or browser page close"
wsdb.status  code:1002  reason:"Protocol error"                 local_only:N  comment:""
wsdb.status  code:1003  reason:"Unsupported message data type"  local_only:N  comment:"Text message while expecting binary and vice-versa"
wsdb.status  code:1005  reason:"No status code"                 local_only:Y  comment:"No status code received in remote close message"
wsdb.status  code:1006  reason:"Abnormal closure"               local_only:Y  comment:"Lower layer failure"
wsdb.status  code:1007  reason:"Invalid message data"           local_only:N  comment:"E.g. non-UTF-8 within text message"
wsdb.status  code:1008  reason:"Policy violation"               local_only:N  comment:""
wsdb.status  code:1009  reason:"Message too big"                local_only:N  comment:""
wsdb.status  code:1010  reason:"Missing extension"              local_only:N  comment:""
wsdb.status  code:1011  reason:"Internal error"                 local_only:N  comment:""
wsdb.status  code:1015  reason:"TLS Handshake"                  local_only:Y  comment:""
```

Here is what we get back from amc. First, the enum is defined:

```
// --- ws_StatusCodeEnum
enum ws_StatusCodeEnum {                                    // ws.StatusCode.value
    ws_StatusCode_Normal_closure                  = 1000
    ,ws_StatusCode_Going_away                      = 1001   // Server going down or browser page close
    ,ws_StatusCode_Protocol_error                  = 1002
    ,ws_StatusCode_Unsupported_message_data_type   = 1003   // Text message while expecting binary and vice-versa
    ,ws_StatusCode_No_status_code                  = 1005   // No status code received in remote close message
    ,ws_StatusCode_Abnormal_closure                = 1006   // Lower layer failure
    ,ws_StatusCode_Invalid_message_data            = 1007   // E.g. non-UTF-8 within text message
    ,ws_StatusCode_Policy_violation                = 1008
    ,ws_StatusCode_Message_too_big                 = 1009
    ,ws_StatusCode_Missing_extension               = 1010
    ,ws_StatusCode_Internal_error                  = 1011
    ,ws_StatusCode_TLS_Handshake                   = 1015
};
```

The `StatusCode` struct looks like this. Due to the use of `fcast` record,
we get an additional `operator ws_StatusCodeEnum`. The constructor from `u16` becomes explicit,
to prevent an unrelated numeric value from becoming `StatusCode`, and we get an additional
constructor for obtaining `StatusCode` from the associated enum type:

```
// --- ws.StatusCode
#pragma pack(push,1)
struct StatusCode { // ws.StatusCode
    u16   value;   //   0
    explicit StatusCode(u16 in_value);
    inline operator ws_StatusCodeEnum() const;
    StatusCode(ws_StatusCodeEnum arg);
    StatusCode();
};
#pragma pack(pop)
```
    
Next, we get a number of functions for setting/accessing the value:

```
// Get value of field as enum type
// func:ws.StatusCode.value.GetEnum
ws_StatusCodeEnum    value_GetEnum(const ws::StatusCode& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:ws.StatusCode.value.SetEnum
void                 value_SetEnum(ws::StatusCode& parent, ws_StatusCodeEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:ws.StatusCode.value.ToCstr
const char*          value_ToCstr(const ws::StatusCode& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
// func:ws.StatusCode.value.Print
void                 value_Print(const ws::StatusCode& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:ws.StatusCode.value.SetStrptrMaybe
bool                 value_SetStrptrMaybe(ws::StatusCode& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:ws.StatusCode.value.SetStrptr
void                 value_SetStrptr(ws::StatusCode& parent, algo::strptr rhs, ws_StatusCodeEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
// func:ws.StatusCode.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(ws::StatusCode& parent, algo::strptr rhs) __attribute__((nothrow));
```

The `gconst` record can also be used when `idfld` (id field) refers to a non-numeric value.

