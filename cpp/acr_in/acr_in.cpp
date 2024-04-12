// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: acr_in (exe) -- ACR Input - compute set of ssimfiles or tuples used by a specific target
// Exceptions: yes
// Source: cpp/acr_in/acr_in.cpp
//
// Print ssim tuples desired by a specified process,
// or print names of ssim files

#include "include/acr_in.h"

// -----------------------------------------------------------------------------

// Find or create an nsssimfile entry
// Nsssimfile = ns/ssimfile, representing a finput relation between ns and ssimfile
acr_in::FNsssimfile &acr_in::ind_nsssimfile_GetOrCreate(algo::strptr ns, algo::strptr ssimfile) {
    tempstr key = tempstr() << ns << "/" << ssimfile;
    acr_in::FNsssimfile *ret = acr_in::ind_nsssimfile_Find(key);
    // create nssimfile entry
    if (!ret) {
        ret = &acr_in::nsssimfile_Alloc();
        ret->nsssimfile = key;
        vrfy_(nsssimfile_XrefMaybe(*ret));
    }
    return *ret;
}

// True if we are doing a reverse lookup of namespaces by ssimfile,
// as opposed to a forward lookup of ssimfiles by namespace
bool acr_in::ReverseLookupQ() {
    return acr_in::_db.cmdline.r.expr != "";
}

// -----------------------------------------------------------------------------

