## atf_fuzz - Generator of bad inputs for targets


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [atf_fuzz - Internals](/txt/exe/atf_fuzz/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_fuzz: Generator of bad inputs for targets
Usage: atf_fuzz [[-target:]<string>] [[-args:]<string>] [options]
    OPTION      TYPE    DFLT                   COMMENT
    -reprofile  string  "temp/atf_fuzz.repro"  File where repros are stored
    [target]    string  ""                     Target to fuzz
    [args]      string  ""                     Additional arguments to target
    -inputfile  string  ""                     File with input tuples.
    -fuzzstrat  regx    "%"                    Strategy to choose
    -in         string  "data"                 Input directory or filename, - for stdin
    -seed       int     0                      Random seed
    -testprob   double  1                      Run each case with this probability
    -verbose    int                            Verbosity level (0..255); alias -v; cumulative
    -debug      int                            Debug level (0..255); alias -d; cumulative
    -help                                      Print help and exit; alias -h
    -version                                   Print version and exit
    -signature                                 Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

`atf_fuzz` is a tool for finding crash bugs in other tools (including itself).
It uses `acr_in` to collect all the inputs for specified target,
and then runs the target repeatedly inside a temporary directory, supplying it
modified inputs.

The list of strategies supported by `atf_fuzz` is as follows:

```
inline-command: acr fuzzstrat
atfdb.fuzzstrat  fuzzstrat:skip_inputs  comment:"Run target in sandbox with various missing inputs"
report.acr  n_select:1  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -reprofile -- File where repros are stored
<a href="#-reprofile"></a>

This is the file where commands that cause a crash are saved after a run.

#### -target -- Target to fuzz
<a href="#-target"></a>

This is the target to run. Additional arguments can be passed via `-args`

#### -args -- Additional arguments to target
<a href="#-args"></a>

#### -inputfile -- File with input tuples.
<a href="#-inputfile"></a>

#### -fuzzstrat -- Strategy to choose
<a href="#-fuzzstrat"></a>

The possible values are
```
inline-command: acr fuzzstrat -report:N | ssimfilt -t
FUZZSTRAT    COMMENT
skip_inputs  Run target in sandbox with various missing inputs

```

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -seed -- Random seed
<a href="#-seed"></a>

#### -testprob -- Run each case with this probability
<a href="#-testprob"></a>

Each test is invoked with this probability. This can be adjusted to sparse out the space
of inputs in case it is too big.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_fuzz` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

