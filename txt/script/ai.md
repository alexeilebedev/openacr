## bin/ai - Bootstrapped compilation script


### Usage: `ai [regx]`
<a href="#usage--ai--regx--"></a>
Normally, all compilation is handled by [abt](/txt/exe/abt/README.md).
But initially, `abt` isn't built, because it's an executable.
This is where `ai` comes in (despite its name, it has nothing to do with machine learning).

`ai` will choose a bootstrap file by looking at what kind of system you're running
on and what compilers you have. It won't install pre-requisites, for that, see
[pre-requisites](/txt/start/setup.md#setup-and-installation).

The bootstrap files are located in bin/bootstrap.

`ai` builds `abt` and `gcache` using using a canned script, then switches to abt
to build the rest. If any of this fails, you may need to file a bug report.
Any options you pass to `ai` are passed to the child `abt` process.

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

If you manually run a bootstrap script (e.g. bin/bootstrap/Linux-g++.release-x86_64),
then soft links will be created under `build` so that this configuration is picked by default
whenever abt subsequently runs.

### Sources
<a href="#sources"></a>
The source code license is GPL

Source file: [bin/ai](/bin/ai)

