// Copyright (C) 2024 AlgoRND
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
// Source: cpp/apm/update.cpp
//

#include "include/algo.h"
#include "include/lib_git.h"
#include "include/apm.h"

// Populate table MERGEFILE with up to 3 lists of files:
// ours, base and theirs (0,1,2)
// We record the path to each as OURS_FILE, BASE_FILE, or THEIRS_FILE
// the modes are OURS_MODE, BASE_MODE, and THEIRS_MODE
// If the file doesn't exist, the corresponding mode is 0 but the filename will be set.
// BASE_DIR and THEIRS_DIR specify locations where package contents are evaluated. Both can be empty,
// but if THEIRS_DIR is specified, then BASE_DIR must also be specified
// The list of package file is evaluated in each directory independently.
// But if the directory doesn't have apm, the local package definition is used.
void apm::CreateMergeFiles(algo::strptr regx_package, algo::strptr base_dir, algo::strptr theirs_dir) {
    tempstr dirs[3];
    dirs[0]="";//ours
    dirs[1]=base_dir;
    dirs[2]=theirs_dir;
    int n = 1 + (base_dir!="") + (base_dir!="" && theirs_dir!="");
    for (int i=0; i<n; i++) {
        command::apm_proc apm;
        apm.path=DirFileJoin(algo::GetCurDir(),"bin/apm");
        if (_db.cmdline.l) { // use local package definitions
            apm.cmd.pkgdata = DirFileJoin(algo::GetCurDir(),_db.pkgdata_recfile);
        }
        algo_lib::FFildes read;
        bool push = dirs[i] != "";
        if (push) {
            algo_lib::PushDir(dirs[i]);
        }
        apm.cmd.package.expr=regx_package;
        apm.cmd.showfile=true;
        apm.cmd.t=true;// include dependencies
        apm_StartRead(apm,read);
        if (push) {
            algo_lib::PopDir();
        }
        ind_beg(algo::FileLine_curs,line,read.fd) {
            //verblog("mergefiles: "<<dirs[i]<<": "<<line);
            dev::Gitfile gitfile;
            if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
                apm::FMergefile &mergefile = ind_mergefile_GetOrCreate(gitfile.gitfile);
                int &mode=(i==0 ? mergefile.ours_mode : i==1 ? mergefile.base_mode : mergefile.theirs_mode);
                cstring &file = (i==0 ? mergefile.ours_file : i==1 ? mergefile.base_file : mergefile.theirs_file);
                file = DirFileJoin(dirs[i],gitfile.gitfile);
                mode=GetFileMode(file);
            }
        }ind_end;
    }
    ind_beg(_db_mergefile_curs,mergefile,_db) {
        verblog(mergefile);
    }ind_end;
}

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
void apm::MergeFiles(apm::FPackage &package) {
    (void)package;
    ind_beg(_db_mergefile_curs,mergefile,_db) {
        tempstr base_file=DirFileJoin(algo_lib::SandboxDir(_db.base_sandbox),mergefile.mergefile);
        tempstr theirs_file=DirFileJoin(algo_lib::SandboxDir(_db.theirs_sandbox),mergefile.mergefile);
        if (mergefile.base_mode == 0 && mergefile.ours_mode == 0 && mergefile.theirs_mode != 0) {
            // create parent directory if necessary
            tempstr dirname(GetDirName(mergefile.mergefile));
            if (dirname != "") {
                _db.script << "mkdir -p "<<strptr_ToBash(dirname) << eol;
            }
            // copy file
            // -p = preserve mode,ownership,timestamps
            // -d = don't dereference (preseve symbolic link)
            _db.script << "cp -p -d "<<strptr_ToBash(mergefile.theirs_file)<<" "<<strptr_ToBash(mergefile.mergefile) << eol;
            _db.script << "git add -f "<<strptr_ToBash(mergefile.mergefile) << eol;
        }
        if (mergefile.ours_mode != 0 && mergefile.theirs_mode != 0) {
            // merge
            _db.script << "git merge-file "
                       <<" -L "<<strptr_ToBash(mergefile.mergefile)
                       <<" -L base"
                       <<" -L package"
                       <<" "<<strptr_ToBash(mergefile.mergefile)
                       <<" "<<(mergefile.base_mode == 0 ? tempstr("/dev/null") : strptr_ToBash(base_file))
                       <<" "<<strptr_ToBash(theirs_file)<<eol;
        }
        if (mergefile.ours_mode != 0 && mergefile.base_mode != 0 && mergefile.theirs_mode == 0) {
            // delete
            _db.script << "git rm -f -q "<<strptr_ToBash(mergefile.mergefile)<<eol;
        }
    }ind_end;
}

// Rewrite dev.package files matching PACKAGE in FILENAME so that ORIGIN,BASEREF match those
// specified in the function arguments
void apm::RewritePackageRecs(algo::strptr origin, algo::strptr baseref, algo::strptr pkgname, algo::strptr filename) {
    tempstr out;
    ind_beg(algo::FileLine_curs,line,filename) {
        dev::Package package;
        if (Package_ReadStrptrMaybe(package,line) && package.package == pkgname) {
            package.origin = origin;
            package.baseref = baseref;
            out<<package<<eol;
        } else {
            out<<line<<eol;
        }
    }ind_end;
    StringToFile(out,filename);
}

