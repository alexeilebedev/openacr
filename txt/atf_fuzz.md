## atf_fuzz: fuzz tester

### Syntax

```
inline-command: atf_fuzz -h


Usage: atf_fuzz [options]
    -reprofile  string  File where repros are stored. default: "temp/atf_fuzz.repro"
    [target]    string  Target to fuzz
    [args]      string  Additional arguments to target
    -inputfile  string  File with input tuples.
    -fuzzstrat  string  Strategy to choose. default: "%"
    -in         string  Input directory or filename, - for stdin. default: "data"
    -seed       int     Random seed. default: 0
    -testprob   double  Run each case with this probability. default: 1
    -verbose            Enable verbose mode
    -debug              Enable debug mode
    -version            Show version information
    -sig                Print SHA1 signatures for dispatches
    -help               Print this screen and exit

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
report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
```

### Command-line flags

* -reprofile: This is the file where commands that cause a crash are saved after a run.
* -target: This is the target to run. Additional arguments can be passed via `-args`
* -fuzzstrat: Strategy to use. See list
* -in: Input data
* -seed: Random seed (see -testprob)
* -testprob: Each test is invoked with this probability. This can be adjusted to sparse out the space
of inputs in case it is too big
