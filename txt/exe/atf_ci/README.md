## atf_ci - Normalization tests (see citest table)


### Syntax
<a href="#syntax"></a>
```usage
atf_ci: Normalization tests (see citest table)
Usage: atf_ci [[-citest:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [citest]      regx    "%"     Regx of tests to run
    -maxerr       int     0       Exit after this many errors
    -cijob        regx    "%"
    -capture                      Capture the output of the test
    -check_clean          Y       Check for modifications after each test
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -citest -- Regx of tests to run
<a href="#-citest"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

#### -cijob -- 
<a href="#-cijob"></a>

#### -capture -- Capture the output of the test
<a href="#-capture"></a>

#### -check_clean -- Check for modifications after each test
<a href="#-check_clean"></a>
