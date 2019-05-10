## abt: A Build Tool

Abt is a build tool. The argument to abt is a target name regex.
Here, target means 'build target'.
Abt reads some ssim files that describe which source files go into 
these targets, and the dependencies between targets;
builds a dependency dag based on #includes; Invokes build commands,
keeping up to N of them running at a time.
The simplest way to build everything is:

    $ abt %

Let's begin by creating a new target.

    $ acr_ed -create -target abc -write

This creates and builds `abc.` Let's clean abc. This cleans both the target
and all of the libraries that it might use. 

    $ abt -clean abc
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    report.abt  n_target:4  time:00:00:00.037978981  n_warn:0  n_err:0  n_install:0

And rebuild it:

    $ abt abc
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    abt.build  line_n:48,413  built_n:13,511  job_n:3  complete:28
                ^^^^^ build in progress

In the example above, I'm running the command on a weak cloud-based VM,
so only 3 jobs run in parallel (as indicated by `job_n`). This default is picked
based on the number of processors in the system. It can be overriden by specifying `-maxjobs`.
The `config` line specifies
which config (release, debug, etc) to use; what OS (Linux), architecture (x86_64),
compiler (g++) and compiler cache.

The `outofdate` line shows what `abt` found to be out of date: 1 precompiled header,
38 source files, 2 libraries and 1 executable.

### Input Tables

Abt's main input tables come from the dev namespace of the default data set
(as specified with `-in`)

* dev.target         buildable target
* dev.targdep        pairwise dependencies between targets
* dev.targsrc        list of source files for each target.
* dev.tool_opt       list of options to use for compilation and linking,

### Environment Variables

Abt reads the following environment variables, but does not require any of them to be set.

* COMPILER - read if `-compiler` is not speified. Default is `g++`.
* UNAME - read if `-uname` is not specified. Default is obtained from `uname` command.
* ARCH - read if `-arch` is not specified. Default is obtained from `uname` command.
* CFG - read if `-cfg` is not specified. Default is `release`.

### Output Directory

The resulting object files are now in `dflt.release-x86_64/abc`:

    $ ls -l dflt.release-x86_64/*abc*
    -rwxrwxr-x. 1 alexei alexei 109128 May  3 18:35 dflt.release-x86_64/abc
    -rw-rw-r--. 1 alexei alexei   1912 May  3 18:34 dflt.release-x86_64/cpp.abc.abc.o
    -rw-rw-r--. 1 alexei alexei  24776 May  3 18:34 dflt.release-x86_64/cpp.gen.abc_gen.o

`abt` places all output files in the same output directory, with no subdirectories.
Source file paths are flattened, substituting `/` with `.`.

The output directory can be overriden with `-out_dir` option. If not specified, the output
directory defaults to `$compiler.$cfg-$arch`.

### The -install option

By default, the resulting files are left in the output directory.
There are already soft links from bin/ to `../dflt.release-x86_64`. If we want to re-point the default
binary to a different version, the `-install` option will rewrite the soft link to point 
to the new executable. 

### Target Definition

We can view the definition of target `abc`, as created by `acr_ed`, with `acr`.
As we can see, headers are considered sourdce files, and there are a couple of libraries
(`lib_prot` and `algo_lib`), and a precompiled header (`algo_pch`).

    $ acr target:abc -ndown 10 > x
    dev.target  target:abc

    dev.targdep  targdep:abc.algo_lib  comment:""
    dev.targdep  targdep:abc.algo_pch  comment:""
    dev.targdep  targdep:abc.lib_prot  comment:""

    dev.targsrc  targsrc:abc/cpp/abc/abc.cpp            comment:""
    dev.targsrc  targsrc:abc/cpp/gen/abc_gen.cpp        comment:""
    dev.targsrc  targsrc:abc/include/abc.h              comment:""
    dev.targsrc  targsrc:abc/include/gen/abc_gen.h      comment:""
    dev.targsrc  targsrc:abc/include/gen/abc_gen.inl.h  comment:""
    report.acr  n_select:9  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

### Customizing Options

When debugging memory errors, we would use `abt -cfg:debug`; or 
`-cfg:coverage` when updating coverage.

It is possible to see what options `abt` will pass to the compiler under a certain configuration.
The slightly non-standard, non-SKNF table `dev.tool_opt` allows customizing these options
on a per-target, per-uname, per-compiler, per-cfg and per-arch basis:

    $ acr cfg:coverage -t
    dev.cfg  cfg:coverage  comment:"coverage measurement"
      dev.builddir  builddir:dflt.coverage-x86_64  comment:""

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

    $ abt abc -disas Main | head -15
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.outofdate  pch:0  src:0  lib:0  exe:0
    0000000000000000 <abc::Main()>:
       0:	53                   	push   %rbx
       1:	be 00 00 00 00       	mov    $0x0,%esi
       6:	48 83 ec 10          	sub    $0x10,%rsp
       a:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 10 <abc::Main()+0x10>
      10:	48 89 e7             	mov    %rsp,%rdi
      13:	48 c7 04 24 00 00 00 	movq   $0x0,(%rsp)
      1a:	00 
      1b:	c7 44 24 08 0d 00 00 	movl   $0xd,0x8(%rsp)
      22:	00 
      23:	e8 00 00 00 00       	callq  28 <abc::Main()+0x28>
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

    $ touch include/abc.h
    $ abt -ood abc -build:N
    dev.target  target:abc
    dev.srcfile  srcfile:cpp/abc/abc.cpp
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.outofdate  pch:0  src:1  lib:0  exe:1
    report.abt  n_target:4  time:00:00:00.031850008  n_warn:0  n_err:0  n_install:0

### The -listincl option

To see the full list of include files, as discovered by abt, for a given target, use

    $ abt -listincl abc 
    dev.Include  include:cpp/abc/abc.cpp:include/algo.h  sys:N  comment:""
    dev.Include  include:cpp/abc/abc.cpp:include/abc.h  sys:N  comment:""
    dev.Include  include:cpp/gen/abc_gen.cpp:include/algo.h  sys:N  comment:""
    ...

### Debugging the build

Just like with other programs, the verbosity level `-v` can be used to trace the execution.
When run with `-v`, abt will show the commands that execute. Otherwise, only the commands that
either fail or produce output are echoed to the screen. By default, they are hidden to keep output
clean.

### Bootstrapping

With the `-printcmd` option, abt doesn't actually run the commands but simply prints them to 
stdout. This can be used to generate bootstrap scripts, such as `bin/abt-bootstrap`. 
