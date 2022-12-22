## abt: A Build Tool

Abt is a build tool. The argument to abt is a target name regex.
Here, target means 'build target'.

The ssimfiles `abt` reads describe which targets can be built,
what are the sources files that comprise the targets, and the various options to use.

Abt outputs are organized under the `build/` directory.
`abt` supports multiple compilers and architectures within the same directory.

When building, `abt` builds a dependency dag based on #includes; 
Invokes build commands, keeping up to N of them running at a time.
The simplest way to build everything is:

    $ abt %

Here we assume that `abt` itself has already been built with `ai`. For more information,
see the [Bootstrap](#Bootstrap) section.

Let's begin by creating a new target.

    $ acr_ed -create -target sample -write
    ...
    
This creates and builds `sample.` Let's clean sample. This cleans both the target
and all of the libraries that it might use. 

    $ abt -clean sample
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    report.abt  n_target:4  time:00:00:00.037978981  n_warn:0  n_err:0  n_install:0

And rebuild it:

    $ abt sample
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    abt.build  line_n:48,413  built_n:13,511  job_n:3  complete:28
                ^^^^^ build in progress

In the example above, `job_n` indicates the number of jobs that are running at the moment. The default is picked
based on the number of processors in the system. It can be overriden by specifying `-maxjobs`.
The `config` line specifies the 4-tuple of uname, compiler, configuration and target architecture
that determine which build options to use. If a compiler cache is being used, it's also specified here.
See the [Compiler Cache](#compiler-cache) section.

The `outofdate` line shows what `abt` found to be out of date: 1 precompiled header,
38 source files, 2 libraries and 1 executable.

### Input Tables

Abt's main input tables come from the dev namespace of the default data set
(as specified with `-in`)

* dev.target         Buildable target
* dev.targdep        Pairwise dependencies between targets
* dev.targsrc        List of source files for each target.
* dev.tool_opt       List of options to use for compilation and linking,

There are also a few general tables.

* dev.uname          List of supported OS's (Linux, Darwin, Cygwin, etc)
* dev.compiler       List of supported compilers (clang++, g++, etc)
* dev.cfg            List of arbitrarily named configurations (release, profile, debug, etc.)
* dev.arch           List of target architectures (such as x86_64)

### Output Directory

The resulting object files are now in `build/release/sample`, 
or more specifically `build/Linux-clang++.release-x86_64/sample`,
since `build/release/sample` is a soft link.


    $ ls -l build/Linux-clang++.release-x86_64/*sample*
    -rwxrwxr-x. 1 alexei alexei 109128 May  3 18:35 build/Linux-clang++.release-x86_64/sample
    -rw-rw-r--. 1 alexei alexei   1912 May  3 18:34 build/Linux-clang++.release-x86_64/cpp.sample.sample.o
    -rw-rw-r--. 1 alexei alexei  24776 May  3 18:34 build/Linux-clang++.release-x86_64/cpp.gen.sample_gen.o

`abt` places all output files in the same output directory, with no subdirectories.
Source file paths are flattened, substituting `/` with `.`. So, `cpp/sample/sample.cpp`
becomes `cpp.sample.sample.o` in this directory.

The output directory can be overriden with `-out_dir` option. 
If not specified, `-cfg` defaults to `-release` and the output directory defaults to `build/$cfg`.
Typically, `build/$cfg` is a soft link installed by whatever bootstrap script you ran.

### Bootstrap System

The bootstrap files are prepared by `atf_ci bootstrap` by scanning the `builddir` table 
and invoking `abt` with `-build:N -printcmd` options. There is one bootstrap file for each tested
combination of uname, compiler and arch. Bootstrap files are prepared only for the release configuation.
The bootstrap files live in `bin/bootstrap`:

    $ ls -l bin/bootstrap
    total 104
    -rwxr-xr-x 1 alexei alexei 21519 May 17 01:19 Darwin-clang++.release-x86_64
    -rwxr-xr-x 1 alexei alexei 23442 May 17 01:19 Linux-clang++.release-x86_64
    -rwxr-xr-x 1 alexei alexei 28566 May 17 01:19 Linux-g++-9.release-x86_64
    -rwxr-xr-x 1 alexei alexei 26072 May 17 01:19 Linux-g++.release-x86_64

When you first run `ai`, it finds that `abt -version` returns an error. This indicates that `abt`
is not usable. `ai` then guesses at the values of `uname`, `compiler` and `arch` by invoking
the `uname` utility and checking if the compiler is installed, and finally runs one of the bootstrap scripts
from `bin/bootstrap`. The bootstrap script create the top-level `build` directory and creates
soft links under it. When `abt` is invoked, this soft link provides abt with the defaults
for each of the options `-uname`, `-compiler`, `-arch`.

The `build/$cfg` (by `$cfg` I mean any one of the supported cfg values) can be rewritten
manually, or by rerunning one of the bootstrap scripts. For instance, if `ai` picked `clang++`
and you wanted `g++-9`, you can simply run the corresponding bootstrap script, and `g++-9`
will become the default compiler.

### The -install option

By default, the resulting files are left in the output directory.
There are already soft links from `bin`/ to `../build/release`. If we want to re-point the default
binary to a different version, the `-install` option will rewrite the soft link to point 
to the new executable. For instance, we may want to install a debug sample version with
`abt -install sample -cfg debug`. This will rewrite the soft link `bin/sample` to be
`../build/Linux-clang++.debug-x86_64/sample`.

### Target Definition

We can view the definition of target `sample`, as created by `acr_ed`, with `acr`.
As usual, several viewing options are available, in this case we skip the tree mode
and use a block output.

    $ acr target:sample -ndown 10 > x
    dev.target  target:sample

    dev.targdep  targdep:sample.algo_lib  comment:""
    dev.targdep  targdep:sample.algo_pch  comment:""
    dev.targdep  targdep:sample.lib_prot  comment:""

    dev.targsrc  targsrc:sample/cpp/sample/sample.cpp            comment:""
    dev.targsrc  targsrc:sample/cpp/gen/sample_gen.cpp        comment:""
    dev.targsrc  targsrc:sample/include/sample.h              comment:""
    dev.targsrc  targsrc:sample/include/gen/sample_gen.h      comment:""
    dev.targsrc  targsrc:sample/include/gen/sample_gen.inl.h  comment:""
    report.acr  n_select:9  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

As we can see, headers are considered sourdce files, and there are a couple of libraries
(`lib_prot` and `algo_lib`), and a precompiled header (`algo_pch`).

### Customizing Options

When debugging memory errors, we would use `abt -cfg:debug`; or 
`-cfg:coverage` when updating coverage.

It is possible to see what options `abt` will pass to the compiler under a certain configuration.
The slightly non-standard, non-SKNF table `dev.tool_opt` allows customizing these options
on a per-target, per-uname, per-compiler, per-cfg and per-arch basis:

    $ acr cfg:coverage -t
    dev.cfg  cfg:coverage  comment:"coverage measurement"
      dev.builddir  builddir:Linux.coverage-x86_64  comment:""

      dev.tool_opt  tool_opt:181  opt_type:CC_OPTS    opt:-ftest-coverage    target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:182  opt_type:CC_OPTS    opt:-fprofile-arcs     target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:183  opt_type:CC_OPTS    opt:"-fprofile-dir=."  target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:184  opt_type:LINK_OPTS  opt:--coverage         target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:185  opt_type:CC_OPTS    opt:-D_COVERAGE        target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      report.acr  n_select:7  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
  
### Disassembling

Abt includes a convenient disassembly mode, which can be invoked with `-disas`.
The parameter is a regular expression that's matched against function names in the 
compiler's assembler output.

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

### Specifying a different compiler

The `-compiler` option tells `abt` to use a compiler other than the default. In addition
to `-arch`, `-uname`, and `-cfg` option, this mostly affects the choice of options from the `tool_opt` 
table. For each of the files it compiles, `abt` scans the `tool_opt` table in full, and includes
any options that apply. There is a handful of custom lines of code in abt for dealing specifically
with `g++` and `clang`, and other cases that cannot be handled by `tool_opt` alone.

### The -ood option

You can find out which files are out-of-date (I don't really see how this could be useful,
but the option is there) with `-ood`:

    $ touch include/sample.h
    $ abt -ood sample -build:N
    dev.target  target:sample
    dev.srcfile  srcfile:cpp/sample/sample.cpp
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:0  src:1  lib:0  exe:1
    report.abt  n_target:4  time:00:00:00.031850008  n_warn:0  n_err:0  n_install:0

### The -listincl option

To see the full list of include files, as discovered by abt, for a given target, use

    $ abt -listincl sample 
    dev.Include  include:cpp/sample/sample.cpp:include/algo.h  sys:N  comment:""
    dev.Include  include:cpp/sample/sample.cpp:include/sample.h  sys:N  comment:""
    dev.Include  include:cpp/gen/sample_gen.cpp:include/algo.h  sys:N  comment:""
    ...

### Debugging the build

Just like with other programs, the verbosity level `-v` can be used to trace the execution.
When run with `-v`, abt will show the commands that execute. Otherwise, only the commands that
either fail or produce output are echoed to the screen. By default, they are hidden to keep output
clean.

### Compiler Cache

`abt` supports two compiler caches. The `ccache` is an open-source tool, and `gcache` is a built-in version.

~TBD~

## Working with source files & targets

### Listing Files

List all targets:

     acr target

List all libraries:

     acr dmmeta.ns.nstype:lib

List all sources for target T, one filename per line:

     acr targsrc:T/% -field:src

Re-build copyright blocks in files:

     src_hdr -write

### Creating Source Files
It is perfectly possible to never use any of these commands, and just do everything by hand.
But the muse of automation requires that working with source files should be automated.

All of these commands perform necessary git commands as well.
Running acr_ed without -write outputs a shell script. -write simply runs the script.

Create new source file (target is picked automatically):

       acr_ed -create -srcfile cpp/lib/algo/x.cpp -write -comment "X functions"

Delete source file:

       acr_ed -del -srcfile cpp/lib/algo/x.cpp -write

Rename source file:

       acr_ed -srcfile cpp/lib/algo/x.cpp -rename cpp/lib/algo/y.cpp -write

Create new header:

       acr_ed -srcfile include/x.h -target amc -write -comment "blah"

### Functions & Headers

Re-scan source files for function prototypes

    src_func -updateproto

List functions in target:

    src_func amc

List function prototypes in target:

    src_func amc -proto

Edit functions in target matching regex:

    src_func amc %X% -e
    <use F4, shift-F4 to visit locations>

Show functions that are not documented:

    src_func amc -listiffy

### Program Text

Edit all functions in program:

     src_func amc -e

Search all hand-written source for pattern:

     src_func % % "%pattern%"

Search all generated source for pattern:

     src_func % % "%pattern%" -gen

Search hand-written source for pattern:

     src_func amc % "%pattern%"

Search source of specific target for pattern and open in editor:

     src_func amc % "%pattern%" -e

Search function arguments (i.e. first line, including return type and arguments):

     src_func % % -func:%amc::FCtype%

Show target functions, sort by name:

     src_func amc -sortname

