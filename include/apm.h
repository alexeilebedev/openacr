// Copyright (C) 2023-2024,2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
    // cpp/apm/main.cpp
    //

    // Initialize zd_sel_package list based on the command line regex
    // For -update, -install -- select parent packages as well
    // For -remove -- select dependent packages
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

    // Bring PACKAGE's origin into this repo and resolve REF to a commit id there.
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
    // The origin's heads come across as a set instead, into a ref namespace of this
    // package's own, and REF is resolved locally afterwards.  Any commit an origin
    // branch reaches is then in this repo's object store and rev-parse finds it, so
    // a commit id, a branch name and HEAD all resolve through one path.  The
    // namespace is consulted before the repo, because REF names something in the
    // origin and a local branch of the same spelling is a different commit.
    tempstr FetchPackageOrigin(algo::strptr pkgname, algo::strptr origin, algo::strptr ref);

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

    // Return the mode bits of FILENAME, which are zero when it cannot be stat'ed.
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
    // void Main(); // main:apm

    // -------------------------------------------------------------------
    // cpp/apm/push.cpp
    //

    // Push any differences in selected packages between current directory
    // and BASE_DIR to BASE_DIR
    void PushDiff(algo::strptr base_dir);

    // Push any local differences between ORIGIN and current directory
    // to ORIGIN directory.
    // The directory must be initially clean.
    // -origin must be specified
    void Main_Push();

    // -------------------------------------------------------------------
    // cpp/apm/rec.cpp
    //

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
    // The two callers want opposite things of a record both a package and one of
    // its extenders capture.  An exclusion key is the package's own statement that
    // the record is not its, so it removes whatever it matches.  The subtraction a
    // relation derives is a statement about the extender instead, and the extender
    // reaches records it never meant to claim: a downstream package asks for one of
    // its own tables, the reference closure follows those rows into a table the base
    // owns, and the base's rows would leave with them.  Naming a record outright is
    // how a package says the record is its regardless, so that claim survives, and a
    // blanket like `dev.%:%` does not.
    //
    // The walk is by hand rather than by cursor because it deletes the rows it
    // visits, and a cursor over a list may not outlive the removal of its own node.
    void DropSelectedPkgrec(apm::FPackage &package, bool keep_literal = false);

    // Select records belonging to package PACKAGE by adding them to zd_selrec.
    // These are all the records that the package references via zd_pkgrec.
    void SelectPkgRecs(apm::FPackage &package);

    // -------------------------------------------------------------------
    // cpp/apm/remove.cpp
    //

    // Remove selected packages
    // - Remove package record
    // - Remove pkgfile and pkgkey records
    // - Remove git files
    // TODO: this must be implemented as an UPDATE
    // with target being an empty set.
    void Main_Remove();

    // -------------------------------------------------------------------
    // cpp/apm/reset.cpp
    //

    // Set the selected package's origin and baseref from the command line.
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

    // Save package records into apm/gen/<package>.ssim
    void Main_Generate();

    // List packages in topological order
    void Main_List();

    // -------------------------------------------------------------------
    // cpp/apm/update.cpp
    //

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

    // Rewrite dev.package files matching PACKAGE in FILENAME so that ORIGIN,BASEREF match those
    // specified in the function arguments
    void RewritePackageRecs(algo::strptr origin, algo::strptr baseref, algo::strptr pkgname, algo::strptr filename);

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
    // This function handles installation as well (the case where package.baseref = empty string)
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
}
