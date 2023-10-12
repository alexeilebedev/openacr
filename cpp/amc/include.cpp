// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/include.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool ArgNeedsFwdDeclQ(amc::FField &field) {
    return false
        || field.reftype == dmmeta_Reftype_reftype_Upptr
        || field.reftype == dmmeta_Reftype_reftype_Base
        || field.reftype == dmmeta_Reftype_reftype_Varlen
        || field.reftype == dmmeta_Reftype_reftype_Opt
        || field.reftype == dmmeta_Reftype_reftype_Fbuf;
}

// -----------------------------------------------------------------------------

static bool ArgNeedsSourceQ(amc::FField &field) {
    return false
        || field.reftype == dmmeta_Reftype_reftype_Val
        || field.reftype == dmmeta_Reftype_reftype_Base
        || field.reftype == dmmeta_Reftype_reftype_Inlary
        || field.reftype == dmmeta_Reftype_reftype_Varlen // needs size of element
        || field.reftype == dmmeta_Reftype_reftype_Ptr
        || field.reftype == dmmeta_Reftype_reftype_Bitfld
        || field.reftype == dmmeta_Reftype_reftype_Lary;
}

// -----------------------------------------------------------------------------

// Recursively collect all library namespaces on which namespace TGT depends
// (via the targdep table)
// This populates TGT.c_parent_ns and TGT.c_cppincl
static void CollectParentns(amc::FNs &tgt, amc::FNs &parentns) {
    amc::FTarget *target = parentns.c_target;
    if (target) {
        ind_beg(amc::target_c_targdep_curs, targdep, *target) {
            if (amc::FNs *otherparent = amc::ind_ns_Find(parent_Get(targdep))) {
                CollectParentns(tgt, *otherparent);
            }
        }ind_end;
        if (target->p_ns->nstype == dmmeta_Nstype_nstype_lib
            || target->p_ns->nstype == dmmeta_Nstype_nstype_exe
            || target->p_ns->nstype == dmmeta_Nstype_nstype_objlist
            ) {
            c_parentns_ScanInsertMaybe(tgt, parentns);
            c_cppincl_ScanInsertMaybe(tgt, parentns);
        }
    }
    // parentns should become usedns
}

// -----------------------------------------------------------------------------

