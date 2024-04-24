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
// Target: acr_in (exe) -- ACR Input - compute set of ssimfiles or tuples used by a specific target
// Exceptions: yes
// Source: cpp/acr_in/data.cpp
//

#include "include/acr_in.h"

// -----------------------------------------------------------------------------

static tempstr EvalAttr(Tuple &tuple, acr_in::FField &field) {
    tempstr ret;
    acr_in::FSubstr *substr = field.c_substr;
    if (substr) {
        tempstr val(attr_GetString(tuple, StripNs("",substr->srcfield)));
        ret = Pathcomp(val, substr->expr.value);// ((a=Pathcomp(a,expr) would be an error)
    } else {
        ret = attr_GetString(tuple, name_Get(field));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Transitive closure filter
//   - Select all records matching $related regex. Call their ctypes related.
//   - Also mark as related any tables theoretically reachable from related ctypes
//   - Select all records belonging to related ctypes
//   - Select non-related tables in entirety
static void Main_Related() {
    // select all related records
    algo_lib::Regx regx_related;
    Regx_ReadSql(regx_related, acr_in::_db.cmdline.related, true);
    ind_beg(acr_in::_db_tuple_curs, tuple, acr_in::_db) {
        if (algo_lib::Regx_Match(regx_related, tuple.key)) {
            tuple.p_ctype->related = true;
            tuple.p_ctype->select = true;
            acr_in::zd_related_Insert(*tuple.p_ctype);
            acr_in::zd_select_Insert(tuple);
        }
    }ind_end;

    // Build list of related ctypes (all ctypes derived from initial selection)
    for (acr_in::FCtype *ctype = acr_in::zd_related_First(); ctype; ctype = ctype->zd_related_next) {
        ind_beg(acr_in::ctype_c_ctype_curs, child, *ctype) {
            acr_in::zd_related_Insert(child);
            child.select = true;
            child.related = true;
        }ind_end;
    }

    // select any children of related records
    for (acr_in::FTuple *tuple = acr_in::zd_select_First(); tuple; tuple = tuple->zd_select_next) {
        ind_beg(acr_in::tuple_c_child_curs, child, *tuple) {
            child.p_ctype->select = true;
            acr_in::zd_select_Insert(child);
        }ind_end;
    }

    // select all records from tables which weren't marked in this
    // algorithm
    ind_beg(acr_in::_db_tuple_curs, tuple, acr_in::_db) if (tuple.p_ctype->parent_of_finput && !tuple.p_ctype->related) {
        acr_in::zd_select_Insert(tuple);
    }ind_end;
}

// -----------------------------------------------------------------------------

// True if given field is a pkey
static bool SsimfilePkeyQ(acr_in::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Pkey && field.p_arg->c_ssimfile;
}

// -----------------------------------------------------------------------------

static void VisitParents(acr_in::FTuple &tuple, Tuple &in_tuple, acr_in::FSsimfile &ssimfile) {
    ind_beg(acr_in::ctype_c_field_curs, field, *ssimfile.p_ctype) if (SsimfilePkeyQ(field)) {
        tempstr key;
        key << field.p_arg->c_ssimfile->ssimfile<<":"<<EvalAttr(in_tuple,field);
        // if parent record is missing, must not fail.
        if (acr_in::FTuple *parent_tuple = acr_in::ind_tuple_Find(key)) {
            acr_in::c_parent_Insert(tuple,*parent_tuple); // Parents
            acr_in::c_child_Insert(*parent_tuple,tuple); // Children
        } else {
            prerr("acr_in.reference_error"
                  <<Keyval("child_key",tuple.key)
                  <<Keyval("field",field.field)
                  <<Keyval("parent_key",key)
                  <<Keyval("comment","Referential integrity check failed"));
        }
    }ind_end;
}

static void LoadTuple(acr_in::FSsimfile &ssimfile, strptr line, bool dag) {
    Tuple in_tuple;
    vrfy(Tuple_ReadStrptrMaybe(in_tuple, line), algo_lib::_db.errtext);
    if (attrs_N(in_tuple)){
        acr_in::FTuple &tuple = acr_in::tuple_Alloc();
        tuple.key           = tempstr()<<in_tuple.head.value<<":"<<attrs_qFind(in_tuple,0).value;
        tuple.str              = line;
        tuple.p_ctype = ssimfile.p_ctype;
        vrfy(tuple_XrefMaybe(tuple), algo_lib::_db.errtext);
        if (dag) {
            VisitParents(tuple,in_tuple,ssimfile);
        }
    }
}

// -----------------------------------------------------------------------------

static void MaybeDeselectChildren(acr_in::FTuple &parent) {
    if (!zd_select_InLlistQ(parent)) {
        ind_beg(acr_in::tuple_c_child_curs, child, parent) if (zd_select_InLlistQ(child)) {
            zd_select_Remove(child);
            MaybeDeselectChildren(child);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Create table acr_in::_db.tuple containing all tuples that should be printed
// for this run
void acr_in::Main_Data() {
    bool dag = ch_N(acr_in::_db.cmdline.related) > 0;

    if (dag) {
        ind_beg(acr_in::_db_zd_ssimfile_curs, ssimfile, acr_in::_db) {
            acr_in::FCtype &ctype = *ssimfile.p_ctype;
            ind_beg(acr_in::ctype_c_field_curs, field, ctype) if (SsimfilePkeyQ(field)) {
                acr_in::c_ctype_Insert(*field.p_arg, ctype); // Children
            }ind_end;
        }ind_end;
    }

    // load all ssimfiles and build database of tuples
    // Build parent/child relationships between data
    ind_beg(acr_in::_db_zd_ssimfile_curs, ssimfile, acr_in::_db) {
        tempstr fname;
        fname << SsimFname(acr_in::_db.cmdline.data_dir, ssimfile.ssimfile);
        algo_lib::MmapFile fmap;
        algo_lib::MmapFile_Load(fmap, fname);
        ind_beg(algo::Line_curs, line, fmap.text) {        // Insert tuples
            LoadTuple(ssimfile,line,dag);
        }ind_end;
    }ind_end;

    // Perform transitive closure on data using selected algorithm
    if (ch_N(acr_in::_db.cmdline.related) > 0) {
        Main_Related();
    } else {
        // just select all records
        ind_beg(acr_in::_db_tuple_curs, tuple, acr_in::_db) {
            zd_select_Insert(tuple);
        }ind_end;
    }

    // deselect children of deselected parents
    // this must be done iteratively
    if (dag) {
        ind_beg(acr_in::_db_tuple_curs, tuple, acr_in::_db) {
            MaybeDeselectChildren(tuple);
        }ind_end;
    }
}
