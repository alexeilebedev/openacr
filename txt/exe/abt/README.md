## abt - Algo Build Tool - build & link C++ targets


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Bootstrapping](#bootstrapping)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Target Definition](#target-definition)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Build Directory](#build-directory)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Debugging the build](#debugging-the-build)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [abt - Internals](/txt/exe/abt/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
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
    -cache      int     auto    Cache mode (auto|none|gcache|gcache-force|ccache)
                                    auto  Select cache automatically among enabled
                                    none  No cache
                                    gcache  Select gcache if enabled (no cache if disabled)
                                    gcache-force  Pass --force to gcache (no cache if disabled)
                                    ccache  Select ccache if enabled (no cache if disabled)
    -shortlink                  Try to shorten sort link if possible
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
Abt is a build tool. The argument to abt is a target name regex.
Target means 'build target'.

The ssimfiles `abt` reads describe which targets can be built,
what are the sources files that comprise the targets, and the various options to use.

Abt outputs are organized under the `build/` directory.
`abt` supports multiple compilers and architectures.

When building, `abt` builds a dependency dag based on #includes; 
Invokes build commands, several jobs running at a time.

Builds happen in parallel by default. The default is picked
based on the number of processors in the system. It can be overriden by specifying `-maxjobs`.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Bootstrapping
<a href="#bootstrapping"></a>
Initially, no executables exist. OpenACR uses a `bootstrap` file, which is a pre-computed
script residing in `bin/bootstrap`, to build `abt`, after which `abt` itself is used to build the rest.
There is one bootstrap file for each supported configuration. When the bootstrap file runs, it also
sets up soft links `build/$cfg` (e.g. `build/release`, `build/debug`).
Bootstrapping can be done by hand, or with the `ai` script. The `ai` script examines the current
system to find a supported coniguration. Bootstrap files are prepared with `abt -printcmd`.

### Target Definition
<a href="#target-definition"></a>

Build-related information for target X can be obtained with `acr target:X -t`.
This includes the [target](/txt/ssimdb/dev/target.md), its [source files](/txt/ssimdb/dev/targsrc.md),
and [dependencies](/txt/ssimdb/dev/targdep.md)

### Build Directory
<a href="#build-directory"></a>
All object, libraries and executables are deposited into the directory
`build/$uname-$compiler.$cfg-$arch`, for instance `build/Linux-g++.release-x86_64`.
There are no subdirectories, all files are placed directly in the build directory.
For source files, the object file is calculated by replacing `/` with `.` in the path,
for instance `cpp/abt/main.cpp` becomes `build/Linux-g++.release-x86_64/cpp.abt.main.o`.

For each executable, there is a soft link `bin/$target` usually pointing to `../build/release/$target`.

### Debugging the build
<a href="#debugging-the-build"></a>

Just like with other programs, the verbosity level `-v` can be used to trace the execution.
When run with `-v`, abt will show the commands that execute. Otherwise, only the commands that
either fail or produce output are echoed to the screen. By default, they are hidden to keep output
clean.

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -target -- Regx of target name
<a href="#-target"></a>

#### -in -- Root of input ssim dir
<a href="#-in"></a>

#### -cfg -- Set config
<a href="#-cfg"></a>

Possible values are
```
inline-command: acr cfg -report:N | ssimfilt -t
CFG       SUFFIX  COMMENT
                  all
coverage  c       coverage measurement
debug     d       bounds-checked build
profile   p       release with various debug options still on
release   r       all bound-checking disabled

```

#### -compiler -- Set compiler.
<a href="#-compiler"></a>

Specify compiler to use.
The default is obtained by reading the soft link `build/$cfg`.
Possible values are
```
inline-command: acr compiler -report:N | ssimfilt -t
COMPILER  RANLIB       AR       LINK      LIBEXT  EXEEXT  PCHEXT  OBJEXT  RC      COMMENT
                                                                                  all
cl                     LIB.EXE  LINK.EXE  .lib    .exe    .pch    .obj    RC.EXE
clang++   llvm-ranlib  llvm-ar  clang++   .a              .gch    .o
g++       ranlib       ar       g++       .a              .gch    .o
g++-9     ranlib       ar       g++-9     .a              .gch    .o

```

This option value can be a regex. Specifying `abt -cfg:% %` will build all targets
in all configurations.

#### -uname -- Set uname (default: guess)
<a href="#-uname"></a>

Specify target uname
The default is obtained by reading the soft link `build/$cfg`.
Possible values are
```
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

Specify architecture.
The default is obtained by reading the soft link `build/$cfg`.
Possible values are
```
inline-command: acr arch -report:N | ssimfilt -t
ARCH    COMMENT
        all
amd64   64-bit mode on FreeBSD
i686    Cygwin 64-bit mode
i86pc   Solaris 64-bit mode
x64     64-bit mode on windows (under cygwin)
x86_64  64-bit mode on linux

```

#### -ood -- List out-of-date source files
<a href="#-ood"></a>

List out-of-date files

    $ touch include/sample.h
    $ abt -ood sample -build:N
    dev.target  target:sample
    dev.srcfile  srcfile:cpp/sample/sample.cpp
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:0  src:1  lib:0  exe:1
    report.abt  n_target:4  time:00:00:00.031850008  n_warn:0  n_err:0  n_install:0

#### -list -- List target files
<a href="#-list"></a>

List files belonging to selected targets

#### -listincl -- List includes
<a href="#-listincl"></a>

List include files belonging to selected targets.
With `-srcfile`, narrow down the list to just the specified files.
Example:

```
inline-command: abt -listincl abt -srcfile cpp/abt/%
dev.include  include:cpp/abt/build.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/disas.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/main.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/ood.cpp:include/algo.h  sys:N  comment:""
dev.include  include:cpp/abt/ood.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/opt.cpp:include/abt.h  sys:N  comment:""
dev.include  include:cpp/abt/scan.cpp:include/algo.h  sys:N  comment:""
dev.include  include:cpp/abt/scan.cpp:include/abt.h  sys:N  comment:""
abt.config  builddir:Linux-g++.release-x86_64  ood_src:***  ood_target:***  cache:***
report.abt  n_target:***  time:***  hitrate:***  pch_hitrate:***  n_warn:0  n_err:0  n_install:***
```

#### -build -- If set, build specified target (all necessary steps)
<a href="#-build"></a>

This is the default unless other options are specified.

For libraries, the `arch` suffix is applied, so the target `algo_lib` produces the library
`build/Linux-g++.release-x86_64/algo_lib-x86_64.a`.

#### -preproc -- Preprocess file, produce .i file
<a href="#-preproc"></a>

#### -srcfile -- Build/disassemble/preprocess specific file
<a href="#-srcfile"></a>

For `-preproc`, `-build`, `-disas`, `-clean`, `-list`, and `-listincl` commands, abt supports narrowing down each
target to a specific set of source files. When this option is specified, the linking step
will be suppressed. It can be used to compile a single file, or disassemble a function from
a single file.

#### -clean -- Delete all output files
<a href="#-clean"></a>

This command deletes all object files and target output files matching input filter.

#### -dry_run -- Print actions, do not perform
<a href="#-dry_run"></a>

#### -maxjobs -- Maximum number of child build processes. 0=pick good default
<a href="#-maxjobs"></a>

#### -printcmd -- Print commands. Do not execute
<a href="#-printcmd"></a>

#### -force -- Assume all files are out-of-date
<a href="#-force"></a>

#### -install -- Update soft-link under bin/
<a href="#-install"></a>

By default, the resulting files are left in the build directory.
There are already soft links from `bin`/ to `../build/release`. If we want to re-point the default
binary to a different version, the `-install` option will rewrite the soft link to point 
to the new executable. For instance, we may want to install a debug version of target `sample` with
`abt -install sample -cfg debug`. This will rewrite the soft link `bin/sample` to be
`../build/Linux-clang++.debug-x86_64/sample`.

#### -coverity -- Run abt in coverity mode
<a href="#-coverity"></a>

#### -package -- Package tag
<a href="#-package"></a>

#### -maxerr -- Max failing commands before rest of pipeline is forced to fail
<a href="#-maxerr"></a>

#### -disas -- Regex of function to disassemble
<a href="#-disas"></a>

The parameter is a regular expression that's matched against function names in the 
compiler's assembler output.

```
    $ abt sample -disas Main | head -15
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:0  src:0  lib:0  exe:0
    0000000000000000 <sample::Main()>:
       0:	53                   	push   %rbx
       1:	be 00 00 00 00       	mov    $0x0,%esi
       6:	48 83 ec 10          	sub    $0x10,%rsp
       a:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 10 <sample::Main()+0x10>
      10:	48 89 e7             	mov    %rsp,%rdi
      13:	48 c7 04 24 00 00 00 	movq   $0x0,(%rsp)
      1a:	00 
      1b:	c7 44 24 08 0d 00 00 	movl   $0xd,0x8(%rsp)
      22:	00 
      23:	e8 00 00 00 00       	callq  28 <sample::Main()+0x28>
      28:	89 da                	mov    %ebx,%edx
      2a:	b9 01 00 00 00       	mov    $0x1,%ecx
```

With `-cfg` selecting more than one configuration, you can quickly compare disassembled output
for more than one configuration.

#### -report -- Print final report
<a href="#-report"></a>

#### -jcdb -- Create JSON compilation database in specified file
<a href="#-jcdb"></a>

This output file is used with `cppcheck`. The parameter is the filename.

#### -cache -- Cache mode
<a href="#-cache"></a>

Specify which compiler cache to use. Default is "auto", which looks for
a directory entry `.ccache` or `.gcache`. If one is found, corresponding
cache is used. See [gcache][/txt/exe/gcache/README.md] for more information.

#### -shortlink -- Try to shorten sort link if possible
<a href="#-shortlink"></a>

This option is used with `-printcmd` to make sure the installed binaries point to `../build/release`
instead of `../build/Linux-g++.release-x86_64`.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`abt` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.uname](/txt/ssimdb/dev/uname.md)|List of known unames|
|[dev.tool_opt](/txt/ssimdb/dev/tool_opt.md)|Compiler/linker options to use|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.syslib](/txt/ssimdb/dev/syslib.md)|Registered system library|
|[dev.targsyslib](/txt/ssimdb/dev/targsyslib.md)|Use of system library by target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.targdep](/txt/ssimdb/dev/targdep.md)|Dependency between targets|
|[dev.syscmd](/txt/ssimdb/dev/syscmd.md)|System command to execute|
|[dev.syscmddep](/txt/ssimdb/dev/syscmddep.md)|Dependency between two commands|
|[dev.include](/txt/ssimdb/dev/include.md)|A site where one file includes another|
|[dev.compiler](/txt/ssimdb/dev/compiler.md)|One of the known compilers|
|[dev.cfg](/txt/ssimdb/dev/cfg.md)|Compiler configuration|
|[dev.arch](/txt/ssimdb/dev/arch.md)|System architecture|
|[dev.builddir](/txt/ssimdb/dev/builddir.md)|Directory where object files/executables go. Determines compile/link options|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

