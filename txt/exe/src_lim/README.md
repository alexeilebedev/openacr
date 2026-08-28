## src_lim - Enforce line length, function length, and indentation limits


### Syntax
<a href="#syntax"></a>
```usage
src_lim: Enforce line length, function length, and indentation limits
Usage: src_lim [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -linelim                    Check various file limits (line length, function length, etc)
    -srcfile    regx    "%"     Filter for source files to process
    -strayfile                  Check for unregistered source files
    -capture                    Generate new dev.linelim records
    -write                      Update ssim database (with -capture)
    -badchar                    Check for bad chars in source files
    -badline    regx    ""      Check badline (acr badline)
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

#### -linelim -- Check various file limits (line length, function length, etc)
<a href="#-linelim"></a>

#### -srcfile -- Filter for source files to process
<a href="#-srcfile"></a>

#### -strayfile -- Check for unregistered source files
<a href="#-strayfile"></a>

#### -capture -- Generate new dev.linelim records
<a href="#-capture"></a>

#### -write -- Update ssim database (with -capture)
<a href="#-write"></a>

#### -badchar -- Check for bad chars in source files
<a href="#-badchar"></a>

#### -badline -- Check badline (acr badline)
<a href="#-badline"></a>
