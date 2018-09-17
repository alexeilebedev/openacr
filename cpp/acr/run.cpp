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
// Source: cpp/acr/run.cpp -- Run query
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

// Primary key of record REC has changed from OLD_VAL to NEW_VAL.
// Create a query that visits any record that refers to REC, and renames the referring
// attribute to NEW_VAL as well.
// If dmmeta.field record changes, then
//   1. Schedule rename of appropriate column in the affected ssimfile
//   2. Rename acr's field record (update schema)
static void ScheduleCascadeUpdate(acr::FRec& rec, strptr old_val, strptr new_val) {
    ind_beg(acr::ctype_c_child_curs,  child, *rec.p_ctype) {
        ind_beg(acr::ctype_c_field_curs, childfld, child) {
            if (childfld.p_arg == rec.p_ctype) {
                acr::FQuery& next  = acr::query_Alloc();
                next.queryop = acr_Queryop_value_set_attr;
                next.ctype   = child.ctype;
                next.field   = name_Get(childfld);
                next.value   = old_val;
                next.new_val = new_val;
                (void)acr::query_XrefMaybe(next);
            }
        }ind_end;
    }ind_end;

    if (rec.p_ctype->ctype == "dmmeta.Field") {
        acr::FCtype *ctype = acr::ind_ctype_Find(StripExt(new_val));
        if (ctype) {
            acr::FQuery& next  = acr::query_Alloc();
            next.ctype        = ctype->ctype;
            next.queryop      = acr_Queryop_value_rename_attr;
            next.field        = StripNs("",old_val);
            next.value        = "%";
            next.new_val      = StripNs("",new_val);
            (void)acr::query_XrefMaybe(next);
        }
        // finish renaming the field (reindex)
        {
            acr::FQuery& next  = acr::query_Alloc();
            next.queryop      = acr_Queryop_value_finish_rename_field;
            next.field        = old_val;
            next.new_val      = new_val;
            (void)acr::query_XrefMaybe(next);
        }
    }
}

// -----------------------------------------------------------------------------

static void VisitField(acr::FQuery& query, acr::FRec& rec, acr::FField &field, algo_lib::Regx &value_regx) {
    acr::FEvalattr evalattr;
    evalattr.field = &field;
    Evalattr_Step(evalattr, rec.tuple);
    bool         match  = Regx_Match(value_regx, query.pk?rec.pkey:evalattr.value);
    if (match) {
        Attr   *attr      = attr_Find(rec.tuple, name_Get(field), 0);
        switch(value_GetEnum(query.queryop)) {
        case acr_Queryop_value_del_attr: {
            if (attr) {
                acr::_db.report.n_update++;
                rec.mod = true;
                attrs_Remove(rec.tuple, attrs_rowid_Get(rec.tuple, *attr));
            }
        } break;

        case acr_Queryop_value_rename_attr: {
            if (attr) {
                acr::_db.report.n_update++;
                rec.mod = true;
                attr->name = query.new_val;
            }
        } break;

        case acr_Queryop_value_set_attr: {
            acr::_db.report.n_update++;
            rec.mod = true;
            if (evalattr.attr) {
                tempstr newval;
                newval << ch_FirstN(evalattr.attr->value, evalattr.val_range.beg);// left part
                newval << query.new_val;//insert new value
                newval << ch_RestFrom(evalattr.attr->value, evalattr.val_range.end);// right part (substring)
                if (evalattr.attr == &attrs_qFind(rec.tuple, 0)) {// pkey changed? propagate in all children
                    ScheduleCascadeUpdate(rec, evalattr.attr->value, newval);
                }
                evalattr.attr->value = newval;
            }
        } break;

        case acr_Queryop_value_select: {
            Rec_Select(rec);
        } break;

        default: {
            // no action
        } break;

        }
    }
}

// -----------------------------------------------------------------------------

