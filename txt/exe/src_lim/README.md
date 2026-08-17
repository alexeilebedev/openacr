## src_lim - Enforce line length, function length, and indentation limits


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [src_lim - Internals](/txt/gen/src_lim/src_lim.md)<br/>

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

### Inputs
<a href="#inputs"></a>
`src_lim` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.badline](/txt/ssimdb/dev/badline.md)|Regex of a dubious source code line|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.include](/txt/ssimdb/dev/include.md)|A site where one file includes another|
|[dev.linelim](/txt/ssimdb/dev/linelim.md)||
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
