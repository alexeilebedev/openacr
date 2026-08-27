## ws - WebSocket binary protocol


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Ctypes](#ctypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### ws.CloseMsg
<a href="#ws-closemsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype/Val.md)|||
|code|[ws.StatusCode](/txt/protocol/ws/README.md#ws-statuscode)|[Val](/txt/exe/amc/reftype/Val.md)|||
|reason|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.ConnectMsg
<a href="#ws-connectmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.DataMsg
<a href="#ws-datamsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype/Val.md)|||
|opcode|[ws.Opcode](/txt/protocol/ws/README.md#ws-opcode)|[Val](/txt/exe/amc/reftype/Val.md)|||
|data|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.Frame
<a href="#ws-frame"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.Frame16
<a href="#ws-frame16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|ext_payload_len|u16|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.Frame64
<a href="#ws-frame64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|ext_payload_len|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.FrameHeader
<a href="#ws-frameheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|byte0|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|byte1|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|fin|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|rsv1|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|rsv2|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|rsv3|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|opcode|[ws.Opcode](/txt/protocol/ws/README.md#ws-opcode)|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|mask|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|payload_len|u8|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||

#### ws.FrameMasked
<a href="#ws-framemasked"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.FrameMasked16
<a href="#ws-framemasked16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|ext_payload_len|u16|[Val](/txt/exe/amc/reftype/Val.md)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.FrameMasked64
<a href="#ws-framemasked64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|ext_payload_len|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ws.HttpState
<a href="#ws-httpstate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.Opcode
<a href="#ws-opcode"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.ServerStartMsg
<a href="#ws-serverstartmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|server|[algo.RspaceStr25](/txt/protocol/algo/README.md#algo-rspacestr25)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.ServerStopMsg
<a href="#ws-serverstopmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|server|[algo.RspaceStr25](/txt/protocol/algo/README.md#algo-rspacestr25)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.Side
<a href="#ws-side"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ws.StatusCode
<a href="#ws-statuscode"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftype/Val.md)|||

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
void ws::FrameHeaderMsgs_Print2(algo::cstring &str, ws::FrameHeader &msg)
```

```c++
bool ws::FrameHeaderMsgs_ReadStrptrMaybe2(algo::strptr str, algo::ByteAry &buf)
```

```c++
ws::FrameMasked *ws::FrameMasked_Castdown(ws::FrameHeader &hdr)
```

```c++
ws::Frame *ws::Frame_Castdown(ws::FrameHeader &hdr)
```

```c++
cstring &ws::operator<<(cstring &lhs, const ws::FrameHeader &rhs)
```

```c++
u32 ws::payload_N(const ws::Frame& parent)
```

```c++
u32 ws::payload_N(const ws::FrameMasked& parent)
```
