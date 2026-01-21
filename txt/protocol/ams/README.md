## ams - Algo Messaging System protocol


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
<!-- dev.mdmark  mdmark:TOC  state:BEG_AUTO  param:Toc -->
&#128196; [ams.ExpectMsg - Pause reading until some output is produced](/txt/protocol/ams/ExpectMsg.md)<br/>
&#128196; [ams.InputLineMsg - Unrecognized input line](/txt/protocol/ams/InputLineMsg.md)<br/>
&#128196; [ams.LogMsg - Log message](/txt/protocol/ams/LogMsg.md)<br/>
&#128196; [ams.MsgBlock - A block of messages from ams commit file](/txt/protocol/ams/MsgBlock.md)<br/>
&#128196; [ams.MsgHeader -](/txt/protocol/ams/MsgHeader.md)<br/>
&#128196; [ams.ProcId - Process ID within topology: type, node index, process index, e.g. proc-0-0](/txt/protocol/ams/ProcId.md)<br/>
&#128196; [ams.TerminateMsg - Message sent to process to terminate it](/txt/protocol/ams/TerminateMsg.md)<br/>
&#128196; [ams.UdpFrame - sniffer-captured ethernet frame](/txt/protocol/ams/UdpFrame.md)<br/>

<!-- dev.mdmark  mdmark:TOC  state:END_AUTO  param:Toc -->

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### ams.ProcEofMsg - EOF from/to a process
<a href="#ams-proceofmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### ams.ProcKillMsg - Kill process
<a href="#ams-prockillmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|signal|i32|[Val](/txt/exe/amc/reftypes.md#val)|15||

#### ams.ProcMsg - Message from/to a process
<a href="#ams-procmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|payload|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### ams.ProcReadMsg - Read process output
<a href="#ams-procreadmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|until|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### ams.ProcStartMsg - Start process
<a href="#ams-procstartmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|pty|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|cmd|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### ams.ProcStatusMsg - Process status
<a href="#ams-procstatusmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|proc|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|status|[algo.RnullStr50](/txt/protocol/algo/README.md#algo-rnullstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### ams.Proctype - 
<a href="#ams-proctype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### ams.RemDirRecurseMsg - Message sent to process to terminate it
<a href="#ams-remdirrecursemsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|remove_topmost|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|pathname|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### ams.ShmFlags - 
<a href="#ams-shmflags"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|write|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|read|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|nonblock|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|write_err|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||Error occurred during output|

#### ams.ShmHbMsg - Shm heartbeat
<a href="#ams-shmhbmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|shmember|[ams.ShmemberId](/txt/protocol/ams/README.md#ams-shmemberid)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key|
|off|u64|[Val](/txt/exe/amc/reftypes.md#val)||Read/write position|
|wbudget|u32|[Val](/txt/exe/amc/reftypes.md#val)||Max. offset for writing|

#### ams.ShmId - Host-unique stream ID. E.g. proc1-0.out-0
<a href="#ams-shmid"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Writer process ID|
|shmtype|[ams.Shmtype](/txt/protocol/ams/README.md#ams-shmtype)|[Val](/txt/exe/amc/reftypes.md#val)||Stream type ID|
|index|u8|[Val](/txt/exe/amc/reftypes.md#val)||Stream index|

#### ams.ShmemberFlags - Shm member flags
<a href="#ams-shmemberflags"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftypes.md#val)|0|Shm member flags|
|r|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|0||
|w|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|0||

#### ams.ShmemberId - Shm/Process - member ID for AMS SHM
<a href="#ams-shmemberid"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|shm_id|[ams.ShmId](/txt/protocol/ams/README.md#ams-shmid)|[Val](/txt/exe/amc/reftypes.md#val)||Stream ID|
|proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Process ID|
|flags|[ams.ShmemberFlags](/txt/protocol/ams/README.md#ams-shmemberflags)|[Val](/txt/exe/amc/reftypes.md#val)|0||

#### ams.Shmmsg - Sequenced message on a shm
<a href="#ams-shmmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|shm_id|[ams.ShmId](/txt/protocol/ams/README.md#ams-shmid)|[Val](/txt/exe/amc/reftypes.md#val)|||
|off|u64|[Val](/txt/exe/amc/reftypes.md#val)|0||
|tstamp|u64|[Val](/txt/exe/amc/reftypes.md#val)|0||
|payload|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Opt](/txt/exe/amc/reftypes.md#opt)||Actual Payload|

#### ams.Shmtype - 
<a href="#ams-shmtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->

