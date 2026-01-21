## lib_ws - WebSocket library


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Restrictions and limitations](#restrictions-and-limitations)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Introduction](#introduction)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Data model](#data-model)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Library initialization](#library-initialization)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Creating and starting new server](#creating-and-starting-new-server)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Registering hooks](#registering-hooks)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [h_open](#h_open)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [h_message](#h_message)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [h_close](#h_close)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sending messages](#sending-messages)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ping/pong](#ping-pong)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tuning server limits](#tuning-server-limits)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

Lib_ws is a lightweight implementation of server part of WebSocket protocol, 
that allows C++ app to be a WebSocket server.

The library implements:
- TCP server,
- opening handshake over HTTP protocol,
- closing handshake,
- sending and receiving WebSocket messages - user and control ones,
- unmasking of incoming messages,
- assembly of fragmented incoming messages,
- ping/pong,
- notifications (app hooks) on connection events:
    - open
    - close
    - incoming message
- tunable incoming frame and message size limits;

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Restrictions and limitations
<a href="#restrictions-and-limitations"></a>

- Only current version of WebSocket protocol is supported (11), it is not checked;
- HTTP/1.1 only;
- Server connections are assumed to be WebSocket only:
    - URL is not seen;
    - HTTP headers are not seen, except `Sec-WebSocket-Key`;
    - use other Web server to serve static files like html pages 
and scripts needed for working with our WebSocket server;
- Fragmentation of outgoing messages is not supported;
- Connection timeouts are not implemented (TODO);
- HTTPS protocol is not supported - use reverse proxy;
- Extensions are not supported - RCVD fields are not analyzed.

### Introduction
<a href="#introduction"></a>

WebSocket protocol has been created for full-duplex communication
between Web browser and server in a single TCP connection without abusing
distinct HTTP calls and long polls for upstream notifications.

WebSocket protocol takes an advantage of:
- bidirectionality,
- real time,
- text and binary message support,
- built-in message framing,
- extremely low overhead thanks to binary protocol and adaptive framing.

WebSocket killer uses are: social feeds, multiplayer games, collaborative editing,
clickstream data, financial tickers, sports updates, multimedia chat, 
location-based apps, communal chalkboard and more.

For theory in deep, refer to RFC 6455.

### Data model
<a href="#data-model"></a>

Library data model consists of two entities:
- `FServer` - server, identified by bind address and port;
- `FConn` - server connection identified by two pairs - server bind address/port and client address/port;

Connection has state machine for HTTP protocol, there are the following states:
```
inline-command: acr dmmeta.fconst:ws.HttpState.value/%
dmmeta.fconst  fconst:ws.HttpState.value/INIT     value:0  comment:"created connection, waiting request-line"
dmmeta.fconst  fconst:ws.HttpState.value/REQLINE  value:1  comment:"request-line received, waiting first header"
dmmeta.fconst  fconst:ws.HttpState.value/HEADER   value:2  comment:"waiting next header"
dmmeta.fconst  fconst:ws.HttpState.value/BODY     value:3  comment:"headers received, waiting body"
dmmeta.fconst  fconst:ws.HttpState.value/FIN      value:4  comment:"connection closed, to be deleted"
report.acr  n_select:5  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

Protocol constructs are defined within `ws` namespace.

WebSocket Status is defined on `wsdb.status` table.

```
inline-command: acr wsdb.status
wsdb.status  code:1000  reason:"Normal closure"                 local_only:N  comment:""
wsdb.status  code:1001  reason:"Going away"                     local_only:N  comment:"Server going down or browser page close"
wsdb.status  code:1002  reason:"Protocol error"                 local_only:N  comment:""
wsdb.status  code:1003  reason:"Unsupported message data type"  local_only:N  comment:"Text message while expecting binary and vice-versa"
wsdb.status  code:1005  reason:"No status code"                 local_only:Y  comment:"No status code received in remote close message"
wsdb.status  code:1006  reason:"Abnormal closure"               local_only:Y  comment:"Lower layer failure"
wsdb.status  code:1007  reason:"Invalid message data"           local_only:N  comment:"E.g. non-UTF-8 within text message"
wsdb.status  code:1008  reason:"Policy violation"               local_only:N  comment:""
wsdb.status  code:1009  reason:"Message too big"                local_only:N  comment:""
wsdb.status  code:1010  reason:"Missing extension"              local_only:N  comment:""
wsdb.status  code:1011  reason:"Internal error"                 local_only:N  comment:""
wsdb.status  code:1015  reason:"TLS Handshake"                  local_only:Y  comment:""
report.acr  n_select:12  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

If necessary, one may add private codes in range 4000-4999 as per RFC 6455.

### Library initialization
<a href="#library-initialization"></a>

Not needed - done statically.

### Creating and starting new server
<a href="#creating-and-starting-new-server"></a>

```
lib_ws::FServer *StartServer(strptr addr);
```

`addr` is user to bind lisen socket, it is in form of `ip:port`, 
which is internally read as `ietf::Ipport` type. 
If port is omitted or zero, default port is assumed. 
Default port is tunable on the library basis (initial value is 80).
If ip address is omitted, 0.0.0.0 (aka INADDR_ANY) is used.

Valid calls are:
```
StartServer("1.2.3.4:5000");
StartServer(":5000");          // 0.0.0.0:5000
StartServer("1.2.3.4");        // 1.2.3.4:80
StartServer("");               // 0.0.0.0:80
```
The API creates server socket, binds it to specified address, starts listening, register iohook for read and write.
The API returns server context on succes, NULL otherwise.
Returned server context may be tuned as appropriate.

### Registering hooks
<a href="#registering-hooks"></a>

Three hooks are available
```
lib_ws::_db.h_open - new connection
lib_ws::_db.h_message - incoming text or binary message on connection
lib_ws::_db.h_close - connection close

```

All three hooks has the same signature - server connection context as an argument.
```
HookFunction(void*,lib_ws::FConn& conn);
```

### h_open
<a href="#h_open"></a>

`h_open` is used to allocate server resources for new client connection.
The hook is called when opening handshake has been completed,
and connection is ready to send and receive WebSocket frames.

### h_message
<a href="#h_message"></a>

`h_message` is used to process incoming text or binary message.
The hook is called as soon as complete message has been received, 
that is if message is fragmented, it is reassembled.

Message bytes or characters are available on `conn.msg`.

Message opcode (text or binary) is available on `msg.opcode`.

### h_close
<a href="#h_close"></a>

`h_close` is used to free connection resources.
The hook is called when server closes connection on its side.
`conn.code` and `conn.reason` are WebSocket status code and reason.

### Sending messages
<a href="#sending-messages"></a>

One may send messages when connection is open i.e. from `h_open` call up to `h_close` call.
To check if an arbitrary connection is open or not use `OpenQ()`.
Do not send messages if connection is not open, this yields an exception.
Message sending is non-blocking operation. 
WebSocket frame is constructed directly on connection write buffer (conn.wrbuf), 
and sent immediately if socket is ready, or later if not.

Core API to send messages:
```
void lib_ws::SendMessage(lib_ws::FConn &conn, ws_OpcodeEnum opcode, strptr payload);
```

And basic helpers:
```
void SendTextMessage(lib_ws::FConn &conn, strptr payload);
void SendBinaryMessage(lib_ws::FConn &conn, algo::memptr payload);
```

Other helpers are intended to be used internally, do not call them directly.

Example:
```
SendTextMessage(conn,"Hello!")
```

#### Closing connection - server initiated
<a href="#closing-connection"></a>

One may call `Close()` API to force connection closing at any time, inside or outside hooks.
To set the code and reason, set `code`, and `reason` as appropriate. 
If code is not set, 1006 will be used. If reason is omited, it is filled from `wsdb.status` table.
`h_close` always called directly from `Close()` if connection was open
(i.e. opening handshake has been completed, and `h_open` has been previously called.

Example:

```
value_SetEnum(conn.code,ws_StatusCode_Internal_Error);
conn.reason = "Assertion failed: ...";
Close(conn);
```

Server sends `close` message with given reason and code, 
and closes connection immediately - it shall not wait response from client.

Note that API does not delete connection context immediately because 
it may be used in various hooks those may access connection context after `Close()` call.

#### Closing connection - client initiated
<a href="#closing-connection"></a>

Client sends close frame - server receives it, echoes it back and immediately closes the connection.

### Ping/pong
<a href="#ping-pong"></a>

On `ping` message, server automatically answers with `pong` message echoing message payload.
Server never sends unsolicited pong message, and ignores incoming one.

This function implemented according to specification, 
but it is useless, as browsers do not support ping/pong.

### Tuning server limits
<a href="#tuning-server-limits"></a>

Configuration parameters on server context protects from malicious attacks using huge frames and messages.
- `max_rcvd_frame_size` limits frame size in bytes (default: 128 kibibytes);
- `max_rcvd_msg_size` limits message size in bytes (default: 10 mebibytes).

### Example
<a href="#example"></a>

For server example, see `cpp/samp_ws.cpp`.

For client example, see  `www/wsdemo.html`.

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// safe cast for cppcheck
template<typename T> ws::FrameHeader *CastbasePtr(T *ptr) 
```

```c++
// Compute Sec-WebSocket-Accept from Sec-WebSocket-Key
tempstr lib_ws::ComputeSecWebSocketAccept(strptr sec_websocket_key) 
```

```c++
ws::FrameHeader *lib_ws::FrameHeader_FmtByteAry(algo::ByteAry &buf, bool fin, bool rsv1, bool rsv2, bool rsv3,  ws::Opcode opcode, bool mask, u32 masking_key, strptr payload) 
```

```c++
void lib_ws::FrameIdx_Frame16(lib_ws::FrameIdx &idx, ws::Frame16 &frame, u32) 
```

```c++
void lib_ws::FrameIdx_Frame64(lib_ws::FrameIdx &idx, ws::Frame64 &frame, u32) 
```

```c++
void lib_ws::FrameIdx_FrameMasked16(lib_ws::FrameIdx &idx, ws::FrameMasked16 &frame, u32) 
```

```c++
void lib_ws::FrameIdx_FrameMasked64(lib_ws::FrameIdx &idx, ws::FrameMasked64 &frame, u32) 
```

```c++
int lib_ws::FrameIdx_Unkmsg(lib_ws::FrameIdx &idx, ws::FrameHeader &hdr, u32) 
```

```c++
void lib_ws::FrameLen_Frame16(i32 &len, ws::Frame16 &frame, u32 msg_len) 
```

```c++
void lib_ws::FrameLen_Frame64(i32 &len, ws::Frame64 &frame, u32 msg_len) 
```

```c++
void lib_ws::FrameLen_FrameMasked16(i32 &len, ws::FrameMasked16 &frame, u32 msg_len) 
```

```c++
void lib_ws::FrameLen_FrameMasked64(i32 &len, ws::FrameMasked64 &frame, u32 msg_len) 
```

```c++
int lib_ws::FrameLen_Unkmsg(i32 &len, ws::FrameHeader &hdr, u32 msg_len) 
```

```c++
i32 lib_ws::GetMsgLen(algo::memptr buf) 
```

```c++
void lib_ws::ToggleMasking(strptr payload, u32 masking_key) 
```

```c++
void lib_ws::ToggleMasking(ws::FrameHeader &frame, bool reset_key) 
```

```c++
strptr lib_ws::payload_Getary(ws::FrameHeader &frame) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_ws` takes the following tables on input:
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
|[cpp/gen/lib_ws_gen.cpp](/cpp/gen/lib_ws_gen.cpp)||
|[cpp/lib_ws/lib_ws.cpp](/cpp/lib_ws/lib_ws.cpp)||
|[include/gen/lib_ws_gen.h](/include/gen/lib_ws_gen.h)||
|[include/gen/lib_ws_gen.inl.h](/include/gen/lib_ws_gen.inl.h)||
|[include/lib_ws.h](/include/lib_ws.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`lib_ws` generated code creates the tables below.
All allocations are done through global `lib_ws::_db` [lib_ws.FDb](#lib_ws-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_ws.FDb](#lib_ws-fdb)||FDb._db (Global)|
|[lib_ws.FrameIdx](#lib_ws-frameidx)||

#### lib_ws.FDb - In-memory database for lib_ws
<a href="#lib_ws-fdb"></a>

#### lib_ws.FDb Fields
<a href="#lib_ws-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ws.FDb._db|[lib_ws.FDb](/txt/lib/lib_ws/README.md#lib_ws-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ws_gen.h](/include/gen/lib_ws_gen.h)
```
struct FDb { // lib_ws.FDb: In-memory database for lib_ws
    lib_ws::trace   trace;   //
};
```

#### lib_ws.FrameIdx - 
<a href="#lib_ws-frameidx"></a>

#### lib_ws.FrameIdx Fields
<a href="#lib_ws-frameidx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ws.FrameIdx.masking_key|u32|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ws.FrameIdx.payload|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FrameIdx
<a href="#struct-frameidx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ws_gen.h](/include/gen/lib_ws_gen.h)
```
struct FrameIdx { // lib_ws.FrameIdx
    u32*           masking_key;   // optional pointer
    algo::strptr   payload;       //
    // func:lib_ws.FrameIdx..Ctor
    inline               FrameIdx() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

