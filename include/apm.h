// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Header: include/apm.h
//

#include "include/gen/apm_gen.h"
#include "include/gen/apm_gen.inl.h"

namespace apm { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/apm/annotate.cpp
    //
    void Main_Annotate();

    // -------------------------------------------------------------------
    // cpp/apm/check.cpp
    //
    void Main_Check();

    // Report every record of each selected open-source package whose license
    // forbids it there, and return how many were found.
    // An open-source package is published outside the company, so a namespace or a
    // file under a proprietary license must never be part of it.  A target the
    // package builds must not link a system library under a proprietary license
    // either, nor one under a copyleft license such as the GPL: say acr_compl links
    // GNU readline, then every acr_compl binary built from the package is a GPL program,
    // whatever license its own sources carry.  Each finding names the package, the
    // record and the license, so the fix is a key that excludes the record, a
    // license change in the record's namespace, or a target that stops linking the
    // library.
    int CheckLicense();

    // Report every record of each selected package that references a record the
    // package does not carry, and return how many were found.
    // A package installed into an empty repository holds exactly its own records and
    // those of the packages it builds on, so a reference to anything else is one
    // that repository cannot resolve: acr -check fails there, and amc stops on the
    // first such reference it needs.  Each finding names the record and its target,
    // so the fix is to bring the target into the package or to move the record out.
    int CheckDangling();

    // Report every source file that defines user functions for records a selected
    // package carries and for records it does not, and return how many were found.
    // Take a comptest driver that defines one test of openacr's and one of an
    // extending package's.  openacr cannot carry the file, since the other test has
    // no row there and amc would
    // generate no prototype for its function.  Nor can openacr drop it, since its own
    // test row would then name a function nothing defines, and the origin fails to
    // link.  apm publishes a file whole, so the only fix is to split it, one owner per
    // file.  Only a package that builds the file's target is asked, since a package
    // that carries a target's data and not its namespace never compiles the file.
    // Each finding names the file, one record on each side, and how many there are of
    // each.
    int CheckMixedFile();

    // Report every file on which the projection of this tree at LOCALREF and the
    // origin ORIGIN at BASEREF disagree, for PACKAGE's destination PKGUPSTREAM, and
    // return how many there are.  A sync is a pair of commits, one per repository,
    // and the origin's commit is the projection of this tree's: that is what lets a
    // push take the whole origin as its base.  A file only one side holds means the
    // pair does not correspond, and the next push would treat the difference as a
    // change nobody made.  File contents are not compared, since each tree
    // regenerates its own generated files; ssimfiles are records, and data/ is left
    // out.  A path that a mapping of the package fills is a file of the projection,
    // and the mappings are this tree's current ones, which a mapping added since
    // LOCALREF makes disagree with the origin until the next sync.
    int CountSyncDrift(apm::FPackage &package, algo::strptr pkgupstream, algo::strptr origin, algo::strptr baseref, algo::strptr localref);

    // Report every file on which a selected package's recorded sync pair, its
    // localref and baseref, disagree, and return how many there are.  A row with
    // either half empty records no pair and is skipped.
    int CheckSyncPair();

    // -------------------------------------------------------------------
    // cpp/apm/diff.cpp
    //

    // Show local modifications made to package contents
    // with respect to baseref
    // With -origin, all packages are evaluated with respected to origin URL, and a single diff is shown
    // Without -origin, diff must be requested for one package only.
    void Main_Diff();

    // -------------------------------------------------------------------
    // cpp/apm/install.cpp
    //

    // Install package specified with options -package, -origin, -ref
    // Install is implemented as an update, i.e. 3-way merge with base = current directory
    void Main_Install();

    // -------------------------------------------------------------------
    // cpp/apm/keyword.cpp
    //

