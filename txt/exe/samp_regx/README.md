## samp_regx - Test tool for regular expressions


### Syntax
<a href="#syntax"></a>
```usage
samp_regx: Test tool for regular expressions
Usage: samp_regx [-expr:]<string> [[-string:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [expr]      string          Expression
    -style      enum    acr     Regx style (default|sql|acr|shell|literal)
    -trace                      Trace regx innards
    -capture                    Use capture groups
    -full               Y       Match full string
    -f                          <string> is a filename, grep the lines
    -match                      Match a string, exit code represnts success
    [string]    string  ""      String to match
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

#### -expr -- Expression
<a href="#-expr"></a>

#### -style -- Regx style
<a href="#-style"></a>

#### -trace -- Trace regx innards
<a href="#-trace"></a>

#### -capture -- Use capture groups
<a href="#-capture"></a>

#### -full -- Match full string
<a href="#-full"></a>

#### -f -- <string> is a filename, grep the lines
<a href="#-f"></a>

#### -match -- Match a string, exit code represnts success
<a href="#-match"></a>

#### -string -- String to match
<a href="#-string"></a>
