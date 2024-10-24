## acr_in - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)
* [Tests](#tests)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [acr_in - ACR Input - compute set of ssimfiles or tuples used by a specific target](/txt/exe/acr_in/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_in/acr_in.cpp](/cpp/acr_in/acr_in.cpp)||
|[cpp/acr_in/data.cpp](/cpp/acr_in/data.cpp)||
|[cpp/gen/acr_in_gen.cpp](/cpp/gen/acr_in_gen.cpp)||
|[include/acr_in.h](/include/acr_in.h)||
|[include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)||
|[include/gen/acr_in_gen.inl.h](/include/gen/acr_in_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`acr_in` generated code creates the tables below.
All allocations are done through global `acr_in::_db` [acr_in.FDb](#acr_in-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[acr_in.FCtype](#acr_in-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|zd_todo (Llist)|zd_related (Llist)|
||||FCtype.c_ctype (Ptrary)|
||||FField.p_arg (Upptr)|
||||FField.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
||||FTuple.p_ctype (Upptr)|
|[acr_in.FDb](#acr_in-fdb)||FDb._db (Global)|
|[acr_in.FDispsig](#acr_in-fdispsig)|[dmmeta.dispsig](/txt/ssimdb/dmmeta/dispsig.md)|FDb.dispsig (Lary)|dispsig (Lary, by rowid)|ind_dispsig (Thash, hash field dispsig)|
|[acr_in.FField](#acr_in-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||FCtype.c_field (Ptrary)|
||||FFinput.p_field (Upptr)|
|[acr_in.FFinput](#acr_in-ffinput)|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|FDb.finput (Lary)|finput (Lary, by rowid)|ind_finput (Thash, hash field field)|
|[acr_in.FNs](#acr_in-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
||||FCtype.p_ns (Upptr)|
||||FDispsig.p_ns (Upptr)|
||||FFinput.p_ns (Upptr)|
||||FNsssimfile.p_ns (Upptr)|
||||FTarget.p_ns (Upptr)|
|[acr_in.FNsssimfile](#acr_in-fnsssimfile)||FDb.nsssimfile (Lary)|nsssimfile (Lary, by rowid)|ind_nsssimfile (Thash, hash field nsssimfile)|
||||FNs.zd_nsssimfile_ns (Llist)|
||||FSsimfile.zd_nsssimfile_ssimfile (Llist)|
|[acr_in.FSsimfile](#acr_in-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|zd_ssimfile (Llist)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
||||FNsssimfile.p_ssimfile (Upptr)|
|[acr_in.FSubstr](#acr_in-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|
|[acr_in.FTargdep](#acr_in-ftargdep)|[dev.targdep](/txt/ssimdb/dev/targdep.md)|FDb.targdep (Lary)|targdep (Lary, by rowid)|
||||FTarget.c_targdep (Ptrary)|
||||FTarget.c_targdep_child (Ptrary)|
|[acr_in.FTarget](#acr_in-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|zd_targ_visit (Llist)|
||||FNs.c_target (Ptr)|
||||FTargdep.p_parent (Upptr)|
|[acr_in.FTuple](#acr_in-ftuple)||FDb.tuple (Lary)|tuple (Lary, by rowid)|ind_tuple (Thash, hash field key)|zd_select (Llist)|zd_deselect (Llist)|
||||FTuple.c_child (Ptrary)|
||||FTuple.c_parent (Ptrary)|

#### acr_in.FCtype - Struct
<a href="#acr_in-fctype"></a>

#### acr_in.FCtype Fields
<a href="#acr_in-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FCtype.msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|acr_in.FCtype.c_ssimfile|[acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_in.FCtype.related|bool|[Val](/txt/exe/amc/reftypes.md#val)||Matches 'related' regex|
|acr_in.FCtype.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FCtype.visit|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FCtype.c_field|[acr_in.FField](/txt/exe/acr_in/internals.md#acr_in-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr_in.FCtype.c_ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||children|
|acr_in.FCtype.parent_of_finput|bool|[Val](/txt/exe/amc/reftypes.md#val)||Ctype required by one of the namespaces|
|acr_in.FCtype.p_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``acr_in.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FCtype { // acr_in.FCtype
    algo::Smallstr100    ctype;              // Identifier. must be ns.typename
    algo::Comment        comment;            //
    acr_in::FSsimfile*   c_ssimfile;         // optional pointer
    bool                 related;            //   false  Matches 'related' regex
    bool                 select;             //   false
    bool                 visit;              //   false
    acr_in::FField**     c_field_elems;      // array of pointers
    u32                  c_field_n;          // array of pointers
    u32                  c_field_max;        // capacity of allocated array
    acr_in::FCtype**     c_ctype_elems;      // array of pointers
    u32                  c_ctype_n;          // array of pointers
    u32                  c_ctype_max;        // capacity of allocated array
    bool                 parent_of_finput;   //   false  Ctype required by one of the namespaces
    acr_in::FNs*         p_ns;               // reference to parent row
    acr_in::FCtype*      ind_ctype_next;     // hash next
    acr_in::FCtype*      zd_todo_next;       // zslist link; -1 means not-in-list
    acr_in::FCtype*      zd_todo_prev;       // previous element
    acr_in::FCtype*      zd_related_next;    // zslist link; -1 means not-in-list
    acr_in::FCtype*      zd_related_prev;    // previous element
    // x-reference on acr_in.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of acr_in.FCtype.c_field prohibits copy
    // reftype Ptrary of acr_in.FCtype.c_ctype prohibits copy
    // x-reference on acr_in.FCtype.p_ns prevents copy
    // func:acr_in.FCtype..AssignOp
    acr_in::FCtype&      operator =(const acr_in::FCtype &rhs) = delete;
    // x-reference on acr_in.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of acr_in.FCtype.c_field prohibits copy
    // reftype Ptrary of acr_in.FCtype.c_ctype prohibits copy
    // x-reference on acr_in.FCtype.p_ns prevents copy
    // func:acr_in.FCtype..CopyCtor
    FCtype(const acr_in::FCtype &rhs) = delete;
private:
    // func:acr_in.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:acr_in.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend acr_in::FCtype&      ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FCtype*      ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FDb - In-memory database for acr_in
<a href="#acr_in-fdb"></a>

#### acr_in.FDb Fields
<a href="#acr_in-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FDb.tuple|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_tuple|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb._db|[acr_in.FDb](/txt/exe/acr_in/internals.md#acr_in-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|acr_in.FDb.cmdline|[command.acr_in](/txt/protocol/command/README.md#command-acr_in)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FDb.finput|[acr_in.FFinput](/txt/exe/acr_in/internals.md#acr_in-ffinput)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.field|[acr_in.FField](/txt/exe/acr_in/internals.md#acr_in-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_field|[acr_in.FField](/txt/exe/acr_in/internals.md#acr_in-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.ssimfile|[acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.zd_todo|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of ctypes that use us|
|acr_in.FDb.zd_ssimfile|[acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)|[Llist](/txt/exe/amc/reftypes.md#llist)||Selected files|
|acr_in.FDb.substr|[acr_in.FSubstr](/txt/exe/acr_in/internals.md#acr_in-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.dispsig|[acr_in.FDispsig](/txt/exe/acr_in/internals.md#acr_in-fdispsig)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_dispsig|[acr_in.FDispsig](/txt/exe/acr_in/internals.md#acr_in-fdispsig)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.zd_select|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr_in.FDb.zd_deselect|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr_in.FDb.zd_related|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr_in.FDb.ind_finput|[acr_in.FFinput](/txt/exe/acr_in/internals.md#acr_in-ffinput)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.target|[acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_target|[acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.targdep|[acr_in.FTargdep](/txt/exe/acr_in/internals.md#acr_in-ftargdep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.zd_targ_visit|[acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr_in.FDb.nsssimfile|[acr_in.FNsssimfile](/txt/exe/acr_in/internals.md#acr_in-fnsssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_in.FDb.ind_nsssimfile|[acr_in.FNsssimfile](/txt/exe/acr_in/internals.md#acr_in-fnsssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_in.FDb.ind_ssimfile|[acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FDb { // acr_in.FDb: In-memory database for acr_in
    acr_in::FTuple*         tuple_lary[32];                 // level array
    i32                     tuple_n;                        // number of elements in array
    acr_in::FTuple**        ind_tuple_buckets_elems;        // pointer to bucket array
    i32                     ind_tuple_buckets_n;            // number of elements in bucket array
    i32                     ind_tuple_n;                    // number of elements in the hash table
    command::acr_in         cmdline;                        //
    acr_in::FFinput*        finput_lary[32];                // level array
    i32                     finput_n;                       // number of elements in array
    acr_in::FField*         field_lary[32];                 // level array
    i32                     field_n;                        // number of elements in array
    acr_in::FField**        ind_field_buckets_elems;        // pointer to bucket array
    i32                     ind_field_buckets_n;            // number of elements in bucket array
    i32                     ind_field_n;                    // number of elements in the hash table
    acr_in::FCtype*         ctype_lary[32];                 // level array
    i32                     ctype_n;                        // number of elements in array
    acr_in::FCtype**        ind_ctype_buckets_elems;        // pointer to bucket array
    i32                     ind_ctype_buckets_n;            // number of elements in bucket array
    i32                     ind_ctype_n;                    // number of elements in the hash table
    acr_in::FSsimfile*      ssimfile_lary[32];              // level array
    i32                     ssimfile_n;                     // number of elements in array
    acr_in::FNs*            ns_lary[32];                    // level array
    i32                     ns_n;                           // number of elements in array
    acr_in::FNs**           ind_ns_buckets_elems;           // pointer to bucket array
    i32                     ind_ns_buckets_n;               // number of elements in bucket array
    i32                     ind_ns_n;                       // number of elements in the hash table
    acr_in::FCtype*         zd_todo_head;                   // zero-terminated doubly linked list
    i32                     zd_todo_n;                      // zero-terminated doubly linked list
    acr_in::FCtype*         zd_todo_tail;                   // pointer to last element
    acr_in::FSsimfile*      zd_ssimfile_head;               // zero-terminated doubly linked list
    i32                     zd_ssimfile_n;                  // zero-terminated doubly linked list
    acr_in::FSsimfile*      zd_ssimfile_tail;               // pointer to last element
    acr_in::FSubstr*        substr_lary[32];                // level array
    i32                     substr_n;                       // number of elements in array
    acr_in::FDispsig*       dispsig_lary[32];               // level array
    i32                     dispsig_n;                      // number of elements in array
    acr_in::FDispsig**      ind_dispsig_buckets_elems;      // pointer to bucket array
    i32                     ind_dispsig_buckets_n;          // number of elements in bucket array
    i32                     ind_dispsig_n;                  // number of elements in the hash table
    acr_in::FTuple*         zd_select_head;                 // zero-terminated doubly linked list
    i32                     zd_select_n;                    // zero-terminated doubly linked list
    acr_in::FTuple*         zd_select_tail;                 // pointer to last element
    acr_in::FTuple*         zd_deselect_head;               // zero-terminated doubly linked list
    i32                     zd_deselect_n;                  // zero-terminated doubly linked list
    acr_in::FTuple*         zd_deselect_tail;               // pointer to last element
    acr_in::FCtype*         zd_related_head;                // zero-terminated doubly linked list
    i32                     zd_related_n;                   // zero-terminated doubly linked list
    acr_in::FCtype*         zd_related_tail;                // pointer to last element
    acr_in::FFinput**       ind_finput_buckets_elems;       // pointer to bucket array
    i32                     ind_finput_buckets_n;           // number of elements in bucket array
    i32                     ind_finput_n;                   // number of elements in the hash table
    acr_in::FTarget*        target_lary[32];                // level array
    i32                     target_n;                       // number of elements in array
    acr_in::FTarget**       ind_target_buckets_elems;       // pointer to bucket array
    i32                     ind_target_buckets_n;           // number of elements in bucket array
    i32                     ind_target_n;                   // number of elements in the hash table
    acr_in::FTargdep*       targdep_lary[32];               // level array
    i32                     targdep_n;                      // number of elements in array
    acr_in::FTarget*        zd_targ_visit_head;             // zero-terminated doubly linked list
    i32                     zd_targ_visit_n;                // zero-terminated doubly linked list
    acr_in::FTarget*        zd_targ_visit_tail;             // pointer to last element
    acr_in::FNsssimfile*    nsssimfile_lary[32];            // level array
    i32                     nsssimfile_n;                   // number of elements in array
    acr_in::FNsssimfile**   ind_nsssimfile_buckets_elems;   // pointer to bucket array
    i32                     ind_nsssimfile_buckets_n;       // number of elements in bucket array
    i32                     ind_nsssimfile_n;               // number of elements in the hash table
    acr_in::FSsimfile**     ind_ssimfile_buckets_elems;     // pointer to bucket array
    i32                     ind_ssimfile_buckets_n;         // number of elements in bucket array
    i32                     ind_ssimfile_n;                 // number of elements in the hash table
    acr_in::trace           trace;                          //
};
```

#### acr_in.FDispsig - Cryptographic signature of all dispatches
<a href="#acr_in-fdispsig"></a>

#### acr_in.FDispsig Fields
<a href="#acr_in-fdispsig-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FDispsig.msghdr|[dmmeta.Dispsig](/txt/ssimdb/dmmeta/dispsig.md)|[Base](/txt/ssimdb/dmmeta/dispsig.md)|||
|acr_in.FDispsig.p_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FDispsig
<a href="#struct-fdispsig"></a>
*Note:* field ``acr_in.FDispsig.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispsig](/txt/ssimdb/dmmeta/dispsig.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FDispsig { // acr_in.FDispsig
    acr_in::FDispsig*   ind_dispsig_next;   // hash next
    algo::Smallstr50    dispsig;            //
    algo::Sha1sig       signature;          //
    acr_in::FNs*        p_ns;               // reference to parent row
    // x-reference on acr_in.FDispsig.p_ns prevents copy
    // func:acr_in.FDispsig..AssignOp
    inline acr_in::FDispsig& operator =(const acr_in::FDispsig &rhs) = delete;
    // x-reference on acr_in.FDispsig.p_ns prevents copy
    // func:acr_in.FDispsig..CopyCtor
    inline               FDispsig(const acr_in::FDispsig &rhs) = delete;
private:
    // func:acr_in.FDispsig..Ctor
    inline               FDispsig() __attribute__((nothrow));
    // func:acr_in.FDispsig..Dtor
    inline               ~FDispsig() __attribute__((nothrow));
    friend acr_in::FDispsig&    dispsig_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FDispsig*    dispsig_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispsig_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FField - Specify field of a struct
<a href="#acr_in-ffield"></a>

#### acr_in.FField Fields
<a href="#acr_in-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|acr_in.FField.p_arg|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FField.p_ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FField.c_substr|[acr_in.FSubstr](/txt/exe/acr_in/internals.md#acr_in-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``acr_in.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FField { // acr_in.FField
    acr_in::FField*     ind_field_next;         // hash next
    algo::Smallstr100   field;                  // Primary key, as ctype.name
    algo::Smallstr100   arg;                    // Type of field
    algo::Smallstr50    reftype;                //   "Val"  Type constructor
    algo::CppExpr       dflt;                   // Default value (c++ expression)
    algo::Comment       comment;                //
    acr_in::FCtype*     p_arg;                  // reference to parent row
    acr_in::FCtype*     p_ctype;                // reference to parent row
    acr_in::FSubstr*    c_substr;               // optional pointer
    bool                ctype_c_field_in_ary;   //   false  membership flag
    // x-reference on acr_in.FField.p_arg prevents copy
    // x-reference on acr_in.FField.p_ctype prevents copy
    // x-reference on acr_in.FField.c_substr prevents copy
    // func:acr_in.FField..AssignOp
    acr_in::FField&      operator =(const acr_in::FField &rhs) = delete;
    // x-reference on acr_in.FField.p_arg prevents copy
    // x-reference on acr_in.FField.p_ctype prevents copy
    // x-reference on acr_in.FField.c_substr prevents copy
    // func:acr_in.FField..CopyCtor
    FField(const acr_in::FField &rhs) = delete;
private:
    // func:acr_in.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:acr_in.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend acr_in::FField&      field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FField*      field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FFinput - Describe input table of a program
<a href="#acr_in-ffinput"></a>

#### acr_in.FFinput Fields
<a href="#acr_in-ffinput-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FFinput.msghdr|[dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md)|[Base](/txt/ssimdb/dmmeta/finput.md)|||
|acr_in.FFinput.p_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FFinput.p_field|[acr_in.FField](/txt/exe/acr_in/internals.md#acr_in-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFinput
<a href="#struct-ffinput"></a>
*Note:* field ``acr_in.FFinput.msghdr`` has reftype ``base`` so the fields of [dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FFinput { // acr_in.FFinput
    acr_in::FFinput*    ind_finput_next;   // hash next
    algo::Smallstr100   field;             // Target field to read
    bool                extrn;             //   false  Call user-provided function
    bool                update;            //   false
    bool                strict;            //   true  Exist process if record contains error
    algo::Comment       comment;           //
    acr_in::FNs*        p_ns;              // reference to parent row
    acr_in::FField*     p_field;           // reference to parent row
    // x-reference on acr_in.FFinput.p_ns prevents copy
    // x-reference on acr_in.FFinput.p_field prevents copy
    // func:acr_in.FFinput..AssignOp
    inline acr_in::FFinput& operator =(const acr_in::FFinput &rhs) = delete;
    // x-reference on acr_in.FFinput.p_ns prevents copy
    // x-reference on acr_in.FFinput.p_field prevents copy
    // func:acr_in.FFinput..CopyCtor
    inline               FFinput(const acr_in::FFinput &rhs) = delete;
private:
    // func:acr_in.FFinput..Ctor
    inline               FFinput() __attribute__((nothrow));
    // func:acr_in.FFinput..Dtor
    inline               ~FFinput() __attribute__((nothrow));
    friend acr_in::FFinput&     finput_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FFinput*     finput_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 finput_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#acr_in-fns"></a>

#### acr_in.FNs Fields
<a href="#acr_in-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FNs.msghdr|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|acr_in.FNs.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FNs.c_target|[acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_in.FNs.zd_nsssimfile_ns|[acr_in.FNsssimfile](/txt/exe/acr_in/internals.md#acr_in-fnsssimfile)|[Llist](/txt/exe/amc/reftypes.md#llist)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``acr_in.FNs.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FNs { // acr_in.FNs
    acr_in::FNs*           ind_ns_next;             // hash next
    algo::Smallstr16       ns;                      // Namespace name (primary key)
    algo::Smallstr50       nstype;                  // Namespace type
    algo::Smallstr50       license;                 // Associated license
    algo::Comment          comment;                 //
    bool                   select;                  //   false
    acr_in::FTarget*       c_target;                // optional pointer
    acr_in::FNsssimfile*   zd_nsssimfile_ns_head;   // zero-terminated doubly linked list
    i32                    zd_nsssimfile_ns_n;      // zero-terminated doubly linked list
    acr_in::FNsssimfile*   zd_nsssimfile_ns_tail;   // pointer to last element
    // x-reference on acr_in.FNs.c_target prevents copy
    // reftype Llist of acr_in.FNs.zd_nsssimfile_ns prohibits copy
    // func:acr_in.FNs..AssignOp
    inline acr_in::FNs&  operator =(const acr_in::FNs &rhs) = delete;
    // x-reference on acr_in.FNs.c_target prevents copy
    // reftype Llist of acr_in.FNs.zd_nsssimfile_ns prohibits copy
    // func:acr_in.FNs..CopyCtor
    inline               FNs(const acr_in::FNs &rhs) = delete;
private:
    // func:acr_in.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:acr_in.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend acr_in::FNs&         ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FNs*         ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FNsssimfile - 
<a href="#acr_in-fnsssimfile"></a>

#### acr_in.FNsssimfile Fields
<a href="#acr_in-fnsssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FNsssimfile.nsssimfile|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*acr_in.FNsssimfile.ns (/RL)*|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*acr_in.FNsssimfile.ssimfile (/RR)*|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|acr_in.FNsssimfile.show|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FNsssimfile.p_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FNsssimfile.p_ssimfile|[acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FNsssimfile
<a href="#struct-fnsssimfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FNsssimfile { // acr_in.FNsssimfile
    acr_in::FNsssimfile*   ind_nsssimfile_next;           // hash next
    acr_in::FNsssimfile*   zd_nsssimfile_ns_next;         // zslist link; -1 means not-in-list
    acr_in::FNsssimfile*   zd_nsssimfile_ns_prev;         // previous element
    algo::Smallstr200      nsssimfile;                    //
    bool                   show;                          //   false
    acr_in::FNs*           p_ns;                          // reference to parent row
    acr_in::FSsimfile*     p_ssimfile;                    // reference to parent row
    acr_in::FNsssimfile*   zd_nsssimfile_ssimfile_next;   // zslist link; -1 means not-in-list
    acr_in::FNsssimfile*   zd_nsssimfile_ssimfile_prev;   // previous element
    // x-reference on acr_in.FNsssimfile.p_ns prevents copy
    // x-reference on acr_in.FNsssimfile.p_ssimfile prevents copy
    // func:acr_in.FNsssimfile..AssignOp
    inline acr_in::FNsssimfile& operator =(const acr_in::FNsssimfile &rhs) = delete;
    // x-reference on acr_in.FNsssimfile.p_ns prevents copy
    // x-reference on acr_in.FNsssimfile.p_ssimfile prevents copy
    // func:acr_in.FNsssimfile..CopyCtor
    inline               FNsssimfile(const acr_in::FNsssimfile &rhs) = delete;
private:
    // func:acr_in.FNsssimfile..Ctor
    inline               FNsssimfile() __attribute__((nothrow));
    // func:acr_in.FNsssimfile..Dtor
    inline               ~FNsssimfile() __attribute__((nothrow));
    friend acr_in::FNsssimfile& nsssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FNsssimfile* nsssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsssimfile_RemoveAll() __attribute__((nothrow));
    friend void                 nsssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FSsimfile - File with ssim tuples
<a href="#acr_in-fssimfile"></a>

#### acr_in.FSsimfile Fields
<a href="#acr_in-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FSsimfile.msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|acr_in.FSsimfile.p_ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FSsimfile.is_finput|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FSsimfile.zd_nsssimfile_ssimfile|[acr_in.FNsssimfile](/txt/exe/acr_in/internals.md#acr_in-fnsssimfile)|[Llist](/txt/exe/amc/reftypes.md#llist)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``acr_in.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FSsimfile { // acr_in.FSsimfile
    acr_in::FSsimfile*     zd_ssimfile_next;              // zslist link; -1 means not-in-list
    acr_in::FSsimfile*     zd_ssimfile_prev;              // previous element
    acr_in::FSsimfile*     ind_ssimfile_next;             // hash next
    algo::Smallstr50       ssimfile;                      //
    algo::Smallstr100      ctype;                         //
    acr_in::FCtype*        p_ctype;                       // reference to parent row
    bool                   is_finput;                     //   false
    acr_in::FNsssimfile*   zd_nsssimfile_ssimfile_head;   // zero-terminated doubly linked list
    i32                    zd_nsssimfile_ssimfile_n;      // zero-terminated doubly linked list
    acr_in::FNsssimfile*   zd_nsssimfile_ssimfile_tail;   // pointer to last element
    // x-reference on acr_in.FSsimfile.p_ctype prevents copy
    // reftype Llist of acr_in.FSsimfile.zd_nsssimfile_ssimfile prohibits copy
    // func:acr_in.FSsimfile..AssignOp
    inline acr_in::FSsimfile& operator =(const acr_in::FSsimfile &rhs) = delete;
    // x-reference on acr_in.FSsimfile.p_ctype prevents copy
    // reftype Llist of acr_in.FSsimfile.zd_nsssimfile_ssimfile prohibits copy
    // func:acr_in.FSsimfile..CopyCtor
    inline               FSsimfile(const acr_in::FSsimfile &rhs) = delete;
private:
    // func:acr_in.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:acr_in.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend acr_in::FSsimfile&   ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FSsimfile*   ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#acr_in-fsubstr"></a>

#### acr_in.FSubstr Fields
<a href="#acr_in-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FSubstr.msghdr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``acr_in.FSubstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FSubstr { // acr_in.FSubstr
    algo::Smallstr100   field;      //
    algo::CppExpr       expr;       //
    algo::Smallstr100   srcfield;   //
    // func:acr_in.FSubstr..AssignOp
    inline acr_in::FSubstr& operator =(const acr_in::FSubstr &rhs) = delete;
    // func:acr_in.FSubstr..CopyCtor
    inline               FSubstr(const acr_in::FSubstr &rhs) = delete;
private:
    // func:acr_in.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:acr_in.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend acr_in::FSubstr&     substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FSubstr*     substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FTargdep - Dependency between targets
<a href="#acr_in-ftargdep"></a>

#### acr_in.FTargdep Fields
<a href="#acr_in-ftargdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FTargdep.msghdr|[dev.Targdep](/txt/ssimdb/dev/targdep.md)|[Base](/txt/ssimdb/dev/targdep.md)|||
|acr_in.FTargdep.p_parent|[acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``acr_in.FTargdep.msghdr`` has reftype ``base`` so the fields of [dev.Targdep](/txt/ssimdb/dev/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FTargdep { // acr_in.FTargdep
    algo::Smallstr50   targdep;                         // Primary key - target.parent
    algo::Comment      comment;                         //
    acr_in::FTarget*   p_parent;                        // reference to parent row
    bool               target_c_targdep_in_ary;         //   false  membership flag
    bool               target_c_targdep_child_in_ary;   //   false  membership flag
    // x-reference on acr_in.FTargdep.p_parent prevents copy
    // func:acr_in.FTargdep..AssignOp
    inline acr_in::FTargdep& operator =(const acr_in::FTargdep &rhs) = delete;
    // x-reference on acr_in.FTargdep.p_parent prevents copy
    // func:acr_in.FTargdep..CopyCtor
    inline               FTargdep(const acr_in::FTargdep &rhs) = delete;
private:
    // func:acr_in.FTargdep..Ctor
    inline               FTargdep() __attribute__((nothrow));
    // func:acr_in.FTargdep..Dtor
    inline               ~FTargdep() __attribute__((nothrow));
    friend acr_in::FTargdep&    targdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FTargdep*    targdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targdep_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FTarget - Build target
<a href="#acr_in-ftarget"></a>

#### acr_in.FTarget Fields
<a href="#acr_in-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FTarget.msghdr|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|acr_in.FTarget.c_targdep|[acr_in.FTargdep](/txt/exe/acr_in/internals.md#acr_in-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of targdeps where we are the child|
|acr_in.FTarget.c_targdep_child|[acr_in.FTargdep](/txt/exe/acr_in/internals.md#acr_in-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of targdeps where we are the parent|
|acr_in.FTarget.p_ns|[acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``acr_in.FTarget.msghdr`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FTarget { // acr_in.FTarget
    acr_in::FTarget*     ind_target_next;         // hash next
    acr_in::FTarget*     zd_targ_visit_next;      // zslist link; -1 means not-in-list
    acr_in::FTarget*     zd_targ_visit_prev;      // previous element
    algo::Smallstr16     target;                  // Primary key - name of target
    acr_in::FTargdep**   c_targdep_elems;         // array of pointers
    u32                  c_targdep_n;             // array of pointers
    u32                  c_targdep_max;           // capacity of allocated array
    acr_in::FTargdep**   c_targdep_child_elems;   // array of pointers
    u32                  c_targdep_child_n;       // array of pointers
    u32                  c_targdep_child_max;     // capacity of allocated array
    acr_in::FNs*         p_ns;                    // reference to parent row
    // reftype Ptrary of acr_in.FTarget.c_targdep prohibits copy
    // reftype Ptrary of acr_in.FTarget.c_targdep_child prohibits copy
    // x-reference on acr_in.FTarget.p_ns prevents copy
    // func:acr_in.FTarget..AssignOp
    inline acr_in::FTarget& operator =(const acr_in::FTarget &rhs) = delete;
    // reftype Ptrary of acr_in.FTarget.c_targdep prohibits copy
    // reftype Ptrary of acr_in.FTarget.c_targdep_child prohibits copy
    // x-reference on acr_in.FTarget.p_ns prevents copy
    // func:acr_in.FTarget..CopyCtor
    inline               FTarget(const acr_in::FTarget &rhs) = delete;
private:
    // func:acr_in.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:acr_in.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend acr_in::FTarget&     target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FTarget*     target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### acr_in.FTuple - 
<a href="#acr_in-ftuple"></a>

#### acr_in.FTuple Fields
<a href="#acr_in-ftuple-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_in.FTuple.key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FTuple.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_in.FTuple.c_child|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||children|
|acr_in.FTuple.p_ctype|[acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_in.FTuple.c_parent|[acr_in.FTuple](/txt/exe/acr_in/internals.md#acr_in-ftuple)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||parents|
|acr_in.FTuple.deselect_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTuple
<a href="#struct-ftuple"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)
```
struct FTuple { // acr_in.FTuple
    acr_in::FTuple*    ind_tuple_next;     // hash next
    acr_in::FTuple*    zd_select_next;     // zslist link; -1 means not-in-list
    acr_in::FTuple*    zd_select_prev;     // previous element
    acr_in::FTuple*    zd_deselect_next;   // zslist link; -1 means not-in-list
    acr_in::FTuple*    zd_deselect_prev;   // previous element
    algo::cstring      key;                //
    algo::cstring      str;                //
    acr_in::FTuple**   c_child_elems;      // array of pointers
    u32                c_child_n;          // array of pointers
    u32                c_child_max;        // capacity of allocated array
    acr_in::FCtype*    p_ctype;            // reference to parent row
    acr_in::FTuple**   c_parent_elems;     // array of pointers
    u32                c_parent_n;         // array of pointers
    u32                c_parent_max;       // capacity of allocated array
    bool               deselect_visited;   //   false
    // reftype Ptrary of acr_in.FTuple.c_child prohibits copy
    // reftype Ptrary of acr_in.FTuple.c_parent prohibits copy
    // func:acr_in.FTuple..AssignOp
    inline acr_in::FTuple& operator =(const acr_in::FTuple &rhs) = delete;
    // reftype Ptrary of acr_in.FTuple.c_child prohibits copy
    // reftype Ptrary of acr_in.FTuple.c_parent prohibits copy
    // func:acr_in.FTuple..CopyCtor
    inline               FTuple(const acr_in::FTuple &rhs) = delete;
private:
    // func:acr_in.FTuple..Ctor
    inline               FTuple() __attribute__((nothrow));
    // func:acr_in.FTuple..Dtor
    inline               ~FTuple() __attribute__((nothrow));
    friend acr_in::FTuple&      tuple_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_in::FTuple*      tuple_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tuple_RemoveAll() __attribute__((nothrow));
    friend void                 tuple_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

### Tests
<a href="#tests"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Tests -->
The following component tests are defined for `acr_in`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[acr_in.Reverse](/test/atf_comp/acr_in.Reverse)||
|[acr_in.Simple](/test/atf_comp/acr_in.Simple)||
|[acr_in.Tree](/test/atf_comp/acr_in.Tree)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Tests -->

