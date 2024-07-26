## lib_iconv - Iconv wrapper


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_iconv.FDb - In-memory database for lib_iconv
<a href="#lib_iconv-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|_db|lib_iconv.FDb|Global|
|icd|lib_iconv.Icd|Cppstack||Provides default name for variables of this type|

#### lib_iconv.Icd - 
<a href="#lib_iconv-icd"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|icd|iconv_t|Val|((iconv_t)-1)|iconv descriptor|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
void lib_iconv::Close(lib_iconv::Icd &icd) 
```

```c++
// open conversion descriptor
void lib_iconv::Open(lib_iconv::Icd& icd, strptr tocode, strptr fromcode) 
```

```c++
// convert multibyte sequence
tempstr lib_iconv::Convert(lib_iconv::Icd &icd, strptr in, bool no_throw) 
```

```c++
void lib_iconv::icd_Cleanup(lib_iconv::Icd &icd) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_iconv_gen.cpp](/cpp/gen/lib_iconv_gen.cpp)||
|[cpp/lib/lib_iconv.cpp](/cpp/lib/lib_iconv.cpp)||
|[include/gen/lib_iconv_gen.h](/include/gen/lib_iconv_gen.h)||
|[include/gen/lib_iconv_gen.inl.h](/include/gen/lib_iconv_gen.inl.h)||
|[include/lib_iconv.h](/include/lib_iconv.h)||

### Inputs
<a href="#inputs"></a>
`lib_iconv` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

