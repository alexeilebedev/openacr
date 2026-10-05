## atf_fuzz - Generator of bad inputs for targets
<a href="#atf_fuzz"></a>
`atf_fuzz` looks for crashes in a tool by running it again and again on
damaged copies of its input.  Each run happens in a sandbox, so the tree stays
untouched, and `atf_fuzz` saves every command line that crashed the tool to a
file you can replay.

### Syntax
<a href="#syntax"></a>
```usage
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
    -verbose    flag                           Verbosity level (0..255); alias -v; cumulative
    -debug      flag                           Debug level (0..255); alias -d; cumulative
    -trace      string  ""                     Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                                      Print help and exit; alias -h
    -version                                   Print version and exit
    -signature                                 Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`atf_fuzz` takes a target, which is a row of `dev.target`, and applies each
strategy of `atfdb.fuzzstrat` that `-fuzzstrat` selects.  The strategies are
compiled into `atf_fuzz`:

```ssim
inline-command: acr fuzzstrat
atfdb.fuzzstrat  fuzzstrat:skip_inputs  comment:"Run target in sandbox with various missing inputs"
report.acr  n_select:1  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

`skip_inputs` first collects every record the target reads with `acr_in -data`,
into `temp/atf_fuzz.in`, unless `-inputfile` names a file.  It resets the
`atf_fuzz` sandbox, `wt/atf_fuzz`.  Then, for each record, it deletes that one
record from the sandbox's data and runs the target there with `-args`.  A run
that dies of SIGSEGV counts as a crash: `atf_fuzz` prints `CRASH` with the
command and appends the command to `-reprofile`.  `atf_fuzz` cleans the sandbox
after every run, so each case removes exactly one record.  A final
`atf_fuzz.skip_inputs.end` line gives the number of cases and crashes.

### Examples
<a href="#examples"></a>

```bash
atf_fuzz acr_in                        # fuzz acr_in with every record it reads
atf_fuzz amc -testprob:0.01 -seed:5    # fuzz amc on about one record in a hundred
atf_fuzz acr 'dmmeta.ns:algo_lib' -v   # fuzz one acr query, showing each run's output
cat temp/atf_fuzz.repro                # the commands that crashed
```

### Caveats
<a href="#caveats"></a>

- `atf_fuzz` counts only SIGSEGV as a crash, so a target that aborts, fails an
  assertion or exits with an error passes.
- There is one run per input record, and `acr_in` alone reads ~30K records.
  Use `-testprob` to sample a large target.
- Each run replaces `-reprofile`, so copy the file before the next run if you
  want to keep it.

### Options
<a href="#options"></a>
#### -reprofile -- File where repros are stored
<a href="#-reprofile"></a>

The file that collects the commands that crashed the target, one per line.
Each saved command changes into the sandbox, deletes the record and runs the
target, so running it from the repo root replays the crash.

#### -target -- Target to fuzz
<a href="#-target"></a>

The target to run, a key of `dev.target`, given by position or as
`-target:<name>`.  A name with no `dev.target` row fails with
`atf_fuzz.badtarget`.  Pass further arguments with `-args`.

#### -args -- Additional arguments to target
<a href="#-args"></a>

Arguments added to the target's command line on every run, as one string.  Use
it to make the target do real work, since many tools do little with no
arguments.

#### -inputfile -- File with input tuples.
<a href="#-inputfile"></a>

A file of ssim records to delete one at a time, in place of the full set that
`acr_in` finds.  Use it to aim the fuzzer at one table.

#### -fuzzstrat -- Strategy to choose
<a href="#-fuzzstrat"></a>

An SQL regex over the strategies, and `atf_fuzz` applies each one that
matches.  The possible values are
```ssim
inline-command: acr fuzzstrat -report:N | ssimfilt -t
FUZZSTRAT    COMMENT
skip_inputs  Run target in sandbox with various missing inputs

```

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory `atf_fuzz` reads `dev.target` from, to look up the target.  The
default is `data`.

#### -seed -- Random seed
<a href="#-seed"></a>

The seed for the random choice that `-testprob` makes.  The same seed picks the
same cases again.

#### -testprob -- Run each case with this probability
<a href="#-testprob"></a>

The chance that each case runs, between zero and one.  The default of one runs
every case.  Lower it when the target's input is too large to try in full.
