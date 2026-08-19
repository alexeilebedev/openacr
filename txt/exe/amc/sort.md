## amc Feature: Sorting — fsort and fcmp
<a href="#amc-feature-sorting-fsort-and-fcmp"></a>
A table of records is read in insertion order, but reports usually have to
come out in some other order.  `acr` writes an ssimfile by collecting the
records it is about to emit into an array and putting that array in sort-key
order first.  Written by hand, that is a comparison function, a sort
routine, and the same pair again for the next table that needs an order.

The order is a property of the data, not of the algorithm, so it belongs in
the schema.  Two records put it there: `dmmeta.fsort` names an array field
and the field to order it by, and `dmmeta.fcmp` overrides how one field
compares.  amc generates the comparison and the sort from those.

```
dmmeta.field      field:algo_lib.FTxttbl.c_txtrow  arg:algo_lib.FTxtrow  reftype:Ptrary  dflt:""  comment:"Array of rows"
  dmmeta.fsort    field:algo_lib.FTxttbl.c_txtrow  sorttype:QuickSort  sortfld:algo_lib.FTxtrow.sortkey  comment:""
dmmeta.field      field:algo_lib.FTxtrow.sortkey   arg:algo.cstring  reftype:Val  dflt:""  comment:"Sort key"
  dmmeta.fcmp     field:algo_lib.FTxtrow.sortkey   versionsort:Y  casesens:N  extrn:N  comment:""
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [fsort](#fsort)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [fcmp](#fcmp)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [See also](#see-also)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>

### fsort
<a href="#fsort"></a>

`dmmeta.fsort` has three fields.  `field` is the array being sorted.
`sortfld` is a field of the array's element ctype, and its value is the
sort key.  `sorttype` picks the algorithm — `QuickSort`, `HeapSort` or
`InsertionSort`, the three rows of `dmmeta.sorttype`.

The array field must be one whose elements are contiguous in memory:
`Tary`, `Ptrary`, `Varlen` or `Inlary`.  A `Ptrary` sorts the pointers and
leaves the records where they are; the other three move the records
themselves.  amc rejects anything else, because a sort has to be able to
swap two elements by index.

Two further restrictions come from the same place.  `sortfld` must be a
`Val` — a pool or an index has no value to compare.  And a by-value sort
of an element ctype that has cross-references is refused unless the ctype
has an instantiating pool: moving such a record would leave every index
that points at it dangling.  Sorting a `Ptrary` is always allowed, since
the records do not move.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/sort.cpp`.  Four functions are public; the rest are
static in the generated `.cpp` and exist to implement them.

|Tfunc|Generated function|Emitted for|Effect|
|---|---|---|---|
|`Sort.SortedQ`|`bool <name>_SortedQ(P&)`|every sorttype|Walk the array and return whether it is already in order.|
|`Sort.InsertionSort`|`void <name>_InsertionSort(P&)`|`InsertionSort`, `QuickSort`|Insertion sort over the whole array.|
|`Sort.HeapSort`|`void <name>_HeapSort(P&)`|`HeapSort`, `QuickSort`|Heap sort over the whole array.|
|`Sort.QuickSort`|`void <name>_QuickSort(P&)`|`QuickSort`|Compute a recursion-depth bound from the element count and call the engine.|
|`Sort.Lt`|`bool <name>_Lt(T&, T&)` *(private)*|every sorttype|Compare two elements by `sortfld`.  Must be anti-symmetric.|
|`Sort.Swap`|`void <name>_Swap(T&, T&)` *(private)*|every sorttype|Exchange two elements.|
|`Sort.Rotleft`|`void <name>_Rotleft(T&, T&, T&)` *(private)*|every sorttype|Rotate three elements left, for median-of-three pivot selection.|
|`Sort.IntInsertionSort`|`void <name>_IntInsertionSort(T*, int n)` *(private)*|`InsertionSort`, `QuickSort`|Insertion sort of a raw range.|
|`Sort.IntHeapSort`|`void <name>_IntHeapSort(T*, int n)` *(private)*|`HeapSort`, `QuickSort`|Heap sort of a raw range.|
|`Sort.IntQuickSort`|`void <name>_IntQuickSort(T*, int n, int depth)` *(private)*|`QuickSort`|The quicksort engine.|

`QuickSort` is an introsort, which is why choosing it emits all three
algorithms.  The engine partitions around a median-of-three pivot while
the range is longer than 16 elements, finishes shorter ranges with
insertion sort, and switches to heap sort when `depth` reaches zero.  The
public entry point sets `depth` to `CeilingLog2(n+1) + 3`, so a degenerate
pivot sequence costs the O(n log n) of heap sort rather than the O(n²) of
quicksort.

`Sort.Lt` reads the sort key directly (`elem_a.<sortfld> <
elem_b.<sortfld>`) unless `sortfld` carries an `dmmeta.fcmp` row, in which
case it calls that field's own `Lt` instead.  This is the one ordering
dependency between the two records described on this page.

