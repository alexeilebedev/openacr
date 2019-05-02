## Directory Structure

* `bin`         All executable files. Add relative path 'bin/' to your PATH
* `conf`        Miscellaneous config files for emacs, vi
* `cpp`         All files ending in .cpp; cpp/gen contains outputs from amc
* `data`        All ssim files -- configuration tables etc.
* `diff`        Patch files
* `extern`      Files whose original home is not here.
* `include`     All files ending in .h; include/gen contains outputs from amc
* `lock`        Runtime lock files
* `temp`        Temp dir
* `txt`         .txt files

### Binaries

All executable files are accessible from bin (both scripts and binary executables).
For each binary, there is a soft link from bin to the directory where the binary really sits.

Binaries are compiled with `abt -build` and installed (linked into bin/) with `abt -install`.

Binaries are never kept in git history, but the soft links are. 
To get abt going, abt generates a bootstrap script for compiling 
itself, and that script sits in `bin/abt-bootstrap`. It is automatically re-generated during
`normalize` so every commit should have a working bootstrap as a result.

### Intermediate Files

Object files, libraries and executables are kept in `dflt.debug-x86_64` or `dflt.release-x86_64`.
Here, `dflt` is the compiler name. Other possible names are `clang`.

Build directories can be wiped with `make clean`. After that, the `abt` binary will no longer exist,
and it would have to be re-created with `ai` or `abt-bootstrap`.

The intermediate directory has a flat structure. The name of each object file is the 
path to the .cpp file with `/`s replaced by `.`s.
    
    $ ls dflt.release-x86_64/
    abc                    cpp.amc.ctype.read.o     cpp.amc.regx.o          cpp.atf.amc.strconv.o ...
    abt                    cpp.amc.delptr.o         cpp.amc.sbrk.o          cpp.atf.amc.tary.o
    acr                    cpp.amc.dflt.o           cpp.amc.signature.o     cpp.atf.amc.varlen.o
    acr_compl              cpp.amc.disp.call.o      cpp.amc.size.o          cpp.atf.amc.zdlist.o
    ...

