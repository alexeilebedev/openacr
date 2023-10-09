// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: acr_in (exe) -- ACR Input - compute set of ssimfiles or tuples used by a specific target
// Exceptions: yes
// Source: cpp/acr/acr_in.cpp
//
// Print ssim tuples desired by a specified process,
// or print names of ssim files

#include "include/algo.h"
#include "include/gen/acr_in_gen.h"
#include "include/gen/acr_in_gen.inl.h"

// Select namespaces
static void Main_Ns() {
    // Select initial set of namespaces
    ind_beg(acr_in::_db_ns_curs, ns, acr_in::_db) {
        ns.select = Regx_Match(acr_in::_db.cmdline.ns, ns.ns);
        if (ns.select && ns.c_target) {
            zd_targ_visit_Insert(*ns.c_target);
        }
    }ind_end;

    // Extend namespace selection to include all dependent targets
    for (acr_in::FTarget *target = acr_in::zd_targ_visit_First(); target; target=target->zd_targ_visit_next) {
        ind_beg(acr_in::target_c_targdep_curs,targdep,*target) {
            acr_in::FTarget *parent = targdep.p_parent;
            if ((parent->p_ns->nstype == dmmeta_Nstype_nstype_exe) || (parent->p_ns->nstype == dmmeta_Nstype_nstype_lib)) {
                zd_targ_visit_Insert(*parent);
            }
        }ind_end;
    }

    // Select namespaces matching selected targets
    ind_beg(acr_in::_db_target_curs, target, acr_in::_db) if (zd_targ_visit_InLlistQ(target)) {
        acr_in::FNs *ns = acr_in::ind_ns_Find(target.target);
        if (ns && !ns->select) {
            verblog("acr_in.ns  select:"<<ns->ns);
            ns->select = true;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------
// Compute ctypes which are finputs

static acr_in::FCtype *Basetype(acr_in::FCtype &ctype) {
    acr_in::FCtype *retval = &ctype;
    ind_beg(acr_in::ctype_c_field_curs,field,ctype) if (field.reftype == dmmeta_Reftype_reftype_Base) {
        retval = field.p_arg;
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

static bool SelectCtypeQ(acr_in::FCtype *ctype) {
    // checkable: include all parents of selected ssimfiles
    // non-checkable: include only actual inputs
    bool follow_parent=acr_in::_db.cmdline.checkable
        || ch_N(acr_in::_db.cmdline.related)>0;
    return ctype->c_ssimfile
        && (follow_parent || ctype->c_ssimfile->is_finput)
        && !Regx_Match(acr_in::_db.cmdline.notssimfile, ctype->c_ssimfile->ssimfile);
}

// -----------------------------------------------------------------------------

static void Main_SelectSsimfile() {
    ind_beg(acr_in::_db_finput_curs, finput, acr_in::_db) if (finput.p_ns->select) {
        acr_in::FCtype *ctype = Basetype(*finput.p_field->p_arg);// prefer base
        if (ctype->c_ssimfile) {
            ctype->c_ssimfile->is_finput = true;
        }
        zd_todo_Insert(*ctype);
    }ind_end;
    // topologically sort ctypes that have ssimfiles
    // zd_todo is a stack.
    while (acr_in::FCtype *ctype = acr_in::zd_todo_Last()) {
        if (!ctype->visit) {
            ctype->visit = true;
            ind_beg(acr_in::ctype_c_field_curs, field, *ctype) if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
                acr_in::zd_todo_Remove(*field.p_arg);
                acr_in::zd_todo_Insert(*field.p_arg);
            }ind_end;
        } else {
            if (SelectCtypeQ(ctype)) {
                ctype->parent_of_finput = true;
                acr_in::zd_ssimfile_Insert(*ctype->c_ssimfile);
            }
            acr_in::zd_todo_Remove(*ctype);
        }
    }
}

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
//
// Transitive closure filter
//   - Select all records matching $related regex. Call their ctypes related.
//   - Also mark as related any tables theoretically reachable from related ctypes
//   - Select all records belonging to related ctypes
//   - Select non-related tables in entirety
//
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

static void Main_Data() {
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

// -----------------------------------------------------------------------------

static bool PrintTupleQ(acr_in::FTuple &tuple) {
    bool ret=zd_select_InLlistQ(tuple);
    bool print_all = acr_in::_db.cmdline.checkable;
    if (!print_all) {// restrict
        ret = ret && tuple.p_ctype->c_ssimfile->is_finput;
    }
    return ret;
}

// -----------------------------------------------------------------------------

void acr_in::Main() {
    // pick reasonable default
    if (!acr_in::_db.cmdline.data && !acr_in::_db.cmdline.list) {
        acr_in::_db.cmdline.list = true;
    }

    Main_Ns();

    Main_SelectSsimfile();

    // output dispsigcheck records for all selected namespaces
    if (acr_in::_db.cmdline.sigcheck) {
        ind_beg(acr_in::_db_dispsig_curs, dispsig, acr_in::_db) if (dispsig.p_ns->select) {
            prlog(dmmeta::Dispsigcheck(dispsig.dispsig, dispsig.signature));
        }ind_end;
    }

    // list ssimfiles
    if (acr_in::_db.cmdline.list) {
        ind_beg(acr_in::_db_zd_ssimfile_curs, ssimfile, acr_in::_db) {
            dmmeta::Ssimfile out;
            ssimfile_CopyOut(ssimfile, out);
            prlog(out);
        }ind_end;
    }

    // Build parent/child relationships between ctypes
    if (acr_in::_db.cmdline.data) {
        Main_Data();
    }

    // print resulting ssim strings
    ind_beg(acr_in::_db_tuple_curs, tuple, acr_in::_db) if (PrintTupleQ(tuple)) {
        prlog(tuple.str);
    }ind_end;
}
