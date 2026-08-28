## amsspy - List ams sessions and monitor traffic on host


### Syntax
<a href="#syntax"></a>
```usage
amsspy: List ams sessions and monitor traffic on host
Usage: amsspy [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -session    regx    "%"     Session regex
    -list                       List sessions
    -shm                        List shms
    -spy        regx    ""      Spy on named shared memory segment
    -f          enum    decode  (output) Output format (decode|raw|text|json)
    -clean                      Unlink orphaned /dev/shm ams segments and exit
    -dump                       Dump every segment's header + reader offsets/lag (out-of-band) and exit
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

#### -session -- Session regex
<a href="#-session"></a>

#### -list -- List sessions
<a href="#-list"></a>

#### -shm -- List shms
<a href="#-shm"></a>

#### -spy -- Spy on named shared memory segment
<a href="#-spy"></a>

#### -f -- (output) Output format
<a href="#-f"></a>

#### -clean -- Unlink orphaned /dev/shm ams segments and exit
<a href="#-clean"></a>

#### -dump -- Dump every segment's header + reader offsets/lag (out-of-band) and exit
<a href="#-dump"></a>