### fcmp
<a href="#fcmp"></a>

`dmmeta.fcmp` changes how one field compares.  Without it, a field
compares with C++ `<` on its value, and a string field compares
bytewise.  The record has three switches:

|Field|Effect|
|---|---|
|`versionsort`|A run of digits in the string compares as a number, so `a9` sorts before `a10`.|
|`casesens`|`N` folds upper case to lower before comparing.|
|`extrn`|amc emits the prototype only; the body is hand-written.|

The comparison is a property of the field within its ctype, not of the
field's type.  The same `algo.Smallstr100` can be case-sensitive in one
ctype and case-insensitive in another, which is why the record hangs off
the field rather than off the type.

|Tfunc|Generated function|Emitted when|Effect|
|---|---|---|---|
|`Cmp.Cmp`|`i32 <name>_Cmp(P&, P& rhs)`|always|Three-way comparison of two records by this field.|
|`Cmp.Lt`|`bool <name>_Lt(P&, P& rhs)`|always|Less-than of two records by this field.|
|`Cmp.Nextchar`|`u64 <name>_Nextchar(const P&, strptr&, int&)` *(private)*|the field's type is a string and `extrn:N`|Pull the next comparison unit — one folded character, or a whole integer under `versionsort`.|

`versionsort` and `casesens` require the target type to have exactly one
data field, since `Nextchar` walks a single string.

Note that `Cmp.Lt` and `Sort.Lt` are different functions with the same
name.  `Cmp.Lt` is named after the compared field and takes two *records*
(`sortkey_Lt(txtrow, rhs)`); `Sort.Lt` is named after the array and takes
two *array elements* (`c_txtrow_Lt(elem_a, elem_b)`).

### Pitfalls
<a href="#pitfalls"></a>

- **The sort is not maintained.**  `fsort` generates a function you call;
  it does not keep the array ordered as elements are inserted.  For an
  index that stays sorted, use [Atree](/txt/exe/amc/reftype/Atree.md) or
  [Bheap](/txt/exe/amc/reftype/Bheap.md).
- **`Sort.Lt` must be anti-symmetric.**  The generated engines assume that
  if `a < b` then not `b < a`; the partition loop has no bounds check and
  relies on it.  A hand-written `extrn:Y` comparison is where this
  assumption gets broken.
- **Sorting a `Ptrary` does not move records, sorting the others does.**
  Any pointer into a `Tary`, `Varlen` or `Inlary` is invalid after a sort.
- **`Sort` runs last.**  Its generator is one of the two the field walk
  runs after `Field2` — see
  [per-field dispatch](/txt/exe/amc/backend/tclass-tfunc.md#per-field-dispatch).

### See also
<a href="#see-also"></a>

- [Atree](/txt/exe/amc/reftype/Atree.md) — an index that is always in order
- [Bheap](/txt/exe/amc/reftype/Bheap.md) — priority queue, when only the extremum is wanted
- [Ctype](/txt/exe/amc/reftype/Ctype.md) — `dmmeta.ccmp`, the comparison interface of a whole ctype
- [tclass / tfunc dispatch](/txt/exe/amc/backend/tclass-tfunc.md) — where `Sort` and `Cmp` sit in the walk
- Source: `cpp/amc/sort.cpp`, `cpp/amc/cmp.cpp`
- Spec tables: `acr 'dmmeta.fsort:%'  acr 'dmmeta.fcmp:%'  acr 'dmmeta.sorttype:%'`
- Tfunc records: `acr 'tfunc:Sort.%'  acr 'tfunc:Cmp.%'`

### Example
<a href="#example"></a>

`algo_lib.FTxttbl` holds the rows of a text table as a `Ptrary`, ordered
by a string sort key that compares case-insensitively and treats embedded
digits as numbers:

```
dmmeta.field      field:algo_lib.FTxttbl.c_txtrow  arg:algo_lib.FTxtrow  reftype:Ptrary
  dmmeta.fsort    field:algo_lib.FTxttbl.c_txtrow  sorttype:QuickSort  sortfld:algo_lib.FTxtrow.sortkey
dmmeta.field      field:algo_lib.FTxtrow.sortkey   arg:algo.cstring  reftype:Val
  dmmeta.fcmp     field:algo_lib.FTxtrow.sortkey   versionsort:Y  casesens:N  extrn:N
```

```c++
algo_lib::FTxttbl &tbl = ...;
c_txtrow_QuickSort(tbl);                  // order the rows
algo_assert(c_txtrow_SortedQ(tbl));
ind_beg(algo_lib::txttbl_c_txtrow_curs, row, tbl) {
    prlog(row.sortkey);                   // row2, row9, row10
} ind_end;
```

Comparing two rows directly, without sorting anything:

```c++
if (sortkey_Lt(*row_a, *row_b)) { ... }   // Cmp.Lt, by sortkey
i32 rel = sortkey_Cmp(*row_a, *row_b);    // Cmp.Cmp, three-way
```
