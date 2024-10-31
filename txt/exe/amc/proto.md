## amc Protocols
<a href="#amc-protocols"></a>

Amc has support for protocols. A protocol is a set of struct layout definitions and
some enumerated types. There is no associated in-memory database.
Mainly we are concerned with controlling exact byte layout of messages,
plus conversion of enumerated types (and entire structs)
to and from wire formats.

To create a new protocol, insert these records and run amc:

```
$ acr -insert -write
   dmmeta.ns  ns:prot1  nstype:protocol  comment:"Some protocol"
      dmmeta.nsproto  ns:prot1  comment:""
^D
$ amc
```

Typically the protocol version is part of its name.
You will need to manually add the resulting source files to git (`git add */gen/proto1_gen.*`)
And include them in some target so that they are compiled and linked in.
Typically all protocols are linked into `lib_prot`:

```
$ acr -insert -write
    dev.targsrc  targsrc:lib_prot/cpp/gen/prot1_gen.cpp        comment:""
    dev.targsrc  targsrc:lib_prot/include/gen/prot1_gen.h      comment:""
    dev.targsrc  targsrc:lib_prot/include/gen/prot1_gen.inl.h  comment:""
^D
$ amc
```

With an empty namespace, the only type amc automatically adds is called a `FieldId`.
This is a type you can ignore, but it's useful because all field names from that protocol
will be added to `FieldId` as enumerated types, achieving two things: giving each future field
in the namespace a c++ symbol, and adding an ability to read field names from strings.

An alternative is to use `acr_ed:`

```
$ acr_ed -create -target prot1 -nstype protocol -write
```

### Protocol Messages
<a href="#protocol-messages"></a>

Messages in a protocol are represented as ctypes that share a header (which is also a ctype), and a related to each
other via `typefld` and `msgtype` records. The header ctype contains
a type field (sometimes known as `discriminant field`), which is a field associated with `typefld` record,
and optionally a length field, which is a field associated with a `lenfld` record..
Protocols with type and value are known as TV protocols, and ones with type-length-value
are known as TLV protocols.

To define a message header, define a ctype and annotate its fields with `typefld` and lenfld`
records. When defining a message header, we will need to define at least one message to go with it.

```
$ acr -insert -write
dmmeta.ctype  ctype:prot1.MsgHeader  comment:""
  dmmeta.cpptype  ctype:prot1.MsgHeader  ctor:Y  dtor:N  cheap_copy:N
  dmmeta.pack  ctype:prot1.MsgHeader  comment:""
    dmmeta.field  field:prot1.MsgHeader.type  acr.rowid:982     arg:u8  reftype:Val  dflt:""  comment:""
      dmmeta.typefld  field:prot1.MsgHeader.type  comment:""
    dmmeta.field  field:prot1.MsgHeader.length  acr.rowid:983     arg:u8  reftype:Val  dflt:""  comment:""
      dmmeta.lenfld  field:prot1.MsgHeader.length  extra:0
    dmmeta.cfmt  cfmt:prot1.MsgHeader.String  printfmt:Tuple  read:Y  print:Y  sep:,  genop:Y  comment:""

dmmeta.ctype  ctype:prot1.HeartbeatMsg  comment:""
  dmmeta.cpptype  ctype:prot1.HeartbeatMsg  ctor:Y  dtor:N  cheap_copy:N
  dmmeta.msgtype  ctype:prot1.HeartbeatMsg  type:1
  dmmeta.pack  ctype:prot1.HeartbeatMsg  comment:""
  dmmeta.field  field:prot1.HeartbeatMsg.base  arg:prot1.MsgHeader  reftype:Base  dflt:""  comment:""
  dmmeta.cfmt  cfmt:prot1.HeartbeatMsg.String  printfmt:Tuple  read:Y  print:Y  sep:,  genop:Y  comment:""

