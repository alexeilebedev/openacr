## Amc: Memory Pools

For each `ctype`, instances of which can be dynamically allocated
(i.e. not on the stack), amc generates two functions: `Alloc` and `Delete`.

This section will explain all of the available pool types, how to chain them
so that one pool allocates its own memory from another, and provide examples.

In `amc`, memory pools are fields with specific reftypes. They are given names and 
they can be referred to.

### Types of Memory Pools

A memory pool can be thought of as any mechanism by which new instances of records can be allocated.
Amc provides the following base set of memory pools, listed here
in alphabetical order.

* Blkpool - A mostly-fifo allocator based on refcounted blocks.
* Delptr - Indirect Val; A single value, always freed in destructor.
* Inlary - A piece of memory for min..max elements inside the parent struct. 
 When `min == max`, there is no `Alloc` function, and it behaves like N `Val`s.
 When `min < max`, new elements can be allocated. Only the last element can be freed.
* Lary - Effectively an Inlary of 32 Tarys, each of size 2^k. Has permanent pointers.
* Lpool - 32 Tpools, each for elements of size up to 2^k.
* Malloc - Simply calls malloc() / free().
* Sbrk - Calls sbrk() to get memory.
* Tary - Pointer to a resizable array (typically growing by powers of 2). Similar to std::vector.
* Tpool - A singly linked list of free elements; Gets new memory from base pool, frees 
  elements into the list.
* Val - A single value, automatically allocated in constructor, and freed in destructor. 

### Declaring a Pool

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

For any pool, you can specify where to get memory from. This is called a basepool,
and is specified with a `dmmeta.basepool` record.
In the example below, the `Lpool` `algo_lib.FDb.lpool` calls `sbrk_AllocMem` 
whenever it needs more memory.

    dmmeta.field  field:algo_lib.FDb.sbrk   arg:u8  reftype:Sbrk   dflt:""  comment:"Base allocator for everything"
    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

The basepool is a really great feature. Imagine a realtime program with strict
memory requirements; Everything must be pre-allocated and system calls are not allowed during runtime,
since they would cause latency spikes 1000x of what's allowed. The configuration of memory
pools can be done entirely outside of the source code base. One can write straightforward bash scripts that try 
different memory pools and measure their relative performance.

To avoid having to specify a base pool for every single pool in a given
namespace, you can (actually, you must) specify a default namespace pool in the `dmmeta.nsx` record.

    dmmeta.nsx  ns:algo_lib   genthrow:N  correct_getorcreate:Y  pool:algo_lib.FDb.lpool   sortxref:N  pack:N  fldoffset_asserts:N  comment:""

### Running Out Of Memory

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
becomes the output, shaving off some latency spikes). In any case, the output of the two proceses
must be identical and depend only on the input. This means that a process is *not allowed* to strategize
around low-memory conditions. That's why existing on out-of-memory is a valid, in fact the only possible
strategy.

### Blkpool

The block pool is a free-list of large-ish blocks. Memory is allocated from the current block,
with others serving as reserve. Allocated elements contain a back-pointer to the beginning
of the block, and increment a refcount on the block. Elements are allocated back-to-back
starting at the beginning of the block until the block is exhausted, respecting the necessary
alignment.

When freeing memory, the refcount is decremented but memory cannot be reused (yet). Only when
the block's refcount goes to zero, the entire block goes back to the free list. This allocator
is suitable for messages that are being processed using a fifo strategy. It is not good
for random access patterns because one unfreed message can hold an entire block in memory,
eventually exhausting memory.

### Delptr: Optional Value

Delptr is an indirect Val. The parent struct gets a pointer which is initially `NULL`.
When you call `_Access`, and the point is still `NULL`, a new value is allocated.
When the the struct is deleted, the value is freed.

Here is an example of a `Delptr` field:

    dmmeta.ctype  ctype:atf_amc.DelType1  comment:"Delptr test 1"
      dmmeta.field  field:atf_amc.DelType1.u32val  arg:u32  reftype:Delptr  dflt:34  comment:""