// Update selected packages to the latest version,
// or to `-ref` if specified
// check that the directory is clean, abort if not.
// step 1. fetch original version of the package into a sandbox (base)
// step 2. fetch new version of the package into a sandbox (b)
// step 3. perform a 3-way merge between current files (a), new package files (b), with respect to base
//    add merged files to index
// merging records:
// extract records for base into file pkgrec.base
// extract records for new version of the package into file pkgrec.b
// extract records for current files into file pkgrec.a
// perform a 3-way merge between a, b and base using acr_dm into pkgrec.merge
// apply changes that can be applied to ssimfiles,
// insert conflicts into ssimfiles in appropriate places
// user continues with `git add ...`, `git commit` or `git reset --hard` to abort
// This function handles installation as well (the case where package.baseref = empty string)
void apm::Main_Update() {
    if (!_db.cmdline.dry_run && _db.cmdline.checkclean) {
        lib_git::CheckGitCleanX(".","Please make sure the working directory is clean or use -dry_run");
    }
    tempstr merged_recfile = tempstr() << "temp/apm.merged.ssim";
    tempstr acrtxn_recfile = tempstr() << "temp/apm.acrtxn.ssim";
    bool has_conflict=false;
    tempstr acrtxn;// final acr transaction
    int rc=0;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        // fetch updated version of package
        tempstr new_package_gitref = FetchPackageOrigin(package.origin,"HEAD");
        vrfy(new_package_gitref!="",tempstr()<<"failed to fetch "<<package.package);
        verblog("apm.update"
                <<Keyval("package",package.package)
                <<Keyval("new_baseref",new_package_gitref));
        if (new_package_gitref == "") {
            algo_lib::_db.exit_code=1;
            break;
        }
        // fetch base version of package
        (void)FetchPackageOrigin(package.origin,package.baseref);
        // create sandbox for original package version
        CreatePackageSandbox(_db.base_sandbox,package.baseref);
        // update package baseref to the version being fetched
        package.baseref=new_package_gitref;

        // create file with original package records
        apm::CollectPkgrecFromDir(package.package,_db.base_recfile,algo_lib::SandboxDir(_db.base_sandbox));

        // create file with current package records
        apm::CollectPkgrecToFile(package,_db.ours_recfile);

        // create sandbox with new package records
        CreatePackageSandbox(_db.theirs_sandbox,new_package_gitref);
        apm::CollectPkgrecFromDir(package.package,_db.theirs_recfile,algo_lib::SandboxDir(_db.theirs_sandbox));

        // update any dev.package records found in the records files
        // to match ORIGIN and BASEREF of the package being updated
        // because they will be inserted back into the acr database in this tree
        apm::RewritePackageRecs(package.origin,package.baseref,package.package,_db.base_recfile);
        apm::RewritePackageRecs(package.origin,package.baseref,package.package,_db.ours_recfile);
        apm::RewritePackageRecs(package.origin,package.baseref,package.package,_db.theirs_recfile);

        // merge records
        command::acr_dm_proc acr_dm;
        arg_Alloc(acr_dm.cmd)=_db.base_recfile;
        arg_Alloc(acr_dm.cmd)=_db.ours_recfile;
        arg_Alloc(acr_dm.cmd)=_db.theirs_recfile;
        acr_dm.fstdout << ">"<<merged_recfile;
        acr_dm_Exec(acr_dm);
        // delete original package records
        ind_beg(algo::FileLine_curs,line,_db.base_recfile) {
            if (Trimmed(line)!="") {
                acrtxn << "acr.delete "<<line << eol;
            }
        }ind_end;
        // insert updated package records
        ind_beg(algo::FileLine_curs,line,merged_recfile) {
            acrtxn << "acr.replace "<<line << eol;
            if (StartsWithQ(line,"<<<<<") || StartsWithQ(line,">>>>>") || StartsWithQ(line,"=====")) {
                has_conflict=true;
            }
        }ind_end;

        CreateMergeFiles(package.package,algo_lib::SandboxDir(_db.base_sandbox),algo_lib::SandboxDir(_db.theirs_sandbox));
        // log the entries
        ind_beg(_db_mergefile_curs,mergefile,_db) {
            _db.script << "# "<<mergefile<<eol;
        }ind_end;
        MergeFiles(package);
        _db.script << "echo "<<strptr_ToBash(tempstr()<<"package "<<package.package<<" updated to "<<new_package_gitref)<<eol;
    }ind_end;

    // save acr transaction records to acrtxn_recfile
    // if the file has conflicts, let user finish
    // otherwise, apply transaction (subject to -dry_run)
    if (rc==0) {
        if (_db.cmdline.dry_run) {
            prlog(acrtxn);
        }
        StringToFile(acrtxn,acrtxn_recfile);
        command::acr_proc acr;
        acr.cmd.replace=true;
        acr.cmd.write=true;
        acr.cmd.print=false;
        acr.cmd.report=false;
        acr.fstdin << "<"<<acrtxn_recfile;
        if (has_conflict) {
            _db.script << "echo "<<algo::strptr_ToBash("apm: NOTICE: Conflicts found when updating package.") << eol;
            _db.script << "echo "<<algo::strptr_ToBash(tempstr()<<"apm: Please edit file "<<acrtxn_recfile<<" and call") << eol;
            _db.script << "echo "<<algo::strptr_ToBash(tempstr()<<"    "<<acr_ToCmdline(acr)) << eol;
        } else {
            // run the insert command TWICE
            // because on the first run, the meta-data for new tuples is missing
            _db.script << acr_ToCmdline(acr) << eol;
            _db.script << acr_ToCmdline(acr) << eol;
        }
    }
    // re-generate code after updating package
    _db.script << "acr ssimfile -cmd 'mkdir -p data/$ns; touch data/$ns/$name.ssim' | bash"<<eol;
    _db.script << "amc -report:N" << eol;
    // create empty ssimfiles since they are required
    _db.script << "git add data"<<eol;// add new ssimfiles
    _db.script << "update-gitfile" << eol;
    if (rc!=0) {
        algo_lib::_db.exit_code=1;
    }
}
