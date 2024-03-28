// Copyright (C) 2023-2024 AlgoRND
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
    // This is the pristine version of the package (as specified with the gitref)
    // If BASEREF is an empty string, then the entire current directory, with whatever
    // local changes, is copied to the sandbox instead
    int CreatePackageSandbox(algo::strptr sandbox_name, algo::strptr baseref);
    tempstr FetchPackageOrigin(algo::strptr origin, algo::strptr ref);

    // Execute any commands accumulated in _DB.SCRIPT
    // if -dry_run, print it to the screen
    // Script is reset after the run
    void Main_Transaction();
    int GetFileMode(algo::strptr filename);

    // Retrun regx of selected packages
    tempstr SelPackageRegx();

    // Topologicaly sort packages by dependency into zd_topo_package list
    void SortPackages();

    // Open selected package definitions for editing
    void Main_Edit();
    void Main();

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

    // Select records belonging to package PACKAGE by adding them to zd_selrec
    // These are all the records that the package references via zd_pkgrec,
    // minus any records claimed by packages that depend on PACKAGE
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
    void Main_Reset();

    // -------------------------------------------------------------------
    // cpp/apm/show.cpp
    //

    // Rewrite ssim records in RECFILE in topological order
    bool RewriteSsimfile(algo::strptr recfile);

    // Topologically sort selected records and save them to file RECFILE
    // Return success code
    bool SaveSelrecToFile(algo::strptr recfile);

    // Collect package records from directory DIR into RECFILE
    // If the remote side doesn't have APM, use local package definition to
    // select files & records belonging to the package.
    // On error, throw exception
    void CollectPkgrecFromDir(algo::strptr package, algo::strptr recfile, algo::strptr dir);

    // Collect package records (dev.gitfile and other keys) into file RECFILE
    // Return success code;
    bool CollectPkgrecToFile(apm::FPackage &package, algo::strptr recfile);

    // Select all package records, save them into a temporary file and print them.
    void Main_Showrec();

    // Show selected package's files
    // Throw exception on error
    void Main_Showfile();

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
    void CreateMergeFiles(algo::strptr regx_package, algo::strptr base_dir, algo::strptr theirs_dir);

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
    void Main_Update();
}
