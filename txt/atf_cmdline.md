## atf_cmdline: Test tool for command line parsing



### Syntax

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
    -help                       Print help an exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Tests

The following component tests are defined for `atf_cmdline`:
```
atf_cmdline.Bare	
atf_cmdline.Debug	
atf_cmdline.Help	
atf_cmdline.Minimal	
atf_cmdline.MinimalExec	
atf_cmdline.Rich	
atf_cmdline.RichExec	
atf_cmdline.Sig	
atf_cmdline.Verbose	
atf_cmdline.Version	
```

