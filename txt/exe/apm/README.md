## apm - Algo Package Manager
<a href="#apm"></a>

APM allows installing, removing, downloading, updating, and managing OpenACR packages.
An OpenACR package is a source-level distribution.
Each package consists of a list of files and ssim record keys that implicitly define what ssim records
belong to the package.
When installed, the package is merged into the current repository tree at the source level, resulting in
a diff that must be committed with git to complete the installation.
A `dev.package` record is created to record the transaction, and table `dev.pkgkey` describes
the files and records belonging to the package.

Since OpenACR is a meta-programming system, the installed packages are integrated directly into the working tree
instead of residing in separate directories. An installed package potentially globally affects generated
code and as far as development is concerned, becomes an indistinguishable part of the repo. A package can modify
an executable belonging to a different package by adding its own source file and related records.

By design, packages are separable. Each package may be installed/uninstalled independently.
After being installed, you can create files and records that will be considered local modifications to the installed package.
These differences can be examined (with `apm -diff`). The diffs are calculated with with respect
to the package base version (as stored in the `dev.package.baseref` field).

Packages can have dependencies, as described in the `dev.pkgdep` table.
When installing a package, its dependencies are installed as well. When removing a package, its dependent packages
are removed as part of the transaction.

Logically, ssimfiles are just collections of set elements. Thus, one ssimfile may contain records from
different packages, as determined by `pkgkey` table.
`apm` takes care of merging changes made to ssimfiles, reducing merge conflicts to a minimum.

Apm doesn't require the two projects to have any common history; All that's required is the gitref
in the package repo corresponding to the last synchronization point.

### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Limitations](#limitations)
* [Package definition](#package-definition)
* [Merge conflicts](#merge-conflicts)
* [Sandboxes](#sandboxes)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
apm: Algo Package Manager
Usage: apm [[-package:]<regx>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    [package]    regx    ""      Regx of package
    -install                     Install new package (specify -origin)
    -update                      Update new package (-origin)
    -list                        List installed packages
    -diff                        Diff package with respect to installed version
    -push                        Evaluate package diff and push it to origin
    -check                       Consistency check
    -remove                      Remove specified package
    -origin      string  ""      Upstream URL of new package
    -ref         string  ""      (with -create) Gitref or branch to fetch
    -dry_run                     Do not execute transaction
    -showrec                     Show records belonging to package
    -showfile                    List package files (gitfile records)
    -R                           reverse the diff direction
    -reset                       Reset package baseref/origin to those provided by the command line
    -checkclean          Y       Ensure that changes are applied to a clean directory
    -t                           Select dependent packages for operation
    -stat                        (with -diff) show stats
    -annotate    string  ""      Read file and annotate each input tuple with package(s) it belongs to
    -gen                 Y       Show differences in generated files
    -data_in     string  "data"  Dataset from which package records are loaded
    -e                           Open selected records in editor
    -binpath     string  "bin"   (internal use)
    -nosort                      (with -showrec) don't sort resulting records
    -verbose     int             Verbosity level (0..255); alias -v; cumulative
    -debug       int             Debug level (0..255); alias -d; cumulative
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -package -- Regx of package
<a href="#-package"></a>

#### -install -- Install new package (specify -origin)
<a href="#-install"></a>

#### -update -- Update new package (-origin)
<a href="#-update"></a>

#### -list -- List installed packages
<a href="#-list"></a>

#### -diff -- Diff package with respect to installed version
<a href="#-diff"></a>

#### -push -- Evaluate package diff and push it to origin
<a href="#-push"></a>

#### -check -- Consistency check
<a href="#-check"></a>

#### -remove -- Remove specified package
<a href="#-remove"></a>

#### -origin -- Upstream URL of new package
<a href="#-origin"></a>

#### -ref -- (with -create) Gitref or branch to fetch
<a href="#-ref"></a>

#### -dry_run -- Do not execute transaction
<a href="#-dry_run"></a>

#### -showrec -- Show records belonging to package
<a href="#-showrec"></a>

#### -showfile -- List package files (gitfile records)
<a href="#-showfile"></a>

#### -R -- reverse the diff direction
<a href="#-r"></a>

#### -reset -- Reset package baseref/origin to those provided by the command line
<a href="#-reset"></a>

#### -checkclean -- Ensure that changes are applied to a clean directory
<a href="#-checkclean"></a>

#### -t -- Select dependent packages for operation
<a href="#-t"></a>

#### -stat -- (with -diff) show stats
<a href="#-stat"></a>

#### -annotate -- Read file and annotate each input tuple with package(s) it belongs to
<a href="#-annotate"></a>

#### -gen -- Show differences in generated files
<a href="#-gen"></a>

#### -data_in -- Dataset from which package records are loaded
<a href="#-data_in"></a>

#### -e -- Open selected records in editor
<a href="#-e"></a>

#### -binpath -- (internal use)
<a href="#-binpath"></a>

#### -nosort -- (with -showrec) don't sort resulting records
<a href="#-nosort"></a>

### Limitations
<a href="#limitations"></a>

Apm doesn't track file renames.

Currently, if you make a local change to a package file, and the file is later renamed in the package origin
repository, your file will be silently deleted upon update. The git history will still have your changes
and you will need to dig them out and re-apply them to the new file.

Similarly, if you rename a package file, and the file changes in the upstream package, the file will
not be updated because it is considered "deleted".

#### -package -- Regx of package
<a href="#-package"></a>

Controls selection of packages for operation.
For `-install`, a specific name must be used (not a regx)
x

#### -install -- Install new package (specify -origin)
<a href="#-install"></a>

```
apm <packagename> -install
git commit -m <message>
```

If the installed package is dependent on other packages (as described in `dev.pkgdep` table)
on the remote side, then all dependent packages are installed as well. The `pkgdep` field `soft` controls whether
a package dependency applies to installation. Most packages will specify a dependency on the `openacr` base package
with `soft:Y`, meaning that updating given package should not entail updating all of opernacr.

#### -update -- Update new package (-origin)
<a href="#-update"></a>

```
apm <packagename> -update
git commit -m <message>
```

This command updates the package and any packages on which the package depends on the remote side, as described
in `pkgdep` table.

#### -diff -- Diff package with respect to installed version
<a href="#-diff"></a>

```
apm <packagename> -diff [-R]
```

The differences between base package and current version are shown, i.e. any local additions
will be maked with "+". With `-R`, the diff direction is reversed.
The options `-showrec`, `-showfile` can be used to constrain the shown difference.

#### -remove -- Remove specified package
<a href="#-remove"></a>

```
apm <packagename> -remove
git commit -m <message>
```

When removing a package, any local modifications (added files and records) are removed
as well. Any packages which depend on the removed package are also removed.
Example:

#### -showrec -- Show records belonging to package
<a href="#-showrec"></a>

Print all records comprising the package. For files, `gitfile` record is printed.
With `-t` option, any records from dependent packages are included as well.

```
apm <packagename> -showrec
```

#### -reset -- Reset package baseref/origin to those provided by the command line
<a href="#-reset"></a>

The reset command can be used to change the dev.package record to
update the package origin URL or the baseref. Use with caution.

```
apm <packagename> -reset [-origin <URL>] [-ref <baseref>]
```

#### -checkclean -- Ensure that changes are applied to a clean directory
<a href="#-checkclean"></a>

By default, `-install` and `-update` will refuse installation if the current
directory is not clean (as defined by `git ls-files -m`). This option can disable
the check.

#### -t -- Select dependent packages for operation
<a href="#-t"></a>

With `-showrec` or `-showfile`, also select packages on which currently
selected packages depend. When installing a package, any dependencies (as defined
remotely in the origin repo) are automatically installed.

#### -stat -- (with -diff) show stats
<a href="#-stat"></a>

Show `git diff --stat` -like output when used with `-diff`.

#### -annotate -- Read file and annotate each input tuple with package(s) it belongs to
<a href="#-annotate"></a>

Apm can read a file with ssim tuples and annotate each tuple with the names of packages to which
this tuple belongs.
Example:
```
acr ctype:command.xyz -t | apm -annotate
...
```

### Package definition
<a href="#package-definition"></a>

A package is defined by the `dev.package` record. The record specifies package name, git commit
corresponding to the base version of the package. The ref DOES NOT refer to the git history
of the current repo, it refers to the history of the ORIGIN's repo. The table `dev.pkgkey` specifies
which files and records are part of the package.

You can define a new package by manually creating these records, then commit the changes. Use "." for origin,
and HEAD for baseref. The repo now defines the base version of the package for others to use.
Other projects that want to install the package defined in your repo can use `apm -install <packagename> -origin <url>`
to fetch objects in your repo, extract package contents, and apply them, recording the git commit of your repo from which the
data was obtained. From that point, they can use `apm -update <packagename>` to apply any new changes that may have
been published. If a project has a package, it now has the full definition of the package, and can serve as an
installation source for other projects.

Each pkgkey record is an acr regex, and apm expands it to the full set of records
from `data_in` dataset when it is invoked. This is done in two stages. First, apm evaluates the regex to
a set of records. Then it finds all references to this set of records.
A given ssim record can end up "belonging" to more than one package. In this case, it is taken to belong to the
"last" package that claims it, in topological order by package dependency. So, if package "openacr" selects files
"cpp/amc/%" and another package X extends amc by adding an extra source file to it and selects "cpp/amc/xyz.cpp",
then the file xyz.cpp will belong to package X and not openacr.

You can examine which packages any record belongs to by streaming these records to `apm -annotate -`.
E.g. `acr citest | apm -annotate -`.

Defining a package can be a tricky process when package files modify executables in other packages.
The executable in question must have a plug-in nature for this to succeed.
It is possible to take a source file, for instance `cpp/atf/ci/apm.cpp` and assign it to the `apm`
package. But if the new source file needs its own internal tables, these records must be marked
as belonging to the package as well. Use `-annotate` option to examine how records are assigned
to packages.

#### External repos
<a href="#external-repos"></a>

`apm` attempts to invoke `apm` in the package directory to get a list of files and records
comprising a package. That's because the package directory has its own definition of the package
file and over times files/records may be added or removed from it. 

But `apm` can also work with any unrelated repo that doesn't have apm, even if that repo is not openacr based
or has no concept of ssimfiles. Running `apm <package> -diff -origin:<URL> -ref:<baseref>` will use the local
package definition and evaluate it in the context of the package repo using the current apm binary.
Thus, apm works as a generic tool for managing submodules.

### Merge conflicts
<a href="#merge-conflicts"></a>

Apm works with files and with records. When merging changes, `apm` uses git command `git merge-file`
to perform a 3-way merge on files, and the `acr_dm` command to perform attribute-level 3-way merge
on tuples. 

Any conflicts in regular files are dealt with using usual git methods -- just edit the
conflict markers and commit.

Conflicts resulting from failed `acr_dm` merge are left
in the temporary file `temp/apm.acrtxn.ssim`. This file must be manually edited after a failed
update, and the results commited with `acr -insert -write < temp/apm.acrtxn.ssim`. `apm` will
repeat these instructions after a failed update. `acr_dm` is aware that ssimfiles are sets,
so it treats them accordingly: it doesn't consider changes to neighboring
lines to be a conflict, because they're not.
It doesn't even consider changes to different attributes of the same tuple
to be a conflict.

### Sandboxes
<a href="#sandboxes"></a>

apm uses [sandboxes](txt/sandbox.md) to hold intermediate state.
The new package is always fetched into the `apm-theirs` sandbox, and the common ancestor is instantiated
in `apm-base` sandbox.

After running the `-diff` command, the sandbox `apm-base` contains the published
version of the package.
After running `-update`, `apm-theirs` contains the new version and `apm-base` contains the common
ancestor version. This is useful when resolving conflicts.
The `-install` command is essentially the same as `-update` with current commit as the common ancestor.

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/apm/annotate.cpp](/cpp/apm/annotate.cpp)||
|[cpp/apm/check.cpp](/cpp/apm/check.cpp)||
|[cpp/apm/diff.cpp](/cpp/apm/diff.cpp)||
|[cpp/apm/install.cpp](/cpp/apm/install.cpp)||
|[cpp/apm/main.cpp](/cpp/apm/main.cpp)||
|[cpp/apm/push.cpp](/cpp/apm/push.cpp)||
|[cpp/apm/rec.cpp](/cpp/apm/rec.cpp)||
|[cpp/apm/remove.cpp](/cpp/apm/remove.cpp)||
|[cpp/apm/reset.cpp](/cpp/apm/reset.cpp)||
|[cpp/apm/show.cpp](/cpp/apm/show.cpp)||
|[cpp/apm/update.cpp](/cpp/apm/update.cpp)||
|[cpp/gen/apm_gen.cpp](/cpp/gen/apm_gen.cpp)||
|[include/apm.h](/include/apm.h)||
|[include/gen/apm_gen.h](/include/gen/apm_gen.h)||
|[include/gen/apm_gen.inl.h](/include/gen/apm_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`apm` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|Extended constraints for ssim records|
|[dev.package](/txt/ssimdb/dev/package.md)|OpenACR package|
|[dev.pkgkey](/txt/ssimdb/dev/pkgkey.md)|Keys belonging to the OpenACR package|
|[dev.pkgdep](/txt/ssimdb/dev/pkgdep.md)|OpenACR Package dependency|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

