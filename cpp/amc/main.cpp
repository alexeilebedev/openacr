// Copyright (C) 2023-2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/main.cpp -- Main driver
//
// Algo Model Compiler (AMC)

#include "include/amc.h"
#include "include/gen/report_gen.h"

// -----------------------------------------------------------------------------

// Query whether a value of CTYPE has to be destroyed rather than simply
// forgotten: whether generated code that stops using one must call a destructor
// for it.
// See gen_plaindata, which computes the flag from what the ctype contains -- an
// owning field, a Cleanup callback, a cascdel dependent, or an index the record
// must unlink itself from. That list is what a generated destructor does, so a
// ctype containing none of it has nothing to destroy.
bool amc::HasDtorQ(amc::FCtype &ctype) {
    return ctype.has_dtor;
}

// -----------------------------------------------------------------------------

//
bool amc::CanCopyQ(amc::FCtype &ctype) {
    amc::FCtype *fldbase = GetBaseType(ctype,NULL);
    bool can_copy = fldbase && zd_varlenfld_EmptyQ(*fldbase) && zd_varlenfld_EmptyQ(ctype);
    if (fldbase) {
        ind_beg(amc::ctype_c_field_curs, fld, *fldbase) {
            if (ValQ(fld)) {
                can_copy &= !CopyPrivQ(*fld.p_arg);
            }
        }ind_end;
    }
    return can_copy;
}

// -----------------------------------------------------------------------------

// Return TRUE if field is a padding field with no semantic significance
bool amc::PadQ(amc::FField &field) {
    return field.arg == "pad_byte";
}

// -----------------------------------------------------------------------------

bool amc::CanDeleteQ(amc::FCtype &ctype) {
    amc::FField *inst=FirstInst(ctype);
    return
        !GlobalQ(ctype)
        && inst
        && inst->p_reftype->del
        && !inst->c_fnoremove;
}

// -----------------------------------------------------------------------------

// Check if this field is a Lenfld and return pointer
// If not, return NULL
amc::FLenfld *amc::GetLenfld(amc::FField &field) {
    amc::FLenfld *lenfld = field.p_ctype->c_lenfld;
    return lenfld && lenfld->p_field == &field ? lenfld : NULL;
}

// -----------------------------------------------------------------------------

// Compose expression for deleting child pointer CHILDREF
// corresponding to FIELD, in the scope of parent PARENTREF.
tempstr amc::DeleteExpr(amc::FField &field, strptr parentref, strptr childref) {
    amc::FField *inst = FirstInst(*field.p_arg);
    tempstr ret;
    if (inst) {
        bool global = GlobalQ(*inst->p_ctype);
        ret << name_Get(*inst) << "_Delete()";
        if (!global) {
            AddArg(ret, parentref);
        }
        AddArg(ret,childref);
    }
    return ret;
}

// -----------------------------------------------------------------------------

amc::FField *amc::GetViafld(amc::FXref &xref) {
    amc::FField *field=NULL;
    if (xref.p_viafld != xref.p_keyfld && xref.p_viafld->field != "") {
        field=xref.p_viafld;
    }
    return field;
}

// -----------------------------------------------------------------------------

amc::FField *amc::GetKeyfld(amc::FXref &xref) {
    amc::FField *field=NULL;
    if (xref.p_keyfld->field != "") {
        field=xref.p_keyfld;
    }
    return field;
}

// -----------------------------------------------------------------------------

void amc::BeginNsBlock(cstring &out, amc::FNs &ns, strptr tag) {
    tempstr comment;
    if (tag == "") {
        comment << "gen:"<<amc::_db.cur_gen;
    } else {
        comment = tag;
    }
    if (ch_N(ns.ns)) {
        out<<"namespace "<<ns.ns<<" { // "<<comment<<eol;
    }
}

// -----------------------------------------------------------------------------

void amc::EndNsBlock(cstring &out, amc::FNs &ns, strptr tag) {
    tempstr comment;
    if (tag == "") {
        comment << "gen:"<<amc::_db.cur_gen;
    } else {
        comment = tag;
    }
    if (ch_N(ns.ns)) {
        out<<"} // "<<comment << eol;
    }
}

// -----------------------------------------------------------------------------

bool amc::PtrQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Ptr
        || field.reftype == dmmeta_Reftype_reftype_Upptr;
}

// -----------------------------------------------------------------------------

// make sure the specified type is forward-declared
// in given namespace.
void amc::AddFwdDecl(amc::FNs &ns, amc::FCtype &ctype) {
    // skip extern types that are not structs (e.g. typedefs like algo.strptr)
    bool dominated = ctype.c_cextern && !ctype.c_cextern->isstruct;
    if (ns_Get(ctype) != "" && !dominated) {
        amc::ind_fwddecl_GetOrCreate(tempstr()<<ns.ns<<"."<<ctype.ctype);
        amc::c_cppincl_ScanInsertMaybe(ns, *ctype.p_ns);
    }
}

// -----------------------------------------------------------------------------

tempstr amc::Refto(strptr s) {
    return tempstr()<<s<<"&";
}

// -----------------------------------------------------------------------------

tempstr amc::ConstRefto(strptr s) {
    return tempstr()<<"const "<<s<<"&";
}

// -----------------------------------------------------------------------------

tempstr amc::Castto(strptr type, strptr expr) {
    tempstr ret;
    if (amc::IdentQ(type)) {
        ret << type;
    } else {
        ret << "(" << type << ")";
    }
    return ret << "(" << expr << ")";
}

// -----------------------------------------------------------------------------

bool amc::CastsToU64Q(amc::FField& field){
    amc::FCtype &ctype = *field.p_arg;
    return ctype.c_bltin && ctype.c_bltin->likeu64;
}

// True if ctype is instantiated through a memory pool.
bool amc::HasPoolQ(amc::FCtype &ctype) {
    bool ret=false;
    ind_beg(amc::ctype_zd_access_curs,access,ctype) {
        if (access.p_reftype->isval) {
            ret=true;
        }
    }ind_end;
    return ret;
}

bool amc::HasFinputsQ(amc::FNs &ns) {
    bool ret=false;
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
        amc::FField *inst = FirstInst(ctype);
        if (inst && inst->c_finput) {
            ret=true;
            break;
        }
    }ind_end;
    return ret;
}

tempstr amc::PkeyCppident(algo::Tuple &tuple) {
    tempstr ret;
    algo::Attr *attr = &attrs_qFind(tuple,0);
    amc::strptr_PrintCppIdent(attr->value,ret,true);
    return ret;
}

// -----------------------------------------------------------------------------

amc::FField *amc::FirstInst(amc::FCtype &ctype) {
    return zd_inst_First(ctype);
}

// -----------------------------------------------------------------------------

// Return TRUE if the given step executes directly from the scheduling Step function
// Indirect steps are those called through a Timehook.
bool amc::DirectStepQ(amc::FFstep &fstep) {
    return fstep.steptype == dmmeta_Steptype_steptype_Callback
        || fstep.steptype == dmmeta_Steptype_steptype_Extern
        || fstep.steptype == dmmeta_Steptype_steptype_Inline
        || fstep.steptype == dmmeta_Steptype_steptype_InlineOnce
        || fstep.steptype == dmmeta_Steptype_steptype_InlineRecur;
}

// -----------------------------------------------------------------------------

bool amc::ExternStepQ(amc::FFstep &fstep) {
    return fstep.steptype == dmmeta_Steptype_steptype_Callback
        || fstep.steptype == dmmeta_Steptype_steptype_Extern;
}

// -----------------------------------------------------------------------------

// add text to c++ body of class
void amc::InsStruct(algo_lib::Replscope &R, amc::FCtype *ctype, const algo::strptr &text) {
    algo_lib::Ins(&R,ctype->body,text);
}

