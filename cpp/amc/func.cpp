// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
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
// Source: cpp/amc/func.cpp -- C++ function output
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Add argument to the most recent function call/declaration inside given string.
// TEXT: AddCallArg("abcd();","x") -> "abcd(x);"
// TEXT: AddCallArg("abcd(y) abc","x") -> "abcd(y, x) abc"
// Experimental, helps in readability: If CONDITION is false, do nothing
void amc::AddArg(cstring &text, strptr cppexpr, bool condition) {
    if (condition) {
        strptr left = Pathcomp(text,")RL");
        tempstr right(Pathcomp(text,")RR"));
        text.ch_n = left.n_elems;
        if (!EndsWithQ(left,"(")) {
            text << ", ";
        }
        text << cppexpr;
        text << ")";
        text << right;
    }
}

// Same but without conditions
void amc::AddArg(cstring &text, strptr cppexpr) {
    AddArg(text,cppexpr,true);
}

// -----------------------------------------------------------------------------

static char LastChar(strptr s, char dflt) {
    return s.n_elems > 0 ? s.elems[s.n_elems-1] : dflt;
}

static tempstr CombineTypeName(strptr type, strptr name) {
    tempstr text;
    text<<type;
    if (EndsWithQ(type," **") || EndsWithQ(type," &") || EndsWithQ(type," *") || EndsWithQ(type," ")) {
    } else {
        text<<' ';
    }
    text << name;
    return text;
}

// Add prototype arg to function FUNC
// The argument type is TYPE, and name is NAME
// Argument is added only if CONDITION is true
amc::Funcarg* amc::AddProtoArg(amc::FFunc &func, strptr type, strptr name, bool condition) {
    amc::Funcarg* ret=NULL;
    if (condition) {
        tempstr expr=CombineTypeName(type,name);
        AddArg(func.proto,expr,condition);
        amc::Funcarg &funcarg = funcarg_Alloc(func);
        funcarg.type = type;
        funcarg.name = name;
        ret=&funcarg;
    }
    return ret;
}

// Same but without condition
amc::Funcarg* amc::AddProtoArg(amc::FFunc &func, strptr type, strptr name) {
    return AddProtoArg(func,type,name,true);
}

// -----------------------------------------------------------------------------

// replace 'tdm_lib.FLevel*' with 'tdm_lib.FLevel *'
static void PrintDecl(strptr type, strptr name, cstring &out) {
    out << type;
    char c=LastChar(out, ' ');
    if (c == '*' || c == '&') {
        out.ch_n--;
        out << ' ' << c;
    } else {
        out << ' ';
    }
    out << name;
}

// -----------------------------------------------------------------------------

// Declare return value for function FUNC.
// Return types are stored in Funcarg array, and marked with RETVAL=true
// The value has type TYPE, name NAME, and is initialized with INITALIZER.
// If NAME is not an empty string, a variable with this name and type is also declared.
// The call to "return <retval>" is automatically inserted during function "finalize" step
// If INITIALIZER is "", the value is declared "as-is"
//    TYPE NAME;
// Otherwise, equals sign is used:
//    TYPE NAME = INITIALIZER;
amc::Funcarg* amc::AddRetval(amc::FFunc &func, strptr type, strptr name, strptr initializer) {
    amc::Funcarg &funcarg = funcarg_Alloc(func);
    funcarg.type = type;
    funcarg.name = name;
    funcarg.retval = true;
    func.ret=type;
    if (!funcarg.initialized && funcarg.name != "" && func.ret != "void") {
        PrintDecl(func.ret,funcarg.name,func.body);
        if (initializer == "") {
        } else {
            func.body<< " = "<<initializer;
        }
        func.body<<";"<<eol;
        funcarg.initialized=true;
    }
    return &funcarg;
}

// -----------------------------------------------------------------------------

// Add a type argument to function FUNC,
// TYPE is a c++ expression.
// Function becomes a template.
amc::Funcarg* amc::AddTypearg(amc::FFunc &func, strptr type) {
    amc::Funcarg &funcarg = funcarg_Alloc(func);
    funcarg.type = type;
    funcarg.typearg = true;
    func.istmpl = true;
    return &funcarg;
}

// -----------------------------------------------------------------------------