    // Report every selected package that carries a word it must not contain, in a
    // record, a file name or a line of a published file, and return how many
    // findings there are.
    //
    // Each package names the words that identify it, in dev.pkgkeyword, and the
    // rows belong to that package.  So openacr carries none of the words of a
    // package that extends it, and carries no list of them either: the list ships
    // only with the extender.  A package names the words it must never carry as
    // forbid:Y rows, and those hold for what it carries too.
    //
    // What is checked is the package's evaluation, records and files alike, since
    // that is exactly what a push carries, so a downstream table named in a
    // published document is reported in the commit that writes it.  A file the
    // package maps to another path is checked under that path's name too.
    //
    // Only the first ten findings of a package are printed, because a package that
    // has gone wrong tends to go wrong in bulk, and -v prints them all.  The summary reports how many there
    // are alongside how many it showed: a fix written from the printout alone would
    // address ten of them and leave the rest to surface next run.
    int CheckKeyword();

    // Report every link of a selected package's published markdown that names a
    // tracked file the package does not carry, and return how many there are.  A
    // markdown file the package maps to another path is published too.  The
    // carrier set is the package and every package it carries, the files a push
    // sends together, so a link from a contained package into its parent's docs
    // resolves.  Every package is checked, whether or not it has a destination yet,
    // so a link that would dangle downstream fails the commit that writes it.
    int CheckLink();

    // -------------------------------------------------------------------
    // cpp/apm/main.cpp
    //

    // Initialize zd_sel_package list based on the command line regex
    // For -update, -install -- select parent packages as well
    // For parents, dependencies marked as 'soft' are not followed.
    // These dependencies are used to establish proper package order for the purposes
    // of determining which file/record belongs to which package (i.e. everything depends
    // on openacr package) but not for installation. We don't automatically update base
    // openacr package when a child package is updated.
    void Main_SelectPackage();

    // Check out package contents into a sandbox
    // Return the wait status of the checkout, zero on success
    // This is the pristine version of the package (as specified with the gitref)
    // If BASEREF is an empty string, then the entire current directory, with whatever
    // local changes, is copied to the sandbox instead
    // A checkout that fails leaves the sandbox directory absent or holding the wrong
    // commit, and every reader of that directory afterwards reads a version of the
    // package nobody asked for. The failure is reported here, naming the sandbox and
    // the ref, and the status is returned for the caller to act on.
    int CreatePackageSandbox(algo::strptr sandbox_name, algo::strptr baseref);

    // Return the commit REV names, or the empty string when it names nothing here.
    // Rev-parse is quiet and verifying, so an unknown name is an empty answer
    // rather than a message on stderr the caller has no use for.
    tempstr RevParseMaybe(algo::strptr rev);

    // Return the destination of PACKAGE that this run acts on, or NULL when the
    // package has none, which is a package that lives in this tree.  A package syncs
    // with each of its dev.pkgupstream rows separately, and -dest names the row by
    // its dest.  Without -dest the package's only row is the one; a package with
    // several rows refuses, since picking one would merge or publish against an
    // origin nobody named.
    apm::FPkgupstream *GetPkgupstream(apm::FPackage &package);

    // Return the namespace under refs/apm/ into which PACKAGE's origin is fetched:
    // <package>/<dest> for a package with a destination, so that two origins of one
    // package never overwrite each other's refs, and the package name otherwise.
    tempstr GetRefns(apm::FPackage &package);

    // Bring ORIGIN into this repo under refs/apm/REFNAME and resolve REF to a commit
    // id there.
    // Return the start of a git command line that talks to an origin.  When
    // APM_TOKEN holds a destination's token, git gets a credential helper that
    // answers with it, so every command that reaches a token destination -- the
    // clone, the push, and the fetch of a reset after it -- authenticates the same
    // way.  The token reaches git through the environment, so it never appears on a
    // command line, in a URL, in a clone's config or in a log.  Without a token the
    // command is plain git, which uses whatever the user's own setup provides.
    tempstr GitCmd();

    // Return the commit, or the empty string when the origin cannot be reached or
    // REF names nothing in it.
    //
    // A baseref has to survive being written down, so it is a commit id rather than
    // a branch name.  But `git fetch <origin> <commit>` fails: both the local
    // transport and an ordinary server match a refspec by name against the refs the
    // origin advertises, and a commit id is not one of them.  Fetching a stored
    // baseref directly therefore dies with "couldn't find remote ref" on the one
    // value the field is supposed to hold.
    //
    // The origin's heads come across as a set instead, into the ref namespace
    // REFNAME, and REF is resolved locally afterwards.  Any commit an origin
    // branch reaches is then in this repo's object store and rev-parse finds it, so
    // a commit id, a branch name and HEAD all resolve through one path.  The
    // namespace is consulted before the repo, because REF names something in the
    // origin and a local branch of the same spelling is a different commit.
    tempstr FetchPackageOrigin(algo::strptr refname, algo::strptr origin, algo::strptr ref);

