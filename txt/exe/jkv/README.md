## jkv - JSON <-> key-value mapping tool
<a href="#jkv"></a>
In regular mode, input (file or stdin) is read as JSON and converted to a series of lines.
Recursively, all fields in the JSON file are given a name. The output is printed 
to stdout. Before printing, the in-memory object is updated by applying kvs from the command line
before writing it back. With -r (reverse), the input is a list of key-values and the
output is JSON. A filename of "-" means stdin. With `-write`, the input file is updated in-place.
Default output format can be overridden with `-output`.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [jkv - Internals](/txt/exe/jkv/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
jkv: JSON <-> key-value mapping tool
Usage: jkv [-file:]<string> [[-kv:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [file]      string          Filename (use - for stdin)
    [kv]...     string          JSON Keyvals
    -r                          Reverse (json keyvals -> JSON) mapping
    -write                      Write the modified file back
    -output     enum    auto    Output format (auto|json|kv)
                                    auto  Automatic
                                    json  Json only
                                    kv  Key-value only
    -pretty     int     2       Pretty-printer (0-compact; 1=algo style; 2=jq style)
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Examples
<a href="#examples"></a>
#### Update values in a JSON file
<a href="#update-values-in-a-json-file"></a>

```
jkv .vscode/launch.json -write configurations-0.program::myprog .args-0::-arg1 ::-arg2 ::-arg3 
```

#### Rename fields in a JSON file
<a href="#rename-fields-in-a-json-file"></a>
```
jkv X.json | sed 's/^attr:/^attr2:/' | jkv -r > tempfile
mv tempfile X.json
```

#### Sort field names in a JSON file
<a href="#sort-field-names-in-a-json-file"></a>
```
jkv X.json | sort -t ':' -k1,1
```

#### Merge two JSON files
<a href="#merge-two-json-files"></a>
```
(jkv A.json; jkv B.json) | jkv -r -
```

#### Select an object from a JSON file
<a href="#select-an-object-from-a-json-file"></a>
```
jkv X.json | grep '^a\.b\.c\.' | jkv -r -
```

#### Construct JSON object from command line
<a href="#construct-json-object-from-command-line"></a>

```
inline-command: jkv /dev/null -r a:true b:false c:null d:[] :3 :{} $'f::line1\nline2' e.g::h
{
    "a": true,
    "b": false,
    "c": null,
    "d": [
        3,
        {}
    ],
    "f": "line1\nline2",
    "e": {
        "g": "h"
    }
}
```

#### Set array element by index
<a href="#set-array-element-by-index"></a>

```
inline-command: jkv /dev/null -r -pretty:0 -- a.b.c-0:true -10:false
{"a":{"b":{"c":[true,null,null,null,null,null,null,null,null,null,false]}}}
```

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -file -- Filename (use - for stdin)
<a href="#-file"></a>

#### -kv -- JSON Keyvals
<a href="#-kv"></a>
Key-value pair. This has the form `<key>:<value>`
`<value>` is a JSON expression, so it could be `null`, a number, `{}`, `[]`, `true`, `false` or a quoted string.
But if `<value>` starts with `:` then the remainder of the value is a JSON string as if there were quotes around it.
I.e. `value::<blah>` is exactly the same as `value:"blah"` but a lot more convenient for use on UNIX command line.
Notice the string must be fully escaped according to JSON rules.

If key starts with `.` or `-`, then it shares a prefix with the most recently seen key. So, specifying `a.b.c:true .d:false`
is the same as specifying `a.b.c:true a.b.d:false`. Specifying `a.b.c-0:true -10:false`
is the same as specifying `a.b.c-0:true a.b.c-10:false` and will result in a JSON array with 11 elements, 9 of which are null.

If key is empty, then it is taken to tbe the last array index plus one. So, `a.b.c-0:true :false ::x`
is the same as specifying `a.b.c-0:true a.b.c-1:false a.b.c-2::x`

#### -r -- Reverse (json keyvals -> JSON) mapping
<a href="#-r"></a>

Read input filename (- for stdin), with one key-value per line.
By default, output a JSON object (may be overridden with `-output`)

#### -write -- Write the modified file back
<a href="#-write"></a>

Write output back to the file from which it was read.

#### -output -- Output format
<a href="#-output"></a>

Select output format. Default (`auto`) is to perform a mapping either to or from kv format,
depending on input.

#### -pretty -- Pretty-printer (0-compact; 1=algo style; 2=jq style)
<a href="#-pretty"></a>

Default value (2) uses jq-style pretty printer.
Value 0 means compact output with no newlines.
Value 1 is algo-style pretty printer (comma at the beginning of the new line)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`jkv` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

