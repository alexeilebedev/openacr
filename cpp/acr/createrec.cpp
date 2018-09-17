// (C) AlgoEngineering LLC 2008-2013
// (C) 2017-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/createrec.cpp -- Create record
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

static acr::FCtype *FindCtype(Tuple &tuple) {
    acr::FCtype *ctype = NULL;
    strptr head = tuple.head.value;
    if (head.n_elems) {
        // try a couple of paths to get this tuple recognized
        acr::FSsimfile *ssimfile = acr::ind_ssimfile_Find(head);
        if (ssimfile) {
            ctype = ssimfile->p_ctype;
        } else {
            ctype = acr::ind_ctype_Find(head);
            if (ctype) {
                ssimfile = ctype->c_ssimfile;
            }
        }
        if (ssimfile && !ssimfile->c_file) {// associate table file
            LoadSsimfile(*ssimfile);
        }
    }
    return ctype;
}

// -----------------------------------------------------------------------------

// Insert record in ACR's database.
// Upon first access of ssimfile, load ssimfile from disk.
//   If -trunc option is set, delete all records
//      This is different from marking records for deletion.
static acr::FRec* ReadTupleX(Tuple &tuple, acr::FFile &file, bool insert) {
    acr::FCtype *ctype = FindCtype(tuple);
    acr::FRec *ret = NULL;
    if (ctype) {
        // truncate table on first insertion
        // there is an implicit commit happening here -- we can't
        // subsequently insert new records as long as undeleted records stay
        // in the index.
        if (!file.autoloaded && ctype->n_insert == 0 && acr::_db.cmdline.trunc) {
            while (acr::FRec *rec = acr::zd_trec_First(*ctype)) {
                acr::rec_Delete(*rec);
            }
        }
        algo::Smallstr50 attr_pkey;
        if (c_field_N(*ctype) > 0) {
            attr_pkey = name_Get(*c_field_Find(*ctype, 0));
        }
        algo::Attr *attr = attr_Find(tuple, strptr(attr_pkey));
        if (!attr) {
            prerr(file.file<<":"<<file.lineno<<": acr.insert"
                  <<Keyval("ctype",ctype->ctype)
                  <<Keyval("attr",attr_pkey)
                  <<Keyval("warning","missing primary key attribute"));
        } else {
            ret = acr::CreateRec(file,ctype,tuple,attr,insert);
        }
    } else if (tuple.head.value == "acr.delete" && attrs_N(tuple) > 0) {
        tuple.head = attrs_qFind(tuple,0);
        attrs_Remove(tuple,0);
        ReadTuple(tuple, file, false);// delete mode
    } else if (tuple.head.value == "acr.insert" && attrs_N(tuple) > 0) {
        tuple.head = attrs_qFind(tuple,0);
        attrs_Remove(tuple,0);
        ReadTuple(tuple, file, true);// insert mode
    }
    return ret;
}

// -----------------------------------------------------------------------------

