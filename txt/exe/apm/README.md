## apm - Algo Package Manager
<a href="#apm"></a>

`apm` installs, updates and publishes OpenACR packages.  A package is a
set of files plus a set of ssim records, and installing one merges both into the
working tree at the source level.  The result is an ordinary diff, which you
commit with git to finish the install.  The two repositories need no common git
history.

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
    -publish                     Verify, show and push the package to its destination, asking first
    -check                       Consistency check
    -origin      string  ""      Upstream URL of new package
    -dest        string  ""      Destination: the dev.pkgupstream row of the package; default its only one
    -ref         string  ""      (with -diff, -reset) Commit, branch or HEAD in the origin
    -dry_run                     Do not execute transaction
    -showrec                     Show records belonging to package
    -showfile                    List package files (gitfile records)
    -generate                    Print the records of every selected package to stdout
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
    -trace       string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### A package is a set of keys and their closure
<a href="#a-package-is-a-set-of-keys-and-their-closure"></a>

A package is named by a `dev.package` row and defined by its `dev.pkgkey` rows.
Each key is an acr query, such as `openacr/dmmeta.ns:amc` or
`openacr/dev.gitfile:README.md`, and the package holds the records the keys
select together with their transitive closure.  `down:Y` adds every record that
references a selected one, `up:Y` every record a selected one references, and
`exclude:Y` takes the selection out.  `ref:Y` follows references the way a
compiler does: a referenced record comes with everything it owns, and an owner
comes alone.

Files are records.  A selection that captures a `dev.gitfile` row puts that file
in the package, so a namespace's closure brings its sources, its generated code
and its readme.

Hand-written code is reached the same way.  `apm` scans every hand-written C++
file for the symbols `amc` generates, and `gendb.cppsym` names the record each
symbol comes from.  A file that uses a symbol references that record, so a
selection that captures the record captures the file.  This is what lets a
package extend another package's tool at the source level.  A package that
extends openacr adds citest `label_gen` to `atf_ci` by claiming its row of
`atfdb.citest`, and the row brings the source file that defines
the citest's function.  openacr's `atf_ci` builds without that file, and the
extending tree's builds with it, with no second tool in either tree.  A file that defines a function for a record also
requires that record, so a package holding the file and not the record drops
the file.

`apm` publishes a file whole, so each file belongs to one side.  `-check`
reports `apm.mixedfile` for a file whose functions belong to records on both
sides of a package, and the fix is to split it.

Use `-showrec`, `-showfile` and `-annotate` to see what a definition selects.

#### Relations between packages
<a href="#relations-between-packages"></a>

`dev.pkgdep` rows relate a package to a parent, and `pkgdeptype` names the
relation:

|`pkgdeptype`|Meaning|
|---|---|
|`contain`|The parent distributes this package, so the parent's content covers it.|
|`extend`|This package builds on the parent without shipping in it, so the parent's content excludes whatever this package captures.|
|`require`|This package needs the parent built first, and neither one's content touches the other's.|

`extend` keeps a downstream tree out of an upstream distribution: a package
that extends openacr takes whatever it claims out of openacr, with no exclusion
keys in openacr's definition.  A record the parent names outright, with no pattern in
the key, such as `dev.netproto:https`, stays with the parent.  A `soft:Y`
dependency orders the packages and is not followed by `-t`.

#### Checking a definition
<a href="#checking-a-definition"></a>

A package installed into an empty repository has to build there, and `-check`
reports what would stop it.  `apm.dangling` is a record that references one the
package and its parents do not carry.  `apm.proprietary` is a record of an
`opensource:Y` package whose license is proprietary.  `apm.copyleft` is a
target of such a package that links a system library under a copyleft license
(`dev.license.copyleft`, the GPL), since every binary built from it would be a
GPL program; each `dev.syslib` row names its library's license.  `-push`
refuses a package with either finding.  A generated file whose namespace the package lacks is dropped, with
every record that references it, and `-v` prints each one as `apm.unmet`.

