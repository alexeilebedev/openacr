## src_hdr - Manage source copyright+license header in source files and scripts


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [src_hdr - Internals](/txt/gen/src_hdr/src_hdr.md)<br/>

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -targsrc -- Regx of targsrc to update
<a href="#-targsrc"></a>

#### -write -- Update files in-place
<a href="#-write"></a>

#### -indent -- Indent source files
<a href="#-indent"></a>

#### -update_copyright -- Update copyright year for current company
<a href="#-update_copyright"></a>

#### -scriptfile -- Regx of scripts to update header
<a href="#-scriptfile"></a>

### Inputs
<a href="#inputs"></a>
`src_hdr` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.copyright](/txt/ssimdb/dev/copyright.md)|Copyrighting entity|
|[dev.license](/txt/ssimdb/dev/license.md)|Source code license|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|Control code-generation and exception handling options for process/library|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
