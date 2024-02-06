// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/acr/run.cpp -- Run query
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

static void MarkModified(acr::FRun &run, acr::FRec &rec) {
    if (!rec.mod) {
        run.n_mod_rec++;
        rec.mod=true;
    }
}

// -----------------------------------------------------------------------------

// Primary key of record REC has changed from OLD_VAL to NEW_VAL.
// Create a query that visits any record that refers to REC, and renames the referring
// attribute to NEW_VAL as well.
// If dmmeta.field record changes, then
//   1. Schedule rename of appropriate column in the affected ssimfile
//   2. Rename acr's field record (update schema)
static void ScheduleCascadeUpdate(acr::FRec& rec, strptr old_val, strptr new_val) {
    ind_beg(acr::ctype_c_child_curs, child, *rec.p_ctype) if (child.c_ssimfile) {
        ind_beg(acr::ctype_c_field_curs, childfld, child) {
            if (childfld.p_arg == rec.p_ctype) {
                acr::FQuery& next  = acr::query_Alloc();
                next.queryop = acr_Queryop_value_set_attr;
                Regx_ReadLiteral(next.ssimfile, child.c_ssimfile->ssimfile);
                Regx_ReadLiteral(next.query.name, name_Get(childfld));
                Regx_ReadLiteral(next.query.value, old_val);
                next.new_val = new_val;
                next.comment << "cascade update of "<<childfld.field<<" triggered by "<<rec.pkey;
                (void)acr::query_XrefMaybe(next);
            }
        }ind_end;
    }ind_end;

    if (rec.p_ctype->ctype == "dmmeta.Field") {
        acr::FField *oldfield = acr::ind_field_Find(old_val);
        acr::FCtype *ctype = acr::ind_ctype_Find(StripExt(new_val));
        // substr fields don't have any presence in ssimfiles so there is nothing to rename
        if ((!oldfield || !oldfield->isfldfunc) && ctype && ctype->c_ssimfile) {
            acr::FQuery& next  = acr::query_Alloc();
            Regx_ReadLiteral(next.ssimfile, ctype->c_ssimfile->ssimfile);
            next.queryop      = acr_Queryop_value_rename_attr;
            Regx_ReadLiteral(next.query.name, StripNs("",old_val));
            Regx_ReadAcr(next.query.value, "%", true);
            next.new_val      = StripNs("",new_val);
            next.comment << "ssimfile update triggered by "<<rec.pkey<<" (Field)";
            (void)acr::query_XrefMaybe(next);
        }
        // finish renaming the field (reindex)
        {
            acr::FQuery& next  = acr::query_Alloc();
            next.queryop      = acr_Queryop_value_finish_rename_field;
            Regx_ReadLiteral(next.query.name, old_val);
            next.new_val      = new_val;
            next.comment << "ssimfile field rename update";
            (void)acr::query_XrefMaybe(next);
        }
    }
}

// -----------------------------------------------------------------------------

// Check that ALL args in query (QUERY.ARGS)
// have matching name:value attributes in REC, which is of type CTYPE.
// Return TRUE if all fields match
static bool MatchWhere(acr::FQuery &query, acr::FRec &rec, acr::FCtype &ctype) {
    bool argmatch=true;
    ind_beg(acr::query_where_curs,where,query) {
        ind_beg(acr::ctype_c_field_curs,otherfield,ctype) {
            if (Regx_Match(where.name,name_Get(otherfield))) {
                acr::FEvalattr otherattr;
                otherattr.field = &otherfield;
                Evalattr_Step(otherattr, rec.tuple);
                argmatch = Regx_Match(where.value, otherattr.value);
                if (!argmatch) {
                    break;
                }
            }
        }ind_end;
        if (!argmatch) {
            break;
        }
    }ind_end;
    return argmatch;
}

// -----------------------------------------------------------------------------

