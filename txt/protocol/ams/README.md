## ams - Algo Messaging System protocol


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Ctypes](#ctypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&#128196; [ams.InputLineMsg - Unrecognized input line](/txt/protocol/ams/InputLineMsg.md)<br/>
&#128196; [ams.LogMsg - Log message](/txt/protocol/ams/LogMsg.md)<br/>
&#128196; [ams.MsgBlock - A block of messages from ams commit file](/txt/protocol/ams/MsgBlock.md)<br/>
&#128196; [ams.MsgHeader -](/txt/protocol/ams/MsgHeader.md)<br/>
&#128196; [ams.ProcId - Process ID within topology: type, node index, process index, e.g. samp_meng-0-0](/txt/protocol/ams/ProcId.md)<br/>
&#128196; [ams.TerminateMsg - Message sent to process to terminate it](/txt/protocol/ams/TerminateMsg.md)<br/>
&#128196; [ams.UdpFrame - sniffer-captured ethernet frame](/txt/protocol/ams/UdpFrame.md)<br/>
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### ams.BoardrefMsg - Reference to a payload on the writer's message board
<a href="#ams-boardrefmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|offset|u64|[Val](/txt/exe/amc/reftype/Val.md)||Byte offset of the payload within the board segment|
|payload_length|u32|[Val](/txt/exe/amc/reftype/Val.md)||Payload length, cross-checked against the header at the offset|

#### ams.Bridge - Bridge identifier: proc_id + grp pair
<a href="#ams-bridge"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftype/Val.md)||Child proc id|
|in_grp|[ams.GrpId](/txt/protocol/ams/README.md#ams-grpid)|[Val](/txt/exe/amc/reftype/Val.md)||Inbound grp (child writes, parent reads)|
|out_grp|[ams.GrpId](/txt/protocol/ams/README.md#ams-grpid)|[Val](/txt/exe/amc/reftype/Val.md)||Outbound grp (parent writes, child reads)|

#### ams.GrpId - Group id: proctype-nodeidx-procidx.grptype-grpidx
<a href="#ams-grpid"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftype/Val.md)||Writer process ID|
|grptype|[ams.Grptype](/txt/protocol/ams/README.md#ams-grptype)|[Val](/txt/exe/amc/reftype/Val.md)||Stream type ID|
|index|u8|[Val](/txt/exe/amc/reftype/Val.md)||Stream index|

#### ams.GrpmemberId - Shm/Process - member ID for AMS SHM
<a href="#ams-grpmemberid"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grp_id|[ams.GrpId](/txt/protocol/ams/README.md#ams-grpid)|[Val](/txt/exe/amc/reftype/Val.md)||Stream ID|
|proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftype/Val.md)||Process ID|
|flags|[ams.ShmemberFlags](/txt/protocol/ams/README.md#ams-shmemberflags)|[Val](/txt/exe/amc/reftype/Val.md)|0||

#### ams.Grptype
<a href="#ams-grptype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.I64Price8 - Fixed-point price, 8 decimals
<a href="#ams-i64price8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.Proctype
<a href="#ams-proctype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampFoldstate - The sample fold's state: the chained hash and how many inputs it accounts for
<a href="#ams-sampfoldstate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|hash|u64|[Val](/txt/exe/amc/reftype/Val.md)||Every input up to the boundary, chained through Fnv1a64|
|n_in|u64|[Val](/txt/exe/amc/reftype/Val.md)||Inputs the hash accounts for|

#### ams.SampMengBookMsg - Matcher state: the book's scalars, opening its snapshot
<a href="#ams-sampmengbookmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|next_order_id|u64|[Val](/txt/exe/amc/reftype/Val.md)||Order id the matcher will assign next|

#### ams.SampMengCancelOrderMsg
<a href="#ams-sampmengcancelordermsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|order|u64|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengCancelReqMsg
<a href="#ams-sampmengcancelreqmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|order|u64|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengMassCancelReqMsg
<a href="#ams-sampmengmasscancelreqmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|user|u32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewOrderMsg
<a href="#ams-sampmengnewordermsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|price|[ams.I64Price8](/txt/protocol/ams/README.md#ams-i64price8)|[Val](/txt/exe/amc/reftype/Val.md)|||
|order|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)|||
|qty|u32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewOrderReqMsg
<a href="#ams-sampmengneworderreqmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|ioc|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|qty|i32|[Val](/txt/exe/amc/reftype/Val.md)||>0 = buy, <0 = sell|
|price|[ams.I64Price8](/txt/protocol/ams/README.md#ams-i64price8)|[Val](/txt/exe/amc/reftype/Val.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)|||
|user|u32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewSymbolMsg
<a href="#ams-sampmengnewsymbolmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewSymbolReqMsg
<a href="#ams-sampmengnewsymbolreqmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewUserMsg
<a href="#ams-sampmengnewusermsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|user|u32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengNewUserReqMsg
<a href="#ams-sampmengnewuserreqmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|user|u32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengOrderTradeMsg
<a href="#ams-sampmengordertrademsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|order|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|qty|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|price|[ams.I64Price8](/txt/protocol/ams/README.md#ams-i64price8)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengRestingMsg - Matcher state: one order resting on the book
<a href="#ams-sampmengrestingmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|order|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)|||
|price|[ams.I64Price8](/txt/protocol/ams/README.md#ams-i64price8)|[Val](/txt/exe/amc/reftype/Val.md)|||
|qty|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|user|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|buy|bool|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengSymbol - Symbol identifier
<a href="#ams-sampmengsymbol"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|symbol|[algo.RnullStr8](/txt/protocol/algo/README.md#algo-rnullstr8)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.SampMengSymbolrowMsg - Matcher state: one symbol, in the order ids were assigned
<a href="#ams-sampmengsymbolrowmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|symbol|[ams.SampMengSymbol](/txt/protocol/ams/README.md#ams-sampmengsymbol)|[Val](/txt/exe/amc/reftype/Val.md)||Symbol name|

#### ams.SampMengTextMsg
<a href="#ams-sampmengtextmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|text|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### ams.SampMengUserrowMsg - Matcher state: one user
<a href="#ams-sampmenguserrowmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|user|i32|[Val](/txt/exe/amc/reftype/Val.md)||User id|

#### ams.ShmFlags
<a href="#ams-shmflags"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|write|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|read|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|write_err|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Error occurred during output|
|signaled|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Writer wakes readers via SIGRTMIN; readers must run in signaled mode|

#### ams.ShmHbMsg - Shm heartbeat
<a href="#ams-shmhbmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|grpmember|[ams.GrpmemberId](/txt/protocol/ams/README.md#ams-grpmemberid)|[Val](/txt/exe/amc/reftype/Val.md)||Primary key|
|off|u64|[Val](/txt/exe/amc/reftype/Val.md)||Read/write position|
|wbudget|u32|[Val](/txt/exe/amc/reftype/Val.md)||Max. offset for writing|

#### ams.Shmember
<a href="#ams-shmember"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grpmember_id|[ams.GrpmemberId](/txt/protocol/ams/README.md#ams-grpmemberid)|[Val](/txt/exe/amc/reftype/Val.md)|||
|pid|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|offset|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|rate|[algo.I64Rate](/txt/protocol/algo/README.md#algo-i64rate)|[Val](/txt/exe/amc/reftype/Val.md)|||
|last_hb|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|ignore2|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|sleeping|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Set by reader before sleeping, cleared by writer before kill()|

#### ams.ShmemberFlags - Shm member flags
<a href="#ams-shmemberflags"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftype/Val.md)|0|Shm member flags|
|r|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|0||
|w|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|0||

#### ams.Shmhdr - Shared memory segment header
<a href="#ams-shmhdr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|magic|u32|[Val](/txt/exe/amc/reftype/Val.md)|0xc001af01||
|grp_id|[ams.GrpId](/txt/protocol/ams/README.md#ams-grpid)|[Val](/txt/exe/amc/reftype/Val.md)||ID of this segment|
|tot_size|u32|[Val](/txt/exe/amc/reftype/Val.md)||Total segment size|
|max_msg_size|u32|[Val](/txt/exe/amc/reftype/Val.md)||Max message size|
|offset_mask|u32|[Val](/txt/exe/amc/reftype/Val.md)||Offset mask for data|
|woff|u64|[Val](/txt/exe/amc/reftype/Val.md)||Writer's offset (absolute)|
|n_shmember|u32|[Val](/txt/exe/amc/reftype/Val.md)||Members registered so far; a reader beyond max_shmember gets no slot|
|max_shmember|u32|[Val](/txt/exe/amc/reftype/Val.md)||Max allowed members|
|datastart|u32|[Val](/txt/exe/amc/reftype/Val.md)||Beginning of data|
|writer_pid|i32|[Val](/txt/exe/amc/reftype/Val.md)||Writer's pid|
|writer_sleeping|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Set by writer before parking on no budget; cleared by reader before kill() (mirror of Shmember.sleeping)|
|nnobudget|u64|[Val](/txt/exe/amc/reftype/Val.md)||Number of times we checked budget and it was isufficient|
|nblock|u64|[Val](/txt/exe/amc/reftype/Val.md)||Number of times shm blocked on writing|
|eof|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Writer exited; readers should drain and stop polling|
|signaled|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Set by writer when in signaled mode; readers SetSignaledMode(true) on open|
|creator_pid|i32|[Val](/txt/exe/amc/reftype/Val.md)||Pid of the process that created the segment; a sweep reclaims only what its creator no longer owns|

#### ams.Trafficclass - Backplane traffic class (see amsdb.trafficclass)
<a href="#ams-trafficclass"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### ams.UprocTextMsg
<a href="#ams-uproctextmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype/Base.md)|||
|seqno|u32|[Val](/txt/exe/amc/reftype/Val.md)||sequence number of uproc message|
|first_seqno|u32|[Val](/txt/exe/amc/reftype/Val.md)||First sequence number of uproc messages|
|last_seqno|u32|[Val](/txt/exe/amc/reftype/Val.md)||Last sequence number of uproc messages|
|name|[algo.RnullStr32](/txt/protocol/algo/README.md#algo-rnullstr32)|[Val](/txt/exe/amc/reftype/Val.md)||uproc instance name|
|text|char|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
// I64Price8: fixed-point price with 8 decimal places
void ams::I64Price8_Print(ams::I64Price8 row, algo::cstring &str)
```

```c++
bool ams::I64Price8_ReadStrptrMaybe(ams::I64Price8 &row, algo::strptr str)
```
