// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
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
// Return success code
// Each list comes from a child whose output this function reads, so a child that
// fails contributes no file rather than an error, and a file the child never
// named is a file the merge leaves at the version it already had. The failure is
// reported per directory and the caller decides what an incomplete table means.
bool apm::CreateMergeFiles(algo::strptr regx_package, algo::strptr base_dir, algo::strptr theirs_dir) {
    bool ok=true;
    tempstr dirs[3];
    dirs[0]="";//ours
    dirs[1]=base_dir;
    dirs[2]=theirs_dir;
    int n = 1 + (base_dir!="") + (base_dir!="" && theirs_dir!="");
    for (int i=0; i<n; i++) {
        command::apm_proc apm;
        apm.path=GetApmPath(dirs[i]);
        if (_db.cmdline.l) { // use local package definitions
            apm.cmd.pkgdata = DirFileJoin(algo::GetCurDir(),_db.pkgdata_recfile);
        }
        bool push = dirs[i] != "";
        if (push) {
            algo_lib::PushDir(dirs[i]);
        }
        apm.cmd.package.expr=regx_package;
        apm.cmd.showfile=true;
        apm.cmd.t=true;// include dependencies
        apm.fstdout = "|";
        apm_Start(apm);
        if (push) {
            algo_lib::PopDir();
        }
        ind_beg(algo::FileLine_curs,line,apm.from_stdout) {
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
        apm_Wait(apm);
        if (apm.status!=0) {
            ok=false;
            prerr("apm.pkgfile_read"
                  <<Keyval("dir",dirs[i])
                  <<Keyval("status",algo::DescribeWaitStatus(apm.status))
                  <<Keyval("comment","package file list could not be read from the directory"));
        }
    }
    ind_beg(_db_mergefile_curs,mergefile,_db) {
        verblog(mergefile);
    }ind_end;
    return ok;
}

// Merge MERGEFILE when one of its sides is a symbolic link.  A link holds no
// lines to merge: its value is its target, and git merge-file would read the
// file the link points to.  So the side that differs from the base wins, and a
// link that both sides changed, to different values, is a conflict that keeps
// ours.  BASE_FILE and THEIRS_FILE are the copies in the two sandboxes.
static void MergeLink(apm::FMergefile &mergefile, algo::strptr base_file, algo::strptr theirs_file) {
    tempstr ours(S_ISLNK(mergefile.ours_mode) ? algo::ReadLink(mergefile.mergefile) : tempstr());
    tempstr base(S_ISLNK(mergefile.base_mode) ? algo::ReadLink(base_file) : tempstr());
    tempstr theirs(S_ISLNK(mergefile.theirs_mode) ? algo::ReadLink(theirs_file) : tempstr());
    bool same_ours = mergefile.ours_mode == mergefile.base_mode && ours == base;
    bool same_theirs = mergefile.theirs_mode == mergefile.base_mode && theirs == base;
    bool agree = mergefile.ours_mode == mergefile.theirs_mode && ours == theirs;
    if (same_ours && !same_theirs) {
        apm::_db.script << "rm -f "<<strptr_ToBash(mergefile.mergefile) << eol;
        apm::_db.script << "cp -p -P "<<strptr_ToBash(theirs_file)<<" "<<strptr_ToBash(mergefile.mergefile) << eol;
        apm::_db.script << "git add -f "<<strptr_ToBash(mergefile.mergefile) << eol;
    } else if (!same_ours && !same_theirs && !agree) {
        prerr("apm.linkconflict"
              <<Keyval("file",mergefile.mergefile)
              <<Keyval("ours",ours)
              <<Keyval("theirs",theirs)
              <<Keyval("comment","both sides changed this link; ours is kept"));
    }
}

// -----------------------------------------------------------------------------

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
        tempstr base_file=DirFileJoin(algo_lib::WtDir(_db.base_sandbox),mergefile.mergefile);
        tempstr theirs_file=DirFileJoin(algo_lib::WtDir(_db.theirs_sandbox),mergefile.mergefile);
        if (mergefile.base_mode == 0 && mergefile.ours_mode == 0 && mergefile.theirs_mode != 0) {
            // create parent directory if necessary
            tempstr dirname(GetDirName(mergefile.mergefile));
            if (dirname != "") {
                _db.script << "mkdir -p "<<strptr_ToBash(dirname) << eol;
            }
            // copy file
            // -p = preserve mode,ownership,timestamps
            // -P = don't dereference (preserve symbolic link)
            _db.script << "cp -p -P "<<strptr_ToBash(mergefile.theirs_file)<<" "<<strptr_ToBash(mergefile.mergefile) << eol;
            _db.script << "git add -f "<<strptr_ToBash(mergefile.mergefile) << eol;
        }
        bool link = S_ISLNK(mergefile.ours_mode) || S_ISLNK(mergefile.theirs_mode) || S_ISLNK(mergefile.base_mode);
        if (link && mergefile.ours_mode != 0 && mergefile.theirs_mode != 0) {
            MergeLink(mergefile, base_file, theirs_file);
        } else if (mergefile.ours_mode != 0 && mergefile.theirs_mode != 0) {
            // merge.  git merge-file exits with the number of conflicts it left
            // as markers, capped at 127, and with 255 on an error.  A conflict
            // is a result the user resolves after the update, so the plan goes
            // on past it and names the file, and only an error stops the script.
            // git merge-file prints nothing for a conflict, so without that line
            // an update ends silent with markers in the tree.
            _db.script << "git merge-file --no-diff3"
                       <<" -L "<<strptr_ToBash(mergefile.mergefile)
                       <<" -L base"
                       <<" -L package"
                       <<" "<<strptr_ToBash(mergefile.mergefile)
                       <<" "<<(mergefile.base_mode == 0 ? tempstr("/dev/null") : strptr_ToBash(base_file))
                       <<" "<<strptr_ToBash(theirs_file)
                       <<" || { rc=$?; echo \"apm.conflict  file:"<<strptr_ToBash(mergefile.mergefile)<<"  nconflict:$rc\" >&2; [ $rc -lt 128 ]; }"<<eol;
        }
        if (mergefile.ours_mode != 0 && mergefile.base_mode != 0 && mergefile.theirs_mode == 0) {
            // delete
            _db.script << "git rm -f -q "<<strptr_ToBash(mergefile.mergefile)<<eol;
        }
    }ind_end;
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
void apm::Main_Update() {
    if (!_db.cmdline.dry_run && _db.cmdline.checkclean) {
        lib_git::CheckGitCleanX(".","Please make sure the working directory is clean or use -dry_run");
    }
    tempstr merged_recfile = tempstr() << "temp/apm.merged.ssim";
    tempstr acrtxn_recfile = tempstr() << "temp/apm.acrtxn.ssim";
    bool has_conflict=false;
    tempstr acrtxn;// final acr transaction
    bool ok=true;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        // fetch updated version of package
        tempstr new_package_gitref = FetchPackageOrigin(GetRefns(package),GetOrigin(package),"HEAD");
        vrfy(new_package_gitref!="",tempstr()<<"failed to fetch "<<package.package);
        verblog("apm.update"
                <<Keyval("package",package.package)
                <<Keyval("new_baseref",new_package_gitref));
        // fetch base version of package
        // An empty baseref is what -install leaves behind, and it means the
        // current directory: there is no earlier version of the package here to
        // fetch, so the merge takes this tree as its base.  Any other baseref
        // names a commit in the origin and has to resolve to one.
        tempstr base_gitref(GetBaseref(package));
        if (base_gitref != "") {
            base_gitref = FetchPackageOrigin(GetRefns(package),GetOrigin(package),GetBaseref(package));
            vrfy(base_gitref!="",tempstr()<<"failed to resolve base version "<<GetBaseref(package)<<" of package "<<package.package);
        }
        // create sandbox for original package version
        ok = CreatePackageSandbox(_db.base_sandbox,base_gitref)==0;
        if (ok) {
            // create file with original package records
            ok = apm::CollectPkgrecFromDir(package.package,_db.base_recfile,algo_lib::WtDir(_db.base_sandbox));
        }
        if (ok) {
            // create file with current package records
            ok = apm::CollectPkgrecToFile(package,_db.ours_recfile);
        }
        if (ok) {
            // create sandbox with new package records
            ok = CreatePackageSandbox(_db.theirs_sandbox,new_package_gitref)==0;
        }
        if (ok) {
            ok = apm::CollectPkgrecFromDir(package.package,_db.theirs_recfile,algo_lib::WtDir(_db.theirs_sandbox));
        }
        if (ok) {
            // merge records
            command::acr_dm_proc acr_dm;
            arg_Alloc(acr_dm.cmd)=_db.base_recfile;
            arg_Alloc(acr_dm.cmd)=_db.ours_recfile;
            arg_Alloc(acr_dm.cmd)=_db.theirs_recfile;
            acr_dm.fstdout << ">"<<merged_recfile;
            int status = acr_dm_Exec(acr_dm);
            ok = status==0;
            if (!ok) {
                prerr("apm.recmerge"
                      <<Keyval("recfile",merged_recfile)
                      <<Keyval("status",algo::DescribeWaitStatus(status))
                      <<Keyval("comment","package records could not be merged"));
            }
        }
        if (ok) {
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
            // A package installed from an origin now sits at the version just
            // fetched.  A merge leaves no commit of this tree whose projection
            // the origin is, so localref is cleared until the next sync sets it.
            if (apm::FPkgupstream *row = GetPkgupstream(package)) {
                dev::Pkgupstream pkgupstream;
                pkgupstream_CopyOut(*row,pkgupstream);
                pkgupstream.baseref = new_package_gitref;
                pkgupstream.localref = "";
                acrtxn << "acr.replace "<<pkgupstream << eol;
            }

            ok = CreateMergeFiles(package.package,algo_lib::WtDir(_db.base_sandbox),algo_lib::WtDir(_db.theirs_sandbox));
        }
        if (ok) {
            // log the entries
            ind_beg(_db_mergefile_curs,mergefile,_db) {
                _db.script << "# "<<mergefile<<eol;
            }ind_end;
            MergeFiles(package);
            _db.script << "echo "<<strptr_ToBash(tempstr()<<"package "<<package.package<<" updated to "<<new_package_gitref)<<eol;
        }
        if (!ok) {
            break;
        }
    }ind_end;

    // save acr transaction records to acrtxn_recfile
    // if the file has conflicts, let user finish
    // otherwise, apply transaction (subject to -dry_run)
    if (ok) {
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
        // re-generate code after updating package
        _db.script << "acr ssimfile -cmd 'mkdir -p data/$ns; touch data/$ns/$name.ssim' | bash"<<eol;
        _db.script << "amc -report:N" << eol;
        // create empty ssimfiles since they are required
        _db.script << "git add data"<<eol;// add new ssimfiles
        _db.script << "update-gitfile" << eol;
    } else {
        algo_lib::_db.exit_code++;
    }
}