// Check whether attribute of record REC described by FIELD matches VALUE_REGX
// If it does, perform action as determined by the query
// - del_attr: remove attribute from tuple
// - rename_attr: set name of attribute to QUERY.NEW_VAL
// - set_attr: set value of attribute to QUERY.NEW_VAL
//     Schedule cascading update of related records
// - select: select parent record
// Return TRUE if the action was performed
static bool VisitField(acr::FRun &run, acr::FQuery& query, acr::FRec& rec, acr::FField &field, algo_lib::Regx &value_regx) {
    run.n_visit_field++;
    acr::FEvalattr evalattr;
    evalattr.field = &field;
    Evalattr_Step(evalattr, rec.tuple);
    run.n_regx_match++;
    bool match = Regx_Match(value_regx, evalattr.value) && MatchWhere(query,rec,*field.p_ctype);
    if (match) {
        switch(value_GetEnum(query.queryop)) {
        case acr_Queryop_value_del_attr: {
            if (evalattr.attr) {
                MarkModified(run,rec);
                attrs_Remove(rec.tuple, attrs_rowid_Get(rec.tuple, *evalattr.attr));
            }
        } break;

        case acr_Queryop_value_rename_attr: {
            // evalattr for a substr field will find the field that encloses the substring
            // expression. when renaming the attribute, there is nothing to do
            if (evalattr.attr && evalattr.attr->name == name_Get(field)) {
                MarkModified(run,rec);
                evalattr.attr->name = query.new_val;
                acr::UpdateSortkey(rec,rec.sortkey.rowid);
            }
        } break;

        case acr_Queryop_value_set_attr: {
            if (evalattr.attr) {
                MarkModified(run,rec);
                tempstr newval;
                newval << ch_FirstN(evalattr.attr->value, evalattr.val_range.beg);// left part
                newval << query.new_val;//insert new value
                newval << ch_RestFrom(evalattr.attr->value, evalattr.val_range.end);// right part (substring)
                if (evalattr.attr == &attrs_qFind(rec.tuple, 0)) {// pkey changed? propagate in all children
                    ScheduleCascadeUpdate(rec, evalattr.attr->value, newval);
                }
                evalattr.attr->value = newval;
                acr::UpdateSortkey(rec,rec.sortkey.rowid);//update sort key
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
    return match;
}

// -----------------------------------------------------------------------------

// extend selected front down
// the search starts with all selected records, where we clear the visit flag
// we then create a list RUN.C_CHILD of all potential ctypes that might reference these selected records,
// we then scan records for these ctypes, and add new records to the selected list
// The function returns the number of records added.
// The function performs one iteration of the downward transitive closure. Looping until
// SelectDown returns 0 finds all downward references.
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
        LoadRecords(child);
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
                LoadRecords(*field.p_arg);
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

// Visit all selected ctypes in RUN, scan all records
// of each ctype and match QUERY against each record.
// The record matches if QUERY's NAME:VALUE pair matches at least one field
// The field values are determined from the  attributes of each record
// (This may require computing substrings of attrs)
static void VisitRecords(acr::FRun &run, acr::FQuery &query) {
    ind_beg(acr::run_c_ctype_curs, ctype, run) if (c_field_N(ctype) > 0) {
        run.n_visit_ctype++;
        // match on pkey if the query field is omitted
        bool is_pkey = !ch_N(query.query.name.expr)
            || (query.query.name.literal && query.query.name.expr == name_Get(*c_field_Find(ctype, 0)));

        // determine set of fields to scan
        // if query is for pkey, we already have that indexed.
        c_field_RemoveAll(run);
        if (is_pkey) {
            c_field_Insert(run, *c_field_Find(ctype, 0));
        } else {
            ind_beg(acr::ctype_c_field_curs, field, ctype) {
                run.n_regx_match++;
                if (Regx_Match(query.query.name, name_Get(field))) {
                    c_field_Insert(run, field);
                }
            }ind_end;
        }

        // determine set of records to scan
        c_rec_RemoveAll(run);
        if (is_pkey && query.query.value.literal) {
            acr::FRec *rec = acr::ind_rec_Find(ctype, query.query.value.expr);
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
                VisitField(run,query,rec,field,query.query.value);
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void SelectCtype(acr::FRun &run, acr::FCtype &ctype) {
    acr::c_ctype_Insert(run,ctype);
    acr::zd_sel_ctype_Insert(ctype);
}

// -----------------------------------------------------------------------------

// Build list run.c_ctype by calculating which ctypes can possibly match the query
static void SelectCtypes(acr::FRun &run, acr::FQuery &query) {
    if (!query.ssimfile.literal) {
        ind_beg(acr::_db_ssimfile_curs, ssimfile,acr::_db) {
            run.n_regx_match++;
            verblog("# match ssimfile "<<ssimfile.ssimfile<<" against "<<query.ssimfile.expr);
            bool match = Regx_Match(query.ssimfile, ssimfile.ssimfile);
            if (match) {
                SelectCtype(run, *ssimfile.p_ctype);
            }
        }ind_end;
    } else {
        acr::FSsimfile *ssimfile = acr::ind_ssimfile_Find(query.ssimfile.expr);
        if (ssimfile) {
            SelectCtype(run, *ssimfile->p_ctype);
        }
    }
}

// -----------------------------------------------------------------------------

static void SelectUpDown(acr::FRun &run, acr::FQuery &query) {
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

// Visit all records matching QUERY.
// Perform actions specified bu QUERY -- rename, delete, select, etc.
// For selection:
//   follow up and down cross-reference links across all types of ssimfiles.
//   Matching records are added to zd_all_selrec index;
//   Matching ssimfiles are added to db.c_sel_ctype index.
void acr::RunQuery(acr::FQuery &query) {
    verblog("#"<<query);
    if (query.queryop == acr_Queryop_value_finish_rename_field) {
        // field rename is done separately since it affects acr's own schema
        if (acr::FField *field = acr::ind_field_Find(query.query.name.expr)) {
            ind_field_Remove(*field);
            field->field = query.new_val;
            ind_field_InsertMaybe(*field);
        }
    } else {
        acr::FRun run;
        // compute list of potential ssimfiles that contain matches
        SelectCtypes(run,query);
        // load ssimfiles (if necessary)
        ind_beg(acr::run_c_ctype_curs, ctype, run) {
            LoadRecords(ctype);
        }ind_end;
        VisitRecords(run,query);
        SelectUpDown(run,query);
        c_child_RemoveAll(run);
        if (query.selmeta) {
            SelectMeta();
        }
        verblog("#"<<run);
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
