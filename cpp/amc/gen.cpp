// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/gen.cpp -- AMC Generators (amcdb.gen)
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

    ind_beg(amc::_db_ns_curs, ns,amc::_db) if (ns.c_nscpp && ch_N(ns.ns)) {
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

// Check that each cascdel field names an unambiguous delete target:
// more than one instance access path to the field's target leaves
// unclear which instance a cascade delete should remove.
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
        // Ptrary and Llist Cascdel delete the last (resp. first) row repeatedly
        // until the index is empty, relying on each row's delete to unlink the
        // row from this index.  The row's Uninit emits that unlink only for a
        // field established as an xref; on a manually populated index nothing
        // shrinks and the loop re-deletes the same freed row forever.
        bool needunlink = field.reftype == dmmeta_Reftype_reftype_Ptrary
            || field.reftype == dmmeta_Reftype_reftype_Llist;
        if (needunlink && !field.c_xref) {
            prerr("amc.cascdel_xref"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype)
                  <<Keyval("comment","cascdel deletes rows until the index is empty; without dmmeta.xref the row's delete cannot unlink it"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_check_ssimsort() {
    ind_beg(_db_ssimsort_curs,ssimsort,_db) {
        amccheck(ssimsort.p_ssimfile->p_ctype  == ssimsort.p_sortfld->p_ctype, "amc.bad_ssimsort"
                 <<Keyval("ssimsort",ssimsort.ssimfile)
                 <<Keyval("sortfld",ssimsort.sortfld)
                 <<Keyval("comment","Sort field unrelated to the type"));
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_countxref() {
    ind_beg(amc::_db_xref_curs, xref,amc::_db) {
        xref.p_field->p_arg->n_xref++;
    }ind_end;
}

// ----------------------------------------------------------------------------

// Check the dependency order of gstatic tables: a table may refer only to
// tables loaded before it (earlier rowid), and never to a finput table --
// finput data is not loaded yet when the static initializers run.
void amc::gen_check_static() {
    ind_beg(amc::_db_xref_curs, xref, amc::_db) {
        amc::FField *parent = FirstInst(*xref.p_field->p_ctype);
        amc::FField *child = FirstInst(*xref.p_field->p_arg);
        if (xref.p_field->reftype == dmmeta_Reftype_reftype_Upptr) {
            algo::TSwap(child,parent);
        }
        if (child && parent && child->c_gstatic && parent->c_gstatic) {
            if (child->c_gstatic->rowid < parent->c_gstatic->rowid) {
                amccheck(0,"amc.gstatic_dep"
                         <<Keyval("ctype",child->arg)
                         <<Keyval("uses",parent->arg)
                         <<Keyval("via",xref.field)
                         <<Keyval("comment","An earlier static table is referring to a later table. To fix: `acr gstatic -e` and change rowids"));
            }
        }
        if (child && parent && child->c_gstatic && parent->c_finput) {
            amccheck(0,"amc.gstatic_uses_finput"
                     <<Keyval("ctype",child->arg)
                     <<Keyval("uses",parent->arg)
                     <<Keyval("via",xref.field)
                     <<Keyval("reftype",xref.p_field->reftype)
                     <<Keyval("comment","A statically loaded table cannot refer to a dynamically loaded one."));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// A field's name-prefix is checked against the fprefix table: a reftype with
// any require:Y pairing must be used under one of its registered prefixes.
// A reftype whose pairings are all require:N (Ptr, Delptr) also accepts
// arbitrary field names; its rows only document the allowed prefixes.
static bool MatchPrefix(strptr prefix, amc::FReftype &reftype) {
    bool constrained = false;
    bool matched = false;
    ind_beg(amc::reftype_zs_fprefix_curs,fprefix,reftype) {
        constrained |= fprefix.require;
        matched |= prefix_Get(fprefix)==prefix;
    }ind_end;
    return !constrained || matched;
}

// ----------------------------------------------------------------------------

void amc::gen_check_prefix() {
    // a prefix pairs with any number of reftypes, but acr_ed's reftype
    // inference needs one unambiguous answer per prefix: the dflt:Y row
    ind_beg(amc::_db_fprefix_curs, fprefix, amc::_db) if (fprefix.dflt) {
        ind_beg(amc::_db_fprefix_curs, other, amc::_db) {
            if (other.dflt && &other != &fprefix && prefix_Get(other)==prefix_Get(fprefix)) {
                prerr("amc.dup_fprefix_dflt"
                      <<Keyval("fprefix",fprefix.fprefix)
                      <<Keyval("other",other.fprefix)
                      <<Keyval("comment","At most one fprefix row per prefix can have dflt:Y"));
                algo_lib::_db.exit_code++;
            }
        }ind_end;
    }ind_end;
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        amc::FReftype &reftype = *field.p_reftype;
        if (!MatchPrefix(Pathcomp(name_Get(field),"_LL"),reftype)) {
            if (field.p_ctype->p_ns->c_nsjs) {
                // js can omit prefix
            } else {
                prerr("amc.bad_prefix"
                      <<Keyval("field",field.field)
                      <<Keyval("reftype",reftype.reftype)
                      <<Keyval("comment","See dmmeta.fprefix table for allowable combinations"));
                algo_lib::_db.exit_code++;
            }
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

// Validate each bitfield against its source field: the source must be a sized
// integer, the bitfield must fit within its bits, and bitfields sharing a
// source field must not overlap.
// The width is bounded above by 64 as well as below by 1, because 64 bits is
// all a mask over the field can select: the mask is built in a u64. A 65-bit
// field of a u128 source field has an offset and a width the source field can
// hold, so the two tests below pass it.
// The bitfield accessors refuse such a width too, but they refuse it later and
// one field at a time: the run ends at the first bad width with a message
// carrying neither the offset nor the width. Reporting it here carries both and
// counts it as a defect rather than ending the run, so a universe with several
// bad widths reports every one -- amc.FconstBitfldWidth holds a negative width,
// a zero one and a 65 and draws a line for each.
void amc::gen_check_bitfld() {
    ind_beg(amc::_db_field_curs, srcfield, amc::_db) {
        u128 value=0;
        ind_beg(amc::field_bh_bitfld_curs,bitfld,srcfield) {
            i64 srcbits = Ctype_Nbit(*srcfield.p_arg);
            if (bitfld.offset < 0 || bitfld.offset >= 128 || bitfld.width < 1 || bitfld.width > 64) {
                prerr("amc.bitfield_bounds"
                      <<Keyval("bitfld",bitfld.field)
                      <<Keyval("offset",bitfld.offset)
                      <<Keyval("width",bitfld.width)
                      <<Keyval("comment","bitfield offset must be 0..127 and width must be 1..64"));
                algo_lib::_db.exit_code++;
            } else if (srcbits > 0 && bitfld.width > srcbits - bitfld.offset) {
                // offset is bounded 0..127 above, so srcbits-offset cannot
                // overflow; comparing width this way avoids overflowing offset+width
                prerr("amc.bitfield_overflow"
                      <<Keyval("bitfld",bitfld.field)
                      <<Keyval("offset",bitfld.offset)
                      <<Keyval("width",bitfld.width)
                      <<Keyval("srcbits",srcbits)
                      <<Keyval("comment","bitfield extends past the storage of its source field"));
                algo_lib::_db.exit_code++;
            } else {
                u128 mask = bitfld.width >= 128 ? ~u128(0) : (((u128(1) << bitfld.width)-1) << bitfld.offset);
                if ((value & mask) != 0) {
                    prerr("amc.overlapbitfld"
                          <<Keyval("bitfld",bitfld.field)
                          <<Keyval("srcfield",srcfield.field)
                          <<Keyval("comment","Bit fields are not allowed to overlap"));
                    algo_lib::_db.exit_code++;
                }
                value |= mask;
            }
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

// Validate every ffunc by the code actually generated: each emission site calls
// FindFfunc with mark_used=true, so an ffunc left unused names a function no
// generator produced -- a typo, or a name not applicable to the field (e.g.
// FindRemove on a Val, OnXref on a field whose access path never inserts,
// InputMaybe on a non-finput field).  This is the sole ffunc-name check; the
// set of tfuncs that run for the field is the authority (replaced amcdb.tcb).
void amc::gen_check_ffunc() {
    ind_beg(amc::_db_ffunc_curs, ffunc,amc::_db) {
        if (!ffunc.used) {
            prerr("amc.bad_ffunc"
                  <<Keyval("ffunc",ffunc.ffunc)
                  <<Keyval("comment","ffunc names no function amc generates for this field (bad name, or not applicable to its reftype)"));
            algo_lib::_db.exit_code++;
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

// Check that each field's reftype is backed by its per-reftype record
// (dmmeta.tary, dmmeta.thash, ...); later gen phases dereference these
// records, so reftype errors end the run here.
void amc::gen_check_reftype() {
    int prev_err = algo_lib::_db.exit_code;
    ind_beg(amc::_db_field_curs, field,amc::_db) {
        tempstr err;
        if (ctype_zd_varlenfld_InLlistQ(field) && !(field.reftype == dmmeta_Reftype_reftype_Varlen)) {
            err = "Field reftype must be Varlen";
        }
        CheckReftype(field, dmmeta_Reftype_reftype_Hook, field.c_hook, dmmeta_Ssimfile_ssimfile_dmmeta_hook, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Tary, field.c_tary, dmmeta_Ssimfile_ssimfile_dmmeta_tary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Ptrary, field.c_ptrary, dmmeta_Ssimfile_ssimfile_dmmeta_ptrary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Inlary, field.c_inlary, dmmeta_Ssimfile_ssimfile_dmmeta_inlary, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Thash, field.c_thash, dmmeta_Ssimfile_ssimfile_dmmeta_thash, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Blkhash, field.c_blkhash, dmmeta_Ssimfile_ssimfile_dmmeta_blkhash, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Smallstr, field.c_smallstr, dmmeta_Ssimfile_ssimfile_dmmeta_smallstr, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Llist, field.c_llist, dmmeta_Ssimfile_ssimfile_dmmeta_llist, err);
        CheckReftype(field, dmmeta_Reftype_reftype_Bitfld, field.c_bitfld, dmmeta_Ssimfile_ssimfile_dmmeta_bitfld, err);
        if (ch_N(err)) {
            prerr("amc.missing_record"
                  <<Keyval("field",field.field)
                  <<Keyval("comment",err));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    // Later gen phases dereference the per-reftype records this phase found
    // missing (c_llist, c_thash, ...), so a run with reftype errors cannot
    // continue past this point. The throw ends the run through the normal
    // error exit -- unlike _exit, it preserves atexit work such as the gcov
    // flush under -cfg:coverage. Errors accumulated by earlier phases (e.g.
    // a bad gconst) leave the model consistent and do not stop the run; they
    // suppress output and set the exit code at the end of Main.
    vrfy(algo_lib::_db.exit_code == prev_err, "reftype errors prevent code generation");
}

// -----------------------------------------------------------------------------

void amc::gen_detectinst() {
    // detect instance fields -- creation access paths
    // ignore instances from different namespaces
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

    // a x-referenced field target has to be a pool
    ind_beg(amc::_db_xref_curs, xref, amc::_db) {
        amccheck(HasPoolQ(*xref.p_field->p_arg)
                 ,"amc.nopool"
                 <<Keyval("field",xref.field)
                 <<Keyval("ctype",xref.p_field->arg));
    }ind_end;
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
    ret &= field.reftype != dmmeta_Reftype_reftype_Blkhash;
    ret &= field.reftype != dmmeta_Reftype_reftype_Ptrary;
    ret &= !ValQ(field);
    return ret;
}

// -----------------------------------------------------------------------------

void amc::gen_prep_field() {
    // other field stuff -- determine ctype, set zd_varlenfld, etc.
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
            amc::zd_varlenfld_Insert(ctype,field);
        }
        if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            amc::c_optfld_InsertMaybe(ctype,field);
        }
        if (field.c_substr) {
            field.c_substr->p_srcfield->has_substr=true;
        }

        field.need_firstchanged = NeedFirstchangedQ(field);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Check that each big-endian field can be stored byteswapped: the field
// is a Val of a builtin type flagged bigendok, and the type's width is one
// the byteswap primitives cover (be16toh/be32toh/be64toh). The width is
// verified independently of the bigendok claim: a bltin row could flag a
// width with no primitive (a 128-bit integer), and Val Get/Set would then
// emit a call to a nonexistent swap function -- uncompilable output with a
// zero exit. A width of zero means the width is not known in this universe
// (the type's csize row is absent), and the diagnostic names the missing
// row instead of prescribing a width change for a type whose width may
// already be right. Each rejection reports and continues, accumulating
// into the exit code.
void amc::gen_check_bigend() {
    ind_beg(amc::_db_fbigend_curs, fbigend, amc::_db) {
        amc::FField &field = *fbigend.p_field;
        i64 nbit = Ctype_Nbit(*field.p_arg);
        if (field.reftype != dmmeta_Reftype_reftype_Val) {
            prerr("amc.bad_bigend"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype)
                  <<Keyval("comment","big-endian storage requires reftype Val"));
            algo_lib::_db.exit_code++;
        } else if (!(field.p_arg->c_bltin && field.p_arg->c_bltin->bigendok)) {
            prerr("amc.bigend_deadend"
                  <<Keyval("field",field.field)
                  <<Keyval("type",field.arg)
                  <<Keyval("comment","big-endian storage not allowed for this type"));
            algo_lib::_db.exit_code++;
        } else if (nbit == 0) {
            prerr("amc.bigend_size"
                  <<Keyval("field",field.field)
                  <<Keyval("type",field.arg)
                  <<Keyval("comment","type size unknown; add a dmmeta.csize row for the type"));
            algo_lib::_db.exit_code++;
        } else if (nbit != 16 && nbit != 32 && nbit != 64) {
            prerr("amc.bigend_width"
                  <<Keyval("field",field.field)
                  <<Keyval("type",field.arg)
                  <<Keyval("nbit",nbit)
                  <<Keyval("comment","no byteswap primitive for this width; use a 16, 32, or 64-bit integer type"));
            algo_lib::_db.exit_code++;
        }
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
            algo_lib::_db.exit_code++;
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

// Query whether FIELD is stored as a plain member of its parent struct: the
// value sits at a fixed offset of fixed size, and the parent allocated nothing
// for it. An inline string, an inline array, a pointer, a bitfield and a
// by-value member are all of this kind.
static bool PlainFieldQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Smallstr
        || field.reftype == dmmeta_Reftype_reftype_Val
        || field.reftype == dmmeta_Reftype_reftype_Inlary
        || field.reftype == dmmeta_Reftype_reftype_Cppstack
        || field.reftype == dmmeta_Reftype_reftype_Ptr
        || field.reftype == dmmeta_Reftype_reftype_Base
        || field.reftype == dmmeta_Reftype_reftype_Bitfld;
}

// -----------------------------------------------------------------------------

// Query whether FIELD is stored in place but with an extent decided at runtime.
// The member trails its parent's fixed portion, so the parent allocated nothing
// for it and destroys nothing, but the parent has no fixed size either.
static bool VarsizeFieldQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Varlen
        || field.reftype == dmmeta_Reftype_reftype_Opt;
}

// -----------------------------------------------------------------------------

// Determine CTYPE's plaindata and has_dtor flags, recursing into the type of
// every member first, and store both on the ctype. See gen_plaindata for what
// the two flags mean and why one walk settles both.
// A ctype already on the visit list keeps the values it was given, so the walk
// costs one pass over the graph and a ctype containing itself terminates.
void amc::PlaindataVisit(amc::FCtype &ctype) {
    // todo: rename this to zs_ctype_visit
    if(!zs_sig_visit_InLlistQ(ctype)) {
        zs_sig_visit_Insert(ctype);
        // assume the best initially...
        bool plaindata= ctype.c_cextern ? ctype.c_cextern->plaindata
            : true;
        // an external type amc did not lay out is destroyed by whatever C++ code
        // declares it, and its cextern row says whether that amounts to anything
        bool has_dtor= ctype.c_cextern && !ctype.c_cextern->plaindata;
        ind_beg(ctype_c_field_curs,field,ctype) {
            // Classification only — mark_used is false (not an emission site).
            bool owned = amc::FindFfunc(field, amcdb_cbtype_Cleanup)
                // A cascdel field means the record owns dependents: deleting it
                // deletes them.
                || field.c_cascdel != NULL;
            if (owned || !(PlainFieldQ(field) || VarsizeFieldQ(field))) {
                plaindata=false;
                has_dtor=true;
            } else if (VarsizeFieldQ(field)) {
                plaindata=false;
            } else if (amc::FindFfunc(field, amcdb_cbtype_Userinit)) {
                // a field amc did not initialize is not a value amc may fabricate
                // by copying bytes, but nothing was allocated for it either
                plaindata=false;
            } else {
                // recursive step
                PlaindataVisit(*field.p_arg);
                plaindata = plaindata && field.p_arg->plaindata;
                has_dtor = has_dtor || field.p_arg->has_dtor;
            }
        }ind_end;
        // An xref links the record into an index, and the record unlinks itself
        // from every one of them when destroyed -- so its value is not merely its
        // bytes, and forgetting it is not enough.
        //
        // Whether a field is an xref is a fact about the field, recorded in
        // dmmeta.xref, and no reftype answers it: a Ptr, a Ptrary and an Upptr
        // are each an xref only sometimes, and between them they carry 922 of the
        // tree's 2135 xrefs. lib_x2.FStartup is held by exactly one, the Ptr xref
        // c_startup, so classifying by reftype reported it as owning nothing --
        // which dropped the destructor call from its pool delete and left the
        // freed row in the index.
        //
        // n_xref counts every xref whose arg is this ctype, and gen_countxref
        // computes it far earlier in the pass order than the zs_xref lists Uninit
        // itself walks. The indexes Uninit removes the record from are the
        // non-Upptr subset of that same set -- an Upptr points up and the parent
        // does not hold the child -- so n_xref is a superset and cannot miss one.
        if (ctype.n_xref > 0) {
            plaindata=false;
            has_dtor=true;
        }
        // save the computed values
        ctype.plaindata=plaindata;
        ctype.has_dtor=has_dtor;
    }
}

// Determine, for each ctype, how its value behaves: whether it can be copied
// with memcpy (plaindata) and whether destroying it does anything (has_dtor).
//
// The two are separate facts, and one field reftype tells them apart. A Varlen
// or an Opt member is addressed in place inside its parent, so the parent
// allocates nothing for it and needs no destructor -- but the member's extent is
// decided at runtime, so the parent has no fixed size and cannot be memcopied.
// Conflating the two would report a destructor for every nested variable-length
// message, and amc refuses to nest one whose element type has a destructor.
//
// Both walk the same ctype graph, so one pass computes both.
void amc::gen_plaindata() {
    zs_sig_visit_RemoveAll();
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        PlaindataVisit(ctype);
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
    } else if (field.p_arg->cheap_copy) { // cheap copy -> copy cpptype
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
        ns.select = ns.c_nscpp || ns.c_nsjs;
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
        if (ch_N(gconst.idfld) && (!gconstfld || gconstfld->p_ctype != namefld.p_ctype)) {
            // Each fconst's value is read from the same input row as its name,
            // so idfld must name a column of the namefld table. A nonexistent
            // idfld would silently fall back to the name column, and a field of
            // any other ctype is absent from the rows and would leave every
            // fconst value empty. The known-bad row is skipped, so one run
            // still reports every offending gconst.
            prerr("amc.gconst_idfld"
                  <<Keyval("gconst",gconst.field)
                  <<Keyval("idfld",gconst.idfld)
                  <<Keyval("namefld",gconst.namefld)
                  <<Keyval("comment","idfld must name a field of the namefld ctype"));
            algo_lib::_db.exit_code++;
        } else {
            amc::FField &idfld = ch_N(gconst.idfld) ? *gconstfld : namefld;
            tempstr      fname = SsimFilename(DataRoot(),*namefld.p_ctype);
            bool is_string = field.p_arg->c_cstr != NULL && field.arg != "char";
            bool is_char = field.arg == "char";
            algo_lib::MmapFile file;
            // The value table's rows are the enum's constants, so a table that
            // is not there would compile an empty enum and exit 0, silently
            // dropping every constant. The two ways it can be absent are
            // reported apart, because the reader can say nothing about the
            // first: a namefld whose ctype reaches no ssimfile names no file at
            // all, while a named file that cannot be read carries an errno. Both
            // skip this gconst and let the run continue, so one run still
            // reaches every other one.
            if (!ch_N(fname)) {
                prerr("amc.gconst_nossimfile"
                      <<Keyval("gconst",gconst.field)
                      <<Keyval("namefld",gconst.namefld)
                      <<Keyval("comment","the namefld's ctype reaches no ssimfile, so there is no value table"));
                algo_lib::_db.exit_code++;
            } else if (!SideloadFile(file,fname)) {
                algo::PrerrFileFail("amc.load", fname, "gconst value table could not be read");
                algo_lib::_db.exit_code++;
            } else {
                int idx = 0;
                int nrec = 0;
                int nline = 0;
                ind_beg(Line_curs,line,file.text) {
                    Tuple tuple;
                    nline++;
                    bool readq = Tuple_ReadStrptrMaybe(tuple, line);
                    tempstr name(amc::EvalAttr(tuple, namefld));
                    tempstr value(amc::EvalAttr(tuple, idfld));
                    char ch = 0;
                    bool bad_char = readq && attrs_N(tuple) > 0 && is_char && !char_ReadStrptrMaybe(ch, value);
                    // A line that does not parse as a tuple is an input error the
                    // reader itself cannot describe: an unterminated quoted value
                    // keeps whatever was read before the quote ran out and jumps
                    // the iterator to end of line, so the truncated value reads
                    // back as a name the value table never wrote and every
                    // attribute past the bad quote, the comment among them,
                    // disappears. The tuple reader leaves no error text of its
                    // own, and algo_lib's shared error buffer holds whatever an
                    // earlier pass put there, so the diagnostic states its own
                    // subject: this gconst, this file, this line, this text. The
                    // line is skipped and the scan continues, so every bad line
                    // in the table is reported once and the auto-numbering below
                    // never consumes an index for a line that produced nothing.
                    if (!readq) {
                        prerr("amc.bad_gconst"
                              <<Keyval("gconst",gconst.field)
                              <<Keyval("file",fname)
                              <<Keyval("line",nline)
                              <<Keyval("text",line)
                              <<Keyval("comment","gconst value table line is not a tuple"));
                        algo_lib::_db.exit_code++;
                    } else if (attrs_N(tuple) == 0 && ch_N(tuple.head.value)) {
                        // A line with a head but no attribute can produce no
                        // constant, yet the auto-numbering below would still
                        // consume an index for it, silently renumbering every
                        // constant that follows on a clean exit; such a line is
                        // malformed input
                        prerr("amc.gconst_headonly"
                              <<Keyval("gconst",gconst.field)
                              <<Keyval("head",tuple.head.value)
                              <<Keyval("comment","head-only line in gconst value table"));
                        algo_lib::_db.exit_code++;
                    } else if (bad_char) {
                        // A char constant needs exactly one character; the row is
                        // reported and skipped so the scan still surfaces every
                        // other offending row in the same run
                        algo_lib::ResetErrtext();
                        prerr("amc.gconst_char"
                              <<Keyval("gconst",gconst.field)
                              <<Keyval("name",name)
                              <<Keyval("value",value)
                              <<Keyval("comment","value must parse as a single character"));
                        algo_lib::_db.exit_code++;
                    } else if (attrs_N(tuple) > 0) {
                        dmmeta::Fconst fconst;
                        // support c++ char
                        if (is_char) {
                            ch_RemoveAll(value);
                            char_PrintCppSingleQuote(ch, value);
                        }
                        // c++ integer
                        if (!ch_N(gconst.idfld) && !is_string && !is_char) {
                            ch_RemoveAll(value);
                            value << idx++;
                        }
                        // name stays as-is, subsequent fconst generator
                        // will sanitize the name when creating a C++ identifier
                        fconst.fconst = tempstr() << gconst.field << "/" << name;
                        fconst.value = algo::CppExpr(value);
                        fconst.comment.value = attr_GetString(tuple, "comment");// import comment, if any
                        // A name that repeats in the value table maps two rows to one
                        // enum constant: the second row would silently vanish from the
                        // enum while still consuming a numbering index, so an edit of
                        // the value table could renumber neighboring constants with a
                        // clean exit
                        if (amc::fconst_InsertMaybe(fconst)) {
                            nrec++;
                        } else {
                            prerr("amc.gconst_dup"
                                  <<Keyval("fconst",fconst.fconst)
                                  <<Keyval("comment","duplicate name in gconst value table"));
                            algo_lib::_db.exit_code++;
                        }
                    }
                }ind_end;
                verblog("amc.load_gconst"
                        <<Keyval("fname",fname)
                        <<Keyval("field",field.field)
                        <<Keyval("nrec", nrec));
            }
        }
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
            // fconst.value = algo::CppExpr(tempstr() << "("<< srcfield.p_arg->ctype << "(1)<<" << bitfld.offset << ")");
            // AP: needs to be compatible with js, do not use native c types
            cstring value;
            algo::u64_PrintHex(u64(1)<<bitfld.offset,value,0);
            fconst.value = algo::CppExpr(value);
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
            errmsg << "malformed constant value '" << fconst.value << "'"; // ignore:hand_quote
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

// Side-load the ssimfile rows behind each gstatic field into the
// static_tuple table; they become the compiled-in initializers of the
// gstatic table.
void amc::gen_load_gstatic() {
    ind_beg(amc::_db_gstatic_curs, gstatic,amc::_db) {
        amc::FField& field = *gstatic.p_field;
        amc::FCtype *ctype = GetBaseType(*field.p_arg,field.p_arg);
        tempstr      fname  = ctype ? amc::SsimFilename(DataRoot(), *ctype) : tempstr();
        algo_lib::MmapFile in;
        // The rows of this table become the compiled-in registry, so a table
        // that is not there is an input error either way: loading zero rows
        // would compile an empty registry and exit 0, dropping every entry. The
        // two ways it can be absent are reported apart, because the reader can
        // say nothing about the first: a gstatic field whose element type
        // reaches no ssimfile names no file at all, while a named file that
        // cannot be read carries an errno. Both skip the field and let the run
        // continue, so one run reports every such field.
        if (!ch_N(fname)) {
            prerr("amc.gstatic_nossimfile"
                  <<Keyval("field",field.field)
                  <<Keyval("basetype",(ctype ? algo::strptr(ctype->ctype) : algo::strptr()))
                  <<Keyval("comment","the element type reaches no ssimfile, so there is no table to load"));
            algo_lib::_db.exit_code++;
        } else if (!SideloadFile(in, fname)) {
            algo::PrerrFileFail("amc.load", fname, "gstatic input file could not be read");
            algo_lib::_db.exit_code++;
        } else {
            Tuple tuple;
            int nrec=0;
            int nline=0;
            ind_beg(Line_curs,line,in.text) {
                nline++;
                bool readq = Tuple_ReadStrptrMaybe(tuple,line);
                // a line that does not parse as a tuple is an input error the
                // reader itself cannot describe: the tuple reader writes no error
                // text of its own, and algo_lib's shared error buffer holds
                // whatever an earlier pass left in it -- bad-number tags naming
                // valid fconst values, for one -- so a message taken from that
                // buffer names the wrong file. The diagnostic states its own
                // subject instead: this gstatic field, this file, this line,
                // this text. The field is part of the subject because two
                // gstatic fields whose element types are based on the same
                // ctype read the same file, and without it their complaints
                // about a line are the same text twice. The run continues, so
                // every bad line in the table is reported once.
                if (!readq) {
                    prerr("amc.bad_gstatic"
                          <<Keyval("field",field.field)
                          <<Keyval("file",fname)
                          <<Keyval("line",nline)
                          <<Keyval("text",line)
                          <<Keyval("comment","gstatic input line is not a tuple"));
                    algo_lib::_db.exit_code++;
                } else if (attrs_N(tuple) > 0 && ch_N(tuple.head.value)) {
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
        }
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
                amc::CloneFields(from,to,ChildRowid(basefield.rowid));
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

// Check that every Ptrary field has its dmmeta.ptrary record, and extend unique ptrarys
// with a membership flag on the target ctype.
void amc::gen_newfield_ptrary() {
    // A field of reftype Ptrary requires an explicit dmmeta.ptrary record
    // (ssimreq dmmeta.ptrary; acr_ed creates the record together with the field).
    // When the record is missing, report an error, then provision a default
    // stand-in record so gen_check_reftype does not report the same missing
    // record a second time and the rest of the run operates on a consistent
    // model; the nonzero exit code suppresses all output at the end of Main.
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.reftype==dmmeta_Reftype_reftype_Ptrary && !field.c_ptrary) {
        amccheck(0,"amc.missing_ptrary"
                 <<Keyval("field",field.field)
                 <<Keyval("comment","Missing dmmeta.ptrary record for this field"));
        dmmeta::Ptrary ptrary;
        ptrary.field = field.field;
        ptrary.unique = false;
        amc::ptrary_InsertMaybe(ptrary);
    }ind_end;

    // add membership flag to unique ptrarys
    // check that ptrary target is in the same namespace --
    // we will be extending it with a membership flag.
    ind_beg(amc::_db_ptrary_curs, ptrary, amc::_db) {
        if (ptrary.unique) {
            vrfy(ptrary.p_field->p_ctype->p_ns == ptrary.p_field->p_arg->p_ns
                 ,tempstr()<<"amc.foreign_ptrary"
                 <<Keyval("field",ptrary.field)
                 <<Keyval("arg",ptrary.p_field->arg)
                 );
            dmmeta::Field newfield;
            tempstr xf;
            if (!GlobalQ(*ptrary.p_field->p_ctype)) {
                xf << amc::Refname(*ptrary.p_field->p_ctype) << "_";
            }
            if (ptrary.heaplike) {
                newfield.field         = tempstr()<<ptrary.p_field->arg<<"."<<xf<<name_Get(*ptrary.p_field)<<"_idx";
                newfield.arg           = "i32";
                newfield.dflt.value    = "-1";
                newfield.comment.value = "Array index (-1 = not in array)";
            } else {
                newfield.field         = tempstr()<<ptrary.p_field->arg<<"."<<xf<<name_Get(*ptrary.p_field)<<"_in_ary";
                newfield.arg           = "bool";
                newfield.dflt.value    = "false";
                newfield.comment.value = "membership flag";
            }
            newfield.reftype       = dmmeta_Reftype_reftype_Val;
            InsField(newfield);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_dispatch() {
    amc::Disp_NewField();
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_cfmt() {
    // Command-line (strfmt:Argv) ctypes are parsed by the field-aware
    // <Name>_ReadArgv; they no longer need an auto-synthesized Tuple read
    // cfmt (which only produced the now-unused <Name>_ReadTupleMaybe).
}

// -----------------------------------------------------------------------------

static bool UnpackedQ(amc::FField &field) {
    return (field.p_reftype->isval || field.reftype ==dmmeta_Reftype_reftype_Base)
        && !field.p_arg->c_pack
        && !field.p_arg->c_cextern
        && !field.c_substr;
}

// Check pack consistency within a namespace: every ctype in a packed namespace must be packed, and every field of a packed ctype must be packed.
void amc::gen_ns_check_pack() {
    amc::FNs &ns =*amc::_db.c_ns;
    bool nspack=ns.c_nsx && ns.c_nsx->pack;
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        bool ctpack=ctype.c_pack;
        if (nspack && !ctpack) {
            amccheck(0,"amc.what_the_pack"
                     <<Keyval("ctype",ctype.ctype)
                     <<Keyval("ns",ns_Get(ctype))
                     <<Keyval("comment","Ctype in packed namespace must also be packed. Insert the following line to fix error (acr -insert -write)"));
            prerr(dmmeta::Pack(ctype.ctype,algo::Comment()));
        }
        if (ctpack) {
            ind_beg(amc::ctype_c_field_curs,field,ctype) if (UnpackedQ(field)) {
                amccheck(0,"amc.back_pack"
                         <<Keyval("ctype",ctype.ctype)
                         <<Keyval("field",field.field)
                         <<Keyval("child_ctype",field.arg)
                         <<Keyval("comment","Field is unpacked but parent ctype is packed. Insert the following line to fix error (acr -insert -write)"));
                prerr(dmmeta::Pack(field.arg,algo::Comment()));
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// A singly linked list requires scanning to locate the element to delete.
// It is considered an access path with quadratic deletion cost.
// So is a Ptrary.
bool amc::SlowDelQ(amc::FField &field) {
    return field.c_xref
        && ((field.c_ptrary && !field.c_ptrary->heaplike)
            ||(field.c_llist && !field.c_llist->p_listtype->haveprev));
}

void amc::gen_ns_check_path() {
    amc::FNs &ns =*amc::_db.c_ns;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        int nslow=0;
        int ndel=0;
        ind_beg(amc::ctype_zd_access_curs,access,ctype) {
            ndel += access.p_reftype->del;
            nslow += SlowDelQ(access);
        }ind_end;
        bool good=ndel==0 || nslow<2;
        amccheck(good
                 , "amc.many_slow_paths"
                 <<Keyval("ctype",ctype.ctype)
                 <<Keyval("ndel",ndel)
                 <<Keyval("nslow",nslow)
                 <<Keyval("comment","Access path with quadratic deletion cost found"));
        if (!good) {
            ind_beg(amc::ctype_zd_access_curs,access,ctype) {
                prerr("amc.access_path"
                      <<Keyval("field",access.field)
                      <<Keyval("reftype",access.reftype)
                      <<Keyval("del",access.p_reftype->del)
                      <<Keyval("slowdel",SlowDelQ(access)));
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_pkeytypedef() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nscpp) {
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
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nscpp) {
        *ns.hdr<<"// gen:ns_enums" << eol;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            amc::Main_GenEnum(ns, ctype); // experimental
        }ind_end;
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
    if (ns.c_nscpp) {
        algo_lib::Replscope R;
        Set(R, "$ns", ns.ns);
        amc::BeginNsBlock(*ns.cpp, ns, "");
        ind_beg(amc::ns_c_func_curs, func,ns) {
            bool print = func.priv && !func.ismacro && !func.globns && !func.disable && !func.member;
            if (print) {
                tempstr proto;
                PrintFuncProto(func, NULL, proto, false);
                algo::InsertIndent(*ns.cpp, proto, 1);
            }
        }ind_end;
        amc::EndNsBlock(*ns.cpp, ns, "");
        amc::_db.lim_ind_func=amc::ind_func_N();
    }
}

void amc::gen_ns_print_struct() {
    amc::FNs &ns =*amc::_db.c_ns;
    if (ns.c_nscpp) {
        amc::BeginNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            if (!ctype.c_cextern) {
                amc::GenStruct(ns, ctype);
            }
            ind_beg(amc::ctype_c_field_curs, field,ctype) {
                int n_hdr = ch_N(*ns.hdr);
                ind_beg(amc::field_c_ffunc_curs, ffunc,field) {
                    if (!ffunc.printed && !ffunc.ismacro && !ffunc.globns && !ffunc.disable) {
                        if (!ffunc.priv && !ffunc.member) {
                            tempstr proto;
                            PrintFuncProto(ffunc,NULL,proto, false);// goes to header
                            algo::InsertIndent(*ns.hdr, proto, 0);
                        }
                        PrintFuncBody(ns,ffunc);// goes to source
                        ffunc.printed=true;
                    }
                }ind_end;
                if (ch_N(*ns.hdr) > n_hdr) {
                    *ns.hdr << eol;
                }
            }ind_end;
            ind_beg(amc::ctype_c_ffunc_curs, ffunc, ctype) {
                if (!ffunc.printed && !ffunc.ismacro && !ffunc.globns && !ffunc.disable) {
                    if (!ffunc.priv && !ffunc.member) {
                        tempstr proto;
                        PrintFuncProto(ffunc,NULL,proto,false);// goes to header
                        algo::InsertIndent(*ns.hdr, proto, 0);
                    }
                    if (!ffunc.extrn && !ffunc.deleted) {
                        PrintFuncBody(ns,ffunc);// goes to source
                    }
                    ffunc.printed=true;
                }
            }ind_end;
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
    }
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
    if (ns.c_nscpp) {
        algo_lib::Replscope R;
        Set(R, "$ns", ns.ns);
        amc::BeginNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_func_curs, func,ns) {
            if (!func.printed && !func.ismacro && !func.disable && !func.member && !func.globns) {
                if (!func.priv) {
                    tempstr proto;
                    PrintFuncProto(func, NULL, proto, false);
                    algo::InsertIndent(*ns.hdr, proto, 0);
                }
                if (!func.extrn && !func.deleted) {
                    PrintFuncBody(ns, func);
                }
                func.printed = true;
            }
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
        ind_beg(amc::ns_c_func_curs, func,ns) {
            if (!func.printed && !func.ismacro && !func.disable && !func.member && func.globns) {
                if (!func.priv) {
                    tempstr proto;
                    PrintFuncProto(func, NULL, proto, false);
                    algo::InsertIndent(*ns.hdr, proto, 0);
                }
                if (!func.extrn && !func.deleted) {
                    PrintFuncBody(ns, func);
                }
                func.printed = true;
            }
        }ind_end;
    }
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
    if (ns.c_nscpp) {
        algo_lib::Replscope R;
        // generate "operator <<" for every print function...
        *ns.hdr<<"// gen:ns_operators" << eol;
        *ns.hdr << "namespace algo {" << eol;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            GenOperators(R,ns,ctype);
        }ind_end;
        *ns.hdr << "}" << eol;
    }
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
                algo_lib::_db.exit_code++;
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
    // create a command::xyz_proc for every command ctype that parses argv
    // (any read:Y cfmt under command.*)
    ind_beg(_db_cfmt_curs,cfmt,_db) if (ns_Get(*cfmt.p_ctype)=="command"
                                        && cfmt.read
                                        ) {
        amc::FCtype &cmdctype = *cfmt.p_ctype;

        dmmeta::Ctype ctype;
        ctype.ctype = tempstr() << cmdctype.ctype << "_proc";
        ctype.comment.value = tempstr() << "Subprocess: " << cmdctype.comment;
        amc::ctype_InsertMaybe(ctype);

        dmmeta::Field field;
        field.field = tempstr() << ctype.ctype << "." << name_Get(cmdctype);
        field.arg = cmdctype.ctype;
        field.reftype = dmmeta_Reftype_reftype_Exec;
        amc::field_InsertMaybe(field);

        dmmeta::Anonfld anonfld;
        anonfld.field = field.field;
        amc::anonfld_InsertMaybe(anonfld);
    }ind_end;
}

// Check that each fcurs row names a cursor supported by its field's
// reftype (the corresponding amcdb.tcurs row is in the input set).
void amc::gen_check_fcurs() {
    ind_beg(_db_fcurs_curs,fcurs,_db) {
        // A field can correspond to several template classes
        // but that relationship is elucidated in the function GenTclass_Field
        // and not known anywhere else; For existing cursors, the reftype alone is sufficient,
        // plus Bitset tclass.
        tempstr tclass(fcurs.p_field->reftype);
        if (fcurs.p_field->c_fbitset) {
            tclass=amc_tclass_Bitset.tclass;
        }
        tempstr key = amcdb::Tfunc_Concat_tclass_name(tclass, curstype_Get(fcurs));
        amc::FTfunc *tfunc=ind_tfunc_Find(key);
        if (!tfunc || !tfunc->c_tcurs) {
            // two distinct failures: the reftype has no cursor by this name
            // (no tfunc), or the cursor exists but the input set omits its
            // amcdb.tcurs row
            strptr comment = !tfunc
                ? strptr("Reftype doesn't support specified cursor")
                : strptr("Cursor exists, but its amcdb.tcurs row is absent from the input set; add the row");
            prerr("amc.bad_fcurs"
                  <<Keyval("fcurs",fcurs.fcurs)
                  <<Keyval("reftype",fcurs.p_field->reftype)
                  <<Keyval("comment",comment));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// Check that the element type of a Varlen or Opt field has no destructor.
// Both reftypes address their element in place inside the enclosing message --
// there is no separate object whose lifetime amc could end -- so an element
// type that needs a destructor would never have one called.
// Check also that no ctype's fields claim the end of the fixed portion twice:
// varlen data and an optional trailing element both begin there, and only
// varlen fields carry the end offset that lets one follow another.
void amc::gen_check_varlen() {
    ind_beg(amc::_db_field_curs,field,amc::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_Varlen
            || field.reftype == dmmeta_Reftype_reftype_Opt) {
            amccheck(!HasDtorQ(*field.p_arg)
                     , "amc.opt_dtor"
                     <<Keyval("field",field.field)
                     <<Keyval("ctype",field.arg)
                     <<Keyval("reftype",field.reftype)
                     <<Keyval("comment","element type of a Varlen/Opt field must have no destructor"));
        }
    }ind_end;
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        // A Varlen field's data and an Opt field's element both begin at the
        // end of the fixed portion. Varlen fields chain -- each one starts
        // where the previous varlen field ends, and carries the end offset
        // that says where that is -- so only the first varlen field of a
        // ctype begins at the end of the fixed portion, and the rest may
        // follow it. An Opt carries no such offset: its accessors compute
        // the element address from the fixed size alone, so every Opt begins
        // there too, and a second field beginning at that address would
        // alias the first, a write through either corrupting the other.
        // Every claim on that address past the first is reported against the
        // field that made the first.
        amc::FField *prev = NULL;
        amc::FField *varlen = NULL;
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            bool varlenQ = field.reftype == dmmeta_Reftype_reftype_Varlen;
            bool claimQ = field.reftype == dmmeta_Reftype_reftype_Opt || (varlenQ && !varlen);
            if (claimQ && prev) {
                prerr("amc.trail_overlay"
                      <<Keyval("ctype",ctype.ctype)
                      <<Keyval("field",field.field)
                      <<Keyval("prev",prev->field)
                      <<Keyval("comment","field begins at the end of the fixed portion, where an earlier field already begins"));
                algo_lib::_db.exit_code++;
            } else if (claimQ) {
                prev = &field;
            }
            if (varlenQ && !varlen) {
                varlen = &field;
            }
        }ind_end;
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

void amc::gen_create_userfunc() {
    ind_beg(amc::_db_func_curs, func, _db) {
        if (func.extrn) {
            amc::FUserfunc &userfunc = userfunc_Alloc();
            userfunc.userfunc = func.func;
            userfunc.acrkey   = func.acrkey;
            userfunc.cppname  = tempstr()<<ns_Get(func)<<"."<<Pathcomp(func.proto,"(LL");
        }
    }ind_end;
}

// Write the derived tables (ctypelen, dispsig, tracefld, tracerec,
// userfunc) back to the output dataset through an acr subprocess, so
// they match the code generated by this run.
void amc::gen_table_write() {
    cstring str;
    ind_beg(_db_ctypelen_curs,ctypelen,_db) {
        dmmeta::Ctypelen out;
        ctypelen_CopyOut(ctypelen,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_dispsig_curs,dispsig,_db) {
        dmmeta::Dispsig out;
        dispsig_CopyOut(dispsig,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_payloadhdr_curs,payloadhdr,_db) {
        dmmeta::Payloadhdr out;
        payloadhdr_CopyOut(payloadhdr,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_msg_curs,msg,_db) {
        dmmeta::Msg out;
        msg_CopyOut(msg,out);
        str << out << eol;
    }ind_end;
    ind_beg(_db_msgfield_curs,msgfield,_db) {
        dmmeta::Msgfield out;
        msgfield_CopyOut(msgfield,out);
        str << out << eol;
    }ind_end;
    int rowid = 0;
    ind_beg(_db_tracefld_curs,tracefld,_db) {
        dmmeta::Tracefld out;
        tracefld_CopyOut(tracefld,out);
        str << out << "  acr.rowid:" << (++rowid) << eol;
    }ind_end;
    ind_beg(_db_tracerec_curs,tracerec,_db) {
        dmmeta::Tracerec out;
        tracerec_CopyOut(tracerec,out);
        str << out << eol;
    }ind_end;
    ind_beg(amc::_db_userfunc_curs,userfunc,_db) {
        dmmeta::Userfunc out;
        userfunc_CopyOut(userfunc,out);
        str << out << eol;
    }ind_end;

    // an empty -out_dir means "write nothing": without the ch_N gate,
    // DirFileJoin("","data") resolves to the CWD repo data/ and the table
    // update would escape into the live tree
    if (algo_lib::_db.exit_code==0 && ch_N(_db.cmdline.out_dir) && !amc::QueryModeQ()) {
        algo_lib::FTempfile temp;
        TempfileInitX(temp,"amc");
        // the tempfile is acr's entire input: a failed write leaves it
        // empty, acr then updates nothing and exits 0, and the run would
        // claim success with the derived tables stale; fail the run
        // before spawning acr
        if (!algo::SaveFile(str,temp.filename,"amc.tempfile_write","table-write input could not be written; the derived tables were not updated")) {
            algo_lib::_db.exit_code++;
        } else {
            command::acr_proc acr;
            acr.cmd.trunc=true;
            acr.cmd.replace=true;
            acr.cmd.in=DirFileJoin(_db.cmdline.out_dir,"data");
            acr.cmd.print=false;
            acr.cmd.write=true;
            acr.cmd.report=true;
            acr.fstdin<<"<"<<temp.filename;
            acr.fstdout = "|";
            acr_Start(acr);
            ind_beg(algo::FileLine_curs,line,acr.from_stdout) {
                report::acr report;
                if (report::acr_ReadStrptrMaybe(report,line)) {
                    _db.report.n_filemod += report.n_file_mod;
                }
            }ind_end;
            // The acr subprocess rewrites the derived tables (ctypelen, dispsig,
            // tracefld, tracerec, userfunc) to match the code generated by this
            // run. A failure would leave them stale (a stale dispsig produces
            // cross-process signature mismatches), so a nonzero status fails
            // the run.
            command::acr_Wait(acr);
            if (acr.status != 0) {
                prerr("amc.table_write"
                      <<Keyval("status",algo::DescribeWaitStatus(acr.status))
                      <<Keyval("comment","acr failed; the ctypelen/dispsig/tracefld/tracerec/userfunc tables were not updated"));
                algo_lib::_db.exit_code++;
            }
        }
    }
}

// -----------------------------------------------------------------------------

void amc::gen_ssimdb() {
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        if (ctype.p_ns->nstype == dmmeta_Nstype_nstype_ssimdb && ctype.original) {
            if (!ctype.c_ssimfile && !ctype.c_nossimfile) {
                prerr("amc.need_ssimfile"
                      <<Keyval("ctype",ctype.ctype)
                      <<Keyval("comment","ctype belonging to a 'nsdb' namespace must have a corresponding ssimfile"));
                algo_lib::_db.exit_code++;
            }
        }
    }ind_end;
}
