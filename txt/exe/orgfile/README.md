## orgfile - Organize and deduplicate files by timestamp and by contents


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading Output As Input](#reading-output-as-input)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Find files with identical contents (but don't do anything with them)](#find-files-with-identical-contents--but-don-t-do-anything-with-them-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Same as above but with an additional filter](#same-as-above-but-with-an-additional-filter)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Delete files in secondary backup that already exist in primary backup](#delete-files-in-secondary-backup-that-already-exist-in-primary-backup)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Organize images by year and day](#organize-images-by-year-and-day)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [orgfile - Internals](/txt/exe/orgfile/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
orgfile: Organize and deduplicate files by timestamp and by contents
Usage: orgfile [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -move       string  ""      Read stdin, rename files based on pattern
    -dedup      regx    ""      Only allow deleting files that match this regx
    -commit                     Apply changes
    -undo                       Read previous orgfile output, undoing movement
    -hash       string  "sha1"  Hash command to use for deduplication
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

Orgfile reads a list of filenames from stdin and performs move and dedup operations with
incoming files. 
This tool is based on openacr and can be merged into any clone of openacr, or compiled stand-alone
with `make`.

If `-commit` is specified, the operation is performed. Otherwise, the description of intended
operation is printed to stdout and no action occurs.

If `-move` is specified, then each incoming file is moved to directory specified by `-move`, if that
argument ends with a /. Otherwise, the file is renamed.

If the target file exists and has the same checksum, as determined by the checksum algorithm, the source file is deleted.
If the target file exists, a suffix "-2", "-3", etc is appended 
to the incoming file's basename (so, `a.txt` becomes `a-2.txt`).

When moving, orgfile treats the target filename as an expression, where the following substitutions occur.
First, $basename, $filename, $ext are replaced with corresponding path components of the original file.
Second, orgfile determines a date associated with the file and replaces %Y, %b, %m and %d in the target pathname
with the discovered date.

The date is discovered using the following algorithm:
- First, the parent directory of the file is checked for any pattern specified in table `dev.timefmt` marked `dirname:Y`.
(For instance, `"%Y-%m-%d"`)
- If this doesn't yield a timestamp, then the filename is checked for any pattern from `timefmt` table marked `dirname:N`.
Any number of patterns can be provided in the table. Default timefmts support photoshop image name formats.
- If this doesn't yield a timestamp, then the file modification time is used.
Note, use of formatting specifiers other than `%Y,%m,%b,%d` for `-subdir` may
yield zeros.

The parameter `-hash` specifies the command to use to compute file hashes. It is `sha1` by default, but can
be the name of any command. The output of the command is filtered (removing spaces and everyhing before `=` sign),
and the rest is taken to the a file checksum for the purposes of determining duplicates (this works both
with `-move` and with `-dedup`)

If `-dedup` is specified, then any incoming file is scanned for its hash.
Any file whose pathname matches the pattern specified by `-dedup`, and the file is proven to be a duplicate,
is deleted.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Reading Output As Input
<a href="#reading-output-as-input"></a>

Orgfile can accept its own output as input. This is useful for inserting filters such as grep.
Aside from filenames, orgfile input can contain tuples that start with orgfile.move and orgfile.dedup.
For orgfile.move, orgfile either performs the raw filesystem move (with no evaluation or deduplication), or, with 
`-undo`, moves the target file back to the source file.
For orgfilededup, orgfile deletes the duplicate file. With `-undo -dedup`, orgfile does nothing
since deletion cannot be undone.
For some move operations, `-undo` will move files back to their original locations.

### Find files with identical contents (but don't do anything with them)
<a href="#find-files-with-identical-contents--but-don-t-do-anything-with-them-"></a>

```
find . | orgfile -dedup
```

### Same as above but with an additional filter
<a href="#same-as-above-but-with-an-additional-filter"></a>

```
find . | orgfile -dedup | grep <blah> | orgfile -commit
```

### Delete files in secondary backup that already exist in primary backup
<a href="#delete-files-in-secondary-backup-that-already-exist-in-primary-backup"></a>

```
find backup backup2 -type f | orgfile -dedup:"backup2/%" -commit
```

### Organize images by year and day
<a href="#organize-images-by-year-and-day"></a>

```
find . -name "*.jpg" | orgfile -move:image/%Y/%Y-%m-%d/ -commit
```

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -move -- Read stdin, rename files based on pattern
<a href="#-move"></a>

#### -dedup -- Only allow deleting files that match this regx
<a href="#-dedup"></a>

#### -commit -- Apply changes
<a href="#-commit"></a>

#### -undo -- Read previous orgfile output, undoing movement
<a href="#-undo"></a>

#### -hash -- Hash command to use for deduplication
<a href="#-hash"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`orgfile` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.timefmt](/txt/ssimdb/dev/timefmt.md)|Time formats supported by orgfile|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

