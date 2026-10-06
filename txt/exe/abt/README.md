## abt - Algo Build Tool - build & link C++ targets
<a href="#abt"></a>
abt compiles and links the C++ targets of the tree.  Give it a regex of target
names, and it builds those targets and everything they depend on, running
several compiles in parallel and recompiling only what is out of date.  The
ssim tables say what a target is made of, so abt needs no makefile.

### Syntax
<a href="#syntax"></a>
```usage
abt: Algo Build Tool - build & link C++ targets
Usage: abt [[-target:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [target]    regx    ""      Regx of target name
    -in         string  "data"  Root of input ssim dir
    -cfg        regx    ""      Set config
    -compiler   string  ""      Set compiler.
    -uname      string  ""      Set uname (default: guess)
    -arch       string  ""      Set architecture (default: guess)
    -ood                        List out-of-date source files
    -list                       List target files
    -listincl                   List includes
    -build                      If set, build specified target (all necessary steps)
    -preproc                    Preprocess file, produce .i file
    -srcfile    regx    "%"     Build/disassemble/preprocess specific file
    -clean                      Delete all output files
    -dry_run                    Print actions, do not perform
    -maxjobs    int     0       Maximum number of child build processes. 0=pick good default
    -printcmd                   Print commands. Do not execute
    -force                      Assume all files are out-of-date
    -install                    Update soft-link under bin/
    -coverity                   Run abt in coverity mode
    -package    string  ""      Package tag
    -maxerr     int     100     Max failing commands before rest of pipeline is forced to fail
    -disas      regx    ""      Regex of function to disassemble
    -report             Y       Print final report
    -jcdb       string  ""      Create JSON compilation database in specified file
    -cache      enum    auto    Cache mode (auto|none|gcache|gcache-force|ccache)
    -shortlink                  Try to shorten sort link if possible
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

abt reads its inputs from the `dev` namespace of the ssim database under `-in`.
`dev.target` names each target, `dev.targsrc` lists its source files, and
`dev.targdep` lists the targets it depends on.  `dev.tool_opt` holds the
compiler and linker flags, and `dev.builddir`, `dev.cfg`, `dev.compiler`,
`dev.uname` and `dev.arch` list the supported configurations.  To see every row
behind one target, run `acr target:<name> -t`.

#### Selecting targets
<a href="#selecting-targets"></a>

The first argument is a regex over `dev.target`.  It matches whole names, with
`%` as the wildcard, so `abt 'acr%'` selects acr and every tool whose name
starts with it.  abt adds every target the
matching ones depend on, recursively, so `abt acr` also builds `algo_lib` and
the other libraries acr links.  A regex that matches nothing is an
`abt.nomatch` error.

With a target and no action flag, abt builds.  With no target at all, it prints
the list of every target it knows.

#### How abt decides what to rebuild
<a href="#how-abt-decides-what-to-rebuild"></a>

abt scans each source file for `#include` lines and follows them through the
headers, so a source file is as new as the newest file it includes.  A source
file newer than its object gets recompiled.  A target whose object files, or
whose dependencies, are newer than its output gets relinked.  abt runs the
compiles and links as a dependency graph, so a target links as soon as its own
objects and its libraries are done.

When a compiler cache is enabled, abt runs each compile through it.  By default
abt uses [gcache](/txt/exe/gcache/README.md) when `.gcache` exists in the
current directory, and ccache when `.ccache` does.

#### The build directory
<a href="#the-build-directory"></a>

A configuration is the four-part key `<uname>-<compiler>.<cfg>-<arch>`, such as
`Linux-g++.release-x86_64`.  Each configuration has one flat directory under
`build/`, and the link `build/<cfg>` names the one in use for that cfg, so
`build/release` points at `Linux-g++.release-x86_64` on a Linux machine.  abt
reads that link to fill in any of `-uname`, `-compiler` and `-arch` left off the
command line.

An object file is named after its source path with each `/` turned into `.`,
so `cpp/abt/main.cpp` compiles to `build/release/cpp.abt.main.o`.  A library
carries its arch, as in `build/release/algo_lib-x86_64.a`.  An executable
carries its bare name, and `bin/<target>` links to it, usually as
`../build/release/<target>`.

#### Bootstrapping
<a href="#bootstrapping"></a>

