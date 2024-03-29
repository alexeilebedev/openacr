## AMC Enumerated Types
<a href="#amc-enumerated-types"></a>

You can associate several key-value pairs with a numeric field using `fconst` records.
For these, `amc` will generate a custom enumerated type whose c++ symbols are derived from the
field name and the value (or, if the field is `anonfld`, the ctype name and the value).

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
    dmmeta.ctype  ctype:algo.Bool  comment:""
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

