## amc Memory Pool
<a href="#amc-memory-pool"></a>

For each `ctype`, instances of which can be dynamically allocated
(i.e. not on the stack), amc generates two functions: `Alloc` and `Delete`.

This section will explain all of the available pool types, how to chain them
so that one pool allocates its own memory from another, and provide examples.

In `amc`, memory pools are fields with specific reftypes. They are given names and
they can be referred to.

Amc provides the following base set of memory pools, listed here in alphabetical order.

* Blkpool - A mostly-fifo allocator based on refcounted blocks.
* Delptr - Indirect Val; A single value, always freed in destructor.
* Inlary - A piece of memory for min..max elements inside the parent struct.
 When `min == max`, there is no `Alloc` function, and it behaves like N `Val`s.
 When `min < max`, new elements can be allocated. Only the last element can be freed.
* Lary - Effectively an Inlary of 36 Tarys, each of size 2^k. Has permanent pointers.
* Lpool - 36 Tpools, each for elements of size up to 2^k.
* Malloc - Simply calls malloc() / free().
* Sbrk - Calls sbrk() to get memory.
* Tary - Pointer to a resizable array (typically growing by powers of 2). Similar to std::vector.
* Tpool - A singly linked list of free elements; Gets new memory from base pool, frees
  elements into the list.
* Val - A single value, automatically allocated in constructor, and freed in destructor.

A pool is declared like a field.

    dmmeta.field  field:acr.FDb.file  arg:acr.FFile  reftype:Lary  dflt:""  comment:"List of all loaded files"

This provides a hook for `amc` to generate the functions initializing
and maintaining the state of the pool.

Amc keeps track of all the pools which claim to be able to provide memory for a given ctype.
In the example above, `amc` would generate functions `file_Alloc` and `file_Delete`.
The names of these functions are not derived from the ctype; They are derived
from the pool name. It is the pool's state that gets altered when one of these functions
is called.

### Base Pools
<a href="#base-pools"></a>

For any pool, you can specify where to get memory from. The linkage is specified with
a `basepool` record:
In the example below, the `Lpool` `algo_lib.FDb.lpool` calls `sbrk_AllocMem`
whenever it needs more memory.

    dmmeta.field  field:algo_lib.FDb.sbrk   arg:u8  reftype:Sbrk   dflt:""  comment:"Base allocator for everything"
    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

Base pools allow you to create non-overlapping memory zones for different types, which can aid
in program security and performance. They can be configured and tuned without touching any code.

A default pool for a namespace is specified in the `nsx` record

```
dmmeta.nsx  ns:algo_lib   genthrow:N  correct_getorcreate:Y  pool:algo_lib.FDb.lpool   sortxref:N  pack:N  fldoffset_asserts:N  comment:""
```

### Generated functions
<a href="#generated-functions"></a>

Every pool reftype hands the same allocation surface to its callers, because
each of their generators calls one shared tclass, `Pool` (source
`cpp/amc/pool.cpp`).  The functions are named after the pool field, not after
the ctype being allocated: it is the pool's state that changes.

|Tfunc|Generated function|Emitted for|Effect|
|---|---|---|---|
|`Pool.Alloc`|`T& <name>_Alloc()`|a fixed-size row type|Allocate one row.  Exits the process on out-of-memory.|
|`Pool.AllocMaybe`|`T* <name>_AllocMaybe()`|a fixed-size row type|Allocate one row; returns NULL on out-of-memory.|
|`Pool.AllocExtra`|`T& <name>_AllocExtra(void *extra, i32 nbyte_extra)`|a variable-size row type|Allocate a row plus `nbyte_extra` trailing bytes, copied from `extra`.  Fatal on out-of-memory.|
|`Pool.AllocExtraMaybe`|`T* <name>_AllocExtraMaybe(void *extra, i32 nbyte_extra)`|a variable-size row type|Same, returning NULL instead of exiting.|
|`Pool.AllocVarlen`|`T& <name>_AllocVarlen(aryptr<V> <varfld>)`|a row type with a `Varlen` field|Typed form of `AllocExtra`: sizes the trailing array from the passed span.|
|`Pool.AllocVarlenMaybe`|`T* <name>_AllocVarlenMaybe(aryptr<V> <varfld>)`|a row type with a `Varlen` field|Same, returning NULL instead of exiting.|
|`Pool.InsertMaybe`|`T* <name>_InsertMaybe(const Base &value)`|a copyable row type|Allocate a row from `value`, then cross-reference it.  NULL if the allocation failed or an index rejected the row (a duplicate key, say).|
|`Pool.UpdateMaybe`|`T* <name>_UpdateMaybe(Base &value)`|`finput` with `update:Y`, a copyable ctype with a primary index|Find the row by primary key and overwrite it, or insert it if absent.  The row is removed from the secondary indexes first and re-inserted after, so a changed key lands in the right bucket.|
|`Pool.Delete`|`void <name>_Delete(T& row)`|a reftype with `del:Y` — Malloc, Tpool, Lpool, Blkpool|Remove `row` from every index it sits in, then return its memory to the pool.|

A row type is *variable-size* when it has a `dmmeta.lenfld` and either an
`Opt` field or a `Varlen` field.  Such a row has no single size, so it gets
the `AllocExtra` family instead of plain `Alloc`, and the pool's reftype has
to permit varlen allocation.

The `Maybe` suffix is the naming rule for the whole family, and it is what
[Running Out Of Memory](#running-out-of-memory) below is about: a function
named `Alloc` exits the process when memory runs out, and `AllocMaybe`
returns NULL.

### Running Out Of Memory
<a href="#running-out-of-memory"></a>

There are two ways to run out of memory: voluntarily (because you decide a table got too big)
and involuntarily (the OS refuses to give you more memory). The first method is on the user --
`amc` does not support pool limits. When the OS is out of memory, the function either exits
the calling process (it's a fatal error) or returns NULL.  A function
with an unambiguous name such as `Alloc` will kill the process when an out-of-memory condition occurs.
A function ending in `Maybe`, such as `AllocMaybe`, will return NULL.

One of the use cases of `amc` is to generate deterministic processes. Think of two programs, running
in parallel on two different physical hosts, processing the same sequence of messages. The output
of both of these processes is sent to the same destination and de-duplicated based on sequence numbers.
This is a hot-hot redundancy scenario with great latency characteristics (the faster of the two messages
becomes the output, shaving off some latency spikes). In any case, the output of the two processes
must be identical and depend only on the input. This means that a process is *not allowed* to strategize
around low-memory conditions. That's why exiting on out-of-memory is a valid, in fact the only possible
strategy.

The full description of all types of memory pools is given in
[Reftypes](/txt/exe/amc/reftype.md); each pool reftype has its own
page under `/txt/exe/amc/reftype/`.
