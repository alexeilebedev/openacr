## gcache: Compiler cache

GCACHE is compiler cache paired with ABT.  When enabled, ABT automatically
detects and uses it.

### Syntax

```
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
    -hitrate                           Report hit rate (specify start time with -after)
    -after      string                 Start time for reporting
    -report                            Show end-of-run report
    -verbose    int                    Verbosity level (0..255); alias -v; cumulative
    -debug      int                    Debug level (0..255); alias -d; cumulative
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig

```

### Enable and disable

The tool needs cache directory `.gcache`.

A number of scripts facilitates managing this directory:

- **gcache -install** - set up `/tmp/gcache` storage (with `-dir` option, this location can be customized)
- **gcache -enable** - create softlink `.gcache` pointing to cache directory (with `-dir` option, a different location can be customized)
- **gcache -disable** - delete softlink `.gcache`, disabling caching
- **gcache -stats** - show cache stats
- **gcache -gc** - force garbage collection
- **gcache -clean** - remove the entire cache

To enable gcache, use:

```
gcache -install
```

To disable gcache, use:

```
gcache -disable
```

Or remove the softlink `.gcache`.

### Invocation

**bin/gcache** is tool executable, which shall be put in front of compiler
command, for example:

```
gcache -- g++ -c sample.cpp -o sample.o
```

First run invokes the compiler, and stores resulting object file under
hash in the cache directory.

On subsequent runs, the same command will copy saved object file to target location.

### Hash algorithm

To compute hash, the tool invokes compiler to preprocess the source, then
hashes command line with preprocessed source file using SHA-1 algorithm.

### Directory structure

Object files are stored under three-level directory structure:

*AB*/*CD*/*ABCD...*

where:
- *AB* - first two hex digits of the hash;
- *CD* - next two hex digits of the hash;
- *ABCD...* - full hex string of hash;

### Garbage collection

Tool automatically performs garbage collection daily.  The file
`gm.time` serves this task, its mtime is the time when last garbage
collection occurred.

Any cache files accessed within the last 2 days are kept.
Any log file entries older than 2 days are discarded.
The files older than one week are deleted.

The garbage collection can be forced with `gcache -gc`

### Debugging

Gcache will print subcommands and a summary if invoked with `-v`. The verbosity is inherited from abt (minus one level),
so running `abt <target> -v -v` will show gcache output.

For instance, to get a list of all cache files that participated in the compilation of a particular target, run

```
abt <target> -force -v -v 2>&1 | ssimfilt -format:field report.gcache -field:cached_file
```

### Logging

Gcache logs its report lines to file `log.ssim`. The report lines include cached file name, whether there was a
hit, size of preprocessed file, and others. These are used when computing statistics.
The hit rate for a time period can be queried with `gcache -hitrate -after <DATETIME>`

You can also watch gcache activity in real-time by running `tail -f` on this file.

### Privacy

By default, the cache is kept in /tmp/gcache. This means that any user on the current system
can read and modify the cached files. Also, the log file includes source file names.
If you are concerned about privacy, use `gcache -install -dir ~/.gcache` to keep the information local to your home directory.

### Inputs

`gcache` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```

