// (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/struct.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool PassFieldViaArgQ(amc::FField &field, amc::FCtype &ctype) {
    bool ok = ((!PadQ(field) && amc::ValQ(field))
               || field.reftype == dmmeta_Reftype_reftype_Ptr
               || field.reftype == dmmeta_Reftype_reftype_Upptr)
        && !(field.c_typefld && ctype.c_msgtype)
        && !(GetLenfld(field) && ctype.c_msgtype)
        && !FldfuncQ(field)
        && (&field != ctype.c_varlenfld);// stack constructor doesn't accept varlen or optional fields
    return ok;
}

// -----------------------------------------------------------------------------

static void GenStruct_Ctor(amc::FCtype &ctype) {
    algo_lib::Replscope R;
    Set(R, "$Name", name_Get(ctype));
    Set(R, "$Cpptype",    ctype.cpp_type);
    Subst(R,"$Cpptype");// mark used
    Subst(R,"$Name");// mark used
    amc::FNs &ns = *ctype.p_ns;
    tempstr argstr;    // generate field-wise constructor
    int n_args = 0;
    amc::FField *single_arg=NULL;
    ListSep ls("\n        ,");
    tempstr initstr;
    strptr ls2 = strptr(":");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (PassFieldViaArgQ(field,ctype)) {
        argstr << ls;
        strptr_PrintPadRight(Argtype(field), argstr,  30);
        argstr << " in_" << name_Get(field);
        ++n_args;
        single_arg = &field;
        if (!FixaryQ(field) && !field.c_fbigend) {
            Set(R, "$ls", ls2);
            Set(R, "$name", name_Get(field));
            Ins(&R, initstr, "    $ls $name(in_$name)");
            ls2 = strptr(",");
        }
    }ind_end;
    if (n_args > 0) {
        Set(R, "$Args", argstr);
        Set(R, "$initstr", initstr);
        Ins(&R, *ns.hdr, "    explicit $Name($Args);");
        Ins(&R, *ns.inl, "inline $Cpptype::$Name($Args)");
        Ins(&R, *ns.inl, "$initstr{");
        ind_beg(amc::ctype_c_field_curs, fld,ctype) {
            bool val = ValQ(fld);
            amc::FLenfld *lenfld=GetLenfld(fld);
            if (val && (FixaryQ(fld) || fld.c_tary) && !PadQ(fld)) {
                vrfy_(!fld.c_fbigend);
                Set(R, "$name", name_Get(fld));
                Ins(&R, *ns.inl, "    $name_Setary(*this, in_$name);");
            } else if (val && fld.c_typefld && ctype.c_msgtype) {
                Set(R, "$Msgtype", ctype.c_msgtype->type.value);
                Set(R, "$assign", amc::AssignExpr(fld, "*this", "$Msgtype", true));
                Ins(&R, *ns.inl, "    $assign;");
            } else if (val && lenfld && ctype.c_msgtype) {
                Set(R, "$extralen", tempstr() << lenfld->extra);
                Set(R, "$assign", amc::AssignExpr(fld, "*this", "sizeof(*this) + ($extralen)", true));
                Ins(&R, *ns.inl, "    $assign;");
            } else if (fld.c_fbigend) {
                Set(R, "$name", name_Get(fld));
                Ins(&R, *ns.inl, "    $name_Set(*this,in_$name);");
            }
        }ind_end;
        Ins(&R, *ns.inl, "}");
    }
    // generate implicit constructor (non-string types only)
    if (single_arg && n_args == 1 && c_fconst_N(*single_arg) && !FieldStringQ(*single_arg)) {
        Set(R, "$implicittype", Enumtype(*single_arg));
        Set(R, "$Set", AssignExpr(*single_arg, "*this", "arg", true));
        Ins(&R, *ns.hdr, "    $Name($implicittype arg);");
        Ins(&R, *ns.inl, "inline $Cpptype::$Name($implicittype arg) { $Set; }");
    }
}

// -----------------------------------------------------------------------------