// Populate ns.select field
// Start by selecting any namespace that matches command line "-ns" argument
// Then extend namespace selection to include all dependent namespaces via targdep
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
        if (!target.p_ns->select) {
            verblog("acr_in.ns  select:"<<target.target);
            target.p_ns->select = true;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Find base type (the one associated with ssimfile) for this ctype
// by looking for a base field
// If none found, return ctype itself
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

// Select ssimfiles loaded by selected namespaces
// There are two way a ssimfile gets selected:
// - it is a finput to a selected namespace
// - it matches command line "-r" argument
// If a ssimfile is selected, it is marked as "is_finput" and its ctype is is added
// to the temporary "zd_todo" list.
// The "zd_todo" is used to topologically order all selected ssimfiles in order
// of their dependence.
// In the end, global "zd_ssimfile" list is populated in the order in which
// the ssimfiles should be printed.
static void Main_SelectSsimfile() {
    // select ssimfiles for lookup
    // select ssimfiles by namespace, or directly via -r option
    ind_beg(acr_in::_db_finput_curs, finput, acr_in::_db) {
        acr_in::FCtype *ctype = Basetype(*finput.p_field->p_arg);// prefer base
        bool add = finput.p_ns->select || (ctype->c_ssimfile && Regx_Match(acr_in::_db.cmdline.r,ctype->c_ssimfile->ssimfile));
        if (add) {
            if (ctype->c_ssimfile) {
                ctype->c_ssimfile->is_finput = true;
                acr_in::ind_nsssimfile_GetOrCreate(finput.p_ns->ns,ctype->c_ssimfile->ssimfile);
            }
            zd_todo_Insert(*ctype);
        }
    }ind_end;
    // topologically sort ctypes that have ssimfiles
    // zd_todo is a stack.
    // this produces global list zd_ssimfile which correctly orders ssimfiles by dependency between each other
    while (acr_in::FCtype *ctype = acr_in::zd_todo_Last()) {
        if (bool_Update(ctype->visit,true)) {
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

static bool PrintTupleQ(acr_in::FTuple &tuple) {
    bool ret=zd_select_InLlistQ(tuple);
    bool print_all = acr_in::_db.cmdline.checkable;
    if (!print_all) {// restrict
        ret = ret && tuple.p_ctype->c_ssimfile->is_finput;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Propagate nsssimfile entries to all dependents
static void Main_PropagateNsSsimfile() {
    int nold=0;
    do {
        nold=acr_in::nsssimfile_N();
        ind_beg(acr_in::_db_nsssimfile_curs,nsssimfile,acr_in::_db) {
            acr_in::FNs *ns=nsssimfile.p_ns;
            if (ns->c_target) {
                ind_beg(acr_in::target_c_targdep_child_curs,child,*ns->c_target) {
                    acr_in::ind_nsssimfile_GetOrCreate(target_Get(child),ssimfile_Get(nsssimfile));
                }ind_end;
            }
        }ind_end;
    } while (acr_in::nsssimfile_N()>nold);
    // select additional namespaces only in -r (reverse lookup mode)
    // otherwise, show only those namespaces which were selected
    if (acr_in::ReverseLookupQ()) {
        ind_beg(acr_in::_db_nsssimfile_curs,nsssimfile,acr_in::_db) {
            nsssimfile.p_ns->select=true;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Main output function
// There are 3 modes:
//   - tree mode (works both with both kinds of lookups)
//   - ssimfile lookup mode (-r)
//   - ns lookup mode (-ns)
static void Main_List() {
    ind_beg(acr_in::_db_zd_ssimfile_curs, ssimfile, acr_in::_db) {
        ind_beg(acr_in::ssimfile_zd_nsssimfile_ssimfile_curs,nsssimfile,ssimfile) {
            nsssimfile.show=true;
        }ind_end;
    }ind_end;
    if (acr_in::_db.cmdline.t) {
        // show tree
        ind_beg(acr_in::_db_ns_curs, ns, acr_in::_db) if (ns.select) {
            int nshow=0;
            ind_beg(acr_in::ns_zd_nsssimfile_ns_curs, nsssimfile, ns) if (nsssimfile.show) {
                nshow++;
            }ind_end;
            if (nshow) {
                prlog(ns.ns<<":");
                ind_beg(acr_in::ns_zd_nsssimfile_ns_curs, nsssimfile, ns) if (nsssimfile.show) {
                    prlog("    dmmeta.ssimfile"
                          <<Keyval("ssimfile",ssimfile_Get(nsssimfile))
                          <<Keyval("ctype",nsssimfile.p_ssimfile->ctype));
                }ind_end;
            }
        }ind_end;
    } else if (acr_in::ReverseLookupQ()) {
        // show list of ns <-> ssimfile pairs
        ind_beg(acr_in::_db_nsssimfile_curs, nsssimfile, acr_in::_db) if (nsssimfile.show) {
            prlog("acr_in.nsssimfile"
                  //<<Keyval("nsssimfile",nsssimfile.nsssimfile)
                  <<Keyval("ns",ns_Get(nsssimfile))
                  <<Keyval("ssimfile",ssimfile_Get(nsssimfile)));
        }ind_end;
    } else {
        // show ssimfiles
        ind_beg(acr_in::_db_zd_ssimfile_curs, ssimfile, acr_in::_db) {
            dmmeta::Ssimfile out;
            ssimfile_CopyOut(ssimfile, out);
            prlog(out);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void acr_in::Main() {
    // pick reasonable default
    if (!acr_in::_db.cmdline.data && !acr_in::_db.cmdline.list) {
        acr_in::_db.cmdline.list = true;
    }
    if (ReverseLookupQ()) {
        acr_in::_db.cmdline.sigcheck=false;
    }
    if (acr_in::_db.cmdline.ns.expr != "" && acr_in::_db.cmdline.r.expr != "") {
        vrfy(0,"acr_in: please choose either [-ns] or -r options, but not both");
    }

    Main_Ns();

    Main_SelectSsimfile();

    if (ReverseLookupQ()) {
        Main_PropagateNsSsimfile();
    }

    // output dispsigcheck records for all selected namespaces
    if (acr_in::_db.cmdline.sigcheck) {
        ind_beg(acr_in::_db_dispsig_curs, dispsig, acr_in::_db) if (dispsig.p_ns->select) {
            prlog(dmmeta::Dispsigcheck(dispsig.dispsig, dispsig.signature));
        }ind_end;
    }

    // list ssimfiles
    if (acr_in::_db.cmdline.list) {
        Main_List();
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
