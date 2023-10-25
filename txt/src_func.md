## src_func: Access / edit functions



### Syntax

```
src_func: Access / edit functions
Usage: src_func [[-targsrc:]<regx>] [[-name:]<regx>] [[-body:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [targsrc]     regx    "%"     Visit these sources (accepts target name)
    [name]        regx    "%"     (with -listfunc) Match function name
    [body]        regx    "%"     (with -listfunc) Match function body
    -func         regx    "%"     (with -listfunc) Match function prototype
    -comment      regx    "%"     (with -listfunc) Match function comment
    -nextfile     string  ""      Print name of next srcfile in targsrc list
    -other                        (with -nextfile), name of previous file
    -updateproto                  Update prototypes in headers
    -listfunc                     Show functions in target
    -iffy                         Select functions that may contain errors
    -proto                        (with -listfunc) List prototypes only
    -gen                          (with -listfunc) Visit generated files
    -showloc              Y       (with -listfunc) Show file location
    -showstatic           Y       (with -listfunc) Show static functions
    -showsortkey                  (with -listfunc) Display function sortkey
    -sortname                     (with -listfunc) Sort functions by name
    -e                            (with -listfunc) Edit found functions
    -baddecl                      Report and fail on bad declarations
    -report
    -verbose      int             Verbosity level (0..255); alias -v; cumulative
    -debug        int             Debug level (0..255); alias -d; cumulative
    -help                         Print help an exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig

```

### Inputs

`src_func` takes the following tables on input:
```
CTYPE            COMMENT
dev.Target       Build target
dev.Targsrc      List of sources for target
dmmeta.Gstatic   Load entries for this table at startup time
dmmeta.Fstep     Generate a main loop step to be executed whenever a field is non-empty
dmmeta.Dispatch  Generate code for a multi-way branch
dmmeta.Ctypelen  Size of Ctype
```

