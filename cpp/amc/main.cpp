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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/main.cpp -- Main driver
//
// Algo Model Compiler (AMC)
// TODO: $ errlist 'cd ~/proj/.testgen && (cd ../ && abt amc && dflt.debug-x86_64/amc -out_dir .testgen) && abt -install %'
// TODO: AB-17400: target:amc: make buffers bidirectional by default
// TODO: acms_script -> amc_sh
// TODO: amc -size is broken:
// TODO: amc can't delete double cascdel
// TODO: amc extra unnecessary strings: ~40k
// TODO: amc steps: tutorial
// TODO: amc.error  where:load_input  amc.xref_error  xref:amc.FFixfield.p_fixtag  via:amc.FDb.ind_fixtag  key:'Account'
// TODO: amc: Base field must be first -- otherwise no castbase
// TODO: amc: Cmp - define over Tary
// TODO: amc: add Alias reftype
// TODO: amc: add UpdateTrace function
// TODO: amc: add fldoffset for first data field of Base -- must be zero?
// TODO: amc: add instname
// TODO: amc: add support for aggregation of attribute across xref.
// TODO: amc: add support for aggregation of attribute across xref. cascdel + no xref
// TODO: amc: all field print functions have to be implement as individual macros
// TODO: amc: allow any program to be a module (what does it mean?)
// TODO: amc: bheap: instead of field_Set(...), use c_field_SetSortkey(...)
// TODO: amc: breakpoint when generating ith line of function in amc.
// TODO: amc: calls to amc::ind_func_Find are dangerous -- depend on the order of generation
// TODO: amc: compute sizes of ctypes
// TODO: amc: create function before calling function generator
// TODO: amc: create per-namespace symbol table
// TODO: amc: cstring: define fcmp
// TODO: amc: delete function if flag is set.
// TODO: amc: disallow gconst with > 100 items if not an executable (too much code)
// TODO: amc: eliminate forder -- use bheaps
// TODO: amc: eliminate use of bltin
// TODO: amc: finput on Val - keep updating same value
// TODO: amc: for timehook, add extra function that does timing.
// TODO: amc: generate bad code if step for bheap has different name than bheap
// TODO: amc: generate builtins.
// TODO: amc: generate cursors as regular ctypes
// TODO: amc: mark whole namespace as key namespace?
// TODO: amc: merge xrefvia + xreffld into xrefpath?
// TODO: amc: newfield: pluggable
// TODO: amc: print ctype functions with their ctype
// TODO: amc: print ctype functions with their ctype. cstring: define fcmp
// TODO: amc: print function for inlary impossible -- tuple format is not respected
// TODO: amc: protect xrefs --
// TODO: amc: replace numstr with a fldfunc.
// TODO: amc: suggest cheapcopy types?
// TODO: amc: support fnotify for ibendpt
// TODO: amc: types that don't have Global don't get StaticCheck generated -- for instance global namespace
// TODO: amc: use top-down Cmp, Lt generator for Smallstr.
// TODO: amc_vis: print ns summary
// TODO: amcdb: add pool table
// TODO: cstring operator ==, < must be defined by amc (default one)
// TODO: do not make amc dependent on the conversion -- use elems_Getary where necessary
// TODO: does amc support defaults for bitfld?
// TODO: fix generation of cross-namespace steps in amc (currently broken -- confuses step namespace with field namespace)
// TODO: idea: unify steps and fields in amc -- cannot be done, steps are defined over fields.
// TODO: in amc: never call built-in operators, always call functions.
// TODO: jira.Issue  issue:AB-13338  basebranch:master  status:"In Progress"  approvedby:""  mergereq:No  assignee:alebedev  summary:"arch: amc_gc cycle"
// TODO: must move strptr to amc.
// TODO: replace aryptr with amc-generated code and aryptr table
// TODO: stringiter: move to amc

#include "include/amc.h"
#include "include/gen/report_gen.h"