// Insert variable declaration into body of struct corresponding to CTYPE,.
// NOTE: field must be explicitly initialized in Init function, this function
// will not provide initialization in constructor!
void amc::InsVar(algo_lib::Replscope &R, amc::FCtype *ctype, const algo::strptr &type, const algo::strptr &name, const algo::strptr &dflt, const algo::strptr &comment) {
    cstring &t = ctype->body;
    t << Subst(R,type)
      << " \t" << Subst(R,name) << ";"
      << " \t// ";
    if (dflt.n_elems > 0) {
        t << "  "<<dflt<<"  ";
    }
    t << Subst(R,comment);
    t << eol;
}

// True if ctype can be read from string
bool amc::ConstructFromStringQ(amc::FCtype &ctype) {
    // char can't be constructed from strptr, have to use _ReadStrptr
    return StringQ(ctype) && ctype.ctype != "char";
}

bool amc::HasRemoveLastQ(amc::FReftype &reftype) {
    return amc::ind_tfunc_Find(amcdb::Tfunc_Concat_tclass_name(reftype.reftype,"RemoveLast")) != NULL;
}

bool amc::GenThrowQ(amc::FNs &ns) {
    return !ns.c_nsx  || ns.c_nsx->genthrow;
}

// -----------------------------------------------------------------------------

amc::FThash *amc::PrimaryIndex(amc::FCtype &ctype) {
    amc::FThash *idx=NULL;
    ind_beg(amc::ctype_zs_xref_curs,xref,ctype) {
        bool good = xref.p_field->reftype == dmmeta_Reftype_reftype_Thash;
        good &= GlobalQ(*xref.p_field->p_ctype);
        good &= xref.inscond.value == "true";
        if (good) {
            idx=xref.p_field->c_thash;
            break;
        }
    }ind_end;
    return idx;
}

// -----------------------------------------------------------------------------

// Compute expression needed to cast the default value (field.dflt) of
// the field to the value that's being stored in the field
// By default, this is just the field's cpp_type
// But if the field has an fcast attached to it, it's the fcast expression
tempstr amc::Initcast(amc::FField &field) {
    amc::FCtype& valtype = *(field).p_arg;
    tempstr cast_type;
    // cast type
    if (c_field_N(valtype)>0 && c_field_Find(valtype,0)->c_fcast) {
        cast_type = c_field_Find(valtype,0)->c_fcast->expr;
    }
    if (!ch_N(cast_type)) {
        cast_type = valtype.cpp_type;
    }
    bool ident = amc::IdentQ(cast_type);
    if (!ident) {
        cast_type = tempstr() << "(" << cast_type << ")";
    }
    return cast_type;
}

// -----------------------------------------------------------------------------

// Emit a statement printing FIELD of the record named PARENTNAME into the
// string named STRNAME: the field's own Print function when one exists
// (parent argument collapsed by ParentArgExpr when the parent is a global),
// otherwise the field type's Print applied to the field value.
void amc::GenPrintStmt(cstring &out, amc::FCtype &parenttype, amc::FField &field, strptr strname, strptr parentname) {
    if (amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
        vrfy(!func->ismacro, tempstr()<<"invalid macro print function "<<func->func);
        out << ns_Get(*field.p_ctype) << "::"
            << name_Get(field)
            << "_Print("<<ParentArgExpr(*field.p_ctype,parentname,true)<<strname<<");\n";
    } else {
        out << name_Get(*field.p_arg)
            <<"_Print("<<FieldvalExpr(&parenttype,field,parentname) <<", "<<strname<<");\n";
    }
}

// -----------------------------------------------------------------------------

bool amc::FldfuncQ(amc::FField &field) {
    return field.c_cppfunc || field.c_substr || field.c_falias;
}

bool amc::PoolVarlenQ(amc::FField &field) {
    bool pool_varlen  = field.reftype == dmmeta_Reftype_reftype_Lpool;
    pool_varlen      |= field.reftype == dmmeta_Reftype_reftype_Malloc;
    pool_varlen      |= field.reftype == dmmeta_Reftype_reftype_Blkpool;
    return pool_varlen;
}

bool amc::BlkpoolQ(amc::FField &field) {
    bool ret = field.reftype == dmmeta_Reftype_reftype_Blkpool;
    return ret;
}

bool amc::FieldStringQ(amc::FField &field) {
    return field.p_arg->c_cstr && field.arg != "char";
}

bool amc::FieldNumericQ(amc::FField &field) {
    return !FieldStringQ(field);
}

// Convert constant string to case label
// RHS       string to convert (1..8 chars)
// RETURN    constant string converted to switch case label
// !!! experimental -  export from amc_enum.cpp to amc_pool.cpp
tempstr amc::ConstStringToCaseLabel(strptr rhs) {
    vrfy_(rhs.n_elems >  0 && rhs.n_elems <= 8);
    tempstr ret;
    if (rhs.n_elems == 1) {
        char_PrintCppSingleQuote(rhs[0], ret);
    } else {
        ret << "LE_STR" << rhs.n_elems << "(" ;
        algo::ListSep ls(",");
        for (int i=0; i<rhs.n_elems; i++) {
            ret << ls;
            char_PrintCppSingleQuote(rhs[i],ret);
        }
        ret << ")";
    }
    return ret;
}

// Convert variable string to to switch value
// NAME          variable name
// LEN           length of string (1..8 chars)
// RETURN        variable string converted to switch value

tempstr amc::VarStringToInteger(strptr name, i32 len) {
    vrfy_(len >  0);
    vrfy_(len <= 8);
    tempstr ret;
    u32 offset = 0 ;
    algo::ListSep ls("|");
    while (len) {
        tempstr off1;
        tempstr off8;
        tempstr x;
        if (offset) {
            off1 << "+" << offset;
            off8 << "<<"<< offset*8;
        }
        if        (len >= 8) { x <<     "algo::ReadLE64("<< name << ".elems"<<off1<<")";  len -= 8; offset += 8;
        } else if (len >= 4) { x << "u64(algo::ReadLE32("<< name << ".elems"<<off1<<"))"; len -= 4; offset += 4;
        } else if (len >= 2) { x << "u64(algo::ReadLE16("<< name << ".elems"<<off1<<"))"; len -= 2; offset += 2;
        } else if (len >= 1) { x << "u64("<< name << "["<<offset<<"])";             len -= 1; offset += 1;
        }
        if (off8.ch_n > 0) {
            ret << ls << "(" << x << off8 << ")";
        } else {
            ret << ls << x ;
        }

    }
    return ret;
}

// -----------------------------------------------------------------------------