// Finalize function. This is done just before printing the function out.
// If the function has a return value, "return RETVAL" is added to the function body
// "MaybeUnused" is issued for all arguments (this is just a heuristic but it doesn't hurt)
// so that if any arguments were unused in the body, compiler error doesn't result
static void FinalizeFunc(amc::FFunc &func) {
    ind_beg(amc::func_funcarg_curs,funcarg,func) {
        if (funcarg.name != "" && funcarg.type != "void" && !funcarg.typearg) {
            MaybeUnused(func,funcarg.name);
        }
    }ind_end;
    ind_beg(amc::func_funcarg_curs,funcarg,func) {
        if (funcarg.retval && funcarg.name != "" && funcarg.type != "void") {
            func.body << "return "<<funcarg.name<<";" << eol;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::SetPresent(amc::FFunc &func, strptr ref, amc::FField &field) {
    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        func.body << name_Get(field)<<"_Set"<<pmaskfld_member.p_pmaskfld->funcname<<"("<<ref<<");" << eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Create function for current field (amc::_db.genctx.p_field) & tfunc (amc::_db.genctx.p_tfunc)
// The function name is constructed from the field name and the tfunc name, e.g. "field_FuncName"
// unless an explicit function name is passed through argument FUNCNAME.
//
// Return a reference to the resulting FUNC record
// The following options are set in FUNC based on the TFUNC prototype:
//     glob, wur, inl, globns, isalloc, nothrow, ismacro, pure.
// The function comment is initialized from tfunc prototype
// Function is DISABLED if namespace has exceptions disabled and the tfunc is labeled HASTHROW.
// If PROTO flag is set, function prototype (FUNC.PROTO) string is initialized to the function name,
// and a default first argument is added unless the function is on a global field.
amc::FFunc &amc::CreateCurFunc(bool proto DFLTVAL(false), algo::strptr funcname DFLTVAL("")) {
    amc::FTfunc &tfunc = *amc::_db.genctx.p_tfunc;
    amc::FField *field=amc::_db.genctx.p_field;
    amc::FCtype *ctype=amc::_db.genctx.p_ctype;
    amc::FNs *ns=amc::_db.genctx.p_ns;
    algo::Smallstr50 name = funcname == "" ? strptr(name_Get(tfunc)) : funcname;
    tempstr key;
    if (field) {
        key << field->field << ".";
    } else if (ctype) {
        key << ctype->ctype << "..";
    } else {
        key << ns->ns << "...";
    }
    key << name;
    amc::FFunc  &func = amc::ind_func_GetOrCreate(key);
    func.glob         = !field;
    func.wur          = tfunc.wur;     // copy warn-unused-result flag
    func.inl          = tfunc.inl;     // default
    func.globns       = !field && ns->ns=="";
    func.isalloc      = tfunc.poolfunc;
    func.nothrow      = !tfunc.hasthrow && tfunc.leaf;
    func.ismacro      = tfunc.ismacro;
    func.pure         = tfunc.pure;
    if (ch_N(tfunc.comment.value)>0) {
        func.comment = tempstr()<<tfunc.comment<<"\n";
    }
    func.disable   = tfunc.hasthrow && !GenThrowQ(*func.p_ns);
    if (proto) {
        if (field) {
            if (field->reftype != dmmeta_Reftype_reftype_Global) {
                // function name based on field
                func.proto << name_Get(*field);
            }
        } else if (ctype) {
            if (!GlobalQ(*ctype) && funcname == "") {
                // function name based on ctype
                func.proto << name_Get(*ctype);
            }
        } else {
            func.proto << ns->ns;
        }
        if (func.proto != "") {
            func.proto<<"_";
        }
        func.proto<<name<<"()";
        // add first argument
        if (field && ctype && !GlobalQ(*ctype)) {
            AddProtoArg(func, Refto(ctype->cpp_type), Refname(*ctype));
        }
    }
    return func;
}

// -----------------------------------------------------------------------------

// Return non-null pointer to ctype's init function.
amc::FFunc *amc::init_GetOrCreate(amc::FCtype &ctype) {
    amc::FFunc *ret = ctype.c_init;
    if (!ret) {
        algo_lib::Replscope R;
        tempstr refname = amc::Refname(ctype);
        bool glob = GlobalQ(ctype);
        Set(R, "$Parent" , glob ? algo::strptr("") : algo::strptr(tempstr() << ctype.cpp_type << "& " << refname));
        Set(R, "$Name"   , name_Get(ctype));
        ret = &amc::ind_func_GetOrCreate(tempstr() << ctype.ctype << "..Init");
        Ins(&R, ret->comment, "Set all fields to initial values.");
        Ins(&R, ret->ret  , "void", false);
        Ins(&R, ret->proto, "$Name_Init($Parent)", false);
        ret->inl = true; // start life as inline
        ret->glob = true;
        ctype.c_init = ret;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void PrintAttrs(amc::FFunc &func, cstring &out) {
    int start = out.ch_n;
    int nattr = 0;
    algo::ListSep ls;
    out << " __attribute__((";
    if (func.deprecate) {
        out << ls << "deprecated";
        nattr++;
    }
    if (func.wur) {
        out << ls << "__warn_unused_result__";
        nattr++;
    }
    if (func.nothrow) {// gcc interprets this as "no throw and no catch"!
        out << ls << "nothrow";
        nattr++;
    }
    if (func.pure) {
        out << ls << "pure";
        nattr++;
    }
    if (ch_N(func.nonnull)) {
        out << ls << "__nonnull__ ("<<func.nonnull<<")";
        nattr++;
    }
    out << "))";
    if (nattr==0) {
        out.ch_n=start;// undo  if there were no attributes
    }
}

// -----------------------------------------------------------------------------

static void TmplPrefix(amc::FFunc& func, cstring &out, bool proto) {
    if (func.istmpl) {
        algo::ListSep ls(", ");
        out << "template<";
        ind_beg(amc::func_funcarg_curs,funcarg,func) {
            if (funcarg.typearg) {
                out << ls << "class "<<funcarg.type;
            }
        }ind_end;
        out << "> ";
    }
    if (func.inl && !func.extrn) {
        out << "inline ";
    }
    if (func.priv && (proto ? true : !func.glob)) {
        out << "static ";
    }
}

// -----------------------------------------------------------------------------

// Emit function prototype into string OUT.
// If ctype_context is specified, then the declaration is intended to be
// used inside the struct, so different C++ syntax rules apply.
void amc::PrintFuncProto(amc::FFunc& func, amc::FCtype *ctype_context, cstring &out) {
    if (func.prepcond != "") {
        out << "#if " << func.prepcond << eol;
    }
    // show function comment but omit it if it's just a friend declaration
    if (!(ctype_context && !func.member)) {
        ind_beg(Line_curs,comment,func.comment) {
            out << "// "<<comment<<'\n';
        }ind_end;
        out << "// func:"<<func.func<<eol;
    }
    if (func.extrn) {
        out << "// this function is 'extrn' and implemented by user"<<eol;
    }
    if (ctype_context && !func.member) {
        out << "friend ";
    }
    if (ctype_context && func.isexplicit) {
        out << "explicit ";
    }
    int start = ch_N(out);
    TmplPrefix(func,out,true);
    out << func.ret;
    // some indentation for readability
    char_PrintNTimes(' ', out, start + 20 - ch_N(out));
    out << " ";
    if (!ctype_context && func.member) {
        if (amc::FCtype *ctype = ind_ctype_Find(ctype_Get(func))) {
            out << ctype->cpp_type << "::";
        }
    }
    out << func.proto;
    if (func.isconst) {
        out <<"const ";
    }
    if (func.deleted) {
        out <<" = delete";
    } else {
        PrintAttrs(func,out);
    }
    out << ";";
    out << eol;
    if (func.prepcond != "") {
        out << "#endif" << eol;
    }
}

// -----------------------------------------------------------------------------

void amc::PrintFuncBody(amc::FNs& ns, amc::FFunc& func) {
    if (!func.extrn && !func.deleted) {
        cstring &impl = (func.inl && !func.priv) ? *ns.inl : *ns.cpp;

        if (bool_Update(func.finalized,true)) {
            FinalizeFunc(func);
        }
        amc::CppSection(impl, func.func, false);
        ind_beg(Line_curs,comment,func.comment) {
            impl << "// "<<comment<<'\n';
        }ind_end;

        tempstr proto;
        TmplPrefix(func,proto,false);
        proto << func.ret << " ";
        if (func.member) {
            if (amc::FCtype *ctype = ind_ctype_Find(ctype_Get(func))) {
                proto << ctype->cpp_type << "::";
            }
        } else if (!func.globns) {// namespace
            if (ch_N(ns.ns)) {
                proto << ns.ns << "::";
            }
        }
        proto << func.proto;
        // this is here to appease clang. can't output __attribute__ here.
        if (func.nothrow) {
            proto << " throw()";
        }
        if (ary_N(func.initializer)) {
            ind_beg(algo::StringAry_ary_curs,initializer,func.initializer) {
                proto << (ind_curs(initializer).index==0 ? "\n    : " : "\n    , ");
                proto << initializer;
            }ind_end;
            proto << eol;
        }
        proto << " {";
        if (func.ismacro) {
            proto << " // internal, automatically inlined";
        }
        if (func.prepcond != "") {
            impl << "#if " << func.prepcond << eol;
        }
        impl << proto << eol;
        algo::InsertIndent(impl, func.body, 1);
        impl << "}" << eol;
        if (func.prepcond != "") {
            impl << "#endif" << eol;
        }
    }
}

// -----------------------------------------------------------------------------

bool amc::SetRetType(amc::FFunc &func, amc::FCtype &ctype) {
    func.ret = ctype.cpp_type;
    return true;
}

// -----------------------------------------------------------------------------

void amc::MaybeUnused(amc::FFunc &func, strptr name) {
    bool used = FindStr(func.body, name, true)!=-1;
    if (!used) {
        ind_beg(algo::StringAry_ary_curs,initializer,func.initializer) {
            if (Pathcomp(initializer,"(LR)RL") == name) {
                used=true;
                break;
            }
        }ind_end;
    }
    if (!used) {
        func.body <<"(void)"<<name<<";//only to avoid -Wunused-parameter" << eol;
    }
}

// -----------------------------------------------------------------------------

amc::Funcarg* amc::FindArg(amc::FFunc &func, strptr name) {
    amc::Funcarg *ret=NULL;
    ind_beg(amc::func_funcarg_curs,arg,func) {
        if (arg.name == name) {
            ret=&arg;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Create function marked as inline,nothrow,ret.
amc::FFunc &amc::CreateInlineFunc(algo::strptr name) {
    amc::FFunc &ret = ind_func_GetOrCreate(name);
    ret.inl = true;
    ret.nothrow=true;
    return ret;
}
