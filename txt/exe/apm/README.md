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

Each dependency also names *how* the two packages stand to each other, through
`dev.pkgdeptype`:

|`pkgdeptype`|Meaning|
|---|---|
|`contain`|The parent distributes this package, so the parent's content covers it.|
|`extend`|This package builds on the parent without shipping in it, so the parent's content excludes whatever this package captures.|
|`require`|This package needs the parent built first, and neither one's content touches the other's.|

`extend` is what keeps a downstream tree out of an upstream distribution. A
package that declares itself an extension of a base is not published as part of
that base, and that holds without the base's definition listing the downstream
package's namespaces one by one. The exclusion follows from the relation, and
the records follow from whatever the extending package claims.

Two rules decide the edges. A record a package **names outright** -- a pkgkey
with no pattern in its value, such as `dev.netproto:https` -- stays with that
package even when an extender's reference closure reaches it; a blanket like
`dev.%:%` does not override the relation. And the subtraction runs after every
package has been evaluated, because packages are evaluated parents first and an
extender's records do not exist yet while its parent is being built.

`apm -check` reports a namespace claimed by both a package and one of its
extenders as `apm.doubleclaim`. That is a modelling error rather than a
preference: one of the two packages does not own the namespace, and until it is
settled the extender's claim quietly removes the parent's records from the
distribution.

Logically, ssimfiles are just collections of set elements. Thus, one ssimfile may contain records from
different packages, as determined by `pkgkey` table.
`apm` takes care of merging changes made to ssimfiles, reducing merge conflicts to a minimum.

Apm doesn't require the two projects to have any common history; All that's required is the gitref
in the package repo corresponding to the last synchronization point.

### Syntax
<a href="#syntax"></a>
```usage
apm: Algo Package Manager
Usage: apm [[-package:]<regx>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    -pkgdata     string  ""      Load package definitions from here
    [package]    regx    ""      Regx of package
    -ns          regx    ""      Operate on specified namespace only
    -install                     Install new package (specify -origin)
    -update                      Update package (-origin)
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
    -generate                    Generate package records into apm/gen/<package>.ssim
    -R                           reverse the diff direction
    -l                           Use local package definition on the remote side
    -reset                       Reset package baseref/origin to those provided by the command line
    -checkclean          Y       Ensure that changes are applied to a clean directory
    -t                           Select parent packages for operation
    -stat                        (with -diff) show stats
    -annotate    string  ""      Read file and annotate each input tuple with package(s) it belongs to
    -data_in     string  "data"  Dataset from which package records are loaded
    -e                           Open selected records in editor
    -binpath     string  "bin"   (internal use)
    -verbose     flag            Verbosity level (0..255); alias -v; cumulative
    -debug       flag            Debug level (0..255); alias -d; cumulative
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### Limitations
<a href="#limitations"></a>

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

```bash
apm <packagename> -install
git commit -m <message>
```

If the installed package is dependent on other packages (as described in `dev.pkgdep` table)
on the remote side, then all dependent packages are installed as well. The `pkgdep` field `soft` controls whether
a package dependency applies to installation. Most packages will specify a dependency on the `openacr` base package
with `soft:Y`, meaning that updating given package should not entail updating all of opernacr.

#### -update -- Update new package (-origin)
<a href="#-update"></a>

```bash
apm <packagename> -update
git commit -m <message>
```

This command updates the package and any packages on which the package depends on the remote side, as described
in `pkgdep` table.

#### -diff -- Diff package with respect to installed version
<a href="#-diff"></a>

```bash
apm <packagename> -diff [-R]
```

The differences between base package and current version are shown, i.e. any local additions
will be maked with "+". With `-R`, the diff direction is reversed.
The options `-showrec`, `-showfile` can be used to constrain the shown difference.

#### -remove -- Remove specified package
<a href="#-remove"></a>

```bash
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

```bash
apm <packagename> -showrec
```

#### -reset -- Reset package baseref/origin to those provided by the command line
<a href="#-reset"></a>

The reset command changes the `dev.package` record, so that a package can be
pointed at a different origin, or declared to match a different version of it.
Use with caution: the baseref is what every later merge is computed against,
and moving it declares a synchronization that may not have happened.

```bash
apm <packagename> -reset [-origin <URL>] [-ref <baseref>]
```

`-ref` is resolved against the origin before it is stored, and the commit it
resolves to is what lands in the record. So `-ref:HEAD` records the origin's
current commit rather than the word `HEAD`, which is how the baseref is set
after a push has been committed on the far side:

