## Setup and Installation

Presently, this project has been tested on the following distributions / compilers:

* OS: RHEL, CentOS, Ubuntu
* g++: 4.8, 8.3, 9
* clang: 3.4.2

The MariaDB and OpenSSL packages are required in order to build mysql2ssim and ssim2mysql tools.

    yum install -y mariadb mariadb-devel mariadb-server

And 

    apt install -y mariadb libmariadb-dev libmariadbd-dev libssl-dev
    apt install llvm llvm-dev  # to enable abt -compiler llvm
    
All commands can be issued from this, top-level directory.
Just add the relative path bin/ to your path.

    set PATH=$PATH:bin/

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

The default compiler abt uses is g++ prior to verison 9.
You will need to set the environment variable `COMPILER` to `g++-9` or `clang++` if 
you want to use those compilers. See the section on [abt](#txt/abt-a-build-tool) for more information.

This should build abt using a bootstrapped shell script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

### Platform Short List

Support for g++ 8.x, clang, other Linux distributions, FreeBSD, Windows (Cygwin) and Darwin
is very must desired, so if you'd like to help, please contact alexei@lebe.dev

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
* COMPILER, UNAME, CFG, ARCH - defaults for abt [abt](#txt/abt-a-build-tool) section.

### Known Issues

Currently, optimization levels `-O2` and higher cannot be used with gcc 8.
