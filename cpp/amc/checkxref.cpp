// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/checkxref.cpp -- X-reference checker
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void CheckXref_Nocascdel(amc::FXref &xref) {
    if (xref.p_field->reftype != dmmeta_Reftype_reftype_Upptr
        && xref.p_field->reftype != dmmeta_Reftype_reftype_Count
        && CanDeleteQ(*xref.p_field->p_ctype)
        && !xref.c_nocascdel) {
        prerr("amc.need_cascdel"
              <<Keyval("xref",xref.field)
              <<Keyval("comment","Cascdel needed because xref was provided"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static void CheckXref_Cascdel(amc::FXref &xref) {
    int ninst =0;
    ind_beg(amc::ctype_zd_access_curs,access,*xref.p_field->p_arg) {
        ninst += access.p_reftype->isval || access.reftype == dmmeta_Reftype_reftype_Val;
    }ind_end;
    if (ninst > 1) {
        prerr("amc.too_many_inst"
              <<Keyval("field",xref.field)
              <<Keyval("comment","Unclear which instance of field to delete on cascdel"));
        ind_beg(amc::ctype_zd_access_curs,access,*xref.p_field->p_arg) if (access.p_reftype->isval) {
            prerr("#  "
                  <<Keyval("field",access.field)
                  <<Keyval("comment","Could be this one"));
        }ind_end;
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static bool HasNoncascdelRefs(amc::FCtype &parent, amc::FField &child, bool show) {
    bool retval=false;
    ind_beg(amc::ctype_c_field_curs,field,parent) {
        if (field.p_arg==child.p_ctype && field.c_xref && !field.c_cascdel) {
            if (show) {
                prerr("dmmeta.cascdel"
                      <<Keyval("field",field.field));
            }
            retval=true;
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

static void CheckXref_DanglingUpref(amc::FXref &xref) {
    amc::FField &field     = *xref.p_field;
    amc::FField *xreffld   = GetKeyfld(xref);
    amc::FField *tgtinst = FirstInst(*field.p_arg);
    bool isupptr = xreffld && field.reftype == dmmeta_Reftype_reftype_Upptr;
    if (tgtinst && isupptr && CanDeleteQ(*field.p_arg) && HasNoncascdelRefs(*field.p_arg,field,false)) {
        prerr("amc.dangling_pointer  "
              <<Keyval("field",field.field)
              <<Keyval("target",field.arg)
              <<Keyval("target_inst",tgtinst->field)
              <<Keyval("comment","Upptr reference to a deletable type requires cascdel xrefs"));
        HasNoncascdelRefs(*field.p_arg,field,true);
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static void CheckXref_Xreffld(amc::FXref &xref) {
    amc::FField &field     = *xref.p_field;
    amc::FCtype &ctype     = *field.p_ctype;
    amc::FField *xreffld   = GetKeyfld(xref);
    amc::FField *viafld = GetViafld(xref);
    bool glob2 = field.p_ctype == ctype.p_ns->c_globfld->p_ctype;
    if (glob2 && xreffld) {
        prerr("amc.extra_xreffld"
              <<Keyval("field",xref.field)
              <<Keyval("comment","extraneous xreffld for global xref"));
        algo_lib::_db.exit_code++;
    }
    if (!glob2 && !xreffld) {
        prerr("amc.missing_xreffld"
              <<Keyval("field",xref.field)
              <<Keyval("comment","xreffld is required for non-global xref"));
        algo_lib::_db.exit_code++;
    }
    bool isptr = xreffld && PtrQ(*xreffld);
    if (!glob2 && isptr && viafld) {
        prerr("amc.badvia"
              <<Keyval("xref",xref.field)
              <<Keyval("xreffld",xreffld->field)
              <<Keyval("comment","Use xreffld instead of xrefvia here"));
        prerr("dmmeta.xreffld"
              <<Keyval("xref",xref.field)
              <<Keyval("field",viafld->field));
        prerr("acr  "
              <<Keyval("dmmeta.xrefvia",xref.field)<<"  -del -write");
        algo_lib::_db.exit_code++;
    }
    if (!glob2 && !isptr && !viafld) {
        prerr("amc.missing_xrefvia  "
              <<Keyval("comment","Xrefvia record must be specified for non-pointer xreffld."));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static void CheckXref_Via(amc::FXref &xref) {
    amc::FField     &field = *xref.p_field;
    amc::FCtype     *parent = xref.p_field->reftype==dmmeta_Reftype_reftype_Upptr ? field.p_arg : field.p_ctype;
    amc::FField *via   = GetViafld(xref);
    // Example:
    // dmmeta.field  field:ssim2odb.FSsimfile.p_ctype  arg:ssim2odb.FCtype  reftype:Upptr  dflt:""  comment:""
    //     dmmeta.xref  field:ssim2odb.FSsimfile.p_ctype  inscond:true  via:ssim2odb.FDb.ind_ctype/dmmeta.Ssimfile.ctype
    // Check taht xrefvia's 'via' is an index that can be indeed used to look up xref.p_field->arg.
    if (via && via->p_arg != parent) {
        prerr("amc.checkxref_via"
              <<Keyval("xrefvia",xref.field)
              <<Keyval("via_target",via->arg)// potentially reachable ctype
              <<Keyval("needed_target",parent->ctype)
              <<Keyval("comment","Xrefvia index is unrelated"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static bool SsimVolatileQ(amc::FCtype &ctype) {
    return ctype.c_ssimfile && ctype.c_ssimfile->c_ssimvolatile;
}

static void CheckXref_Volatile(amc::FXref &xref) {
    amc::FField     &field = *xref.p_field;
    amc::FCtype     *parent = xref.p_field->reftype==dmmeta_Reftype_reftype_Upptr ? field.p_arg : field.p_ctype;
    amc::FCtype     *child = field.p_ctype == parent ? field.p_arg : field.p_ctype;
    amc::FCtype *baseparent = GetBaseType(*parent,NULL);
    amc::FCtype *basechild = GetBaseType(*child,NULL);
    if (baseparent && basechild && SsimVolatileQ(*baseparent) && basechild->c_ssimfile && !SsimVolatileQ(*basechild)) {
        prerr("amc.checkxref_volatile"
              <<Keyval("xref",xref.field)
              <<Keyval("ssimfile",basechild->c_ssimfile->ssimfile)
              <<Keyval("refers_to",baseparent->c_ssimfile->ssimfile)
              <<Keyval("comment","X-ref dependency error on a table marked ssimvolatile is not allowed"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

static void CheckXref_Double(amc::FXref &xref) {
    amc::FField &field = *xref.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FField *keyfld = GetKeyfld(xref);
    // The only allowed dependency between xrefs must involve Upptr.
    if (keyfld && keyfld->c_xref) {
        bool ok =
            // true x-reference (not a dependency between 2 x-refs from same source)
            (xref.p_field->p_ctype != keyfld->p_ctype)
            // sortxref + upptr
            || (ns.c_nsx && ns.c_nsx->sortxref && keyfld->reftype == dmmeta_Reftype_reftype_Upptr);
        if (!ok) {
            prerr("amc.xref_dependency"
                  <<Keyval("xref",xref.field)
                  <<Keyval("xreffld",keyfld->field)
                  <<Keyval("comment","dependency between xref is not allowed"));
            algo_lib::_db.exit_code++;
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckXref_Impossible(amc::FXref &xref) {
    amc::FField     &field = *xref.p_field;
    // I temporarily allowed x-refs across namespace, but this quickly
    // turned out to be a bad idea  (Upptr is an exception)
    if (field.reftype != dmmeta_Reftype_reftype_Upptr && !(amc::ns_Get(*field.p_ctype) == amc::ns_Get(*field.p_arg))) {
        prerr("amc.xref_across_namespace"
              <<Keyval("xref",xref.field)
              <<Keyval("comment","xref across namespaces is not supported: parent and child must be in the same ns"));
        algo_lib::_db.exit_code++;
    }
    if (keyfld_Get(xref) == xref.field) {
        prerr("amc.meaningless_key"
              <<Keyval("xref",xref.field)
              <<Keyval("comment","keyfld cannot be the same as the xref itself."));
        algo_lib::_db.exit_code++;
    }
    if (xref.p_field->c_cppfunc) {
        prerr("amc.noxref_cppfunc"
              <<Keyval("xref",xref.field)
              <<Keyval("comment","Xref using cppfunc field is not allowed (unsafe)"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

void amc::gen_check_xref() {
    // Require presence of xref on certain fields, or accept exception record
    ind_beg(amc::_db_field_curs,field,amc::_db) {
        if (field.p_reftype->isxref && !field.c_xref && !field.c_noxref) {
            prerr("amc.xref_required"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    ind_beg(amc::_db_xref_curs,xref,amc::_db) {
        if (!xref.p_field->p_ctype->p_ns->c_globfld) {
            prerr("amc.missing_glob"
                  <<Keyval("field",xref.field)
                  <<Keyval("comment","namespace is missing a Global field required to process x-refs"));
            algo_lib::_db.exit_code++;
        }
        if (algo_lib::_db.exit_code == 0) {
            CheckXref_Via(xref);
            CheckXref_Volatile(xref);
            CheckXref_Double(xref);
            CheckXref_Impossible(xref);
            CheckXref_Xreffld(xref);
            if (xref.p_field->c_cascdel) {
                CheckXref_Cascdel(xref);
            } else {
                CheckXref_Nocascdel(xref);
            }
            CheckXref_DanglingUpref(xref);
        }
    }ind_end;
}