A fresh checkout has no abt to build abt with.  The directory `bin/bootstrap`
holds one shell script per supported configuration, and each script compiles
abt, gcache and a few other tools directly.  It also creates the `build/<cfg>`
links.  The [ai](/txt/script/ai.md) script picks the script that fits the
machine, runs it, and then builds the rest with abt.  The scripts are made with
`abt -printcmd`.

#### Build identity
<a href="#build-identity"></a>

abt stamps the build identity into `build/gitinfo.h` and `build/gitinfo.ssim`,
as a `dev.gitinfo` tuple naming the commit the tree is at.  `algo_lib` compiles the
header in, so every executable reports the tuple under `-version`:

```ssim
dev.gitinfo  gitinfo:2026-09-25.a2d5b7d53.abt  package:""  gitref:a2d5b7d5378b4e4152109a99271173143a464125  commitdate:2026-09-26T06:03:34  builddate:2026-09-26T12:59:47  comment:""
```

`gitref` is the commit and `commitdate` is when it was made.  `builddate` is the
moment of the first build at that commit, and `package` is the `-package` tag.
abt rewrites the pair of files only when the commit or the tag changes, so
repeated builds at one commit recompile nothing on its account.  A binary built
without abt, or from a tree with no git history, reports `comment:unversioned`.

### Examples
<a href="#examples"></a>

```bash
abt acr                                  # build acr and every library it links
abt -install acr                         # build acr and point bin/acr at it
abt -cfg:debug -install acr              # build a debug acr and point bin/acr at it
abt -cfg:coverage 'acr|amc'              # build two targets under coverage
abt -list abt                            # list the targets and source files abt would build
abt -listincl abt -srcfile:cpp/abt/main.cpp            # list what one file includes
abt acr -srcfile:cpp/acr/main.cpp        # compile one file, without linking
abt -preproc acr -srcfile:cpp/acr/main.cpp             # write the preprocessed .i for one file
abt bash2html -disas Main                # disassemble every function matching Main
abt -force -cache:none acr               # recompile acr from scratch, bypassing the cache
abt -printcmd bash2html > build.sh       # write the build as a shell script, running nothing
abt -jcdb:compile_commands.json '%'      # write a compilation database for clangd or cppcheck
abt -clean acr                           # delete the outputs of acr and its libraries
```

### Caveats
<a href="#caveats"></a>

- abt echoes a command only when it fails or prints something.  Add `-v` to see
  every command it runs, and `-v -v` to see gcache's own output too.
- `-force` with gcache enabled still serves most compiles from the cache.  Use
  `-cache:none` for a real recompile, or `-cache:gcache-force` to recompile and
  refresh the cache entries.
- `-clean` deletes the outputs of every selected target, and the selection
  includes the dependencies, so `abt -clean acr` removes the `algo_lib` library
  too.

### Options
<a href="#options"></a>
#### -target -- Regx of target name
<a href="#-target"></a>

Select the targets to act on, as a regex over `dev.target`.  abt adds each
selected target's dependencies to the selection.  With no target, abt prints the
list of all targets and exits.

#### -in -- Root of input ssim dir
<a href="#-in"></a>

Read the ssim tables from this directory.  The default is `data`, the tree's
own database.

#### -cfg -- Set config
<a href="#-cfg"></a>

Select the configuration to build, as a regex.  The default is `release`.  A
regex matching several configurations builds each of them in turn, so `abt
-cfg:% acr` builds acr in every configuration.  Each configuration selected
needs its `build/<cfg>` link, or abt stops with `abt.builddir`.
Possible values are
```ssim
inline-command: acr cfg -report:N | ssimfilt -t
CFG       SUFFIX  COMMENT
                  all
coverage  c       coverage measurement
debug     d       unoptimized, with symbols
memcheck  m       release with memory checks
profile   p       release with various debug options still on
release   r       optimized, without symbols

```

#### -compiler -- Set compiler.
<a href="#-compiler"></a>

Select the compiler.  The default is the one named by the link `build/<cfg>`.
The combination with `-uname`, `-cfg` and `-arch` must be a row of
`dev.builddir`, or abt stops with `abt.builddir`.
Possible values are
```ssim
inline-command: acr compiler -report:N | ssimfilt -t
COMPILER  RANLIB  AR       LINK      LIBEXT  EXEEXT  PCHEXT  OBJEXT  RC      COMMENT
                                                                             all
cl                LIB.EXE  LINK.EXE  .lib    .exe    .pch    .obj    RC.EXE
clang++   ranlib  ar       clang++   .a              .gch    .o
g++       ranlib  ar       g++       .a              .gch    .o
g++-9     ranlib  ar       g++-9     .a              .gch    .o

```

