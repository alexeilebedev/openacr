// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/tclass.cpp -- Driver for tfuncs
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::ResetVars(amc::Genctx &ctx) {
    algo_lib::Replscope &R = ctx.R;
    amc::FNs &ns = *ctx.p_ns;
    amc::FCtype *parent = ctx.p_ctype;
    amc::FField *field = ctx.p_field;
    ind_replvar_Cascdel(R);// clean out varsx
    Set(R, "$ns", ns.ns);

    if (parent) {
        tempstr parname = Refname(*parent);// _db or pool name, or the word 'parent'
        amc::FField *pool=FirstInst(*parent);
        bool glob = GlobalQ(*parent);

        Set(R, "$Partype"  , parent->cpp_type); // parent's c++ type, e.g. abt::FTarget
        Set(R, "$Name"     , name_Get(*parent)); // name without namespace, e.g. FTarget
        Set(R, "$Parname"  , (pool ? strptr(name_Get(*pool)) : strptr(name_Get(*parent)))); // target or FTarget
        Set(R, "$parname"  , parname); // target
        Set(R, "$pararg"   , glob ? strptr("") : parname); // target, or empty string -- parent's name when passed as argument to function
        Set(R, "$Parent"   , glob ? strptr("") : strptr("$Partype& $parname")); // abt::FTarget& target
        Set(R, "$Cparent"  , glob ? strptr("") : "const $Parent");

        if (field) {
            Set(R, "$field"    , field->field);
            Set(R, "$name"     , name_Get(*field));
            Set(R, "$xfname"   , glob ? "$name" : "$parname_$name");// prefix for fields added to child record - must include parent name for disambiguation
            Set(R, "$Cpptype"  , field->p_arg->cpp_type);
            Set(R, "$Ctype"    , name_Get(*field->p_arg));
            if (field->p_reftype->usebasepool) {
                amc::FField* basepool=GetBasepool(*field);
                Set(R, "$basepool", tempstr() << ns_Get(*basepool->p_ctype) << "::" << name_Get(*basepool));
            }
        }
    }
}
// -----------------------------------------------------------------------------

// Call tclass, tfunc, and cursor generators for this template
// Context is provided via _db.genctx:
// - genctx.p_field   --- field pointer (NULL for tclass, cursor, and Ctype, set for individual fields)
// - genctx.p_ctype   --- current ctype, never NULL
// - genctx.p_tfunc   --- pointer to tfunc, never NULL
// First, the tclass function is called
// Then, for each tfunc, its function is called
// The tfunc may be a cursor generator (indicated by its tcurs row, or by
// its name when the row is absent from the input set).
// If it's a cursor generator, then a forward-declaration for the cursor is created.
// In this case, variables $fcurs (cursor key) and $curstype (cursor type) are set.
void amc::GenTclass(amc::FTclass &tclass) {
    amc::FNs &ns = *amc::_db.genctx.p_ns;
    tclass.step();
    ind_beg(amc::tclass_c_tfunc_curs,tfunc,tclass) {
        amc::_db.genctx.p_tfunc = &tfunc;
        bool skip = tfunc.hasthrow && !GenThrowQ(ns);
        // Cursor generator: skip unless the cursor generates by default or
        // an explicit dmmeta.fcurs row requests it. The amcdb.tcurs row is
        // the only carrier of the dflt flag, and a universe that omits the
        // row (a single-file fixture) must not receive every index cursor
        // unconditionally, so the tfunc is recognized by its name as well
        // and defaults to request-only. Tcurs.curstype is a substring of
        // the tfunc key, so the cursor key and type derive from the name
        // alike. Cursors walk a field; a cursor tfunc reached without a
        // field context (a ctype- or ns-level tclass) generates nothing.
        // (The Ptrary/Llist generators require the tcurs row itself when
        // they run -- see RequireTcurs.)
        if (tfunc.c_tcurs || EndsWithQ(name_Get(tfunc),"curs")) {
            amc::FField *field = amc::_db.genctx.p_field;
            bool dflt = tfunc.c_tcurs && tfunc.c_tcurs->dflt;
            if (!field) {
                skip = true;
            } else {
                tempstr key(dmmeta::Fcurs_Concat_field_curstype(field->field,name_Get(tfunc)));
                if (!dflt && amc::ind_fcurs_Find(key) == NULL) {
                    skip = true;
                }
                algo_lib::Replscope &R = amc::_db.genctx.R;
                Set(R,"$fcurs",key);
                Set(R,"$curstype",name_Get(tfunc));
                if (!skip) {
                    amc::ind_fwddecl_GetOrCreate(Subst(R,"$ns.$ns.$Parname_$name_$curstype"));
                }
            }
        }
        if (!skip) {
            tfunc.step();
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// A cursor generator's amcdb.tcurs row is part of its input set: a universe
// that reaches the generator without the row is incomplete, so the missing
// row is reported as a generation error naming the tfunc and the field.
// Generation continues -- the cursor name derives from the tfunc name, so
// the emitted cursor is the same one the complete universe produces -- and
// the error count withholds all output. Called by the generators that
// consume $curstype (Ptrary, Llist) before they emit anything.
void amc::RequireTcurs() {
    amc::FTfunc &tfunc = *amc::_db.genctx.p_tfunc;
    if (!tfunc.c_tcurs) {
        prerr("amc.missing_tcurs"
              <<Keyval("tfunc",tfunc.tfunc)
              <<Keyval("field",amc::_db.genctx.p_field->field)
              <<Keyval("comment","cursor generator without its amcdb.tcurs row; add the row to the input set"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

// Get pointer to the field's memory pool
// Any field can be assigned a custom memory pool with the basepool table.
// Field doesn't have a custom pool, then the namespace custom pool (as specified in nsx table)
// is used.
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
    GenTclass(amc_tclass_Field);

    if (c_pmaskfld_N(*field.p_ctype)) {
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
// Each field triggers zero or more tclass generators
// (template class, no relation to C++ notion of template or class)
// based on its type and associated records, and each tclass generates zero or more tfuncs
void amc::gen_ns_tclass_field() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nscpp) {
        amc::_db.genctx.p_ns = &ns;
        amc::BeginNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            amc::_db.genctx.p_ctype = &ctype;
            ind_beg(amc::ctype_c_field_curs, field,ctype) {
                amc::_db.genctx.p_field = &field;
                ResetVars(amc::_db.genctx);
                GenTclass_Field(field);
                field.processed=true;
            }ind_end;
            amc::_db.genctx.p_ctype = NULL;
            amc::_db.genctx.p_field = NULL;
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
    }ind_end;
}

// -----------------------------------------------------------------------------

// Call tfunc generators without field context (Ctype generators)
// This must be called after field-specific generators, since by this time
// ctype sizes have been computed.
void amc::gen_ns_tclass_ctype() {
    amc::FNs &ns =*amc::_db.c_ns;
    amc::_db.genctx.p_ns = amc::_db.c_ns;
    amc::_db.genctx.p_field = NULL;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        amc::_db.genctx.p_ctype = &ctype;
        ResetVars(amc::_db.genctx);
        GenTclass(amc_tclass_Ctype);
        // 2nd pass
        GenTclass(amc_tclass_Ctype2);
    }ind_end;
    amc::_db.genctx.p_ctype = NULL;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_tclass_ns() {
    amc::_db.genctx.p_ns = amc::_db.c_ns;
    amc::_db.genctx.p_ctype = NULL;
    amc::_db.genctx.p_field = NULL;
    ResetVars(amc::_db.genctx);
    GenTclass(amc_tclass_Ns);
}
