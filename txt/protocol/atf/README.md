## atf - Algo Testing Framework


### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### atf.Testresult - 
<a href="#atf-testresult"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### atf.Testrun - 
<a href="#atf-testrun"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|testrun|[atfdb.Unittest](/txt/ssimdb/atfdb/unittest.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Test name|
|testresult|[atf.Testresult](/txt/protocol/atf/README.md#atf-testresult)|[Val](/txt/exe/amc/reftypes.md#val)|0|Result|
|n_step|u64|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of sub-steps|
|n_cmp|u64|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of comparisons made during test|
|comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Testcase comment|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->

