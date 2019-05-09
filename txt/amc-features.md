## Amc Features

### Scaled Decimal Types

A scaled decimal type is an integer with an implied decimal point. For instance, a number
with two implied decimal places can be constructed with an `fdec` record, specifying `nplace:2`.

Attribute `fixedfmt` controls whether trailing zeros are left in place when printing.
With `fixedfmt:Y`, 0 is printed as `0.00`, otherwise it is just printed as 0.

Example:

    dmmeta.field  field:algo.U64Dec2.value  arg:u64  reftype:Val  dflt:""  comment:""
      dmmeta.fdec  field:algo.U64Dec2.value  nplace:2  fixedfmt:Y  comment:"0 prints as 0.00"

The scaled decimal type generates functions to convert between the stored (scaled) value 
and a numeric value: `value_qSetDouble`, `value_SetDoubleMaybe`, `value_GetDouble`, `value_GetInt`.

    // Set value of field value.
    // The value is rounded to the nearest integer.
    // This ensures that truncation of a near-integer value does not occur.
    // Example: 1.3 cannot be represented exactly as a double, the actual
    // stored value will be 1.29999999. when we apply C truncation,
    // we want to end up with 1.3, not 1.2.
    void                 value_qSetDouble(algo::U64Dec2& parent, double val) __attribute__((nothrow));

    double               value_GetDouble(algo::U64Dec2& parent) __attribute__((nothrow));

    // Return integer portion (divide number by 100)
    u64                  value_GetInt(algo::U64Dec2& parent) __attribute__((nothrow));

    // Return constant 100
    u64                  U64Dec2_GetScale() __attribute__((nothrow));

    // Set value of field value, using rounding.
    // If value is out of range for the target type, return false.
    bool                 value_SetDoubleMaybe(algo::U64Dec2& parent, double val) __attribute__((nothrow));

Functions to print and read the value are also generated.
You can query the generated source for `value_Print` with `amc algo.U64Dec2.value.Print`

    bool                 value_ReadStrptrMaybe(algo::U64Dec2& parent, algo::strptr in) __attribute__((nothrow));
    void                 value_Print(algo::U64Dec2& parent, cstring &outstr) __attribute__((nothrow));

### Big-Endian Fields

Adding an `fbigend` record to a field indicates that the in-memory byte order is big-endian.

    dmmeta.field  field:atf_amc.TypeBE16.value  arg:u16  reftype:Val  dflt:""  comment:""
      dmmeta.fbigend  field:atf_amc.TypeBE16.value  comment:""
              
Amc generates values `value_Get` and `value_Set` to access the value.

    u16                  value_Get(const atf_amc::TypeBE16& parent) __attribute__((__warn_unused_result__, nothrow));
    void                 value_Set(atf_amc::TypeBE16& parent, u16 rhs) __attribute__((nothrow));

When generating the C++ struct, amc adds `_be` to the field name so that the programmer
becomes aware, should they choose to access it manually, that the field may be encoded.

    $ amc atf_amc.TypeBE16%
    // --- atf_amc.TypeBE16
    struct TypeBE16 { // atf_amc.TypeBE16
        u16   value_be;   //   0
        TypeBE16();
    };

    ...

    // --- atf_amc.TypeBE16.value.Get
    inline u16 atf_amc::value_Get(const atf_amc::TypeBE16& parent) {
        return be16toh(parent.value_be); // read big-endian value from memory
    }

    // --- atf_amc.TypeBE16.value.Set
    inline void atf_amc::value_Set(atf_amc::TypeBE16& parent, u16 rhs) {
        parent.value_be = htobe16(rhs); // write big-endian value to memory
    }
    
    ...
    
### Bitfields

Amc supports bitfields. In the example below, the field `bits5` is defined
as bits 5..10 of the value of the field called `value`. It is important that 
the source field is explicitly specified. This eliminates any ambiguity about where
in memory the bitfield may be. The source field can be any integer type.

Bitfields also work identically between big- and little-endian source fields.
The reason is that we define the bitfield in terms of a shift- and mask- operation
on the source field. It is purely an arithmetic operation that produces the value of the
bitfield.

If you've looked at Linux system headers for network protocols, you've seen ungainly `#ifdef` blocks.
They are needed in C because C bitfields are defined against memory, not against the value of 
a source field.

    dmmeta.field  field:atf_amc.BitfldType1.value  arg:u64  reftype:Val     dflt:""  comment:""
    dmmeta.field  field:atf_amc.BitfldType1.bits5   arg:u64  reftype:Bitfld  dflt:""  comment:""
      dmmeta.bitfld  field:atf_amc.BitfldType1.bits5   offset:5  width:10  srcfield:atf_amc.BitfldType1.value  comment:""
                    
