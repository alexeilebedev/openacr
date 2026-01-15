## lib_amcdb - Library used by amc


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_amcdb` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

