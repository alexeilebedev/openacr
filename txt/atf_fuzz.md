## atf_fuzz: Generator of bad inputs for targets



### Syntax

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

### Description

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

### Command-line flags

* -reprofile: This is the file where commands that cause a crash are saved after a run.
* -target: This is the target to run. Additional arguments can be passed via `-args`
* -fuzzstrat: Strategy to use. See list
* -in: Input data
* -seed: Random seed (see -testprob)
* -testprob: Each test is invoked with this probability. This can be adjusted to sparse out the space
of inputs in case it is too big

### Inputs

`atf_fuzz` takes the following tables on input:
```
CTYPE                COMMENT
dev.Target           Build target
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```

