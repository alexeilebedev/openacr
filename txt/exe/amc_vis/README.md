## amc_vis - Draw access path diagrams


### Syntax
<a href="#syntax"></a>
```usage
amc_vis: Draw access path diagrams
Usage: amc_vis [[-ctype:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [ctype]     regx    "%"     Ctype regexp to compute access path diagram
    -in         string  "data"  Input directory or filename, - for stdin
    -dot        string  ""      Save dot file with specified filename
    -xref                       Include all ctypes referenced by selected ones
    -xns                Y       Cross namespace boundaries
    -noinput                    Deselect module inputs
    -check                      Check model for dependency problems
    -render             Y       Produce an ascii drawing
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

The single parameter to `amc_vis` is a ctype regex,
and whatever ctypes are matched by the regex will be shown
via ASCII art.

Here is an example:

    $ amc_vis amc.FCtype\|amc.FField

                  / amc.FCtype
                  |
                  |                   / amc.FField
                  |Ptrary c_field---->|
                  |Ptr c_pkeyfield--->|
                  |Ptrary c_datafld-->|
                  |Llist zd_inst----->|
                  |<------------------|Upptr p_arg
                  |Ptr c_optfld------>|
                  |<------------------|Upptr p_ctype
                  |Llist zd_access--->|
                  |Ptr c_varlenfld--->|
                  -                   |
                                      |
                                      -

amc_vis can also output an dot file, which can then be viewed in a browser:

    $ amc_vis -xref:N amc.FCtype\|amc.FField -dot xyz.dot
        amc_vis.dot  out_dot:xyz.dot  out_svg:xyz.svg

    $ firefox xyz.svg

### Options
<a href="#options"></a>
#### -ctype -- Ctype regexp to compute access path diagram
<a href="#-ctype"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -dot -- Save dot file with specified filename
<a href="#-dot"></a>

#### -xref -- Include all ctypes referenced by selected ones
<a href="#-xref"></a>

#### -xns -- Cross namespace boundaries
<a href="#-xns"></a>

#### -noinput -- Deselect module inputs
<a href="#-noinput"></a>

#### -check -- Check model for dependency problems
<a href="#-check"></a>

#### -render -- Produce an ascii drawing
<a href="#-render"></a>
