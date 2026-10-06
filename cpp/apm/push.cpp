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
// Source: cpp/apm/push.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// Add ARG to command line COMMAND, which starts with PREFIX
// If COMMAND gets too large, flush it to OUT and re-initialize with PREFIX
static void Xargs(cstring &command, algo::strptr prefix, algo::strptr arg, algo::cstring &out) {
    if (command=="") {
        command<<prefix;
    }
    command << " " << strptr_ToBash(arg);
    if (ch_N(command) > 1000) {
        out << command << eol;
        command = "";
    }
}

// -----------------------------------------------------------------------------

// True when a selected package excludes every row of SSIMFILE, with a key such
// as dev.pkgkey:%.  Such a table is repo-local: each repo holds its own rows,
// so the push neither sends ours nor deletes the origin's.
static bool LocalTableQ(algo::strptr ssimfile) {
    bool ret = false;
    ind_beg(apm::_db_zd_sel_package_curs, package, apm::_db) {
        ind_beg(apm::package_zd_pkgkey_curs, pkgkey, package) if (pkgkey.exclude) {
            algo::strptr rec = algo::Pathcomp(pkgkey.pkgkey, "/LR");
            ret = ret || (algo::Pathcomp(rec, ":LR") == "%" && algo::Pathcomp(rec, ":LL") == ssimfile);
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Write the records the origin BASE_DIR holds to the base record file.  The
// origin is the projection of the last sync, so its records are package content
// and the push deletes those the projection no longer holds.  The rows of a
// repo-local table (LocalTableQ) are the origin's own, and they are left out of
// the base, so the push leaves them as they are.  An origin no package has
// reached holds none.  Return success code.
static bool CollectOriginRecs(algo::strptr base_dir) {
    tempstr full_recfile = DirFileJoin(algo::GetCurDir(), apm::_db.base_recfile);
    bool ok = true;
    if (apm::EmptyOriginQ(base_dir)) {
        StringToFile("", full_recfile);
    } else {
        command::acr_proc acr;
        acr.cmd.in = DirFileJoin(base_dir, "data");
        acr.cmd.query = "%";
        acr.cmd.loose = true;
        acr.cmd.report = false;
        acr.fstdout << ">" << full_recfile;
        ok = acr_Exec(acr) == 0;
        if (!ok) {
            prerr("apm.pkgrec_read"
                  <<Keyval("dir",base_dir)
                  <<Keyval("comment","the origin's records could not be read"));
        } else {
            cstring base;
            ind_beg(algo::FileLine_curs, line, full_recfile) {
                if (!LocalTableQ(algo::Pathcomp(line, " LL"))) {
                    base << line << eol;
                }
            }ind_end;
            ok = SafeStringToFile(base, full_recfile);
        }
    }
    return ok;
}

// -----------------------------------------------------------------------------

// Add every file git tracks in the origin BASE_DIR to the merge table as the
// base side.  The origin is the projection of the last sync, so a file there that
// the new projection does not name is one the downstream tree deleted, and the
// push deletes it too.  Files under data/ are ssimfiles, which the records carry.
static void AddOriginFiles(algo::strptr base_dir) {
    if (!apm::EmptyOriginQ(base_dir)) {
        tempstr out(SysEval(tempstr() << "git -C " << strptr_ToBash(base_dir) << " ls-files", FailokQ(false), 1024*1024*64));
        ind_beg(algo::Line_curs, path, out) {
            if (ch_N(path) > 0 && !algo::StartsWithQ(path, "data/")) {
                apm::FMergefile &mergefile = apm::ind_mergefile_GetOrCreate(path);
                mergefile.base_file = DirFileJoin(base_dir, path);
                mergefile.base_mode = apm::GetFileMode(mergefile.base_file);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Make the origin BASE_DIR equal the projection of the selected packages.  The
// origin is the projection of the last sync, so the base of the push is the
// whole origin: every file and record it holds that the new projection does not
// is one the downstream tree deleted, and it is deleted there too.
void apm::PushDiff(algo::strptr base_dir) {
    // collect package records in local dir
    // collect package records in sandbox
    // diff package records
    zd_selrec_RemoveAll();
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        SelectPkgRecs(package);
    }ind_end;
    vrfy(SaveSelrecToFile(_db.ours_recfile), "failed to collect package records");
    // Create every ssimfile the package's records live in, so that the records
    // have somewhere to land even in an origin that has never held them.
    // The directories go first: a long touch line flushes while the paths are
    // still being collected, so it is held apart until the mkdir lines are out.
    cstring cmd_touch;
    cstring script_touch;
    ind_beg(_db_zd_selrec_curs,rec,_db) if (rec.p_ssimfile->ssimfile == "dmmeta.ssimfile") {
        tempstr ssimfile(Pathcomp(rec.rec,":LR"));
        tempstr path(DirFileJoin(base_dir,tempstr()<<"data/"<<Pathcomp(ssimfile,".LL")<<"/"<<Pathcomp(ssimfile,".LR")<<".ssim"));
        ind_mkdir_GetOrCreate(GetDirName(path));
        Xargs(cmd_touch, "touch", path, script_touch);
    }ind_end;
    cstring cmd_mkdirdata;
    ind_beg(_db_mkdir_curs,mkdir,_db) {
        Xargs(cmd_mkdirdata, "mkdir -p", mkdir.mkdir, _db.script);
    }ind_end;
    _db.script << cmd_mkdirdata << eol;
    _db.script << script_touch << cmd_touch << eol;
    mkdir_RemoveAll();
    zd_selrec_RemoveAll();

    tempstr regx_package=SelPackageRegx();
    vrfy(CollectOriginRecs(base_dir), "failed to collect the origin's records");

    // delete any base records first
    {
        command::acr_proc acr;
        acr.cmd.in = algo::DirFileJoin(base_dir,"data");
        acr.cmd.query = "%";
        acr.cmd.in = _db.base_recfile;
        acr.cmd.del=true;
        acr.fstdout << ">"<<_db.merged_recfile;
        _db.script << acr_ToCmdline(acr) << eol;
    }
    // add merged records
    {
        command::acr_dm_proc acr_dm;
        arg_Alloc(acr_dm.cmd)=_db.base_recfile;
        arg_Alloc(acr_dm.cmd)=_db.ours_recfile;
        acr_dm.cmd.rowid=true;
        acr_dm.fstdout << ">>"<<_db.merged_recfile;
        _db.script << acr_dm_ToCmdline(acr_dm) << eol;
    }
    // apply acr transaction in sandbox directory
    {
        command::acr_proc acr;
        acr.cmd.in = algo::DirFileJoin(base_dir,"data");
        acr.cmd.replace=true;
        acr.cmd.write=true;
        acr.cmd.print=false;
        acr.cmd.report=false;
        acr.fstdin << "<"<<_db.merged_recfile;
        _db.script << acr_ToCmdline(acr) << eol;
    }

    mergefile_RemoveAll();
    vrfy(CreateMergeFiles(regx_package,"",""),// ours
         "failed to collect the package file list");
    AddOriginFiles(base_dir);
    if (algo_lib::_db.cmdline.verbose>1) {
        _db.script << "set -x"<<eol;
    }
    // collect directories to be created
    mkdir_RemoveAll();
    ind_beg(_db_mergefile_curs,mergefile,_db) if (mergefile.ours_mode != 0) {
        ind_mkdir_GetOrCreate(GetDirName(DirFileJoin(base_dir,mergefile.mergefile)));
    }ind_end;
    cstring cmd_mkdir;
    ind_beg(_db_mkdir_curs,mkdir,_db) {
        Xargs(cmd_mkdir, "mkdir -p", mkdir.mkdir, _db.script);
    }ind_end;
    _db.script << cmd_mkdir << eol;// flush the rest
    ind_beg(_db_mergefile_curs,mergefile,_db) {
        if (mergefile.ours_mode != 0) {
            // -P = preserve soft link
            // -p = preserve mode/permissions
            _db.script << "cp -P -p " << strptr_ToBash(mergefile.mergefile)
                       << " " << strptr_ToBash(DirFileJoin(base_dir,mergefile.mergefile))<< eol;
        }
    }ind_end;
    _db.script << "pushd "<<strptr_ToBash(base_dir)<<eol;
    cstring git_add;
    cstring git_rm;
    ind_beg(_db_mergefile_curs,mergefile,_db) {
        if (mergefile.ours_mode==0) {
            Xargs(git_rm, "git rm -f", mergefile.mergefile, _db.script);
        } else {
            Xargs(git_add, "git add -f -N", mergefile.mergefile, _db.script);
        }
    }ind_end;
    _db.script <<git_add<<eol;// flush the rest
    _db.script <<git_rm<<eol;
    _db.script <<"popd"<<eol;
}

// -----------------------------------------------------------------------------

// Refuse a push onto an origin that has moved past the last sync.  The push
// replaces the projection of the last sync, and that projection is the origin at
// the package's baseref, so an origin whose HEAD is another commit holds changes
// nobody has merged downstream, and the push would delete them.  apm -update
// merges them first.  A package with no dev.pkgupstream row lives in this tree,
// and it and an origin no package has reached have nothing to compare.  Only a
// package the command names is checked: a push selects the packages it builds on
// as well, and their sync pairs belong to their own origins.  Pushing a package
// that requires openacr carries openacr, whose baseref is a commit of the
// OpenACR origin and means nothing in the other package's origin.
static void CheckOriginAtBaseref(algo::strptr base_dir) {
    if (!apm::EmptyOriginQ(base_dir)) {
        ind_beg(apm::_db_zd_sel_package_curs, package, apm::_db) {
            apm::FPkgupstream *pkgupstream = Regx_Match(apm::_db.cmdline.package, package.package) ? apm::GetPkgupstream(package) : NULL;
            if (pkgupstream) {
                algo_lib::PushDir(base_dir);
                tempstr base(apm::RevParseMaybe(pkgupstream->baseref));
                tempstr head(apm::RevParseMaybe("HEAD"));
                algo_lib::PopDir();
                vrfy(base != "" && base == head, tempstr() << "apm.origin_moved"
                     << Keyval("package", package.package)
                     << Keyval("baseref", pkgupstream->baseref)
                     << Keyval("head", head)
                     << Keyval("comment", "the origin is not at the last sync; run apm -update first"));
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Push any local differences between ORIGIN and current directory
// to ORIGIN directory.
// The directory must be initially clean.
// -origin must be specified
void apm::Main_Push() {
    vrfy(zd_sel_package_N()>0, "no package selected");
    tempstr base_dir(_db.cmdline.origin);
    vrfy(DirectoryQ(base_dir),"-push only works with a directory name. please make sure -origin is a valid directory");
    vrfy(base_dir!="", "-push requires -origin");

    // check that origin directory is clean
    vrfy(Trimmed(algo::SysEval(tempstr()<<"cd "<<base_dir<<" && git ls-files -m",FailokQ(false),1024*100))==""
         , "origin directory not clean");

    vrfy(CheckLicense()==0, "apm.license  comment:'an open-source package holds proprietary content or links a copyleft library'");
    CheckOriginAtBaseref(base_dir);
    PushDiff(base_dir);

    // The origin's own tools may not be built yet, so the steps below run this
    // tree's acr.  acr reads its schema from the data it is given, so the
    // origin's schema governs either way.
    tempstr bindir(DirFileJoin(algo::GetCurDir(),_db.cmdline.binpath));
    _db.script << "pushd "<<strptr_ToBash(base_dir)<<eol;
    _db.script << strptr_ToBash(DirFileJoin(bindir,"acr"))<<" ssimfile -cmd 'mkdir -p data/$ns; touch data/$ns/$name.ssim' | bash"<<eol;
    // an ssimfile whose table the projection no longer has leaves the origin
    _db.script << "comm -23 <(git ls-files 'data/*.ssim' | sort) <("<<strptr_ToBash(DirFileJoin(bindir,"acr"))
               <<" ssimfile -cmd 'echo data/$ns/$name.ssim' | bash | sort) | xargs -r git rm -q -f"<<eol;
    _db.script << "git add -N data"<<eol;// add new ssimfiles
    _db.script << "PATH="<<strptr_ToBash(bindir)<<":\"$PATH\" "<<strptr_ToBash(DirFileJoin(bindir,"update-gitfile"))<<eol;// rebuild gitfile table
    _db.script << "popd" << eol;
    _db.script << "echo "<<algo::strptr_ToBash(tempstr()<<"changes pushed to "<<base_dir) << eol;
}
