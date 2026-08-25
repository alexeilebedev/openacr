## Setup And Installation
<a href="#setup-and-installation"></a>
Presently, this project has been tested with the following distributions / compilers:

* g++ on Linux
* Apple Clang on macOS (Apple Silicon)

The following combinations have been tested at some point but are no longer maintained
and are most likely broken:

* clang on FreeBSD
* clang on Linux (RHEL, CentOS, Ubuntu, Debian)
* g++ on Cygwin
* g++-9 on Linux

MariaDB, OpenSSL, and GNU Readline are the main external dependencies.
MariaDB (formerly MySQL) is not really required, but it's used by `acr_my`, `ssim2mysql` and
`mysql2ssim` utilities. OpenSSL provides `libcrypto` for SHA1 functions, and
Readline provides interactive command-line editing.

### Pre-requisites: CentOS:
<a href="#pre-requisites-centos"></a>
    yum install -y mariadb mariadb-devel mariadb-server

### Pre-requisites: Ubuntu/Debian
<a href="#pre-requisites-ubuntu-debian"></a>
    apt install -y mariadb-server mariadb-client libmariadb-dev libmariadbd-dev \
        libssl-dev libcurl4-openssl-dev liblz4-dev libpq-dev cppcheck
    apt install llvm llvm-dev  # to enable abt -compiler llvm

