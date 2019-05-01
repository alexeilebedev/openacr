## Setup and Installation

Presently, the project only builds on CentOS Linux. It has been tested on

* RHEL 7.0
* RHEL 7.3
* CentOS 7.6

The MariaDB packages are required in order to build mysql2ssim and ssim2mysql tools.

    yum install -y mariadb mariadb-devel mariadb-server
    
All commands can be issued from this, top-level directory.
Just add the relative path bin/ to your path.

    set PATH=$PATH:bin/

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

This should build abt using a bootstrapped shell script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

The home for this project is at https://github.com/alexeilebedev/openacr

After you run ai, you will see a couple of directories appear:

    drwxr-xr-x  2 alexei users  4096 Apr 29 10:57 dflt.debug-x86_64
    drwxr-xr-x  2 alexei users 12288 Apr 30 17:52 dflt.release-x86_64

These contain all object, library and executable files -- outputs of abt.
Each directory is just a flat set of files. Object file names are created by
replacing '/' with '.' in the relative path to the source:

    $ ls -l dflt.release-x86_64/ | grep abt
    -rwxr-xr-x 1 alexei users   335848 Apr 30 17:51 abt
    -rw-r--r-- 1 alexei users    14952 Apr 30 17:51 cpp.abt.build.o
    -rw-r--r-- 1 alexei users     9248 Apr 30 17:51 cpp.abt.disas.o
    -rw-r--r-- 1 alexei users    71528 Apr 30 17:51 cpp.abt.main.o
    -rw-r--r-- 1 alexei users    12776 Apr 30 17:51 cpp.abt.opt.o
    -rw-r--r-- 1 alexei users   232360 Apr 30 17:51 cpp.gen.abt_gen.o

These temporary directories shouldn't be in the path, instead
just bin/ is in the path. When compiling any executable with -install flag, abt creates
a soft link under bin/ pointing to one of these directories. This allows 'installing'
debug binaries, or profile binaries, or binaries built with another compiler.
At the time of writing, gcc and clang are supported as compilers.
To undo any non-standard installation, type 'ai'. This will reinstall all binaries
using the release configuration.

### Tests

There are two kinds of tests in OpenACR, unit tests and normalization tests.
Unit tests just check that some functions do what they should, and they can be invoked
with

    atf_unit

Normalization tests enforce various invariants on this repository, such as checking
that all files have an entry in the gitfile table, or round-tripping all data values
through a temporary MariaDB instance to ensure portability. These are invoked with

    atf_norm

All tests together are died in the script called normalize

    normalize

If this script succeeds, your latest changes are OK (relatively speaking).