    // Execute any commands accumulated in _DB.SCRIPT
    // if -dry_run, print it to the screen
    // Script is reset after the run
    void Main_Transaction();

    // Return the apm binary that evaluates a package inside directory DIR.
    // Evaluating a package means loading that directory's dmmeta tables, and a
    // directory under sync holds tables of a different vintage than ours.  Openacr
    // carries dmmeta.Cdflt.jsdflt, a field this tree does not have, so our binary
    // stops on the first line of its cdflt.ssim and reads nothing at all.  A tree's
    // own apm is compiled against that tree's schema, which makes it the one binary
    // guaranteed to read it, so prefer it wherever DIR has one.  Ours answers for
    // the two cases that leaves: a directory carrying package files but no openacr
    // tooling, which is how apm serves a plain submodule, and a run under -l, where
    // the package definition being evaluated is ours and only our binary parses it.
    // Command line field BINPATH names the subdirectory the binaries sit in.
    tempstr GetApmPath(algo::strptr dir);

    // Return the absolute path of this tree's apm, in the subdirectory that command
    // line field BINPATH names.
    tempstr GetOurApmPath();

    // Return the mode bits of FILENAME, which are zero when it does not exist.  A
    // symbolic link reports its own mode, because git tracks the link: a link whose
    // target is not built yet still exists, and one whose target is a binary is
    // still a link.
    int GetFileMode(algo::strptr filename);

    // Retrun regx of selected packages
    tempstr SelPackageRegx();

    // Topologicaly sort packages by dependency into zd_topo_package list
    void SortPackages();

    // Open selected package definitions for editing
    void Main_Edit();

    // Definte fake packages based on 'ns' regx
    void DefPackages();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:apm

    // -------------------------------------------------------------------
    // cpp/apm/pkgfile.cpp
    //

    // Add to table pkgfile the mapping that each package's readmefile implies.  A
    // package whose front page is R ships R as its projection's txt/README.md, so its
    // readmefile stands for the row <package>/txt/README.md naming R, and the rest of
    // apm reads the two forms the same way.  A readmefile of txt/README.md, the
    // default, names the file at its own path and implies nothing.  A row the definition already holds
    // for that path stays, and a row naming another file contradicts the readmefile:
    // the run reports it as apm.readmefile and fails.
    void DerivePkgfile();

    // Fill table pkgdest with the paths that the mappings of the carriers fill, the
    // packages SetCarrier marked, each with the mapping that wins it, and return the
    // number of conflicts found.
    //
    // Take a push of an SDK package that builds on openacr, so the push carries
    // openacr as well.  Say both packages map a file to txt/README.md: the SDK its
    // front page, openacr a page of its own.  The projection has one file at that
    // path, so one of the two mappings has to lose.  The push is about the SDK, and
    // openacr comes along because the SDK needs it, so the SDK's mapping is the one
    // the push means.  So a mapping of a package the action is for, NAMED or else
    // the command line's -package, wins over a mapping of a package that is merely
    // carried, and the named packages are resolved first.  Two mappings that name
    // the same file agree.  Two mappings of one path that name different files, of
    // packages of the same rank, cannot be ordered, and each such pair is reported
    // as apm.pkgfile_conflict.
    int ResolvePkgfile(apm::FPackage *named);

    // Make each path of table pkgdest a file of the projection, so that a push writes
    // the content of its mapping's source there.  The path replaces whatever file of a
    // carried package the projection held at it: an SDK's front page replaces
    // openacr's txt/README.md.  The source still goes out at its own path when a key
    // of the package carries it.  A source that is not in this tree stops the push
    // with apm.pkgfile_nosrc, since the push would otherwise delete the path in the
    // origin.
    void ShipPkgdest();

