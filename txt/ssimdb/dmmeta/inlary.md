## dmmeta.inlary - Generate inline array of fixed or variable length (all entries fit within parent struct)
<a href="#dmmeta-inlary"></a>

The `Inlary` generates code to turn a single value field
into an inline array of up to N values. An inline array is located directly in the
record which houses it. There is no dynamic allocation. There are two types of `Inlary`:
ones where `min = max`, in this case a C++ array is generated.
And the case where `min < max`. In this case, `amc` declares an array of `u8` of appropriate size,
and generates generates usual pool functions `Alloc`, `RemoveLast`, `RemoveAll`.
Here is an example:

```
inline-command: acr -t field:algo_lib.FDb.temp_strings | egrep algo_lib.FDb
    dmmeta.ctype  ctype:algo_lib.FDb  comment:"In-memory database for algo_lib"
      dmmeta.field  field:algo_lib.FDb.temp_strings  arg:algo.cstring  reftype:Inlary  dflt:""  comment:"* initialization order is important *"
        dmmeta.inlary  field:algo_lib.FDb.temp_strings  min:8  max:8  comment:""
```

The following functions are generated for a fixed array:

```
inline-command: amc -report:N -proto algo_lib.FDb.temp_strings.%
// Set all elements of fixed array to value RHS
// func:algo_lib.FDb.temp_strings.Fill
void                 temp_strings_Fill(const algo::cstring &rhs) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:algo_lib.FDb.temp_strings.Find
algo::cstring*       temp_strings_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));
// Access fixed array temp_strings as aryptr.
// func:algo_lib.FDb.temp_strings.Getary
algo::aryptr<algo::cstring> temp_strings_Getary() __attribute__((nothrow));
// Return max number of items in the array
// func:algo_lib.FDb.temp_strings.Max
i32                  temp_strings_Max() __attribute__((nothrow));
// Return number of items in the array
// func:algo_lib.FDb.temp_strings.N
i32                  temp_strings_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Set contents of fixed array to RHS; Input length is trimmed as necessary
// func:algo_lib.FDb.temp_strings.Setary
void                 temp_strings_Setary(const algo::aryptr<algo::cstring> &rhs) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking in release.
// func:algo_lib.FDb.temp_strings.qFind
algo::cstring&       temp_strings_qFind(u64 t) __attribute__((nothrow));
// Read array from string
// Convert string to field. Return success value
// func:algo_lib.FDb.temp_strings.ReadStrptrMaybe
bool                 temp_strings_ReadStrptrMaybe(algo::strptr in_str) __attribute__((nothrow));

```

The following functions are generated for a non-fixed array:

```
inline-command: amc -report:N -proto atf_amc.FTypeA.typec.%
// Allocate memory for new default row.
// If out of memory, process is killed.
// func:atf_amc.FTypeA.typec.Alloc
atf_amc::FTypeC&     typec_Alloc(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
// func:atf_amc.FTypeA.typec.AllocMaybe
atf_amc::FTypeC*     typec_AllocMaybe(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Create new row from struct.
// Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)
// func:atf_amc.FTypeA.typec.InsertMaybe
atf_amc::FTypeC*     typec_InsertMaybe(atf_amc::FTypeA& typea, const atf_amc::TypeC &value) __attribute__((nothrow));
// Allocate space for one element. If no memory available, return NULL.
// func:atf_amc.FTypeA.typec.AllocMem
void*                typec_AllocMem(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:atf_amc.FTypeA.typec.EmptyQ
bool                 typec_EmptyQ(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:atf_amc.FTypeA.typec.Find
atf_amc::FTypeC*     typec_Find(atf_amc::FTypeA& typea, u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
// func:atf_amc.FTypeA.typec.Getary
algo::aryptr<atf_amc::FTypeC> typec_Getary(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Return constant 10 -- max. number of items in the pool
// func:atf_amc.FTypeA.typec.Max
i32                  typec_Max(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Return number of items in the array
// func:atf_amc.FTypeA.typec.N
i32                  typec_N(const atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow, pure));
// Destroy all elements of Inlary
// func:atf_amc.FTypeA.typec.RemoveAll
void                 typec_RemoveAll(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:atf_amc.FTypeA.typec.RemoveLast
void                 typec_RemoveLast(atf_amc::FTypeA& typea) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking in release.
// func:atf_amc.FTypeA.typec.qFind
atf_amc::FTypeC&     typec_qFind(atf_amc::FTypeA& typea, u64 t) __attribute__((nothrow));
// Compute row id of element given element's address
// func:atf_amc.FTypeA.typec.rowid_Get
u64                  typec_rowid_Get(atf_amc::FTypeA& typea, atf_amc::FTypeC &row) __attribute__((nothrow));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:atf_amc.FTypeA.typec.XrefMaybe
bool                 typec_XrefMaybe(atf_amc::FTypeC &row);

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Inlary

* file:[data/dmmeta/inlary.ssim](/data/dmmeta/inlary.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|min|i32|Val||Min. elements to preallocate|
|max|i32|Val||Max. elements|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FInlary

