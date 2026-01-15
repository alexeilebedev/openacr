## atf_exp - ATF expect
<a href="#atf_exp"></a>

Usage example: <br/>
<br/>
acr tmsg:atf_exp.Kafka/% -field msg | atf_exp
<br/>

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Input Messages](#input-messages)<br/>
&#128196; [atf_exp - Internals](/txt/exe/atf_exp/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_exp: ATF expect
Usage: atf_exp [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -trace      string  ""      Regx of things to trace
    -bindir     string  "bin"   Directory with binaries
    -comptest   string  ""      Component test name
    -tempdir    string  ""      Temporary directory for the component test
    -timeout    double  8.0     Timeout in seconds for a step
    -dbgshell                   Invoke bash with curr envs in case of timeout
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -trace -- Regx of things to trace
<a href="#-trace"></a>

#### -bindir -- Directory with binaries
<a href="#-bindir"></a>

#### -comptest -- Component test name
<a href="#-comptest"></a>

#### -tempdir -- Temporary directory for the component test
<a href="#-tempdir"></a>

#### -timeout -- Timeout in seconds for a step
<a href="#-timeout"></a>

#### -dbgshell -- Invoke bash with curr envs in case of timeout
<a href="#-dbgshell"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_exp` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Input Messages
<a href="#input-messages"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:InputMessages -->
`atf_exp` Consumes the following messages via atf_exp.In ()
|Message|Comment|
|---|---|
|[ams.ProcEofMsg](/txt/protocol/ams/README.md#ams-proceofmsg)|EOF from/to a process|
|[ams.ProcKillMsg](/txt/protocol/ams/README.md#ams-prockillmsg)|Kill process|
|[ams.ProcMsg](/txt/protocol/ams/README.md#ams-procmsg)|Message from/to a process|
|[ams.ProcReadMsg](/txt/protocol/ams/README.md#ams-procreadmsg)|Read process output|
|[ams.ProcStartMsg](/txt/protocol/ams/README.md#ams-procstartmsg)|Start process|
|[ams.RemDirRecurseMsg](/txt/protocol/ams/README.md#ams-remdirrecursemsg)|Message sent to process to terminate it|
|[ams.TerminateMsg](/txt/protocol/ams/TerminateMsg.md)|Message sent to process to terminate it|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:InputMessages -->

