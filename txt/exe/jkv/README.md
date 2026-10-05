## jkv - JSON <-> key-value mapping tool
<a href="#jkv"></a>

jkv converts a JSON document into one line per value, and converts such lines back into
JSON.  The line form suits `grep`, `sed` and `sort`, so a JSON file can be searched and
edited with ordinary shell tools.  jkv can also set values from the command line and write
the file back in place.

### Syntax
<a href="#syntax"></a>
```usage
jkv: JSON <-> key-value mapping tool
Usage: jkv [-file:]<string> [[-kv:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [file]      string          Filename (use - for stdin)
    [kv]...     string          JSON Keyvals
    -r                          Reverse (json keyvals -> JSON) mapping
    -write                      Write the modified file back
    -output     enum    auto    Output format (auto|json|kv)
    -pretty     int     2       Pretty-printer (0-compact; 1=algo style; 2=jq style)
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

jkv reads the input file, or stdin when the file is `-`, and builds the document in
memory.  It then applies each key-value from the command line, and prints the result.

In the default mode the input is JSON and the output is key-value lines.  Each line names
one value by its path: an object field adds `.name`, and an array element adds `-index`.
A string value follows `::`, and any other value follows `:` as JSON.

```
a.b:1
a.c-0:1
a.c-1::x
d:true
```

With `-r` the input is key-value lines and the output is JSON.  Lines read with `-r` follow
the same rules as key-values on the command line, described under `-kv`.

### Examples
<a href="#examples"></a>

```bash
jkv X.json                                  # print X.json as key-value lines
jkv X.json a.b:2 -output:json               # print X.json with a.b set to 2
jkv X.json a.b:2 -write                     # set a.b to 2 in the file itself
jkv X.json | grep '^a\.b\.c\.' | jkv -r -   # extract the object a.b.c
```

#### Update values in a JSON file
<a href="#update-values-in-a-json-file"></a>

```bash
jkv .vscode/launch.json -write configurations-0.program::myprog .args-0::-arg1 ::-arg2 ::-arg3
```

#### Rename fields in a JSON file
<a href="#rename-fields-in-a-json-file"></a>
```bash
jkv X.json | sed 's/^attr:/attr2:/' | jkv -r - > tempfile
mv tempfile X.json
```

#### Sort field names in a JSON file
<a href="#sort-field-names-in-a-json-file"></a>
```bash
jkv X.json | sort -t ':' -k1,1 | jkv -r -
```

#### Merge two JSON files
<a href="#merge-two-json-files"></a>

A value from `B.json` replaces the value at the same path in `A.json`.

```text
(jkv A.json; jkv B.json) | jkv -r -
```

#### Select an object from a JSON file
<a href="#select-an-object-from-a-json-file"></a>
```bash
jkv X.json | grep '^a\.b\.c\.' | jkv -r -
```

#### Construct JSON object from command line
<a href="#construct-json-object-from-command-line"></a>

```bash
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

```bash
inline-command: jkv /dev/null -r -pretty:0 -- a.b.c-0:true -10:false
{"a":{"b":{"c":[true,null,null,null,null,null,null,null,null,null,false]}}}
```

### Caveats
<a href="#caveats"></a>

- A field name that contains `.` or `-` does not survive the round trip.  `{"a-b":1}`
  prints as `a-b:1`, and reading that line back yields `{"a":[1]}`.
- jkv reports no error for input that is not valid JSON.  It keeps what it parsed before
  the error, so `-write` on a broken file can silently drop the rest of it.
- A key-value on the command line that starts with `-` needs a `--` somewhere before it.
  Without one it never reaches the document, and jkv reports no error.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

jkv reads no ssim data, so this option has no effect.  The file to convert is `-file`.

#### -file -- Filename (use - for stdin)
<a href="#-file"></a>

The file to read, usually given as the first positional argument.  Use `-` to read stdin,
and `/dev/null` to start from an empty document.

#### -kv -- JSON Keyvals
<a href="#-kv"></a>

A key-value pair of the form `<key>:<value>`, applied to the document after it is read.
`<value>` is a JSON expression: `null`, a number, `{}`, `[]`, `true`, `false` or a quoted
string.  A value that starts with `:` is a string without the quotes, so `value::blah` is
the same as `value:"blah"` and much easier to type in a shell.  The string must still be
escaped by JSON rules.

A key that starts with `.` or `-` shares a prefix with the key before it.  So
`a.b.c:true .d:false` is the same as `a.b.c:true a.b.d:false`.  And `a.b.c-0:true
-10:false` is the same as `a.b.c-0:true a.b.c-10:false`, which makes an array of 11
elements, 9 of them null.

An empty key means the previous array index plus one.  So `a.b.c-0:true :false ::x` is the
same as `a.b.c-0:true a.b.c-1:false a.b.c-2::x`.

#### -r -- Reverse (json keyvals -> JSON) mapping
<a href="#-r"></a>

Reads the input as key-value lines, one per line, and prints JSON.  `-output` can choose
the other format.

#### -write -- Write the modified file back
<a href="#-write"></a>

Writes the result back to the input file in the format it was read in, so a JSON file
stays JSON.  With `-` as the file, the result goes to stdout.

#### -output -- Output format
<a href="#-output"></a>

`json` or `kv` fixes the output format.  The default, `auto`, prints the opposite of the
input format, or the same format with `-write`.

#### -pretty -- Pretty-printer (0-compact; 1=algo style; 2=jq style)
<a href="#-pretty"></a>

Sets the JSON layout.  The default, 2, indents the way `jq` does.  0 prints everything on
one line, and 1 puts each comma at the start of a new line.
