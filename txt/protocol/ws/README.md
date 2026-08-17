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
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype.md#val)|||
|code|[ws.StatusCode](/txt/protocol/ws/README.md#ws-statuscode)|[Val](/txt/exe/amc/reftype.md#val)|||
|reason|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.ConnectMsg
<a href="#ws-connectmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.DataMsg
<a href="#ws-datamsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|conn|[algo.RspaceStr50](/txt/protocol/algo/README.md#algo-rspacestr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|side|[ws.Side](/txt/protocol/ws/README.md#ws-side)|[Val](/txt/exe/amc/reftype.md#val)|||
|opcode|[ws.Opcode](/txt/protocol/ws/README.md#ws-opcode)|[Val](/txt/exe/amc/reftype.md#val)|||
|data|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.Frame
<a href="#ws-frame"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.Frame16
<a href="#ws-frame16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|ext_payload_len|u16|[Val](/txt/exe/amc/reftype.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.Frame64
<a href="#ws-frame64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|ext_payload_len|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.FrameHeader
<a href="#ws-frameheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|byte0|u8|[Val](/txt/exe/amc/reftype.md#val)|||
|byte1|u8|[Val](/txt/exe/amc/reftype.md#val)|||
|fin|bool|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|rsv1|bool|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|rsv2|bool|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|rsv3|bool|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|opcode|[ws.Opcode](/txt/protocol/ws/README.md#ws-opcode)|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|mask|bool|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||
|payload_len|u8|[Bitfld](/txt/exe/amc/reftype.md#bitfld)|||

#### ws.FrameMasked
<a href="#ws-framemasked"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.FrameMasked16
<a href="#ws-framemasked16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|ext_payload_len|u16|[Val](/txt/exe/amc/reftype.md#val)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.FrameMasked64
<a href="#ws-framemasked64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ws.FrameHeader](/txt/protocol/ws/README.md#ws-frameheader)|[Base](/txt/exe/amc/reftype.md#base)|||
|ext_payload_len|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|masking_key|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftype.md#varlen)|||

#### ws.HttpState
<a href="#ws-httpstate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.Opcode
<a href="#ws-opcode"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.ServerStartMsg
<a href="#ws-serverstartmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|server|[algo.RspaceStr25](/txt/protocol/algo/README.md#algo-rspacestr25)|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.ServerStopMsg
<a href="#ws-serverstopmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|server|[algo.RspaceStr25](/txt/protocol/algo/README.md#algo-rspacestr25)|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.Side
<a href="#ws-side"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftype.md#val)|||

#### ws.StatusCode
<a href="#ws-statuscode"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftype.md#val)|||

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