// extend front down
// if the select record is a pkey, and it is being referenced by another pkey,
// we can select the source record with a direct lookup.
// if the reference is from a non-indexed column, we must scan the ssimfile.
// we currently always scan the entrie ssimfile.
static int SelectDown(acr::FRun &run, acr::FQuery &query) {
    int nmatch=0;
    // find the set of all ssimfiles which may reference records selected so far.
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        rec.p_ctype->visit=false;
    }ind_end;
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        if (rec.seldist <= 0 && -rec.seldist < query.ndown && bool_Update(rec.p_ctype->visit,true)) {
            ind_beg(acr::ctype_c_field_curs,  field, *rec.p_ctype) {
                ind_beg(acr::ctype_c_child_curs, child, *field.p_ctype) {
                    c_child_Insert(run, child);
                }ind_end;
            }ind_end;
        }
    }ind_end;
    // walk child ssimfiles
    // walk all records of each ssimfile
    // add records which reference one of selected records
    ind_beg(acr::run_c_child_curs, child, run) {
        if (child.c_ssimfile && !child.c_ssimfile->c_file) {
            acr::LoadSsimfile(*child.c_ssimfile);
        }
        ind_beg(acr::ctype_zd_trec_curs, rec, child) {
            ind_beg(acr::ctype_c_field_curs,  field, child) if (field.p_arg->c_ssimfile) {
                tempstr val(EvalAttr(rec.tuple, field));
                acr::FRec *parrec = acr::ind_rec_Find(*field.p_arg, val);
                bool good = parrec;
                good = good && parrec->seldist <= 0;
                good = good && acr::zd_all_selrec_InLlistQ(*parrec);
                if (query.unused) {
                    // "unused" option -- instead of selecting child,
                    // deselect parent. this effectively selects only those records
                    // which have no one pointing to them.
                    // cmdline.nup=0, cmdline.ndown=1, otherwise this algorithm doesn't
                    // work.
                    if (good) {
                        zd_all_selrec_Remove(*parrec);
                        zd_selrec_Remove(*parrec->p_ctype, *parrec);
                    }
                } else {
                    good = good && Rec_Select(rec);
                    if (good) {
                        rec.seldist = parrec->seldist - 1;
                        nmatch++;
                        break; // already selected, no need to continue search
                    }
                }
            }ind_end;
        }ind_end;
    }ind_end;
    return nmatch;
}

// -----------------------------------------------------------------------------