### Prerequisites: macOS (Apple Silicon)
<a href="#pre-requisites-macos"></a>
Install [Homebrew](https://brew.sh/). Then install the dependencies and make
their headers and libraries visible to the compiler:

    brew install mariadb openssl readline
    mkdir -p temp/homebrew-include
    ln -sfn /opt/homebrew/opt/mariadb/include/mysql temp/homebrew-include/mariadb
    export CPATH="$PWD/temp/homebrew-include:/opt/homebrew/include${CPATH:+:$CPATH}"
    export LIBRARY_PATH="/opt/homebrew/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"

Homebrew uses `/opt/homebrew` on Apple Silicon. The local MariaDB link provides
the include layout expected by OpenACR without changing system directories.
Run these commands from the OpenACR top-level directory; the exported paths
affect only the current shell session.

TODO: Update OpenACR to recognize Homebrew's `<mysql/mysql.h>` header layout.
OpenACR currently expects `<mariadb/mysql.h>`, so `temp/homebrew-include` acts
as a compatibility shim by exposing Homebrew's `mysql` include directory as
`mariadb`. Remove the temporary directory and symlink from these instructions
after the source supports the Homebrew layout directly.

TODO: Add a native macOS wakeup backend for `lib_ams` signaled mode. Linux uses
`signalfd` and real-time signals, which macOS does not provide. Regular
busy-poll mode is supported on macOS; the optional signaled sleep mode is not.

### Path
<a href="#path"></a>
Add the relative path `bin` to your `PATH`.
All commands are issued from this top-level directory. This normalizes all
pathnames to a single form. In macOS's default shell (`zsh`), and in `bash`, use:

    export PATH="$PATH:bin"

`set PATH=$PATH:bin` does not assign `PATH` in `zsh` or `bash`. The `export`
command above affects only the current shell session. For a persistent macOS
configuration, add an absolute path to `~/.zshrc` and reload it:

    export PATH="$PATH:/absolute/path/to/openacr/bin"
    source ~/.zshrc

Using an absolute path is more reliable because the relative `bin` entry is
resolved against the current working directory.

### Building
<a href="#building"></a>
Run `ai`.
See also [ai](/txt/script/ai.md) and [abt](/txt/exe/abt/README.md)

On Apple Silicon, Clang's undefined-inline diagnostic remains enabled. An inline
function declared without a definition is a source error and should be fixed
rather than hidden with `-Wno-undefined-inline`.

### Editor configuration files
<a href="#editor-configuration-files"></a>
See files in `conf/` for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up. You may already have some of these files, so you'll need
to decide whether you want to merge provided configs with yours, replace or do something else.

    ln -s $PWD/conf/emacs.el ~/.emacs
    ln -s $PWD/conf/elisp ~/elisp
    ln -s $PWD/conf/vimrc ~/.vimrc
    ln -s $PWD/conf/vim ~/.vim
    ln -s $PWD/conf/alexei/bash_profile ~/.bash_profile

### Environment Variables
<a href="#environment-variables"></a>
* EDITOR - standard environment variable specifying which editor to use.

### Directory Structure
<a href="#directory-structure"></a>
* `bin`         All executable files. Add relative path 'bin/' to your PATH
* `conf`        Miscellaneous config files for emacs, vi
* `cpp`         All files ending in .cpp; cpp/gen contains outputs from amc
* `data`        All ssim files -- configuration tables etc.
* `diff`        Patch files
* `extern`      Files whose original home is not here.
* `include`     All files ending in .h; include/gen contains outputs from amc
* `lock`        Runtime lock files
* `temp`        Temp dir
* `txt`         .txt and .md (markdown) files

### Binaries
<a href="#binaries"></a>
All executable files are accessible from bin (both scripts and binary executables).
For each binary, there is a soft link from bin to the directory where the binary really sits.

Binaries are compiled with `abt -build` and the soft link in `bin/` can be updated with `abt -install`.

Binaries are not kept in git, only the soft links in bin/ are stored.
To make these links valid, the targets must be built with `ai`, which is a bootstrap script
that will build `abt` if it doesn't yet exist.

### Intermediate Files
<a href="#intermediate-files"></a>
Object files, libraries and executables are kept in `build/release/` directory.
Build directories can be wiped with `make clean`. 
After that, the `abt` binary will no longer exist, and it would have to be re-created with `ai`

The intermediate directory has a flat structure. The name of each object file is the 
path to the .cpp file with `/`s replaced by `.`s.
    
    $ ls build/release/
    abc                    cpp.amc.ctype.read.o     cpp.amc.regx.o          cpp.atf.amc.strconv.o ...
    abt                    cpp.amc.delptr.o         cpp.amc.sbrk.o          cpp.atf.amc.tary.o
    acr                    cpp.amc.dflt.o           cpp.amc.signature.o     cpp.atf.amc.varlen.o
    acr_compl              cpp.amc.disp.call.o      cpp.amc.size.o          cpp.atf.amc.zdlist.o
    ...

### Ssimfiles
<a href="#ssimfiles"></a>
`Ssim`, which stands for *super-simple*, is a line-oriented text format for
describing data in the form of tables of tuples. Each tuple consists
of a type tag and key-value pairs called attributes. The first
key-value pair is a primary key.

    $ acr ctype:amc.% | head
    dmmeta.ctype  ctype:amc.BltinId       comment:""
    dmmeta.ctype  ctype:amc.CppkeywordId  comment:""
    dmmeta.ctype  ctype:amc.Enumstr       comment:Key
    dmmeta.ctype  ctype:amc.FAnonfld      comment:""
    ^^^^^^^^^^^^        ^^^^^^^^^^^^
        |                    |
        type tag             primary key

Every line is treated as an element of a set.  There are no headers or
footers or other file markers, although lines can be commented out with `#`.
Any concatenation, permutation, or subset of two ssim files is a
valid ssim file, just like you would expect with sets.

Leading and trailing whitespace, and whitespace between key-value pairs
is ignored, and may be used to aid legibility.

Both keys and values may encode arbitrary byte sequences. A string
containing non-obvious characters and be enclosed either with single
or double quotes (there being no difference between these types of quotes),
and inside the quotes, C++ string rules exactly apply. So "\t":"\001" is a valid
key-value pair. The list of ssim characters not requiring quotes is given by
`acr charset:%.SsimQuotesafe`.

A single file can contain tuples of any type.
An OpenACR process can accept tuples on input, convert it to binary representation, automatically
place the record in the in-memory database and index it with global and partitioned indexes.
