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
// Source: cpp/apm/show.cpp
//

#include "include/algo.h"
#include "include/apm.h"
#include "include/sha.h"

// -----------------------------------------------------------------------------

// Topologically sort selected records and save them to file RECFILE
// Return success code
// Every apm action that needs these records reads them back from RECFILE rather
// than from the selection they were written from, so a write that fails leaves
// the file absent and every reader after it sees an empty record set. The
// failure is reported here, at the one place that performs the write, naming the
// file and the decoded errno; what a failed write means for the run is the
// caller's to decide.
bool apm::SaveSelrecToFile(algo::strptr recfile) {
    tempstr recs;
    DeleteFile(recfile);
    ind_beg(_db_zd_selrec_curs,rec,_db) {
        recs << rec.tuple << eol;
    }ind_end;
    return algo::SaveFile(recs,recfile,"apm.recfile_write","package records could not be written; the records they select were not acted on");
}

// -----------------------------------------------------------------------------

// Save local package definitions to file
void apm::SavePackageDefs(algo::strptr filename) {
    cstring str;
    ind_beg(_db_package_curs,package,_db) {
        dev::Package out;
        package_CopyOut(package,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_pkgdep_curs,pkgdep,_db) {
        dev::Pkgdep out;
        pkgdep_CopyOut(pkgdep,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_pkgkey_curs,pkgkey,_db) {
        dev::Pkgkey out;
        pkgkey_CopyOut(pkgkey,out);
        str << out << eol;
    }ind_end;
    StringToFile(str,filename);
}

// -----------------------------------------------------------------------------

// Return true if DIR is a repository no package has reached yet: it has no data
// directory, as in a fresh checkout that a first push populates.  An empty DIR
// names the current directory, which is never empty.
bool apm::EmptyOriginQ(algo::strptr dir) {
    return dir != "" && !DirectoryQ(DirFileJoin(dir,"data"));
}

// -----------------------------------------------------------------------------

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
bool apm::CollectPkgrecFromDir(algo::strptr package, algo::strptr recfile, algo::strptr dir) {
    tempstr full_recfile=DirFileJoin(algo::GetCurDir(),recfile);
    bool ok = true;
    command::apm_proc apm;
    apm.path = GetApmPath(dir);
    if (_db.cmdline.l) { // use local package definitions
        apm.cmd.pkgdata = DirFileJoin(algo::GetCurDir(),_db.pkgdata_recfile);
    }
    algo_lib::PushDir(dir);
    apm.cmd.package.expr=package;
    apm.cmd.showrec=true;
    apm.cmd.t=_db.cmdline.t;
    // make filename absolute because we're now in a different directory
    apm.fstdout << ">"<<full_recfile;
    apm_Start(apm);
    algo_lib::PopDir();
    apm_Wait(apm);
    ok = apm.status==0;
    if (!ok) {
        prerr("apm.pkgrec_read"
              <<Keyval("dir",dir)
              <<Keyval("recfile",recfile)
              <<Keyval("status",algo::DescribeWaitStatus(apm.status))
              <<Keyval("comment","package records could not be read from the directory"));
    }
    return ok;
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
            prlog(rec->tuple);
        } else {
            zd_selrec_Remove(*rec);
        }
        rec=next;
    }
    zd_selrec_RemoveAll();
}

// -----------------------------------------------------------------------------

static tempstr ComputeSha1(strptr gitfile) {
    Sha1Ctx sha1;
    tempstr content = algo::FileToString(gitfile);
    Update(sha1,algo::strptr_ToMemptr(content));
    Finish(sha1);
    algo::Signature sig = GetDigest(sha1);
    tempstr out;
    strptr_PrintBase64(algo::memptr_ToStrptr(signature_Getary(sig)), out);
    return out;
}

static bool GenFileQ(strptr gitfile) {
    bool ret = false;
    ret |= StartsWithQ(gitfile, "aws/gen/");
    ret |= StartsWithQ(gitfile, "cpp/gen/");
    ret |= StartsWithQ(gitfile, "include/gen/");
    ret |= StartsWithQ(gitfile, "js/gen/");
    ret |= StartsWithQ(gitfile, "ts/gen/");
    ret |= StartsWithQ(gitfile, "txt/ssimdb/");
    return ret;
}

static tempstr ComputeSha1NoteMaybe(apm::FRec& rec) {
    tempstr ret;
    if (rec.p_ssimfile->ssimfile == dmmeta_Ssimfile_ssimfile_dev_gitfile) {
        algo::Attr* attr = algo::attr_Find(rec.tuple, "gitfile");
        if (attr && !GenFileQ(attr->value)) {
            tempstr sha1 = ComputeSha1(attr->value);
            ret << "# SHA1 = " << sha1 << eol;
        }
    }
    return ret;
}

// Print the records of every selected package to stdout, in sorted order, with a
// sha1 note above each hand-written file the package carries.
//
// The records used to be written into apm/gen/<package>.ssim and kept in git,
// where they were regenerated on every normalize run and conflicted with every
// branch that touched a tracked file.  Nothing read them: they are a view of the
// package, and a view is computed when it is asked for.
void apm::Main_Generate() {
    ind_beg(apm::_db_package_curs, package, apm::_db) {
        if (Regx_Match(_db.cmdline.package, package.package)) {
            zd_selrec_RemoveAll();
            SelectPkgRecs(package);

            apm::c_rec_RemoveAll();
            ind_beg(apm::_db_zd_selrec_curs, rec, apm::_db) {
                apm::c_rec_Insert(rec);
            }ind_end;
            apm::c_rec_QuickSort();

            tempstr recs;
            ind_beg(apm::_db_c_rec_curs, rec, apm::_db) {
                // non-generated files get a comment with their SHA1 to track the changes
                recs << ComputeSha1NoteMaybe(rec);
                recs << rec.tuple << eol;
            }ind_end;

            prlog(recs);
        }
    }ind_end;
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