static void SelectUp(acr::FRun &, acr::FQuery &query) {
    // run through selected records, add new ones to the front
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        if (rec.seldist >= 0 && rec.seldist < query.nup) {
            ind_beg(acr::ctype_c_field_curs,  field, *rec.p_ctype) if (field.p_arg->c_ssimfile) {
                // read ssimfile if necessary
                acr::FSsimfile *rel = field.p_arg->c_ssimfile;
                if (!rel->c_file) {
                    acr::LoadSsimfile(*rel);
                }
                // look up item in the parent record.
                // if found -- add that record to the match set
                tempstr val(EvalAttr(rec.tuple, field));
                acr::FRec *parrec = acr::ind_rec_Find(*field.p_arg, val);
                if (parrec && Rec_Select(*parrec)) {
                    parrec->seldist = rec.seldist + 1;
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void VisitRecords(acr::FRun &run, acr::FQuery &query) {
    ind_beg(acr::run_c_ctype_curs, ctype, run) if (c_field_N(ctype) > 0) {
        // specifying an
        bool is_pkey = !ch_N(query.field);
        is_pkey |= query.field == name_Get(*c_field_Find(ctype, 0));

        // determine set of fields to scan
        // if query is for pkey, we already have that indexed.
        c_field_RemoveAll(run);
        if (is_pkey) {
            c_field_Insert(run, *c_field_Find(ctype, 0));
        } else {
            ind_beg(acr::ctype_c_field_curs, field, ctype) {
                if (Regx_Match(run.field_regx, name_Get(field))) {
                    c_field_Insert(run, field);
                }
            }ind_end;
        }

        // determine set of records to scan
        c_rec_RemoveAll(run);
        if (is_pkey && !algo_lib::SqlRegxQ(query.value)) {
            acr::FRec *rec = acr::ind_rec_Find(ctype, query.value);
            if (rec) {
                c_rec_Insert(run, *rec);
            }
        } else {
            ind_beg(acr::ctype_zd_trec_curs, rec, ctype) {
                c_rec_Insert(run, rec);
            }ind_end;
        }

        // scan records
        ind_beg(acr::run_c_rec_curs, rec, run) {
            ind_beg(acr::run_c_field_curs, field, run) {
                VisitField(query,rec,field,run.value_regx);
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void SelectCtypes(acr::FRun &run, acr::FQuery &query) {
    if (algo_lib::SqlRegxQ(query.regx_ssimfile)) {
        algo_lib::Regx ssimfile_regx;
        Regx_ReadSql(ssimfile_regx, query.regx_ssimfile, true);
        ind_beg(acr::_db_ssimfile_curs, ssimfile,acr::_db) {
            bool match = Regx_Match(ssimfile_regx, ssimfile.ssimfile);
            if (match) {
                c_ctype_Insert(run, *ssimfile.p_ctype);
            }
        }ind_end;
    } else {
        acr::FSsimfile *ssimfile = acr::ind_ssimfile_Find(query.regx_ssimfile);
        if (ssimfile) {
            c_ctype_Insert(run, *ssimfile->p_ctype);
        }
    }

    // compute list of potential ctypes that contain matches
    if (algo_lib::SqlRegxQ(query.ctype)) {
        algo_lib::Regx ctype_regx;
        Regx_ReadSql(ctype_regx, query.ctype, true);
        ind_beg(acr::_db_ctype_curs, ctype,acr::_db) {
            bool match = Regx_Match(ctype_regx, ctype.ctype);
            if (match) {
                c_ctype_Insert(run, ctype);
            }
        }ind_end;
    } else {
        acr::FCtype *ctype = acr::ind_ctype_Find(query.ctype);
        if (ctype) {
            c_ctype_Insert(run, *ctype);
        }
    }
}

// -----------------------------------------------------------------------------

static void LoadSsimfiles(acr::FRun &run) {
    if (!acr::FileInputQ()) {
        ind_beg(acr::run_c_ctype_curs, ctype, run) {
            if (ctype.c_ssimfile && !ctype.c_ssimfile->c_file) {
                acr::LoadSsimfile(*ctype.c_ssimfile);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static void Xref(acr::FRun &run, acr::FQuery &query) {
    if (query.nup > 0) {
        SelectUp(run,query);
    }

    frep_(iter, query.ndown) {
        c_child_RemoveAll(run);
        int nmatch = SelectDown(run,query);
        if (nmatch==0) {
            break;
        }
    }
}

// -----------------------------------------------------------------------------

static void MarkDelete(acr::FQuery &query) {
    if (query.delrec) {
        ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
            rec.del = true;
            acr::_db.report.n_delete++;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Visit all records matching QUERY.
// Perform actions specified bu QUERY -- rename, delete, select, etc.
// For selection:
//   follow up and down cross-reference links across all types of ssimfiles.
//   Matching records are added to zd_all_selrec index;
//   Matching ssimfiles are added to db.c_sel_ctype index.
void acr::RunQuery(acr::FQuery &query) {
    if (query.queryop == acr_Queryop_value_finish_rename_field) {
        if (acr::FField *field = acr::ind_field_Find(query.field)) {
            ind_field_Remove(*field);
            field->field = query.new_val;
            ind_field_InsertMaybe(*field);
        }
    } else {
        acr::FRun run;
        // compute list of potential ssimfiles that contain matches
        SelectCtypes(run,query);
        // load ssimfiles (if necessary)
        LoadSsimfiles(run);
        // determine fields and records to scan
        Regx_ReadSql(run.field_regx  , query.field, true);
        Regx_ReadSql(run.value_regx  , query.value, true);
        VisitRecords(run,query);
        // next step -- x-ref the thing
        Xref(run,query);
        MarkDelete(query);
        c_child_RemoveAll(run);
    }
}

// -----------------------------------------------------------------------------

void acr::RunAllQueries() {
    // run all queries
    // pop next item off of todo list.
    // running one query may create additional queries.
    while (acr::FQuery *query=acr::zs_query_First()) {
        RunQuery(*query);
        acr::query_Delete(*query);
    }
}