Here is the generated struct:

    // --- atf_amc.DelType1
    struct DelType1 { // atf_amc.DelType1: Delptr test 1
        u32*   u32val;   // Private pointer to value
        DelType1();
        ~DelType1();
    private:
        // reftype of atf_amc.DelType1.u32val prohibits copy
        DelType1(const DelType1&){ /*disallow copy constructor */}
        void operator =(const DelType1&){ /*disallow direct assignment */}
    };

And here is the implementation of `_Access`.
The underlying memory pool used is the one associated with its namespace
via the `nsx` record.

    // --- atf_amc.DelType1.u32val.Access
    // Get or Create
    // Access value, creating it if necessary. Process dies if not successful.
    u32& atf_amc::u32val_Access(atf_amc::DelType1& parent) {
        u32 *ret=parent.u32val;
        if (!ret) {
            ret = (u32*)algo_lib::malloc_AllocMem(sizeof(u32));
            if (!ret) {
                FatalErrorExit("out of memory allocating u32 (in atf_amc::DelType1.u32val)");
            }
            new(ret) u32(34);
            parent.u32val = ret;
        }
        return *ret;
    }

### Inlary: Inline Array

*Tbd*

### Lary: Level array

Lary is implemented as 32 pointers in the parent struct. Level k holds as pointer
to a block of elements of length 2^k. Indexed lookup involves just 1 memory access,
because amc uses BitScanReverse to find which level the element lives on. When a level is exhausted,
another level, 2x the size, is allocated. Since none of the previous levels need to be
reallocated, the pointers returned by Lary are stable and so elements can be freely cross-referenced.
Lary is the most common pool.

The permanent pointer promise is the main reason for Lary's existence. Lary is the default
pool for most records.
Here is an example of one:

    dmmeta.field  field:ssim2mysql.FDb.ctype  arg:ssim2mysql.FCtype  reftype:Lary  dflt:""  comment:""
    
And here are the generated functions.

#### Alloc
    // Allocate memory for new default row.
    // If out of memory, process is killed.
    ssim2mysql::FCtype&  ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));

#### AllocMaybe

    // Allocate memory for new element. If out of memory, return NULL.
    ssim2mysql::FCtype*  ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));

#### InsertMaybe

    // Create new row from struct.
    // Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)
    ssim2mysql::FCtype*  ctype_InsertMaybe(const dmmeta::Ctype &value) __attribute__((nothrow));

#### AllocMem

    // Allocate space for one element. If no memory available, return NULL.
    void*                ctype_AllocMem() __attribute__((__warn_unused_result__, nothrow));

#### EmptyQ

    // Return true if index is empty
    bool                 ctype_EmptyQ() __attribute__((nothrow));

#### Find

    // Look up row by row id. Return NULL if out of range
    ssim2mysql::FCtype*  ctype_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));

#### Last

    // Return pointer to last element of array, or NULL if array is empty
    ssim2mysql::FCtype*  ctype_Last() __attribute__((nothrow, pure));

#### N

    // Return number of items in the pool
    i32                  ctype_N() __attribute__((__warn_unused_result__, nothrow, pure));

#### RemoveLast

    // Delete last element of array. Do nothing if array is empty.
    void                 ctype_RemoveLast() __attribute__((nothrow));

#### qFind

    // 'quick' Access row by row id. No bounds checking.
    ssim2mysql::FCtype&  ctype_qFind(u64 t) __attribute__((nothrow));

#### XrefMaybe

    // Insert row into all appropriate indices. If error occurs, store error
    // in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
    bool                 ctype_XrefMaybe(ssim2mysql::FCtype &row);

### Lpool: Level pool

Lpool is 32 Tpools, one for each allocation size. When allocating memory, the request
is bumped up to the nearest power of 2 and from there Tpool logic is followed.

Here is an example of an Lpool declaration:

    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

And here are the generated functions. As always, the actual code 
of these functions can be queried with `amc algo_lib.FDb.lpool.%`

#### FreeMem

    // Free block of memory previously returned by Lpool.
    void                 lpool_FreeMem(void *mem, u64 size) __attribute__((nothrow));