    // Make the origin's copy of each mapped file side SIDE of the source's merge
    // entry, where SIDE is 1 for the base and 2 for the incoming version, and DIR is
    // that version's checkout.  The source here is the entry's own side, whether or
    // not a key of the package carries it.  The merge then compares the origin's
    // copy with the source, and this tree's own file at the mapped path stays out of
    // it: openacr's txt/README.md is never merged with an SDK's front page.
    //
    // The origin holds the mapped file twice when the package also carries the
    // source at its own path.  An SDK holds its front page as txt/README.md and at
    // the path of its source, and a contributor may edit either copy.  The base holds
    // the two copies equal, since one push wrote both.  So the incoming side takes
    // the copy at the mapped path when that copy changed since the base, and
    // otherwise the copy at the source's own path, which carries whatever change was
    // made there.  When both copies changed, differently, the copy at the mapped path
    // is merged and the other is reported as apm.pkgfile_diverge.
    void MapOriginCopy(int side, algo::strptr dir);

    // Report every mapping of a selected package that a push of it cannot carry out,
    // and return how many were found.  The mappings of a package and the packages it
    // carries must not collide (apm.pkgfile_conflict).  A mapping must name a file of
    // this tree (apm.pkgfile_nosrc).  And the package must not carry the mapped path
    // whole from its own keys, since the projection would then hold two different
    // files at one path (apm.pkgfile_ambiguous).
    int CheckPkgfile();

    // -------------------------------------------------------------------
    // cpp/apm/publish.cpp
    //

    // Publish the selected package to its destination, the dev.pkgupstream row -dest
    // names: produce it in a fresh clone of the destination's origin, regenerate,
    // verify, show the maintainer what goes out, and push it after a yes.
    //
    // Each stage prints a heading and stops the run when it fails, so the clone under
    // temp/apm-publish is left as the stage found it for the maintainer to look at.
    // -dry_run runs every stage up to and including the CI, and stops before the
    // question.  The stages, in order:
    //
    // - check: apm -check over this tree, which refuses a word the package must not
    // carry, a dangling record, or proprietary content in an open-source package;
    // - clone: the origin, fresh, which must sit at the destination's baseref, since a
    // push replaces what the origin holds and anything newer there would be lost;
    // - produce: apm -push of this tree's projection into the clone;
    // - regenerate: amc, update-hdr and, for a package with a front page of its own,
    // the front page's README.md copy in the clone, with this tree's binaries,
    // since a generated file holds the output over the database that wrote it;
    // - build: ai in the clone, which builds the clone's own binaries;
    // - docs: the clone's abt_md, which regenerates and checks every document;
    // - secrets: the clone's secret badlines over every file of the clone;
    // - show: the size of the tree and the delta against the origin's head;
    // - commit: a local commit in the clone, which sends nothing;
    // - ci: the destination's own CI on that commit, which says the package works;
    // - push, after a yes, with the destination's credd token;
    // - reset: this tree records the pushed commit as the destination's baseref.
    void Main_Publish();

    // Build each selected package from nothing: produce it in an empty repository of
    // its own under temp/apm-build/<package>, regenerate it and build it there, with
    // the same stages as -publish.  The CI job sdk runs this for every package that
    // leaves this tree, so a change that breaks a distribution's build fails here.
    // A package that fails sets the exit code, and the others still build.  A
    // selection that matches no package fails, so a renamed package cannot pass the
    // job by building nothing.  With -dry_run each package's line is the plan, and
    // nothing runs.
    void Main_Build();

    // -------------------------------------------------------------------
    // cpp/apm/push.cpp
    //

    // Make the origin BASE_DIR equal the projection of the selected packages.  The
    // origin is the projection of the last sync, so the base of the push is the
    // whole origin: every file and record it holds that the new projection does not
    // is one the downstream tree deleted, and it is deleted there too.
    void PushDiff(algo::strptr base_dir);

    // Push any local differences between ORIGIN and current directory
    // to ORIGIN directory.
    // The directory must be initially clean.
    // -origin must be specified
    void Main_Push();

    // -------------------------------------------------------------------
    // cpp/apm/rec.cpp
    //

