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

// Push any differences in selected packages between current directory
// and BASE_DIR to BASE_DIR
void apm::PushDiff(algo::strptr base_dir) {
    // collect package records in local dir
    // collect package records in sandbox
    // diff package records
    zd_selrec_RemoveAll();
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        SelectPkgRecs(package);
    }ind_end;
    vrfy(SaveSelrecToFile(_db.ours_recfile), "failed to collect package records");
    zd_selrec_RemoveAll();

    tempstr regx_package=SelPackageRegx();
    CollectPkgrecFromDir(regx_package,_db.base_recfile,base_dir);// in sandbox
    // rewrite remote 'package' record to match local ORIGIN and BASEREF fields
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        cstring origin(_db.cmdline.origin == "" ? algo::strptr(package.origin) : algo::strptr(_db.cmdline.origin));
        cstring baseref(_db.cmdline.ref == "" ? algo::strptr(package.baseref) : algo::strptr(_db.cmdline.ref));
        apm::RewritePackageRecs(origin,baseref,package.package,_db.ours_recfile);
        apm::RewritePackageRecs(origin,baseref,package.package,_db.base_recfile);
    }ind_end;

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
    CreateMergeFiles(regx_package,base_dir,""); // ours,base
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
            // -d = preserve soft link
            // -p = preserve mode/permissions
            _db.script << "cp -d -p " << strptr_ToBash(mergefile.mergefile)
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

    PushDiff(base_dir);

    _db.script << "pushd "<<base_dir<<eol;
    _db.script << "acr ssimfile -cmd 'mkdir -p data/$ns; touch data/$ns/$name.ssim' | bash"<<eol;
    _db.script << "git add -N data"<<eol;// add new ssimfiles
    _db.script << "update-gitfile"<<eol;// rebuild gitfile table
    _db.script << "popd" << eol;
    _db.script << "echo "<<algo::strptr_ToBash(tempstr()<<"changes pushed to "<<base_dir) << eol;
}
