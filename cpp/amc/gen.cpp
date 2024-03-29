// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
// Source: cpp/amc/gen.cpp
//
// All the functions starting with gen_

#include "include/amc.h"

// -----------------------------------------------------------------------------

amc::FField *amc::InsField(const dmmeta::Field &field) {
    amc::FField *ffield = amc::field_InsertMaybe(field);
    vrfy(ffield
         , tempstr()<<"amc.badfield"
         <<Keyval("field",field.field)
         <<Keyval("comment","could not create new field"));
    vrfy(!ffield->p_ctype->c_bltin
         , tempstr()<<"amc.bltin_extend"
         <<Keyval("field",field.field)
         <<Keyval("comment","cannot extend bltin type"));
    return ffield;
}

// -----------------------------------------------------------------------------

// Move FIELD so that it appears immediately after field AFTER.
void amc::PlaceFieldAfter(amc::FField &field, amc::FField &after) {
    if (field.p_ctype == after.p_ctype) {
        field.rowid = after.rowid + 0.001;
        c_field_QuickSort(*field.p_ctype);
    }
}

// -----------------------------------------------------------------------------

bool amc::HasAllocQ(amc::FField &field) {
    amc::FCtype &parent = *field.p_ctype;
    bool has_alloc = field.reftype == dmmeta_Reftype_reftype_Tary && GlobalQ(parent);
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Lary;
    has_alloc     |= WidthMax(field) > WidthMin(field) && GlobalQ(parent);
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Tpool;
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Malloc;
    // no traces for u8 pool -- it's just bytes
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Lpool && field.arg != "u8";
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Sbrk;
    has_alloc     |= field.reftype == dmmeta_Reftype_reftype_Blkpool;
    return has_alloc;
}

// -----------------------------------------------------------------------------

void amc::gen_usedns() {
    amc::GenUsedNs();
}

