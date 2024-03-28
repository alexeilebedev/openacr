## AMC Reftypes
<a href="#amc-reftypes"></a>
The main attributes of a field are its name, arg, and `reftype`.
Arg is the type, and reftype is a modifier, or 'type constructor'.

    dmmeta.field  field:amc.FDb.ind_ns        arg:amc.FNs        reftype:Thash   dflt:""  comment:""
    dmmeta.field  field:dmmeta.Field.field     arg:algo.Smallstr100  reftype:Val  dflt:"  comment:"

The reftype are documented below:

### Reftypes
<a href="#reftypes"></a>

#### Alias - Access another field
<a href="#alias"></a>

#### Atree - X-reference: AVL tree
<a href="#atree"></a>

#### Base - Incorporate base ctype fields
<a href="#base"></a>

#### Bheap - X-reference: Binary heap built on top of ptrary.
<a href="#bheap"></a>

#### Bitfld - A field whose value is stored in the bits of another field
<a href="#bitfld"></a>

#### Blkpool - variable size block-based allocator suitable for fifo use
<a href="#blkpool"></a>

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

#### Charset - 
<a href="#charset"></a>

#### Count - A cross-reference that simply counts # of children
<a href="#count"></a>

#### Cppstack - A pool whose only purpose is to provide a name
<a href="#cppstack"></a>

#### Delptr - Pointer to a private cascade-delete value (like Val by starts as NULL)
<a href="#delptr"></a>

Delptr is an indirect Val. The parent struct gets a pointer which is initially `NULL`.
When you call `_Access`, and the point is still `NULL`, a new value is allocated.
When the the struct is deleted, the value is freed.

Here is an example of a `Delptr` field:
```
    dmmeta.ctype  ctype:atf_amc.DelType1  comment:"Delptr test 1"
      dmmeta.field  field:atf_amc.DelType1.u32val  arg:u32  reftype:Delptr  dflt:34  comment:""
```

The following functions are generated:

```
inline-command: amc -report:N atf_amc.DelType1.u32val.% -proto
// Get or Create
// Access value, creating it if necessary. Process dies if not successful.
// func:atf_amc.DelType1.u32val.Access
u32&                 u32val_Access(atf_amc::DelType1& parent) __attribute__((nothrow));
// Delete value.
// func:atf_amc.DelType1.u32val.Delete
void                 u32val_Delete(atf_amc::DelType1& parent) __attribute__((nothrow));

```

#### Exec - Subprocess
<a href="#exec"></a>

#### Fbuf - Byte buffer with epoll integration & message scanning
<a href="#fbuf"></a>

#### Global - Global variable -- root of all access paths
<a href="#global---global-variable"></a>

#### Hook - Pointer to a function
<a href="#hook"></a>

#### Inlary - Zero to N values, allocated inside the parent struct
<a href="#inlary"></a>

#### Lary - Array of 32 non-resizable arrays of values. Permanent pointers.
<a href="#lary"></a>

Lary is the default allocator for most records that don't need to be deleted after they are created.
It is implemented as an inline array of 36 pointers in the parent struct. The first pointer
is either NULL or points to a block of 4 elements. The next pointer is either NULL or pointers to a block
of 8 elements, etc. Thus, the maximum size of Lary is 2^41.
Level k holds as pointer
to a block of elements of length 2^(k+2). Indexed lookup involves just 1 memory access,
using BitScanReverse to find which level the element lives on. When a level is exhausted,
another level, 2x the size, is allocated. Since none of the previous levels need to be
reallocated, the pointers returned by Lary are stable and so elements can be freely cross-referenced.

```
dmmeta.field  field:ssim2mysql.FDb.ctype  arg:ssim2mysql.FCtype  reftype:Lary  dflt:""  comment:""
```

The following functions are generated:
```
inline-command: amc -report:N ssim2mysql.FDb.ctype.% -proto
// Allocate memory for new default row.
// If out of memory, process is killed.
// func:ssim2mysql.FDb.ctype.Alloc
ssim2mysql::FCtype&  ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
// func:ssim2mysql.FDb.ctype.AllocMaybe
ssim2mysql::FCtype*  ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
// Create new row from struct.
// Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)
// func:ssim2mysql.FDb.ctype.InsertMaybe
ssim2mysql::FCtype*  ctype_InsertMaybe(const dmmeta::Ctype &value) __attribute__((nothrow));
// Allocate space for one element. If no memory available, return NULL.
// func:ssim2mysql.FDb.ctype.AllocMem
void*                ctype_AllocMem() __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:ssim2mysql.FDb.ctype.EmptyQ
bool                 ctype_EmptyQ() __attribute__((nothrow, pure));
// Look up row by row id. Return NULL if out of range
// func:ssim2mysql.FDb.ctype.Find
ssim2mysql::FCtype*  ctype_Find(u64 t) __attribute__((__warn_unused_result__, nothrow, pure));
// Return pointer to last element of array, or NULL if array is empty
// func:ssim2mysql.FDb.ctype.Last
ssim2mysql::FCtype*  ctype_Last() __attribute__((nothrow, pure));
// Return number of items in the pool
// func:ssim2mysql.FDb.ctype.N
i32                  ctype_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Delete last element of array. Do nothing if array is empty.
// func:ssim2mysql.FDb.ctype.RemoveLast
void                 ctype_RemoveLast() __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:ssim2mysql.FDb.ctype.qFind
ssim2mysql::FCtype&  ctype_qFind(u64 t) __attribute__((nothrow, pure));
// func:ssim2mysql.FDb.ctype.InputMaybe
static bool          ctype_InputMaybe(dmmeta::Ctype &elem) __attribute__((nothrow));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:ssim2mysql.FDb.ctype.XrefMaybe
bool                 ctype_XrefMaybe(ssim2mysql::FCtype &row);

```