A bitfield can have any type - `bool`, `u64`, or a custom type that wraps an integer.

    $ amc atf_amc.BitfldType1.bits5.% -proto

    // Retrieve bitfield from value of field value
    //    10 bits starting at bit 5.
    u64                  bits5_Get(const atf_amc::BitfldType1& parent) __attribute__((__warn_unused_result__, nothrow));

    // Set bitfield in value of field 'value'
    //    10 bits starting at bit 5.
    void                 bits5_Set(atf_amc::BitfldType1& parent, u64 rhs) __attribute__((nothrow));

The implementation is straightforward:

    $ amc atf_amc.BitfldType1.bits5.Set
    
    // --- atf_amc.BitfldType1.bits5.Set
    // Set bitfield in value of field 'value'
    //    10 bits starting at bit 5.
    inline void atf_amc::bits5_Set(atf_amc::BitfldType1& parent, u64 rhs) {
        u64 t1    = u64(0x3ff) << 5;
        u64 t2    = (u64(rhs) & 0x3ff) << 5;
        parent.value = u64((parent.value & ~t1) | t2);
    }

bitfields can be read from string tuples just like other fields.
When printing a ctype containing bitfields to an ssim tuple, amc does not print the source field;
instead, all the bitfields are printed instead. 

When printing a ctype containing bitfields using the `Raw` format, only the source field is printed,
and bitfields are skipped.

It is an error to specify a combination of offset and width that is out of bounds for the source type.
Amc will flag this. It is also an error to have two bitfields overlap. All of the `offset+width` 
ranges must be disjoint.

Here is an example of the code amc generates when the source field is big-endian. 
(Endianness tends to be confusing, because languages try hard to erase the boundary between 
memory and values, so the user gets an extra warning).

    $ amc atf_amc.TypeBE64sf.bit63.Get

    // --- atf_amc.TypeBE64sf.bit63.Get
    // Retrieve bitfield from value of field value
    //    1 bits starting at bit 63.
    //    NOTE: bits correspond to the the integer value of the field.
    //    The value is obtained by reading bytes from memory and swapping them.
    inline u64 atf_amc::bit63_Get(const atf_amc::TypeBE64sf& parent) {
        return u64((value_Get(parent) >> 63) & 0x01);
    }


### Steps

*UNDER CONSTRUCTION*.

Steps are a scheduling construct to associate actions to fields.

A step can be thought of as a cooperative thread -- a function that's assigned to
some top-level variable (pointer, bool, list or heap) that is invoked whenever that variable is non-empty,
and advances the process towards its goal.

The amc model for a server process is a top-level while loop, which calls
a top-level Step function for each namespace linked into the process.
The namespace Step function then performs some action for each fstep defined in that namespace.

The main loop is defined in terms of real time: it executes until the value of CPU clock (rdtsc)
reaches some limit (typically 2^64-1). The scheduling cycle begins by setting next\_loop to the limit,
then executing all the steps. the steps adjust next\_loop down depending on their scheduling needs.
At the end of the scheduling cycle, unused remaining time is given up to the OS via epoll or nanosleep
(if there are no file descriptors open). This way, both hot-spinning and blocking processes are covered
with minimal overhead.

The following step types are defined: Inline, InlineRecur, TimeHookRecur, Callback
To define a step that is performed periodically on a timer, use fdelay.

Inline step is the simplest: on every pass,
the empty condition is checked on the underlying field, and a user-provided Step function
is called if the field is non-empty.

InlineRecur step requires an fdelay record specifying the initial delay between steps.
The logic is the same as Inline, with a time-based delay between steps.

### Tracing

*UNDER CONSTRUCTION*.
Ftrace record can be used to enable counting of alloc/delete calls for each pool.

### Base: Mixin

*UNDER CONSTRUCTION*.

The Base reftype copies all fields from one arg to field's parent ctype.
There are two main use cases.

1. Protocol messages, where Base is used to declare a common message header for all message types.
Amc generates functions to go back and forth between header and specific type using Castdown / Castbase functions.

2. In-memory tables based on ssimfiles, which inherit all of the fields defined
in the ssimfile. These do not allow casting back and forth since memory layouts differ, there
are extra pointers in the in-memory version, some attributes are being used for cross-references (joins), etc.

### Bheap: Binary Heap

*UNDER CONSTRUCTION*.

The binary heap is implemented as an flat array of pointers (e.g. a Ptrary).

### Bitset: Bitset over an array

*UNDER CONSTRUCTION*.

Bitsets can be created on top of any integer field (e.g. u8 to u128) or array field (Inlary, Tary).
Amc generates functions to provide indexed access to bits of the underlying field.

### Count: Count elements

*UNDER CONSTRUCTION*.

Count is a xref type that simply keeps track of the number of child
elements referring to a given parent. The elements themselves are not accessible
via this field.

### Fconst: Enumerated type

*UNDER CONSTRUCTION*.
This reftype is not specified explicitly. It is applied when fconst record appears.
Each fconst record names a symbol and a C++ expression (value). The symbol is the string representation
of the vaule. Amc creates an enum type with values, and creates to-string and from-string functions
that translate between values and symbols.

A related record is Gconst, which can be used in place of fconst to create an enum type out of an entire table.