static void GenStruct_Cstring(algo_lib::Replscope &R, amc::FCtype &ctype) {
    amc::FNs &ns = *ctype.p_ns;

    Ins(&R, *ns.hdr, "    cstring& operator =(const strptr &t);");
    Ins(&R, *ns.hdr, "    cstring(const cstring &t);");
    Ins(&R, *ns.hdr, "    cstring& operator =(const cstring &s);");
    Ins(&R, *ns.hdr, "    explicit cstring(const strptr &s);");
    Ins(&R, *ns.hdr, "    cstring(const tempstr &rhs);");
    Ins(&R, *ns.hdr, "    operator const strptr() const {");
    Ins(&R, *ns.hdr, "        return strptr(ch_elems,ch_n);");
    Ins(&R, *ns.hdr, "    }");
    Ins(&R, *ns.hdr, "    operator strptr() {");
    Ins(&R, *ns.hdr, "        return strptr(ch_elems,ch_n);");
    Ins(&R, *ns.hdr, "    }");

    Ins(&R, *ns.cpp, "algo::cstring& algo::cstring::operator =(const algo::strptr &rhs) {");
    Ins(&R, *ns.cpp, "    algo::ch_Setary(*this, rhs);");
    Ins(&R, *ns.cpp, "    return *this;");
    Ins(&R, *ns.cpp, "}");
    Ins(&R, *ns.cpp, "");

    Ins(&R, *ns.cpp, "algo::cstring::cstring(const algo::cstring &rhs) {");
    Ins(&R, *ns.cpp, "    cstring_Init(*this);");
    Ins(&R, *ns.cpp, "    algo::ch_Setary(*this, (algo::cstring&)rhs);");
    Ins(&R, *ns.cpp, "}");
    Ins(&R, *ns.cpp, "");

    Ins(&R, *ns.inl, "inline cstring& cstring::operator =(const cstring &rhs) {");
    Ins(&R, *ns.inl, "    algo::ch_Setary(*this, (algo::cstring&)rhs);");
    Ins(&R, *ns.inl, "    return *this;");
    Ins(&R, *ns.inl, "}");
    Ins(&R, *ns.inl, "");

    Ins(&R, *ns.inl, "inline  cstring::cstring(const strptr &rhs) {");
    Ins(&R, *ns.inl, "    algo::cstring_Init(*this);");
    Ins(&R, *ns.inl, "    algo::ch_Addary(*this, aryptr<char>((char*)rhs.elems, rhs.n_elems));");
    Ins(&R, *ns.inl, "}");
    Ins(&R, *ns.inl, "");

    Ins(&R, *ns.inl, "inline cstring::cstring(const tempstr &rhs) {");
    Ins(&R, *ns.inl, "    ch_elems = rhs.ch_elems;");
    Ins(&R, *ns.inl, "    ch_n     = rhs.ch_n;");
    Ins(&R, *ns.inl, "    ch_max   = rhs.ch_max;");
    Ins(&R, *ns.inl, "    cstring &r  = (cstring&) rhs;");
    Ins(&R, *ns.inl, "    r.ch_elems     = 0;");
    Ins(&R, *ns.inl, "    r.ch_n   = 0;");
    Ins(&R, *ns.inl, "    r.ch_max = 0;");
    Ins(&R, *ns.inl, "}");
    Ins(&R, *ns.inl, "");
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
        if (inst.p_ctype->p_ns == ctype.p_ns && !inst.p_reftype->isval && GlobalQ(*inst.p_ctype)) {
            str_glob << "// global access: " << name_Get(inst) << " ("<< inst.reftype<<")"<<eol;
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
    amc::FFunc *init = amc::ind_func_Find(tempstr() << ctype.ctype << "..Init");
    amc::FFunc *uninit = amc::ind_func_Find(tempstr() << ctype.ctype << "..Uninit");
    bool pooled = PoolHasAllocQ(ctype);
    bool priv_ctor = pooled;
    // default constructor
    if (priv_ctor) {
        Ins(&R, *ns.hdr, "private:");
        ind_beg(amc::ctype_zd_inst_curs,inst,ctype) if (inst.p_reftype->hasalloc) {
            ind_beg(amc::field_c_ffunc_curs,func,inst) if (func.isalloc) {
                *ns.hdr<<"    ";
                GenFuncProto(ns,func,false,true);
            }ind_end;
        }ind_end;
    }
    Ins(&R, *ns.hdr, "    $Name();");
    Ins(&R, *ns.inl, "inline $Cpptype::$Name() {");
    // call init function
    if (init) {
        Ins(&R, *ns.inl, "    $ns::$Name_Init(*this);");
    }
    // produce coverity annotation for intentionally uninitialized fields
    ind_beg(amc::ctype_c_field_curs,field,ctype) {
        bool has_uninitfld = false;
        has_uninitfld     |= field.reftype == dmmeta_Reftype_reftype_Fbuf;
        has_uninitfld     |= field.reftype == dmmeta_Reftype_reftype_Inlary
            && field.c_inlary->max > field.c_inlary->min;
        if (has_uninitfld) {
            Set(R,"$field",field.field);
            Set(R,"$reftype",field.reftype);
            Ins(&R, *ns.inl, "    // added because $field ($reftype) does not need initialization");
            Ins(&R, *ns.inl, "    // coverity[uninit_member]"); // applies to '}', should be last
            break;
        }
    }ind_end;
    Ins(&R, *ns.inl, "}\n");
    // generate destructor -- if the uninit function was generated.
    if (uninit) {
        Ins(&R, *ns.hdr, "    ~$Name();");
        Ins(&R, *ns.inl, "inline $ns::$Name::~$Name() {");
        Ins(&R, *ns.inl, "    $ns::$Name_Uninit(*this);");
        Ins(&R, *ns.inl, "}\n");
    }
    // make copy constructor private if:
    // - any x-references exist for this ctype
    // - copy_priv is requested
    if (ctype.n_xref > 0 || CopyPrivQ(ctype)) {
        if (!priv_ctor) {
            Ins(&R, *ns.hdr, "private:");
        }
        int n=0;
        ind_beg(Line_curs,line,ctype.copy_priv_reason) {
            if (ch_N(line)) {
                Ins(&R, *ns.hdr, tempstr()<<"    // "<<line);
            }
            if (++n >= 3) {
                Ins(&R, *ns.hdr, tempstr()<<"    // ... and several other reasons");
                break;
            }
        }ind_end;
        Ins(&R, *ns.hdr, "    $Name(const $Name&){ /*disallow copy constructor */}");
        Ins(&R, *ns.hdr, "    void operator =(const $Name&){ /*disallow direct assignment */}");
    }
    // make main cosntructor private if:
    // there is a creation path, but no Cppstack access path!
}

// -----------------------------------------------------------------------------

static bool HasCcmpOpQ(amc::FCtype &ctype) {
    return ctype.c_ccmp && ctype.c_ccmp->genop;
}

// -----------------------------------------------------------------------------

static void GenStruct_Op(algo_lib::Replscope &R, amc::FCtype& ctype) {
    amc::FNs& ns = *ctype.p_ns;
    bool gen_cmpop = HasCcmpOpQ(ctype);
    if (gen_cmpop && amc::ind_func_Find(tempstr()<<ctype.ctype<<"..Eq")) {
        Ins(&R, *ns.hdr, "    bool operator ==(const $Cpptype &rhs) const;");
        Ins(&R, *ns.inl, "");
        Ins(&R, *ns.inl, "inline bool $Cpptype::operator ==(const $Cpptype &rhs) const {");
        Ins(&R, *ns.inl, "    return $Cpptype_Eq(const_cast<$Cpptype&>(*this),const_cast<$Cpptype&>(rhs));");
        Ins(&R, *ns.inl, "}");
    }
    if (gen_cmpop && amc::ind_func_Find(tempstr()<<ctype.ctype<<"..EqStrptr")) {
        Ins(&R, *ns.hdr, "    bool operator ==(const algo::strptr &rhs) const;");
        Ins(&R, *ns.inl, "");
        Ins(&R, *ns.inl, "inline bool $Cpptype::operator ==(const algo::strptr &rhs) const {");
        Ins(&R, *ns.inl, "    return $Cpptype_EqStrptr(const_cast<$Cpptype&>(*this),rhs);");
        Ins(&R, *ns.inl, "}");
    }
    if (gen_cmpop && amc::ind_func_Find(tempstr()<<ctype.ctype<<"..Lt")) {
        Ins(&R, *ns.hdr, "    bool operator <(const $Cpptype &rhs) const;");
        Ins(&R, *ns.inl, "");
        Ins(&R, *ns.inl, "inline bool $Cpptype::operator <(const $Cpptype &rhs) const {");
        Ins(&R, *ns.inl, "    return $Cpptype_Lt(const_cast<$Cpptype&>(*this),const_cast<$Cpptype&>(rhs));");
        Ins(&R, *ns.inl, "}");
    }
    // If enum comparison exists, then generate operator for it.
    // Ignore cmpop setting here
    if (amc::ind_func_Find(tempstr()<<ctype.ctype<<"..EqEnum")) {
        Set(R, "$Type", Enumtype(*c_datafld_Find(ctype,0)));
        Ins(&R, *ns.hdr, "    bool operator ==($Type rhs) const;");
        Ins(&R, *ns.inl, "");
        Ins(&R, *ns.inl, "inline bool $Cpptype::operator ==($Type rhs) const {");
        Ins(&R, *ns.inl, "    return $Cpptype_EqEnum(const_cast<$Cpptype&>(*this),rhs);");
        Ins(&R, *ns.inl, "}");
    }
}

// -----------------------------------------------------------------------------

// Same as algo::Tabulated, but indent at most MAXGROUP lines at a time
static tempstr ChunkyTabulated(strptr in, strptr sep, strptr fmt, int colspace, int maxgroup) {
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
    amc::InsertIndent(*ns.hdr, Tabulated(ctype.body, "\t", "ll", 2), 1);
    bool gen_ctor = ctype.c_cpptype && ctype.c_cpptype->ctor;
    if (gen_ctor) {
        GenStruct_Ctor(ctype);
    }
    if (ctype.ctype == "algo.cstring") {
        GenStruct_Cstring(R,ctype);
    }
    if (true) {
        GenStruct_Op(R,ctype);
    }
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
