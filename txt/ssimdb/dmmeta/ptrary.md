## dmmeta.ptrary - Required for fields with reftype:Ptrary
<a href="#dmmeta-ptrary"></a>

This record is required for any field with reftype:Ptrary.
Example:
```
acr ptrary:atf_amc.% | head -1
dmmeta.ptrary  field:atf_amc.FCascdel.c_child_ptrary  unique:Y
```

The attribute `unique` controls whether only a pointer to one record
can be added to the array more than once. Without `unique:Y`, you could add a pointer
multiple times. Using `ptrary` with `xref` is only allowed if `unique:Y`.
A flag is added to the child record to record whether it's inserted in the array.
Removing a record from `ptrary` involves linear scanning back-to-front to find the element
and shifting the remaining elements over by one. Removing from a non-unique
ptrary only removes the first instance and is generally not recommended.

The following functions are generated:
```
inline-command: amc -report:N -proto atf_amc.FCascdel.c_child_ptrary.%
// Delete all elements pointed to by the index.
// func:atf_amc.FCascdel.c_child_ptrary.Cascdel
void                 c_child_ptrary_Cascdel(atf_amc::FCascdel& cascdel) __attribute__((nothrow));
// Return true if index is empty
// func:atf_amc.FCascdel.c_child_ptrary.EmptyQ
bool                 c_child_ptrary_EmptyQ(atf_amc::FCascdel& cascdel) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:atf_amc.FCascdel.c_child_ptrary.Find
atf_amc::FCascdel*   c_child_ptrary_Find(atf_amc::FCascdel& cascdel, u32 t) __attribute__((__warn_unused_result__, nothrow));
// Return array of pointers
// func:atf_amc.FCascdel.c_child_ptrary.Getary
algo::aryptr<atf_amc::FCascdel*> c_child_ptrary_Getary(atf_amc::FCascdel& cascdel) __attribute__((nothrow));
// Insert pointer to row into array. Row must not already be in array.
// If pointer is already in the array, it may be inserted twice.
// func:atf_amc.FCascdel.c_child_ptrary.Insert
void                 c_child_ptrary_Insert(atf_amc::FCascdel& cascdel, atf_amc::FCascdel& row) __attribute__((nothrow));
// Insert pointer to row in array.
// If row is already in the array, do nothing.
// Return value: whether element was inserted into array.
// func:atf_amc.FCascdel.c_child_ptrary.InsertMaybe
bool                 c_child_ptrary_InsertMaybe(atf_amc::FCascdel& cascdel, atf_amc::FCascdel& row) __attribute__((nothrow));
// Return number of items in the pointer array
// func:atf_amc.FCascdel.c_child_ptrary.N
i32                  c_child_ptrary_N(const atf_amc::FCascdel& cascdel) __attribute__((__warn_unused_result__, nothrow, pure));
// Find element using linear scan. If element is in array, remove, otherwise do nothing
// func:atf_amc.FCascdel.c_child_ptrary.Remove
void                 c_child_ptrary_Remove(atf_amc::FCascdel& cascdel, atf_amc::FCascdel& row) __attribute__((nothrow));
// Empty the index. (The rows are not deleted)
// func:atf_amc.FCascdel.c_child_ptrary.RemoveAll
void                 c_child_ptrary_RemoveAll(atf_amc::FCascdel& cascdel) __attribute__((nothrow));
// Reserve space in index for N more elements;
// func:atf_amc.FCascdel.c_child_ptrary.Reserve
void                 c_child_ptrary_Reserve(atf_amc::FCascdel& cascdel, u32 n) __attribute__((nothrow));
// Return reference without bounds checking
// func:atf_amc.FCascdel.c_child_ptrary.qFind
atf_amc::FCascdel&   c_child_ptrary_qFind(atf_amc::FCascdel& cascdel, u32 idx) __attribute__((nothrow));
// True if row is in any ptrary instance
// func:atf_amc.FCascdel.c_child_ptrary.InAryQ
bool                 cascdel_c_child_ptrary_InAryQ(atf_amc::FCascdel& row) __attribute__((nothrow));
// Reference to last element without bounds checking
// func:atf_amc.FCascdel.c_child_ptrary.qLast
atf_amc::FCascdel&   c_child_ptrary_qLast(atf_amc::FCascdel& cascdel) __attribute__((nothrow));

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ptrary

* file:[data/dmmeta/ptrary.ssim](/data/dmmeta/ptrary.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|unique|bool|Val||Search for and ignore duplicates|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FPtrary