#### AllocMem

    // Allocate new piece of memory at least SIZE bytes long.
    // If not successful, return NULL
    // The allocated block is 16-byte aligned
    void*                lpool_AllocMem(u64 size) __attribute__((__warn_unused_result__, nothrow));

#### ReserveBuffers

    // Add N buffers of some size to the free store
    bool                 lpool_ReserveBuffers(int nbuf, u64 bufsize) __attribute__((nothrow));

#### ReallocMem

    // Allocate new block, copy old to new, delete old.
    // New memory is always allocated (i.e. size reduction is not a no-op)
    // If no memory, return NULL: old memory untouched
    void*                lpool_ReallocMem(void *oldmem, u64 old_size, u64 new_size) __attribute__((nothrow));
              
### Malloc

Pass-through to libc's malloc / free.

### Sbrk

*Tbd*

### Tary: Flat indirect array (vector)

Tary is a dynamically allocated resizable array of values. A single block of memory
is used for all elements.
Taking the address of a Tary element is not allowed, although it cannot be prevented
Records allocated with Tary cannot be cross-referenced, this is enforced by `amc`.
`algo.ByteAry` is defined as Tary of u8. `algo.cstring` is defined as Tary of char.
When growing a full Tary (such as from Reserve or Alloc functions), 
the size is always at least doubled.

Here is an example of a Tary field:

    dmmeta.field  field:algo.LineBuf.buf  arg:char  reftype:Tary  dflt:""  comment:""
      dmmeta.tary  field:algo.LineBuf.buf  aliased:Y  comment:""

#### Aliasing

The `aliased` attribute of the `tary` record specifies whether functions involving
`aryptr` will be generated.
When they are, it is possible that the `aryptr` being passed to `Addary` or `Setary` is 
a subrange of the array itself. A check is inserted for this condition, and it's a fatal 
program error if the check fails. Even though `amc` could adjust the incoming pointer
before and after calling `Reserve`, the caller still has a bad `aryptr` on their hands,
which means there is a program error.

The following functions are generated:

#### Addary

    // Reserve space (this may move memory). Insert N element at the end.
    // Return aryptr to newly inserted block.
    // If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
    algo::aryptr<char>   buf_Addary(algo::LineBuf& parent, algo::aryptr<char> rhs) __attribute__((nothrow));

#### Alloc

    // Reserve space. Insert element at the end
    // The new element is initialized to a default value
    char&                buf_Alloc(algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow));

#### AllocAt

    // Reserve space for new element, reallocating the array if necessary
    // Insert new element at specified index. Index must be in range or a fatal error occurs.
    char&                buf_AllocAt(algo::LineBuf& parent, int at) __attribute__((__warn_unused_result__, nothrow));

#### AllocN

    // Reserve space. Insert N elements at the end of the array, return pointer to array
    algo::aryptr<char>   buf_AllocN(algo::LineBuf& parent, int n_elems) __attribute__((__warn_unused_result__, nothrow));

#### EmptyQ

    // Return true if index is empty
    bool                 buf_EmptyQ(algo::LineBuf& parent) __attribute__((nothrow));

#### Find

    // Look up row by row id. Return NULL if out of range
    char*                buf_Find(algo::LineBuf& parent, u64 t) __attribute__((__warn_unused_result__, nothrow));

#### Getary

    // Return array pointer by value
    algo::aryptr<char>   buf_Getary(algo::LineBuf& parent) __attribute__((nothrow));

#### Last

    // Return pointer to last element of array, or NULL if array is empty
    char*                buf_Last(algo::LineBuf& parent) __attribute__((nothrow, pure));

#### Max

    // Return max. number of items in the array
    i32                  buf_Max(algo::LineBuf& parent) __attribute__((nothrow));

#### N

    // Return number of items in the array
    i32                  buf_N(const algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow, pure));

#### Remove

    // Remove item by index. If index outside of range, do nothing.
    void                 buf_Remove(algo::LineBuf& parent, u32 i) __attribute__((nothrow));

#### RemoveAll

    void                 buf_RemoveAll(algo::LineBuf& parent) __attribute__((nothrow));

