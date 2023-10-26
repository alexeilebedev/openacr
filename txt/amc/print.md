## Reading and Printing

Amc can generate conversion functions for mapping between each ctype and a variety of formats.
Conversion code is requested with `cmft` record:

```
dmmeta.cfmt  cfmt:atf.Testrun.String          printfmt:Tuple  read:Y  print:Y  sep:""   genop:Y  comment:""
```

The primary key for cfmt is a combination of `ctype` and `strfmt`, where `strfmt` may be
`String`, `Json`, `Argv`, or `Tuple`.

Additionally, when `strfmt` is `String`,
`printfmt` field (somewhat confusinlgy confusingly named in a similar way to `strfmt`)
may be `Sep`, `CompactSep`, `Tuple` and `Auto`.

For all other conversion types, recognized values are `Extern` and `Auto`.
If `printfmt` is `Extern`, then a forward declaration is created for the underlying ctype
and nothing else is done.

### Strfmt:String with printfmt:Tuple

This is the main generator of code for reading and printing ctypes.
The tuple format is the default format for amc/acr types, and is the format used for all ssimfiles.
Each line has a type tag, which is just the name of the ctype, following by
space-separated key-value pairs. For instance,

```
dmmeta.ctype  ctype:amc.FCtype  comment:""
```

The key-value pairs may appear in any order. The value may contain any characters, using the same
character escape rules as c++. Amc supports efficient reading of tuples even for structs that contain
hundreds of fields using `FieldId` type.

For printing, the following rules hold:

- Only fields with reftype `Varlen`, `Val`, `Inlary`, `Smallstr`, `Ptr`, `Opt`, `Bitfld`, `RegSql` are considered.
- `Substr` and `Cppfunc` fields are not printed.
- `Srcfield` (underlying type for bitfields) is not printed as Tuple.
- `Pmaskfld` (field containing presence mask bits) is not printed as Tuple.
- `Lenfld` and `Typefld` are not printed as Tuple.
- If the ctype has a presence mask (`pmask`), then each field is printed only if present.
- Any bool stored as a bitfield whose underlying storage is `anonfld` is printed only if its value is true.
- An `Opt` field is printed only if present
- Inlary fields are not printed.
- `Varlen` fields of type `char` or `u8` are printed as a string (with correct escaping of characters)
- Elements of `Varlen` fields whose type is not char or u8 are printed using names and
successive integers, e.g. if the varlen
field has 2 elements with values x and y, attributes `name.0:x  name.1:y` are printed.

Thus, the tuple will always contain all field values, except for `false` bitfields  defined over
`anonfld` and unset fields with presence mask.

### Strfmt:String with printfmt:Sep or CompactSep

Amc generates code to print the ctype as a set of values (i.e. field names are not printed).
With `printfmt` equal to `Sep`, the length of `sep` attribute must be equal to the number
of fields in the struct minus one. The fields are printed one by one, separated by individual characters
as described by sep.

If `printfmt` is `CompactSep`, then the `sep` field is the separator string, and the same
separator string is placed between each field. When using `CompactSep`, `RegxSql` and `Val`
fields whose value is equal to their default are printed as empty strings.

### Strfmt:Json

Code is generated to print the ctype as a valid Json string.

### Strfmt:Argv

Code is generated to convert the ctype to a unix command line (array of pointers to C strings).
This is used to generate correct command line for subprocess invocation.

### Strfmt:Tuple

Code is generated to convert the ctype directly to a Tuple (array of key-value pairs).

