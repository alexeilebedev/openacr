## abt - A Build Tool

ABT is a build tool. The argument to abt is a target name regex.
ABT reads some ssim files (use acr_in abt to find out which ones);
Builds a dependency dag based on #includes; Invokes build commands.

With -cfg, abt can select different option sets.
Cfg can be release, profile, coverage, debug, or some custom config.

The list of available targets is loaded from dev.target ("acr dev.target" to list).
List of source files for each target is in by dev.targsrc.
To view the configuration for a given target, use acr dev.target:amc -t -e

Using configuration cfg:release, source file
cpp/amc/main.cpp compiles to dflt.release-x86_64/cpp.amc.main.o

Example: Build all

         $ abt -install %

Example: Re-build all, verbose mode

         $ abt -clean -install %

Example: Disassemble one function

         $ abt -cfg release amc -disas:%Main%

Example: Find out what files are out of date for a target

         $ abt -ood amc

Example: Install debug version of binaries into bin/

         $ abt % -install -cfg:debug

Example: Add a library to link with an executable

         echo dev.targdep:amc.json_lib | acr -replace -write


