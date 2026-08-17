## amc Backend: output layout
<a href="#amc-backend-output-layout"></a>

How amc organizes generated files on disk, and the schema of the
one-line report it prints on exit.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [File layout](#file-layout)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Per-namespace files](#per-namespace-files)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Filemod detection](#filemod-detection)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Report schema](#report-schema)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Exit codes](#exit-codes)<br/>

### File layout
<a href="#file-layout"></a>

*To be written.*  Generated trees:
- `cpp/gen/<ns>_gen.cpp` — function bodies
- `include/gen/<ns>_gen.h` — declarations
- `include/gen/<ns>_gen.inl.h` — inline bodies

Source: `cpp/amc/outfile.cpp`.

### Per-namespace files
<a href="#per-namespace-files"></a>

*To be written.*  Each `amc::FNs` carries open file handles for
its three output files.

### Filemod detection
<a href="#filemod-detection"></a>

*To be written.*  amc writes to a temp file and only renames if
the contents actually changed; `n_filemod:N` in the report counts
the files that were updated.

### Report schema
<a href="#report-schema"></a>

*To be written.*  Document every field of the `report.amc` line:

    report.amc  n_cppfile:123  n_cppline:258301  n_ctype:970  n_func:22524  n_xref:460  n_filemod:0

### Exit codes
<a href="#exit-codes"></a>

*To be written.*