    // Make PACKAGE and the packages whose content it carries the carriers that
    // CarriedQ asks about, and every other package not one.
    // With PACKAGE NULL there is no carrier.
    void SetCarrier(apm::FPackage *package);

    // Make the selected packages the carriers that CarriedQ asks about: the set a
    // push or a comparison with the origin sends together.
    void SetCarrierSelected();

    // Return true if a carrier set by SetCarrier holds REC.
    bool CarriedQ(apm::FRec &rec);

    // Return the text a package carries in place of the file PATH when it carries
    // the file as a stub.  A stub keeps the page's title and its anchor, so a link to
    // the page still lands on a page, and says in one sentence that the rest stays
    // with the tree the package came from.  A file that is not markdown is carried
    // empty.
    tempstr GetStubText(algo::strptr path);

    // Return the text a package publishes for the file PATH: the stub text when
    // STUB, else the file itself.
    tempstr GetPublishText(algo::strptr path, bool stub);

    // Return true when PACKAGE carries REC as a stub.
    bool StubQ(apm::FRec &rec, apm::FPackage &package);

    // Load all records (FRec) from dataset _db.cmdline.data_in)
    // For each record (FRec), compute p_ssimfile, pkey, tuple
    // Populate global zd_rec index
    // Populate zd_ssimfile_rec for each ssimfile (records grouped by ssimfile)
    // Populate c_child and c_left_child arrays for each record (these are records referring
    // to choosen records)
    // For each record, evaluate ssimreq rules. If there is a match, find corresponding
    // record and add it as a "match" to this key.
    //
    // For each match between FPkgkey and FRec, Create an FPkgrec record,
    // and group FPkgrec by FRec (zd_rec_pkgrec) and by FPackage (zd_rec)
    // This structure allows full analysis of package composition and checking
    void LoadRecs();

    // Whether PKGKEY names REC outright, rather than matching it.
    // A pkgkey and a record are spelled the same way, `<ssimfile>:<pkey>`, so the
    // test is that the two strings agree and that the key holds no pattern.
    // Reaching a record through the reference closure of a literal key does not
    // count: `dmmeta.ns:abt_md` names a namespace and nothing else, whatever its
    // closure goes on to visit.
    bool NamesRecQ(apm::FPkgkey &pkgkey, apm::FRec &rec);

    // Remove from PACKAGE every pkgrec whose record is currently in zd_selrec.
    // With KEEP_LITERAL, a record the package names outright is kept.
    //
    // A record a package names outright stays with it against both of the package's
    // statements about what is not its own: an exclusion, which is a blanket such as
    // `dev.package:%` beside the literal `dev.package:openacr`, and the subtraction a
    // relation derives, where an extender's reference closure reaches a table the
    // base owns.  Naming a record outright is how a package says the record is its
    // regardless, and a blanket like `dev.%:%` does not.  A record whose requirement
    // the package lacks goes whatever named it, so DropUnmet passes KEEP_LITERAL false.
    //
    // The walk is by hand rather than by cursor because it deletes the rows it
    // visits, and a cursor over a list may not outlive the removal of its own node.
    void DropSelectedPkgrec(apm::FPackage &package, bool keep_literal = false);

    // Select records belonging to package PACKAGE by adding them to zd_selrec.
    // These are all the records that the package references via zd_pkgrec.
    void SelectPkgRecs(apm::FPackage &package);

    // -------------------------------------------------------------------
    // cpp/apm/reset.cpp
    //

    // Set the selected package's origin and baseref from the command line, and
    // record this tree's HEAD as localref when the origin's commit is the
    // projection of HEAD: -reset with -ref closes the sync loop after a push.
    // When the two disagree on a file -- this tree gained one after the
    // publication, or the destination dropped one that this tree still has -- no
    // pair exists, and localref is left empty, with each such file reported as
    // apm.syncdrift and the row as apm.unpaired.
    // The three values form the dev.pkgupstream row of the destination -dest names,
    // which the reset creates when the package has no such row; the destination
    // defaults to the package's only one, or to "origin" for a package with none.
    // A ref given with -ref is resolved against the origin before it is stored, so
    // what lands in the record is a commit id.  Storing the name instead would let
    // the origin move the branch afterwards, and the record would then describe a
    // version this tree has never merged while still reading as the version it has.
    void Main_Reset();

