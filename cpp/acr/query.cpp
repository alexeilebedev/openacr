// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/acr/query.cpp -- Run query
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

static void MarkModified(acr::FQuery &query, acr::FRec &rec) {
    if (!rec.mod) {
        query.n_mod_rec++;
        rec.mod=true;
    }
}

// -----------------------------------------------------------------------------

// Primary key of record REC is about to change from OLD_VAL to NEW_VAL.
// Create a query that visits any record that refers to REC, and renames the referring
// attribute to NEW_VAL as well.
// If dmmeta.field record changes, then
//   1. Schedule rename of appropriate column in the affected ssimfile
//   2. Rename acr's field record (update schema)
static void ScheduleCascadeUpdate(acr::FRec& rec, strptr old_val, strptr new_val) {
    // schedule value change for any fields that refers to this record
    ind_beg(acr::ctype_c_child_curs, child, *rec.p_ctype) if (child.c_ssimfile) {
        LoadRecords(child);
        ind_beg(acr::ctype_c_field_curs, childfld, child) {
            if (childfld.p_arg == rec.p_ctype && childfld.reftype == dmmeta_Reftype_reftype_Pkey) {
                acr::FQuery& next  = acr::query_Alloc();
                next.queryop = acr_Queryop_value_set_attr;
                Regx_ReadLiteral(next.ssimfile, child.c_ssimfile->ssimfile);
                Regx_ReadLiteral(next.query.name, name_Get(childfld));
                Regx_ReadLiteral(next.query.value, old_val);
                next.new_val = new_val;
                next.comment << "cascade update"
                             <<Keyval("ssimfile",child.c_ssimfile->ssimfile)
                             <<Keyval("from",old_val)
                             <<Keyval("to",new_val)
                             <<Keyval("triggered_by",tempstr()<<rec.p_ctype->c_ssimfile->ssimfile<<":"<<rec.pkey);
                (void)acr::query_XrefMaybe(next);
            }
        }ind_end;
    }ind_end;
    // schedule rename of any record implied by ssimreq
    if (acr::_db.cmdline.x) {
        ind_beg(acr::ctype_c_ssimreq_curs,ssimreq,*rec.p_ctype) {
            acr::FField *pkey_field=c_field_Find(*rec.p_ctype,0);
            if (ssimreq.p_parent_field == pkey_field && Regx_Match(ssimreq.regx_value,old_val)) {
                verblog("ssimreq parentfld "<<pkey_field->field<<" old val "<<old_val<<" regx "<<ssimreq.regx_value.expr<<" match");
                algo_lib::Replscope R;
                R.eatcomma=false;
                acr::FCtype *child = ssimreq.p_child_ssimfile->p_ctype;
                LoadRecords(*child);
                tempstr old_child_key = acr::GetChildKey(rec,ssimreq,rec.tuple,R);
                if (acr::ind_ctype_rec_Find(*child,old_child_key)) {
                    algo::Tuple newtuple;
                    attrs_Addary(newtuple,attrs_Getary(rec.tuple));
                    attrs_Find(newtuple,0)->value=new_val;
                    tempstr new_child_key = GetChildKey(rec,ssimreq,newtuple,R);
                    acr::FQuery& next  = acr::query_Alloc();
                    next.queryop = acr_Queryop_value_set_attr;
                    Regx_ReadLiteral(next.ssimfile, ssimreq.p_child_ssimfile->ssimfile);
                    // next.query.name empty   ->  pkey
                    Regx_ReadLiteral(next.query.value, old_child_key);
                    next.new_val = new_child_key;
                    next.comment << "cascade update"
                                 <<Keyval("ssimfile",ssimreq.p_child_ssimfile->ssimfile)
                                 <<Keyval("from",old_child_key)
                                 <<Keyval("to",new_child_key)
                                 <<Keyval("triggered_by",tempstr()<<rec.p_ctype->c_ssimfile->ssimfile<<":"<<rec.pkey);
                    (void)acr::query_XrefMaybe(next);
                }
            }
        }ind_end;
    }
    if (rec.p_ctype == acr::_db.c_field_ctype) {
        acr::FField *oldfield = acr::ind_field_Find(old_val);
        acr::FCtype *newctype = acr::ind_ctype_Find(StripExt(new_val));
        // substr fields don't have any presence in ssimfiles so there is nothing to rename
        if ((!oldfield || !oldfield->isfldfunc) && newctype && newctype->c_ssimfile) {
            acr::FQuery& next  = acr::query_Alloc();
            Regx_ReadLiteral(next.ssimfile, newctype->c_ssimfile->ssimfile);
            next.queryop      = acr_Queryop_value_rename_attr;
            Regx_ReadLiteral(next.query.name, name_Get(*oldfield));
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
    } else if (rec.p_ctype == acr::_db.c_ssimfile_ctype) {
        // rewrite type tag in ssimfile
        // e.g.
        // if ssimfile contents was
        // dev.target target:X
        // and we renamed ssimfile dev.target -> dev.xyz
        // then we need to update all tuples to have typetag dev.xyz
        // the ssimfile will be saved to the original filename.
        acr::FQuery& next  = acr::query_Alloc();
        Regx_ReadLiteral(next.ssimfile, old_val);
        next.queryop     = acr_Queryop_value_rename_typetag;
        //next.query.name -- leave empty -- matches pkey
        Regx_ReadAcr(next.query.value, "%", true);
        next.new_val      = new_val;
        next.comment << "rewrite ssimfile typetags";
        (void)acr::query_XrefMaybe(next);

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
// - rename_typetag: rewrite typetag of the tuple
// - set_attr: set value of attribute to QUERY.NEW_VAL
//     Schedule cascading update of related records
// - select: select parent record
// Return TRUE if the action was performed
static bool VisitField(acr::FQuery& query, acr::FRec& rec, acr::FField &field, algo_lib::Regx &value_regx) {
    query.n_visit_field++;
    // we may be renaming a substring; Evalattr determines the range of characters corresponding to the match,
    acr::FEvalattr evalattr;
    evalattr.field = &field;
    Evalattr_Step(evalattr, rec.tuple);
    query.n_regx_match++;
    bool match = Regx_Match(value_regx, evalattr.value) && MatchWhere(query,rec,*field.p_ctype);
    if (match) {
        switch(value_GetEnum(query.queryop)) {
        case acr_Queryop_value_del_attr: {
            if (evalattr.attr) {
                MarkModified(query,rec);
                attrs_Remove(rec.tuple, attrs_rowid_Get(rec.tuple, *evalattr.attr));
            }
        } break;

        case acr_Queryop_value_rename_typetag: {
            rec.tuple.head.value = query.new_val;
            MarkModified(query,rec);
        } break;

        case acr_Queryop_value_rename_attr: {
            // evalattr for a substr field will find the field that encloses the substring
            // expression. when renaming the attribute, there is nothing to do
            if (evalattr.attr && evalattr.attr->name == name_Get(field)) {
                MarkModified(query,rec);
                evalattr.attr->name = query.new_val;
                acr::UpdateSortkey(rec,rec.sortkey.rowid);
            }
        } break;

        case acr_Queryop_value_set_attr: {
            if (evalattr.attr) {
                MarkModified(query,rec);
                tempstr newval;
                newval << ch_FirstN(evalattr.attr->value, evalattr.val_range.beg);// left part
                newval << query.new_val;//insert new value
                newval << ch_RestFrom(evalattr.attr->value, evalattr.val_range.end);// right part (substring)
                if (evalattr.value != newval) {
                    bool ispkey = evalattr.attr == &attrs_qFind(rec.tuple, 0);
                    bool collision = ispkey && ind_ctype_rec_Find(*field.p_ctype, newval);
                    if (ispkey) {// pkey changed? propagate in all children
                        ScheduleCascadeUpdate(rec, evalattr.attr->value, newval);
                        if (!rec.oldpkey) {
                            oldpkey_Access(rec) = evalattr.attr->value;// save old pkey after rename, but only once
                        }
                        ind_ctype_rec_Remove(*field.p_ctype,rec);// deindex record, pkey changed
                    }
                    if (collision) {
                        rec.del=true;// delete old record
                    } else {
                        evalattr.attr->value = newval;// update attribute value
                        if (ispkey) {
                            rec.pkey = newval;// update pkey and reindex record, this will succeed
                            acr::ind_ctype_rec_InsertMaybe(*field.p_ctype,rec);
                        }
                    }
                }
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

// Visit all records matching QUERY.
// Perform actions specified bu QUERY -- rename, delete, select, etc.
// For selection:
//   follow up and down cross-reference links across all types of ssimfiles.
//   Matching records are added to zd_all_selrec index;
//   Matching ssimfiles are added to db.c_sel_ctype index.
void acr::RunQuery(acr::FQuery &query) {
    verblog("acr.runquery  "<<query);
    if (query.queryop == acr_Queryop_value_finish_rename_field) {
        // field rename is done separately since it affects acr's own schema
        if (acr::FField *field = acr::ind_field_Find(query.query.name.expr)) {
            ind_field_Remove(*field);
            field->field = query.new_val;
            ind_field_InsertMaybe(*field);
        }
    } else {
        // compute list of potential ssimfiles that contain matches
        if (!query.ssimfile.literal) {
            ind_beg(acr::_db_ssimfile_curs, ssimfile,acr::_db) {
                query.n_regx_match++;
                if (Regx_Match(query.ssimfile, ssimfile.ssimfile)) {
                    acr::c_ctype_Insert(query,*ssimfile.p_ctype);
                }
            }ind_end;
        } else {
            acr::FSsimfile *ssimfile = acr::ind_ssimfile_Find(query.ssimfile.expr);
            if (ssimfile) {
                acr::c_ctype_Insert(query,*ssimfile->p_ctype);
            }
        }
        // load ssimfiles (if necessary)
        ind_beg(acr::query_c_ctype_curs, ctype, query) {
            LoadRecords(ctype);
        }ind_end;
        // Visit all selected ctypes in QUERY, scan all records
        // of each ctype and match QUERY against each record.
        // The record matches if QUERY's NAME:VALUE pair matches at least one field
        // The field values are determined from the  attributes of each record
        // (This may require computing substrings of attrs)
        ind_beg(acr::query_c_ctype_curs, ctype, query) if (c_field_N(ctype) > 0) {
            query.n_visit_ctype++;
            // match on pkey if the query field is omitted
            bool is_pkey = !ch_N(query.query.name.expr)
                || (query.query.name.literal && query.query.name.expr == name_Get(*c_field_Find(ctype, 0)));

            // determine set of fields to scan
            // if query is for pkey, we already have that indexed.
            c_field_RemoveAll(query);
            if (is_pkey) {
                c_field_Insert(query, *c_field_Find(ctype, 0));
            } else {
                ind_beg(acr::ctype_c_field_curs, field, ctype) {
                    query.n_regx_match++;
                    if (Regx_Match(query.query.name, name_Get(field))) {
                        c_field_Insert(query, field);
                    }
                }ind_end;
            }

            // determine set of records to scan
            c_rec_RemoveAll(query);
            if (is_pkey && query.query.value.literal) {
                acr::FRec *rec = acr::ind_ctype_rec_Find(ctype, query.query.value.expr);
                if (rec) {
                    c_rec_Insert(query, *rec);
                }
            } else {
                ind_beg(acr::ctype_zd_ctype_rec_curs, rec, ctype) {
                    c_rec_Insert(query, rec);
                }ind_end;
            }

            // scan records
            ind_beg(acr::query_c_rec_curs, rec, query) {
                ind_beg(acr::query_c_field_curs, field, query) {
                    VisitField(query,rec,field,query.query.value);
                }ind_end;
            }ind_end;
        }ind_end;
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