// Emit #pragma once for all headers and inline headers
// Emit #includes for all cpp files
// Emit include scan guards for all headers
void amc::GenInclude() {
    algo_lib::Replscope R;

    // load copyright for all generated files
    amc::_db.copyright = FileToString(dev_gitfile_conf_copyright_txt);

    // load licenses
    ind_beg(amc::_db_license_curs,license,amc::_db) {
        if (license.license != "") {
            license.text = FileToString(tempstr()<<"conf/"<<license.license<<".license.txt",algo::FileFlags());
            if (ch_N(license.text)) {
                license.text << eol;
            }
        }
    }ind_end;

    // keep gen file names unique -- gdb requires unique names to set breakpoints
    ind_beg(amc::_db_ns_curs, ns,amc::_db) {
        Set(R, "$ns", ns.ns);
        ns.hdr  = &outfile_Create(Subst(R, "include/gen/$ns_gen.h")).text;
        ns.inl  = &outfile_Create(Subst(R, "include/gen/$ns_gen.inl.h")).text;
        ns.cpp  = &outfile_Create(Subst(R, "cpp/gen/$ns_gen.cpp")).text;
        *ns.cpp  << "#include \"include/algo.h\"  // hard-coded include\n";
    }ind_end;

    // Add pragmas
    ind_beg(amc::_db_ns_curs, ns,amc::_db) if (ch_N(ns.ns)) {
        Ins(&R, *ns.hdr,"#pragma once");
        Ins(&R, *ns.inl,"#pragma once");
    }ind_end;

    // If generating executable target, instantiate all databases in dependency order.
    ind_beg(amc::_db_ns_curs, ns,amc::_db) {
        CollectParentns(ns, ns);
    }ind_end;

    // Add auto-generated includes
    ind_beg(amc::_db_ns_curs, ns,amc::_db) if (ch_N(ns.ns)) {
        ind_beg(amc::ns_c_nsinclude_curs, nsinclude, ns) {
            if (nsinclude.sys) {
                *ns.hdr <<"#include <"<< name_Get(nsinclude) << ">" << eol;
            } else {
                *ns.hdr <<"#include \""<< name_Get(nsinclude) << "\"" << eol;
            }
        }ind_end;
        ind_beg(amc::ns_c_hdrincl_curs, usedns, ns) if (ch_N(usedns.ns) > 0 && &usedns != &ns) {
            *ns.hdr << "#include \"include/gen/"<<usedns.ns<<"_gen.h\"" << eol;
            *ns.inl << "#include \"include/gen/"<<usedns.ns<<"_gen.inl.h\"" << eol;
        }ind_end;
        ind_beg(amc::ns_c_cppincl_curs, usedns, ns) if (ch_N(usedns.ns) > 0) {
            *ns.cpp << "#include \"include/gen/"<<usedns.ns<<"_gen.h\"" << eol;
            *ns.cpp << "#include \"include/gen/"<<usedns.ns<<"_gen.inl.h\"" << eol;
        }ind_end;
        // this trick pragma allows abt to stop scanning headers after this point
        // and compute not-of-of-date files faster.
        *ns.inl << "//#pragma endinclude" << eol;
        *ns.hdr << "//#pragma endinclude" << eol;
        *ns.cpp << "//#pragma endinclude" << eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Determine .cpp and .h dependencies between namespaces based
// on fields
// I.e. if a struct in ns A includes by-value a struct from ns B,
// then there is a header dependency between namespaces.
// If struct in ns A has a Tary of structs from ns B,
// then there is a cpp dependency. Etc.
// Non-field dependencies are created by dispatch, pnew, and some cursors.
void amc::GenUsedNs() {
    // each ns uses itself
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        amc::c_cppincl_ScanInsertMaybe(ns, ns);
        AddHeaderDep(ns, ns);
    }ind_end;

    // visit all fields of all ctypes
    // figure out if a given namespace is being used by cpp, header, or inl
    // special case: smallstr
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        amc::FNs &ns = *ctype.p_ns;
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            if (ArgNeedsSourceQ(field)) {
                AddFullDep(ns, *field.p_arg->p_ns);
            }
            if (ArgNeedsFwdDeclQ(field)) {
                AddFwdDecl(ns,*field.p_arg);
            }
            if (field.reftype == dmmeta_Reftype_reftype_Regx) {
                AddSourceDep(ns, *amc::ind_ns_Find("algo_lib"));
            }
            if (field.c_smallstr && field.c_smallstr->c_numstr) {
                AddFullDep(ns, *field.c_smallstr->c_numstr->p_numtype->p_ns);
            }
        } ind_end;
    }ind_end;

    // visit dispatches
    ind_beg(amc::_db_dispatch_curs, dispatch, amc::_db) {
        amc::FNs &ns = *dispatch.p_ns;
        if (dispatch.c_dispctx) {
            AddFwdDecl(ns,*dispatch.c_dispctx->p_ctype);
        }
        // make sure casetype is accessible from the header.
        AddFullDep(ns,*dispatch.p_casetype->p_ns);
        if (dispatch.p_ctype_hdr) {
            AddFullDep(ns,*dispatch.p_ctype_hdr->p_ns);
        }
        ind_beg(amc::dispatch_c_dispatch_msg_curs, dispmsg, dispatch) {
            AddFwdDecl(ns,*dispmsg.p_ctype);
        }ind_end;
    }ind_end;

    ind_beg(amc::_db_pnew_curs, pnew, amc::_db) {
        amc::FNs &ns = *pnew.p_ns;
        AddFwdDecl(ns,*pnew.p_ctype);
        ind_beg(amc::ctype_c_field_curs, field, *pnew.p_ctype) {
            AddFwdDecl(ns,*field.p_arg);
        }ind_end;
    }ind_end;

    // check that one exe doesn't use another exe (will break during linking, but not fatal for amc)
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        ind_beg(amc::ns_c_cppincl_curs, usedns, ns) if (&usedns != &ns) {
            if (usedns.nstype == dmmeta_Nstype_nstype_exe
                || (ns.nstype == dmmeta_Nstype_nstype_protocol
                    && usedns.nstype == dmmeta_Nstype_nstype_ssimdb)
                || (ns.nstype == dmmeta_Nstype_nstype_ssimdb && ExeQ(usedns))) {
                prerr("amc.execdep"
                      <<Keyval("ns1",ns.ns)
                      <<Keyval("ns1type",ns.nstype)
                      <<Keyval("ns2",usedns.ns)
                      <<Keyval("ns2type",usedns.nstype)
                      <<Keyval("comment","Invalid reference from one namespace to another"));
                algo_lib::_db.exit_code += 1;
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Add .h dependencies to NS on PREREQ
void amc::AddHeaderDep(amc::FNs &ns, amc::FNs &prereq) {
    c_hdrincl_ScanInsertMaybe(ns,prereq);
}

// -----------------------------------------------------------------------------

// Add .cpp dependency to NS on PREREQ
void amc::AddSourceDep(amc::FNs &ns, amc::FNs &prereq) {
    c_cppincl_ScanInsertMaybe(ns,prereq);
}

// -----------------------------------------------------------------------------

// Add .cpp and .h dependencies to NS on PREREQ
void amc::AddFullDep(amc::FNs &ns, amc::FNs &prereq) {
    c_cppincl_ScanInsertMaybe(ns,prereq);
    c_hdrincl_ScanInsertMaybe(ns,prereq);
}

// -----------------------------------------------------------------------------

// Check if the header for NS includes header for OTHER
bool amc::HdrIncludesQ(amc::FNs &ns, amc::FNs &other) {
    bool ret = &ns == &other;
    if (!ret) {
        ind_beg(amc::ns_c_hdrincl_curs,hdrincl,ns) {
            if (&hdrincl == &other) {
                ret=true;
                break;
            }
        }ind_end;
    }
    return ret;
}