void amc::gen_include() {
    amc::GenInclude();

    ind_beg(amc::_db_ns_curs, ns,amc::_db) if (ch_N(ns.ns)) {
        tempstr glob_text;
        if (ExeQ(ns)) {
            glob_text << eol;
            glob_text << "// Instantiate all libraries linked into this executable,\n";
            glob_text << "// in dependency order\n";
            ind_beg(amc::ns_c_parentns_curs, parentns, ns) if (parentns.c_globfld) {
                glob_text << parentns.ns<<"::FDb \t"<<parentns.ns<<"::"<<name_Get(*parentns.c_globfld)
                          <<";  \t// dependency found via dev.targdep " << eol;
            }ind_end;
            glob_text << eol;
        }
        *ns.cpp << Tabulated(glob_text, "\t", "ll",2);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_check_main() {
    amc::FNs &ns=*amc::_db.c_ns;
    if (ns.c_main) {
        bool ok = ns.nstype == dmmeta_Nstype_nstype_exe
            || ns.nstype == dmmeta_Nstype_nstype_lib;
        amccheck(ok,"amc.main_course"
                 <<Keyval("main",ns.ns)
                 <<Keyval("comment","Main is only allowed on an executable or a library"));
    }
}

// -----------------------------------------------------------------------------

void amc::gen_check_cascdel() {
    // todo: disable deletion for fields which have xrefs down to fields
    // that cannot be randomly deleted.
    ind_beg(amc::_db_cascdel_curs,cascdel,amc::_db) {
        amc::FField &field=*cascdel.p_field;
        int ninst =0;
        ind_beg(amc::ctype_zd_access_curs,access,*field.p_arg) {
            ninst += access.p_reftype->isval;
        }ind_end;
        if (ninst > 1) {
            prerr("amc.too_many_inst"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","Unclear which instance of field to delete on cascdel"));
            ind_beg(amc::ctype_zd_access_curs,access,*field.p_arg) if (access.p_reftype->isval) {
                prerr("#  "
                      <<Keyval("field",access.field)
                      <<Keyval("reftype",access.reftype)
                      <<Keyval("comment","Could be this one"));
            }ind_end;
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_countxref() {
    ind_beg(amc::_db_xref_curs, xref,amc::_db) {
        xref.p_field->p_arg->n_xref++;
    }ind_end;
}

// ----------------------------------------------------------------------------

// Check that a struct marked cheap_copy does not contain structs not marked that way
void amc::gen_check_cheapcopy() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) if (ctype.c_cpptype && ctype.c_cpptype->cheap_copy) {
        ind_beg(amc::ctype_c_field_curs, field, ctype)  if (field.reftype == dmmeta_Reftype_reftype_Val) {
            amccheck(!field.p_arg->c_cpptype || field.p_arg->c_cpptype->cheap_copy, "amc.check_cheap_copy"
                     <<Keyval("ctype",ctype.ctype)
                     <<Keyval("field",field.field)
                     <<Keyval("field_type",field.arg)
                     <<Keyval("comment","Ctype is marked cheap_copy but child type isn't"));
        }ind_end;
    }ind_end;
}

// ----------------------------------------------------------------------------

void amc::gen_check_static() {
    // check if gstatic field has references to fields which are finput
    ind_beg(amc::_db_gstatic_curs, gstatic, amc::_db) {
        // Ptrary can be both xref and non-xref -- for now assume it's xref
        ind_beg(amc::ctype_c_field_curs, field, *gstatic.p_field->p_arg) {
            bool isxref = field.p_reftype->isxref;
            isxref |= (field.reftype == dmmeta_Reftype_reftype_Ptrary);
            isxref |= (field.reftype == dmmeta_Reftype_reftype_Ptr);
            isxref |= (field.reftype == dmmeta_Reftype_reftype_Hook);
            amc::FField *inst = amc::FirstInst(*field.p_arg);
            if (!isxref && inst && inst->c_finput) {
                amccheck(0,"amc.gstatic_uses_finput"
                         <<Keyval("ctype",gstatic.p_field->arg)
                         <<Keyval("uses",field.arg)
                         <<Keyval("via",field.field)
                         <<Keyval("reftype",field.reftype)
                         <<Keyval("comment","A statically loaded table cannot refer to a dynamically loaded one."));
            }
            if (!isxref && inst && inst->c_gstatic && gstatic.p_field->p_arg->topo_idx < inst->c_gstatic->p_field->p_arg->topo_idx) {
                amccheck(0,"amc.gstatic_dep"
                         <<Keyval("ctype",gstatic.p_field->arg)
                         <<Keyval("uses",field.arg)
                         <<Keyval("via",field.field)
                         <<Keyval("comment","An earlier static table is referring to a later table"));
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool MatchPrefix(strptr prefix, amc::FReftype &reftype) {
    bool ret=amc::zs_fprefix_EmptyQ(reftype)
        || (prefix=="c" && reftype.reftype==dmmeta_Reftype_reftype_Ptr);
    ind_beg(amc::reftype_zs_fprefix_curs,fprefix,reftype) {
        if (fprefix.fprefix==prefix) {
            ret=true;
            break;
        }
    }ind_end;
    return ret;
}

// ----------------------------------------------------------------------------

void amc::gen_check_prefix() {
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        amc::FReftype &reftype = *field.p_reftype;
        if (!MatchPrefix(Pathcomp(name_Get(field),"_LL"),reftype)) {
            prerr("amc.bad_prefix"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",reftype.reftype)
                  <<Keyval("comment","See dmmeta.fprefix table for allowable combinations"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_basepool() {
    ind_beg(amc::_db_field_curs,field,amc::_db) if (field.p_reftype->usebasepool && !field.c_basepool) {
        amc::FNs &ns = *field.p_ctype->p_ns;
        amc::FField* basepool= ns.c_nsx ? ns.c_nsx->p_pool : amc::_db.c_malloc;
        vrfy(basepool, tempstr()
             <<"amc.missingpool"
             <<Keyval("ns",ns.ns)
             <<Keyval("comment","missing nsx record for namespace"));
        amc::basepool_InsertMaybe(dmmeta::Basepool(field.field,basepool->field));
    }ind_end;
}

// ----------------------------------------------------------------------------

static bool NeedNsxQ(amc::FNs &ns) {
    return ns.nstype == dmmeta_Nstype_nstype_exe
        || (ns.nstype == dmmeta_Nstype_nstype_objlist && amc::ind_ctype_Find(tempstr()<<ns.ns<<".FDb"));
}

// TODO: also check that targdep path exists between namespaces?
void amc::gen_check_basepool() {
    // Check that namespaces that require use of dmmeta.nsx have one defined
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (!ns.c_nsx && NeedNsxQ(ns)) {
        prerr("amc.nsx_required"
              <<Keyval("ns",ns.ns)
              <<Keyval("comment","this namespace requires an nsx record like one shown below"));
        prerr("dmmeta.nsx"
              <<Keyval("ns",ns.ns)
              <<Keyval("genthrow","Y")
              <<Keyval("pool","algo_lib.FDb.malloc"));
        algo_lib::_db.exit_code++;
    }ind_end;
    // Validate nsx records
    ind_beg(amc::_db_nsx_curs, nsx, amc::_db) {
        // Check that basepool is defined if the pool specified for nsx requires one
        // (Basepool provides backing for another pool)
        if (nsx.p_pool->p_reftype->usebasepool && !nsx.p_pool->c_basepool) {
            prerr("amc.bad_nsaloc"
                  <<Keyval("nsx",nsx.ns)
                  <<Keyval("pool",nsx.pool)
                  <<Keyval("comment","Default namespace pool must have a basepool"));
            algo_lib::_db.exit_code++;
        }
        // Simple check for circular dependency (base pool pointing to itself)
        if (nsx.p_pool->c_basepool && nsx.p_pool->c_basepool->p_base == nsx.p_pool) {
            prerr("amc.bad_nsaloc"
                  <<Keyval("nsx",nsx.ns)
                  <<Keyval("pool",nsx.pool)
                  <<Keyval("comment","Default namespace pool must have a basepool different from itself"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    // if field and basepool occur within the same ctype, basepool must come first...
    // there isn't much else we can do here.
    ind_beg(amc::_db_basepool_curs, basepool, amc::_db) {
        amc::FField &field = *basepool.p_field;
        if (!GlobalQ(*basepool.p_base->p_ctype)) {
            prerr("amc.nonglobal_basepool"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","Basepool must be global"));
            algo_lib::_db.exit_code++;
        }
        if (field.p_ctype == basepool.p_base->p_ctype && field.rowid < basepool.p_base->rowid) {
            prerr("amc.late_basepool"
                  <<Keyval("field",field.field)
                  <<Keyval("basepool",basepool.base)
                  <<Keyval("comment","Basepool must be declared before field that uses it."));
            algo_lib::_db.exit_code++;
        }
        if (!basepool.p_field->p_reftype->usebasepool) {
            prerr("acr.delete  dmmeta.basepool"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","Basepool record is not relevant here."));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// ----------------------------------------------------------------------------

void amc::gen_check_bitfld() {
    // check that bitfields don't overlap
    // only handle bitfields up to 16 bytes
    ind_beg(amc::_db_field_curs, srcfield, amc::_db) {
        u128 value=0;
        ind_beg(amc::field_bh_bitfld_curs,bitfld,srcfield) {
            u128 mask = ((u128(1) << bitfld.width)-1) << bitfld.offset;
            if ((value & mask) != 0) {
                prerr("amc.overlapbitfld"
                      <<Keyval("bitfld",bitfld.field)
                      <<Keyval("srcfield",srcfield.field)
                      <<Keyval("comment","Bit fields are not allowed to overlap"));
                algo_lib::_db.exit_code++;
            }
            value |= mask;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static amc::FField *ResolvePkey(amc::FField &field) {
    amc::FField *root = &field;
    int niter=0;
    while (root->reftype == dmmeta_Reftype_reftype_Pkey) {
        if (c_field_N(*root->p_arg) == 0) {
            amccheck(0,"amc.pkey_error1"
                     <<Keyval("field",field.field)
                     <<Keyval("comment","Pkey target has no fields"));
            vrfy(0,"fatal error");
        }
        if (niter++ >= 100) {
            amccheck(0,"amc.pkey_error3"
                     <<Keyval("field",field.field)
                     <<Keyval("comment","Pkey chain too long (cycle?)"));
            vrfy(0,"fatal error");
        }
        root = c_field_Find(*root->p_arg,0);
        amccheck(!(root->reftype == dmmeta_Reftype_reftype_Base)
                 ,"amc.pkey_base"
                 <<Keyval("field",field.field)
                 <<Keyval("comment","Pkey target cannot be a Base field"));
        amccheck(!(root->reftype == dmmeta_Reftype_reftype_Ptr)
                 ,"amc.pkey_ptr"
                 <<Keyval("field",field.field)
                 <<Keyval("comment","No pointer type defined"));
    }
    amccheck(!(root->reftype != dmmeta_Reftype_reftype_Val && root->reftype != dmmeta_Reftype_reftype_Upptr)
             ,"amc.pkey_error4"
             <<Keyval("field",field.field)
             <<Keyval("root_field",root->field)
             <<Keyval("root_reftype",root->reftype)
             <<Keyval("comment","Root of Pkey chain must be Val or Ptr"));
    return root;
}

// -----------------------------------------------------------------------------

// Rewrite reftype:Pkey fields into reftype:Val
void amc::gen_lookuppkey() {
    ind_beg(amc::_db_field_curs, field,amc::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
            amc::FField *root = field.p_arg->c_pkeyfield;
            if (!root) {
                root =ResolvePkey(field);
                field.p_arg->c_pkeyfield = root;// save it
            }
            field.reftype   = root->reftype;
            field.arg       = root->arg;
            field.p_arg     = root->p_arg;
            field.p_reftype = root->p_reftype;
            field.reftype   = root->reftype;
            if (!FldfuncQ(field) && !ch_N(field.dflt.value)) {
                field.dflt = root->dflt;
            }
        } else if (c_field_Find(*field.p_ctype,0) == &field) {
            // first field of every table is its pkey
            if (field.reftype == dmmeta_Reftype_reftype_Val) {
                if (!field.p_ctype->c_pkeyfield && field.p_ctype->p_ns->nstype == dmmeta_Nstype_nstype_ssimdb) {
                    field.p_ctype->c_pkeyfield = &field;
                }
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Ensure fregx record exists for each field of type Regx or RegxSql
// Rewrite RegxSql fields as Regx, regxtype Sql
// (generalization of original RegxSql reftype)
void amc::gen_rewrite_regx() {
    ind_beg(amc::_db_field_curs, field,amc::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_RegxSql) {
            field.reftype = dmmeta_Reftype_reftype_Regx;
            field.p_reftype = ind_reftype_Find(field.reftype);
        }
        if (field.reftype == dmmeta_Reftype_reftype_Regx) {
            if (!field.c_fregx) {
                amc::FFregx &fregx = amc::fregx_Alloc();
                fregx.field=field.field;
                fregx.partial=false;
                fregx.regxtype=amcdb_regxtype_Sql;
                vrfy_(fregx_XrefMaybe(fregx) && field.c_fregx==&fregx);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CheckReftype(amc::FField &field, strptr reftype, bool haschild, strptr ssimfile, cstring &err) {
    if (haschild     && !(field.reftype == reftype    )) {
        err << "Field reftype must be a " << reftype;
    }
    if (field.reftype == reftype  && !haschild) {
        err << "Missing "<<ssimfile<<" record";
    }
}

// -----------------------------------------------------------------------------

void amc::gen_check_reftype() {
    ind_beg(amc::_db_field_curs, field,amc::_db) {
        tempstr err;
        if (field.p_ctype->c_varlenfld == &field && !(field.reftype == dmmeta_Reftype_reftype_Varlen)) {
            err = "Field reftype must be Varlen";
        }
        CheckReftype(field, dmmeta_Reftype_reftype_Hook, field.c_hook, dmmeta_Ssimfile_ssimfile_dmmeta_hook, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Tary, field.c_tary, dmmeta_Ssimfile_ssimfile_dmmeta_tary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Ptrary, field.c_ptrary, dmmeta_Ssimfile_ssimfile_dmmeta_ptrary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Inlary, field.c_inlary, dmmeta_Ssimfile_ssimfile_dmmeta_inlary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Thash, field.c_thash, dmmeta_Ssimfile_ssimfile_dmmeta_thash, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Smallstr, field.c_smallstr, dmmeta_Ssimfile_ssimfile_dmmeta_smallstr, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Llist, field.c_llist, dmmeta_Ssimfile_ssimfile_dmmeta_llist, err);
        if (ch_N(err)) {
            prerr("amc.missing_record"
                  <<Keyval("field",field.field)
                  <<Keyval("comment",err));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    if (algo_lib::_db.exit_code>0) {
        _exit(1);
    }
}

// -----------------------------------------------------------------------------

void amc::gen_detectinst() {
    // detect instance fields -- creation access paths
    // ignore instances from different namespaces -- assume the ctype can
    // only be created in this namespace.
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        amc::FReftype& reftype = *field.p_reftype;
        bool ispool = reftype.inst || (reftype.isval && field.c_finput);
        if (ispool && field.p_ctype->p_ns == field.p_arg->p_ns) {
            zd_inst_Insert(*field.p_arg, field);
        }
    }ind_end;

    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        if (amc::FField *pool = amc::FirstInst(ctype)) {
            amc::FNs& ns = *ctype.p_ns;
            if (pool->reftype == dmmeta_Reftype_reftype_Global) {
                c_globfld_InsertMaybe(ns,*pool);// more than 1 will give an error
            }
        }
    }ind_end;

    // a x-referenced field target have be a pool
    ind_beg(amc::_db_xref_curs, xref, amc::_db) {
        amccheck(HasPoolQ(*xref.p_field->p_arg)
                 ,"amc.nopool"
                 <<Keyval("field",xref.field)
                 <<Keyval("ctype",xref.p_field->arg));
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool BadDtorQ(amc::FField &field) {
    return field.p_reftype->isval
        && (!field.p_arg->c_cpptype || field.p_arg->c_cpptype->dtor)
        && !field.p_arg->c_cextern;
}

void amc::gen_check_cpptype() {
    ind_beg(amc::_db_cpptype_curs,cpptype,amc::_db) {
        amc::FCtype &ctype = *cpptype.p_ctype;
        if (!cpptype.dtor) {
            ind_beg(amc::ctype_c_field_curs,field,ctype) {
                if (BadDtorQ(field)) {
                    prerr("amc.check_cpptype"
                          <<Keyval("ctype",ctype.ctype)
                          <<Keyval("field",field.field)
                          <<Keyval("child_ctype",field.arg)
                          <<Keyval("comment","Field has destructor but ctype is marked dtor:N"));
                    prerr("acr.insert"
                          <<Keyval("","dmmeta.cpptype")
                          <<Keyval("ctype",field.arg)
                          <<Keyval("dtor","N"));
                    algo_lib::_db.exit_code=1;
                }
            }ind_end;
        }
        if (cpptype.ctor) {
            int nfields = NValFields(ctype);
            if (nfields > 10) {
                prerr("amc.non_constructive"
                      <<Keyval("ctype",cpptype.ctype)
                      <<Keyval("fields",nfields)
                      <<Keyval("comment","Constructor with >10 arguments is error-prone, disallowed"));
                prerr("acr.insert"
                      <<Keyval("","dmmeta.cpptype")
                      <<Keyval("ctype",cpptype.ctype)
                      <<Keyval("ctor","N")
                      <<Keyval("comment","Constructor with >10 arguments is error-prone, disallowed"));
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Count # fields, excluding substrings, cppfuncs, and bitfields
int amc::NValFields(amc::FCtype &ctype) {
    int ret=0;
    ind_beg(ctype_c_field_curs,field,ctype) {
        ret += !FldfuncQ(field) && !field.c_bitfld;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

static bool NeedFirstchangedQ(amc::FField &field) {
    // determine if field needs FirstChanged hook
    bool ret = field.c_fstep != NULL;
    // ZSListMT doesn't support fstep
    ret &= field.reftype != dmmeta_Reftype_reftype_ZSListMT;
    // Ptr doesn't need firstchanged
    ret &= field.reftype != dmmeta_Reftype_reftype_Ptr;
    ret &= field.reftype != dmmeta_Reftype_reftype_Tary;
    ret &= field.reftype != dmmeta_Reftype_reftype_Lary;
    ret &= field.reftype != dmmeta_Reftype_reftype_Thash;
    ret &= field.reftype != dmmeta_Reftype_reftype_Ptrary;
    ret &= !ValQ(field);
    return ret;
}

// -----------------------------------------------------------------------------

void amc::gen_prep_field() {
    // other field stuff -- determine ctype, set c_varlenfld, etc.
    ind_beg(amc::_db_field_curs, field,amc::_db) {
        amc::FCtype& ctype     = *field.p_ctype;

        tempstr ident(name_Get(field));
        algo::StringIter s(ident);
        if (field.c_anonfld) {
            field.c_anonfld->anon_idx  = ctype.next_anon_idx;
            ctype.next_anon_idx             = ctype.next_anon_idx + 1;
        }

        tempstr temp;
        amc::strptr_PrintCppIdent(ident, temp, true);
        amccheck(ch_N(ident)==0 || temp == ident, "amc.prep_field"
                 <<Keyval("field",field.field)
                 <<Keyval("ident",ident)
                 <<Keyval("comment","bad name for c++ identifier"));

        Field_UpdateDflt(field);
        if (field.reftype == dmmeta_Reftype_reftype_Ptr || field.reftype == dmmeta_Reftype_reftype_Upptr) {
            field.cpp_type = tempstr() << amc::NsToCpp(field.arg) << "*";
        } else {
            field.cpp_type = amc::NsToCpp(field.p_arg->ctype);
        }
        if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
            amc::c_varlenfld_InsertMaybe(ctype,field);
        }
        if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            amc::c_optfld_InsertMaybe(ctype,field);
        }
        if (field.c_substr) {
            field.c_substr->p_srcfield->has_substr=true;
        }

        if (field.reftype == dmmeta_Reftype_reftype_Varlen || field.reftype == dmmeta_Reftype_reftype_Opt) {
            amc::FCpptype *cpptype = field.p_arg->c_cpptype;
            amccheck(!(cpptype && cpptype->dtor)
                     , "amc.opt_dtor"
                     <<Keyval("ctype",cpptype->ctype)
                     <<Keyval("instance",field.field)
                     <<Keyval("reftype",field.reftype)
                     <<Keyval("comment","field cannot have cpptype.dtor flag defined"));
        }

        field.need_firstchanged = NeedFirstchangedQ(field);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_check_bigend() {
    ind_beg(amc::_db_fbigend_curs, fbigend, amc::_db) {
        amc::FField &field = *fbigend.p_field;
        vrfy(field.reftype == dmmeta_Reftype_reftype_Val
             , tempstr()<<"amc.bad_bigend"
             <<Keyval("comment","Big-endian field: only Val or Pkey is supported"));
        vrfy(field.p_arg->c_bltin && field.p_arg->c_bltin->bigendok
             , tempstr()<<"amc.bigend_deadend"
             <<Keyval("field",field.field)
             <<Keyval("type",field.arg)
             <<Keyval("comment","big-endian storage not allowed for this type"));
    }ind_end;
}

// -----------------------------------------------------------------------------


// walk over all xrefs and populate ctype.c_parent array,
// which lists all the "parent" ctypes (ones used by this ctype)
// check
void amc::gen_xref_parent() {
    ind_beg(amc::_db_xref_curs, xref, amc::_db) {
        amc::FCtype *child = xref.p_field->p_ctype;
        amc::FCtype *parent = xref.p_field->p_arg;
        if (xref.p_field->reftype != dmmeta_Reftype_reftype_Upptr) {
            algo::TSwap(child,parent);
        }
        c_parent_Insert(*child,*parent);
        amc::FCtype *base1=amc::GetBaseType(*child,child);
        amc::FCtype *base2=amc::GetBaseType(*parent,parent);
        if (base1->c_ssimfile && base2->c_ssimfile && base1->c_ssimfile->topoindex < base2->c_ssimfile->topoindex) {
            prerr("amc.bad_xref"
                  <<Keyval("field",xref.field)
                  <<Keyval("child_type",child->ctype)
                  <<Keyval("parent_type",parent->ctype)
                  <<Keyval("child_ssimfile",base1->c_ssimfile->ssimfile)
                  <<Keyval("parent_ssimfile",base2->c_ssimfile->ssimfile)
                  <<Keyval("comment","The x-ref relationship between child_type and parent_type implies a different ordering"
                           "than the Pkey relationship between child_ssimfile and parent_ssimfile"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

void amc::gen_datafld() {
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        if (!FldfuncQ(field) && !field.c_bitfld) {
            c_datafld_Insert(*field.p_ctype,field);
        }
    }ind_end;
}

void amc::gen_ctype_toposort() {
    // topological sort of ctypes within same namespace.
    // determine type properties
    ind_beg(amc::_db_ns_curs, ns,amc::_db) {
        amc::c_ctype_sorted_RemoveAll();
        // There is no Addary function for Ptrary, only for Tary.
        // But I need one here.
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            amc::c_ctype_sorted_Insert(ctype);
        }ind_end;
        amc::c_ctype_sorted_QuickSort();
        c_ctype_RemoveAll(ns);
        ind_beg(amc::_db_c_ctype_sorted_curs, ctype, amc::_db) {
            TopoSortVisit(ns, ctype);
        }ind_end;
        vrfy(amc::c_ctype_sorted_N() == c_ctype_N(ns), "Internal: bad topo sort");
    }ind_end;
}

tempstr amc::Argtype(amc::FField &field) {
    tempstr retval;
    // determine arg type
    if (field.reftype == dmmeta_Reftype_reftype_Varlen && field.p_arg->c_lenfld) {
        retval  = "algo::memptr";
    } else if (FixaryQ(field) || field.reftype == dmmeta_Reftype_reftype_Varlen) {
        retval  = tempstr() << "algo::aryptr<" << amc::NsToCpp(field.p_arg->ctype) << " >";// varlen, array: by aryptr
    } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        retval  = tempstr() << amc::NsToCpp(field.p_arg->ctype) << "*"; // optional parameter: by pointer
    } else if (PtrQ(field)) { // pointer: make it cheap copy!
        retval = field.cpp_type;
    } else if (field.p_arg->c_cstr && field.p_arg->c_cstr->strequiv) {// copy cpptype??
        retval = "const algo::strptr&";
    } else if (field.p_arg->c_cpptype && field.p_arg->c_cpptype->cheap_copy) { // cheap copy -> copy cpptype
        retval = field.cpp_type;
    }
    if (!ch_N(retval)) {
        retval << "const "<<amc::NsToCpp(field.p_arg->ctype) << "&";
    }
    return retval;
}

void amc::gen_prep_ctype() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        ctype.cpp_type       = amc::NsToCpp(ctype.ctype);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_copypriv() {
}

void amc::gen_xref2() {
    // for each ctype, calculate list of xrefs where the ctype is a child
    ind_beg(amc::_db_xref_curs, xref, amc::_db) {
        amc::FCtype *ctype = xref.p_field->p_arg; // first guess of child
        if (xref.p_field->reftype == dmmeta_Reftype_reftype_Upptr) {
            ctype = xref.p_field->p_ctype; // correction
        }
        xref.p_ctype = ctype;
        zs_xref_Insert(*ctype,xref);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_select_ns() {
    ind_beg(amc::_db_ns_curs, ns,amc::_db) {
        ns.select = ns.c_nscpp;
    }ind_end;
}


//
// Generate Fconst from a column of some table.
//
//
void amc::gen_gconst() {
    ind_beg(amc::_db_gconst_curs, gconst,amc::_db) {
        amc::FField &field = *gconst.p_field;
        amc::FField &namefld = *gconst.p_namefld;
        amc::FField *gconstfld = amc::ind_field_Find(gconst.idfld);
        amc::FField &idfld = ch_N(gconst.idfld) && gconstfld ? *gconstfld : namefld;
        tempstr      fname = SsimFilename("data",*namefld.p_ctype, true);
        bool is_string = field.p_arg->c_cstr != NULL && field.arg != "char";
        bool is_char = field.arg == "char";
        algo_lib::MmapFile file;
        vrfy(MmapFile_Load(file,fname),tempstr()<<"amc.load"<<Keyval("filename",fname));
        int idx = 0;
        int nrec = 0;
        ind_beg(Line_curs,line,file.text) {
            Tuple tuple;
            Tuple_ReadStrptr(tuple, line, false);
            if (attrs_N(tuple) > 0 || ch_N(tuple.head.value)) {
                dmmeta::Fconst fconst;
                tempstr name(amc::EvalAttr(tuple, namefld));
                tempstr value(amc::EvalAttr(tuple, idfld));
                // support c++ char
                if (is_char) {
                    char ch;
                    vrfy(char_ReadStrptrMaybe(ch, value), algo_lib::_db.errtext);
                    ch_RemoveAll(value);
                    char_PrintCppSingleQuote(ch, value);
                }
                // c++ integer
                if (!ch_N(gconst.idfld) && !is_string && !is_char) {
                    ch_RemoveAll(value);
                    value << idx++;
                }
                // #AL# name stays as-is, subsequent fconst generator
                // will sanitize the name when creating a C++ identifier
                if (attrs_N(tuple) > 0) {
                    fconst.fconst = tempstr() << gconst.field << "/" << name;// shouldn't this be VALUE instead???
                    fconst.value = algo::CppExpr(value);
                    fconst.comment.value = attr_GetString(tuple, "comment");// import comment, if any
                    amc::fconst_InsertMaybe(fconst);
                    nrec++;
                }
            }
        }ind_end;
        verblog("amc.load_gconst"
                <<Keyval("fname",fname)
                <<Keyval("field",field.field)
                <<Keyval("nrec", nrec));
    }ind_end;
}

// -----------------------------------------------------------------------------

//
// Generate Fconst from a column of some table.
//

void amc::gen_bitfldenum() {
    ind_beg(amc::_db_field_curs, srcfield, amc::_db) {
        ind_beg(amc::field_bh_bitfld_curs,bitfld,srcfield) if (bitfld.width==1 && bitfld.p_field->arg == "bool") {
            dmmeta::Fconst fconst;
            fconst.fconst = tempstr() << srcfield.field << "/" << name_Get(*bitfld.p_field);
            fconst.comment = algo::Comment(bitfld.field);
            // cpp_type has not yet defined, assume cpp_type = ctype
            fconst.value = algo::CppExpr(tempstr() << "("<< srcfield.p_arg->ctype << "(1)<<" << bitfld.offset << ")");
            amc::fconst_InsertMaybe(fconst);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_prep_fconst() {
    ind_beg(amc::_db_fconst_curs,fconst,amc::_db) {
        // create cpp symbol name:
        // if field is regular, then it is just the fconst symbol.
        // for anonymous field, skip the field name: get ctype name + constant
        tempstr temp;
        // zero field is anon automatically.
        if (fconst.p_field->c_anonfld) {
            temp << ctype_Get(*fconst.p_field) << "_" << name_Get(fconst);
        } else {
            temp << fconst.fconst;
        }
        amc::strptr_PrintCppIdent(temp, fconst.cpp_name, true);

        algo::StringIter s(fconst.value.value);
        // try to parse LE_STRd("c..")
        if (SkipStrptr(s, "LE_STR")) {
            tempstr errmsg;
            errmsg << "malformed constant value '" << fconst.value << "'";
            int len = s.Peek() - '0';
            vrfy(len >= 1 && len <= 8,tempstr()<< errmsg << ": missing or wrong length (allowed 1..8)" );
            s.GetChar();
            vrfy(SkipChar(s, '('),tempstr()<< errmsg << ": no open paren");
            tempstr ch;
            vrfy(cstring_ReadCmdarg(ch,s,true), tempstr() << errmsg << ": no value");
            vrfy(SkipChar(s, ')'),tempstr()<< errmsg << ": no close paren");
            vrfy(s.EofQ(),tempstr()<< errmsg << ": extra chars after close paren");
            vrfy(len == ch_N(ch) ,tempstr()<< errmsg << ": mismatch between declared and actual length");
            tempstr val("LE_STR");
            val << len << "(" ;
            algo::ListSep ls(",");
            ind_beg_aryptr(char,c,ch) {
                val << ls;
                char_PrintCppSingleQuote(c,val);
            }ind_end_aryptr;
            val << ")";
            fconst.cpp_value = val;
        } else {
            fconst.cpp_value = fconst.value.value;
        }
    }ind_end;

    // field constants
    ind_beg(amc::_db_field_curs,field,amc::_db) if (c_fconst_N(field)) {
        ind_beg(amc::field_c_fconst_curs,fconst,field) {
            (void)i64_ReadStrptrMaybe(fconst.int_val,fconst.value.value);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_load_gstatic() {
    ind_beg(amc::_db_gstatic_curs, gstatic,amc::_db) {
        amc::FField& field = *gstatic.p_field;
        amc::FCtype *ctype = GetBaseType(*field.p_arg,field.p_arg);
        vrfy(ctype && ctype->c_ssimfile
             , tempstr()
             <<"amc.missing_ssimfile_for_gstatic"
             <<Keyval("field",field.field)
             <<Keyval("basetype",(ctype ? algo::strptr(ctype->ctype) : algo::strptr()))
             <<Keyval("comment","cannot determine ssimfile to load."));
        tempstr      fname  = amc::SsimFilename("data", *ctype, true);
        algo_lib::MmapFile in;
        MmapFile_Load(in, fname);
        Tuple tuple;
        int nrec=0;
        ind_beg(Line_curs,line,in.text) {
            vrfy(Tuple_ReadStrptrMaybe(tuple,line), algo_lib::_db.errtext);
            if (attrs_N(tuple) > 0 && ch_N(tuple.head.value)) {
                amc::FStatictuple& row = amc::static_tuple_Alloc();
                algo::TSwap(row.tuple, tuple);
                row.ctype = field.arg;
                amc::static_tuple_XrefMaybe(row);
                nrec++;
            }
        }ind_end;
        verblog("amc.load_gstatic"
                <<Keyval("field",field.field)
                <<Keyval("ctype", ctype->ctype)
                <<Keyval("datafile",fname)
                <<Keyval("nrec", nrec));
    }ind_end;
}

// Generate numeric fconsts from string fconsts
void amc::gen_clonefconst() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        ind_beg(amc::ctype_c_field_curs, field,ctype) if (!field.c_gconst || field.c_gconst->wantenum) {
            bool is_string = field.p_arg->c_cstr != NULL && field.arg != "char";
            bool clone = c_fconst_N(field) > 0;
            ind_beg(amc::field_c_fconst_curs,fconst,field) {
                clone &= FindChar(fconst.value.value,'"')==-1;// bad-looking fconst
                clone &= FindChar(fconst.value.value,'(')==-1;// appears in c++ exprs like LE_STR...
            }ind_end;
            if (is_string && clone) {
                Main_CloneFconst_Field(field);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_exec() {
    NewFieldExec();
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_count() {
    // generate new fields for Count xref
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_Count) {
            Field_AddChild(field, dmmeta::Field(tempstr() << field.field << "_n"
                                                , "u32", dmmeta_Reftype_reftype_Val
                                                , algo::CppExpr()
                                                , algo::Comment("Number of elements in the set")));// parent: n
            Field_AddChild(field, dmmeta::Field(tempstr() << field.arg << "." << name_Get(field) << "_value"
                                                , "bool", dmmeta_Reftype_reftype_Val
                                                , algo::CppExpr()
                                                , algo::Comment("Value contributed to the sum")));// child: value
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool CanCloneBaseQ(amc::FField &base) {
    amc::FCtype &from = *base.p_arg;
    amc::FCtype &to = *base.p_ctype;
    bool retval = base.reftype == dmmeta_Reftype_reftype_Base;
    retval &= !to.fields_cloned;
    retval &= !GetBaseType(from,NULL) || from.fields_cloned;
    return retval;
}

// convert cbase records into fields of reftype Base
void amc::gen_newfield_cbase() {
    int nclone_old = 0;
    int nclone_new = 0;
    do {
        nclone_new = nclone_old;
        ind_beg(amc::_db_field_curs, basefield, amc::_db) {
            if (CanCloneBaseQ(basefield)) {
                amc::FCtype &from = *basefield.p_arg;
                amc::FCtype &to = *basefield.p_ctype;
                to.fields_cloned = true;
                nclone_new++;
                amc::CloneFields(from,to,ChildRowid(basefield.rowid),basefield);
            }
        }ind_end;
    } while (nclone_new > nclone_old);
}

// -----------------------------------------------------------------------------

// convert cbase records into fields of reftype Base
void amc::gen_check_basefield() {
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.reftype == dmmeta_Reftype_reftype_Base) {
        amccheck(c_field_Find(*field.p_ctype,0) == &field
                 , "amc.first_base"
                 <<Keyval("field",field.field)
                 <<Keyval("comment","Base field must be first"));
    }ind_end;
}


// -----------------------------------------------------------------------------

void amc::gen_newfield_sortfld() {
    ind_beg(amc::_db_sortfld_curs, sortfld, amc::_db) {
        if (!sortfld.p_sortfld->c_fcmp && !sortfld.p_sortfld->p_arg->c_bltin) {
            dmmeta::Fcmp fcmp;
            fcmp.casesens = true;
            fcmp.field    = sortfld.sortfld;
            amc::fcmp_InsertMaybe(fcmp);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_ptrary() {
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.reftype==dmmeta_Reftype_reftype_Ptrary && !field.c_ptrary) {
        dmmeta::Ptrary ptrary;
        ptrary.field = field.field;
        ptrary.unique = false;// TODO: change to field.c_xref != NULL
        amccheck(0,"amc.missing_ptrary"
                 <<Keyval("field",field.field));
        amc::ptrary_InsertMaybe(ptrary);
    }ind_end;

    // add membership flag to unique ptrarys
    // check that ptrary target is in the same namespace --
    // we will be extending it with a membership flag.
    ind_beg(amc::_db_ptrary_curs, ptrary, amc::_db) if (ptrary.unique) {
        vrfy(ptrary.p_field->p_ctype->p_ns == ptrary.p_field->p_arg->p_ns
             ,tempstr()<<"amc.foreign_ptrary"
             <<Keyval("field",ptrary.field)
             <<Keyval("arg",ptrary.p_field->arg)
             );
        dmmeta::Field newfield;
        tempstr refname = amc::Refname(*ptrary.p_field->p_ctype);
        newfield.field         = tempstr()<<ptrary.p_field->arg<<"."<<refname<<"_"<<name_Get(*ptrary.p_field)<<"_in_ary";
        newfield.arg           = "bool";
        newfield.dflt.value    = "false";
        newfield.comment.value = "membership flag";
        newfield.reftype       = dmmeta_Reftype_reftype_Val;
        InsField(newfield);
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_dispatch() {
    amc::Disp_NewField();
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_cfmt() {
    // cfmt.Argv requries cfmt.Tuple read
    ind_beg(amc::_db_cfmt_curs, cfmt,amc::_db) if (strfmt_Get(cfmt)==dmmeta_Strfmt_strfmt_Argv) {
        amc::FCfmt *tupleread=amc::ind_cfmt_Find(dmmeta::Cfmt_Concat_ctype_strfmt(ctype_Get(cfmt),dmmeta_Strfmt_strfmt_Tuple));
        if (tupleread && !tupleread) {
            prerr("amc.need_tuple_read"
                  <<Keyval("cfmt",cfmt.cfmt)
                  <<Keyval("comment","cfmt Argv print requires cfg Tuple read"));
            algo_lib::_db.exit_code=1;
        }
        if (!tupleread) {
            dmmeta::Cfmt rec(dmmeta::Cfmt_Concat_ctype_strfmt(ctype_Get(cfmt),dmmeta_Strfmt_strfmt_Tuple)
                             , dmmeta_Printfmt_printfmt_Auto
                             , true/*print*/
                             , false/*read*/
                             , ""
                             , true // genop
                             , algo::Comment());
            // OK to insert into Lary as you loop over Lary.
            amc::cfmt_InsertMaybe(rec);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool UnpackedQ(amc::FField &field) {
    return (field.p_reftype->isval || field.reftype ==dmmeta_Reftype_reftype_Base)
        && !field.p_arg->c_pack
        && !field.p_arg->c_cextern
        && !field.c_substr;
}

void amc::gen_ns_check_pack() {
    amc::FNs &ns =*amc::_db.c_ns;
    bool nspack=ns.c_nsx && ns.c_nsx->pack;
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        bool ctpack=ctype.c_pack;
        if (nspack && !ctpack) {
            amccheck(0,"amc.what_the_pack"
                     <<Keyval("ctype",ctype.ctype)
                     <<Keyval("ns",ns_Get(ctype))
                     <<Keyval("comment","Ctype in packed namespace must also be packed"));
            prerr("acr.insert"
                  <<Keyval("","dmmeta.pack")
                  <<Keyval("ctype",ctype.ctype));
        }
        if (ctpack) {
            ind_beg(amc::ctype_c_field_curs,field,ctype) if (UnpackedQ(field)) {
                amccheck(0,"amc.back_pack"
                         <<Keyval("ctype",ctype.ctype)
                         <<Keyval("field",field.field)
                         <<Keyval("child_ctype",field.arg)
                         <<Keyval("comment","Field is unpacked but parent ctype is packed"));
                prerr("acr.insert"
                      <<Keyval("","dmmeta.pack")
                      <<Keyval("ctype",field.arg));
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_check_path() {
    amc::FNs &ns =*amc::_db.c_ns;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        int nslow=0;
        int ndel=0;
        ind_beg(amc::ctype_zd_access_curs,access,ctype) {
            ndel += access.p_reftype->del;
            if (access.c_xref) {// don't count raw ptrary
                nslow += access.reftype==dmmeta_Reftype_reftype_Ptrary;
                nslow += access.reftype==dmmeta_Reftype_reftype_Llist
                    && access.c_llist && !access.c_llist->p_listtype->haveprev;
            }
        }ind_end;
        bool good=ndel==0 || nslow<2;
        amccheck(good
                 , "amc.many_slow_paths"
                 <<Keyval("ctype",ctype.ctype)
                 <<Keyval("npaths",nslow)
                 <<Keyval("comment","Impractically slow access path"));
        if (!good) {
            ind_beg(amc::ctype_zd_access_curs,access,ctype) {
                prerr("# "<<access.field<<" "<<access.reftype);
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_pkeytypedef() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.select) {
        amc::BeginNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) if (ctype.c_pkeyfield) {
            *ns.hdr<<"    typedef"
                   <<" "<<ctype.c_pkeyfield->cpp_type
                   <<" "<<name_Get(ctype)<<"Pkey"
                   <<";"<<eol;
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_enums() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.select) {
        *ns.hdr<<"// gen:ns_enums" << eol;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            amc::Main_GenEnum(ns, ctype); // experimental
        }ind_end;
    }ind_end;
}

void amc::gen_ns_field() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.select) {
        amc::BeginNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            GenTclass_Fields(ctype);
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
    }ind_end;
}

void amc::gen_ns_include() {
    // todo: is this the right place for it?
    amc::FNs &ns =*amc::_db.c_ns;
    ind_beg(amc::ns_include_curs, include, ns) {
        *ns.cpp  << include << "\n";
    }ind_end;
}

void amc::gen_ns_funcindex() {
    amc::FNs &ns =*amc::_db.c_ns;
    // manually build an index of functions
    ind_beg(amc::ns_c_func_curs, func,ns) {
        amc::FField *field = amc::ind_field_Find(field_Get(func));
        if (field) {
            c_ffunc_Insert(*field, func);
        }
        if (!field) {
            amc::FCtype *ctype = amc::ind_ctype_Find(ctype_Get(func));
            if (ctype) {
                c_ffunc_Insert(*ctype, func);
            }
        }
    }ind_end;
}

// Print static function prototype declarations
// to the cpp file
void amc::gen_ns_print_proto() {
    amc::FNs &ns =*amc::_db.c_ns;
    algo_lib::Replscope R;
    Set(R, "$ns", ns.ns);
    amc::BeginNsBlock(*ns.cpp, ns, "");
    ind_beg(amc::ns_c_func_curs, func,ns) {
        bool print = func.priv && !func.ismacro && !func.globns && !func.oper && !func.disable && !func.member;
        if (print) {
            tempstr proto;
            PrintFuncProto(func, NULL, proto);
            algo::InsertIndent(*ns.cpp, proto, 1);
        }
    }ind_end;
    amc::EndNsBlock(*ns.cpp, ns, "");
    amc::_db.lim_ind_func=amc::ind_func_N();
}

void amc::gen_ns_print_struct() {
    amc::FNs &ns =*amc::_db.c_ns;
    amc::BeginNsBlock(*ns.hdr, ns, "");
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
        if (!ctype.c_cextern) {
            amc::GenStruct(ns, ctype);
        }
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            int n_hdr = ch_N(*ns.hdr);
            ind_beg(amc::field_c_ffunc_curs, ffunc,field) {
                if (!ffunc.printed && !ffunc.ismacro && !ffunc.globns && !ffunc.disable) {
                    if (!ffunc.priv && !ffunc.oper && !ffunc.member) {
                        tempstr proto;
                        PrintFuncProto(ffunc,NULL,proto);// goes to header
                        algo::InsertIndent(*ns.hdr, proto, 0);
                    }
                    if (!ffunc.extrn) {
                        PrintFuncBody(ns,ffunc);// goes to source
                    }
                    ffunc.printed=true;
                }
            }ind_end;
            if (ch_N(*ns.hdr) > n_hdr) {
                *ns.hdr << eol;
            }
        }ind_end;
        ind_beg(amc::ctype_c_ffunc_curs, ffunc, ctype) {
            if (!ffunc.printed && !ffunc.ismacro && !ffunc.globns && !ffunc.disable) {
                if (!ffunc.priv && !ffunc.oper && !ffunc.member) {
                    tempstr proto;
                    PrintFuncProto(ffunc,NULL,proto);// goes to header
                    algo::InsertIndent(*ns.hdr, proto, 0);
                }
                if (!ffunc.extrn) {
                    PrintFuncBody(ns,ffunc);// goes to source
                }
                ffunc.printed=true;
            }
        }ind_end;
    }ind_end;
    amc::EndNsBlock(*ns.hdr, ns, "");
}

void amc::gen_ns_curstext() {
    amc::FNs &ns =*amc::_db.c_ns;
    if (ch_N(ns.curstext)) {
        amc::BeginNsBlock(*ns.hdr, ns, "");
        *ns.hdr << ns.curstext;
        Refurbish(ns.curstext);// free up ram
        amc::EndNsBlock(*ns.hdr, ns, "");
    }
}

void amc::gen_ns_pnew() {
    amc::FNs &ns =*amc::_db.c_ns;
    ind_beg(amc::ns_c_pnew_curs, pnew, ns) {
        amc::GenPnew(ns,pnew,*pnew.p_ctype);
    }ind_end;
}

void amc::gen_ns_func() {
    amc::FNs &ns =*amc::_db.c_ns;
    algo_lib::Replscope R;
    Set(R, "$ns", ns.ns);
    amc::BeginNsBlock(*ns.hdr, ns, "");
    ind_beg(amc::ns_c_func_curs, func,ns) {
        if (!func.printed && !func.ismacro && !func.disable && !func.member && !func.globns) {
            if (!func.priv && !func.oper) {
                tempstr proto;
                PrintFuncProto(func, NULL, proto);
                algo::InsertIndent(*ns.hdr, proto, 0);
            }
            if (!func.extrn) {
                PrintFuncBody(ns, func);
            }
            func.printed = true;
        }
    }ind_end;
    amc::EndNsBlock(*ns.hdr, ns, "");
    ind_beg(amc::ns_c_func_curs, func,ns) {
        if (!func.printed && !func.ismacro && !func.disable && !func.member && func.globns) {
            if (!func.priv && !func.oper) {
                tempstr proto;
                PrintFuncProto(func, NULL, proto);
                algo::InsertIndent(*ns.hdr, proto, 0);
            }
            if (!func.extrn) {
                PrintFuncBody(ns, func);
            }
            func.printed = true;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool HasStringOpQ(amc::FCtype &ctype) {
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.print) {
        if (strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_String && cfmt.genop) {
            return true;
        }
    }ind_end;
    return false;
}

static bool HasStringFcastQ(amc::FCtype &ctype) {
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (field.c_fcast && field.c_fcast->expr == "algo::strptr") {
            return true;
        }
    }ind_end;
    return false;
}

// -----------------------------------------------------------------------------

static void GenOperators(algo_lib::Replscope &R, amc::FNs &ns, amc::FCtype &ctype) {
    Set(R, "$Ctype", ctype.ctype);
    Set(R, "$Cpptype", ctype.cpp_type);
    Subst(R,"$Ctype");// mark used
    Subst(R,"$Cpptype");// mark used
    if (HasStringOpQ(ctype) && !HasStringFcastQ(ctype)) {
        Ins(&R, *ns.hdr, "inline algo::cstring &operator <<(algo::cstring &str, const $Cpptype &row);// cfmt:$Ctype.String");
        Ins(&R, *ns.inl, "");
        Ins(&R, *ns.inl, "inline algo::cstring &algo::operator <<(algo::cstring &str, const $Cpptype &row) {// cfmt:$Ctype.String");
        Ins(&R, *ns.inl, "    $Cpptype_Print(const_cast<$Cpptype&>(row), str);");
        Ins(&R, *ns.inl, "    return str;");
        Ins(&R, *ns.inl, "}");
    }
}

void amc::gen_ns_operators() {
    amc::FNs &ns =*amc::_db.c_ns;
    algo_lib::Replscope R;
    // generate "operator <<" for every print function...
    *ns.hdr<<"// gen:ns_operators" << eol;
    *ns.hdr << "namespace algo {" << eol;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        GenOperators(R,ns,ctype);
    }ind_end;
    *ns.hdr << "}" << eol;
}

// -----------------------------------------------------------------------------

// Generate code to load gstatic tables
// (added to FDb Init function)
// Gstatics are loaded in the order of their ctypes
void amc::gen_ns_gstatic() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) {
        algo_lib::Replscope R;
        Set(R, "$cur_gen", amc::_db.cur_gen);
        // insert initialization code
        ind_beg(amc::ns_c_gstatic_curs, gstatic, ns) {
            if (!ns.c_globfld) {
                prerr("amc.null_gstatic"
                      <<Keyval("gstatic",gstatic.field)
                      <<Keyval("comment","no in-memory database -- cannot load static"));
                algo_lib::_db.exit_code=1;
                break;
            }
            amc::FFunc *init = amc::init_GetOrCreate(*ns.c_globfld->p_arg);
            amc::FField& field = *gstatic.p_field;
            Set(R, "$name", name_Get(field));
            Set(R, "$gstatic", gstatic.field);
            Set(R, "$Ctype", field.p_arg->ctype);
            Ins(&R, init->body, "$name_LoadStatic(); // gen:$cur_gen  gstatic:$gstatic  load $Ctype records");
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_check_lim() {
    amccheck((u32)amc::ind_func_N() == amc::_db.lim_ind_func
             ,"amc.late_to_the_party"
             <<Keyval("func",amc::func_Find(amc::_db.lim_ind_func)->func)
             <<Keyval("comment","Function was created after checkpoint"));
}

// -----------------------------------------------------------------------------

void amc::gen_proc() {
    // create a command::xyz_proc for every
    // executable xyz
    ind_beg(amc::_db_ns_curs,ns,amc::_db) {
        if (ns.nstype == dmmeta_Nstype_nstype_exe) {
            tempstr cmdtype = tempstr() << "command." << ns.ns;
            if (ind_ctype_Find(cmdtype)) {
                dmmeta::Ctype ctype;
                ctype.ctype = tempstr() << cmdtype << "_proc";
                ctype.comment.value = tempstr() << "Subprocess: "<<ns.comment;
                amc::ctype_InsertMaybe(ctype);

                dmmeta::Field field;
                field.field = tempstr() << ctype.ctype << "."<<ns.ns;
                field.arg = cmdtype;
                field.reftype = dmmeta_Reftype_reftype_Exec;
                amc::field_InsertMaybe(field);

                dmmeta::Anonfld anonfld;
                anonfld.field = field.field;
                amc::anonfld_InsertMaybe(anonfld);
            }
        }
    }ind_end;
}

void amc::gen_check_fcurs() {
    ind_beg(_db_fcurs_curs,fcurs,_db) {
        tempstr key = amcdb::Tfunc_Concat_tclass_name(fcurs.p_field->reftype, curstype_Get(fcurs));
        amc::FTfunc *tfunc=ind_tfunc_Find(key);
        if (!tfunc || !tfunc->c_tcurs) {
            prlog("amc.cursory_examination"
                  <<Keyval("fcurs",fcurs.fcurs)
                  <<Keyval("reftype",fcurs.p_field->reftype)
                  <<Keyval("comment","Reftype doesn't support specified cursor"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

void amc::gen_check_varlen() {
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        // messages with Varlen etc cannot be cheap copy
        if ((ctype.c_varlenfld || ctype.c_optfld) && ctype.c_cpptype && ctype.c_cpptype->cheap_copy) {
            prlog("ams.a_little_too_cheap"
                  <<Keyval("ctype",ctype.ctype)
                  <<Keyval("comment","Types with Varlen/Opt fields should not be marked cheap_copy. Merge the following line to fix error"));
            prlog("dmmeta.cpptype"
                  <<Keyval("ctype",ctype.ctype)
                  <<Keyval("cheap_copy","N"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

// Generate a global list c_ssimfile_sorted
// which indexes ssimfiles in topological order
void amc::gen_sortssimfile() {
    ind_beg(amc::_db_ssimfile_curs, ssimfile, amc::_db) {
        zd_ssimfile_todo_Insert(ssimfile);
    }ind_end;
    while (amc::FSsimfile *S = amc::zd_ssimfile_todo_Last()) {
        if (bool_Update(S->topovisit,true)) {
            ind_beg(amc::ctype_c_field_curs, field, *S->p_ctype) {
                amc::FSsimfile *parent=field.p_arg->c_ssimfile;
                if (parent && field.reftype == dmmeta_Reftype_reftype_Pkey) {
                    amc::zd_ssimfile_todo_Remove(*parent);
                    amc::zd_ssimfile_todo_Insert(*parent);
                }
            }ind_end;
        } else {
            zd_ssimfile_todo_Remove(*S);
            c_ssimfile_sorted_Insert(*S);
        }
    }
    ind_beg(amc::_db_c_ssimfile_sorted_curs,ssimfile,_db) {
        ssimfile.topoindex=ind_curs(ssimfile).index;
    }ind_end;
}
