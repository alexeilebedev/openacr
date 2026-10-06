## atf_amc - Unit tests for amc (see amctest table)
<a href="#atf_amc"></a>
`atf_amc` tests the code `amc` generates.  Its namespace declares ~140 ctypes
that exercise the reftypes, and each test calls the generated functions on them
and checks the result.  CI runs it in the `comp` cijob as the `atf_amc` citest.

### Syntax
<a href="#syntax"></a>
```usage
atf_amc: Unit tests for amc (see amctest table)
Usage: atf_amc [[-amctest:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [amctest]   regx    "%"     SQL regex, selecting test to run
    -dofork             Y       Use fork
    -q                          Quiet mode
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Each test is a row of `atfdb.amctest`, ~330 of them, and a function
`atf_amc::amctest_<name>` under `cpp/atf_amc/`.  The table is compiled into
`atf_amc`, so a new row needs an `amc` run and a rebuild before it can run.

`atf_amc` runs every test whose name matches the regx.  By default each one runs
in a child process of its own, several at a time, so a test starts from a fresh
database and a crash fails only that test.  Each test prints an `atf_amc.run`
line with `success:Y` or `success:N`.  After the run, `atf_amc` prints an
`atf_amc.failure` line for every test that failed, and it exits non-zero when
any did.  A regx that matches no test fails with `atf_amc.nomatch`.

To add a test, insert its row and run `amc`.  Then `src_func -createmissing`
writes the empty `amctest_<name>` function for you to fill in.

### Examples
<a href="#examples"></a>

```bash
atf_amc                                 # run every test, in parallel
atf_amc CastDown                        # run one test
atf_amc 'Cast%'                         # run the tests whose names start with Cast
atf_amc 'Cast%' -dofork:N               # run them in sequence, in one process
acr amctest:Cast%                       # list the tests a regx selects, with their comments
```

```
$ atf_amc 'Cast%'
atf_amc.begin  amctest:Cast%  nmatch:5  dofork:Y
atf_amc.begin  amctest:CastDown  nmatch:1  dofork:N
atf_amc.run  amctest:CastDown  success:Y  comment:"Cast from header to message"
...
atf_amc.run  amctest:CastUp  success:Y  comment:"Cast from message to its header"
```

### Caveats
<a href="#caveats"></a>

- Under `-dofork:N` the tests share one process, so a test sees what the tests
  before it left in the database.  A test that checks a pristine database, such
  as an index's initial size, can then fail though it passes alone.
- A test in a child process gets 900 seconds before it is killed.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The standard input-directory option, `data` by default.  The tests build their
own input, and the list of tests is compiled in, so the option has no everyday
use.

#### -amctest -- SQL regex, selecting test to run
<a href="#-amctest"></a>

The tests to run, as an SQL regx over `atfdb.amctest` names.  The default `%`
runs them all.

#### -dofork -- Use fork
<a href="#-dofork"></a>

Run each test in its own child process, several in parallel, on by default.
`-dofork:N` runs the selected tests one after another in the `atf_amc` process
itself, which is easier to follow in a debugger.  A regx that selects one test
always runs it in-process.

#### -q -- Quiet mode
<a href="#-q"></a>

Leave out the `atf_amc.begin` line for the whole run and the closing list of
failures.  Each child process still prints its own `begin` and `run` lines.
