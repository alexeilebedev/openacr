## lib_json - Full json support library


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

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
// Get node value as u32, EXCEPTION on any error
// true/false is converted to 0/1
// 
// PARENT    node to start from
// PATH      dot-separated list of field keys
u32 lib_json::u32_Get(lib_json::FNode* parent, strptr path) 
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

### Sources
<a href="#sources"></a>
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

### Inputs
<a href="#inputs"></a>
`lib_json` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

