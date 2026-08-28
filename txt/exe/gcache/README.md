## gcache - Compiler cache
<a href="#gcache"></a>

GCACHE is compiler cache paired with ABT.  When enabled, ABT automatically
detects and uses it.

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
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig
```

### Enable and disable
<a href="#enable-and-disable"></a>

The tool needs cache directory `.gcache`.

A number of scripts facilitates managing this directory:

- **gcache -install** - set up `/tmp/gcache` storage (with `-dir` option, this location can be customized)
- **gcache -enable** - create softlink `.gcache` pointing to cache directory (with `-dir` option, a different location can be customized)
- **gcache -disable** - delete softlink `.gcache`, disabling caching
- **gcache -stats** - show cache stats
- **gcache -gc** - force garbage collection
- **gcache -clean** - remove the entire cache

To enable gcache, use:

```bash
gcache -install
```

To disable gcache, use:

```bash
gcache -disable
```

Or remove the softlink `.gcache`.

### Invocation
<a href="#invocation"></a>

**bin/gcache** is tool executable, which shall be put in front of compiler
command, for example:

```bash
gcache -- g++ -c sample.cpp -o sample.o
```

First run invokes the compiler, and stores resulting object file under
hash in the cache directory.

On subsequent runs, the same command will copy saved object file to target location.

A saved object file is copied to the target only when the cache holds it as a
regular file with bytes in it.  A path under the cache directory can hold
something else: an entry of no bytes, which is what a build that ran out of disk
leaves behind, or a directory, which an earlier version of the cache wrote where
this one writes a single file.  Neither can produce the object the run was asked
for, and a run served from the cache writes nothing back to it, so such a path
would answer every later build the same way.  A run that finds one compiles
instead, and the publish that follows replaces whatever stood at the path.

The exit code sums what the run has to report.  The wrapped command's own exit
status is one term of the sum.  A failure that keeps the run from delivering the
object it was asked for adds one more: a cache directory that does not exist, the
`-install` marker that cannot be written, a cached file that cannot be written to
its target.  A run whose command exits 1 beside one such failure exits 2, so
neither of the two hides the other.

A failure that costs only a future cache miss is reported and adds nothing.
Publishing an entry is that case: the compile the entry came from has already
produced the object the build asked for, and a read-only cache directory or a
full disk takes nothing away from it.  Such a run prints a `gcache.warning` line
naming the file and the errno, and exits 0.  So the exit code answers whether the
run delivered its object, and it is not a count of the lines gcache printed.

### Precompiled header support
<a href="#precompiled-header-support"></a>

The tool may precompile and use precompiled headers.
The following line is used to mark the header as eligible to be precompiled.

```c++
void __gcache_pragma_pch_preprocess();
```

This line is in form of function prototype, although there is no such real function.

The reasons are as follows:

- gcache analyzes already preprocessed source;
- function prototype may be repeated without any harm;

The marker may be put in any place of header file.
Marked header may be included to compilation unit directly
or indirectly via one or mode parent header files.
Main rule is: no any token shall be placed before this header at
toplevel file and any of parent includes.
Preprocessor directives and commants are OK.
Best practice - include this header first.

Note that only one header may be preprocessed per translation unit.
If tool meets the marker more than once, the latest occurrence is taken,
but only if there is no any token before the header.

Gcache extracts header text from preprocessed source, precompiles it,
and put result to cache, to be reused by other translation units.
To use precompiled header, gcache edits preprocessed source replacing
header text by `#pragma GCC pch_preprocess "cached_pch_file.gch"`,
putting it at first position, above all other headers.

### Hash algorithm
<a href="#hash-algorithm"></a>

To compute hash, the tool invokes compiler to preprocess the source, then
hashes command line with preprocessed source file using SHA-1 algorithm.

### Directory structure
<a href="#directory-structure"></a>

Object files are stored under three-level directory structure:

*AB*/*CD*/*ABCD...*

where:
- *AB* - first two hex digits of the hash;
- *CD* - next two hex digits of the hash;
- *ABCD...* - full hex string of hash;

### Garbage collection
<a href="#garbage-collection"></a>

Tool automatically performs garbage collection daily.  The file
`gc.time` serves this task, its mtime is the time when last garbage
collection occurred.

Any cache files accessed within the last 2 days are kept.
Any log file entries older than 2 days are discarded.
The files older than one week are deleted.

An entry is kept by moving its modification time up to now, because the
age pass reads that time and not the log.  An entry a surviving log line
names may be gone already, evicted for the byte budget or deleted by an
earlier age pass, and no entry is created for such a line: an entry of no
bytes is not something a later compile can use.

Age alone does not bound the cache in bytes, so after the age pass the
cache is held to a size budget (`-maxmb`, default 10240): entries are
evicted oldest-last-use first until the total fits.  A cache shared by
every checkout and build configuration on a machine thus manages its
own disk footprint.

The garbage collection can be forced with `gcache -gc`

### Debugging
<a href="#debugging"></a>

Gcache will print subcommands and a summary if invoked with `-v`. The verbosity is inherited from abt (minus one level),
so running `abt <target> -v -v` will show gcache output.

For instance, to get a list of all cache files that participated in the compilation of a particular target, run

```bash
abt <target> -force -v -v 2>&1 | ssimfilt -format:field report.gcache -field:cached_file
```

### Logging
<a href="#logging"></a>

Gcache logs its report lines to file `log.ssim`. The report lines include cached file name, whether there was a
hit, size of preprocessed file, and others. These are used when computing statistics.
The hit rate for a time period can be queried with `gcache -hitrate -after <DATETIME>`

You can also watch gcache activity in real-time by running `tail -f` on this file.

### Privacy
<a href="#privacy"></a>

By default, the cache is kept in /tmp/gcache. This means that any user on the current system
can read and modify the cached files. Also, the log file includes source file names.
If you are concerned about privacy, use `gcache -install -dir ~/.gcache` to keep the information local to your home directory.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -cmd -- Command to execute
<a href="#-cmd"></a>

#### -install -- Create gcache directory and enable gcache
<a href="#-install"></a>

#### -stats -- Show cache stats
<a href="#-stats"></a>

#### -enable -- Create .gcache link to enable gcache use
<a href="#-enable"></a>

#### -disable -- Remove .gcache link to disable gcache
<a href="#-disable"></a>

#### -gc -- Clean old files from .gcache
<a href="#-gc"></a>

#### -clean -- Clean the entire cache
<a href="#-clean"></a>

#### -dir -- (With -install,-enable) cache directory
<a href="#-dir"></a>

#### -maxmb -- Cache size budget in MB; GC evicts oldest entries past it
<a href="#-maxmb"></a>

#### -hitrate -- Report hit rate (specify start time with -after)
<a href="#-hitrate"></a>

#### -after -- Start time for reporting
<a href="#-after"></a>

#### -report -- Show end-of-run report
<a href="#-report"></a>

#### -force -- Force recompile and update cache
<a href="#-force"></a>
