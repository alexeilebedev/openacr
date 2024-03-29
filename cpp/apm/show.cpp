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
// Source: cpp/apm/show.cpp
//

#include "include/algo.h"
#include "include/apm.h"

// -----------------------------------------------------------------------------

// Rewrite ssim records in RECFILE in topological order
bool apm::RewriteSsimfile(algo::strptr recfile) {
    command::acr_proc acr;
    acr.cmd.in=recfile;
    acr.cmd.query="%";
    acr.cmd.write=true;
    acr.cmd.loose=true;
    acr.cmd.rowid=true;
    acr.cmd.print=false;
    acr.cmd.report=false;
    return acr_Exec(acr)==0;
}

// -----------------------------------------------------------------------------

// Topologically sort selected records and save them to file RECFILE
// Return success code
bool apm::SaveSelrecToFile(algo::strptr recfile) {
    tempstr recs;
    DeleteFile(recfile);
    ind_beg(_db_zd_selrec_curs,rec,_db) {
        recs << rec.tuple << Keyval("acr.rowid",rec.rowid) << eol;
    }ind_end;
    return SafeStringToFile(recs,recfile);
}

// -----------------------------------------------------------------------------

// Collect package records from directory DIR into RECFILE
// If the remote side doesn't have APM, use local package definition to
// select files & records belonging to the package.
// On error, throw exception
void apm::CollectPkgrecFromDir(algo::strptr package, algo::strptr recfile, algo::strptr dir) {
    bool remote_has_apm = FileQ(DirFileJoin(dir,"bin/apm"));
    tempstr full_recfile=DirFileJoin(algo::GetCurDir(),recfile);
    command::apm_proc apm;
    if (!remote_has_apm) {
        prlog("# apm doesn't seem to exist in the target package. using local package definition");
        apm.path = DirFileJoin(algo::GetCurDir(),"bin/apm");
        apm.cmd.in = DirFileJoin(algo::GetCurDir(),"temp/apm.inputs");
        command::acr_in_proc acr_in;
        acr_in.cmd.ns.expr="apm";
        acr_in.cmd.data=true;
        acr_in.fstdout << ">"<<apm.cmd.in;
        acr_in_ExecX(acr_in);
    }
    algo_lib::PushDir(dir);
    apm.cmd.package.expr=package;
    apm.cmd.showrec=true;
    apm.cmd.nosort=true;
    apm.cmd.t=_db.cmdline.t;
    // make filename absolute because we're now in a different directory
    apm.fstdout << ">"<<full_recfile;
    // #AL# todo: failure?
    apm_Exec(apm);
    algo_lib::PopDir();
}

// -----------------------------------------------------------------------------

// Collect package records (dev.gitfile and other keys) into file RECFILE
// Return success code;
bool apm::CollectPkgrecToFile(apm::FPackage &package, algo::strptr recfile) {
    zd_selrec_RemoveAll();
    SelectPkgRecs(package);
    bool ok=SaveSelrecToFile(recfile);
    zd_selrec_RemoveAll();
    return ok;
}

// -----------------------------------------------------------------------------

// Select all package records, save them into a temporary file and print them.
void apm::Main_Showrec() {
    _db.script <<" set -x"<<eol;
    zd_selrec_RemoveAll();
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        SelectPkgRecs(package);
    }ind_end;
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile, "apm.recs");
    vrfy(SaveSelrecToFile(tempfile.filename), "error creating records file");
    if (!_db.cmdline.nosort) {
        apm::RewriteSsimfile(tempfile.filename);
    }
    zd_selrec_RemoveAll();
    prlog(FileToString(tempfile.filename));
}

// -----------------------------------------------------------------------------

// Show selected package's files
// Throw exception on error
void apm::Main_Showfile() {
    _db.script <<" set -x"<<eol;
    zd_selrec_RemoveAll();
    // select package records
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        SelectPkgRecs(package);
    }ind_end;
    // ssimfiles are excluded from the list of package files
    // they are merged separately with acr_dm
    algo_lib::Regx regx_exclude;
    Regx_ReadAcr(regx_exclude,"dev.gitfile:data/%/%.ssim",true);
    // extract just gitfile records and print them
    apm::FSsimfile *gitfile=ind_ssimfile_Find("dev.gitfile");
    for (apm::FRec *rec=zd_selrec_First(); rec;) {
        apm::FRec *next=zd_selrec_Next(*rec);
        if (rec->p_ssimfile == gitfile && !Regx_Match(regx_exclude,rec->rec)) {
            prlog(rec->tuple<<Keyval("acr.rowid",rec->rowid));
        } else {
            zd_selrec_Remove(*rec);
        }
        rec=next;
    }
    zd_selrec_RemoveAll();
}

// -----------------------------------------------------------------------------

// List packages in topological order
void apm::Main_List() {
    algo_lib::FTxttbl tbl;
    AddCol(tbl,"package");
    AddCol(tbl,"depends");
    AddCol(tbl,"comment");
    // show packages in topological order
    ind_beg(_db_zd_topo_package_curs,package,_db) {
        AddRow(tbl);
        AddCell(tbl)=package.package;
        {
            cstring dep;
            algo::ListSep ls;
            ind_beg(package_c_pkgdep_curs,pkgdep,package) {
                dep<<ls<<parent_Get(pkgdep);
            }ind_end;
            AddCell(tbl)=dep;
        }
        AddCell(tbl)=package.comment;
    }ind_end;
    cstring out;
    FTxttbl_Print(tbl,out);
    prlog(out);
}