```bash
apm <packagename> -push -origin:<dir>
(cd <dir> && git commit -m "...")
apm <packagename> -reset -ref:HEAD -origin:<URL>
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
```bash
acr ctype:command.xyz -t | apm -annotate
...
```

### Package definition
<a href="#package-definition"></a>

A package is defined by the `dev.package` record. The record specifies package name, git commit
corresponding to the base version of the package. The ref DOES NOT refer to the git history
of the current repo, it refers to the history of the ORIGIN's repo.

That commit is fetched by bringing the origin's heads into `refs/apm/<package>/`
and resolving the ref there. Fetching the commit id on its own does not work:
a refspec is matched by name against the refs the origin advertises, and a
commit id is not one of them.

The `origin` and `baseref` fields describe where this repo stands with respect
to the package, so they are meaningless in any other repo. When `-push` sends
the package's own `dev.package` record to the origin, it rewrites those two
fields to `.` and `HEAD` -- what a publisher says about a package it defines. The table `dev.pkgkey` specifies
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
A given ssim record can end up "belonging" to more than one package.

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

apm uses [sandboxes](/txt/ssimdb/dev/README.md) to hold intermediate state.
The new package is always fetched into the `apm-theirs` sandbox, and the common ancestor is instantiated
in `apm-base` sandbox.

After running the `-diff` command, the sandbox `apm-base` contains the published
version of the package.
After running `-update`, `apm-theirs` contains the new version and `apm-base` contains the common
ancestor version. This is useful when resolving conflicts.
The `-install` command is essentially the same as `-update` with current commit as the common ancestor.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pkgdata -- Load package definitions from here
<a href="#-pkgdata"></a>

#### -package -- Regx of package
<a href="#-package"></a>

#### -ns -- Operate on specified namespace only
<a href="#-ns"></a>

#### -install -- Install new package (specify -origin)
<a href="#-install"></a>

#### -update -- Update package (-origin)
<a href="#-update"></a>

APM will perform a 3-way merge between the base (attribute dev.package.baseref),
the current directory, and the most recent version of the package.
This results in a git diff which must be committed.

#### -list -- List installed packages
<a href="#-list"></a>

#### -diff -- Diff package with respect to installed version
<a href="#-diff"></a>

Apm shows the difference between package's base version (as defined by attribute dev.package.baseref)
and its current state in the current directory.

#### -push -- Evaluate package diff and push it to origin
<a href="#-push"></a>

Apm pushes any files/records corresponding to selected packages into the origin
directory, disregarding any base commit.

#### -check -- Consistency check
<a href="#-check"></a>

#### -remove -- Remove specified package
<a href="#-remove"></a>

The specified package and all dependent packages are removed.
This results in a git diff which then must be committed.

#### -origin -- Upstream URL of new package
<a href="#-origin"></a>

This option must be specified with `-install`. But it can also be used
to override package's `origin` attribute when invoked with `-diff`, `-push`, or `-update`.

#### -ref -- (with -create) Gitref or branch to fetch
<a href="#-ref"></a>

#### -dry_run -- Do not execute transaction
<a href="#-dry_run"></a>

#### -showrec -- Show records belonging to package
<a href="#-showrec"></a>

#### -showfile -- List package files (gitfile records)
<a href="#-showfile"></a>

#### -generate -- Generate package records into apm/gen/<package>.ssim
<a href="#-generate"></a>

#### -R -- reverse the diff direction
<a href="#-r"></a>

When used with `-diff`, this reverses the patch direction.

#### -l -- Use local package definition on the remote side
<a href="#-l"></a>

If the remote side (i.e. the origin repo) doesn't have the definition
of the package in question, this option will use the local `pkgkey`/`pkgdep` records
when evaluating package contents on the remote side.

#### -reset -- Reset package baseref/origin to those provided by the command line
<a href="#-reset"></a>

This option updates the local `package` record with values provided in
`-ref` and `-origin`. The ref is resolved against the origin first, so the
record ends up holding a commit id.

#### -checkclean -- Ensure that changes are applied to a clean directory
<a href="#-checkclean"></a>

Normally, `apm` will refuse to update a non-clean repo. Use this option to override
that behavior.

#### -t -- Select parent packages for operation
<a href="#-t"></a>

With `-install`, this option is forced to true.
With all other operations, the selection of packages is extended to include all parents
before proceeding.

#### -stat -- (with -diff) show stats
<a href="#-stat"></a>

#### -annotate -- Read file and annotate each input tuple with package(s) it belongs to
<a href="#-annotate"></a>

This option is used for package definitions. With it, you can check packages any given ssim record
belongs to. The argument is the file to read, `-` for stdin.

```bash
$ acr gitfile:README.md | apm -annotate -
dev.gitfile  gitfile:README.md  pkgkey:openacr/dev.%:%  pkgkey:openacr/dev.gitfile:README.md
```

Apm will print each input line, appending all the pkgkeys which capture the record.
To check files, simply use the `gitfile` table.

#### -data_in -- Dataset from which package records are loaded
<a href="#-data_in"></a>

#### -e -- Open selected records in editor
<a href="#-e"></a>

#### -binpath -- (internal use)
<a href="#-binpath"></a>