#### Llist - X-reference: any of 32 possible types of linked list
<a href="#llist"></a>

#### Lpool - Varlen pool, implemented as array of 32 Tpools
<a href="#lpool"></a>

#### Malloc - Pass-through for malloc / free
<a href="#malloc"></a>

#### Opt - Optional trailing struct field occupying rest of space
<a href="#opt"></a>

#### Pkey - Primary key reference
<a href="#pkey"></a>

#### Protocol - 
<a href="#protocol"></a>

#### Ptr - Cross-reference pointer to a future record
<a href="#ptr"></a>

#### Ptrary - Array of pointers
<a href="#ptrary"></a>

#### Regx - Pkey regx reference (on relational types), Regx when using in-memory DB
<a href="#regx"></a>

#### RegxSql - Pkey regx reference
<a href="#regxsql"></a>

#### Sbrk - Call Sbrk
<a href="#sbrk"></a>

#### Smallstr - Fixed-length string field
<a href="#smallstr"></a>

#### Tary - Flat, reallocatable array of values
<a href="#tary"></a>

#### Thash - X-reference; hash built on top of ptrary.
<a href="#thash"></a>

#### Tpool - Singly linked free-list.
<a href="#tpool"></a>

#### Upptr - Pointer to a past (pre-existing) record
<a href="#upptr"></a>

#### Val - Single cascade-insert in-place value.
<a href="#val"></a>

#### Varlen - variable-length field
<a href="#varlen"></a>

#### ZSListMT - zero-terminated singly linked list with thread-safe access
<a href="#zslistmt"></a>

### Lpool: Level pool
<a href="#lpool-level-pool"></a>

Lpool is 36 Tpools, one for each allocation size.
When allocating memory, the request
is bumped up to the nearest power of 2 and from there Tpool logic is followed.
The minimum allocation size is 16 (so, the first slot in the array of Tpools corresponds
to size 16). The maximum size is `2^40`.

Here is an example of an Lpool declaration:

    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

The following functions are generated:
```
inline-command: amc -report:N algo_lib.FDb.lpool.% -proto
// Free block of memory previously returned by Lpool.
// func:algo_lib.FDb.lpool.FreeMem
void                 lpool_FreeMem(void *mem, u64 size) __attribute__((nothrow));
// Allocate new piece of memory at least SIZE bytes long.
// If not successful, return NULL
// The allocated block is 16-byte aligned
// func:algo_lib.FDb.lpool.AllocMem
void*                lpool_AllocMem(u64 size) __attribute__((__warn_unused_result__, nothrow));
// Add N buffers of some size to the free store
// func:algo_lib.FDb.lpool.ReserveBuffers
bool                 lpool_ReserveBuffers(int nbuf, u64 bufsize) __attribute__((nothrow));
// Allocate new block, copy old to new, delete old.
// New memory is always allocated (i.e. size reduction is not a no-op)
// If no memory, return NULL: old memory untouched
// func:algo_lib.FDb.lpool.ReallocMem
void*                lpool_ReallocMem(void *oldmem, u64 old_size, u64 new_size) __attribute__((nothrow));

```

#### Malloc - Pass-through for malloc / free
<a href="#malloc"></a>

This pool uses the libc's `malloc()` / `free()` functions to allocate/deallocate memory.

#### Opt - Optional trailing struct field occupying rest of space
<a href="#opt"></a>

#### Pkey - Primary key reference
<a href="#pkey"></a>

#### Protocol - 
<a href="#protocol"></a>

#### Ptr - Cross-reference pointer to a future record
<a href="#ptr"></a>

#### Ptrary - Array of pointers
<a href="#ptrary"></a>

#### Regx - Pkey regx reference (on relational types), Regx when using in-memory DB
<a href="#regx"></a>

#### RegxSql - Pkey regx reference
<a href="#regxsql"></a>

