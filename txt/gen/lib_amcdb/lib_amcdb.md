## lib_amcdb - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
<!-- abt_md.toc_end -->

### Description
<a href="#description"></a>
for usage, see [lib_amcdb - Library used by amc](/txt/lib/lib_amcdb/README.md)

### Inputs
<a href="#inputs"></a>
`lib_amcdb` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/amcdb_gen.cpp](/cpp/gen/amcdb_gen.cpp)||
|[cpp/gen/lib_amcdb_gen.cpp](/cpp/gen/lib_amcdb_gen.cpp)||
|[cpp/lib_amcdb/lib_amcdb.cpp](/cpp/lib_amcdb/lib_amcdb.cpp)||
|[include/gen/amcdb_gen.h](/include/gen/amcdb_gen.h)||
|[include/gen/amcdb_gen.inl.h](/include/gen/amcdb_gen.inl.h)||
|[include/gen/lib_amcdb_gen.h](/include/gen/lib_amcdb_gen.h)||
|[include/gen/lib_amcdb_gen.inl.h](/include/gen/lib_amcdb_gen.inl.h)||
|[include/lib_amcdb.h](/include/lib_amcdb.h)||

### Dependencies
<a href="#dependencies"></a>
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