dmmeta.ctype  ctype:prot1.LoginMsg  comment:""
  dmmeta.cpptype  ctype:prot1.LoginMsg  ctor:Y  dtor:N  cheap_copy:N
  dmmeta.msgtype  ctype:prot1.LoginMsg  type:2
  dmmeta.pack  ctype:prot1.LoginMsg  comment:""
  dmmeta.field  field:prot1.LoginMsg.base  arg:prot1.MsgHeader  reftype:Base  dflt:""  comment:""
  dmmeta.field  field:prot1.LoginMsg.username  arg:algo.RspaceStr16  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:prot1.LoginMsg.password  arg:algo.RspaceStr16  reftype:Val  dflt:""  comment:""
  dmmeta.cfmt  cfmt:prot1.LoginMsg.String  printfmt:Tuple  read:Y  print:Y  sep:,  genop:Y  comment:""
^D
```

* The `pack` record specifies that ctype `prot1.MsgHeader` should use byte-level packing (`#pragma pack`)
  It is also possible to require that all ctypes in a namespace be packed by setting `nsx.pack` attribute to true.
* Each `field` record specifies one field. Field with reftype `Base` is an instruction to copy
  all fields from the `arg` type to the type being defined.
* The `typefld` record indicates that the field is a type field. All ctypes using this ctype a base
  and having a `msgtype` record are collected together by amc and form a set.
* The `cpptype` record is a set of general hints about the type.
- `ctor:Y` asks amc to generate a fieldwise constructor for the type.
- `dtor:N` tells amc that no special destructor is associated either with the type of any of its fields.
  This an annotation that affects some code generation steps and can be ignored for now.
- `cheap_copy:N` tells amc to pass the specified type by const-reference. `cheap_copy:N` passes it by value
  whenever it appears as an argument to a function
* The `lenfld` record allocates determining the length of the actual message when looking at the message header.
  The value of this field, plus the value of `extra`, is the message length. This field is used for  validation and
  for skipping messages when scanning memory.
* The `cfmt` record asks amc to generate print/read functions for the type:
- suffix `.String` is string conversion (other methods are supported and can be ignored for now)
- `printfmt:Tuple` means to print the type as an ssim tuple
- `read:Y` asks amc to generate a `_ReadStrptrMaybe` function.
- `print:Y` asks amc to generate a `_Print` function.
- `sep:,` is used for non-Tuple print methods and can be ignored for now
- `genop:Y` generates associated `operator <<` to make printing simpler.

### Scanning memory
<a href="#scanning-memory"></a>

Another useful type `amc` automatically provides is the cursor.
User can iterate over a memory range using a `MsgHeader&` that automatically skips to the next message
and provides bounds checking.

```
memptr bytes;
ind_beg(prot1::MsgHeader_curs, msg, bytes) {
    // msg is a MsgHeader&
}ind_end;
```

The code for any generated message can be examined with `amc`:

```
$ amc prot1.HeartbeatMsg

// --- prot1.HeartbeatMsg
#pragma pack(push,1)

struct HeartbeatMsg { // prot1.HeartbeatMsg
    u8   type;     //   1
    u8   length;   //   sizeof(parent) + (0)
    HeartbeatMsg();
};
#pragma pack(pop)

inline prot1::HeartbeatMsg::HeartbeatMsg() {
    prot1::HeartbeatMsg_Init(*this);
}

report.amc  n_cppfile:0  n_cppline:14  n_ctype:1806  n_func:92  n_xref:717  n_filemod:0
```

### Dispatches
<a href="#dispatches"></a>

With `Dispatch`, you declare a set of ctypes and one or more associated actions.
Amc generates a switch statement (encased in a function) and performs one of these actions.
Here is an example:

```
dmmeta.dispatch  dispatch:prot1.In  unk:N  read:N  print:Y  haslen:Y  call:Y  strict:N  comment:""
dmmeta.dispatch_msg  dispatch_msg:prot1.In/prot1.HeartbeatMsg       comment:""
dmmeta.dispatch_msg  dispatch_msg:prot1.In/prot1.LoginMsg       comment:""
```

