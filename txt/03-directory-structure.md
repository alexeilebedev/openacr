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

Intermediate binaries are kept in dflt.debug-x86_64 or dflt.release-x86_64

### Binaries

All executable files are in bin (both scripts and binary executables).
Binaries are compiled with abt -build and installed with abt -install.

### Editor configuration files

See files in conf/ for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up:

    ln -sf $PWD/conf/emacs.el ~/.emacs
    ln -sf $PWD/conf/elisp ~/elisp
    ln -sf $PWD/conf/vimrc ~/.vimrc
    ln -sf $PWD/conf/vim ~/.vim

