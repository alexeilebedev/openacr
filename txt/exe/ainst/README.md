## ainst - Install third-party software described by dev.extpkg
<a href="#ainst"></a>

`ainst` installs the third-party software this tree depends on, about 35
packages from kafka and minio to the aws client and a kernel module.  It writes
each install as a bash program, and it can print that program, run it, or run it
under a staging root and package the result as an rpm.  The printed form is what
an image build uses, since it runs on a machine with no binary of this tree.

### Syntax
<a href="#syntax"></a>
```usage
ainst: Install third-party software described by dev.extpkg
Usage: ainst [[-extpkg:]<regx>] [options]
    OPTION      TYPE    DFLT                COMMENT
    -in         string  "data"              Input directory or filename, - for stdin
    [extpkg]    regx    "%"                 (select) Packages to act on
    -install                                (action) Install the selected packages on this host
    -script                                 (action) Print the install script instead of running it
    -lib                                    (action) Print the install functions of the selected packages as a shell library
    -rpm                                    (action) Stage each package and build an rpm out of the staged tree
    -update                                 (action) Look upstream for a newer release and record its version, url and sha256
    -root       string  ""                  Stage files under this directory instead of /
    -cache      string  "temp/ainst/cache"  Where verified artifacts are kept, one file per sha256
    -outdir     string  "temp/ainst"        (with -rpm) Directory the rpm is written to
    -dry_run                                Print actions, do not perform
    -verbose    flag                        Verbosity level (0..255); alias -v; cumulative
    -debug      flag                        Debug level (0..255); alias -d; cumulative
    -trace      string  ""                  Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                                   Print help and exit; alias -h
    -version                                Print version and exit
    -signature                              Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Select packages by name with the positional regx and pick one action.  With no action, `ainst` prints the selected
`ssimfile:dev.extpkg` rows.

#### What a package is made of
<a href="#what-a-package-is-made-of"></a>

A package here is an external package: third-party software this tree installs
but does not build, named `extpkg` to keep it apart from an apm package.  It is a
row of `ssimfile:dev.extpkg` and a C++ function in `cpp/ainst/<extpkg>.cpp` that
writes its install steps.  The row carries a `probe`,
a shell command that exits 0 on a host that holds the package.  The release it
pins is a row of `ssimfile:dev.extpkgver`, whose `upstream` field says where
newer releases appear.  Each file it downloads is a row of
`ssimfile:dev.extpkgsrc`, keyed `<extpkg>/<name>`, with the url and the sha256
the bytes must have.  `ainst` knows nothing about images: an image builder keeps
its own list of the packages an image installs, asks `ainst -lib` for their
functions, and writes the calls in the image's order.

The generated script fetches every artifact first, checks its sha256, and keeps
it in the `-cache` directory under its hash.  A fetch whose bytes do not match
stops the script with `ainst.badsha`.  The sha256 covers the fetches only: post
steps also reach the network through `apt-get install`, `pip install`,
`npm install` and `git clone`, and nothing checks what those bring in.

#### Staging and post-install steps
<a href="#staging-and-post-install-steps"></a>

A package's steps fall into two groups.  Staging steps place files under the
staging root, and they are what an rpm packages.  Post-install steps act on the
live system: adding a user, enabling a systemd unit, running `apt-get install`.
Each package becomes three shell functions: `inst_<pkg>_stage`,
`inst_<pkg>_post`, and `inst_<pkg>` which calls the two in order.

A package function writes staging steps with `ainst::AddStage` and post steps
with `ainst::AddPost`.  `ainst::GetPkgsrcVar` names the verified local path of
one artifact.  `ainst::AddStageFileFrom` copies a file from
`conf/ainst/<pkg>/` into the script, for a payload longer than a line or two
such as an init script or a systemd unit.  The staging steps use six shell
verbs, which apply the staging root:

|verb|answers with, or does|
|---|---|
|`ainst_fetch <url> <sha256>`|the local path of one verified artifact|
|`ainst_unpack <archive>`|the directory the archive unpacked into|
|`ainst_install <mode> <src> <dest>`|places a file under the staging root|
|`ainst_write <dest>`|writes stdin to a file under the staging root|
|`ainst_mkdir <dir>`|creates a directory under the staging root|
|`ainst_root <path>`|the path, under the staging root|

#### Installing and printing
<a href="#installing-and-printing"></a>

`-script` prints the whole program: a preamble with the verbs, the functions of
every selected package, and the calls in order.  `-install` writes the same
program to `<outdir>/ainst.sh` and runs it with bash.  `-lib` prints the
functions without the calls, for a script that sources them and decides at run
time which packages to install.

#### Building an rpm
<a href="#building-an-rpm"></a>

`-rpm` stages each selected package into `<outdir>/buildroot/<pkg>` and hands
that directory to `rpmbuild`.  The rpm lands in `<outdir>/rpm`, and it carries
exactly the files a live install places.  Its `%files` is the staged tree, its
`%post` is the package's post-install steps, and its `Version` is the pinned
version with dashes turned to underscores.  A package that only adds a vendor
apt repository stages nearly nothing, so its rpm is a small `%files` and a
`%post` that runs apt.  `rpmbuild` comes from the `rpm` deb.

#### Following a new upstream release
<a href="#following-a-new-upstream-release"></a>

`-update` asks each selected package's upstream for its newest release.  When
that release is newer than the pin, `ainst` rewrites every source url, fetches
and hashes the new bytes, and records the new version.  It writes the rows
through `acr`, one package at a time.

The `upstream` field reads `<scheme>:<locator>`, and `ssimfile:dev.upstreamtype`
lists the schemes.  `github:<owner>/<repo>` reads the repo's latest release and
falls back to its newest tag.  A url is rewritten by replacing the old version
string with the new one, so `v3.11.0` and `3.11.0` both move.  Each package
answers with one line:

|line|meaning|
|---|---|
|`ainst.update`|moved; every source was refetched and rehashed|
|`ainst.current`|upstream names the release already pinned|
|`ainst.notnewer`|upstream names an older release than the pin, which is left alone|
|`ainst.noupstream`|the package records nowhere to look|
|`ainst.badupstream`|no `ssimfile:dev.upstreamtype` row reads this scheme|
|`ainst.unreadable`|upstream answered nothing; it may be down, rate-limiting, or renamed|
|`ainst.verinsrc`|`cpp/ainst` spells the version too, so rows alone cannot move it|
|`ainst.nosubst`, `ainst.nofetch`|a source url could not be rebuilt or does not answer|
|`ainst.skip`|follows any of the last three; the whole package stays at its release|

A package built from source names its version in C++ as well, such as the
directory an archive unpacks into.  It answers `ainst.verinsrc`, and bumping it
takes a row edit and a one-line source edit together.  `edenhill/kcat` answers
`ainst.notnewer` every time, because its newest marked release is older than
the tag it is pinned to.

#### Adding a package
<a href="#adding-a-package"></a>

Insert the package, its pinned version, and one source row per file it
downloads.  Use underscores in the key, never dashes, because the key becomes
part of a C function name.

```bash
echo 'dev.extpkg  pkg:mytool  probe:"command -v mytool >/dev/null"  comment:"what it is"' | acr -insert -check -write
echo 'dev.extpkgver  pkg:mytool  version:1.2.3  upstream:github:<owner>/<repo>  comment:""' | acr -insert -check -write
curl -fsSL <url> | sha256sum
echo 'dev.extpkgsrc  pkgsrc:mytool/bin  url:"<url>"  sha256:<hash>  comment:""' | acr -insert -check -write
```

Create the source file with `acr_ed -create -srcfile:cpp/ainst/mytool.cpp
-target:ainst -write`, which registers it with the build, and write exactly one
function in it, `ainst::extpkg_mytool`.  Then run `amc` and
`abt ainst -build -install`.  A citest checks that each
`ainst::extpkg_<extpkg>` sits in `cpp/ainst/<extpkg>.cpp` and that every package has a
`ssimfile:dev.extpkgver` row.  Then attach the package to the images that want
it:

```bash
echo 'imgdb.pkgimage  pkgimage:mytool/user  comment:""' | acr -insert -check -write
```

### Examples
<a href="#examples"></a>

```bash
ainst                                   # list every package row
ainst shfmt -script                     # print the install program for shfmt
acr imgdb.pkgimage:%/user           # the packages image user installs, in order
ainst mc -install                       # install mc on this host
ainst mc -install -dry_run              # write temp/ainst/ainst.sh and print the command that would run it
ainst mc -install -root:/tmp/stage      # stage mc's files under /tmp/stage
ainst mc -rpm                           # build temp/ainst/rpm/.../mc-*.rpm
ainst -lib > ainst-lib.sh               # print every package's functions as a sourceable library
ainst -update -dry_run                  # survey every package for newer releases
ainst shfmt -update                     # move shfmt to upstream's current release
```

The program for one package reads like this, after the preamble:

```bash
$ ainst shfmt -script
...
# shfmt 3.11.0 -- shell formatter
inst_shfmt_stage() {
    art_shfmt_bin=$(ainst_fetch https://github.com/mvdan/sh/releases/download/v3.11.0/shfmt_v3.11.0_linux_amd64 1904ec6bac715c1d05cd7f6612eec8f67a625c3749cb327e5bfb4127d09035ff)
    ainst_install 755 "$art_shfmt_bin" /usr/local/bin/shfmt
    :
}

inst_shfmt_post() {
    :
}

inst_shfmt() {
    inst_shfmt_stage
    inst_shfmt_post
}

# -----------------------------------------------------------------------------
inst_shfmt
```

### Caveats
<a href="#caveats"></a>

- `-root` moves the staged files only.  `-install -root:<dir>` still runs every
  post-install step against the live system.
- `-update -dry_run` writes no rows, but it still downloads each new release to
  compute its sha256.
- `-install` and `-rpm` run the generated script under `set -euo pipefail`, so
  the first failing command stops the whole install.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The dataset `ainst` reads its package tables from.  The package names and the
upstream types are compiled into `ainst`, and every other table is read from here
at run time.

#### -extpkg -- (select) Packages to act on
<a href="#-extpkg"></a>

A regx over package names, all of them by default.  Packages run in name order.

#### -install -- (action) Install the selected packages on this host
<a href="#-install"></a>

Write the install program to `<outdir>/ainst.sh` and run it with bash.  It
needs root for most packages, since they write under `/usr` and `/etc`.

#### -script -- (action) Print the install script instead of running it
<a href="#-script"></a>

Print the whole install program to stdout.  An image build pastes this text into
its own script.

#### -lib -- (action) Print the install functions of the selected packages as a shell library
<a href="#-lib"></a>

Print the preamble and the three functions of every selected package, with no
line that calls one.  A caller sources the library and calls `inst_<pkg>` for
the packages it decides at run time to install.

#### -rpm -- (action) Stage each package and build an rpm out of the staged tree
<a href="#-rpm"></a>

Stage each selected package into its own buildroot under `-outdir` and build an
rpm from it with `rpmbuild`.  The rpms land in `<outdir>/rpm`.

#### -update -- (action) Look upstream for a newer release and record its version, url and sha256
<a href="#-update"></a>

Ask each selected package's upstream for a newer release, and when there is one,
rewrite its `ssimfile:dev.extpkgsrc` rows and its pinned version.  A package that
cannot move whole stays where it was.  Add `-dry_run` to see what would move.

#### -root -- Stage files under this directory instead of /
<a href="#-root"></a>

The staging root the generated program writes files under.  The program reads
it as `ROOT`, and a `ROOT` set in the environment overrides it when the script
runs.

#### -cache -- Where verified artifacts are kept, one file per sha256
<a href="#-cache"></a>

The directory downloads are kept in, each named by its sha256, so the same bytes
are fetched once.  The program reads it as `AINST_CACHE`, which the environment
can override.  An rpm's `%post` uses a fresh temporary directory.

#### -outdir -- (with -rpm) Directory the rpm is written to
<a href="#-outdir"></a>

Where `ainst` writes its working files: `ainst.sh` for `-install` and `-rpm`,
the spec files, buildroots and rpms for `-rpm`, and `update.ssim` for
`-update`.

#### -dry_run -- Print actions, do not perform
<a href="#-dry_run"></a>

Print the command that would run and do not run it.  With `-install` or `-rpm`
the script is still written to `<outdir>/ainst.sh` for you to read.  With
`-update` it prints the `acr` command that would record the rows.