#### Sbrk - Call Sbrk
<a href="#sbrk"></a>

This pool allocates memory by calling sbrk.
Here is an example:
```
dmmeta.field  field:algo_lib.FDb.sbrk  arg:u8  reftype:Sbrk  dflt:""  comment:"Base allocator for everything"
```

The following functions are generated:
```
inline-command: amc -report:N algo_lib.FDb.sbrk.% -proto
// Allocate a new piece of memory at least SIZE bytes long.
// If out of memory, return NULL
// Newly allocated memory is initialized to zeros
// func:algo_lib.FDb.sbrk.AllocMem
void*                sbrk_AllocMem(u32 size) __attribute__((__warn_unused_result__, nothrow));
// func:algo_lib.FDb.sbrk.FreeMem
void                 sbrk_FreeMem(void *mem, u32 size) __attribute__((nothrow));

```

Sbrk supports huge memory pages. To enable this behavior, set `<field>_huge_limit` to some value.
The total number of bytes allocated is kept track of in the `<field>_huge_alloc` variable.
By default, newly returned memory is zeroed. To disable this behavior, set `<field>_zeromem` to false.

```
u64   sbrk_huge_limit;                              // Huge page limit in bytes -- set to 0 with fork!
u64   sbrk_huge_alloc;                              // Huge page bytes allocated
bool  sbrk_zeromem;                                 // Zero allocated memory
```            

#### Smallstr - Fixed-length string field
<a href="#smallstr"></a>

#### Tary - Flat, reallocatable array of values
<a href="#tary"></a>
Tary is a dynamically allocated resizable array of values. It is similar to C++'s std::vector.
A single block of memory is used for all elements.
Records allocated with Tary cannot be cross-referenced, this is enforced by `amc`.
When growing a full Tary (such as from Reserve or Alloc functions),
the size is always at least doubled.

Here is an example of a Tary field:

    dmmeta.field  field:algo.LineBuf.buf  arg:char  reftype:Tary  dflt:""  comment:""
      dmmeta.tary  field:algo.LineBuf.buf  aliased:Y  comment:""

The `aliased` attribute of the `tary` record specifies whether functions involving
`aryptr` will be generated.
When they are, it is possible that the `aryptr` being passed to `Addary` or `Setary` is
a subrange of the array itself. A check is inserted for this condition, and it's a fatal
program error if the check fails. Even though `amc` could adjust the incoming pointer
before and after calling `Reserve`, the caller still has a bad `aryptr` on their hands,
which means there is a program error.

The following functions are generated for Tary:
```
inline-command: amc -report:N algo.LineBuf.buf.% -proto
// Reserve space (this may move memory). Insert N element at the end.
// Return aryptr to newly inserted block.
// If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
// func:algo.LineBuf.buf.Addary
algo::aryptr<char>   buf_Addary(algo::LineBuf& parent, algo::aryptr<char> rhs) __attribute__((nothrow));
// Reserve space. Insert element at the end
// The new element is initialized to a default value
// func:algo.LineBuf.buf.Alloc
char&                buf_Alloc(algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow));
// Reserve space for new element, reallocating the array if necessary
// Insert new element at specified index. Index must be in range or a fatal error occurs.
// func:algo.LineBuf.buf.AllocAt
char&                buf_AllocAt(algo::LineBuf& parent, int at) __attribute__((__warn_unused_result__, nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:algo.LineBuf.buf.AllocN
algo::aryptr<char>   buf_AllocN(algo::LineBuf& parent, int n_elems) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:algo.LineBuf.buf.EmptyQ
bool                 buf_EmptyQ(algo::LineBuf& parent) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:algo.LineBuf.buf.Find
char*                buf_Find(algo::LineBuf& parent, u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
// func:algo.LineBuf.buf.Getary
algo::aryptr<char>   buf_Getary(algo::LineBuf& parent) __attribute__((nothrow));
// Return pointer to last element of array, or NULL if array is empty
// func:algo.LineBuf.buf.Last
char*                buf_Last(algo::LineBuf& parent) __attribute__((nothrow, pure));
// Return max. number of items in the array
// func:algo.LineBuf.buf.Max
i32                  buf_Max(algo::LineBuf& parent) __attribute__((nothrow));
// Return number of items in the array
// func:algo.LineBuf.buf.N
i32                  buf_N(const algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow, pure));
// Remove item by index. If index outside of range, do nothing.
// func:algo.LineBuf.buf.Remove
void                 buf_Remove(algo::LineBuf& parent, u32 i) __attribute__((nothrow));
// func:algo.LineBuf.buf.RemoveAll
void                 buf_RemoveAll(algo::LineBuf& parent) __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:algo.LineBuf.buf.RemoveLast
void                 buf_RemoveLast(algo::LineBuf& parent) __attribute__((nothrow));
// Make sure N *more* elements will fit in array. Process dies if out of memory
// func:algo.LineBuf.buf.Reserve
void                 buf_Reserve(algo::LineBuf& parent, int n) __attribute__((nothrow));
// Make sure N elements fit in array. Process dies if out of memory
// func:algo.LineBuf.buf.AbsReserve
void                 buf_AbsReserve(algo::LineBuf& parent, int n) __attribute__((nothrow));
// Copy contents of RHS to PARENT.
// func:algo.LineBuf.buf.Setary
void                 buf_Setary(algo::LineBuf& parent, algo::LineBuf &rhs) __attribute__((nothrow));
// Copy specified array into buf, discarding previous contents.
// If the RHS argument aliases the array (refers to the same memory), throw exception.
// func:algo.LineBuf.buf.Setary2
void                 buf_Setary(algo::LineBuf& parent, const algo::aryptr<char> &rhs) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:algo.LineBuf.buf.qFind
char&                buf_qFind(algo::LineBuf& parent, u64 t) __attribute__((nothrow));
// Return reference to last element of array. No bounds checking
// func:algo.LineBuf.buf.qLast
char&                buf_qLast(algo::LineBuf& parent) __attribute__((nothrow));
// Return row id of specified element
// func:algo.LineBuf.buf.rowid_Get
u64                  buf_rowid_Get(algo::LineBuf& parent, char &elem) __attribute__((nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:algo.LineBuf.buf.AllocNVal
algo::aryptr<char>   buf_AllocNVal(algo::LineBuf& parent, int n_elems, const char& val) __attribute__((nothrow));

```