The `dispatch` record controls a generator of switch statements.
- `read` asks amc to generate a function that reads any of the specified messages from string.
- `print` asks amc to generate a function to print any of the specified messages to string.
- `call` asks amc to generate a `_Dispatch` function which calls one of N user-provided functions
 after checking input parameters. For the code above, this generates the following lines in the header
 `include/gen/prot1_gen.h`:
 
```
// User-implemented callback function for dispatch In
void                 In_HeartbeatMsg(prot1::HeartbeatMsg &msg, u32 msg_len);
// User-implemented callback function for dispatch In
void                 In_LoginMsg(prot1::LoginMsg &msg, u32 msg_len);
int                  InDispatch(prot1::MsgHeader& msg, u32 msg_len);
```

User must implement functions `In_HeartbeatMsg` and `In_LoginMsg`. The name of the dispatch (`prot1.In`)
effectively names the switch statement. If `strict:Y` is specified, the parameter `len` must match
message size exactly. Otherwise, it just has to be enough to fit the message.
The return value is the size of the dispatched message, of 0 if no function was called.

```
// --- prot1.In..DispatchRaw
int prot1::InDispatchRaw(prot1::InCase type, u8 *msg, u32 len) {
    int ret = 0;
    switch(type) {
    case 1: if (sizeof(prot1::HeartbeatMsg) <= len) {
            prot1::In_HeartbeatMsg((prot1::HeartbeatMsg&)*msg, len);
            ret = (int)sizeof(prot1::HeartbeatMsg);
        } break;
    case 2: if (sizeof(prot1::LoginMsg) <= len) {
            prot1::In_LoginMsg((prot1::LoginMsg&)*msg, len);
            ret = (int)sizeof(prot1::LoginMsg);
        } break;
    default:
        break;
    }
    return ret;
}

// --- prot1.In..Dispatch
int prot1::InDispatch(prot1::MsgHeader& msg, u32 msg_len) {
    return InDispatchRaw(prot1::InCase(msg.type), (u8*)&msg, msg_len);
}
```

- `unk` specifies whether the `default` branch will be generated.
- `haslen:N` omits `u32 len` parameter from each function, including the dispatch function.
  It is assumed that it has already been determined that the message fit its buffer space.

Each `dispatch` results in corresponding `Case` type (see above discussion for the default Case
type that covers all messages). So, with `read:Y`, the following code is generated:

```
+// --- prot1.In..ReadStrptr
+// Parse ascii representation of message into binary, appending new data to BUF.
+prot1::InCase prot1::In_ReadStrptr(algo::strptr str, algo::ByteAry &buf) {
...
```

When messages share a common header, use of this type may be unnecessary since one may just
look at `buf` as a `MsgHeader` and determine the type that was read. But `dispatch` is more generic
and can look at any set of messages, in which the resulting `Case` message describes what was read.

### Dispctx - Dispatch Context
<a href="#dispctx"></a>

`amc` can pass a reference to a `Dispatch Context` to each function during callback.
This is specified with `dispctx` record:

Example:
```
dmmeta.dispctx  dispatch:prot1.In                   ctype:prot1.Ctx     comment:""
```

The generated code is affected as follows. First, `prot1.Ctx` is added
as the first argument of `Dispatch`. It is passed to each callback function:

```
int                  OmegaDispatch(prot1.Ctx &ctx, omega::Header& msg, u32 msg_len);

// User-implemented callback function for dispatch Omega
void                 Omega_OrderAdd(prot1.Ctx &ctx, omega::OrderAdd &msg, u32 msg_len);
```

That this example notwithstanding, dispatches are never generated in the protocol's namespace,
because they belong with the application, not the protocol.

### Dispsig - Dispatch Signatures
<a href="#dispsig"></a>

`amc` can generate a cryptographic (SHA1) signature for any dispatch. The signature captures
the types of all fields of all the structs in the dispatch and is recursively computed. This can be
used to ensure that an application is never passed an incompatible binary input.

