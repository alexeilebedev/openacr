## atf_cmdline - Test tool for command line parsing


### Syntax
<a href="#syntax"></a>
```usage
atf_cmdline: Test tool for command line parsing
Usage: atf_cmdline [-astr:]<string> [[-anum:]<int>] [[-adbl:]<double>] -str:<string> [[-amnum:]<int>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -exec                       Execv itself
    [astr]      string          Required anon string
    [anum]      int     0       Anon number
    [adbl]      double  0.0     Anon double
    [aflag]                     Anon flag
    -str        string          Required string
    -num        int     0       Required Number
    -dbl        double  0.0     Required double
    -flag                       Required flag
    -dstr       string  "blah"  Predefined string
    -dnum       int     -33     Predefined number
    -ddbl       double  0.0001  Predefined double
    -dflag              Y       Predefined flag
    -mstr...    string          String array
    -mnum...    int             Number array
    -mdbl...    double          Double array
    [amnum]...  int             Anon number array
    -fconst     enum    high    Fconst for field (high|medium|low)
    -cconst     enum    None    Fconst for arg ctype (January|February|March|April|May|June|July|August|September|October|November|December|None)
    -dregx      regx    "%"     Predefined regx
    -dpkey      string  ""      Predefined pkey
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

#### -exec -- Execv itself
<a href="#-exec"></a>

#### -astr -- Required anon string
<a href="#-astr"></a>

#### -anum -- Anon number
<a href="#-anum"></a>

#### -adbl -- Anon double
<a href="#-adbl"></a>

#### -aflag -- Anon flag
<a href="#-aflag"></a>

#### -str -- Required string
<a href="#-str"></a>

#### -num -- Required Number
<a href="#-num"></a>

#### -dbl -- Required double
<a href="#-dbl"></a>

#### -flag -- Required flag
<a href="#-flag"></a>

#### -dstr -- Predefined string
<a href="#-dstr"></a>

#### -dnum -- Predefined number
<a href="#-dnum"></a>

#### -ddbl -- Predefined double
<a href="#-ddbl"></a>

#### -dflag -- Predefined flag
<a href="#-dflag"></a>

#### -mstr -- String array
<a href="#-mstr"></a>

#### -mnum -- Number array
<a href="#-mnum"></a>

#### -mdbl -- Double array
<a href="#-mdbl"></a>

#### -amnum -- Anon number array
<a href="#-amnum"></a>

#### -fconst -- Fconst for field
<a href="#-fconst"></a>

#### -cconst -- Fconst for arg ctype
<a href="#-cconst"></a>

#### -dregx -- Predefined regx
<a href="#-dregx"></a>

#### -dpkey -- Predefined pkey
<a href="#-dpkey"></a>
