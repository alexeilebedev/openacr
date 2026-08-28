## samp_make - sample program for Makefile management
<a href="#samp_make"></a>
This program is a sample program covered in [Tutorial 9: Transitioning from Makefile to OpenACR build](/txt/tut/tut09.md).
It parses a specified Makefile, produces all requisite ssimfiles, and generates Makefile(s) from the tables `ns:samp_make` lists.

### Syntax
<a href="#syntax"></a>
```usage
samp_make: sample program for Makefile management
Usage: samp_make [options]
    OPTION       TYPE    DFLT                              COMMENT
    -in          string  "data"                            Input directory or filename, - for stdin
    -target      regx    "%"                               Create Makefile for selected targets
    -parse_make                                            Parse extern/gnumake/Simple-Makefile
    -makefile    string  "extern/gnumake/Simple-Makefile"  (with parse_make) makefile to parse
    -write                                                 P(with parse_make) write ssimfiles, otherwise print them
    -verbose     flag                                      Verbosity level (0..255); alias -v; cumulative
    -debug       flag                                      Debug level (0..255); alias -d; cumulative
    -help                                                  Print help and exit; alias -h
    -version                                               Print version and exit
    -signature                                             Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -target -- Create Makefile for selected targets
<a href="#-target"></a>

#### -parse_make -- Parse extern/gnumake/Simple-Makefile
<a href="#-parse_make"></a>

#### -makefile -- (with parse_make) makefile to parse
<a href="#-makefile"></a>

#### -write -- P(with parse_make) write ssimfiles, otherwise print them
<a href="#-write"></a>
