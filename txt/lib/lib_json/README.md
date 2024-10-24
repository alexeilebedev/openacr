## lib_json - Full json support library


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Ctypes](#ctypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_json.FDb - In-memory database for lib_json
<a href="#lib_json-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lpool|u8|Lpool||private memory pool|
|_db|lib_json.FDb|Global|
|node|lib_json.FNode|Tpool|
|ind_objfld|lib_json.FNode|Thash|
|JsonNumChar|algo.Charset|Charset|
|JsonFirstNumChar|algo.Charset|Charset|

#### lib_json.FNode - 
<a href="#lib_json-fnode"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|p_parent|lib_json.FNode|Upptr||Parent node, may be NULL|
|c_child|lib_json.FNode|Ptrary||Child node(s)|
|type|u32|Val|
|value|algo.cstring|Val|
|fldkey|lib_json.FldKey|Val|

#### lib_json.FParser - 
<a href="#lib_json-fparser"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|buf|algo.strptr|Val|
|ind|i32|Val|
|node|lib_json.FNode|Ptr|
|root_node|lib_json.FNode|Ptr|
|state|u32|Val|
|need_comma|bool|Val|
|have_comma|bool|Val|
|need_colon|bool|Val|
|err_info|algo.cstring|Val|
|err_offset|i32|Val|
|offset|i32|Val|
|uesc_value|u32|Val|
|uesc_need|u8|Val|
|value|algo.cstring|Val|

#### lib_json.FldKey - 
<a href="#lib_json-fldkey"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|object|lib_json.FNode|Ptr|
|field|algo.strptr|Val|

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// Note: this detects duplicate field names.
inline bool NodeParentingRulesOkQ(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr value) 
```

```c++
// Create new node
// enforce hierarchy rules
lib_json::FNode &lib_json::NewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr field) 
```

```c++
// PARENT    parent node or NULL
// TYPE      node type
// VALUE     node value where applicable (only for field, string, number)
lib_json::FNode &lib_json::NewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type) 
```

```c++
lib_json::FNode &lib_json::NewFieldNode(lib_json::FNode *parent, strptr field) 
```

```c++
lib_json::FNode &lib_json::NewObjectNode(lib_json::FNode *parent, strptr field = strptr()) 
```

```c++
lib_json::FNode &lib_json::NewArrayNode(lib_json::FNode *parent, strptr field = strptr()) 
```

```c++
lib_json::FNode &lib_json::NewStringNode(lib_json::FNode *parent, strptr field = strptr(), strptr value = strptr()) 
```

```c++
lib_json::FNode &lib_json::NewNumberNode(lib_json::FNode *parent, strptr field = strptr(), strptr value = strptr("0")) 
```

```c++
inline bool ScalarNodeQ(lib_json::FNode &node) 
```

```c++
// Parses JSON text into tree structure according to ECMA-404
// can be invoked repeadetly with new data portions (can be used directly as read hook)
// 
// PARSER    parser handle
// BUF       input buffer to process
// non-zero-length - parse new data portion
// zero-length     - to finalize and check completeness
// RETURN    true for OK, false for parse error
// 
// after finalization, root node pointer is available at: parser.root_node
// whole tree is automatically deleted upon parser destruction,
// to detach tree from parser do parser.root_node=NULL
bool lib_json::JsonParse(lib_json::FParser &parser, strptr buf) 
```

```c++
// AMC cleanup function - automatically delete parsed JSON tree
void lib_json::root_node_Cleanup(lib_json::FParser& parent) 
```

```c++
// encode json string
// "The representation of strings is similar to conventions used in the C
// family of programming languages.  A string begins and ends with
// quotation marks.  All Unicode characters may be placed within the
// quotation marks except for the characters that must be escaped:
// quotation mark, reverse solidus, and the control characters (U+0000
// through U+001F)."
// -- this says that solidus need not be escaped when printing -- only when parsing!
void lib_json::JsonSerializeString(algo::strptr str, algo::cstring &lhs) 
```

```c++
// Serialize to string
// Serialize to JSON tree to text
// NODE      root node to start from
// LHS       target string
// PRETTY    whether or not pretty-format
// INDENT    level of indenting (for pretty-formatting)
void lib_json::JsonSerialize(lib_json::FNode* node, cstring &lhs, bool pretty, u32 indent) 
```

```c++
void lib_json::JsonSerialize(lib_json::FNode* node, cstring &lhs, bool pretty) 
```

```c++
void lib_json::JsonSerialize(lib_json::FNode* node, cstring &lhs) 
```

```c++
// Find node in object chain
// PARENT    node to start from
// PATH      dot-separated list of field keys
// e.g. path abc.def.ghi and parent pointing to {"abc:{"def":{"ghi":value}}}
// yields value node
lib_json::FNode* lib_json::node_Find(lib_json::FNode* parent, strptr path) 
```

```c++
// Get node value as string
// Empty string is returned in case of any error or null value
// true/false is converted to 0/1
// 
// PARENT    node to start from
// PATH      dot-separated list of field keys
strptr lib_json::strptr_Get(lib_json::FNode* parent, strptr path) 
```

```c++
// Get array node, NULL in case of any error
// 
// PARENT    node to start from
// PATH      dot-separated list of field keys
lib_json::FNode* lib_json::node_GetArray(lib_json::FNode* parent, strptr path) 
```

```c++
// Get node value as u32
// If the path is not found, or the value is malformatted, DFLT is returned.
// true/false is converted to 0/1
// 
// PARENT    node to start from
// PATH      dot-separated list of field keys
u32 lib_json::u32_Get(lib_json::FNode* parent, strptr path, int dflt = 0) 
```

```c++
// 
// predefined conversion functions
// 
lib_json::FNode *u64_FmtJson(u64 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *u32_FmtJson(u32 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *u16_FmtJson(u16 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *u8_FmtJson(u8 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *i64_FmtJson(i64 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *i32_FmtJson(i32 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *i16_FmtJson(i16 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *i8_FmtJson(i8 value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *double_FmtJson(double value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *float_FmtJson(float value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *bool_FmtJson(bool value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *char_FmtJson(char value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *algo::strptr_FmtJson(const algo::strptr value, lib_json::FNode *parent) 
```

```c++
lib_json::FNode *algo::cstring_FmtJson(algo::cstring &value, lib_json::FNode *parent) 
```

```c++
lib_json::FldKey lib_json::fldkey_Get(lib_json::FNode &node) 
```

```c++
// Check for parse error
// 
// PARSER    parser handle
// RETURN    true for OK, false for parse error
inline bool lib_json::JsonParseOkQ(lib_json::FParser &parser) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_json` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_json_gen.cpp](/cpp/gen/lib_json_gen.cpp)||
|[cpp/lib/lib_json.cpp](/cpp/lib/lib_json.cpp)||
|[include/gen/lib_json_gen.h](/include/gen/lib_json_gen.h)||
|[include/gen/lib_json_gen.inl.h](/include/gen/lib_json_gen.inl.h)||
|[include/lib_json.h](/include/lib_json.h)||
|[include/lib_json.inl.h](/include/lib_json.inl.h)||

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
`lib_json` generated code creates the tables below.
All allocations are done through global `lib_json::_db` [lib_json.FDb](#lib_json-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_json.FDb](#lib_json-fdb)||FDb._db (Global)|
|[lib_json.FldKey](#lib_json-fldkey)||
|[lib_json.FNode](#lib_json-fnode)||FDb.node (Tpool)|ind_objfld (Thash, hash field fldkey)|
||||FNode.p_parent (Upptr)|
||||FNode.c_child (Ptrary)|
||||FParser.node (Ptr)|
||||FParser.root_node (Ptr)|
||||FldKey.object (Ptr)|
|[lib_json.FParser](#lib_json-fparser)||

#### lib_json.FDb - In-memory database for lib_json
<a href="#lib_json-fdb"></a>

#### lib_json.FDb Fields
<a href="#lib_json-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_json.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftypes.md#lpool)||private memory pool|
|lib_json.FDb._db|[lib_json.FDb](/txt/lib/lib_json/README.md#lib_json-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|lib_json.FDb.node|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_json.FDb.ind_objfld|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_json.FDb.JsonNumChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|lib_json.FDb.JsonFirstNumChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_json_gen.h](/include/gen/lib_json_gen.h)
```
struct FDb { // lib_json.FDb: In-memory database for lib_json
    lpool_Lpblock*      lpool_free[36];             // Lpool levels
    u64                 node_blocksize;             // # bytes per block
    lib_json::FNode*    node_free;                  //
    lib_json::FNode**   ind_objfld_buckets_elems;   // pointer to bucket array
    i32                 ind_objfld_buckets_n;       // number of elements in bucket array
    i32                 ind_objfld_n;               // number of elements in the hash table
    lib_json::trace     trace;                      //
};
```

#### lib_json.FldKey - 
<a href="#lib_json-fldkey"></a>

#### lib_json.FldKey Fields
<a href="#lib_json-fldkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_json.FldKey.object|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_json.FldKey.field|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FldKey
<a href="#struct-fldkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_json_gen.h](/include/gen/lib_json_gen.h)
```
struct FldKey { // lib_json.FldKey
    lib_json::FNode*   object;   // optional pointer
    algo::strptr       field;    //
    // func:lib_json.FldKey..EqOp
    inline bool          operator ==(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..NeOp
    inline bool          operator !=(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..LtOp
    inline bool          operator <(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..GtOp
    inline bool          operator >(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..LeOp
    inline bool          operator <=(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..GeOp
    inline bool          operator >=(const lib_json::FldKey &rhs) const __attribute__((nothrow));
    // func:lib_json.FldKey..Ctor
    inline               FldKey() __attribute__((nothrow));
    // func:lib_json.FldKey..FieldwiseCtor
    explicit inline               FldKey(lib_json::FNode* in_object, algo::strptr in_field) __attribute__((nothrow));
};
```

#### lib_json.FNode - 
<a href="#lib_json-fnode"></a>

#### lib_json.FNode Fields
<a href="#lib_json-fnode-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_json.FNode.p_parent|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Parent node, may be NULL|
|lib_json.FNode.c_child|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Child node(s)|
|lib_json.FNode.type|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FNode.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FNode.fldkey|[lib_json.FldKey](/txt/lib/lib_json/README.md#lib_json-fldkey)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FNode
<a href="#struct-fnode"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_json_gen.h](/include/gen/lib_json_gen.h)
```
struct FNode { // lib_json.FNode
    lib_json::FNode*    node_next;             // Pointer to next free element int tpool
    lib_json::FNode*    ind_objfld_next;       // hash next
    lib_json::FNode*    p_parent;              // reference to parent row
    lib_json::FNode**   c_child_elems;         // array of pointers
    u32                 c_child_n;             // array of pointers
    u32                 c_child_max;           // capacity of allocated array
    u32                 type;                  //   0
    algo::cstring       value;                 //
    bool                node_c_child_in_ary;   //   false  membership flag
    // reftype Ptrary of lib_json.FNode.c_child prohibits copy
    // func:lib_json.FNode..AssignOp
    inline lib_json::FNode& operator =(const lib_json::FNode &rhs) = delete;
    // reftype Ptrary of lib_json.FNode.c_child prohibits copy
    // func:lib_json.FNode..CopyCtor
    inline               FNode(const lib_json::FNode &rhs) = delete;
private:
    // func:lib_json.FNode..Ctor
    inline               FNode() __attribute__((nothrow));
    // func:lib_json.FNode..Dtor
    inline               ~FNode() __attribute__((nothrow));
    friend lib_json::FNode&     node_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_json::FNode*     node_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 node_Delete(lib_json::FNode &row) __attribute__((nothrow));
};
```

#### lib_json.FParser - 
<a href="#lib_json-fparser"></a>

#### lib_json.FParser Fields
<a href="#lib_json-fparser-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_json.FParser.buf|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.ind|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.node|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_json.FParser.root_node|[lib_json.FNode](/txt/lib/lib_json/README.md#lib_json-fnode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_json.FParser.state|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.need_comma|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.have_comma|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.need_colon|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.err_info|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.err_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.uesc_value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.uesc_need|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_json.FParser.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FParser
<a href="#struct-fparser"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_json_gen.h](/include/gen/lib_json_gen.h)
```
struct FParser { // lib_json.FParser
    algo::strptr       buf;          //
    i32                ind;          //   0
    lib_json::FNode*   node;         // optional pointer
    lib_json::FNode*   root_node;    // optional pointer
    u32                state;        //   0
    bool               need_comma;   //   false
    bool               have_comma;   //   false
    bool               need_colon;   //   false
    algo::cstring      err_info;     //
    i32                err_offset;   //   0
    i32                offset;       //   0
    u32                uesc_value;   //   0
    u8                 uesc_need;    //   0
    algo::cstring      value;        //
    // func:lib_json.FParser..Ctor
    inline               FParser() __attribute__((nothrow));
    // func:lib_json.FParser..Dtor
    inline               ~FParser() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

