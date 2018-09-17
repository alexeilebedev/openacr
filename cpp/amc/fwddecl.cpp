// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/fwddecl.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Return TRUE if namespace NS already contains a forward declaration
// of FWDDECL, either directly, or transitively through one of the namespaces
// included in ns_gen.h
bool amc::FwdDeclExistsQ(amc::FNs &ns, amc::FFwddecl &fwddecl) {
    bool ret = false;
    ind_beg(amc::ns_c_hdrincl_curs, usedns, ns) {
        if (ch_N(usedns.ns) > 0 && &usedns != &ns) {
            if (amc::ind_fwddecl_Find(tempstr()<<usedns.ns<<"."<<ctype_Get(fwddecl))) {
                ret=true;
                break;
            }
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE if FIELD requires a forward declaration
bool amc::FwdDeclQ(amc::FField &field) {
    return ns_Get(*field.p_arg)!=""
        && !field.p_arg->c_cextern
        && (field.reftype == dmmeta_Reftype_reftype_Fbuf
            || field.reftype == dmmeta_Reftype_reftype_Opt
            || field.reftype == dmmeta_Reftype_reftype_Varlen
            || field.reftype == dmmeta_Reftype_reftype_Lary);
}

// -----------------------------------------------------------------------------

static void Fwddecl_GenStructNs(amc::FNs &ns) {
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) {// forward-declare all ctypes in the namespace
        if (!ctype.c_bltin && !ctype.c_cextern) {
            AddFwdDecl(ns,ctype);
        }
    }ind_end;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (FwdDeclQ(field)) {
            AddFwdDecl(ns,*field.p_arg);
        }ind_end;
    }ind_end;
    // forward declarations for cursors
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            ind_beg(amc::tclass_c_tfunc_curs, tfunc, *field.p_reftype->p_tclass) if (tfunc.c_tcursor) {
                amc::FField *pool = FirstInst(ctype);
                tempstr parname = pool ? tempstr(name_Get(*pool)) : tempstr(name_Get(ctype));
                amc::ind_fwddecl_GetOrCreate(tempstr()<<ns.ns<<"."<<ns.ns<<"."<<parname<<"_"<<name_Get(field)<<"_"<<name_Get(tfunc));
            }ind_end;
        }ind_end;
    }ind_end;
    ind_beg(amc::ns_c_fwddecl_curs,fwddecl,ns) {
        dmmeta::CtypePkey ctypepkey=ctype_Get(fwddecl);// may be a non-existent ctype -- such as a cursor
        if (!FwdDeclExistsQ(ns,fwddecl)) {
            *ns.hdr << "namespace "<<dmmeta::Ctype_ns_Get(ctypepkey)<<" { struct "<<dmmeta::Ctype_name_Get(ctypepkey)<<"; }"<<eol;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Fwddecl_Global(amc::FNs &ns) {
    // Forward-declarations of all ctypes for which cdecl.fwddecl is set.
    ind_beg(amc::_db_cdecl_curs,cdecl,amc::_db) {
        if (cdecl.fwddecl) {
            if (cdecl.p_ctype->p_ns->ns == "") {
                *ns.hdr << "struct "<<cdecl.p_ctype->cpp_type<<";\n";
            } else {
                *ns.hdr << "namespace "<<ns_Get(*cdecl.p_ctype)<<" { struct "<<name_Get(*cdecl.p_ctype)<<"; }\n";
            }
        }
        if (cdecl.gen_using) {
            *ns.hdr << "using "<<cdecl.p_ctype->cpp_type<<";\n";
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_fwddecl2() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        if (ns.select) {
            Fwddecl_GenStructNs(ns);
            if (ns.ns == "") {
                Fwddecl_Global(ns);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// FIELD - hook field, e.g. amc.FTclass.step
// GSTATIC - describes table over which to loop
// Create prototypes for user-implemented functions by scanning
// all tuples loaded for the gstatic.
static void Hook_FwdDecl(amc::FField &field, amc::FGstatic &gstatic) {
    ind_beg(amc::ctype_c_static_curs, static_tuple, *gstatic.p_field->p_arg) {
        // TODO: add arguments to this function
        tempstr key=amc::PkeyCppident(static_tuple.tuple);
        amc::FFunc &func = amc::ind_func_GetOrCreate(StaticHookPkey(field,key));
        func.ret   = "void";
        func.proto<<StripNs("",StaticHookPkey(field,key))<<"()";
        if (field.c_hook) {
            if (field.arg != "") {// additional argument
                amc::AddArg(func.proto, amc::Refto(field.p_arg->cpp_type));
            }
        }
        func.comment<<"User-implemented function from gstatic:"<<gstatic.field;
        func.glob  = true;
        func.extrn = true;
    }ind_end;
}

// -----------------------------------------------------------------------------

// emit forward-declarations of steps
void amc::gen_ns_fwddecl() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.select) {
        ind_beg(amc::ns_c_gstatic_curs, gstatic,ns) {
            ind_beg(amc::ctype_c_field_curs, field, *gstatic.p_field->p_arg) {
                if (field.reftype == dmmeta_Reftype_reftype_Hook) {
                    Hook_FwdDecl(field,gstatic);
                }
            }ind_end;
        }ind_end;
    }ind_end;
}
