// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/createrec.cpp -- Create record
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

// Find ctype from tuple
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
    }
    return ctype;
}

// -----------------------------------------------------------------------------

// Insert record in ACR's database.
// Upon first access of ssimfile, load ssimfile from disk.
// If -trunc option is set, mark all records for deletion
acr::FRec* acr::ReadTuple(Tuple &tuple, acr::FFile &file, acr::ReadMode read_mode) {
    acr::FCtype *ctype = FindCtype(tuple);
    acr::FRec *ret = NULL;
    if (ctype) {
        LoadRecords(*ctype);
        // truncate table on first insertion
        if (!file.autoloaded && ctype->n_insert == 0 && acr::_db.cmdline.trunc) {
            ind_beg(acr::ctype_zd_ctype_rec_curs,rec,*ctype) {
                rec.del=true;
            }ind_end;
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
            ret = acr::CreateRec(file,ctype,tuple,attr,read_mode);
        }
    } else if (read_mode_SetStrptrMaybe(read_mode, tuple.head.value)) {// override mode from input
        // one of acr.insert, acr.replace, acr.delete, acr.merge, acr.update
        if (attrs_N(tuple) > 0) {
            tuple.head = attrs_qFind(tuple,0);
            attrs_Remove(tuple,0);
            ret = ReadTuple(tuple, file, read_mode);
        }
    } else {
        // unknown ctype
        if (tuple.head.value != "") {
            _db.report.n_ignore++;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Calculate record's SORTKEY which is a combination
// of the value of its SSIMSORT attribute and the newly provided ROWID.
void acr::UpdateSortkey(acr::FRec &rec, float rowid) {
    rec.sortkey.ctype=rec.p_ctype->ctype;
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
    acr::FSortkey &sortkey = acr::ind_sortkey_GetOrCreate(rec.sortkey);
    if (rowid==-1) {
        rowid = sortkey.next_rowid;
    }
    double_Update(sortkey.next_rowid,rowid+1);
    rec.sortkey.rowid = rowid;
}

// -----------------------------------------------------------------------------

// Determine output file for ctype CTYPE loaded from input file INFILE.
// If the file has flag "STICKY", then the record is associated back to that file
// Otherwise, the records is assigned to the appropriate ssimfile (based on the type)
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

// Create a new record from tuple TUPLE, having primary key PKEY_ATTR and type
// CTYPE (as found via the type tag).
// if INSERT is specified, the record inserted. Otherwise, it's deleted
// This function checks CMDLINE.REPLACE flag to see if the record is allowed
// to replace an existing record; if CMDLINE.MERGE is specified, attributes are merged
// into an existing record if one exists
acr::FRec *acr::CreateRec(acr::FFile &file, acr::FCtype *ctype, algo::Tuple &tuple, algo::Attr *pkey_attr, acr::ReadMode read_mode) {
    strptr pkey     = pkey_attr->value;
    acr::FRec *ret  = acr::ind_ctype_rec_Find(*ctype, pkey);
    // row id for this tuple
    // if unspecified, -1 is the default (will be calculated later in UpdateSortkey)
    float rowid   = ret ? ret->sortkey.rowid : -1;
    if (read_mode == acr_ReadMode_acr_select) {
        if (ret) {
            Rec_Select(*ret);
        }
    } else if (read_mode == acr_ReadMode_acr_delete) {
        if (ret) {
            ret->del = true;
        } else {
            _db.report.n_ignore++;
        }
    } else {
        // detect acr.rowid attribute and update record rowid
        // (which allows inserting records into the middle of an unsorted file)
        algo::Attr *rowid_attr = attr_Find(tuple, "acr.rowid");
        if (rowid_attr) {
            (void)float_ReadStrptrMaybe(rowid, rowid_attr->value);
            attrs_Remove(tuple, attrs_rowid_Get(tuple, *rowid_attr));
        }
        if (read_mode == acr_ReadMode_acr_insert) {// insert of existing record -> ignore
            if (ret && !ret->del) {
                _db.report.n_ignore++;
                return NULL;
            }
        } else if (read_mode == acr_ReadMode_acr_update) {// update of non-existent record -> ignore
            if (!(ret && !ret->del)) {
                _db.report.n_ignore++;
                return NULL;
            }
        }
        if (!ret || ret->del) {
            ctype->n_insert += !file.autoloaded;
        }
        if (!ret) {
            ret             = &acr::rec_Alloc();
            ret->lineno     = file.lineno;
            ret->p_ctype    = ctype;
            ret->p_infile   = &file;
            ret->p_outfile  = PickOutfile(file, ctype);
            ret->pkey       = pkey;
            ret->isnew      = !file.autoloaded;
            (void)acr::rec_XrefMaybe(*ret);
        }
        // save old tuple
        algo::Tuple prev;
        prev.head=ret->tuple.head;
        attrs_Addary(prev,attrs_Getary(ret->tuple));
        // if the record was scheduled for deletion, cancel that!
        ret->del = false;
        ret->tuple.head.value = tuple.head.value;
        // reset existing tuple if replace or insert
        if (read_mode == acr_ReadMode_acr_replace || read_mode == acr_ReadMode_acr_insert) {
            attrs_RemoveAll(ret->tuple);
        }
        // merge incoming tuple into existing tuple
        // incoming tuple may have duplicate attributes (e.g. a:b a:c)
        // so we can't just copy them over. must iterate and merge
        ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
            algo::Attr *attr2=attr_Find(ret->tuple,attr.name);
            if (attr2) {
                attr2->value = attr.value;
            } else {
                attr_Add(ret->tuple,attr.name,attr.value);
            }
        }ind_end;
        // mark record as modified if something changed
        if (!file.autoloaded && !ret->isnew && !Tuple_Eq(prev,ret->tuple)) {
            ret->mod = true;
        }
        UpdateSortkey(*ret,rowid);
    }
    return ret;
}