### Disptrace - tracing the number of cycles spent in a dispatch
<a href="#disptrace"></a>

```
dmmeta.disptrace dispatch:prot1.In cycles:Y
```

When `disptrace` is specified, amc adds one trace field to the namespace's `FDb.trace` record
for each mesage type in the dispatch. The field is incremented by one whenever the callback
for this message type is called. If `cycles:Y` is specifie, the total number of cycles spent
in each callback is calculated as well (and stored in another trace variable in the same location).
A trace variable is simply a u64 field added to ctype `ns.trace`. A single copy of `ns.trace`
is kept in the global database record.

### The `Case` dispatch
<a href="#the--case--dispatch"></a>

The `Case` type is a special case of a `Dispatch`. 
When generating code for a protocol, `amc` automatically generates a `MsgHeaderMsgsCase` type
(it's the message type plus the suffix `MsgsCase`), which has field `value` and one `fconst` for each
found message type. The purpose of this type is to provide recursive reading and printing of messages
using the following two functions:

```
// Print message to STR. If message is too short for MSG_LEN, print nothing.
// MSG.LENGTH must have already been validated against msg_len.
// This function will additionally validate that sizeof(Msg) <= msg_len
bool                 MsgHeaderMsgs_Print(algo::cstring &str, prot1::MsgHeader &msg, u32 msg_len);

// Parse ascii representation of message into binary, appending new data to BUF.
bool                 MsgHeaderMsgs_ReadStrptrMaybe(algo::strptr str, algo::ByteAry &buf);
```

For `Print`, we can provided a `MsgHeader` and `amc` will automatically dispatch to any known
message type and print the result to string.
For `ReadStrptrMaybe`, `amc` will convert a string representation of any known message to binary,
and store the result in the provided `ByteAry`. The resulting byte array, if no-empty, can be examined
as a `MsgHeader` and will contain the actual message.

### Variable-length messages
<a href="#variable-length-messages"></a>

Amc supports messages where the last field has reftype `Opt` or `Varlen`.
With `Opt`, the field is optional: there is 0 or 1 copies of the field, depending on length.
With `Varlen`, there is an array of 0 or more fields, again depending on message length. Let's declare
a `Varlen` field:

```
    dmmeta.ctype  ctype:prot1.DataMsg  comment:""
    dmmeta.cpptype  ctype:prot1.DataMsg  ctor:Y  dtor:N  cheap_copy:N
    dmmeta.msgtype  ctype:prot1.DataMsg  type:3
    dmmeta.pack  ctype:prot1.DataMsg  comment:""
    dmmeta.field  field:prot1.DataMsg.base  arg:prot1.MsgHeader  reftype:Base    dflt:""  comment:""
    dmmeta.field  field:prot1.DataMsg.data  arg:u8  reftype:Varlen  dflt:""  comment:""
```

For the `Varlen` field, the `_Getary` function is generated, which returns a pointer
to N elements in memory, as well as `_N` function which counts the elements.
The `_Addr` function returns the address of the elements.

```
// Access var-length portion as an aryptr. Length is determined from one of the fields.
algo::aryptr<u8>     data_Getary(prot1::DataMsg& parent) __attribute__((nothrow));
u8*                  data_Addr(prot1::DataMsg& parent);
// Return number of elements in varlen field
u32                  data_N(const prot1::DataMsg& parent) __attribute__((__warn_unused_result__, nothrow, pure));
```

### Casting
<a href="#casting"></a>

Since `amc` does not use C++ inheritance in generated code (this is intentional), some utilities
are provided to take advantage of the shared memory layout in some cases. When `amc` determines that a
message shares its memory layout with a `Base` type, it generates functions `_Castdown` and `_Castbase`.

```
$ amc prot1.DataMsg.Cast% -proto
// Check if prot1::MsgHeader is an instance of DataMsg by checking the type field
// If it is, return the pointer of target type.
// Additionally, check if the length field permits valid instance of DataMsg.
// If not successful, quietly return NULL.
prot1::DataMsg*      DataMsg_Castdown(prot1::MsgHeader &hdr);
prot1::MsgHeader&    Castbase(prot1::DataMsg& parent);
```

Note that in `_Castdown` case, even when the type of the messages matches `DataMsg` value,
its length must be at least `sizeof(DataMsg)`, otherwise the function will return NULL.

### Ccmp
<a href="#ccmp"></a>

To ask `amc` to generate a field-wise comparison function for a given ctype,
use the `ccmp` record. We will use `ietf.Ipport` as an example

```
dmmeta.ccmp  ctype:ietf.Ipport  extrn:N  genop:Y  order:Y  minmax:Y  comment:""
```

The fields are as follows:
- `extrn:N` means that amc should generate the code. Otherwise, `amc` omits implementations
but generates forward declarations for all functions and assumes the user will provide the implementations
(or get a link error)
- `order:Y` generates a comparison function. This is defined lexicographically (as operating on each field in turn).
 If `order:N` is specified, then only the equality function is generatored.
- `minmax:Y` asks to generate `_Min`, `_Max`, `_UpdateMin` and `_UpdateMax` functions. Altogether,
- `genop:Y` generates c++ operators `==` and `>` as necessary

Altogether, the following prototypes are generated for the example above:

```
in all cases:
bool                 Ipport_Eq(ietf::Ipport lhs, ietf::Ipport rhs) __attribute__((nothrow));

order:Y
bool                 Ipport_Lt(ietf::Ipport lhs, ietf::Ipport rhs) __attribute__((nothrow));
i32                  Ipport_Cmp(ietf::Ipport lhs, ietf::Ipport rhs) __attribute__((nothrow));
// Set value. Return true if new value is different from old value.
bool                 Ipport_Update(ietf::Ipport &lhs, ietf::Ipport rhs) __attribute__((nothrow));

minmax:Y
// Attempt to make LHS bigger. Return true if it was changed
bool                 Ipport_UpdateMax(ietf::Ipport &lhs, ietf::Ipport rhs) __attribute__((nothrow));
// Return the lesser of two values
ietf::Ipport         Ipport_Min(ietf::Ipport lhs, ietf::Ipport rhs) __attribute__((nothrow));
// Attempt to make LHS smaller. Return true if it was changed
bool                 Ipport_UpdateMin(ietf::Ipport &lhs, ietf::Ipport rhs) __attribute__((nothrow));
// Return the greater of two values
ietf::Ipport         Ipport_Max(ietf::Ipport lhs, ietf::Ipport rhs) __attribute__((nothrow));
```

### Big-endian fields
<a href="#big-endian-fields"></a>

When a field is is stored in memory in a big-endian format, use `fbigend` to reflect it.
Other than that, use the same type for the field. `amc` will rename the data field to make its
sure that doesn't refer to it as-is, and generate `_Get`, and `_Set` functions:

```
      dmmeta.field  field:sxou11.TimeInForce.value  arg:u32  reftype:Val  dflt:""  comment:""
              dmmeta.fbigend  field:sxou11.TimeInForce.value  comment:""

...

// --- sxou11.TimeInForce.value.Get
inline u32 sxou11::value_Get(const sxou11::TimeInForce& parent) {
    return be32toh(parent.value_be); // read big-endian value from memory
}

// --- sxou11.TimeInForce.value.Set
inline void sxou11::value_Set(sxou11::TimeInForce& parent, u32 rhs) {
    parent.value_be = htobe32(rhs); // write big-endian value to memory
}
```

### Namespace-wide packing
<a href="#namespace-wide-packing"></a>

The `nsx` record holds various namespace options. Of interest here is the `pack` option,
which simply requires that all ctypes that are part of the namespace are marked as `pack`.

```
dmmeta.nsx  ns:prot1     pack:Y ...
```

