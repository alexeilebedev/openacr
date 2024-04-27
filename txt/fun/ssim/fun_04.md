## **ssimfile** cross-references
<a href="#--ssimfile---cross-references"></a>
This description deals with the definitions. 

Ssim manipulations are given in [Ssimfiles](/txt/recipe/ssim/README.md) and all ssimfiles are given in [Ssim Databases](/txt/ssimdb/README.md).

First **field** of each **ssimfile** record is always unique, or a **"primary key"**, see [**ssim** tuples and SQL Database](/txt/fun/ssim/fun_02.md) chapter.

Each **field** of any **ssimfile** can be defined as a **foreign key**  to the **primary key** (first **field**) of any **ssimfile**.

Importantly, any **fldfunc field** ,see [**fldfunc fields** in **ssim** tuples](/txt/fun/ssim/fun_02.md) chapter, can be defined as a **foreign key**  to the **primary key** (first **field**) of any **ssimfile**. So a **primary key** of a **ssimfile**  can be split into several **fldfunc fields**, and each **fldfunc field** can be defined with a **foreign key** constraint.

### strong referential integrity checks
<a href="#strong-referential-integrity-checks"></a>
Defined constraints allow a strong referential integrity checks for all **openacr ssimfile** records. A CLI tool [acr](/txt/exe/acr/README.md) checks each ssimfile record for the referential integrity. The checks make data entry  (adding new records to ssimfiles) a fool-proof process.

### information management via cross-reference
<a href="#information-management-via-cross-reference"></a>
**ssimfiles** strong cross-reference properties and ease of **foreign key** constraints allow for organizing **ssimfiles** information into a relational database.

### Examples of **cross-referenced** records
<a href="#examples-of---cross-referenced---records"></a>
A CLI tool [acr](/txt/exe/acr/README.md) helps the display and the manipulation of the cross-referenced ssimfile records. Below a tuple

```
dmmeta.ctype  ctype:acr.Queryop  comment:"Operation to perform"
```

is shown with all its cross-referenced relations

```
# Ctype            Comment
# dmmeta.Ctype     Struct
# dmmeta.Ns        Namespace (for in-memory database, protocol, etc)
# dmmeta.Nstype    Namespace type
# dev.License      Source code license
# dmmeta.Cfmt      Specify options for printing/reading ctypes into multiple formats
# dmmeta.Strfmt    Cfmt conversion type (misnomer, not necessarily a string)
# dmmeta.Printfmt  Print format
# bool             1-byte bool, c++ type.
# dmmeta.Cpptype   Specify whether a ctype can be passed by value, and other c++ options
# dmmeta.Ctypelen  Size of Ctype
# dmmeta.Field     Specify field of a struct
# dmmeta.Reftype   Field type constructor (e.g. reference type)
# dmmeta.Fcast     Generate implicit conversion from field to c++ expression
# dmmeta.Fconst    Specify enum value (integer + string constant) for a field

# e.g. dmmeta.ctype  ctype:""  comment:""
# e.g. dmmeta.ns  ns:""  nstype:""  license:""  comment:""
# e.g. dmmeta.nstype  nstype:""  comment:""
# e.g. dev.license  license:""  comment:""
# e.g. dmmeta.cfmt  cfmt:""  printfmt:""  read:""  print:""  sep:""  genop:""  comment:""
# e.g. dmmeta.cpptype  ctype:""  ctor:""  dtor:""  cheap_copy:""
# e.g. dmmeta.ctypelen  ctype:""  len:""  alignment:""  padbytes:""
# e.g. dmmeta.field  field:""  arg:""  reftype:""  dflt:""  comment:""
# e.g. dmmeta.fcast  field:""  expr:""  comment:""
# e.g. dmmeta.fconst  fconst:""  value:""  comment:""

dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:exe  comment:Executable
  dmmeta.ns  ns:acr  nstype:exe  license:GPL  comment:"Algo Cross-Reference - ssimfile database & update tool"
    dmmeta.ctype  ctype:acr.Queryop  comment:"Operation to perform"
      dmmeta.cpptype  ctype:acr.Queryop  ctor:Y  dtor:Y  cheap_copy:N
      dmmeta.field  field:acr.Queryop.value  acr.rowid:0       arg:u8  reftype:Val  dflt:""  comment:""
        dmmeta.fcast  field:acr.Queryop.value  expr:""  comment:""
        dmmeta.fconst  fconst:acr.Queryop.value/none                 acr.rowid:0       value:0  comment:""
        dmmeta.fconst  fconst:acr.Queryop.value/select               acr.rowid:1       value:1  comment:"Select record"
        dmmeta.fconst  fconst:acr.Queryop.value/set_attr             acr.rowid:2       value:2  comment:"Set attribute value"
        dmmeta.fconst  fconst:acr.Queryop.value/rename_attr          acr.rowid:3       value:3  comment:"Set attribute name"
        dmmeta.fconst  fconst:acr.Queryop.value/del_attr             acr.rowid:4       value:4  comment:"Delete attribute"
        dmmeta.fconst  fconst:acr.Queryop.value/finish_rename_field  acr.rowid:5       value:5  comment:"Reindex renamed field"
        dmmeta.fconst  fconst:acr.Queryop.value/rename_typetag       acr.rowid:6       value:6  comment:"Rewrite type tag in renamed ssimfile"

      dmmeta.cfmt  cfmt:acr.Queryop.String  printfmt:Raw  read:N  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:acr.Queryop  len:1  alignment:1  padbytes:0

dmmeta.field  field:acr.FQuery.queryop  acr.rowid:0       arg:acr.Queryop  reftype:Val  dflt:""  comment:"Type of operation to perform"

# Field                      Arg               Reftype  Comment
# dmmeta.Ctype.ctype         algo.Smallstr100  Val      Identifier. must be ns.typename
# dmmeta.Ctype.ns            dmmeta.Ns         Pkey     Parent namespace
# dmmeta.Ctype.name          algo.Smallstr100  Val      Name sans namespace
# dmmeta.Ns.ns               algo.Smallstr16   Val      Namespace name (primary key)
# dmmeta.Ns.nstype           dmmeta.Nstype     Pkey     Namespace type
# dmmeta.Ns.license          dev.License       Pkey     Associated license
# dmmeta.Cpptype.ctor        bool              Val      if true, generate non-default constructor from all fields
# dmmeta.Cpptype.dtor        bool              Val      generate non-default destructor
# dmmeta.Cpptype.cheap_copy  bool              Val      Pass by value whenever possible
# dmmeta.Ctypelen.ctype      dmmeta.Ctype      Pkey     Identifies the Ctype
# dmmeta.Field.field         algo.Smallstr100  Val      Primary key, as ctype.name
# dmmeta.Field.ctype         dmmeta.Ctype      Pkey     Parent structure
# dmmeta.Field.ns            dmmeta.Ns         Pkey     Parent namespace
# dmmeta.Field.name          algo.Smallstr50   Val      Name of the field
# dmmeta.Field.arg           dmmeta.Ctype      Pkey     Type of field
# dmmeta.Field.reftype       dmmeta.Reftype    Pkey     Type constructor
# dmmeta.Field.dflt          algo.CppExpr      Val      Default value (c++ expression)
```