#### -uname -- Set uname (default: guess)
<a href="#-uname"></a>

Select the operating system to build for.  The default is the one named by the
link `build/<cfg>`.
Possible values are
```ssim
inline-command: acr uname -report:N | ssimfilt -t
UNAME    COMMENT
         default
Darwin   Last tested version 10.14.4 (Mojave)
FreeBSD  11.2, 12.0
Linux    Ubuntu 17, CentOS 7.6, Debian
SunOS    Tested on solaris 5.11

```

#### -arch -- Set architecture (default: guess)
<a href="#-arch"></a>

Select the architecture to build for.  The default is the one named by the link
`build/<cfg>`.
Possible values are
```ssim
inline-command: acr arch -report:N | ssimfilt -t
ARCH    COMMENT
        all
amd64   64-bit mode on FreeBSD
arm64   64-bit mode on Apple Silicon
i686    Cygwin 64-bit mode
i86pc   Solaris 64-bit mode
x64     64-bit mode on windows (under cygwin)
x86_64  64-bit mode on linux

```

#### -ood -- List out-of-date source files
<a href="#-ood"></a>

List what a build would recompile, without building: each out-of-date target,
followed by its out-of-date source files.  The `abt.config` line carries the
counts, as `ood_src` for source files and `ood_target` for targets.

```
$ abt -ood -force bash2html
abt.config  builddir:Linux-g++.release-x86_64  ood_src:40  ood_target:2  cache:gcache
dev.target  target:bash2html
dev.srcfile  srcfile:cpp/bash2html.cpp
dev.srcfile  srcfile:cpp/gen/bash2html_gen.cpp
dev.target  target:algo_lib
...
```

#### -list -- List target files
<a href="#-list"></a>

Print a `dev.target` line for each selected target, followed by a `dev.srcfile`
line for each of its source files.  `-srcfile` narrows the source files shown.
`-list` alone does not build.

#### -listincl -- List includes
<a href="#-listincl"></a>

Print a `dev.include` line for each include of the selected targets' source
files, following the includes through the headers.  `-srcfile` narrows the list
to the includes of the matching files.  `-listincl` alone does not build.

```ssim
inline-command: abt -listincl abt -srcfile cpp/abt/%
abt.config  builddir:***  ood_src:***  ood_target:***  cache:***
dev.include  include:cpp/abt/build.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/disas.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/gitinfo.cpp:include/algo.h  sys:N  comment:""
dev.include  include:cpp/abt/gitinfo.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/main.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/ood.cpp:include/algo.h  sys:N  comment:""
dev.include  include:cpp/abt/ood.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/opt.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/scan.cpp:include/algo.h  sys:N  comment:""
dev.include  include:cpp/abt/scan.cpp:include/abt.h  sys:N  comment:""
report.abt  n_target:***  time:***  hitrate:***  pch_hitrate:***  n_warn:0  n_err:0  n_install:***
```

#### -build -- If set, build specified target (all necessary steps)
<a href="#-build"></a>

Compile the out-of-date source files and link the out-of-date targets.  abt
builds by default when a target is given with none of `-list`, `-listincl`,
`-ood`, `-clean` and `-preproc`.  Pass `-build` next to one of those to do both.

#### -preproc -- Preprocess file, produce .i file
<a href="#-preproc"></a>

Run the preprocessor on each selected source file and write the result next to
its object, with the `.i` extension.  Combine it with `-srcfile` to look at one
file.  `-preproc` alone does not build.

#### -srcfile -- Build/disassemble/preprocess specific file
<a href="#-srcfile"></a>

Narrow the source files of each selected target to those matching this regex.
It applies to `-build`, `-preproc`, `-disas`, `-clean`, `-list` and
`-listincl`.  A build narrowed this way compiles the matching files and skips
the link, which is how to compile a single file.

#### -clean -- Delete all output files
<a href="#-clean"></a>

Delete the object files and the output file of every selected target, in every
selected configuration.  A coverage build also loses its `.gcda` and `.gcno`
files.  `-clean` alone does not build, and `-clean -build` rebuilds from
scratch.