`apm.keyword` is a record, a file name or a line of a published file that
carries a word the package must not contain.  A package's `dev.pkgkeyword` rows
name the words that identify it, and those are kept out of every package it
builds on; a row with `forbid:Y` names a word kept out of the package itself and
of everything it carries.  A line regenerated downstream is skipped (a copyright
notice, an `update-hdr` block, a usage block, an inline command's output), and a
line that must keep a word carries `ignore:pkgkeyword`.

A table that a package excludes whole, with a key such as `dev.pkgkey:%`, is
repo-local: each repo keeps its own rows, so a push neither sends this tree's
rows nor deletes the origin's.  The package definitions are such tables.

`apm.deadlink` is a reference in a published markdown file to something that
exists here and that the push leaves behind, so the reference is broken
downstream.  The reference is a link to a tracked file, or an inline span such
as `dev.license:GPL` that names a row.  A target that a carried package holds resolves.  Only a package
with a `dev.pkgupstream` row gets the check, because only such a package has a
downstream.

```bash
apm openacr -check                          # every finding, keywords included
```

#### Where a package comes from
<a href="#where-a-package-comes-from"></a>

A package that syncs with another repo has a `dev.pkgupstream` row for each
destination, keyed `<package>/<dest>`.  Its `origin` is the repo, and its
`baseref` is the origin's commit this tree last merged or published.  Its
`localref` is the commit of this tree whose projection the origin holds at
`baseref`, and it is empty until a push and a `-reset` pair the two.  Its `cred`
names the credd credential that holds the token for pushing there; the origin
URL carries no token.  The rows are private to this tree and no package carries
them.  A package with no row lives here.

```
dev.pkgupstream  pkgupstream:openacr/github  origin:git@github.com:alexeilebedev/openacr.git  cred:""  baseref:fce279e503...  localref:""
```

Name the destination with `-dest`.  Without it, an action uses the package's
only destination, and refuses a package that has several.

#### Installing and updating
<a href="#installing-and-updating"></a>

`-update` merges the origin's current version into this tree.  It checks out
the origin's HEAD in sandbox `apm-theirs` and the version at `baseref` in
`apm-base`.  Files get a three-way merge from `git merge-file`, and records get
an attribute-level three-way merge from [acr_dm](/txt/exe/acr_dm/README.md), so
changes to neighboring rows or to different attributes of one row never
conflict.  `baseref` then moves to the fetched commit, and `apm` runs `amc` and
`update-gitfile`.  A conflict in a file leaves git markers.  A conflict in
records is left in `temp/apm.acrtxn.ssim`, and `apm` prints the `acr` command
that applies it once you have edited it.

`-install` is an update whose base is the current tree.  It creates the
`dev.pkgupstream` row from `-origin` and brings in the package and every package
it depends on at the origin.

Each of the two writes its plan to `temp/apm.script.sh` and runs it, and
`-dry_run` prints the plan instead.  Commit the result to finish.  `wt apm-base
-shell` opens a sandbox while you resolve a conflict.  See
[wt](/txt/exe/wt/README.md).

#### Publishing and the sync pair
<a href="#publishing-and-the-sync-pair"></a>

An origin holds only projections, so `-push` makes the checkout named by
`-origin` equal to this tree's projection of the package: it copies the files
and records, and deletes there whatever the projection no longer holds.  The
origin has to be at `baseref`, or the push refuses with `apm.origin_moved`
until `-update` has merged what the origin gained.  Commit in the origin and
publish the commit, then run `-reset -ref:<commit>`, which points `baseref` at
that commit and `localref` at this tree's HEAD.  The two commits are the sync
pair, and `-check` reports `apm.syncdrift` for every file they disagree on.

`-publish` runs that whole cycle for one destination and proves the result
before anything leaves.  It runs `-check`, clones the origin fresh under
`temp/apm-publish/<package>/<dest>`, pushes the projection there, regenerates it
and builds the clone with its own tools.  It runs the clone's `abt_md` and its
secret scan (`src_lim -badline:secret_%`), and shows the delta against the
origin's head: the size of the tree, files added, changed and deleted per top
directory, and every new file by name.  It then commits in the clone and runs
the destination's own CI there, `bin/normalize && bin/atf_ci -cijob:comp`, with
its log beside the clone.  Any failed stage stops the run, and `-dry_run` stops
after the CI.  Otherwise it asks before pushing, takes the token from the
maintainer's credd under the row's `cred`, and finishes with `-reset`.

```bash
apm openacr -publish -dest:github -dry_run  # verify and show what would go out
apm openacr -publish -dest:github           # the same, then commit, push, reset
```

#### Comparing with the base version
<a href="#comparing-with-the-base-version"></a>

`-diff` checks out the base version in `apm-base`, makes it this tree's
projection, and shows `git diff` there.  `-origin` and `-ref` override the
recorded origin and `baseref` for one comparison.

#### Repos that do not run apm
<a href="#repos-that-do-not-run-apm"></a>

To learn what a package holds on the far side, `apm` runs that checkout's own
`apm`, so that repo's definition decides.  `-l` evaluates both sides with this
tree's definition, which works against any repo, including one with no
ssimfiles: `apm <package> -diff -l -origin:<url> -ref:<baseref>`.

### Limitations
<a href="#limitations"></a>

- A file that you changed locally and that the origin later renamed is deleted
  on update, with no warning.  Your change is still in git history.
- A file that you renamed locally stops receiving upstream changes, because the
  update sees the old name as deleted.
- `-update` and `-install` refuse a working tree with modified files.  Commit
  first, or pass `-checkclean:N`.
- A dry run still fetches the origin, checks out both sandboxes, and writes the
  record files under `temp/`.
- The source scan misses a symbol used unqualified inside its own namespace.
- `-diff` without `-origin` compares one package only.

### Examples
<a href="#examples"></a>

```bash
apm -list                                   # list the packages, parents first
apm apm -showfile                           # list the files of package apm
apm apm -showrec -t                         # records of apm and the packages it depends on
apm -ns:acr_dm -showfile                    # what a package made of namespace acr_dm would hold
acr dmmeta.ns:amc | apm -annotate -         # say which pkgkeys select a record
apm openacr -check -l                       # check package openacr's definition
apm <package> -diff -stat                   # local changes against the base version
apm <package> -install -origin:<url>        # install a package, then commit
apm <package> -update -dry_run              # show what an update would do
```

`-annotate` appends every pkgkey that selects each input record:

```bash
$ acr gitfile:README.md | apm -annotate -
dev.gitfile  gitfile:README.md  pkgkey:openacr/dev.%:%  pkgkey:openacr/dev.gitfile:README.md
```

A round trip with an origin checkout pulls, pushes, then records the origin's
new commit:

```bash
apm <package> -update -l                           # merge what upstream added
apm <package> -push -l -origin:<dir>               # make the origin this tree's projection
(cd <dir> && git commit -a -m "..." && git push)   # the origin records it
apm <package> -reset -ref:<commit> -origin:<url>   # pair baseref and localref
git commit -m "..."                                # commit the merge and the pair together
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The dataset `apm` reads the schema and the package tables from: `dev.package`,
`dev.pkgkey`, `dev.pkgdep`, `dev.pkgupstream` and the `dmmeta` tables that
describe each ssimfile.
The records a package selects come from `-data_in`.

#### -pkgdata -- Load package definitions from here
<a href="#-pkgdata"></a>

Replace the `dev.package`, `dev.pkgdep` and `dev.pkgkey` rows from `-in` with
the ones in this file.  `apm` passes it to the `apm` it runs on the far side
under `-l`, and you rarely need it yourself.

#### -package -- Regx of package
<a href="#-package"></a>

The packages to act on, as a regx.  `-install` takes one literal name.  With no
action, a regx that matches nothing prints the package list and exits non-zero.
With an action it does nothing and exits 0.

#### -ns -- Operate on specified namespace only
<a href="#-ns"></a>

Define one package per namespace matching this regx, holding the namespace and
everything that references it, and select those packages.  It implies `-l`.
Use it to see or move one namespace without writing a package definition.

#### -install -- Install new package (specify -origin)
<a href="#-install"></a>

Install the named package from `-origin` into this tree, with every package it
depends on at the origin.  It refuses a package already in `dev.package`.
Commit the result to finish.

#### -update -- Update package (-origin)
<a href="#-update"></a>

Merge the origin's current HEAD into this tree with a three-way merge against
`baseref`, then move `baseref` to that commit and clear `localref`.
Conflicts are left for you to resolve, and the result is a diff to commit.

#### -list -- List installed packages
<a href="#-list"></a>

Print every package with its parents and comment, parents first.

#### -diff -- Diff package with respect to installed version
<a href="#-diff"></a>

Show how this tree's copy of the package differs from its base version, as a
`git diff` run in the `apm-base` sandbox.  `-R` reverses the direction and
`-stat` prints a diffstat.

#### -push -- Evaluate package diff and push it to origin
<a href="#-push"></a>

Make the checkout that `-origin` names equal to this tree's projection of the
selected packages and the packages they require, records and files alike.  The
origin must be a local directory with no modified files, at the `baseref` of
each package the command names; a required package's sync pair belongs to its
own origin and is not compared.  New files are marked with `git add -N` and
deletions are staged, so commit with `git commit -a`.

#### -publish -- Verify, show and push the package to its destination, asking first
<a href="#-publish"></a>

#### -check -- Consistency check
<a href="#-check"></a>

Check the selected packages' definitions.  A key that selects nothing is
`apm.emptykey`, and a key naming an ssimfile under `data/` is `apm.badkey`.  A
namespace that both a package and one of its `extend` children claim is
`apm.doubleclaim`.  The first two reports carry the `acr` line that would fix
them.  `apm.dangling`, `apm.proprietary`, `apm.copyleft`, `apm.mixedfile` and
`apm.syncdrift` are described above.

#### -origin -- Upstream URL of new package
<a href="#-origin"></a>

The repo to install from, which `-install` requires.  With `-diff` and `-reset`
it overrides the recorded origin, and with `-push` it names the local checkout
to copy into.

#### -dest -- Destination: the dev.pkgupstream row of the package; default its only one
<a href="#-dest"></a>

Pick one of the package's `dev.pkgupstream` rows by its `dest`.  `-update`,
`-diff` and `-push` refuse a destination the package lacks, and `-reset` and
`-install` create it, named `origin` when `-dest` is absent.

#### -ref -- (with -diff, -reset) Commit, branch or HEAD in the origin
<a href="#-ref"></a>

With `-diff` it replaces `baseref` for one comparison.  With `-reset` it becomes
the new `baseref`, resolved to a commit id first.

#### -dry_run -- Do not execute transaction
<a href="#-dry_run"></a>

Print the plan to stdout and run none of it.  An update still fetches, fills
both sandboxes and writes its record files under `temp/`, because the plan is
computed from them.

#### -showrec -- Show records belonging to package
<a href="#-showrec"></a>

Print every record the selected packages hold, a file appearing as its
`dev.gitfile` row.  Add `-t` to include the packages they depend on.

#### -showfile -- List package files (gitfile records)
<a href="#-showfile"></a>

Print the `dev.gitfile` rows of the selected packages.  Ssimfiles under `data/`
are left out, since their records merge separately.

#### -generate -- Print the records of every selected package to stdout
<a href="#-generate"></a>

Print each selected package's records in sorted order, with a `# SHA1 =` line
above every hand-written file.  Compare two runs to see what changed in a
package.

#### -R -- reverse the diff direction
<a href="#-r"></a>

With `-diff`, show the patch in the reverse direction, from this tree to the
base version.

#### -l -- Use local package definition on the remote side
<a href="#-l"></a>

Evaluate the package on the far side with this tree's `dev.pkgkey` and
`dev.pkgdep` rows, using this tree's `apm`.  Use it when the origin has no
definition of the package, or when this tree is where the definition is
maintained.

#### -reset -- Reset package baseref/origin to those provided by the command line
<a href="#-reset"></a>

Set one package's `dev.pkgupstream` row from `-origin` and `-ref`, creating the
row if there is none.  The ref is resolved against the origin to a commit id,
which becomes `baseref`, and this tree's HEAD becomes `localref`.  Use it after
a push that the origin has committed.

#### -checkclean -- Ensure that changes are applied to a clean directory
<a href="#-checkclean"></a>

`-install` and `-update` refuse a working tree with modified files unless you
pass `-checkclean:N`.  A dry run skips the check.

#### -t -- Select parent packages for operation
<a href="#-t"></a>

Add the selected packages' parents to the selection, following every
dependency that is not `soft`.  `-install` turns it on.

#### -stat -- (with -diff) show stats
<a href="#-stat"></a>

With `-diff`, print a diffstat in place of the full diff.

#### -annotate -- Read file and annotate each input tuple with package(s) it belongs to
<a href="#-annotate"></a>

Read ssim tuples from this file, `-` for stdin, and print each one with a
`pkgkey:` attribute for every key that selects it.  Use it while writing a
package definition to see where a record lands.

#### -data_in -- Dataset from which package records are loaded
<a href="#-data_in"></a>

The records that pkgkeys are evaluated against, as a data directory or one ssim
file.  It defaults to `data`.

#### -e -- Open selected records in editor
<a href="#-e"></a>

Open the selected packages' `dev.package` rows, with everything that references
them, in `acr -e`.

#### -binpath -- (internal use)
<a href="#-binpath"></a>

The directory, relative to a checkout, where `apm` looks for that checkout's
own `apm` binary.
