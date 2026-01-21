// Copyright (C) 2024,2026 AlgoRND
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
// Source: cpp/apm/rec.cpp
//

#include "include/algo.h"
#include "include/apm.h"
#include "include/lib_ctype.h"

// Load tuples from FILENAME into REC table
static void LoadRecsFile(algo::strptr filename) {
    ind_beg(algo::FileLine_curs,line,filename) {
        apm::FRec &rec=apm::rec_Alloc();
        bool good=false;
        if (Tuple_ReadStrptrMaybe(rec.tuple,line) && attrs_N(rec.tuple)>0) {
            rec.p_ssimfile=apm::ind_ssimfile_Find(rec.tuple.head.value);
            if (rec.p_ssimfile) {
                rec.rec=tempstr()<<rec.p_ssimfile->ssimfile<<":"<<attrs_Find(rec.tuple,0)->value;
                good=rec_XrefMaybe(rec);
            }
        }
        if (!good) {
            rec_Delete(rec);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Add any records from ssimfile SSIMFILE matching regx VALUE_REGX
// to ZD_CHOOSEREC (initial selection).
static void ChooseRec(apm::FSsimfile &ssimfile, algo_lib::Regx &value_regx) {
    if (literal_Get(value_regx.flags)) {
        tempstr key=tempstr()<<ssimfile.ssimfile<<":"<<value_regx.expr;
        if (apm::FRec *rec=apm::ind_rec_Find(key)) {
            zd_chooserec_Insert(*rec);
        }
    } else {
        ind_beg(apm::ssimfile_zd_ssimfile_rec_curs,rec,ssimfile) {
            if (Regx_Match(value_regx,attrs_Find(rec.tuple,0)->value)) {
                zd_chooserec_Insert(rec);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Evaluate regx in pkgkey, compute transitive closure according to pkgkey.l, pkgkey.ndown
// and add any selected records to global zd_selrec table
static void SelectPkgkeyRecs(apm::FPkgkey &pkgkey) {
    tempstr key(key_Get(pkgkey));
    algo_lib::Regx ssimfile_regx;
    algo_lib::Regx value_regx;
    Regx_ReadAcr(ssimfile_regx,Pathcomp(key,":LL"),true);
    Regx_ReadAcr(value_regx,Pathcomp(key,":LR"),true);
    vrfy_(!apm::zd_chooserec_N());
    // produce initial chooserec selection
    if (literal_Get(ssimfile_regx.flags)) {
        if (apm::FSsimfile *ssimfile=apm::ind_ssimfile_Find(ssimfile_regx.expr)) {
            ChooseRec(*ssimfile,value_regx);
        }
    } else {
        ind_beg(apm::_db_ssimfile_curs,ssimfile,apm::_db) if (Regx_Match(ssimfile_regx,ssimfile.ssimfile)) {
            ChooseRec(ssimfile,value_regx);
        }ind_end;
    }
    // add referencing records in NDOWN steps
    // the zd_chooserec list grows as we scan it, each new record is added with level=parent.level+1
    // When level exceeds pkgkey.ndown, we stop adding, and eventually reach the end of the list
    ind_beg(apm::_db_zd_chooserec_curs,rec,apm::_db) {
        ind_beg(apm::rec_c_child_curs,childrec,rec) {
            if (!zd_chooserec_InLlistQ(childrec)) {
                prcat(verbose2,pkgkey.pkgkey<<": adding child "<<childrec.rec<<", level "<<rec.level+1);
                zd_chooserec_Insert(childrec);
                childrec.level=rec.level+1;
            }
        }ind_end;
    }ind_end;
    // reset level, and add choosen records to zd_selrec
    ind_beg(apm::_db_zd_chooserec_curs,rec,apm::_db) {
        zd_selrec_Insert(rec);
        rec.level=0;
    }ind_end;
    // clear zd_chooserec
    apm::zd_chooserec_RemoveAll();
}

// -----------------------------------------------------------------------------

// return TRUE if the field is a valid edge for transitive closure.
// The field is chosen if it's the pkey, or a leftmost subtring of pkey
bool apm::LeftCheckQ(apm::FField &field) {
    apm::FCtype &ctype=*field.p_ctype;
    bool ret=field.reftype == dmmeta_Reftype_reftype_Pkey;
    apm::FField *pkey =c_field_Find(ctype,0);
    if (pkey) {
        ret = pkey==&field
            || (field.c_substr && field.c_substr->srcfield==pkey->field && algo::LeftPathcompQ(field.c_substr->expr.value));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Load all records (FRec) from dataset _db.cmdline.data_in)
// For each record (FRec), compute p_ssimfile, pkey, tuple
// Populate global zd_rec index
// Populate zd_ssimfile_rec for each ssimfile (records grouped by ssimfile)
// Populate c_child and c_left_child arrays for each record (these are records referring
//    to choosen records)
// For each record, evaluate ssimreq rules. If there is a match, find corresponding
// record and add it as a "match" to this key.
//
// For each match between FPkgkey and FRec, Create an FPkgrec record,
// and group FPkgrec by FRec (zd_rec_pkgrec) and by FPackage (zd_rec)
// This structure allows full analysis of package composition and checking
void apm::LoadRecs() {
    if (DirectoryQ(_db.cmdline.data_in)) {
        // load all ssimfile records
        // use acr with `-rowid` option to capture correct rowids
        command::acr_proc acr;
        acr.cmd.query="%";
        acr.cmd.loose=true;
        acr.cmd.rowid=true;
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile, "apm.recs");
        acr.fstdout << ">"<<tempfile.filename;
        acr_Exec(acr);
        LoadRecsFile(tempfile.filename);
    } else {
        LoadRecsFile(_db.cmdline.data_in);
    }
    verblog("loaded "<<ind_rec_N()<<" records");
    // build graph of all records
    // compute c_child, c_leftchild
    ind_beg(_db_zd_rec_curs,rec,_db) {
        ind_beg(ctype_c_field_curs,field,*rec.p_ssimfile->p_ctype) if (LeftCheckQ(field)) {
            algo::Attr *attr=attr_Find(rec.tuple,name_Get(field.c_substr ? *field.c_substr->p_srcfield:field));
            if (attr && field.p_arg->c_ssimfile) {
                algo::strptr value = field.c_substr ? Pathcomp(attr->value,field.c_substr->expr.value) : attr->value;
                tempstr parent_key=tempstr()<<field.p_arg->c_ssimfile->ssimfile<<":"<<value;
                if (apm::FRec *parent = apm::ind_rec_Find(parent_key)) {
                    c_child_Insert(*parent,rec);
                }
            }
        }ind_end;
        // evaluate ssimreq, add child record to the set
        ind_beg(apm::ctype_c_ssimreq_curs,ssimreq,*rec.p_ssimfile->p_ctype) if (!ssimreq.exclude) {
            apm::FField &reqfield=*ssimreq.p_field;
            algo::Attr *attr=attr_Find(rec.tuple,name_Get(reqfield.c_substr ? *reqfield.c_substr->p_srcfield:reqfield));
            if (attr) {
                algo::strptr value = reqfield.c_substr ? Pathcomp(attr->value,reqfield.c_substr->expr.value) : attr->value;
                if (Regx_Match(ssimreq.regx_value,value)) {
                    algo_lib::Replscope R;
                    lib_ctype::FillReplscope(R, rec.tuple);
                    tempstr child_key = Subst(R,ssimreq.ssimreq);
                    apm::FRec *child = apm::ind_rec_Find(child_key);
                    if (child && child != &rec) {
                        prcat(verbose2,child->rec<<" now child of "<<rec.rec);
                        c_child_Insert(rec,*child);
                    }
                }
            }
        }ind_end;
    }ind_end;

    // Evaluate each package's pkgkeys, and create lists
    // package.zd_pkgrec
    // rec.zd_rec_pkgrec
    // Packages are scanned in topological order, so
    // we can say the last package to reference a record is the package
    // that owns it.
    ind_beg(_db_zd_topo_package_curs,package,_db) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,package) {
            zd_selrec_RemoveAll();
            SelectPkgkeyRecs(pkgkey);
            ind_beg(_db_zd_selrec_curs,rec,_db) {
                apm::FPkgrec &pkgrec=pkgrec_Alloc();
                pkgrec.p_package=&package;
                pkgrec.p_rec=&rec;
                pkgrec.p_pkgkey=&pkgkey;
                vrfy_(pkgrec_XrefMaybe(pkgrec));
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Select records belonging to package PACKAGE by adding them to zd_selrec
// These are all the records that the package references via zd_pkgrec,
// minus any records claimed by packages that depend on PACKAGE
void apm::SelectPkgRecs(apm::FPackage &package) {
    ind_beg(package_zd_pkgrec_curs,pkgrec,package) {
        // only select records for which this is the last package to claim it
        if (&pkgrec == zd_rec_pkgrec_Last(*pkgrec.p_rec)) {
            zd_selrec_Insert(*pkgrec.p_rec);
        }
    }ind_end;
}
