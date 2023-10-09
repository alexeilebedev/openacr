// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
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
// Source: cpp/acr/check.cpp
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

static void CheckArgs_Rec(acr::FRec &rec, acr::FCtype &ctype, acr::FCheck &check) {
    check.n_record++;
    ind_beg(acr::ctype_c_field_curs,  field, ctype){
        if (Attr *attr = attr_Find(rec.tuple,name_Get(field), 0)) {
            int maxlen = field.max_attr_len;
            if (!maxlen && c_field_N(*field.p_arg)==1) {
                maxlen = c_field_Find(*field.p_arg,0)->max_attr_len;
            }
            int attrlen = 0;
            if (maxlen) {
                attrlen = ch_N(EvalAttr(rec.tuple, field));
                if (maxlen && attrlen > maxlen) {
                    if (++check.n_err < acr::_db.cmdline.maxshow) {
                        NoteErr(NULL,&rec,&field
                                ,tempstr()<<"acr.attr_too_long"
                                <<Keyval("field",field.field)
                                <<Keyval("length",attrlen)
                                <<Keyval("limit",maxlen)
                                <<Keyval("comment","Attribute too long."));
                    }
                }
            }
            if (field.arg == "char" && ch_N(attr->value) != 1) {
                if (++check.n_err < acr::_db.cmdline.maxshow) {
                    NoteErr(NULL,&rec,&field
                            ,tempstr()<<"Attribute '"<<name_Get(field)
                            <<"' must have length 1");
                }
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CheckArgs(acr::FCheck &check) {
    ind_beg(acr::_db_zd_sel_ctype_curs, ctype, acr::_db) {
        ind_beg(acr::ctype_zd_selrec_curs,  rec, ctype) {
            CheckArgs_Rec(rec,ctype,check);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void SuggestAlternatives(acr::FCtype &ctype, acr::FField &field, acr::FCheck &check) {
    ind_beg(acr::check_c_bad_rec_curs,rec,check) {
        NoteErr(NULL,&rec,&field
                ,tempstr()<< "Invalid value "<<name_Get(field)
                <<":"<<EvalAttr(rec.tuple, field));
    }ind_end;
    tempstr help;
    help << "Type               "<<field.arg << eol;
    help << "Valid values       ";
    algo::ListSep ls(", ");
    int idx = 0;
    ind_beg(acr::ctype_zd_trec_curs,  rec, *field.p_arg) {
        if (idx++ > 100) {
            help << ", ...";
            break;
        }
        help << ls << rec.pkey;
    }ind_end;
    NoteErr(&ctype,NULL,NULL,help);
}

// -----------------------------------------------------------------------------

static void CheckXref_Field(acr::FCtype &ctype, acr::FField &field, acr::FCheck &check) {
    if (RecordsLoadedQ(*field.p_arg)) {// don't check ssimfiles that are not loaded
        acr::c_bad_rec_RemoveAll(check);
        ind_beg(acr::ctype_zd_selrec_curs, rec, ctype) {// loop through all records for this ctype
            tempstr attr(EvalAttr(rec.tuple, field));// find attribute value
            if (!acr::ind_rec_Find(*field.p_arg,attr)) {// check index for pkey
                c_bad_rec_Insert(check, rec);
            }
        }ind_end;
        if (c_bad_rec_N(check)) {
            check.n_err++;
            if (check.n_err < acr::_db.cmdline.maxshow) {
                SuggestAlternatives(ctype,field,check);
            }
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckXrefs(acr::FCheck &check) {
    ind_beg(acr::_db_zd_sel_ctype_curs, ctype, acr::_db) {
        int nbefore = check.n_err;
        ind_beg(acr::ctype_c_field_curs, field, ctype) {
            if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
                CheckXref_Field(ctype,field,check);
            }
        }ind_end;
        if (check.n_err > nbefore) {
            NoteErr(&ctype,NULL,NULL
                    ,tempstr()<<"acr.badrefs"
                    <<Keyval("ctype",ctype.ctype)
                    <<Keyval("nbad",check.n_err));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CheckFunique() {
    ind_beg(acr::_db_zd_sel_ctype_curs, ctype, acr::_db) {
        ind_beg(acr::ctype_c_field_curs, field, ctype) if (field.unique) {
            // compute key: it is field + field value
            ind_beg(acr::ctype_zd_selrec_curs, rec, ctype) {// loop through all records for this ctype
                tempstr value(EvalAttr(rec.tuple, field));// find attribute value
                tempstr key = tempstr()<<field.field<<":"<<value;// is this a valid combination?
                if (acr::ind_uniqueattr_Find(key)) {
                    NoteErr(NULL,&rec,&field,tempstr()<<"acr.duplicate_value"
                            <<Keyval("field",field.field)
                            <<Keyval("value",value)
                            <<"  comment:'Duplicate value for unique field'");
                } else {
                    acr::ind_uniqueattr_GetOrCreate(key);
                }
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void acr::CheckSsimreq() {
    ind_beg(acr::_db_ssimreq_curs,ssimreq,acr::_db) {
        acr::FCtype *sub = ssimreq.p_ssimfile->p_ctype;
        acr::FCtype *super = ssimreq.p_field->p_ctype;

        // load files that are needed to execute the check
        // (but only if the involved records are already loaded)
        if (!acr::zd_selrec_EmptyQ(*sub) && super->c_ssimfile && !super->c_ssimfile->c_file) {
            LoadSsimfile(*super->c_ssimfile);
        }
        if (ssimreq.bidir) {
            if (!acr::zd_selrec_EmptyQ(*super) && !sub->c_ssimfile->c_file) {
                LoadSsimfile(*sub->c_ssimfile);
            }
        }

        algo_lib::Regx regx;
        Regx_ReadSql(regx,ssimreq.value,true);

        // check that every subtype record has a corresponding supertype record
        ind_beg(acr::ctype_zd_selrec_curs, rec, *sub) {
            if (acr::FRec *parentrec=ind_rec_Find(*super,rec.pkey)) {
                tempstr value(acr::EvalAttr(parentrec->tuple, *ssimreq.p_field));
                if (!Regx_Match(regx,value)) {
                    NoteErr(NULL,&rec,ssimreq.p_field,tempstr()<<"acr.ssimreq"
                            <<Keyval("comment",tempstr()<<ssimreq.ssimfile
                                     <<" requires that "<<super->c_ssimfile->ssimfile<<"."<<name_Get(*ssimreq.p_field)
                                     <<" have value "<<ssimreq.value<<" (found:"<<value<<")"));
                }
            }
        }ind_end;

        // check that every supertype record has a corresponding subtype record
        if (ssimreq.bidir) {
            ind_beg(acr::ctype_zd_selrec_curs, rec, *super) {
                tempstr value(acr::EvalAttr(rec.tuple, *ssimreq.p_field));
                if (Regx_Match(regx,value) && !ind_rec_Find(*sub,rec.pkey)) {
                    NoteErr(NULL,&rec,ssimreq.p_field,tempstr()<<"acr.ssimreq"
                            <<Keyval("rec",tempstr()<<super->ctype<<":"<<rec.pkey)
                            <<Keyval("value",tempstr()<<name_Get(*ssimreq.p_field)<<":"<<value)
                            <<Keyval("comment",tempstr()<<"This value requires a corresponding entry in "<<ssimreq.ssimfile));
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CheckPkey() {
    ind_beg(acr::_db_zd_sel_ctype_curs, ctype, acr::_db) if (ctype.c_ssimfile) {
        ind_beg(acr::ctype_c_field_curs,  field, ctype){
            if (field.reftype == dmmeta_Reftype_reftype_Val && field.p_arg->c_ssimfile) {
                NoteErr(NULL, NULL, &field, tempstr()<<"Relational"
                        <<Keyval("field",field.field)
                        <<" of ctype:"<<ctype.ctype
                        <<" includes a relation of ctype:"<<field.arg
                        <<" by value.");
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void ShowErrRecs() {
    int nshow=0;
    ind_beg(acr::_db_zd_all_err_curs, err,acr::_db) {
        tempstr msg;
        if (err.rec) {
            msg << err.rec->p_infile->file << ":" << err.rec->lineno << ": ";
        }
        msg << err.text;
        if (err.rec) {
            msg << eol << err.rec->tuple;
        }
        msg << eol;
        if (nshow < acr::_db.cmdline.maxshow) {
            prerr(msg);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void acr::Main_Check() {
    acr::FCheck check;
    CheckPkey();

    CheckArgs(check);

    // Uniqueness check for secondary fields
    // Since ACR is record-oriented, this check is done using a separate hash.
    CheckFunique();

    // X-reference -- check one field at a time
    CheckXrefs(check);

    // check constraints specified in ssimreq
    CheckSsimreq();

    // select only bad records
    SelectErrRecs();
    ShowErrRecs();

    if (acr::_db.cmdline.report) {
        prlog(report::acr_check(check.n_record, acr::zd_all_err_N()));
    }
    if (acr::zd_all_err_N()) {
        acr::_db.check_failed = true;
        algo_lib::_db.exit_code = 1;
    }
}
