## ssim2csv - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [ssim2csv - Ssim -> csv conversion tool](/txt/exe/ssim2csv/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/ssim2csv_gen.cpp](/cpp/gen/ssim2csv_gen.cpp)||
|[cpp/ssim2csv.cpp](/cpp/ssim2csv.cpp)||
|[include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)||
|[include/gen/ssim2csv_gen.inl.h](/include/gen/ssim2csv_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`ssim2csv` generated code creates the tables below.
All allocations are done through global `ssim2csv::_db` [ssim2csv.FDb](#ssim2csv-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[ssim2csv.FDb](#ssim2csv-fdb)||FDb._db (Global)|
|[ssim2csv.FExpand](#ssim2csv-fexpand)||FDb.expand (Lary)|expand (Lary, by rowid)|ind_expand (Thash, hash field expand)|
|[ssim2csv.FFlatten](#ssim2csv-fflatten)||FDb.flatten (Tary)|
|[ssim2csv.FOutfile](#ssim2csv-foutfile)||FDb.outfile (Lary)|outfile (Lary, by rowid)|ind_outfile (Thash, hash field outfile)|

#### ssim2csv.FDb - In-memory database for ssim2csv
<a href="#ssim2csv-fdb"></a>

#### ssim2csv.FDb Fields
<a href="#ssim2csv-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssim2csv.FDb._db|[ssim2csv.FDb](/txt/exe/ssim2csv/internals.md#ssim2csv-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|ssim2csv.FDb.cmdline|[command.ssim2csv](/txt/protocol/command/README.md#command-ssim2csv)|[Val](/txt/exe/amc/reftypes.md#val)|||
|ssim2csv.FDb.expand|[ssim2csv.FExpand](/txt/exe/ssim2csv/internals.md#ssim2csv-fexpand)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|ssim2csv.FDb.ind_expand|[ssim2csv.FExpand](/txt/exe/ssim2csv/internals.md#ssim2csv-fexpand)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|ssim2csv.FDb.outfile|[ssim2csv.FOutfile](/txt/exe/ssim2csv/internals.md#ssim2csv-foutfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|ssim2csv.FDb.ind_outfile|[ssim2csv.FOutfile](/txt/exe/ssim2csv/internals.md#ssim2csv-foutfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|ssim2csv.FDb.name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|ssim2csv.FDb.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|ssim2csv.FDb.flatten|[ssim2csv.FFlatten](/txt/exe/ssim2csv/internals.md#ssim2csv-fflatten)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)
```
struct FDb { // ssim2csv.FDb: In-memory database for ssim2csv
    command::ssim2csv      cmdline;                     //
    ssim2csv::FExpand*     expand_lary[32];             // level array
    i32                    expand_n;                    // number of elements in array
    ssim2csv::FExpand**    ind_expand_buckets_elems;    // pointer to bucket array
    i32                    ind_expand_buckets_n;        // number of elements in bucket array
    i32                    ind_expand_n;                // number of elements in the hash table
    ssim2csv::FOutfile*    outfile_lary[32];            // level array
    i32                    outfile_n;                   // number of elements in array
    ssim2csv::FOutfile**   ind_outfile_buckets_elems;   // pointer to bucket array
    i32                    ind_outfile_buckets_n;       // number of elements in bucket array
    i32                    ind_outfile_n;               // number of elements in the hash table
    algo::cstring*         name_elems;                  // pointer to elements
    u32                    name_n;                      // number of elements in array
    u32                    name_max;                    // max. capacity of array before realloc
    algo::cstring*         value_elems;                 // pointer to elements
    u32                    value_n;                     // number of elements in array
    u32                    value_max;                   // max. capacity of array before realloc
    ssim2csv::FFlatten*    flatten_elems;               // pointer to elements
    u32                    flatten_n;                   // number of elements in array
    u32                    flatten_max;                 // max. capacity of array before realloc
    ssim2csv::trace        trace;                       //
};
```

#### ssim2csv.FExpand - 
<a href="#ssim2csv-fexpand"></a>

#### ssim2csv.FExpand Fields
<a href="#ssim2csv-fexpand-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssim2csv.FExpand.expand|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FExpand
<a href="#struct-fexpand"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)
```
struct FExpand { // ssim2csv.FExpand
    ssim2csv::FExpand*   ind_expand_next;   // hash next
    algo::cstring        expand;            //
    // func:ssim2csv.FExpand..AssignOp
    inline ssim2csv::FExpand& operator =(const ssim2csv::FExpand &rhs) = delete;
    // func:ssim2csv.FExpand..CopyCtor
    inline               FExpand(const ssim2csv::FExpand &rhs) = delete;
private:
    // func:ssim2csv.FExpand..Ctor
    inline               FExpand() __attribute__((nothrow));
    // func:ssim2csv.FExpand..Dtor
    inline               ~FExpand() __attribute__((nothrow));
    friend ssim2csv::FExpand&   expand_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend ssim2csv::FExpand*   expand_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 expand_RemoveAll() __attribute__((nothrow));
    friend void                 expand_RemoveLast() __attribute__((nothrow));
};
```

#### ssim2csv.FFlatten - 
<a href="#ssim2csv-fflatten"></a>

#### ssim2csv.FFlatten Fields
<a href="#ssim2csv-fflatten-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssim2csv.FFlatten.in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|ssim2csv.FFlatten.prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FFlatten
<a href="#struct-fflatten"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)
```
struct FFlatten { // ssim2csv.FFlatten
    algo::cstring   in;       //
    algo::cstring   prefix;   //
    // func:ssim2csv.FFlatten..Ctor
    inline               FFlatten() __attribute__((nothrow));
};
```

#### ssim2csv.FOutfile - 
<a href="#ssim2csv-foutfile"></a>

#### ssim2csv.FOutfile Fields
<a href="#ssim2csv-foutfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssim2csv.FOutfile.fildes|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)|||
|ssim2csv.FOutfile.header|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|ssim2csv.FOutfile.outfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FOutfile
<a href="#struct-foutfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)
```
struct FOutfile { // ssim2csv.FOutfile
    ssim2csv::FOutfile*   ind_outfile_next;   // hash next
    algo_lib::FFildes     fildes;             //
    algo::cstring         header;             //
    algo::cstring         outfile;            //
    // func:ssim2csv.FOutfile..AssignOp
    inline ssim2csv::FOutfile& operator =(const ssim2csv::FOutfile &rhs) = delete;
    // func:ssim2csv.FOutfile..CopyCtor
    inline               FOutfile(const ssim2csv::FOutfile &rhs) = delete;
private:
    // func:ssim2csv.FOutfile..Ctor
    inline               FOutfile() __attribute__((nothrow));
    // func:ssim2csv.FOutfile..Dtor
    inline               ~FOutfile() __attribute__((nothrow));
    friend ssim2csv::FOutfile&  outfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend ssim2csv::FOutfile*  outfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 outfile_RemoveAll() __attribute__((nothrow));
    friend void                 outfile_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->