// The builtin CTYPE stands for, as a bltin id: a ctype whose single field is
// a Val stands for whatever that field holds, so algo.Uint32 -- one Val field
// over u32 -- stands for u32, and a chain of such wrappers stands for the type
// at its end. True when the chain ends at a builtin amc knows by name, false
// when it ends at a struct. The name is the authority, not the amcdb.bltin
// row: a universe that omits the row still names the builtin the same way.
// A chain of such wrappers can also close on itself -- a ctype whose one Val
// field names the ctype itself, or a pair that name each other -- and a walk
// of it would never reach an end. The descent is therefore capped, and a
// chain that reaches the cap names no builtin, which is what the callers
// already report field by field before the size computation reports the
// cycle: test/amc/bad_size_cycle.ssim carries a cycle of each shape with a
// decimal field on it, so the run stays a diagnostic and never a hang.
bool amc::GetBltinId(amc::FCtype& _ctype, amc::BltinId &bltin_id) {
    amc::FCtype *ctype = &_ctype;
    int niter = 0;
    while (niter < 100 && c_field_N(*ctype) == 1 && c_field_Find(*ctype,0)->reftype == dmmeta_Reftype_reftype_Val) {
        ctype = c_field_Find(*ctype,0)->p_arg;
        niter++;
    }
    bool retval = false;
    if (niter < 100) {
        retval = value_SetStrptrMaybe(bltin_id,ctype->ctype);
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Why a value of the integer builtin CTYPE stands for cannot move in and out
// of CTYPE, as a sentence naming what is missing; empty when the value moves
// both ways.
// A field declared with a wrapper ctype -- one whose single Val field holds the
// builtin -- stores the wrapper, while the range checks, scale factors and
// parse results the numeric generators build are arithmetic on the builtin. So
// every such generator converts in both directions. Out of the wrapper the
// conversion is the operator dmmeta.fcast generates. Into it the conversion is
// the fieldwise constructor, which the ctype gets from a dmmeta.cpptype row
// asking for a constructor. A ctype carrying only one of the two takes the
// value in one direction while the generated code names the other anyway, so
// the answer names the one that is missing and the generator reports the field
// rather than emitting C++ that does not compile.
// The fieldwise constructor takes one argument per field, of that field's own
// type, so it converts from the builtin only when the builtin is what the one
// field holds. A chain of two wrappers converts from the inner wrapper instead,
// and is named as standing for the builtin only through another ctype. The
// builtin itself needs no conversion at all and is the empty answer.
tempstr amc::BadBltinCast(amc::FCtype &ctype) {
    tempstr retval;
    amc::BltinId bltin_id(amc_BltinIdEnum(0));
    if (!value_SetStrptrMaybe(bltin_id, ctype.ctype)) {
        amc::FField *field = c_field_N(ctype) == 1 ? c_field_Find(ctype,0) : NULL;
        bool direct = field != NULL
            && field->reftype == dmmeta_Reftype_reftype_Val
            && value_SetStrptrMaybe(bltin_id, field->p_arg->ctype);
        if (!direct) {
            retval << "ctype stands for the builtin only through another ctype; one Val field over the builtin converts";
        } else {
            tempstr need;
            algo::ListSep ls(" and ");
            if (!field->c_fcast) {
                need << ls << "dmmeta.fcast to read the builtin value out of it";
            }
            if (!amc::FieldwiseCtorQ(ctype)) {
                need << ls << "dmmeta.cpptype ctor:Y to construct one from a builtin value";
            }
            if (ch_N(need)) {
                retval << "ctype needs " << need;
            }
        }
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Numeric range of CTYPE as u64 bit patterns; CTYPE may wrap the bltin in
// a chain of single-Val-field ctypes. The resolved bltin id determines
// bounds and signedness alike: a signed integer named by a universe that
// lacks the type's amcdb.bltin row must still get signed range checks.
// The types with a range are the fixed-width integers, u8 through i64.
// Returns false for everything else -- bool, char, pad_byte, u128, float
// and double -- and hands back the widest signed bounds as a fallback:
// bool has only two values and cannot carry a magnitude, char's signedness
// is the compiler's choice, and amc has no arithmetic for the rest.
// Range checks built from the fallback bound nothing, so each generator
// that needs a range rejects the field before its checks are emitted
// (gen_check_lenfld for a length word, tclass_Dec for a fixed-point value,
// tclass_Numstr for a numeric string), and the run exits nonzero.
bool amc::GetMinMax(amc::FCtype& _ctype, u64 &min, u64 &max, bool &issigned) {
    amc::BltinId bltin_id(amc_BltinIdEnum(0));
    bool known = GetBltinId(_ctype, bltin_id);
    issigned = false;
    if (known) {
        switch (bltin_id.value) {
        case amc_BltinId_u8:      min = 0;                  max = 0xff;                               break;
        case amc_BltinId_i8:      min = 0xffffffffffffff80; max = 0x7f;               issigned=true;  break;
        case amc_BltinId_u16:     min = 0x00;               max = 0xffff;                             break;
        case amc_BltinId_i16:     min = 0xffffffffffff8000; max = 0x7fff;             issigned=true;  break;
        case amc_BltinId_u32:     min = 0x00000000;         max = 0xffffffff;                         break;
        case amc_BltinId_i32:     min = 0xffffffff80000000; max = 0x7fffffff;         issigned=true;  break;
        case amc_BltinId_u64:     min = 0x0000000000000000; max = 0xffffffffffffffff;                 break;
        case amc_BltinId_i64:     min = 0x8000000000000000; max = 0x7fffffffffffffff; issigned=true;  break;
        default:                  known = false;            break;
        }
    }
    if (!known) {
        min = 0x8000000000000000;
        max = 0x7fffffffffffffff;
        issigned = true;
    }
    return known;
}

// -----------------------------------------------------------------------------

// Query whether FIELD's type is passed by value; see gen_cheapcopy.
bool amc::CheapCopyQ(amc::FField &field) {
    return field.p_arg->cheap_copy;
}

bool amc::HasFcast(amc::FCtype& ctype){
    int result =false;
    ind_beg(amc::ctype_c_field_curs,field,ctype) if(field.c_fcast) {
        result=true;
    }ind_end;
    return result;
}

// Determine if ctype should have a private constructor
bool amc::CopyPrivQ(amc::FCtype &ctype) {
    bool copy_priv=false;
    vrfy(bool_Update(ctype.in_copy_priv,true),tempstr()<<"circular type definition in "<<ctype.ctype);
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (!copy_priv) {
        if (field.reftype == dmmeta_Reftype_reftype_Global) {
            copy_priv = true;
        } else if (field.p_reftype->isval) {
            if (CopyPrivQ(*field.p_arg)) {
                copy_priv = true;
            }
        } else if (field.reftype == dmmeta_Reftype_reftype_Regx || field.reftype == dmmeta_Reftype_reftype_RegxSql) {
        } else if (!field.p_reftype->cancopy) {
            copy_priv = true;
        } else if (field.c_xref != NULL) {
            copy_priv = true;
        } else if (field.c_cascdel != NULL) {
            copy_priv = true;
        } else if (amc::FindFfunc(field, amcdb_cbtype_Cleanup) != NULL) {
            copy_priv = true;
        } else if (amc::FindFfunc(field, amcdb_cbtype_Userinit) != NULL) {
            copy_priv = true;
        }
    }ind_end;
    ctype.in_copy_priv=false;
    return copy_priv;
}

// Return c++ type for rowid of FCtype
// By default this is a u64, but if the struct has a field marked rowid,
// then the type of that field is the row id type.
tempstr amc::EvalRowid(amc::FCtype &ctype) {
    tempstr rowid("u64");
    ind_beg(amc::ctype_c_field_curs,field,ctype) if (field.c_rowid) {
        if(CastsToU64Q(field) || HasFcast(*field.p_arg)){
            rowid = field.p_arg->cpp_type;
        }
    }ind_end;
    return rowid;
}

// True if given ctype is a global, i.e. it has only a single instance
// of type Global.
bool amc::GlobalQ(amc::FCtype &ctype) {
    bool retval=false;
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {
        if (inst.reftype == dmmeta_Reftype_reftype_Global) {
            retval=true;
            break;
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

// Append to OUT the instname of a ctype named NAME (the name with no namespace):
// the lower_under form of that name, with a leading F dropped.
// The F marks a ctype that lives in a pool and is not part of the name, so
// FTarget gives target.  A word boundary is a capital that follows a lowercase
// letter or a digit, which makes AbcDef into abc_def and H2Stream into
// h2_stream, while a run of capitals carries no boundary and ABCDef stays
// abcdef.
void amc::strptr_PrintInstname(algo::strptr name, algo::cstring &out) {
    bool poolmark = elems_N(name) > 1 && name.elems[0] == 'F' && algo_lib::UpperCharQ(name.elems[1]);
    algo::strptr_PrintLowerUnder(poolmark ? ch_RestFrom(name,1) : name, out);
}

// -----------------------------------------------------------------------------

// The name CTYPE contributes to a generated identifier -- a trace counter, a
// static hook function, a hook ctype, a ptrary membership field.
// Derived from the ctype's own name, so it does not depend on which fields
// happen to instantiate the ctype, or on the order those fields are declared in.
// Stored on the ctype: the derivation runs once per ctype and every later reader
// gets the field.  It runs on first read rather than in a generation pass
// because the two constraints on such a pass cannot both be met -- gen_trace
// composes an identifier from an instname and runs sixth, while gen_proc,
// gen_msgcurs and gen_clonefconst create ctypes of their own much later, and
// ResetVars asks for the instname of every one of them.
strptr amc::Instname(amc::FCtype &ctype) {
    if (!ch_N(ctype.instname)) {
        amc::strptr_PrintInstname(name_Get(ctype), ctype.instname);
    }
    return ctype.instname;
}

// -----------------------------------------------------------------------------

// The name of the variable that holds a record of CTYPE.
// A global ctype is held by the namespace global, whose name is the instname
// behind an underscore: _db.  Every other record is held by whatever field
// instantiates it, and that field is named after the ctype, so the instname
// names it.
// Identifiers that read as a path to a value are built from this rather than
// from the instname alone, because the underscore is part of the global's name:
// a trace counter over the malloc pool is _db.trace.alloc__db_malloc, and the
// metric layer recovers the pool's name by stripping the three leading
// components that spelling gives it.
tempstr amc::Varname(amc::FCtype &ctype) {
    tempstr ret;
    if (GlobalQ(ctype)) {
        ret << "_";
    }
    ret << Instname(ctype);
    return ret;
}

// -----------------------------------------------------------------------------

// The name a generated function body uses to refer to a record of CTYPE.
// A global ctype has exactly one record and the body names it directly, as the
// namespace global: the instname behind an underscore, which is _db.
// Every other record arrives as the function's first argument, and that
// argument is named parent in every generated function without exception.
// One fixed word is what makes the name safe, and a name derived from the ctype
// is not.  Consider lib_sqlite.FRow, whose body opens by aliasing its argument
// to row -- the name every generated body uses for the record it is about.  A
// parameter also called row would shadow that alias, so the schema names the
// parameter trow instead, and 33 ctypes in the tree would collide the same way
// if the parameter took the ctype's name.  A single reserved word cannot
// collide, and it also removes the need for two generators to agree: the Atree
// child Init and the ctype's own Init write the same parameter by construction
// rather than by both deriving the same string.
tempstr amc::Refname(amc::FCtype &ctype) {
    tempstr refname;
    if (GlobalQ(ctype)) {
        refname << Varname(ctype);
    } else {
        refname << "parent";
    }
    return refname;
}

// -----------------------------------------------------------------------------

// Returns TRUE if the field is an inline value, except for Varlen and Opt
bool amc::ValQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Val
        || field.reftype == dmmeta_Reftype_reftype_Inlary
        || field.reftype == dmmeta_Reftype_reftype_Smallstr;
}

// -----------------------------------------------------------------------------

bool amc::ComputedFieldQ(amc::FField &field) {
    return field.c_typefld || GetLenfld(field);
}

// -----------------------------------------------------------------------------

tempstr amc::char_ToCppSingleQuote(char c) {
    tempstr out;
    char_PrintCppSingleQuote(c,out);
    return out;
}

// -----------------------------------------------------------------------------

// return separator to be used for array field FIELD
// if not found, return 0 (NUL)
char amc::GetSep(amc::FField &field) {
    char ret = 0;
    amc::FCfmt *cfmt = FindStringRead(*field.p_ctype);
    if (cfmt && cfmt->sep != "") {
        ret = cfmt->sep.ch[0];
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Evaluate value of SSIM attribute as described by field FIELD
// given tuple TUPLE
tempstr amc::EvalAttr(Tuple &tuple, amc::FField &field) {
    tempstr ret;
    if (amc::FSubstr *substr = field.c_substr) {
        tempstr val(attr_GetString(tuple, name_Get(*substr->p_srcfield)));
        ret = Pathcomp(val, substr->expr.value);// ((a=Pathcomp(a,expr) would be an error)
    } else {
        ret = attr_GetString(tuple, name_Get(field));
    }
    return ret;
}

// -----------------------------------------------------------------------------

i32 amc::WidthMin(amc::FField &field) {
    i32 ret;
    if (field.c_inlary) {
        ret = field.c_inlary->min;
    } else {
        ret = 1;
    }
    return ret;
}

i32 amc::WidthMax(amc::FField &field) {
    i32 ret;
    if (field.c_inlary) {
        ret = field.c_inlary->max;
    } else {
        ret = 1;
    }
    return ret;
}

bool amc::IdentQ(strptr type) {
    bool ident = true;
    frep_(i,elems_N(type)) {
        ident = ident && (algo_lib::IdentCharQ(type[i]) || type[i] == ':');
    }
    return ident;
}

// Byte size of FIELD's storage: the arg's byte size (via Ctype_Nbit: builtin
// csize, or the totsize_byte computed by gen_compute_size) times the inline
// array multiple. Before gen_compute_size runs, a non-builtin arg reports 0.
// i64 like Ctype_Nbit: an uncapped csize times an array multiple can exceed
// i32, and callers scale the result back up to bits.
i64 amc::Field_Sizeof(amc::FField &field) {
    return amc::Ctype_Nbit(*field.p_arg)/8 * amc::WidthMax(field);
}

// Remove naming layers from ctype (i.e. as long as ctype contains
// a single val field, keep going down, then finally return the basic type).
// If the ctype contains more than one such field at any given layer, the
// layer names no single type below it and the answer is DFLT.
// The layers can also close on themselves -- a ctype whose one Val field
// names the ctype itself, or a pair that name each other -- and a walk of
// such a chain reaches no basic type at all. The descent is therefore
// capped, and a chain that reaches the cap answers DFLT as well: a cycle
// names no type below it any more than an ambiguous layer does. Every
// caller already has an answer for DFLT, so a cyclic ctype is left to the
// diagnostics the schema checks report for it -- the field-level ones
// naming each field, and the size computation naming the cycle itself.
// test/amc/bad_size_cycle.ssim carries a bitfield whose arg is a cycle and
// a cycle under a bitfield's source field, so the run is pinned as a
// diagnostic rather than an internal error.
amc::FCtype *amc::StripWrappers(amc::FCtype &ctype, amc::FCtype *dflt) {
    amc::FCtype *parent = &ctype;
    amc::FCtype *retval = dflt;
    bool done = false;
    int niter = 0;
    while (!done && niter < 100) {
        amc::FCtype *layer = NULL;
        bool ambig = false;
        ind_beg(amc::ctype_c_field_curs, field, *parent) {
            if (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Inlary) {
                ambig = ambig || layer != NULL;
                layer = field.p_arg;
            }
        }ind_end;
        if (ambig) {
            done = true;// two or more fields -- no match
        } else if (layer == NULL) {
            retval = parent;
            done = true;
        } else {
            parent = layer;
            niter++;
        }
    }
    return retval;
}

bool amc::FixaryQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Inlary && field.c_inlary->min == field.c_inlary->max;
}

// Return the parent-argument prefix for a call to one of the field
// accessors of CTYPE (Get, Set, ReadStrptrMaybe, Set<Present>, the pmask
// bitset accessors): the accessors of a global ctype's fields take no
// parent argument -- there is exactly one instance, reached through the
// namespace global -- so for a global CTYPE the result is empty.
// Otherwise the result is PARENT, followed by ", " when COMMA is set
// (the parent precedes further arguments in the call).
tempstr amc::ParentArgExpr(amc::FCtype &ctype, strptr parent, bool comma) {
    tempstr ret;
    if (!GlobalQ(ctype)) {
        ret << parent;
        if (comma) {
            ret << ", ";
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return C++ expression for accessing the 'value' of the field
// given parent reference PARNAME.
// if CTYPE is NULL, it is assumed to be FIELD.P_CTYPE
// The field may be an immediate field of CTYPE, or a field of one of the Val fields
// of CTYPE; But the search must yield exactly one match, otherwise it is an error
// If the field is accessed via a Get function, the corresponding expression is constructed.
// i.e. the outputs of this function could be
//     parent.subfield.field
//     parent.field
//     field_Get(parent)
//     field_Get(parent.subfield)
tempstr amc::FieldvalExpr(amc::FCtype *ctype, amc::FField &field, strptr parname, strptr fldname DFLTVAL(strptr())) {
    cstring name(fldname);
    if (!ch_N(name)) {
        name = name_Get(field);
    }
    tempstr ret;
    bool need_get = field.c_fbigend || FldfuncQ(field) || field.c_bitfld;
    tempstr path;
    // compute subfield access path
    int nfound = 0;
    if (!ctype) {
        nfound = 1;
    }
    // search direct fields
    if (nfound == 0) {
        ind_beg(amc::ctype_c_field_curs,child,*ctype) if (nfound==0) {
            if (&child==&field) {
                nfound++;
                break;
            }
        }ind_end;
    }
    // search parent fields
    amc::FCtype *base=NULL;
    if (ctype) {
        base = amc::GetBaseType(*ctype,NULL);
        if (base) {
            ind_beg(amc::ctype_c_field_curs,child,*base) {
                if (&child==&field) {
                    nfound++;
                    break;
                }
            }ind_end;
        }
    }
    // search fields one level deep
    // don't stop when found -- if there is more than one match, we have an an ambiguous selection
    if (nfound == 0) {
        ind_beg(amc::ctype_c_field_curs,child,*ctype) {
            ind_beg(amc::ctype_c_field_curs,subchild,*child.p_arg) if (&subchild==&field) {
                if (parname != "") {
                    path << ".";
                }
                path << name_Get(child);
                nfound++;
            }ind_end;
        }ind_end;
        if (base) {
            vrfy(ctype->fields_cloned, "bad call to FieldvarExpr before fields were cloned");
        }
    }
    if (!nfound) {
        prerr("amc.badref"
              <<Keyval("ctype",ctype->ctype)
              <<Keyval("field",field.field)
              <<Keyval("parname",parname)
              <<Keyval("comment","Value of field is undefined in the context of this ctype (reachable via parname)"));
        algo_lib::_db.exit_code++;
    }
    if (nfound > 1) {
        prerr("amc.badref"
              <<Keyval("ctype",ctype->ctype)
              <<Keyval("field",field.field)
              <<Keyval("comment","ambiguous reference -- more than one match"));
        algo_lib::_db.exit_code++;
    }
    // use the shortest form of accessing the field -- omit _Get if necessary
    if (need_get) {
        // with a non-empty path the accessor belongs to the subfield's
        // ctype and the parent reference is a member access, never the
        // collapsed accessor of a global (ParentArgExpr)
        amc::FCtype *parctype = ctype ? ctype : field.p_ctype;
        tempstr parentarg;
        if (ch_N(path)) {
            parentarg << parname << path;
        } else {
            parentarg << ParentArgExpr(*parctype, parname, false);
        }
        ret << name<<"_Get("<<parentarg<<")";
    } else {
        if (parname != "" || path != "") {
            ret << parname << path << ".";
        }
        ret << name;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True when CTYPE's frame length is known only at runtime: a varlen tail
// appends bytes past the fixed portion, and an Opt field appends one optional
// element there. A ctype with neither occupies exactly its fixed size, so
// every store of its length writes a value the generator itself computed.
// This is the only predicate in amc for the question "is this frame's length
// fixed": gen_check_lenfld reads it to decide which totals it can prove
// storable at generation time, and the Fmt* constructor, the TS Encode and
// the dispatch read consult it before deciding whether their store of a
// runtime total needs a guard.
bool amc::RuntimeFrameLenQ(amc::FCtype &ctype) {
    return !zd_varlenfld_EmptyQ(ctype) || ctype.c_optfld != NULL;
}

// -----------------------------------------------------------------------------

// Largest frame byte count the wire format admits, in bytes.
// Consider a 3GiB frame whose length word is an unsigned 32-bit field: the
// word holds the count exactly, so a writer that consults only the word's
// range sends the frame. Its reader reconstructs the total through
// LengthExpr, which evaluates in i32, and reads a negative length -- the
// frame is framed as a message nobody wrote. The count a length word can
// hold is therefore not the count the format admits, and the two backends
// cannot each pick their own: whatever one encoder produces, the other's
// reader must be able to represent.
// The narrower of the two representations is the i32 the C++ reader
// reconstructs in, which is also what every buffer size argument in the
// tree is expressed as, so the domain is i32 and it is stated here alone.
// The C++ range guard drops its maximum term once the length word already
// covers this bound (LenfldCheckExpr), the message constructor refuses a
// total past it, and the TypeScript encoder throws at it.
u64 amc::FrameLenMax() {
    return 0x7fffffff;
}

// -----------------------------------------------------------------------------

// Return C++ expression computing total length of ctype CTYPE
// accessible via name NAME.
tempstr amc::LengthExpr(amc::FCtype &ctype, strptr name) {
    tempstr ret;
    ret << "i32(";
    if (ctype.c_lenfld) {
        // a scale<=0 lenfld is rejected by gen_check_lenfld, which reports
        // every offender in one run; the expression built here for such a
        // schema is never consumed because the run exits nonzero
        ret << FieldvalExpr(&ctype, *ctype.c_lenfld->p_field, name);
        if (ctype.c_lenfld->scale != 1) {
            ret << " * " << ctype.c_lenfld->scale;
        }
        if (ctype.c_lenfld->extra > 0) {
            ret << " - " << ctype.c_lenfld->extra;
        } else if (ctype.c_lenfld->extra < 0) {
            ret << " + " << -ctype.c_lenfld->extra;
        }
    } else {
        ret << "sizeof("<<ctype.cpp_type<<")";
    }
    ret << ")";
    return ret;
}

// -----------------------------------------------------------------------------

// Return C++ expression computing total length of ctype CTYPE accessible
// via name NAME, in i64 domain. LengthExpr runs the scale multiply in the
// length word's own type, so a corrupt stored value wraps mod 2^N into a
// small plausible total; here the word is widened to i64 first, and the
// exact total survives for a caller that validates an untrusted length
// word against the storable range before narrowing (InsertMaybe).
// The widening is exact only while the arithmetic fits i64: for a word
// range wider than u32, at any scale, the caller must bound the raw word
// before evaluating this expression (InsertMaybe emits a generation-time
// bound) -- otherwise the scaled multiply, or at scale 1 the extra and
// fixed-size adjustment after the u64->i64 wrap, itself overflows.
tempstr amc::LengthExpr64(amc::FCtype &ctype, strptr name) {
    tempstr ret;
    if (ctype.c_lenfld) {
        ret << "i64(" << FieldvalExpr(&ctype, *ctype.c_lenfld->p_field, name) << ")";
        if (ctype.c_lenfld->scale != 1) {
            ret << " * " << ctype.c_lenfld->scale;
        }
        if (ctype.c_lenfld->extra > 0) {
            ret << " - " << ctype.c_lenfld->extra;
        } else if (ctype.c_lenfld->extra < 0) {
            ret << " + " << -ctype.c_lenfld->extra;
        }
    } else {
        ret << "i64(sizeof("<<ctype.cpp_type<<"))";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Max value LENFLD's word can store, which is the same question FieldMaxStore
// answers about the word's field: the arg type's max, clamped to the bits a
// Bitfld store keeps.
// Returns false when the arg resolves to no integer type: the word then has
// no numeric range, MAX holds the widest-signed fallback, and gen_check_lenfld
// rejects the schema, so generated expressions built from the fallback are
// never consumed.
bool amc::LenfldMaxStore(amc::FLenfld &lenfld, u64 &max) {
    return amc::FieldMaxStore(*lenfld.p_field, max);
}

// -----------------------------------------------------------------------------

// Largest frame byte total LENFLD can store: max storable word * scale - extra
// (the reader formula at the word's max). Exact for a word range within u32 --
// every range an emitter guards; a wider word stores any frame total either
// backend can meet (C++ i32, JS 2^53), so the formula, which could wrap u64
// there (a u64 word with a negative extra), saturates instead. A shipping
// schema cannot go negative (gen_check_lenfld rejects extra beyond the
// range); the clamp keeps the reject-run expression harmless.
u64 amc::LenfldMaxLen(amc::FLenfld &lenfld) {
    u64 max = 0;
    amc::LenfldMaxStore(lenfld, max);// no numeric range: rejected by gen_check_lenfld
    u64 ret = 0xffffffffffffffff;
    if (max <= 0xffffffff) {
        ret = u64(i64_Max(i64(max) * lenfld.scale - lenfld.extra, 0));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True if a runtime byte total stored through LENFLD can fall below the
// smallest total the word represents, so that the store numerator
// (total + extra) goes negative and wraps through the word's own type.
// A total of 1 byte stored through a u8 word with extra:-8 writes 249, and
// the reader reconstructs 257 -- a frame 256 bytes longer than was written.
// Only a negative extra can produce it, and only one whose magnitude exceeds
// the ctype's fixed size: a store site's smallest total is that fixed size,
// so a schema whose fixed size already covers the extra can never reach the
// low end. gen_check_lenfld rejects the same shape outright wherever the
// stored total is the fixed size itself; here the total is a runtime one.
bool amc::LenfldLowGuardNeededQ(amc::FLenfld &lenfld) {
    amc::FCtype &ctype = *lenfld.p_field->p_ctype;
    return lenfld.extra < 0 && !ctype.size_unknown && i64(ctype.totsize_byte) + lenfld.extra < 0;
}

// -----------------------------------------------------------------------------

// True if a runtime byte total stored through LENFLD needs a guard before
// the store: an indivisible total truncates through the store formula
// (scale != 1), a total beyond the storable range wraps mod 2^N through
// the word's own type, and a total below the low end wraps the same way
// (LenfldLowGuardNeededQ). A scale-1 lenfld whose range covers the whole
// frame-length domain (FrameLenMax), and whose fixed size covers its extra,
// can store any total and needs no test at all.
bool amc::LenfldGuardNeededQ(amc::FLenfld &lenfld) {
    return lenfld.scale != 1 || amc::LenfldMaxLen(lenfld) < amc::FrameLenMax() || amc::LenfldLowGuardNeededQ(lenfld);
}

// -----------------------------------------------------------------------------

// Append LENFLD's extra term to RET, the numerator of the store formula
// LENEXPR + extra: "+extra" for a positive extra, "-|extra|" for a negative
// one, nothing when extra is zero.
static void AppendLenfldExtra(cstring &ret, amc::FLenfld &lenfld) {
    if (lenfld.extra > 0) {
        ret << "+" << lenfld.extra;
    } else if (lenfld.extra < 0) {
        ret << lenfld.extra;
    }
}

// -----------------------------------------------------------------------------

// Return C++ boolean expression testing that LENEXPR, an expression for a
// total byte length, is representable in LENFLD: the store formula
// (LENEXPR + extra) / scale truncates unless scale divides the numerator
// (the reader reconstructs stored*scale - extra, so only a multiple
// round-trips), a total beyond the storable range (LenfldMaxLen) wraps
// mod 2^N through the word's own type, framing a shorter message than was
// written, and a total below the low end (LenfldLowGuardNeededQ) wraps the
// same way, framing a longer one. Emitters of a runtime-length store guard
// with this expression when LenfldGuardNeededQ; each term is emitted only
// when its failure is possible, so the expression is never vacuous at a
// guarded site.
tempstr amc::LenfldCheckExpr(amc::FLenfld &lenfld, strptr lenexpr) {
    tempstr ret;
    algo::ListSep ls(" && ");
    if (amc::LenfldLowGuardNeededQ(lenfld)) {
        ret << ls << "(" << lenexpr << ") >= " << -lenfld.extra;
    }
    if (lenfld.scale != 1) {
        ret << ls << "(" << lenexpr;
        AppendLenfldExtra(ret, lenfld);
        ret << ") % " << lenfld.scale << " == 0";
    }
    u64 maxlen = amc::LenfldMaxLen(lenfld);
    if (maxlen < amc::FrameLenMax()) {
        ret << ls << "(" << lenexpr << ") <= " << maxlen;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return C++ expression computing the value stored in LENFLD from LENEXPR,
// an expression for the total byte length: (LENEXPR + extra) / scale --
// the exact inverse of the reader formula (LengthExpr: total = stored*scale - extra).
tempstr amc::LenfldStoreExpr(amc::FLenfld &lenfld, strptr lenexpr) {
    bool scaled = lenfld.scale != 1;
    tempstr ret;
    if (scaled) {
        ret << "(";
    }
    ret << lenexpr;
    AppendLenfldExtra(ret, lenfld);
    if (scaled) {
        ret << ") / " << lenfld.scale;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True if the field has no direct member to write: the value lives behind
// an accessor pair (fbigend byte-swapped storage, a bitfld slice of its
// source word, a fldfunc computed view), so any store must go through the
// generated $name_Set. Codecs that manage side effects themselves (the
// kafka decoder, whose presence bits depend on the wire's null flag and
// must not be set by a pmask Set) route stores on this predicate alone.
bool amc::NoDirectMemberQ(amc::FField &field) {
    return field.c_fbigend || FldfuncQ(field) || field.c_bitfld;
}

// -----------------------------------------------------------------------------

// True if a store to the field must go through the generated $name_Set:
// the field has no direct member (NoDirectMemberQ), or a Set exists to
// dispatch side effects (fcond membership, pmask presence).
// Generated store sites that run after the record is xreffed (assignment,
// read fallbacks) route on this predicate. Init is the deliberate
// exception: it writes the raw member of a freshly allocated record,
// before XrefMaybe derives fcond membership from the value and before
// the presence mask may be touched.
bool amc::AssignViaSetQ(amc::FField &field) {
    bool has_set = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Set"));
    return has_set || amc::NoDirectMemberQ(field);
}

// -----------------------------------------------------------------------------

// Return C++ expression assigning value VALUE to field FIELD
// given parent reference PARNAME.
// If NEEDS_CAST is set, a cast is added to the target type
tempstr amc::AssignExpr(amc::FField &field, strptr parname, strptr value, bool needs_cast, strptr fldname DFLTVAL(strptr())) {
    cstring name(fldname);
    if (!ch_N(name)) {
        name = name_Get(field);
    }
    tempstr ret;
    tempstr value_expr;
    if (needs_cast) {
        value_expr << field.cpp_type << "(";
        value_expr << value;
        value_expr << ")";
    } else {
        value_expr << value;
    }
    if (AssignViaSetQ(field)) {
        ret << name<<"_Set("<<ParentArgExpr(*field.p_ctype,parname,true)<<value_expr<<")";
    } else {
        if (elems_N(parname)>0 && parname[0] == '*') {
            ret << RestFrom(parname,1) << "->";
        } else {
            ret << parname << ".";
        }
        ret << name<<" = "<<value_expr;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Heuristically determine if the type is a string type.
// When in doubt, the function answers no.
bool amc::StringQ(amc::FCtype &ctype) {
    amc::FCtype *wrapped_type = &ctype;
    while (amc::c_datafld_N(*wrapped_type)==1) {
        wrapped_type = amc::c_datafld_Find(*wrapped_type, 0)->p_arg;
    }
    return wrapped_type->c_cstr != NULL;
}

// check if ctype has a string print function
bool amc::HasStringPrintQ(amc::FCtype &ctype) {
    if (ctype.ctype == "algo.strptr") {
        return true;
    }
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.print) {
        if (strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_String) {
            return true;
        }
    }ind_end;
    return false;
}

// check if ctype has an argv print function
bool amc::HasArgvPrintQ(amc::FCtype &ctype) {
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.print) {
        if (strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv || strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_ArgvGnu) {
            return true;
        }
    }ind_end;
    return false;
}

// check if ctype has a string read function
bool amc::HasStringReadQ(amc::FCtype &ctype) {
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.read) {
        if (strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_String) {
            return true;
        }
    }ind_end;
    return false;
}

bool amc::HasReadQ(amc::FCtype &ctype) {// check if ctype has a string read function
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.read) {
        return true;
    }ind_end;
    return false;
}

bool amc::HasArgvReadQ(amc::FCtype &ctype) {// check if ctype has an argv read function
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.read && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Argv) {
            return true;
        }
    }ind_end;
    return false;
}

// -----------------------------------------------------------------------------

// Set IDENT to sanitized version of FROM
// if MASK_KEYWORD is set, and FROM is a known c++ keyword, prepend '_' to it.
// If FROM is an empty string, use "_"
// Replace /, -, ., etc. with underscores.
// Replace + with P, ' with A, " with Q (rationale?)
// Example:
// strptr_PrintCppIdent("abcd")   -> abcd
// strptr_PrintCppIdent("ab.cd")  -> ab_cd
// strptr_PrintCppIdent("+-$")    -> P__
// strptr_PrintCppIdent("int")    -> _int
// strptr_PrintCppIdent("")       -> _
void amc::strptr_PrintCppIdent(strptr from, cstring &out, bool mask_keyword) {
    amc::CppkeywordId kw_id;
    if (!elems_N(from) || algo_lib::DigitCharQ(from[0])) {
        out << "_";
    }
    out << from;
    if (mask_keyword) {
        bool collision = id_SetStrptrMaybe(kw_id,out);
        if (collision) {
            out << "_";
        }
    }
    Translate(out
              , "+/.-<>!@#$%^&*():; '\"|[]{}"
              , "P__________________AQ_____");
}

// -----------------------------------------------------------------------------

tempstr amc::strptr_ToCppIdent(strptr s, bool mask_keyword) {
    tempstr ret;
    amc::strptr_PrintCppIdent(s,ret,mask_keyword);
    return ret;
}

// -----------------------------------------------------------------------------
//
// IDENT        identifier in question
// RETURN       identifier with . replaced with ::
//
tempstr amc::NsToCpp(strptr ident) {
    tempstr ret(ident);
    Replace(ret,".","::");
    return ret;
}

tempstr amc::NsTo_(strptr ident) {
    tempstr ret(ident);
    Replace(ret,".","_");
    return ret;
}

// Return TRUE if the type is relational (ctype is in a ssimdb namespace)
bool amc::RelationalQ(amc::FCtype &ctype) {
    return GetBaseType(ctype,&ctype)->p_ns->nstype == dmmeta_Nstype_nstype_ssimdb;
}

// Return base class of ctype, DFLT if it doesn't have a base
// Base is found by locating a field of reftype Base.
amc::FCtype *amc::GetBaseType(amc::FCtype &ctype, amc::FCtype *dflt) {
    amc::FCtype *retval = dflt;
    ind_beg(amc::ctype_c_field_curs,field,ctype) if (field.reftype == dmmeta_Reftype_reftype_Base) {
        retval = field.p_arg;
        break;
    }ind_end;
    return retval;
}

amc::FCtype *amc::UltimateBaseType(amc::FCtype *ctype, amc::FCtype *dflt) {
    amc::FCtype *retval = ctype;
    while (amc::FCtype *base = GetBaseType(*retval,NULL)) {
        retval = base;
    }
    return retval==ctype ? dflt : retval;
}

// Root for side-loaded table data (gconst and gstatic values, gsymbol
// tables): the -in_dir root when it names a directory. A single-file or
// stdin -in_dir provides no directory for side files; the default data
// root applies. The root is resolved once, in Main -- the answer cannot
// change within the run, and DataRoot is consulted per gconst, gstatic,
// and gsymbol row and per manifest side-load.
algo::strptr amc::DataRoot() {
    return strptr(amc::_db.dataroot);
}

// Load side table data (gconst and gstatic values, gsymbol tables) from
// FNAME, a filename already resolved against DataRoot(). When -in_dir is
// not a directory, side tables resolve against ./data of the current
// working directory, which may belong to a different universe than the
// -in_dir schema: a run from an unrelated checkout would quietly generate
// symbol and constant values from that checkout's tables and exit 0. The
// first successful load through the fallback root therefore prints a
// notice naming the root and the file, making the side-load visible in
// the run's output.
bool amc::SideloadFile(algo_lib::MmapFile &file, strptr fname) {
    bool ret = MmapFile_Load(file,fname);
    if (ret && amc::_db.dataroot_dflt && !amc::_db.sideload_noted) {
        amc::_db.sideload_noted = true;
        prerr("amc.sideload"
              <<Keyval("root",DataRoot())
              <<Keyval("file",fname)
              <<Keyval("comment","-in_dir is not a directory; side table data loads from the default root"));
    }
    return ret;
}

// Path under ROOT of the ssimfile CTYPE's rows live in, either because CTYPE
// owns the ssimfile or because the type it is based on does; empty when CTYPE
// reaches no ssimfile.
// A ctype fails to reach a file two ways: it declares no base at all, or a base
// that owns no ssimfile. Both are the same answer to this function's question,
// so both come back as an empty path rather than as an abort. Ending the run
// here would take the diagnostic away from the caller, and the caller is the one
// that knows which gconst or gstatic field asked -- which is what it names.
tempstr amc::SsimFilename(strptr root, amc::FCtype& ctype) {
    tempstr name;
    amc::FCtype *base = &ctype;
    if (!base->c_ssimfile) {
        base = amc::GetBaseType(*base,NULL);
    }
    if (base && base->c_ssimfile) {
        name = SsimFname(root, base->c_ssimfile->ssimfile);
    }
    return name;
}

// -----------------------------------------------------------------------------

//
// Append horizontal separator to STR
//
void amc::CppSection(algo::cstring &out, strptr label, bool major) {
    (void)major;
    out<<eol;
    out << "// --- ";
    out << label;
    out << eol;
}

void amc::CppSectionAll(amc::FNs& ns, strptr label) {
    CppSection(*ns.hdr, label, true);
    CppSection(*ns.cpp, label, true);
    CppSection(*ns.inl, label, true);
}

// -----------------------------------------------------------------------------

void amc::TopoSortVisit(amc::FNs& ns, amc::FCtype& ctype) {
    if (ctype.p_ns == &ns && bool_Update(ctype.topo_visited,true)) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.p_reftype->cascins) {// cascins
            amc::TopoSortVisit(ns, *field.p_arg);
        } ind_end;
        ind_beg(amc::ctype_zd_cafter_curs,cafter,ctype) {// cafter
            amc::TopoSortVisit(ns, *cafter.p_after);
        }ind_end;
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.c_smallstr && field.c_smallstr->c_numstr) {// numstr
            amc::TopoSortVisit(ns,*field.c_smallstr->c_numstr->p_numtype);
        } ind_end;
        ctype.topo_idx = amc::c_ctype_N(ns);
        amc::c_ctype_Insert(ns, ctype);
    }
}

bool amc::ExeQ(amc::FNs &ns) {
    return ns.nstype == dmmeta_Nstype_nstype_exe;
}

// -----------------------------------------------------------------------------

// Return C++ expression for the arg type for ctype CTYPE
// argtype is how the type is passed to a function
// If the type is "cheap copy", the value is type itself (T). Otherwise
// it is T&.
// The optional ISCONST argument marks the expression as const
// For cheap types, 'const' is omitted since there is no sense in protecting
// a copy.
tempstr amc::ByvalArgtype(amc::FCtype &ctype, bool isconst DFLTVAL(false)) {
    bool cheap = ctype.cheap_copy;
    return tempstr() << (isconst && !cheap ? "const " : "") << ctype.cpp_type<<(cheap ? "" : "&");
}

// -----------------------------------------------------------------------------

static void Main_ReportCycle() {
    u64 gen_cycle_total=0;
    ind_beg(amc::_db_gen_curs, gen,amc::_db) {
        gen_cycle_total += gen.cycle_total;
        verblog("amc.gen"
                <<Keyval("gen",gen.gen)
                <<Keyval("time", gen.cycle_total / algo::get_cpu_hz()));
    }ind_end;
    verblog("amc.cycle_total"
            <<Keyval("gen_time", gen_cycle_total / algo::get_cpu_hz()));
}

// -----------------------------------------------------------------------------

void amc::Main_CloneFconst_Field(amc::FField &field) {
    // generate ctype with a single field mapping string values of FIELD's fconsts
    // to numbers
    dmmeta::Ctype ctype_enum(tempstr() << ctype_Get(field) << "Case"
                             , algo::Comment(tempstr()<<"enum helper "<<field.p_ctype->comment));
    dmmeta::Field field_enum(tempstr() << ctype_enum.ctype << "." << name_Get(field)
                             , "u8", dmmeta_Reftype_reftype_Val, algo::CppExpr(), algo::Comment());
    dmmeta::Fcast fcast(field_enum.field, "", algo::Comment());
    dmmeta::Cpptype cpptype;
    cpptype.ctype = ctype_enum.ctype;
    cpptype.ctor = true;
    dmmeta::Pack pack(ctype_enum.ctype, algo::Comment());// insert pack
    dmmeta::Anonfld anonfld(field_enum.field, algo::Comment());

    // skip if already exists
    if (!amc::ind_ctype_Find(ctype_enum.ctype)) {
        bool ok=amc::ctype_InsertMaybe(ctype_enum);
        ok = amc::InsField(field_enum)&&ok;
        ok = amc::fcast_InsertMaybe(fcast) && ok;
        ok = amc::cpptype_InsertMaybe(cpptype) && ok;
        ok = amc::pack_InsertMaybe(pack) && ok;
        ok = amc::anonfld_InsertMaybe(anonfld) && ok;
        if (!ok) {
            prerr("amc.fconst_clone_failed"
                  <<Keyval("field",field.field));
        }
        int next_val = 1;
        ind_beg(amc::field_c_fconst_curs,fconst,field) {
            // map empty string to 0, everything to integers 1+
            tempstr val = tempstr() << (ch_N(fconst.value.value)>0 ? int(next_val++) : int(0));
            dmmeta::Fconst fconst_enum(tempstr() << field_enum.field << "/" << fconst.value
                                       , algo::CppExpr(val), algo::Comment(fconst.comment));
            if (amc::ind_fconst_Find(fconst_enum.fconst)) {
                verblog("amc.clone_fconst:'"<< fconst_enum.fconst<<"  comment:'Value skipped because it is a duplicate'");
            } else {
                amc::fconst_InsertMaybe(fconst_enum);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// HOW IS THIS DIFFERENT FROM INSFIELD?
void amc::Field_AddChild(amc::FField &parent, const dmmeta::Field &field) {// subfield??
    amc::FField *child = amc::field_InsertMaybe(field);
    if (child) {
        child->synthetic = true;
        child->rowid = ChildRowid(double_Max(parent.child_rowid,parent.rowid));
        c_field_QuickSort(*child->p_ctype);// place after parent
        parent.child_rowid = child->rowid;// update
    }
}

// -----------------------------------------------------------------------------

bool amc::NeedSetQ(amc::FField &field) {
    amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;

    // don't generate Set function when insert condition is not interesting --
    // it is confusing the user.
    bool ret = field.c_xref && field.c_xref->inscond.value != "true" && field.c_xref->inscond.value != "false";
    // don't generate Set function for computed fields
    ret = ret && !FldfuncQ(sortfld);
    // don't generate Set function for complex fields
    ret = ret && ValQ(sortfld);
    return ret;
}

// -----------------------------------------------------------------------------

void amc::tclass_Cppstack() {
    // not used
}

void amc::tclass_Pkey() {
    // does not occur
}

// -----------------------------------------------------------------------------

int amc::c_anonfld_N(amc::FCtype &ctype) {
    int n_anon = 0;
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (!FldfuncQ(field)) {
        n_anon += field.c_anonfld != NULL;
    }ind_end;
    return n_anon;
}

// -----------------------------------------------------------------------------

static void CheckCumulativeError(amc::FGen &gen, int &prev_err) {
    if (i32_UpdateMax(prev_err,algo_lib::_db.exit_code)) {// check if an error occured in this step
        verblog("amc.gen_error"
                <<Keyval("gen",gen.gen)
                <<Keyval("gen_comment",gen.comment));
    }
}

// -----------------------------------------------------------------------------

static void Main_Edit() {
    command::acr_proc acr;
    tempstr key(algo::Pathcomp(amc::_db.cmdline.query, ":RL"));
    tempstr val(algo::Pathcomp(amc::_db.cmdline.query, ":RR"));
    if (key == "") {
        key = "(ns|ctype|field|dispatch)";
    }
    acr.cmd.query = tempstr()<<key<<":"<<val<<"";
    acr.cmd.t = true;
    acr.cmd.e = true;
    acr.cmd.print = true;
    // the editor owns the terminal: acr inherits amc's stdout so vim can
    // draw, which also puts acr's report line on the screen rather than in
    // a pipe amc could parse. the exit status therefore carries only
    // success or failure. a failed edit aborts the run before codegen -- a
    // session whose changes were never written back must not end with amc
    // regenerating and reporting success. after a successful edit the run
    // proceeds unconditionally; regeneration from an unchanged schema is a
    // no-op.
    int status = acr_Exec(acr);
    vrfy(status==0, tempstr()<<"amc.edit"
         <<Keyval("cmd",acr_ToCmdline(acr))
         <<Keyval("status",algo::DescribeWaitStatus(acr.status))
         <<Keyval("comment","edit failed; no code was regenerated"));
    amc::_db.cmdline.query = "";// revert to a normal run
}

// -----------------------------------------------------------------------------

static void Main_Report() {
    amc::_db.report.n_xref = amc::xref_N();
    amc::_db.report.n_ctype   += amc::ctype_N();
    amc::_db.report.n_func    += amc::func_N();
    algo_lib::Reset(amc::_db.genctx.R);// defensive

    if (amc::_db.cmdline.report) {
        prlog(amc::_db.report);
    }
}

// -----------------------------------------------------------------------------

static void RunGen(amc::FGen &gen, int &prev_err) {
    u64 c=algo::get_cycles();
    amc::_db.cur_gen=gen.gen;
    gen.step();
    CheckCumulativeError(gen,prev_err);
    gen.cycle_total += algo::get_cycles()-c;
}

// Go over `amcdb.gen` table and invoke each global generator
// Collect performance data long the way
// This is where amc spends most of the time. See the `amcdb.gen` table for more information.
void amc::Main_Gen() {
    int prev_err=0;
    ind_beg(amc::_db_gen_curs,gen,amc::_db) {
        if (!gen.perns) {
            RunGen(gen,prev_err);
        } else {
            ind_beg(amc::_db_ns_curs, ns, amc::_db) {
                if (ns.select) {
                    amc::_db.c_ns=&ns;
                    RunGen(gen,prev_err);
                }
            }ind_end;
        }
    }ind_end;
}

// ----------------------------------------------------------------------------
// OUTPUT        Generate files for the namespace(s)

void amc::Main() {
    _db.genctx.R.strict=2;
    algo_lib::ApplyTrace(_db.cmdline.trace.expr);
    // open in editor before loading data
    if (amc::_db.cmdline.e) {
        Main_Edit();
    }
    // resolve the root for side-loaded table data once (one stat per run)
    amc::_db.dataroot_dflt = !DirectoryQ(amc::_db.cmdline.in_dir);
    amc::_db.dataroot = amc::_db.dataroot_dflt ? strptr("data") : strptr(amc::_db.cmdline.in_dir);
    vrfy(amc::LoadTuplesMaybe(amc::_db.cmdline.in_dir,true), algo_lib::_db.errtext);
    // Look up default allocator
    amc::_db.c_malloc = amc::ind_field_Find("algo_lib.FDb.malloc");
    vrfy(amc::_db.c_malloc, tempstr()<<"amc.fieldnotfound"
         <<Keyval("field","algo_lib.FDb.malloc")
         <<Keyval("comment", "missing pre-requisite record"));

    // generated files are written somewhere inside
    // this step
    Main_Gen();
    Main_ReportCycle();

    if (QueryModeQ()) {
        Main_Querymode();
    }

    // a failing run reports amc.no_output only when it truly wrote nothing;
    // a run that fails after writing some files must not claim otherwise
    if (algo_lib::_db.exit_code!=0 && amc::_db.report.n_filemod==0) {
        prerr("amc.no_output"
              <<Keyval("comment","no files were modified"));
    }
    Main_Report();
}