// insert tuple into database.
// if tuple cannot be inserted, return NULL.
// FILE    input file. if NULL, assign file of ssimfile!
//
acr::FRec* acr::ReadTuple(Tuple &tuple, acr::FFile &file, bool insert) {
    acr::FRec *ret = NULL;
    try {
        ret = ReadTupleX(tuple,file,insert);
        // deselect record if nothing changed.
        // desired effect:
        // after editing session, only changes are printed.
        if (file.deselect && ret && !ret->isnew && !ret->mod && !ret->del) {
            acr::Rec_Deselect(*ret);
        }
    } catch (algo_lib::ErrorX &) {
        prerr(file.file<<":"<<file.lineno<<": "<<tuple);// provide context
        throw;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void MergeTuple(algo::Tuple &tuple, acr::FRec &rec) {
    rec.tuple.head.value    = tuple.head.value;
    if (attrs_N(rec.tuple) > 0 && acr::_db.cmdline.merge) {
        ind_beg(algo::Tuple_attrs_curs,attr,tuple) {// merge incoming tuple into existing tuple
            algo::Attr *attr2=attr_Find(rec.tuple,attr.name);
            if (attr2) {
                attr2->value = attr.value;
            } else {
                attr_Add(rec.tuple,attr.name,attr.value);
            }
        }ind_end;
    } else {
        attrs_RemoveAll(rec.tuple);
        attrs_Addary(rec.tuple, attrs_Getary(tuple));
    }
}

// -----------------------------------------------------------------------------

static void InitSortkey(acr::FRec &rec, float rowid) {
    // initialize string sort field
    rec.sortkey.num=0;
    ch_RemoveAll(rec.sortkey.str);
    // update sort key for new record.
    acr::FSsimfile *ssimfile = rec.p_ctype->c_ssimfile;
    if (ssimfile && ssimfile->c_ssimsort) {
        acr::FField *sortfld = acr::ind_field_Find(ssimfile->c_ssimsort->sortfld);
        if (sortfld) {
            rec.sortkey.str = acr::EvalAttr(rec.tuple, *sortfld);
        }
        if (sortfld && sortfld->p_arg->c_bltin) {// numeric check
            double_ReadStrptrMaybe(rec.sortkey.num, rec.sortkey.str);
            ch_RemoveAll(rec.sortkey.str);
        }
    }
    rec.sortkey.rowid = rowid;
}

// -----------------------------------------------------------------------------

// Determine output file for ctype CTYPE loaded from input file INFILE.
static acr::FFile *PickOutfile(acr::FFile &infile, acr::FCtype *ctype) {
    acr::FFile *ret = &infile;
    if (!infile.sticky) {
        acr::FSsimfile *ssimfile = ctype->c_ssimfile;
        if (ssimfile && ssimfile->c_file) {
            ret = ssimfile->c_file;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// TUPLE: input tuple
// CTYPE: associated type, as found via the type tag
// ATTR: pkey attr
acr::FRec *acr::CreateRec(acr::FFile &file, acr::FCtype *ctype, algo::Tuple &tuple, algo::Attr *pkey_attr, bool insert) {
    bool isnew = !file.autoloaded;
    strptr pkey     = pkey_attr->value;
    acr::FRec *ret  = acr::ind_rec_Find(*ctype, pkey);
    // determine line of record
    // each time a record is inserted into a ssimfile, assign it ssimfile's next_rowid,
    // and increment ssimfile's next_rowid field.
    // if the cmdline.rowid option is enabled, and the input line contains the
    // attribute acr.rowid, then allow this attribute to set the line id,
    // and strip the attribute before saving record.
    //
    // records are written to disk in rowid order.
    float rowid   = ret ? ret->sortkey.rowid : ctype->next_rowid;
    if (insert) {
        ctype->next_rowid = float_Max(ctype->next_rowid, rowid + 1);
        algo::Attr *rowid_attr = attr_Find(tuple, "acr.rowid");
        if (rowid_attr) {
            (void)float_ReadStrptrMaybe(rowid, rowid_attr->value);
            attrs_Remove(tuple, attrs_rowid_Get(tuple, *rowid_attr));
        }
        // detect duplicate records...
        if (ret && !acr::_db.cmdline.replace) {
            prerr("acr.duplicate_key  key:"<<ctype->ctype<<":"<<pkey_attr->value);
            algo_lib::_db.exit_code=1;
            return NULL;
        }
        if (!ret) {
            ret             = &acr::rec_Alloc();
            ret->lineno     = file.lineno;
            ret->p_ctype    = ctype;
            ret->p_infile   = &file;
            ret->p_outfile  = PickOutfile(file, ctype);
            ret->pkey       = pkey;
            ret->isnew      = isnew;
            acr::_db.report.n_insert += isnew;
            (void)acr::rec_XrefMaybe(*ret);
            if (isnew) {
                Rec_Select(*ret);
            }
        }
        // if the record was scheduled for deletion, cancel that!
        ret->del            = false;
        // if this is not an initial insertion (isnew==false),
        // see if this record is any different from the one before...
        // if so, mark as modified.
        // the record is already marked as not-deleted
        if (isnew && !Tuple_EqualQ(ret->tuple, tuple)) {
            ret->mod = true;
            acr::_db.report.n_update++;
        }
        MergeTuple(tuple,*ret);
        InitSortkey(*ret,rowid);
        ctype->n_insert += isnew;
    } else {
        if (ret) {
            ret->del = true;
            Rec_Select(*ret);
        }
    }
    return ret;
}