    // -------------------------------------------------------------------
    // cpp/apm/show.cpp
    //

    // Topologically sort selected records and save them to file RECFILE
    // Return success code
    // Every apm action that needs these records reads them back from RECFILE rather
    // than from the selection they were written from, so a write that fails leaves
    // the file absent and every reader after it sees an empty record set. The
    // failure is reported here, at the one place that performs the write, naming the
    // file and the decoded errno; what a failed write means for the run is the
    // caller's to decide.
    bool SaveSelrecToFile(algo::strptr recfile);

    // Save local package definitions to file
    void SavePackageDefs(algo::strptr filename);

    // Return true if DIR is a repository no package has reached yet: it has no data
    // directory, as in a fresh checkout that a first push populates.  An empty DIR
    // names the current directory, which is never empty.
    bool EmptyOriginQ(algo::strptr dir);

    // Collect package records from directory DIR into RECFILE
    // Return success code
    // We run our executable in the remote directory to get predictable results
    // The caller reads the records back from RECFILE, and the child's stdout
    // redirect truncates RECFILE before the child runs. A child that fails
    // therefore leaves an empty file, which reads exactly like a directory whose
    // copy of the package holds no record at all -- and a merge given that empty
    // side concludes the package's records were all deleted. The wait status is the
    // only thing that tells the two apart, so it is reported here, at the one place
    // that runs the child; what an unread side means for the run is the caller's to
    // decide.
    // The child is started inside DIR and waited for after the current directory is
    // restored, so nothing that reports the failure runs while DIR is pushed.
    bool CollectPkgrecFromDir(algo::strptr package, algo::strptr recfile, algo::strptr dir);

    // Collect package records (dev.gitfile and other keys) into file RECFILE
    // Return success code;
    bool CollectPkgrecToFile(apm::FPackage &package, algo::strptr recfile);

    // Select all package records, save them into a temporary file and print them.
    void Main_Showrec();

    // Show selected package's files
    // Throw exception on error
    void Main_Showfile();

    // Print the records of every selected package to stdout, in sorted order, with a
    // sha1 note above each hand-written file the package carries.
    //
    // The records used to be written into apm/gen/<package>.ssim and kept in git,
    // where they were regenerated on every normalize run and conflicted with every
    // branch that touched a tracked file.  Nothing read them: they are a view of the
    // package, and a view is computed when it is asked for.
    void Main_Generate();

    // List packages in topological order
    void Main_List();

    // -------------------------------------------------------------------
    // cpp/apm/src.cpp
    //

    // Give every hand-written C++ file in the database a reference to each record
    // whose generated symbol its code names or whose user function it defines.
    // Take cpp/amc/ctype.cpp, which compares a field's reftype against
    // dmmeta_Reftype_reftype_Val.  The constant exists because dmmeta.reftype has a
    // row for it, so a package that carries the file and not the row ships code that does
    // not compile -- and no record says so, because the file's dev.gitfile row
    // references nothing.  amc knows which record each symbol comes from and writes
    // that to gendb.cppsym, so reading the file is enough to draw the edge.  A user
    // function is the same edge in the other direction: a file defining
    // atf_comp::comptest_apm_SrcScan needs atfdb.comptest:apm.SrcScan, or amc
    // generates no prototype for it and the definition fails to compile.  So the
    // file requires that record, and a package that lacks the record cannot carry
    // the file.  The record requires the file in turn: generated code calls the
    // function, so a package that carries atfdb.comptest:apm.SrcScan and not the
    // driver that defines it does not link, and the row leaves.  That holds only in
    // a package that builds the target compiling the driver, since only that
    // target's generated code calls the function.  A push makes the
    // origin equal to the projection, so no origin keeps a definition the projection
    // lacks.  The edge from the file is also an ordinary reference: the ref closure
    // brings the row with the file, and apm -check reports it as dangling for a
    // package that leaves it behind.
    void ScanSources();

    // -------------------------------------------------------------------
    // cpp/apm/update.cpp
    //

