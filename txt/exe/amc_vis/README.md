## amc_vis - Draw access path diagrams
<a href="#amc_vis"></a>
`amc_vis` draws the access paths between ctypes: which record reaches which, and
through what kind of field.  It reads the same ssim schema `amc` generates from,
so the picture shows the in-memory database a namespace will have.  It also
checks the schema for circular dependencies, which CI runs over the whole tree.

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
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Drawing the diagram
<a href="#drawing-the-diagram"></a>

Give `amc_vis` a ctype regx, and it draws each matching ctype as a column that
opens with `/ <ctype>` and closes with `-`.  Each field that points from one
selected ctype to another becomes an arrow labeled with the field's reftype and
name.  An `Upptr` or `Pkey` field points back at the record it refers to, so
its arrow runs from the child column to the parent.  A field that holds its target
by value, such as a `Val`, draws its arrow into the top of the target's column.

`amc_vis` reads `dmmeta.ctype`, `dmmeta.field`, `dmmeta.finput` and
`dmmeta.reftype`.  In a terminal it colors the ctypes the regx matched, and it
pipes a drawing too big for the screen through `less`.

#### Saving a Graphviz file
<a href="#saving-a-graphviz-file"></a>

`-dot:<file>` writes the same graph as a Graphviz `.dot` file, and then runs
`dot` to render an `.svg` next to it.  Open the `.svg` in a browser for a
diagram too large for a terminal.

#### Checking for circular dependencies
<a href="#checking-for-circular-dependencies"></a>

`-check` lays the ctypes out and reports every cycle of references among them,
without drawing.  A cycle prints its chain of fields and then an
`amc_vis.circular_dependency` line, and `amc_vis` exits 1.  The fix is usually
to turn one `Upptr` in the cycle into a `Ptr`.  The `normalize_amc_vis` citest
runs `amc_vis -check` over every ctype.

[acr_ed](/txt/exe/acr_ed/README.md) runs `amc_vis` over the records it proposes
and prints the drawing beside them, so a schema change shows its shape before
you write it.  [doc](/txt/exe/doc/README.md) shows the same drawing under `-vis`.

### Examples
<a href="#examples"></a>

```bash
amc_vis 'dmmeta.Ns|dmmeta.Ctype'                # the access paths between two ctypes
amc_vis dmmeta.Field                            # one ctype, with everything its fields reach
amc_vis dmmeta.Field -xns:N                     # the same, staying inside dmmeta
amc_vis 'amc.FCtype|amc.FField' -xref:N         # two ctypes and nothing else
amc_vis amc.% -dot:/tmp/amc.dot -render:N       # write amc.dot and amc.svg for a whole namespace
amc_vis % -check                                # look for circular dependencies across the tree
```

Two ctypes of `dmmeta`, where each ctype names its namespace through a `Pkey`:

```
$ amc_vis 'dmmeta.Ns|dmmeta.Ctype'
                        / dmmeta.Ctype
           / dmmeta.Ns  |
           |<-----------|Pkey ns
           |            -
           -
```

The in-memory side of `amc`, where `Upptr` fields point back at the parent:

```
$ amc_vis 'amc.FCtype|amc.FField' -xref:N
                 / amc.FCtype
                 |                     / amc.FField
                 |Ptrary c_field------>|
                 |Llist zd_varlenfld-->|
                 |Llist zd_inst------->|
                 |Llist zd_access----->|
                 |<--------------------|Upptr p_ctype
                 |Ptrary c_datafld---->|
                 |<--------------------|Upptr p_arg
                 |Ptr c_pkeyfield----->|
                 |Ptr c_optfld-------->|
                 -                     |
                                       -
```

### Caveats
<a href="#caveats"></a>

- A regx that matches one ctype turns on `-xref`, so the drawing grows to every
  ctype its fields reach.  Match two or more ctypes, or pass `-xref:N` with
  them, to see only the ones you named.
- `-dot` needs Graphviz installed to produce the `.svg`.  Without it `amc_vis`
  still writes the `.dot` file.
- `-in:-` reads the tables from stdin, and the input has to carry every ctype
  and reftype the fields name.  A partial input fails with
  `amc_vis.bad_xref`.

### Options
<a href="#options"></a>
#### -ctype -- Ctype regexp to compute access path diagram
<a href="#-ctype"></a>

The ctypes to draw, as an SQL regx such as `dmmeta.%` or `amc.FCtype|amc.FField`.
The default `%` selects every ctype in the schema.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Where to read the schema tables, `data` by default.  `-in:-` reads them from
stdin, which is how `acr_ed` draws records that are not yet written.

#### -dot -- Save dot file with specified filename
<a href="#-dot"></a>

Write the graph to this Graphviz file and render an `.svg` beside it with the
same base name.  Combine it with `-render:N` to skip the terminal drawing.

#### -xref -- Include all ctypes referenced by selected ones
<a href="#-xref"></a>

Add to the selection every ctype a selected ctype's fields name, and the `FDb`
of the selected ctype's namespace.  It turns on by itself when the regx matches
a single ctype; pass `-xref:N` to keep the selection as matched.

#### -xns -- Cross namespace boundaries
<a href="#-xns"></a>

Let `-xref` pull in ctypes from other namespaces, on by default.  With
`-xns:N`, `-xref` adds only ctypes from the same namespace as the one that names
them.

#### -noinput -- Deselect module inputs
<a href="#-noinput"></a>

Drop from the selection every ctype that some namespace loads as an input
table, a `dmmeta.finput`.  Use it to see the structure a namespace builds
without the ssim tables it reads.

#### -check -- Check model for dependency problems
<a href="#-check"></a>

Report circular dependencies among the selected ctypes and exit 1 when there
are any.  It turns `-render` off, so it prints only the cycles it finds.

#### -render -- Produce an ascii drawing
<a href="#-render"></a>

Print the ASCII drawing, on by default.  Pass `-render:N` when only the `-dot`
file is wanted.
