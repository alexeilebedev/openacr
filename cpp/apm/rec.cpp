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

static float GetRowId(algo::Tuple& tuple) {
    float ret = 0;
    algo::Attr* attr = attr_Find(tuple, "acr.rowid");
    if (attr) {
        float_ReadStrptrMaybe(ret, attr->value);
    }
    return ret;
}

static tempstr EvalAttr(algo::Tuple& tuple, apm::FField& field) {
    tempstr ret;
    algo::Attr* attr = attr_Find(tuple, name_Get(field.c_substr ? *field.c_substr->p_srcfield : field));
    if (attr) {
        ret << (field.c_substr ? Pathcomp(attr->value, field.c_substr->expr.value) : attr->value);
    }
    return ret;
}

static void SetSortkey(apm::FRec& rec) {
    rec.sortkey.ctype = rec.p_ssimfile->p_ctype->ctype;
    rec.sortkey.num = 0;
    if (rec.p_ssimfile->c_ssimsort) {
        apm::FField* sortfld = apm::ind_field_Find(rec.p_ssimfile->c_ssimsort->sortfld);
        if (sortfld) {
            rec.sortkey.str = EvalAttr(rec.tuple, *sortfld);
        }
        if (sortfld && sortfld->p_arg->c_bltin) { // numeric check
            double_ReadStrptrMaybe(rec.sortkey.num, rec.sortkey.str);
            ch_RemoveAll(rec.sortkey.str);
        }
    }
    rec.sortkey.rowid = GetRowId(rec.tuple);
}

// Load tuples from FILENAME into REC table
static void LoadRecsFile(algo::strptr filename) {
    ind_beg(algo::FileLine_curs, line, filename) {
        apm::FRec& rec = apm::rec_Alloc();
        bool good = false;
        if (Tuple_ReadStrptrMaybe(rec.tuple, line) && attrs_N(rec.tuple) > 0) {
            rec.p_ssimfile = apm::ind_ssimfile_Find(rec.tuple.head.value);
            if (rec.p_ssimfile) {
                rec.rec = tempstr() << rec.p_ssimfile->ssimfile << ":" << attrs_Find(rec.tuple, 0)->value;
                SetSortkey(rec);
                good = rec_XrefMaybe(rec);
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

// Evaluate regx in pkgkey, compute transitive closure according to pkgkey.up, pkgkey.down, pkgkey.exclude
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

    // explicit pkgkey selection
    ind_beg(apm::_db_zd_chooserec_curs,rec,apm::_db) {
        zd_selrec_Insert(rec);
    }ind_end;
    pkgkey.n_explicit = apm::zd_selrec_N();

    apm::zd_chooserec_RemoveAll();

    // closure down
    if (pkgkey.down) {
        // restore original zd_chooserec list
        apm::FRec* r = apm::zd_selrec_First();
        for(u32 i = 0; i < pkgkey.n_explicit; ++i) {
            apm::zd_chooserec_Insert(*r);
            r = apm::zd_selrec_Next(*r);
        }

        // add referencing records in steps
        // the zd_chooserec list grows as we scan it, each new record is added with level=parent.level+1
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            ind_beg(apm::rec_c_child_curs, childrec, rec) {
                if (!zd_chooserec_InLlistQ(childrec)) {
                    prcat(verbose2, pkgkey.pkgkey << ": adding child " << childrec.rec << ", level " << rec.level + 1);
                    zd_chooserec_Insert(childrec);
                    childrec.level = rec.level + 1;
                }
            }ind_end;
        }ind_end;

        // reset level, and add choosen records to zd_selrec
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            zd_selrec_Insert(rec);
            rec.level = 0;
        }ind_end;
        // clear zd_chooserec
        apm::zd_chooserec_RemoveAll();

        pkgkey.n_down = apm::zd_selrec_N() - pkgkey.n_explicit;
    }

    // closure up
    if (pkgkey.up) {
        // restore original zd_chooserec list
        apm::FRec* r = apm::zd_selrec_First();
        for(u32 i = 0; i < pkgkey.n_explicit; ++i) {
            apm::zd_chooserec_Insert(*r);
            r = apm::zd_selrec_Next(*r);
        }

        // add referenced records in steps
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            ind_beg(apm::rec_c_parent_curs, parentrec, rec) {
                if (!zd_chooserec_InLlistQ(parentrec)) {
                    prcat(verbose2, pkgkey.pkgkey << ": adding parent " << parentrec.rec << ", level " << rec.level + 1);
                    zd_chooserec_Insert(parentrec);
                    parentrec.level = rec.level + 1;
                }
            }ind_end;
        }ind_end;

        // reset level, and add choosen records to zd_selrec
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            zd_selrec_Insert(rec);
            rec.level = 0;
        }ind_end;
        // clear zd_chooserec
        apm::zd_chooserec_RemoveAll();

        pkgkey.n_up = apm::zd_selrec_N() - pkgkey.n_explicit - pkgkey.n_down;
    }

    // clear zd_chooserec
    apm::zd_chooserec_RemoveAll();
}

// -----------------------------------------------------------------------------

