## atf - Algo Testing Framework


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### atf.Testresult - 
<a href="#atf-testresult"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|Val|

#### atf.Testrun - 
<a href="#atf-testrun"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|testrun|[atfdb.Unittest](/txt/ssimdb/atfdb/unittest.md)|Pkey||Test name|
|testresult|atf.Testresult|Val|0|Result|
|n_step|u64|Val|0|Number of sub-steps|
|n_cmp|u64|Val|0|Number of comparisons made during test|
|comment|algo.cstring|Val||Testcase comment|