    // Record FILE as side SIDE of MERGEFILE, where SIDE is 0 for this tree, 1 for
    // the base and 2 for the incoming version, together with the file's mode, which
    // is zero when FILE does not exist.
    void SetMergeSide(apm::FMergefile &mergefile, int side, algo::strptr file);

    // Populate table MERGEFILE with up to 3 lists of files:
    // ours, base and theirs (0,1,2)
    // We record the path to each as OURS_FILE, BASE_FILE, or THEIRS_FILE
    // the modes are OURS_MODE, BASE_MODE, and THEIRS_MODE
    // If the file doesn't exist, the corresponding mode is 0 but the filename will be set.
    // BASE_DIR and THEIRS_DIR specify locations where package contents are evaluated. Both can be empty,
    // but if THEIRS_DIR is specified, then BASE_DIR must also be specified
    // The list of package file is evaluated in each directory independently.
    // But if the directory doesn't have apm, the local package definition is used.
    // Return success code
    // Each list comes from a child whose output this function reads, so a child that
    // fails contributes no file rather than an error, and a file the child never
    // named is a file the merge leaves at the version it already had. The failure is
    // reported per directory and the caller decides what an incomplete table means.
    // The table is keyed by the path in this tree.  A file of the origin at a path
    // that a mapping fills (table pkgdest, which the caller resolves) is the
    // origin's copy of the mapping's source, so it is entered under the source's
    // path (MapOriginCopy).
    bool CreateMergeFiles(algo::strptr regx_package, algo::strptr base_dir, algo::strptr theirs_dir);

    // Scan mergefile table and perform per-file 3-way non-history-aware merge
    // each file may be existent or non-existent; each file has a mode
    // BASE        OURS       THEIRS      RESULT
    // exist       exist      exist      3-way merge
    // exist       exist      non-exist  delete
    // exist       non-exist  exist      no action
    // exist       non-exit   non-exist  delete
    // non-exist   exist      exist      3-way merge
    // non-exist   exist      non-exist  no action
    // non-exist   non-exist  exist      create file
    // non-exist   non-exit   non-exist  <not possible>
    // there are 3 possible actions: copy over, delete, or merge
    void MergeFiles(apm::FPackage &package);

    // Update selected packages to the latest version,
    // or to `-ref` if specified
    // check that the directory is clean, abort if not.
    // step 1. fetch original version of the package into a sandbox (base)
    // step 2. fetch new version of the package into a sandbox (b)
    // step 3. perform a 3-way merge between current files (a), new package files (b), with respect to base
    // add merged files to index
    // merging records:
    // extract records for base into file pkgrec.base
    // extract records for new version of the package into file pkgrec.b
    // extract records for current files into file pkgrec.a
    // perform a 3-way merge between a, b and base using acr_dm into pkgrec.merge
    // apply changes that can be applied to ssimfiles,
    // insert conflicts into ssimfiles in appropriate places
    // user continues with `git add ...`, `git commit` or `git reset --hard` to abort
    // This function handles installation as well (the case where the baseref is an empty string)
    //
    // Each step below produces one of the inputs the next steps read: a sandbox
    // directory, one of the three sides of the record merge, the merged records, or
    // the table of files to merge. Every one of those inputs is a file or a
    // directory, so a step that fails leaves its input empty rather than missing,
    // and an empty input is a legitimate value everywhere it is read -- an empty
    // "theirs" says the incoming version deleted every record of the package, and
    // the plan built from it deletes the package's records with nothing to put back.
    // So a step that fails stops the update: the plan is not composed at all, and
    // the nonzero exit keeps the transaction from running. The steps, and what apm
    // does when each of them fails, are pinned by comptest apm.UpdateFate.
    void Main_Update();

    // -------------------------------------------------------------------
    // include/apm.inl.h
    //

    // Return the origin of PACKAGE's destination: the URL it was installed from, or "." for a
    // package that has no dev.pkgupstream row and so lives in this tree.
    inline algo::strptr GetOrigin(apm::FPackage &package);

    // Return the baseref of PACKAGE's destination: the commit of its origin this tree last merged
    // or published, or HEAD for a package that lives in this tree.
    inline algo::strptr GetBaseref(apm::FPackage &package);
}

#include "include/apm.inl.h"