#### RemoveLast

    // Delete last element of array. Do nothing if array is empty.
    void                 buf_RemoveLast(algo::LineBuf& parent) __attribute__((nothrow));

#### Reserve

    // Make sure N *more* elements will fit in array. Process dies if out of memory
    void                 buf_Reserve(algo::LineBuf& parent, int n) __attribute__((nothrow));

#### AbsReserve

    // Make sure N elements fit in array. Process dies if out of memory
    void                 buf_AbsReserve(algo::LineBuf& parent, int n) __attribute__((nothrow));

#### Setary (copy one Tary to another)

    // Copy contents of RHS to PARENT.
    void                 buf_Setary(algo::LineBuf& parent, algo::LineBuf &rhs) __attribute__((nothrow));

#### Setary

    // Copy specified array into buf, discarding previous contents.
    // If the RHS argument aliases the array (refers to the same memory), throw exception.
    void                 buf_Setary(algo::LineBuf& parent, const algo::aryptr<char> &rhs) __attribute__((nothrow));

#### qFind

    // 'quick' Access row by row id. No bounds checking.
    char&                buf_qFind(algo::LineBuf& parent, u64 t) __attribute__((nothrow));

#### qLast

    // Return reference to last element of array. No bounds checking
    char&                buf_qLast(algo::LineBuf& parent) __attribute__((nothrow));

### Tpool

This pool type only supports fixed-size allocation. Free elements area stored in a singly
linked list. if the list is empty, tpool uses the base allocator
(or the namespace default allocator)
to fulfill the request. The free list can be refilled with ReserveMem. 
The memory obtained by Tpool from the base allocator is never returned. 
Tpools can only be global (otherwise, memory leaks would occur).
This is the fastest allocator, because it only takes a couple of instructions to peel a free element
off of the free list.

Here is an example of a `Tpool`:

    dmmeta.field  field:ssim2mysql.FDb.cmd     arg:ssim2mysql.FCmd     reftype:Tpool   dflt:""  comment:""

And here is the generated code:

#### Alloc

    // Allocate memory for new default row.
    // If out of memory, process is killed.
    ssim2mysql::FCmd&    cmd_Alloc() __attribute__((__warn_unused_result__, nothrow));

#### AllocMaybe

    // Allocate memory for new element. If out of memory, return NULL.
    ssim2mysql::FCmd*    cmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));

#### Delete

    // Remove row from all global and cross indices, then deallocate row
    void                 cmd_Delete(ssim2mysql::FCmd &row) __attribute__((nothrow));

#### AllocMem

    // Allocate space for one element
    // If no memory available, return NULL.
    void*                cmd_AllocMem() __attribute__((__warn_unused_result__, nothrow));

#### FreeMem

    // Remove mem from all global and cross indices, then deallocate mem
    void                 cmd_FreeMem(ssim2mysql::FCmd &row) __attribute__((nothrow));

#### Reserve

    // Preallocate memory for N more elements
    // Return number of elements actually reserved.
    u64                  cmd_Reserve(u64 n_elems) __attribute__((nothrow));

#### ReserveMem

    // Allocate block of given size, break up into small elements and append to free list.
    // Return number of elements reserved.
    u64                  cmd_ReserveMem(u64 size) __attribute__((nothrow));

#### XrefMaybe

    // Insert row into all appropriate indices. If error occurs, store error
    // in algo_lib::_db.errtext and return false. Call Unref or Delete to cleanup partially inserted row.
    bool                 cmd_XrefMaybe(ssim2mysql::FCmd &row);

### Val: Value

Val is the simplest reftype. A field of type val becomes a regular struct member
on output.
Here is an example of a `Val` field:

    dmmeta.ctype  ctype:algo.UnTime  comment:"unix time * 1e9 + nanoseconds"
      dmmeta.field  field:algo.UnTime.value  arg:i64  reftype:Val  dflt:""  comment:""

And here is what `amc` produces on output:

    // --- algo.UnTime
    struct UnTime { // algo.UnTime: unix time * 1e9 + nanoseconds
        i64   value;   //   0
        UnTime();
    };
