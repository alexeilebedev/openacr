## Ssim Fundamentals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [**ssim** tuples and **ssimfiles**](#--ssim---tuples-and---ssimfiles--)
* [SQL - **ssim** tuples and SQL Database](#sql)
* [fldfunc - **fldfunc fields** in **ssim** tuples](#fldfunc)
* [**ssimfile** cross-references](#--ssimfile---cross-references)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### **ssim** tuples and **ssimfiles**
<a href="#--ssim---tuples-and---ssimfiles--"></a>
All information in OpenACR is expressed in **"ssim"** tuples format.
"ssim" is short for "super-simple".

This description deals with the definitions. 

Some common recipes are available in [Ssimfiles](/txt/recipe/README.md) and all ssimfiles are described in [Ssim Databases](/txt/ssimdb/README.md).

#### **ssim** tuples
<a href="#--ssim---tuples"></a>
The **ssim** tuple format is:

```
<namespace>.<table>   <primary_key>  [<key>:<value>  ...  <key>:<value>]
```

The format consists of several tokens separated by spaces. First two tokens are mandatory, the rest is optional.

First token, called **"tuple head"**, has two parts, **namespace**.**table**, separated by dot "."

Subsequent tokens are called **fields**.

Each **field** has two parts, **key:value"**, separated by colon ":".

First **field's** **value** is always unique across all tuples with a given **"tuple head"**.

The optional rest of the **fields** also have two parts, **key:value**, separated by colon ":"

First **field** is a **primary key**, and each **field** is a DB table **column** in SQL Database sense. See [**ssim** tuples and SQL Database](#sql) chapter.

#### Keeping tuples as **ssimfile** *.ssim files
<a href="#keeping-tuples-as---ssimfile-----ssim-files"></a>
All tuples are kept in text files with ***.ssim** extension in "data" directory. The file name is fully defined by the **"tuple head"**.
It is formed as **data/namespace/table.ssim**. Here **namespace** and **table** are coming from **"tuple head"** in the tuple. Each line in the **.ssim** file is a tuple.

#### Examples of **ssim** tuples
<a href="#examples-of---ssim---tuples"></a>
```
dmmeta.ns  ns:lib_amcdb    nstype:lib       license:GPL   comment:"Library used by amc"

dev.target  target:lib_amcdb

dmmeta.nstype  nstype:lib  comment:Library
```

#### Examples of **ssimfiles** files
<a href="#examples-of---ssimfiles---files"></a>
Tuples with **"tuple head"**  **dmmeta.ns** are kept in **data/dmmeta/ns.ssim** file

Tuples with **"tuple head"** **dev.target** are kept in **data/dev/target.ssim** file

Tuples with **"tuple head"** **dmmeta.nstype** are kept in **data/dmmeta/nstype.ssim** file

There are more than 200 ssimfiles that come with openacr distribution - they can be found in "data" directory.

### SQL - **ssim** tuples and SQL Database
<a href="#sql"></a>
There is, by design, a full equivalence between ssim tuples and SQL DB columns and records. Each tuple represents one record in a table in a database. Each **"tuple head"**, presented as ```<namespace>.<table>``` defines database (via it's namespace) and table (via it's table) names.

#### **ssimfile** as SQL table and **field** as a table column
<a href="#--ssimfile---as-sql-table-and---field---as-a-table-column"></a>
Each tuple **field** defines table column name (primary_key or key) and a record column value (value).
Each **ssimfile** can be understood as an SQL table, with this table first column is a **primary key**.

#### Guarantees of ssimfile and SQL Database equivalence
<a href="#guarantees-of-ssimfile-and-sql-database-equivalence"></a>
OpenACR verification process loads all **ssimfiles** to a temporary MariaDB instance and writes them back, guaranteeing full equivalence.

#### Examples of SQL Database equivalence
<a href="#examples-of-sql-database-equivalence"></a>
A CLI tool [acr_my](/txt/exe/acr_my/README.md) allows **ssimfiles** to be loaded into MySQL, manipulated via SQL means and written back. The examples below show the ssim tuples equivalence to SQL records.

ssim tuple

```
...
dmmeta.ns  ns:lib_amcdb    nstype:lib       license:GPL   comment:"Library used by amc"
...
```

SQL record

```

MariaDB [dmmeta]> select * from ns where ns like "lib_amcdb%";
+-----------+--------+---------+---------------------+
| ns        | nstype | license | comment             |
+-----------+--------+---------+---------------------+
| lib_amcdb | lib    | GPL     | Library used by amc |
+-----------+--------+---------+---------------------+
1 row in set (0.001 sec)

MariaDB [dmmeta]>

```

ssim tuple

```
dmmeta.nstype  nstype:exe       comment:Executable
dmmeta.nstype  nstype:lib       comment:Library
dmmeta.nstype  nstype:none      comment:"Skip building"
dmmeta.nstype  nstype:objlist   comment:"Object files are added to dependents' link line"
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
dmmeta.nstype  nstype:ssimdb    comment:"Ssim database (not a target)"
```

SQL record

```
MariaDB [dmmeta]> select * from nstype;
+----------+-------------------------------------------------+
| nstype   | comment                                         |
+----------+-------------------------------------------------+
| exe      | Executable                                      |
| lib      | Library                                         |
| none     | Skip building                                   |
| objlist  | Object files are added to dependents' link line |
| protocol | Protocol (not a target)                         |
| ssimdb   | Ssim database (not a target)                    |
+----------+-------------------------------------------------+
```

ssim tuple

```
dev.target  target:lib_amcdb
```

SQL record

```
MariaDB [dev]> select * from target where target like "lib_amcdb";
+-----------+----------------------------+
| target    | extra_column_for_roundtrip |
+-----------+----------------------------+
| lib_amcdb |                            |
+-----------+----------------------------+
```

### fldfunc - **fldfunc fields** in **ssim** tuples
<a href="#fldfunc"></a>
This description deals with the definitions. 

#### **fldfunc-fields** in **ssim tuples**
<a href="#--fldfunc-fields---in---ssim-tuples--"></a>
Each **field** in a **ssimfile** tuple is a **key:value** pair. The **ssimfile** definitions, described in [How to define a new ssimfile](/txt/recipe/README.md) allow a **field** to be partitioned into additional **fldfunc** **fields** of **key:value** pair format. These additional **fields** provide **keys** (names) for the substrings of the **value** of the original **key:value** pair.

The **value** string is subdivided into substrings, with the explicit separator character, defined and visible in the string. The substrings provide new **values**, with the new **keys** coming from **ssimfile** definitions. The definitions creation is described here:[How to define a new ssimfile](/txt/recipe/README.md).

#### Substring extraction mechanism
<a href="#substring-extraction-mechanism"></a>
The substring extraction mechanism is defined by the expression

```
<separator character><direction_1direction_2>[<separator character><direction_1direction_2>...]
```

direction1 and direction2 are either L(eft) or R(ight), so an example of the expression would be

```
new_field expr:.LL/RL
```

which opens up as this algorithm, always executed left to right of the expr: "going from the L(eft), take the L(eft) reminder of the string after the first dot "." is found. Next, with the resulting string, go from the R(igt) till first slash "/" is found, and take the L(eft) reminder.

So if ``field:c1/c2c3.c4c5`` is the expression, first part of the algorithm, ``.LL``, will result in ``c1/c2c3`` substring, and second part, ``/RL``, will deliver ``c1``. So the final **fldunc**``new_field`` will be ``new_field:c1``

#### Examples of **fldfunc fields**
<a href="#examples-of---fldfunc-fields--"></a>
A CLI tool [acr](/txt/exe/acr/README.md) shows ssimfiles with and without **fldfunc fields**.

First example would be this tuple, defined as [dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md).

A tuple kept in the **ssimfile** would be this:

```
...
dmmeta.ctype  ctype:dev.Cfg  comment:"Compiler configuration"
...
```

an expanded tuple with **fldfunc** fields would be this

```
...
dmmeta.ctype  ctype:dev.Cfg  ns:dev  name:Cfg  comment:"Compiler configuration"
...
```

here the value of``ctype:dev.Cfg`` is expanded into two additional **fldfunc fields** ``ns:dev and name:Cfg``

the **value** separator in this case is dot "." and the``ns`` and ``name`` **keys** came from **ssimfile** description, not present in the tuple.

The description that defines the substrings is this:

```
name  expr:.RR
ns    expr:.RL
```

A more involved example would be this tuple, defined as [dev.tool_opt - Compiler/linker options to use](/txt/ssimdb/dev/tool_opt.md).

```
dev.tool_opt  tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"  comment:""
```

an expanded tuple with **fldfunc** fields would be this

```
dev.tool_opt  tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"  uname:Linux  compiler:g++  cfg:%  arch:%  target:mysql2ssim  opt_type:LINK  opt:"-L /usr/lib64/mysql"  sortfld:Linux-g++  comment:""
```

here the value of
``tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"`` is expanded into 8 additional fields **fldfunc fields**:``arch, cfg, compiler, opt, opt_type, sortfld, target, uname``

The description that defines the substrings is this:

```
arch      expr:/LL.LR-LR
cfg       expr:/LL.LR-LL
compiler  expr:/LL.LL-LR
opt       expr:/LR:LR   
opt_type  expr:/LR:LL-LR
sortfld   expr:.LL      
target    expr:/LR:LL-LL
uname     expr:/LL.LL-LL
```

We can see that "/", ".", ":", and "-" all used as the separators in this case, and the expression mechanisms are quite involved.

### **ssimfile** cross-references
<a href="#--ssimfile---cross-references"></a>
This description deals with the definitions. 

First **field** of each **ssimfile** record is always unique, or a **"primary key"**, see [**ssim** tuples and SQL Database](#sql) chapter.

Each **field** of any **ssimfile** can be defined as a **foreign key**  to the **primary key** (first **field**) of any **ssimfile**.

Importantly, any **fldfunc field** ,see [**fldfunc fields** in **ssim** tuples](#fldfunc) chapter, can be defined as a **foreign key**  to the **primary key** (first **field**) of any **ssimfile**. So a **primary key** of a **ssimfile**  can be split into several **fldfunc fields**, and each **fldfunc field** can be defined with a **foreign key** constraint.

#### strong referential integrity checks
<a href="#strong-referential-integrity-checks"></a>
Defined constraints allow a strong referential integrity checks for all **openacr ssimfile** records. A CLI tool [acr](/txt/exe/acr/README.md) checks each ssimfile record for the referential integrity. The checks make data entry  (adding new records to ssimfiles) a fool-proof process.

#### information management via cross-reference
<a href="#information-management-via-cross-reference"></a>
**ssimfiles** strong cross-reference properties and ease of **foreign key** constraints allow for organizing **ssimfiles** information into a relational database.

#### Examples of **cross-referenced** records
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

