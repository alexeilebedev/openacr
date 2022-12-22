## Setup and Installation

Presently, this project has been tested with the following distributions / compilers:

* clang on MacOS (a.k.a. Darwin)
* clang on FreeBSD
* clang on Linux (RHEL, CentOS, Ubuntu, Debian)
* g++ on Linux
* g++-9 on Linux

The MariaDB and OpenSSL packages are the main external dependencies.
MariaDB (formerly MySQL) is not really required, but it's used by `acr_my`, `ssim2mysql` and
`mysql2ssim` utilities. OpenSSL provides `libcrypto` which is for SHA1 functions.

### Pre-requisites: CentOS:

    yum install -y mariadb mariadb-devel mariadb-server

### Pre-requisites: Ubuntu/Debian

    apt install -y mariadb-server mariadb-client libmariadb-dev libmariadbd-dev \
        libssl-dev libcurl4-openssl-dev liblz4-dev cppcheck
    apt install llvm llvm-dev  # to enable abt -compiler llvm

### Pre-requisites: MacOS

Install brew. Then,

    brew install mariadb openssl
    ln -s /usr/local/opt/openssl/lib/libcrypto.a /usr/local/lib/
    ln -s /usr/local/opt/openssl/lib/libssl.a /usr/local/lib/

### Path

Add the relative path bin to your path.
All commands are issued from this, top-level directory. This normalizes all 
pathnames to a single form.

    set PATH=$PATH:bin

### Building

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

`ai` will choose a bootstrap file by looking at what kind of system you're running
on and what compilers you have. It won't install pre-requisites, that fun exercise
is left to the reader. See the section on [abt](#txt/abt-a-build-tool) for complete
documentation of this process.

`ai` should build abt using a canned script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

### Cygwin Support

Support of Windows (Cygwin) is needed, so if you'd like to help, please send pull requests.

### Editor configuration files

See files in `conf/` for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up. You may already have some of these files, so you'll need
to decide whether you want to merge provided configs with yours, replace or do something else.

    ln -s $PWD/conf/emacs.el ~/.emacs
    ln -s $PWD/conf/elisp ~/elisp
    ln -s $PWD/conf/vimrc ~/.vimrc
    ln -s $PWD/conf/vim ~/.vim
    ln -s $PWD/conf/alexei/bash_profile ~/.bash_profile

### Environment Variables

* EDITOR - standard environment variable specifying which editor to use.

### Known Issues

Currently, optimization levels `-O2` and higher cannot be used with gcc 8 and higher,
due to the way the optimizer results in corruptions.