// return TRUE if the field is a valid edge for transitive closure.
// The field is chosen if it's the pkey, or a leftmost subtring of pkey
static bool LeftCheckQ(apm::FField &field) {
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
        // The file is read back on the next line, and the redirect truncated it
        // before the child ran, so a child that fails hands the run an empty
        // database rather than an error. Every action apm can take is computed
        // from these records, and an action computed from none of them looks
        // like an action on a package that holds nothing: the checking variant
        // stops the run instead.
        acr_ExecX(acr);
        LoadRecsFile(tempfile.filename);
    } else {
        LoadRecsFile(_db.cmdline.data_in);
    }
    verblog("loaded "<<ind_rec_N()<<" records");
    // build graph of all records
    // compute c_child, c_leftchild
    ind_beg(_db_zd_rec_curs, rec, _db) {
        ind_beg(ctype_c_field_curs, field, *rec.p_ssimfile->p_ctype) if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
            algo::Attr *attr = attr_Find(rec.tuple,name_Get(field.c_substr ? *field.c_substr->p_srcfield:field));
            if (attr && field.p_arg->c_ssimfile) {
                algo::strptr value = field.c_substr ? Pathcomp(attr->value,field.c_substr->expr.value) : attr->value;
                tempstr parent_key=tempstr()<<field.p_arg->c_ssimfile->ssimfile<<":"<<value;
                if (apm::FRec *parent = apm::ind_rec_Find(parent_key)) {
                    if (LeftCheckQ(field)) {
                        c_child_Insert(*parent, rec);
                    }
                    c_parent_Insert(rec, *parent);
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
    ind_beg(_db_zd_topo_package_curs, package, _db) {
        // included records
        ind_beg(package_zd_pkgkey_curs, pkgkey, package) if (!pkgkey.exclude) {
            zd_selrec_RemoveAll();
            SelectPkgkeyRecs(pkgkey);
            ind_beg(_db_zd_selrec_curs, rec, _db) {
                apm::FPkgrec& pkgrec = pkgrec_Alloc();
                pkgrec.p_package    = &package;
                pkgrec.p_rec        = &rec;
                pkgrec.p_pkgkey     = &pkgkey;
                vrfy(pkgrec_XrefMaybe(pkgrec), algo_lib::_db.errtext);
            }ind_end;
        }ind_end;

        // excluded records
        ind_beg(package_zd_pkgkey_curs, pkgkey, package) if (pkgkey.exclude) {
            zd_selrec_RemoveAll();
            SelectPkgkeyRecs(pkgkey);
            DropSelectedPkgrec(package);
        }ind_end;
    }ind_end;

    // Subtract from each package the records the packages extending it capture.
    //
    // A package that extends another is not part of it: a downstream package
    // builds on a base distribution without shipping inside it, so nothing the
    // downstream package captures may go out as the base.  Written as regxes,
    // that fact costs one negative key per leaked record, and a base package
    // whose downstream is a whole platform carries thousands of them.  They sit
    // in the base package rather than in the package the records belong to, and
    // each says only that something does not belong, never whose it is.
    // Written as a relation it is one `dev.pkgdep` row of type `extend`, and
    // the records follow from whatever the extending package claims.
    //
    // The pass runs after every package has been evaluated rather than inside
    // the loop above.  That loop visits a package before the packages that
    // extend it -- a package is evaluated after its parents -- so an extender's
    // records do not exist yet at the moment its parent is being built.
    ind_beg(_db_pkgdep_curs,pkgdep,_db) if (pkgdep.pkgdeptype == dev_pkgdeptype_extend) {
        zd_selrec_RemoveAll();
        SelectPkgRecs(*pkgdep.p_package);
        DropSelectedPkgrec(*pkgdep.p_parent,true);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Whether PKGKEY names REC outright, rather than matching it.
// A pkgkey and a record are spelled the same way, `<ssimfile>:<pkey>`, so the
// test is that the two strings agree and that the key holds no pattern.
// Reaching a record through the reference closure of a literal key does not
// count: `dmmeta.ns:abt_md` names a namespace and nothing else, whatever its
// closure goes on to visit.
bool apm::NamesRecQ(apm::FPkgkey &pkgkey, apm::FRec &rec) {
    bool ret = key_Get(pkgkey) == rec.rec;
    if (ret) {
        algo_lib::Regx value_regx;
        Regx_ReadAcr(value_regx,Pathcomp(key_Get(pkgkey),":LR"),true);
        ret = literal_Get(value_regx.flags);
    }
    return ret;
}

// -----------------------------------------------------------------------------

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
void apm::DropSelectedPkgrec(apm::FPackage &package, bool keep_literal DFLTVAL(false)) {
    apm::FPkgrec* cur = apm::zd_pkgrec_First(package);
    while (cur) {
        apm::FPkgrec* next = apm::package_zd_pkgrec_Next(*cur);
        bool named = keep_literal && NamesRecQ(*cur->p_pkgkey,*cur->p_rec);
        if (zd_selrec_InLlistQ(*cur->p_rec) && !named) {
            pkgrec_Delete(*cur);
        }
        cur = next;
    }
}

// -----------------------------------------------------------------------------

// Select records belonging to package PACKAGE by adding them to zd_selrec.
// These are all the records that the package references via zd_pkgrec.
void apm::SelectPkgRecs(apm::FPackage &package) {
    ind_beg(package_zd_pkgrec_curs, pkgrec, package) {
        zd_selrec_Insert(*pkgrec.p_rec);
    }ind_end;
}