// -----------------------------------------------------------------------------

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
        ret << Refname(*field.p_arg) << "_Delete()";
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
    if (ns_Get(ctype) != "") {
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

bool amc::DirectStepQ(amc::FFstep &fstep) {
    return !(fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur);
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

// True if ctype is instantiated through a memory pool that has an Alloc function.
bool amc::PoolHasAllocQ(amc::FCtype &ctype) {
    bool retval=false;
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) if (inst.p_reftype->hasalloc) {
        retval=true;
        break;
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

void amc::GenPrintStmt(cstring &out, amc::FCtype &parenttype, amc::FField &field, strptr strname, strptr parentname) {
    if (amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
        if (func->ismacro) {// inline the function!
            out << func->body;
        } else {
            out << ns_Get(*field.p_ctype) << "::"
                << name_Get(field) << "_Print("<<parentname<<"), "<<strname<<");\n";
        }
    } else {
        out << name_Get(*field.p_arg)
            <<"_Print("<<FieldvalExpr(&parenttype,field,parentname) <<", "<<strname<<");\n";
    }
}

// -----------------------------------------------------------------------------

// print binary octet string as hex byte array initializer
void amc::memptr_PrintOctetsHexArray(algo::memptr ary, cstring &out, bool caps) {
    out << "{";
    algo::ListSep ls(",");
    frep_(i,elems_N(ary)) {
        out << ls;
        u64_PrintHex(ary[i],out,2,true,caps);
    }
    out << "}";
}

bool amc::FldfuncQ(amc::FField &field) {
    return field.c_cppfunc || field.c_substr || field.c_falias;
}

bool amc::CanCopyQ(amc::FCtype &ctype) {
    amc::FCtype *fldbase = GetBaseType(ctype,NULL);
    bool can_copy = fldbase && !fldbase->c_varlenfld && !ctype.c_varlenfld;
    // cannot copy if any of the parent fields are a Tary
    // cannot copy if any of the parent fields have a private copy constructor
    // or disallow a copy operator.
    if (fldbase) {
        ind_beg(amc::ctype_c_field_curs, fld, *fldbase) {
            //can_copy &= !fld.c_tary;
            if (ValQ(fld)) {
                can_copy &= !CopyPrivQ(*fld.p_arg);
            }
        }ind_end;
    }
    return can_copy;
}

bool amc::PoolVarlenQ(amc::FField &field) {
    bool pool_varlen  = field.reftype == dmmeta_Reftype_reftype_Lpool;
    pool_varlen      |= field.reftype == dmmeta_Reftype_reftype_Malloc;
    pool_varlen      |= field.reftype == dmmeta_Reftype_reftype_Blkpool;
    return pool_varlen;
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

void amc::GetMinMax(amc::FCtype& _ctype, u64 &min, u64 &max, bool &issigned) {
    amc::FCtype *ctype = &_ctype;
    while (c_field_N(*ctype) == 1 && c_field_Find(*ctype,0)->reftype == dmmeta_Reftype_reftype_Val) {
        ctype = c_field_Find(*ctype,0)->p_arg;
    }
    if (ctype->c_bltin) {
        issigned=ctype->c_bltin->issigned;
    }
    amc::BltinId bltin_id(amc_BltinIdEnum(0));
    value_SetStrptrMaybe(bltin_id,ctype->ctype);
    switch (bltin_id.value) {
    case amc_BltinId_bool:    min = 0;                  max = 1;                  break;
    case amc_BltinId_u8:      min = 0;                  max = 0xff;               break;
    case amc_BltinId_i8:      min = 0xffffffffffffff80; max = 0x7f;               break;
    case amc_BltinId_u16:     min = 0x00;               max = 0xffff;             break;
    case amc_BltinId_i16:     min = 0xffffffffffff8000; max = 0x7fff;             break;
    case amc_BltinId_u32:     min = 0x00000000;         max = 0xffffffff;         break;
    case amc_BltinId_i32:     min = 0xffffffff80000000; max = 0x7fffffff;         break;
    case amc_BltinId_u64:     min = 0x0000000000000000; max = 0xffffffffffffffff; break;
    case amc_BltinId_i64:
        // min signed 64-bit value is off by one because even GCC can't
        // properly recognize this constant
    default:                     min = 0x8000000000000001; max = 0x7fffffffffffffff; break;
    }
}

// -----------------------------------------------------------------------------

bool amc::CheapCopyQ(amc::FField &field) {
    amc::FCtype         &tgttype    = *field.p_arg;
    return tgttype.c_cpptype && tgttype.c_cpptype->cheap_copy;
}

static bool HasFcast(amc::FCtype& ctype){
    int result =false;
    ind_beg(amc::ctype_c_field_curs,field,ctype) if(field.c_fcast) {
        result=true;
    }ind_end;
    return result;
}

tempstr amc::EvalRowid(amc::FCtype &ctype) {// return c++ type for rowid
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

// Pick a name with which to refer to a record of type CTYPE
tempstr amc::Refname(amc::FCtype &ctype) {
    tempstr refname;
    ind_beg(amc::ctype_zd_inst_curs,inst,ctype) {// has global instance? use it
        if (inst.reftype == dmmeta_Reftype_reftype_Global) {
            refname << name_Get(inst);
        }
    }ind_end;
    if (!ch_N(refname)) {
        amc::FField *pool=FirstInst(ctype);
        if (pool) {// pick first usable instance
            refname << name_Get(*pool);
        } else {
            refname << "parent";
        }
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

int amc::Field_Sizeof(amc::FField &field) {
    amc::FCtype &ctype = *field.p_arg;
    int size = ctype.c_csize ? ctype.c_csize->size : 0;
    int width = amc::WidthMax(field);
    return size * width;
}

// Remove naming layers from ctype (i.e. as long as ctype contains
// a single val field, keep going down, then finally return the basic type).
// If the ctype contains more than one field at any given layer, return DFLT.
amc::FCtype *amc::StripWrappers(amc::FCtype &ctype, amc::FCtype *dflt) {
    amc::FCtype *parent = &ctype;
    int niter=0;
    for (; niter<100; niter++) {
        bool found = false;
        ind_beg(amc::ctype_c_field_curs, field, *parent) {
            if (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Inlary) {
                if (found) {
                    return dflt;// two or more fields -- no match
                }
                parent = field.p_arg;
                found = true;
            }
        }ind_end;
        if (!found) {
            break;
        }
    }
    vrfy(niter<100, tempstr()<<"amc.deep_impact"
         <<Keyval("ctype",ctype.ctype)
         <<Keyval("comment","Recursion limit exceeded"));
    return parent;
}

bool amc::FixaryQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Inlary && field.c_inlary->min == field.c_inlary->max;
}

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
tempstr amc::FieldvalExpr(amc::FCtype *ctype, amc::FField &field, strptr parname) {
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
                path << "." << name_Get(child);
                nfound++;
            }ind_end;
        }ind_end;
        // why is this necessary? shouldn't we have cloned first?
        // the answer is that sometimes FieldvalExpr is called before cloning.
        if (base && !ctype->fields_cloned) {
            ind_beg(amc::ctype_c_field_curs,child,*base) {
                ind_beg(amc::ctype_c_field_curs,subchild,*child.p_arg) if (&subchild==&field) {
                    path << "." << name_Get(child);
                    nfound++;
                }ind_end;
            }ind_end;
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
        ret << name_Get(field)<<"_Get("<<parname<<path<<")";
    } else {
        ret << parname<<path<<"."<<name_Get(field);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return C++ expression computing total length of ctype CTYPE
// accessible via name NAME.
tempstr amc::LengthExpr(amc::FCtype &ctype, strptr name) {
    tempstr ret;
    ret << "i32(";
    if (ctype.c_lenfld) {
        ret << FieldvalExpr(&ctype, *ctype.c_lenfld->p_field, name);
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

// Return C++ expression assigning value VALUE to field FIELD
// given parent reference PARNAME.
// If NEEDS_CAST is set, a cast is added to the target type
tempstr amc::AssignExpr(amc::FField &field, strptr parname, strptr value, bool needs_cast) {
    tempstr ret;
    tempstr value_expr;
    if (needs_cast) {
        value_expr << field.cpp_type << "(";
        value_expr << value;
        value_expr << ")";
    } else {
        value_expr << value;
    }
    bool has_set = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Set"));
    if (has_set || field.c_fbigend || FldfuncQ(field) || field.c_bitfld ) {
        ret << name_Get(field)<<"_Set("<<parname<<", "<<value_expr<<")";
    } else {
        if (elems_N(parname)>0 && parname[0] == '*') {
            ret << RestFrom(parname,1) << "->";
        } else {
            ret << parname << ".";
        }
        ret << name_Get(field)<<" = "<<value_expr;
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

// -----------------------------------------------------------------------------

// True if ctype contains a varlen field
bool amc::VarlenQ(amc::FCtype &ctype) {// has varlen or opt field
    bool ret = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        ret = field.reftype == dmmeta_Reftype_reftype_Varlen
            || field.reftype == dmmeta_Reftype_reftype_Opt;
        if (ret) {
            break;
        }
    }ind_end;
    return ret;
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

tempstr amc::SsimFilename(strptr root, amc::FCtype& ctype, bool do_throw) {
    tempstr name;
    amc::FCtype *base = &ctype;
    if (!base->c_ssimfile) {
        base = amc::GetBaseType(*base,NULL);
    }
    vrfy(base && (base->c_ssimfile || !do_throw),tempstr()<< "no ssimfile defined for ["<<ctype.ctype<<"]");
    if (base->c_ssimfile) {
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
    bool cheap = ctype.c_cpptype && ctype.c_cpptype->cheap_copy;
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
    dmmeta::Cpptype cpptype(ctype_enum.ctype, true/*ctor*/,true/*dtor*/, false/*cheap_copy*/);
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
                                       , algo::CppExpr(val), fconst.comment);
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
    acr.cmd.query = tempstr()<<"(ns|ctype|field|dispatch):("<<amc::_db.cmdline.query<<")";
    acr.cmd.t = true;
    acr.cmd.e = true;
    acr.cmd.print = true;
    // vim uses stdout, so redirecting output is not an option
    // return code from acr -e indicates how many files were modified.
    int rc = acr_Exec(acr);
    if (rc==0) {
        // acr did nothing, exit as quickly as possible
        _exit(0);
    }
    amc::_db.cmdline.query = "";// revert to a normal run
}

// -----------------------------------------------------------------------------

static void Main_Report() {
    amc::_db.report.n_xref = amc::xref_N();
    amc::_db.report.n_ctype   += amc::ctype_N();
    amc::_db.report.n_func    += amc::func_N();
    ind_replvar_Cascdel(amc::_db.genfield.R);// defensive

    if (amc::_db.cmdline.report) {
        prlog(amc::_db.report);
    }
}

// -----------------------------------------------------------------------------

// Go over `amcdb.gen` table and invoke each global generator
// Then, go over namespaces
//    Go over per-namespace `amcdb.gen` entries and invoke them.
// Collect performance data long the way
// This is where amc spends most of the time. See the `amcdb.gen` table for more information.
void amc::Main_Gen() {
    int prev_err=0;
    // run non-per-namespace generators
    ind_beg(amc::_db_gen_curs,gen,amc::_db) if (!gen.perns) {
        u64 c=algo::get_cycles();
        _db.cur_gen=gen.gen;
        gen.step();
        CheckCumulativeError(gen,prev_err);
        gen.cycle_total += algo::get_cycles()-c;
    }ind_end;
    // run per-namespace generators
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        if (ns.select) {
            ind_beg(amc::_db_zs_gen_perns_curs,gen,amc::_db) {
                u64 c=algo::get_cycles();
                amc::_db.c_ns=&ns;
                _db.cur_gen=gen.gen;
                gen.step();
                CheckCumulativeError(gen,prev_err);
                gen.cycle_total += algo::get_cycles()-c;
            }ind_end;
        }
    }ind_end;
}

// ----------------------------------------------------------------------------
// OUTPUT        Generate files for the namespace(s)

void amc::Main() {
    // open in editor before loading data
    if (amc::_db.cmdline.e) {
        Main_Edit();
    }
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
        // Print generated code to stdout
        Main_Querymode();
    } else {
        // Save files
        if (algo_lib::_db.exit_code==0) {
            amc::_db.report.n_filemod += amc::SaveTuples(DirFileJoin(amc::_db.cmdline.out_dir,"data"));
        } else {
            prerr("amc.no_output"
                  <<Keyval("comment","no files were modified"));
        }
    }

    Main_Report();
}
