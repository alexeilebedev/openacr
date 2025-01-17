## samp_regx - Internals


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
for usage, see [samp_regx - Test tool for regular expressions](/txt/exe/samp_regx/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/samp_regx_gen.cpp](/cpp/gen/samp_regx_gen.cpp)||
|[cpp/samp_regx/samp_regx.cpp](/cpp/samp_regx/samp_regx.cpp)||
|[include/gen/samp_regx_gen.h](/include/gen/samp_regx_gen.h)||
|[include/gen/samp_regx_gen.inl.h](/include/gen/samp_regx_gen.inl.h)||
|[include/samp_regx.h](/include/samp_regx.h)||

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
`samp_regx` generated code creates the tables below.
All allocations are done through global `samp_regx::_db` [samp_regx.FDb](#samp_regx-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[samp_regx.FDb](#samp_regx-fdb)||FDb._db (Global)|

#### samp_regx.FDb - In-memory database for samp_regx
<a href="#samp_regx-fdb"></a>

#### samp_regx.FDb Fields
<a href="#samp_regx-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_regx.FDb._db|[samp_regx.FDb](/txt/exe/samp_regx/internals.md#samp_regx-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|samp_regx.FDb.cmdline|[command.samp_regx](/txt/protocol/command/README.md#command-samp_regx)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_regx_gen.h](/include/gen/samp_regx_gen.h)
```
struct FDb { // samp_regx.FDb: In-memory database for samp_regx
    command::samp_regx   cmdline;   //
    samp_regx::trace     trace;     //
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

