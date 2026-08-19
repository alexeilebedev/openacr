## amc_vis - Draw access path diagrams


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [amc_vis - Internals](/txt/gen/amc_vis/amc_vis.md)<br/>

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

### Inputs
<a href="#inputs"></a>
`amc_vis` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|Describe input table of a program|
|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|Field type constructor (e.g. reference type)|
