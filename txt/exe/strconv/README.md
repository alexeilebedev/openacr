## strconv - A simple string utility


### Syntax
<a href="#syntax"></a>
```usage
strconv: A simple string utility
Usage: strconv [-str:]<string> [options]
    OPTION         TYPE    DFLT    COMMENT
    [str]          string          String parameter
    -tocamelcase                   Convert string to camel case
    -tolowerunder                  Convert string to lower-under
    -in            string  "data"  Input directory or filename, - for stdin
    -pathcomp      string  ""      Extract path component from string
    -verbose       flag            Verbosity level (0..255); alias -v; cumulative
    -debug         flag            Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -str -- String parameter
<a href="#-str"></a>

#### -tocamelcase -- Convert string to camel case
<a href="#-tocamelcase"></a>

#### -tolowerunder -- Convert string to lower-under
<a href="#-tolowerunder"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pathcomp -- Extract path component from string
<a href="#-pathcomp"></a>
