## acr_compl - ACR shell auto-complete for all targets


### Syntax
<a href="#syntax"></a>
```usage
acr_compl: ACR shell auto-complete for all targets
Usage: acr_compl [options]
    OPTION        TYPE    DFLT    COMMENT
    -data         string  "data"  Source for completions (dir or file or -)
    -schema       string  "data"  Source for schema information
    -line         string  ""      Simulates COMP_LINE (debug)
    -point        string  ""      Simulates COMP_POINT (debug). default: whole line
    -type         string  "9"     Simulates COMP_TYPE (debug)
    -install                      Produce bash commands to install the handler
    -debug_log    string  ""      Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
    -check                        Check command line validity
    -check_batch                  Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -data -- Source for completions (dir or file or -)
<a href="#-data"></a>

#### -schema -- Source for schema information
<a href="#-schema"></a>

#### -line -- Simulates COMP_LINE (debug)
<a href="#-line"></a>

#### -point -- Simulates COMP_POINT (debug). default: whole line
<a href="#-point"></a>

#### -type -- Simulates COMP_TYPE (debug)
<a href="#-type"></a>

#### -install -- Produce bash commands to install the handler
<a href="#-install"></a>

#### -debug_log -- Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
<a href="#-debug_log"></a>

#### -check -- Check command line validity
<a href="#-check"></a>

#### -check_batch -- Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure
<a href="#-check_batch"></a>
