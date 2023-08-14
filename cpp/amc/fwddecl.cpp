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

// Return TRUE if FIELD requires a forward declaration for its type
// This may be required since the type may be unknown at the point where the field is being
// defined.
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
        if (!ctype.c_bltin && (!ctype.c_cextern || ctype.c_cextern->isstruct)) {
            AddFwdDecl(ns,ctype);
        }
    }ind_end;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (FwdDeclQ(field)) {
            AddFwdDecl(ns,*field.p_arg);
        }ind_end;
    }ind_end;

    ind_beg(amc::ns_c_fwddecl_curs,fwddecl,ns) {
        dmmeta::CtypePkey ctypepkey=ctype_Get(fwddecl);// may be a non-existent ctype -- such as a cursor
        if (!FwdDeclExistsQ(ns,fwddecl)) {
            *ns.hdr << "namespace "<<dmmeta::Ctype_ns_Get(ctypepkey)<<" { struct "<<dmmeta::Ctype_name_Get(ctypepkey)<<"; }"<<eol;
        }
    }ind_end;
    // extern declaration for global variables (usually just one, FDb)
    if (amc::FField *field = ns.c_globfld) {
        *ns.hdr << "namespace "<<ns.ns<<" { extern struct "<<field->cpp_type<<" "<<name_Get(*field)<<"; }"<<eol;
    }

    // hook function typedefs
    {
        tempstr out;
        int n=0;
        BeginNsBlock(out,ns,"hook_fcn_typedef");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.c_hook) {
                amc::FHook &hook=*field.c_hook;
                out<<"    typedef void (*"<<name_Get(*hook.p_funcptr)<<")(); // hook:"<<field.field<<eol;
                n++;
                if (!StaticQ(hook)) {// user context (any reference -- assigned via template)
                    amc::AddArg(out, tempstr() << "void*" << " userctx");
                }
                if (field.arg != "") {// additional argument
                    amc::AddArg(out, tempstr() << amc::Refto(field.p_arg->cpp_type) << " arg");
                }
            }ind_end;
        }ind_end;
        EndNsBlock(out,ns,"hook_decl");
        if (n) {
            *ns.hdr << out;
        }
    }
}

// -----------------------------------------------------------------------------

void amc::gen_ns_fwddecl2() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        if (ns.select) {
            *ns.hdr << "// gen:ns_fwddecl2" << eol;
            Fwddecl_GenStructNs(ns);
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
        // fwd declaration for json formatters
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
                if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Json) {
                    amc::ind_fwddecl_GetOrCreate(tempstr()<<ns_Get(ctype)<<".lib_json.FNode");
                }
            }ind_end;
        }ind_end;
        // hooks
        ind_beg(amc::ns_c_gstatic_curs, gstatic,ns) {
            ind_beg(amc::ctype_c_field_curs, field, *gstatic.p_field->p_arg) {
                if (field.reftype == dmmeta_Reftype_reftype_Hook) {
                    Hook_FwdDecl(field,gstatic);
                }
            }ind_end;
        }ind_end;
    }ind_end;
}
