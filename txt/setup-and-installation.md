## Setup and Installation

Presently, this project has been tested on the following distributions / compilers:

* OS: RHEL, CentOS, Ubuntu
* g++: 4.8, 8.3, 9
* clang: 3.4.2
* MacOS: LLVM 10.0.1
* FreeBSD: Clang 6.0.0

The MariaDB and OpenSSL packages are required in order to build mysql2ssim and ssim2mysql tools.

### Pre-requisites: CentOS:

    yum install -y mariadb mariadb-devel mariadb-server

### Pre-requisites: Ubuntu/Debian

    apt install -y mariadb libmariadb-dev libmariadbd-dev libssl-dev
    apt install llvm llvm-dev  # to enable abt -compiler llvm

### Pre-requisites: MacOS

Install brew.
Then,

    brew install mariadb openssl
    ln -s /usr/local/opt/openssl/lib/libcrypto.a /usr/local/lib/
    ln -s /usr/local/opt/openssl/lib/libssl.a /usr/local/lib/

Please note that mariadb, not mysql should be installed.
Since MariaDB is the successor to `mysql`, there shouldn't be a problem.

### Path

All commands can be issued from this, top-level directory.
Add the relative path bin/ to your path.

    set PATH=$PATH:bin/

### Building

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

The default compiler abt uses is g++ prior to verison 9.
You will need to set the environment variable `COMPILER` to `g++-9` or `clang++` if 
you want to use those compilers. See the section on [abt](#txt/abt-a-build-tool) for more information.

This should build abt using a bootstrapped shell script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

### Platform Short List

Support FreeBSD and Windows (Cygwin) 
is very must desired, so if you'd like to help, please send pull requests.

### Editor configuration files

See files in conf/ for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up.

    ln -s $PWD/conf/emacs.el ~/.emacs
    ln -s $PWD/conf/elisp ~/elisp
    ln -s $PWD/conf/vimrc ~/.vimrc
    ln -s $PWD/conf/vim ~/.vim
    ln -s $PWD/conf/alexei/bash_profile ~/.bash_profile

### Environment Variables

* EDITOR - standard environment variable specifying which editor to use

### Known Issues

Currently, optimization levels `-O2` and higher cannot be used with gcc 8 and higher,
due to the way the optimizer results in corruptions.


