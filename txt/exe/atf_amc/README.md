## atf_amc - Unit tests for amc (see amctest table)


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
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -amctest -- SQL regex, selecting test to run
<a href="#-amctest"></a>

#### -dofork -- Use fork
<a href="#-dofork"></a>

#### -q -- Quiet mode
<a href="#-q"></a>