#### -dry_run -- Print actions, do not perform
<a href="#-dry_run"></a>

Print the commands the build would run, as `dev.syscmd` records and the
`dev.syscmddep` records that order them, and run none of them.  `-printcmd`
prints the same commands as a shell script.

#### -maxjobs -- Maximum number of child build processes. 0=pick good default
<a href="#-maxjobs"></a>

Run at most this many compiles and links at once.  The default of 0 picks half
the processors on the machine, and never fewer than 4.  `-printcmd` sets it to
1.

#### -printcmd -- Print commands. Do not execute
<a href="#-printcmd"></a>

Print the whole build as a shell script, running nothing.  The script opens
with `set -e` and `set -x` and lists every command in order.  `-printcmd`
implies `-force` and `-maxjobs:1`, leaves out the compiler cache, and does not
need the compiler to be installed, which is how the bootstrap scripts are made.

#### -force -- Assume all files are out-of-date
<a href="#-force"></a>

Treat every source file and every target as out of date, so everything
selected is compiled and linked again.  With gcache enabled, the compiles are
still served from the cache, and `-cache:gcache-force` makes them real.

#### -install -- Update soft-link under bin/
<a href="#-install"></a>

Build the selected executables, then point `bin/<target>` at each one.  A plain
build leaves `bin/` alone.  `abt -install -cfg:debug acr` makes `bin/acr` a
link to `../build/debug/acr`, and `abt -install acr` points it back at the
release build.

#### -coverity -- Run abt in coverity mode
<a href="#-coverity"></a>

abt accepts this flag and ignores it.

#### -package -- Package tag
<a href="#-package"></a>

Stamp this tag into the build identity, as the `package` attribute of
`dev.gitinfo`.  Every executable built afterwards reports it under `-version`.
Changing the tag rewrites the stamp, and the next build recompiles the one
source file that carries it.

#### -maxerr -- Max failing commands before rest of pipeline is forced to fail
<a href="#-maxerr"></a>

Stop starting new commands once this many have failed.  The commands left are
marked failed without running, so a broken build ends quickly.  The default is
100.

#### -disas -- Regex of function to disassemble
<a href="#-disas"></a>

Show the disassembly of every function whose demangled name contains a match
for this regex.
abt builds the selected targets first, then runs `objdump` over their object
files.  Use `-srcfile` to search fewer files, and a `-cfg` regex matching
several configurations to compare their code.

```
$ abt bash2html -disas Main -srcfile:cpp/bash2html.cpp
abt.config  builddir:Linux-g++.release-x86_64  ood_src:0  ood_target:0  cache:gcache
# abt.disas  srcfile:cpp/bash2html.cpp  objfile:build/release/cpp.bash2html.o
0000000000000000 <bash2html::Main()>:
       0:	f3 0f 1e fa          	endbr64
       4:	41 56                	push   %r14
       6:	41 55                	push   %r13
...
```

#### -report -- Print final report
<a href="#-report"></a>

Print the `abt.config` line for each configuration and the `report.abt` line at
the end.  The report counts targets, warnings, errors and installed binaries,
and gives the gcache hit rate.  `-report:N` leaves both lines out.

#### -jcdb -- Create JSON compilation database in specified file
<a href="#-jcdb"></a>

Write a JSON compilation database for the selected targets to this file, and
build nothing.  Tools such as clangd and cppcheck read the file to learn how
each source file compiles.  The commands in it carry no compiler cache prefix.

#### -cache -- Cache mode
<a href="#-cache"></a>

Choose the compiler cache.  `auto`, the default, uses
[gcache](/txt/exe/gcache/README.md) when `.gcache` exists and `bin/gcache` is
built, and ccache when `.ccache` exists.  `gcache` and `ccache` ask for one
cache and print an `abt.notice` when it is not set up.  `gcache-force` is
`gcache` with each compile done afresh and its cache entry replaced, and `none`
compiles directly.

#### -shortlink -- Try to shorten sort link if possible
<a href="#-shortlink"></a>

Write the build into the full configuration directory, such as
`build/Linux-g++.release-x86_64`, and point `-install` links there.  Without
the flag, abt writes through the link `build/<cfg>`, so `bin/acr` points at
`../build/release/acr`.  The build goes through that link whatever `-compiler`,
`-uname` and `-arch` say, so pass `-shortlink` when you override one of them.
