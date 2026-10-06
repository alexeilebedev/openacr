## gcache - Compiler cache
<a href="#gcache"></a>

gcache is a compiler cache.  It wraps a compile command, and when it has seen
the same compile before it copies the stored object file into place and skips
the compiler.  [abt](/txt/exe/abt/README.md) runs every compile through gcache
once the cache is enabled in the checkout.

### Syntax
<a href="#syntax"></a>
```usage
gcache: Compiler cache
Usage: gcache [[-cmd:]<string>] [options]
    OPTION      TYPE    DFLT           COMMENT
    -in         string  "data"         Input directory or filename, - for stdin
    [cmd]...    string                 Command to execute
    -install                           Create gcache directory and enable gcache
    -stats                             Show cache stats
    -enable                            Create .gcache link to enable gcache use
    -disable                           Remove .gcache link to disable gcache
    -gc                                Clean old files from .gcache
    -clean                             Clean the entire cache
    -dir        string  "/tmp/gcache"  (With -install,-enable) cache directory
    -maxmb      int     10240          Cache size budget in MB; GC evicts oldest entries past it
    -hitrate                           Report hit rate (specify start time with -after)
    -after      string                 Start time for reporting
    -report                            Show end-of-run report
    -force                             Force recompile and update cache
    -verbose    flag                   Verbosity level (0..255); alias -v; cumulative
    -debug      flag                   Debug level (0..255); alias -d; cumulative
    -trace      string  ""             Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Enabling the cache
<a href="#enabling-the-cache"></a>

The cache lives in a directory, `/tmp/gcache` by default.  A checkout uses it
through a link named `.gcache` in its top directory, and abt uses gcache
whenever that link exists.  `gcache -install` creates the directory, makes it
writable by the group, and creates the link.  `gcache -enable` creates the link
to a directory that already exists, and `gcache -disable` removes it.  One cache
directory can serve every checkout and every user on the machine.

#### Caching a compile
<a href="#caching-a-compile"></a>

Put `gcache --` in front of a compiler command line.  gcache caches a command
that compiles one source file to an object, which is a command with `-c` and
`-o`, and runs any other command unchanged.

For a cacheable command, gcache first runs the preprocessor, and then hashes
the command line together with the preprocessed text.  A coverage compile adds
the directory it runs in to the hash.  When the cache holds an entry under that
hash, gcache copies it to the `-o` path.  Otherwise gcache compiles the
preprocessed text and stores the object under the hash.  A coverage compile
stores its object and its `.gcno` notes as one entry, and a hit restores both.

#### Precompiled headers
<a href="#precompiled-headers"></a>

gcache can precompile a header once and reuse it across every source file that
includes it.  Mark the header by putting this line anywhere in it:

```c++
void __gcache_pragma_pch_preprocess();
```

The line is a function prototype that nothing defines, so the compiler accepts
it and gcache finds it in the preprocessed text.  gcache precompiles the marked
header, stores the result in the cache, and makes the compile read the
precompiled header in place of the header's text.

A marked header qualifies only when no code comes before it, in the source file
and in every header on the way to it.  Preprocessor lines and comments are
allowed, so include the header first.  A source file uses at most one
precompiled header, the last marked header that qualifies.  In this tree
`include/algo.h` carries the marker.

#### The cache directory
<a href="#the-cache-directory"></a>

An entry is a file named after the hex SHA-1 hash, two levels deep:
`ab/cd/abcd...`.  A precompiled header adds a `.gch` file and a `.lock` file
beside its hash.  `log.ssim` holds one `report.gcache` line per compile, naming
the source file, the entry, and whether it was a hit.  `gc.time` records the
last cleanup.

#### Cleanup
<a href="#cleanup"></a>

gcache cleans the cache on its own once a day, during the first gcache command
that runs after the day is up.  The cleanup drops log lines older than two days,
keeps every entry a remaining log line names, and deletes other entries older
than a week.  Then it deletes the least recently used entries until the cache
fits in `-maxmb`.

### Examples
<a href="#examples"></a>

```bash
gcache -install                         # create /tmp/gcache and enable it in this checkout
gcache -install -dir:$HOME/.gcache      # keep a private cache in the home directory
gcache -enable -dir:/tmp/gcache         # enable an existing cache in another checkout
gcache -disable                         # stop abt from using the cache here
gcache -stats                           # show the cache's file count and size
gcache -hitrate -after:2026-09-26T00:00:00   # show the hit rate since midnight
gcache -gc                              # run the cleanup now
gcache -clean                           # empty the cache
gcache -report -- g++ -c x.cpp -o x.o   # compile through the cache and print the report line
tail -f .gcache/log.ssim                # watch compiles as they happen
```

To list every cache entry the compiles of a target used, run this.

```bash
abt <target> -force -v -v 2>&1 | ssimfilt report.gcache -field:cached_file -format:field
```

`-hitrate` counts the hits in the log.

```
$ gcache -hitrate -after:2026-09-26T00:00:00
report.gcache_hitrate  hitrate:45%  pch_hitrate:54%
```

### Caveats
<a href="#caveats"></a>

- The default cache in `/tmp/gcache` is readable and writable by every user on
  the machine, and its log names your source files.  Use
  `gcache -install -dir:$HOME/.gcache` for a private one.
- A coverage entry serves only the checkout that wrote it, so a second checkout
  of the same commit misses on its first coverage build.
- The exit code is the compiler's exit status, plus one for each failure that
  kept gcache from delivering the object.  A failure to store an entry only
  costs a later miss, so gcache prints a `gcache.warning` and leaves the exit
  code alone.
- A marked header that fails to precompile prints a `gcache.warning`, and the
  compile goes ahead without it.
- abt passes its verbosity to gcache minus one level, so `abt -v -v` shows
  gcache's report lines and subcommands.
- The daily cleanup can run inside any gcache command, including an ordinary
  compile in the middle of a build.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

gcache accepts this flag and reads nothing from it.

#### -cmd -- Command to execute
<a href="#-cmd"></a>

The compile command to run through the cache, given after `--`.  gcache caches
a command that compiles one file with `-c` and `-o`, and runs any other command
as it is.

#### -install -- Create gcache directory and enable gcache
<a href="#-install"></a>

Create the cache directory named by `-dir`, make it and your files in it
writable by the group, and then do what `-enable` does.  Run it again on an
existing cache to repair its permissions.  gcache prints `done` once every step
has succeeded.

#### -stats -- Show cache stats
<a href="#-stats"></a>

Print the cache directory, the number of files in it, and its size on disk.
When the checkout has no `.gcache` link, gcache shows the directory named by
`-dir`.

#### -enable -- Create .gcache link to enable gcache use
<a href="#-enable"></a>

Point the `.gcache` link in the current directory at the directory named by
`-dir`.  When that directory does not exist, gcache says so and leaves the link
as it was.

#### -disable -- Remove .gcache link to disable gcache
<a href="#-disable"></a>

Remove the `.gcache` link, so abt compiles without the cache.  The cache
directory and its entries stay.

#### -gc -- Clean old files from .gcache
<a href="#-gc"></a>

Run the cleanup now and print a report of what it removed.  The cleanup is the
one gcache otherwise runs once a day.

#### -clean -- Clean the entire cache
<a href="#-clean"></a>

Delete every entry in the cache and print the report.  A precompiled header
that a running compile holds is kept.

#### -dir -- (With -install,-enable) cache directory
<a href="#-dir"></a>

The cache directory for `-install` and `-enable`, `/tmp/gcache` by default.
Compiles and the cleanup use the directory the `.gcache` link names.

#### -maxmb -- Cache size budget in MB; GC evicts oldest entries past it
<a href="#-maxmb"></a>

The size the cleanup holds the cache to, in megabytes.  The default is 10240.
The cleanup deletes the least recently used entries until the cache fits, so
the cache can grow past the budget between cleanups.

#### -hitrate -- Report hit rate (specify start time with -after)
<a href="#-hitrate"></a>

Print the share of compiles since `-after` that were cache hits, and the share
that used a cached precompiled header.  abt runs this after each build to fill
in the hit rate of its report.

#### -after -- Start time for reporting
<a href="#-after"></a>

The start of the period `-hitrate` reports on, as a time such as
`2026-09-26T00:00:00`.  Leave it out to count every compile in the log, which
the cleanup trims to the last few days.

#### -report -- Show end-of-run report
<a href="#-report"></a>

Print the `report.gcache` line for this compile: the source file, the entry,
whether it hit, and the time it took.  gcache writes the same line to the log
for every cacheable compile.

#### -force -- Force recompile and update cache
<a href="#-force"></a>

Compile even when the cache holds an entry, and replace the entry with the new
result.  A marked header is precompiled again too.  `abt -cache:gcache-force`
passes this flag to every compile.
