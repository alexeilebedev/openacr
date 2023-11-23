// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr_ed/field.cpp -- Create, delete, rename field
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

// Guess ctype which a field is going to refer to, from field name
// alone
// E.g. amc.FCtype.p_ns -> guess 'amc.FNs'
// because amc.FDb.ns is a pool
static acr_ed::FCtype *GuessArg(strptr field) {
    acr_ed::FCtype *ret = NULL;
    tempstr name(dmmeta::Field_name_Get(field));
    tempstr ns(dmmeta::Field_ns_Get(field));
    prlog(ns << ".FDb." << name);
    prlog(ns << ".FDb." << Pathcomp(name,"_LR"));
    if (acr_ed::FField *f1 = acr_ed::ind_field_Find(tempstr() << ns << ".FDb." << name)) {
        ret = f1->p_arg;
    } else if (acr_ed::FField *f2 = acr_ed::ind_field_Find(tempstr() << ns << ".FDb." << Pathcomp(name,"_LR"))) {
        ret = f2->p_arg;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static bool IsPkey(acr_ed::FCtype &ctype, algo::strptr fieldname) {
    acr_ed::FCtype *ctypebase = acr_ed::Basetype(ctype);
    return ctypebase && c_field_N(*ctypebase)>0 && c_field_Find(*ctypebase,0)->field==fieldname;
}

// -----------------------------------------------------------------------------

// Try to find a field in CHILD which could be used to locate a record of PARENT
static tempstr GuessXreffld(dmmeta::Field &field, acr_ed::FCtype &parent, acr_ed::FCtype &child) {
    acr_ed::FCtype *childbase = acr_ed::Basetype(child);
    acr_ed::FCtype *parentbase = acr_ed::Basetype(parent);
    bool error = false;
    int nmatch = 0;
    acr_ed::FField *match1=NULL;
    acr_ed::FField *match2=NULL;
    tempstr ret;
    ind_beg(acr_ed::ctype_c_field_curs, child_field, *childbase) {
        bool is_valid_ref = child_field.reftype == dmmeta_Reftype_reftype_Pkey
            && (child_field.arg == parent.ctype || child_field.arg == parentbase->ctype);
        is_valid_ref |= child_field.reftype == dmmeta_Reftype_reftype_Upptr
            && child_field.arg == parent.ctype;
        if (is_valid_ref) {
            prlog("acr_ed.guess_xref  "
                  <<Keyval("field",child_field.field)
                  <<Keyval("parentbase",parentbase->ctype));
            if (!match1) {
                match1 = &child_field;
            } else if (!match2) {
                match2 = &child_field;
            }
            ret = child_field.field;
            nmatch++;
        }
    }ind_end;
    if (nmatch == 0) {
        prerr("acr_ed.missing_via  "
              <<Keyval("field",field.field)
              <<Keyval("arg",child.ctype)
              <<Keyval("comment","Please provide -via"));
        error = true;
    }
    if (nmatch > 1) {
        prerr("acr_ed.ambiguous_via  "
              <<Keyval("field",field.field)
              <<Keyval("comment","Please provide -via'"));
        error = true;
    }
    if (match1) {
        prerr("acr_ed.via_match1"
              <<Keyval("child",match1->field)
              <<Keyval("comment","This child field is a possible via key candidate"));
    }
    if (match2) {
        prerr("acr_ed.via_match2"
              <<Keyval("child",match2->field)
              <<Keyval("comment","This child field is a possible via key candidate"));
    }
    if (error) {
        prerr("acr_ed.xrefdetail"
              <<Keyval("field",field.field)
              <<Keyval("parentbase",(parentbase ? strptr(parentbase->ctype) : strptr("''")))
              <<Keyval("childbase",(childbase ? strptr(childbase->ctype) : strptr("''"))));
    }
    vrfy(!error,"error");
    return ret;
}

// -----------------------------------------------------------------------------

// Guess reftype based on field prefix (see fprefix table)
// - There is one little difficulty... Both Ptr and Ptrary use c_ prefix,
// so we must guess which one to use:
// If the cardinality of field's arg is the same or smaller as the parent type, pick Ptr.
// this becomes known later so we just note that the field could be a pointer here.
// - If there is no special prefix for the field name, pick either Val or Pkey as the reftype
// depending on whether both field and arg belong to a relational table
static void GuessReftype(dmmeta::Field &field) {
    dmmeta::FprefixPkey key(Pathcomp(name_Get(field),"_LL"));
    acr_ed::FFprefix *fprefix=acr_ed::ind_fprefix_Find(key);
    acr_ed::FCtype &arg=acr_ed::ind_ctype_FindX(field.arg);
    acr_ed::FCtype &ctype=acr_ed::ind_ctype_FindX(ctype_Get(field));
    if (fprefix) {
        acr_ed::_db.cmdline.reftype = fprefix->reftype;
    }
    if (!ch_N(acr_ed::_db.cmdline.reftype)) {
        acr_ed::_db.cmdline.reftype = (ctype.c_ssimfile && arg.c_ssimfile)
            ? dmmeta_Reftype_reftype_Pkey : dmmeta_Reftype_reftype_Val;
    }
    acr_ed::_db.could_be_ptr = acr_ed::_db.cmdline.reftype == dmmeta_Reftype_reftype_Ptrary;
}

// -----------------------------------------------------------------------------

static bool PtrQ(strptr reftype) {
    return reftype == dmmeta_Reftype_reftype_Upptr
        || reftype == dmmeta_Reftype_reftype_Ptr;
}

// -----------------------------------------------------------------------------

static void GuessVia(dmmeta::Field &, acr_ed::FCtype &parent) {
    bool is_global = name_Get(parent) == "FDb";
    // must pick an access path from root to parent for non-global xref
    // do not pick if the xreffld is a pointer!
    acr_ed::FField &fxreffld = acr_ed::ind_field_FindX(acr_ed::_db.keyfld);
    if (!PtrQ(fxreffld.reftype) && acr_ed::_db.cmdline.xref && !is_global && ch_N(acr_ed::_db.viafld) == 0) {
        int nmatch = 0;
        acr_ed::FCtype &fdb = acr_ed::ind_ctype_FindX(tempstr() << ns_Get(parent) << ".FDb");
        ind_beg(acr_ed::ctype_c_field_curs, dbfield, fdb) {
            if (dbfield.reftype == dmmeta_Reftype_reftype_Thash && dbfield.arg == parent.ctype) {
                acr_ed::_db.viafld = dbfield.field;
                nmatch++;
            }
        }ind_end;
        if (nmatch == 0 || nmatch > 1) {
            prerr("For xref between two tables where the parent is non-global, via must be specified");
            prerr("   So that the parent row can be found.");
        }
        if (nmatch > 1) {
            prerr("There is more than one global hash that that can access "<<parent.ctype);
            ind_beg(acr_ed::ctype_c_field_curs, dbfield, fdb) {
                if (dbfield.reftype == dmmeta_Reftype_reftype_Thash && dbfield.arg == parent.ctype) {
                    prerr("->   "<<dbfield.field);
                }
            }ind_end;
            prerr("Choose one of the above, and re-run acr_ed with -via argument like this: "
                  << "-via:"<<acr_ed::_db.viafld<<"/SOMEKEY");
        }
        if (nmatch == 0 || nmatch > 1) {
            prerr("Via was not specified, and could not be guessed");
            vrfy(0,"error");
        }
    }
}

// -----------------------------------------------------------------------------

static void PrintNewField(dmmeta::Field &field) {
    acr_ed::_db.out_ssim << field << eol;

    acr_ed::FCtype &parent = acr_ed::ind_ctype_FindX(ctype_Get(field));

    // if last field is named comment, insert before that...
    if (ch_N(acr_ed::_db.cmdline.before) == 0 && c_field_N(parent) > 0) {
        if (name_Get(*c_field_Find(parent, c_field_N(parent)-1)) == "comment") {
            acr_ed::_db.cmdline.before = c_field_Find(parent, c_field_N(parent)-1)->field;
        }
    }

    if (ch_N(acr_ed::_db.cmdline.before) > 0) {
        acr_ed::FField &rhs = acr_ed::ind_field_FindX(acr_ed::_db.cmdline.before);
        acr_ed::_db.out_ssim.ch_n--;// newline?
        acr_ed::_db.out_ssim << "  acr.rowid:"<<rhs.rowid-0.5 << eol;
    }
}
void acr_ed::edaction_Delete_Field() {
    prlog("acr_ed.delete_field"
          <<Keyval("field",acr_ed::_db.cmdline.field));
    command::acr acr;
    acr.query << "field:"<<acr_ed::_db.cmdline.field;
    acr.del=true;
    acr.write=true;
    acr_ed::_db.script << acr_ToCmdline(acr) << eol;
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Rename_Field() {
    prlog("acr_ed.rename_field"
          <<Keyval("field",acr_ed::_db.cmdline.field));
    command::acr acr;
    // if called as
    // acr_ed -field a.B.c -rename d,
    // rewrite as
    // acr_ed -field a.B.c -rename a.B.d
    vrfy(ind_field_Find(_db.cmdline.field), "no such field");
    if (dmmeta::Field_ctype_Get(_db.cmdline.rename) == "") {
        _db.cmdline.rename = tempstr() << dmmeta::Field_ctype_Get(_db.cmdline.field)
                                       << "." << _db.cmdline.rename;
    }
    acr.query << "field:"<<acr_ed::_db.cmdline.field;
    acr.rename=_db.cmdline.rename;
    acr.write=true;
    acr_ed::_db.script << acr_ToCmdline(acr) << eol;
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Create_Field() {
    prlog("acr_ed.create_field"
          <<Keyval("field",acr_ed::_db.cmdline.field));

    // create this field
    dmmeta::Field field;
    field.field         = acr_ed::_db.cmdline.field;

    acr_ed::FField *existing_field = acr_ed::ind_field_Find(field.field);

    // no arg specified, but a field is a cross product of 2 other types
    // use Smallstr50 as the field type
    // the two other types will be read as substrings
    if (acr_ed::_db.cmdline.arg == "" && acr_ed::_db.cmdline.subset != "" && acr_ed::_db.cmdline.subset2 != "") {
        acr_ed::_db.cmdline.arg = "algo.Smallstr50";
    }

    // -subset can be used in place of -arg:
    // acr_ed -create -field a.B.c -subset x.Y is rewritten as
    //    acr_ed -create -field a.B.c -arg x.Y -reftype Pkey
    if (acr_ed::_db.cmdline.arg == "" && acr_ed::_db.cmdline.subset != "" && acr_ed::_db.cmdline.subset2 == "") {
        acr_ed::_db.cmdline.arg = acr_ed::_db.cmdline.subset;
        acr_ed::_db.cmdline.reftype = SubsetPickReftype(acr_ed::_db.cmdline.subset);
    }

    // guess target type from field name
    if (!existing_field && acr_ed::_db.cmdline.arg == "") {
        if (acr_ed::FCtype *farg = GuessArg(acr_ed::_db.cmdline.field)) {
            prlog("acr_ed.guess_arg"
                  <<Keyval("arg",farg->ctype));
            acr_ed::_db.cmdline.arg = farg->ctype;
            acr_ed::_db.cmdline.xref = true;
        }
    }

    if (!existing_field) {
        vrfy(ch_N(acr_ed::_db.cmdline.arg) > 0, "Please specify field type with -arg");
        field.comment.value = acr_ed::_db.cmdline.comment;
        field.arg           = acr_ed::_db.cmdline.arg;
        field.dflt.value    = acr_ed::_db.cmdline.dflt;
    } else {
        field.arg           = existing_field->arg;// will be accessed later
    }

    // guess reftype
    if (ch_N(acr_ed::_db.cmdline.reftype) == 0) {
        GuessReftype(field);
    }
    field.reftype = acr_ed::_db.cmdline.reftype;

    acr_ed::FCtype *parent = &acr_ed::ind_ctype_FindX(ctype_Get(field));
    acr_ed::FCtype *child = &acr_ed::ind_ctype_FindX(field.arg);
    if (StartsWithQ(name_Get(field),"p_")) {
        algo::TSwap(parent,child);
    }

    bool is_global = name_Get(*parent) == "FDb";
    acr_ed::vis_Alloc() << parent->ctype;
    acr_ed::vis_Alloc() << child->ctype;
    if (acr_ed::FCtype *glob = acr_ed::ind_ctype_Find(tempstr() << ns_Get(*parent) << ".FDb")) {
        acr_ed::vis_Alloc() << glob->ctype;
    }

    // xref is implied:
    if (ch_N(acr_ed::_db.cmdline.via) > 0) {
        acr_ed::_db.cmdline.xref = true;
    }
    if (acr_ed::_db.cmdline.xref) {
        acr_ed::_db.keyfld = dmmeta::Xref_keyfld_Get(acr_ed::_db.cmdline.via);
        acr_ed::_db.viafld = dmmeta::Xref_viafld_Get(acr_ed::_db.cmdline.via);
    }

    // must pick an xreffld. look in fields of child type until we find a field of type pkey parent.
    if (acr_ed::_db.cmdline.xref && !is_global && ch_N(acr_ed::_db.keyfld) == 0) {
        acr_ed::_db.keyfld = GuessXreffld(field, *parent, *child);
        prlog("keyfield is "<<acr_ed::_db.keyfld<<", child is "<<child->ctype<<", is pkey?"<<IsPkey(*child,acr_ed::_db.keyfld));
        if (IsPkey(*child, acr_ed::_db.keyfld) && acr_ed::_db.could_be_ptr) {
            prlog("acr_ed.subset"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","Chooseing Ptr reftype because xref goes through primary key"));
            field.reftype = dmmeta_Reftype_reftype_Ptr;
        }
    }

    // output the computed field
    if (!existing_field) {
        PrintNewField(field);
    }

    // create fstep should be a separate thing.
    if (ch_N(acr_ed::_db.cmdline.fstep) > 0) {
        dmmeta::Fstep fstep;
        fstep.fstep = tempstr() << ns_Get(*parent) << ".FDb." << name_Get(field);
        fstep.steptype = acr_ed::_db.cmdline.fstep;
        acr_ed::_db.out_ssim << fstep << eol;
    }

    if (ch_N(acr_ed::_db.cmdline.substr) > 0) {
        dmmeta::Substr substr;
        substr.field = field.field;
        // determine srcfield... if doesn't exist, pick the first field of parent ctype!
        if (!ch_N(acr_ed::_db.cmdline.srcfield)) {
            vrfy(c_field_N(*parent) > 0, "Cannot guess srcfield for substr: no fields in parent");
            acr_ed::_db.cmdline.srcfield = c_field_Find(*parent,0)->field;
        }
        substr.srcfield = acr_ed::_db.cmdline.srcfield;
        substr.expr.value = acr_ed::_db.cmdline.substr;
        acr_ed::_db.out_ssim << substr << eol;
    }

    if (ch_N(acr_ed::_db.keyfld) > 0) {
        GuessVia(field, *parent);
    }

    if (acr_ed::_db.cmdline.anon) {
        dmmeta::Anonfld anonfld;
        anonfld.field = field.field;
        acr_ed::_db.out_ssim << anonfld << eol;
    }

    if (acr_ed::_db.cmdline.bigend) {
        vrfy(field.reftype == dmmeta_Reftype_reftype_Val, "Bigend should be used with Val only");
        dmmeta::Fbigend fbigend;
        fbigend.field = field.field;
        acr_ed::_db.out_ssim << fbigend << eol;
    }
    if (ch_N(acr_ed::_db.cmdline.cppfunc)) {
        dmmeta::Cppfunc cppfunc;
        cppfunc.field = field.field;
        cppfunc.expr.value = acr_ed::_db.cmdline.cppfunc;
        Cppfunc_Print(cppfunc,acr_ed::_db.out_ssim);
        acr_ed::_db.out_ssim << eol;
    }
    if (acr_ed::_db.cmdline.cascdel) {
        dmmeta::Cascdel cascdel;
        cascdel.field = field.field;
        acr_ed::_db.out_ssim << cascdel << eol;
    }
    if (acr_ed::_db.cmdline.alias) {
        vrfy(_db.cmdline.srcfield != "" && ind_field_Find(_db.cmdline.srcfield),
             tempstr()<<"-alias requires -srcfield");
        dmmeta::Falias falias;
        falias.field = field.field;
        falias.srcfield = _db.cmdline.srcfield;
        acr_ed::_db.out_ssim << falias << eol;
    }
    if (acr_ed::_db.cmdline.anonfld) {
        dmmeta::Anonfld anonfld;
        anonfld.field = field.field;
        acr_ed::_db.out_ssim << anonfld << eol;
    }

    // this field is cross-referenced with arg.
    if (acr_ed::_db.cmdline.xref) {
        dmmeta::Xref xref;
        xref.field = field.field;
        xref.inscond.value = acr_ed::_db.cmdline.inscond;// c++ expr
        xref.via = acr_ed::_db.viafld != ""
            ? strptr(tempstr()<<acr_ed::_db.viafld<<"/"<<acr_ed::_db.keyfld)
            : strptr(acr_ed::_db.keyfld);
        acr_ed::_db.out_ssim << xref << eol;
    }

    if (_db.cmdline.subset2 != "") {
        dmmeta::Ctype ctype;
        ctype_CopyOut(*parent,ctype);
        acr_ed::CreateCrossProduct(ctype, field);
    }

    if (existing_field == NULL) {
        InsertFieldExtras(field.field, field.arg, field.reftype);
    }
    if (_db.cmdline.indexed) {
        CreateHashIndex(field);
    }
}

// -----------------------------------------------------------------------------

static tempstr GuessSortfld(strptr arg) {
    tempstr ret(acr_ed::_db.cmdline.sortfld);
    if (!ch_N(ret)) {
        ret = acr_ed::PkeyField(arg)->field;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr GuessHashfld(strptr arg) {
    tempstr ret(acr_ed::_db.cmdline.hashfld);
    if (!ch_N(ret)) {
        ret = acr_ed::PkeyField(arg)->field;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Look at field FIELD, which is of reftype acr_ed::_db.cmdline.reftype.
// Create any required record for it:
// Ptrary -> dmmeta.ptrary
// Llist -> dmmeta.llist
// Tary -> dmmeta.tary
// Bheap -> dmmeta.bheap
// Thash -> dmmeta.thash
// Inlary -> dmmeta.inlary
void acr_ed::InsertFieldExtras(strptr field, strptr arg, strptr reftype) {
    // the x-ref is a ptrary -- print defaults
    if (reftype == dmmeta_Reftype_reftype_Ptrary) {
        dmmeta::Ptrary ptrary;
        ptrary.field   = field;
        ptrary.unique  = true;
        acr_ed::_db.out_ssim << ptrary << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Llist) {
        dmmeta::Llist llist;
        llist.field   = field;
        acr_ed::FListtype &listtype = acr_ed::ind_listtype_FindX(listtype_Get(llist));
        llist.havecount = listtype.haveprev;// heuristic
        llist.havetail = listtype.instail && !(listtype.circular && listtype.haveprev);
        acr_ed::_db.out_ssim << llist << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Tary) {
        dmmeta::Tary tary;
        tary.field   = field;
        acr_ed::_db.out_ssim << tary << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Bheap ||
               reftype == dmmeta_Reftype_reftype_Atree) {
        dmmeta::Sortfld sortfld;
        sortfld.field   = field;
        sortfld.sortfld   = GuessSortfld(arg);
        acr_ed::_db.out_ssim << sortfld << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Thash) {
        dmmeta::Thash thash;
        thash.field   = field;
        thash.hashfld = GuessHashfld(arg);
        acr_ed::_db.out_ssim << thash << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Inlary) {
        dmmeta::Inlary inlary;
        inlary.field   = field;
        Inlary_Print(inlary, acr_ed::_db.out_ssim);
        acr_ed::_db.out_ssim << eol;
    } else if (reftype == dmmeta_Reftype_reftype_Hook) {
        dmmeta::Hook hook;
        hook.field   = field;
        acr_ed::_db.out_ssim << hook << eol;
    }
}

// -----------------------------------------------------------------------------

// Trivial function to make a field indexed by a hash.
// This is equivalent to creating an FDb.ind_<name>
void acr_ed::CreateHashIndex(dmmeta::Field &field) {
    tempstr ctype(ctype_Get(field));
    tempstr ns(ns_Get(field));
    dmmeta::Field hash;
    hash.field   = tempstr() << ns << ".FDb.ind_" << name_Get(field);
    hash.arg     = ctype;
    hash.reftype = dmmeta_Reftype_reftype_Thash;
    acr_ed::_db.out_ssim << hash << eol;

    dmmeta::Thash thash;
    thash.field   = hash.field;
    thash.hashfld = field.field;
    acr_ed::_db.out_ssim << thash   << eol;

    dmmeta::Xref xref;
    xref.field     = hash.field;
    xref.inscond.value = "true";
    acr_ed::_db.out_ssim << xref    << eol;
}