#### Thash - X-reference; hash built on top of ptrary.
<a href="#thash"></a>

#### Tpool - Singly linked free-list.
<a href="#tpool"></a>

This pool type only supports fixed-size allocation. Free elements area stored in a singly
linked list. if the list is empty, tpool uses the base allocator
(or the namespace default allocator)
to fulfill the request. The free list can be pre-filled with ReserveMem.
The memory obtained by Tpool from the base allocator is never returned.
Tpools can only be global (otherwise, memory leaks would occur).
This is one of the fastest pool types.

Here is an example of a `Tpool`:

    dmmeta.field  field:ssim2mysql.FDb.cmd     arg:ssim2mysql.FCmd     reftype:Tpool   dflt:""  comment:""

And here is the generated code:

```
inline-command: amc -report:N ssim2mysql.FDb.cmd.% -proto
// Allocate memory for new default row.
// If out of memory, process is killed.
// func:ssim2mysql.FDb.cmd.Alloc
ssim2mysql::FCmd&    cmd_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
// func:ssim2mysql.FDb.cmd.AllocMaybe
ssim2mysql::FCmd*    cmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
// Remove row from all global and cross indices, then deallocate row
// func:ssim2mysql.FDb.cmd.Delete
void                 cmd_Delete(ssim2mysql::FCmd &row) __attribute__((nothrow));
// Allocate space for one element
// If no memory available, return NULL.
// func:ssim2mysql.FDb.cmd.AllocMem
void*                cmd_AllocMem() __attribute__((__warn_unused_result__, nothrow));
// Remove mem from all global and cross indices, then deallocate mem
// func:ssim2mysql.FDb.cmd.FreeMem
void                 cmd_FreeMem(ssim2mysql::FCmd &row) __attribute__((nothrow));
// Preallocate memory for N more elements
// Return number of elements actually reserved.
// func:ssim2mysql.FDb.cmd.Reserve
u64                  cmd_Reserve(u64 n_elems) __attribute__((nothrow));
// Allocate block of given size, break up into small elements and append to free list.
// Return number of elements reserved.
// func:ssim2mysql.FDb.cmd.ReserveMem
u64                  cmd_ReserveMem(u64 size) __attribute__((nothrow));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:ssim2mysql.FDb.cmd.XrefMaybe
bool                 cmd_XrefMaybe(ssim2mysql::FCmd &row);

```

#### Upptr - Pointer to a past (pre-existing) record
<a href="#upptr"></a>

Upptr is a pointer to a record.
Upptr is a reference. I.e. its value is established by a user or by a lookup, and does not change
in response to other records being inserted/delete.
Both Upptr and Ptr translate to a simple pointer in C++.

#### Val - Single cascade-insert in-place value.
<a href="#val"></a>

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

#### Varlen - variable-length field
<a href="#varlen"></a>
Varlen corresponds to a (possibly empty) array of fixed-size records appended to the end of a struct.
Varlen can only appear as the last field of a struct.
The parent ctype must have a length field defined. Varlen fields cane be read from a string or
iterated over. This type is frequently used by wire protocols to specify a repeated section of a message.

#### ZSListMT - zero-terminated singly linked list with thread-safe access
<a href="#zslistmt"></a>