When printing, if there is no symbol associated with the numeric value of the field being printed, the
value is printed as a number. When reading, if input symbol doesn't map to any known value, it is parsed
as an integer instead.

### Inlary: Inline array

*UNDER CONSTRUCTION*.

Inlary uses memory reserved directly inside the parent struct. A dmmeta.inlary record
is required, specifying min,max elements. If min=max, it is considered a fixed array.
Fixed array has neither Alloc nor Delete functions, and there is no count of elements.
If min < max, then the first min elements are created when the parent is constructed, and the rest
can be dynamically allocated. The Inlary supports random access because it is an array.

One particularly cool use of Inlary is with gstatic. Whenever gstatic is specified, the contents of an ssim
table are ``statically'' included into the generated source file. When gstatic is combined with Inlary,
amc creates a C++ compiler symbol with a name derived from the primary key of the source table.
The symbol is a reference whose value is a compile-time constant since the offset to the variable
is known. The symbol can be used at runtime to access the record, which is guaranteed to exist
and be properly cross-referenced.

### Lary: Level array

*UNDER CONSTRUCTION*.

### Llist: Linked list

*UNDER CONSTRUCTION*.

Amc generates 32 flavors of linked lists, almost as many as Heinz for ketchup.
Linked lists can be:

Singly or doubly linked (as indicated by letter s or d in the prefix)
Zero-terminated or circular (as indicated by letter z or c in the prefix)
Fifo or lifo (as indicated by presence of letter l in the prefix, l for lifo)
With or without count
With or without tail pointer

Circular linked lists are often used with steps, because it is convenient to call RotateFirst
to both grab an element off the head of the list, and move this element to the back.

### Opt: Optional last field in variable-length struct

*UNDER CONSTRUCTION*.

Opt corresponds to 0 or 1 values at the end of a struct. This type is frequently used by protocols
to specify optional payload. Amc provides functions that can validate and access the payload.

### Pmask: Presence mask

*UNDER CONSTRUCTION*.

Amc provides convenient presence mask support. If any field in a ctype is marked Pmask,
then amc allocates 1 bit in the pmask for each field in the struct. The bits are initially zero.
Amc then generates a Set, PresentQ, and SetPresent functions for each field, unless one already exists.
Whenever the Set function is called on a field, the corresponding pmask bit is set to 1.
When reading from a string, the pmask bits are populated for each scanned attribute.
When printing, fields with zero pmask bit are not printed.

Regardless of pmask status, all fields of a struct are initialized to default values upon construction,
so there is no speed to be gained from using pmask. Access to fields is not protected in any way --
they can be accessed as if the pmask didn't exist. The generated PresentQ function can be used to check
if a field was previously Set.

### Ptr

*UNDER CONSTRUCTION*.

Ptr is a cross-reference type used when there can be 0 or 1 child records for each parent record.

### Ptrary

*UNDER CONSTRUCTION*.

Tary of Ptrs.

### RegxSql:

*UNDER CONSTRUCTION*.

When using this field type, amc ignores the field arg and inserts an algo_lib.Regx into
the parent structure. The expression intended to match the primary key of the target type.
This reftype is very useful in command line arguments.

### Smallstr

*UNDER CONSTRUCTION*.

Smallstr is a fixed-length character field. Memory is reserved inline in the parent struct.
Strings can be length-suffixed (Rpascal), left-padded or right-padded.
For padded strings, the string value is calculated by discarding the pad characters from the edge.
Any smallstr is castable to strptr.

### Thash: hash table

*UNDER CONSTRUCTION*.

Hash table, implemented as a Tary of pointers.
Collisions are implemented as a singly linked list. Hash tables grow automatically.
Whenever the number of entries in the hash table is greater
than the number of buckets (pointers), the hash table size is doubled.
The size of the array of pointers is always a power of 2. This means that a hash function has
to be good. Amc can generate hash functions for any ctype, typically CRC32 is used.

Hash tables can be unique of non-unique. A unique hash table rejects insertions of duplicate keys.
A non-unique hash table allows them.

For records that have only one hash access path defined for them, amc generates a GetOrCreate function
which is a convenient way to force creation of an element when you know its key.

### Upptr:

*UNDER CONSTRUCTION*.

Upptr is a pointer to a pre-existing record, usually non-null.
Upptr is a reference. By contrast, Ptr is a cross-reference.
I.e. a Ptr becomes non-null when another record starts pointing at the parent of the Ptr.
Upptr becomes non-null when a lookup in an index is performed.
amc_vis will complain if there are circular dependencies implied by Upptr (i.e. A has Upptr to B,
B has Upptr to A). Circular dependencies between Ptr fields are OK.

### Varlen: variable-length tail portion of a struct

*UNDER CONSTRUCTION*.

Varlen corresponds to a (possibly empty) array of fixed-size records appended to the end of a struct.
the parent ctype must have a length field defined. varlen fields cane be read from a string or
iterated over. This type is frequently used by wire protocols to specify a repeated section of a message.

