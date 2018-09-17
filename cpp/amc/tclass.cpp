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
// Source: cpp/amc/tclass.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Call all tfunc generators for specified tclass.
// Context is provided via _db.genfield: (which needs to be renamed)
// - genfield.p_field   --- field pointer, possibly NULL
// - genfield.p_ctype   --- current ctype, never NULL
void amc::GenTclass(amc::FTclass &tclass) {
    amc::FNs &ns = *amc::_db.genfield.p_ctype->p_ns;
    tclass.step();
    ind_beg(amc::tclass_c_tfunc_curs,tfunc,tclass) {
        amc::_db.genfield.p_tfunc = &tfunc;
        bool skip = tfunc.hasthrow && !GenThrowQ(ns);
        if (!skip) {
            tfunc.step();
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

amc::FField *amc::GetBasepool(amc::FField &field) {
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FField *ret = field.c_basepool ? field.c_basepool->p_base : NULL;
    if (!ret) {
        if (ns.c_nsx) {
            ret = ns.c_nsx->p_pool;
            if (!ret) {
                ret = amc::_db.c_malloc;
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Call all applicable generators for specified field
static void GenTclass_Field(amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FNs &ns = *field.p_ctype->p_ns;
    bool glob = GlobalQ(*field.p_ctype);
    amc::FCtype& parent = *field.p_ctype;
    tempstr parname = Refname(parent);// _db or pool name, or the word 'parent'
    amc::FField *pool=FirstInst(parent);

    // algo_lib::Replscope's Ins function automatically strips trailing . and ', ' after replacing
    // an empty string.
    // depending on syntactic context, either _db or empty string are used.
    Set(R, "$ns"       , ns.ns);
    Set(R, "$name"     , name_Get(field));
    Set(R, "$field"    , field.field);
    // name with which we refer to the parent.
    Set(R, "$parname"  , parname);
    Set(R, "$pararg"   , glob ? strptr("")    : parname);
    Set(R, "$Parent"   , glob ? strptr("")    : strptr(tempstr() << parent.cpp_type << "& " << parname));
    Set(R, "$Cparent"  , glob ? strptr("")    : strptr(tempstr() << "const "<<parent.cpp_type << "& " << parname));
    // same as $parname, but use ctype name instead of the word 'parent'
    Set(R, "$Parname"  , (pool ? strptr(name_Get(*pool)) : strptr(name_Get(parent))));
    Set(R, "$Partype"  , parent.cpp_type);
    Set(R, "$Cpptype"  , field.p_arg->cpp_type);
    Set(R, "$Ctype"    , name_Get(*field.p_arg));

    if (field.p_reftype->usebasepool) {
        amc::FField* basepool=GetBasepool(field);
        Set(R, "$basepool", tempstr() << ns_Get(*basepool->p_ctype) << "::" << name_Get(*basepool));
    }

    GenTclass(amc_tclass_Field);

    if (field.p_ctype->c_pmaskfld) {
        GenTclass(amc_tclass_Pmask);
    }
    if (field.c_fdec) {
        GenTclass(amc_tclass_Dec);
    }
    if (field.c_fbitset) {
        GenTclass(amc_tclass_Bitset);
    }
    if (field.c_fcmp) {
        GenTclass(amc_tclass_Cmp);
    }
    GenTclass(*field.p_reftype->p_tclass);

    if (field.c_smallstr && field.c_smallstr->c_numstr) {
        GenTclass(amc_tclass_Numstr);
    }
    if (c_fconst_N(field)) {
        GenTclass(amc_tclass_Fconst);
    }
    if (field.c_substr) {
        GenTclass(amc_tclass_Substr);
    }
    if (field.c_cppfunc) {
        GenTclass(amc_tclass_Cppfunc);
    }
    if (field.c_fcast) {
        GenTclass(amc_tclass_Fcast);
    }
    if (field.c_fstep) {
        GenTclass(amc_tclass_Step);
    }

    GenTclass(amc_tclass_Field2);

    // ! destructive
    if (field.c_fsort) {
        GenTclass(amc_tclass_Sort);
    }
    // ! destructive
    if (field.c_gstatic || field.c_finput || field.c_foutput) {
        GenTclass(amc_tclass_Io);
    }
}

// -----------------------------------------------------------------------------

// Call tfunc generators for every field in this ctype
void amc::GenTclass_Fields(amc::FCtype &ctype) {
    amc::_db.genfield.p_ctype = &ctype;
    ind_beg(amc::ctype_c_field_curs, field,ctype) {
        ind_replvar_Cascdel(amc::_db.genfield.R);
        amc::_db.genfield.p_field = &field;
        GenTclass_Field(field);
        field.processed=true;
    }ind_end;
    amc::_db.genfield.p_ctype = NULL;
    amc::_db.genfield.p_field = NULL;
}

// -----------------------------------------------------------------------------

// Call tfunc generators without field context (Ctype generators)
// This must be called after field-specific generators, since by this time
// ctype sizes have been computed.
void amc::gen_ns_ctype() {
    amc::FNs &ns =*amc::_db.c_ns;
    amc::_db.genfield.p_field = NULL;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        amc::_db.genfield.p_ctype = &ctype;
        GenTclass(amc_tclass_Ctype);
    }ind_end;
    amc::_db.genfield.p_ctype = NULL;
}
