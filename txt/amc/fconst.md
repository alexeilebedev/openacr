## Fconst/Gconst: Enumerated Types

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
    
### Fcast
    
One thing that's useful is to be able to cast the wrapper struct to the enum type.
This is done by adding an fcast record:

```
    dmmeta.fcast  field:algo.Bool.value  expr:""  comment:""
```

This adds the following function to the `algo::Bool` type:

```
    inline operator algo_BoolEnum() const;
```
    
### Anonfld

Another commonly used feature is the `anonfld` record.

```
    dmmeta.anonfld  field:algo.Bool.value  comment:""
```
    
The `anonfld` record strips the name of the field from the enum constants. If there were no anonfld,
the enum's symbols would be named `algo_Bool_value_true`, etc. With anonfld, it becomes `algo_Bool_true`,
etc.

### Gconst: Taking values from a table
    
The related table is `gconst`, which allows specifying `fconsts` using two columns
from an arbitrary ssimfile. It's programmer's choice whether to use `fconst` or `gconst`.

Here are the function prototypes generated for `algo.Bool`:

```
inline-command: amc -report:N -proto algo.Bool.%
// Get value of field as enum type
algo_BoolEnum        value_GetEnum(const algo::Bool& parent) __attribute__((nothrow));
// Set value of field from enum type.
void                 value_SetEnum(algo::Bool& parent, algo_BoolEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
const char*          value_ToCstr(const algo::Bool& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
void                 value_Print(const algo::Bool& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
bool                 value_SetStrptrMaybe(algo::Bool& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
void                 value_SetStrptr(algo::Bool& parent, algo::strptr rhs, algo_BoolEnum dflt) __attribute__((nothrow));
// Set all fields to initial values.
void                 Bool_Init(algo::Bool& parent);
// Convert algo::Bool to a string (user-implemented function)
void                 Bool_Print(algo::Bool row, algo::cstring &str) __attribute__((nothrow));

```

The mapping from strings to integers is done using an efficient `static hash`,
which implements a hash lookup over a fixed set of values using a doubly nested
switch statement:

```
inline-command: amc -report:N  algo.Bool.%.SetStrptrMaybe

// --- algo.Bool.value.SetStrptrMaybe
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
bool algo::value_SetStrptrMaybe(algo::Bool& parent, algo::strptr rhs) {
    bool ret = false;
    switch (elems_N(rhs)) {
        case 1: {
            switch (u64(rhs[0])) {
                case '0': {
                    value_SetEnum(parent,algo_Bool_0); ret = true; break;
                }
                case '1': {
                    value_SetEnum(parent,algo_Bool_1); ret = true; break;
                }
                case 'N': {
                    value_SetEnum(parent,algo_Bool_N); ret = true; break;
                }
                case 'Y': {
                    value_SetEnum(parent,algo_Bool_Y); ret = true; break;
                }
            }
            break;
        }
        case 2: {
            switch (u64(algo::ReadLE16(rhs.elems))) {
                case LE_STR2('n','o'): {
                    value_SetEnum(parent,algo_Bool_no); ret = true; break;
                }
                case LE_STR2('o','n'): {
                    value_SetEnum(parent,algo_Bool_on); ret = true; break;
                }
            }
            break;
        }
        case 3: {
            switch (u64(algo::ReadLE16(rhs.elems))|(u64(rhs[2])<<16)) {
                case LE_STR3('o','f','f'): {
                    value_SetEnum(parent,algo_Bool_off); ret = true; break;
                }
                case LE_STR3('y','e','s'): {
                    value_SetEnum(parent,algo_Bool_yes); ret = true; break;
                }
            }
            break;
        }
        case 4: {
            switch (u64(algo::ReadLE32(rhs.elems))) {
                case LE_STR4('t','r','u','e'): {
                    value_SetEnum(parent,algo_Bool_true); ret = true; break;
                }
            }
            break;
        }
        case 5: {
            switch (u64(algo::ReadLE32(rhs.elems))|(u64(rhs[4])<<32)) {
                case LE_STR5('f','a','l','s','e'): {
                    value_SetEnum(parent,algo_Bool_false); ret = true; break;
                }
            }
            break;
        }
    }
    return ret;
}

```

