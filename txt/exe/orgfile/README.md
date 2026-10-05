## orgfile - Organize and deduplicate files by timestamp and by contents
<a href="#orgfile"></a>
orgfile sorts and deduplicates a set of files that you pipe to it by name.  It
moves each file to a path built from the file's name and date, and it deletes
files whose contents repeat a file seen earlier.  Every run prints its plan as
ssim tuples, and nothing on disk changes until you add `-commit`.

### Syntax
<a href="#syntax"></a>
```usage
orgfile: Organize and deduplicate files by timestamp and by contents
Usage: orgfile [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -move       string  ""      Read stdin, rename files based on pattern
    -dedup      regx    ""      Only allow deleting files that match this regx
    -commit                     Apply changes
    -undo                       Read previous orgfile output, undoing movement
    -hash       string  "sha1"  Hash command to use for deduplication
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

orgfile reads file names from stdin, one per line, and reads `dev.timefmt`
from the `-in` directory.  For each file it prints an `orgfile.move` or an
`orgfile.dedup` tuple that describes what it does, or would do, to that file.
With `-commit`, it carries the action out.

#### Moving files by name and date
<a href="#moving-files-by-name-and-date"></a>

`-move:<pattern>` gives each file a new path.  When the pattern ends in `/` or
names an existing directory, the file keeps its name and moves into that
directory.  Any other pattern is the whole new path.

orgfile expands the pattern in two passes.  The first replaces `$filename`,
`$basename`, `$ext` and `$dir` with the parts of the original path, where
`$basename` is the name without its extension and `$ext` keeps the dot.  The
second replaces `%Y`, `%m`, `%d` and `%b` with the year, month, day and month
name of the file's date.

The file's date comes from the first of these that yields one:

- the name of the directory holding the file, read with a `dev.timefmt` row
  marked `dirname:Y`;
- the file's own name, read with a `dev.timefmt` row marked `dirname:N`, such
  as the `PSX_%Y%m%d` format of Photoshop Express;
- the file's modification time.

When the new path already holds a file, orgfile compares the two hashes.
When the contents are the same, the incoming file replaces the old one, and
the tuple says `proven duplicate`.  When they differ, the old file stays, and
the incoming file takes a numbered name such as `a-2.txt`.  orgfile moves with
`mv`, creating directories as needed, so a move may cross file systems.

#### Deleting duplicates
<a href="#deleting-duplicates"></a>

`-dedup:<regx>` hashes every incoming file in input order.  A file whose path
matches the regx and whose hash equals that of an earlier file is a duplicate.
orgfile prints both paths, and with `-commit` it deletes the duplicate.  The
earlier file stays, whether or not its own path matches.

The `-hash` command computes the hash.  orgfile runs it with the file name as
its argument and keeps the text after the last `=` and before the next space,
which reads the output of both `sha1` and `sha1sum`.

#### Reading its own output
<a href="#reading-its-own-output"></a>

A line of input that names no existing file is read as an `orgfile.move` or
`orgfile.dedup` tuple.  That makes the plan of one run the input of the next,
with a `grep` or an edit between them.

For an `orgfile.move` tuple, orgfile moves `pathname` to `tgtfile` as written,
with no pattern expansion and no hash check.  `-undo` moves `tgtfile` back to
`pathname`.  For an `orgfile.dedup` tuple, orgfile deletes `duplicate`, and
under `-undo` it does nothing, since a deletion cannot be undone.

### Examples
<a href="#examples"></a>

```bash
find . -type f | orgfile -dedup:%                                   # list files whose contents repeat an earlier file
find backup backup2 -type f | orgfile -dedup:backup2/% -commit     # delete files in backup2 that backup already holds
find . -name "*.jpg" | orgfile -move:image/%Y/%Y-%m-%d/             # plan a move of photos into year and day directories
find . -name "*.jpg" | orgfile -move:image/%Y/%Y-%m-%d/ -commit     # carry that move out
find . -type f | orgfile -dedup:% | grep 'duplicate:./old/' | orgfile -commit   # delete only the duplicates under old/
orgfile -undo -commit < moves.log                                   # move files back, from the saved output of a -move run
```

A dry run of a move prints one tuple per file.  Here `a/f.txt` and `b/f.txt`
have the same contents, and `b/g.txt` was last modified in 2021:

```
$ find a b -type f | orgfile -move:out/%Y/%Y-%m-%d/ -hash:sha1sum
orgfile.move  pathname:a/f.txt  tgtfile:out/2026/2026-09-26/f.txt  comment:"move file"
orgfile.move  pathname:b/f.txt  tgtfile:out/2026/2026-09-26/f.txt  comment:"move file"
orgfile.move  pathname:b/g.txt  tgtfile:out/2021/2021-03-04/g.txt  comment:"move file"
```

### Caveats
<a href="#caveats"></a>

- A hash command that fails, or prints nothing, stops the run with
  `algo_lib.cmd_status_error` or `orgfile.nohash`, before orgfile acts on the
  file it was hashing.  Files earlier in the input have already been handled.
- The default `sha1` is a BSD command.  On Linux, the repo's `bin/sha1` wraps
  `sha1sum`, but the shell finds it only when you run orgfile from the repo
  root.  Anywhere else, pass `-hash:sha1sum`.
- A dry run checks each target against the disk as it stands.  Two files bound
  for the same path both read `move file` in the plan, as in the example
  above.  On `-commit`, the second one becomes a duplicate or takes a numbered
  name.
- `-dedup` with no value, or an empty value, deduplicates nothing.  Pass `%` to
  consider every file.
- `-move` takes precedence over `-dedup`, so a run given both only moves.
- The date that comes from a directory or a file name has no time of day.
  `%H`, `%M` and `%S` then expand to zeros, and they carry the real time only
  when the date comes from the modification time.
- Save the output of a `-commit` run if you may want `-undo` later.  orgfile
  keeps no log of its own.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Read `dev.timefmt` from this directory, which is `data` by default.  Point it
at a directory with your own `dev/timefmt.ssim` to recognize the dates your
camera or app writes into file names.

#### -move -- Read stdin, rename files based on pattern
<a href="#-move"></a>

Move each incoming file to the path this pattern expands to, as described
under [Moving files by name and date](#moving-files-by-name-and-date).  End the
pattern in `/` to keep file names.

#### -dedup -- Only allow deleting files that match this regx
<a href="#-dedup"></a>

Report each incoming file whose path matches this sql regx and whose contents
equal an earlier file's.  With `-commit`, delete it.

#### -commit -- Apply changes
<a href="#-commit"></a>

Carry out the moves and deletions the run prints.  Without it, orgfile only
prints its plan.

#### -undo -- Read previous orgfile output, undoing movement
<a href="#-undo"></a>

Reverse the `orgfile.move` tuples on stdin, moving each `tgtfile` back to its
`pathname`, and skip the `orgfile.dedup` tuples.  It affects only tuple input,
and it needs `-commit` to move anything.

#### -hash -- Hash command to use for deduplication
<a href="#-hash"></a>

The command that hashes a file, `sha1` by default; see the caveat about
running it outside the repo root.  Any command that prints
the hash after an `=`, or before the first space, works, such as `sha1sum` or
`md5sum`.
