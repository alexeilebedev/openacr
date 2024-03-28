## atf_cmdline - Test tool for command line parsing


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)
* [Tests](#tests)

### Syntax
<a href="#syntax"></a>
```
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
    -fconst     int     high    Fconst for field (high|medium|low)
                                    high  Cool
                                    medium  So-so
                                    low  Bad
    -cconst     enum    None    Fconst for arg ctype (January|February|March|April|May|June|July|August|September|October|November|December|None)
    -dregx      regx    "%"     Predefined regx
    -dpkey      string  ""      Predefined pkey
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf/atf_cmdline.cpp](/cpp/atf/atf_cmdline.cpp)||
|[cpp/gen/atf_cmdline_gen.cpp](/cpp/gen/atf_cmdline_gen.cpp)||
|[include/atf_cmdline.h](/include/atf_cmdline.h)||
|[include/gen/atf_cmdline_gen.h](/include/gen/atf_cmdline_gen.h)||
|[include/gen/atf_cmdline_gen.inl.h](/include/gen/atf_cmdline_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_cmdline` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Tests
<a href="#tests"></a>
The following component tests are defined for `atf_cmdline`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|atf_cmdline.Bare||
|atf_cmdline.Debug||
|atf_cmdline.Help||
|atf_cmdline.Minimal||
|atf_cmdline.MinimalExec||
|atf_cmdline.Rich||
|atf_cmdline.RichExec||
|atf_cmdline.Sig||
|atf_cmdline.Verbose||
|atf_cmdline.Version||

