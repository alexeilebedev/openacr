// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/struct.cpp -- C++ struct output
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// True if the field can be passed via constructor
bool amc::PassFieldViaArgQ(amc::FField &field, amc::FCtype &ctype) {
    bool ok = ((!PadQ(field) && amc::ValQ(field))
               || field.reftype == dmmeta_Reftype_reftype_Ptr
               || field.reftype == dmmeta_Reftype_reftype_Upptr)
        && !(field.c_typefld && ctype.c_msgtype)
        && !(GetLenfld(field) && ctype.c_msgtype)
        && !FldfuncQ(field)
        && !ctype_zd_varlenfld_InLlistQ(field);// stack constructor doesn't accept varlen or optional fields
    return ok;
}

// -----------------------------------------------------------------------------

static tempstr ShowAccessPaths(amc::FCtype &ctype) {
    tempstr str_create;
    tempstr str_glob;
    tempstr str_nonglob;
    ind_beg(amc::ctype_zd_access_curs, inst, ctype) {
        if (inst.p_ctype->p_ns == ctype.p_ns && inst.p_reftype->isval && inst.reftype != dmmeta_Reftype_reftype_Val) {
            str_create << "// create: "<< inst.field << " ("<< inst.reftype<<")"<<eol;
        }
    }ind_end;
    ind_beg(amc::ctype_zd_access_curs, inst, ctype) {
        if (inst.p_ctype->p_ns == ctype.p_ns && (!inst.p_reftype->isval || inst.reftype == dmmeta_Reftype_reftype_Lary) && GlobalQ(*inst.p_ctype)) {
            str_glob << "// global access: " << name_Get(inst) << " ("<< inst.reftype;
            if (inst.reftype == dmmeta_Reftype_reftype_Lary) {
                str_glob << ", by rowid";
            }
            if (inst.c_thash) {
                str_glob << ", hash field "<<name_Get(*inst.c_thash->p_hashfld);
            }
            if (inst.c_sortfld) {
                str_glob << ", sort field "<<name_Get(*inst.c_sortfld->p_sortfld);
            }
            str_glob <<")"<<eol;
        }
    }ind_end;
    ind_beg(amc::ctype_zd_access_curs, inst, ctype) {
        if (inst.p_ctype->p_ns == ctype.p_ns && !inst.p_reftype->isval && !GlobalQ(*inst.p_ctype)) {
            str_nonglob << "// access: "<< inst.field << " ("<< inst.reftype<<")"<<eol;
        }
    }ind_end;
    tempstr retval;
    if (ch_N(str_create)) {
        retval << str_create;
    }
    if (ch_N(str_glob)) {
        retval << str_glob;
    }
    if (ch_N(str_nonglob)) {
        retval << str_nonglob;
    }
    return retval;
}

// -----------------------------------------------------------------------------

static void GenStruct_Ctor2(algo_lib::Replscope &R, amc::FCtype &ctype) {
    amc::FNs &ns = *ctype.p_ns;
    // keeps track of which section we're in (public or private)
    bool cur_priv=false;

    // print member function prototypes, first public then private
    for (int priv=0; priv<2; priv++) {
        // field-attached functions
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            ind_beg(amc::field_c_ffunc_curs,func,field) if (func.member && func.isprivate==priv) {
                if (bool_Update(cur_priv, func.isprivate)) {
                    Ins(&R, *ns.hdr, (cur_priv ? "private:" : "public:"));
                }
                tempstr proto;
                PrintFuncProto(func,&ctype,proto,false);
                algo::InsertIndent(*ns.hdr, proto,1);
            }ind_end;
        }ind_end;
        // ctype-attached functions
        // TODO: create an empty field with reftype:Ctype
        // so that this code doesn't need to be repeated
        ind_beg(amc::ctype_c_ffunc_curs,func,ctype) if (func.member && func.isprivate==priv) {
            if (bool_Update(cur_priv, func.isprivate)) {
                Ins(&R, *ns.hdr, (cur_priv ? "private:" : "public:"));
            }
            tempstr proto;
            PrintFuncProto(func,&ctype,proto,false);
            algo::InsertIndent(*ns.hdr,proto,1);
        }ind_end;
    }

    // print friend functions
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) if (inst.p_reftype->hasalloc) {
        ind_beg(amc::field_c_ffunc_curs,func,inst) if (func.isalloc) {
            if (bool_Update(cur_priv, true)) {
                Ins(&R, *ns.hdr, (cur_priv ? "private:" : "public:"));
            }
            tempstr proto;
            PrintFuncProto(func,&ctype,proto,false);
            algo::InsertIndent(*ns.hdr,proto,1);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Same as algo::Tabulated, but indent at most MAXGROUP lines at a time
static tempstr ChunkyTabulated(algo::strptr in, algo::strptr sep, algo::strptr fmt, int colspace, int maxgroup) {
    int n=0;
    tempstr ret;
    tempstr temp;
    ind_beg(Line_curs,line,in) {
        temp << line << eol;
        n++;
        if (n==maxgroup) {
            ret << Tabulated(temp,sep,fmt,colspace);
            n=0;
            ch_RemoveAll(temp);
        }
    }ind_end;
    if (ch_N(temp)) {
        ret << Tabulated(temp,sep,fmt,colspace);
    }
    return ret;
}

// -----------------------------------------------------------------------------

void amc::GenStruct(amc::FNs& ns, amc::FCtype& ctype) {// print struct contents
    algo_lib::Replscope R;
    Set(R, "$Name", name_Get(ctype));
    Set(R, "$ns", ns_Get(ctype));
    Set(R, "$Cpptype",    ctype.cpp_type);
    Subst(R,"$Cpptype");// mark used
    amc::CppSection(*ns.hdr, tempstr() << ctype.ctype, true);
    // show all places this ctype is instantiated
    *ns.hdr << ShowAccessPaths(ctype);
    if (ctype.c_pack) {
        Ins(&R, *ns.hdr, "#pragma pack(push,1)");
    }
    tempstr comment;
    Ins(&R, comment, "$ns.$Name", false);
    if (ch_N(ctype.comment.value)) {
        comment << ": "<<ctype.comment;
    }
    Set(R, "$comment", comment);
    Ins(&R, *ns.hdr, "struct $Name { // $comment");
    (void)ChunkyTabulated;
    algo::InsertIndent(*ns.hdr, Tabulated(ctype.body, "\t", "ll", 2), 1);
    if (!GlobalQ(ctype)) {
        GenStruct_Ctor2(R,ctype);
    }
    *ns.hdr << "}";
    *ns.hdr << ";\n" ;
    if (ctype.c_pack) {
        Ins(&R, *ns.hdr, "#pragma pack(pop)");
    }
    *ns.hdr << "\n" ;
}
