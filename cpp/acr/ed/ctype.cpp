// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr/ed/ctype.cpp -- Create, delete, rename ctype
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

// Determine name of pkey for the newly created ctype
// By converting the ctype's name to lower_under form.
static tempstr PkeyName(strptr ctype) {
    tempstr ret(acr_ed::ToLowerUnder(dmmeta::Ctype_name_Get(ctype)));
    // if command line read -create -ctype amc.FCtype,
    // remove leading f_ from pkey name.
    if (StartsWithQ(ret,"f_")) {
        ret=ch_RestFrom(ret,2);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Structured pkey creation: triggered with -subset X -subset2 Y -separator Z
// The key consists of 2 substrs, each of which is a foreign key, separated
// by a certain character.
static void CreateCrossProduct(dmmeta::Ctype &ctype, dmmeta::Field &field_pkey) {
    acr_ed::_db.out_ssim<<eol;
    // left half
    dmmeta::Field field_substr1;
    field_substr1.field   = tempstr()<<ctype.ctype << "." << PkeyName(acr_ed::_db.cmdline.subset);
    field_substr1.arg     = acr_ed::_db.cmdline.subset;
    field_substr1.reftype = dmmeta_Reftype_reftype_Pkey;
    acr_ed::_db.out_ssim << field_substr1 << eol;

    dmmeta::Substr substr_substr1;
    substr_substr1.field = field_substr1.field;
    substr_substr1.expr.value = tempstr() << acr_ed::_db.cmdline.separator << "RL";
    substr_substr1.srcfield = field_pkey.field;
    acr_ed::_db.out_ssim << substr_substr1 << eol;

    // right half
    // right half can be a relation (e.g. acmdb.Device) or a string (e.g. algo.Smallstr100).
    // If it is a string (which we know by loading cstr table)
    acr_ed::_db.out_ssim<<eol;
    dmmeta::Field field_substr2;
    acr_ed::FCtype &ctype2 = acr_ed::ind_ctype_FindX(acr_ed::_db.cmdline.subset2);
    field_substr2.field   = tempstr()<<ctype.ctype << "." << PkeyName(acr_ed::_db.cmdline.subset2);
    field_substr2.arg     = acr_ed::_db.cmdline.subset2;
    field_substr2.reftype = dmmeta_Reftype_reftype_Pkey;
    if (ctype2.c_cstr) {
        field_substr2.field    = tempstr() << ctype_Get(field_substr2) << ".name";
        field_substr2.reftype =  dmmeta_Reftype_reftype_Val;
    }
    acr_ed::_db.out_ssim << field_substr2 << eol;

    dmmeta::Substr substr_substr2;
    substr_substr2.field = field_substr2.field;
    substr_substr2.srcfield = field_pkey.field;
    substr_substr2.expr.value = tempstr() << acr_ed::_db.cmdline.separator << "RR";
    acr_ed::_db.out_ssim << substr_substr2 << eol;
    acr_ed::_db.out_ssim<<eol;
}

// -----------------------------------------------------------------------------

// Create a new ctype
// The new type can be relational, i.e. a subset of a cross product of 2 other types,
// or an in-memory type.
// Example 1:
// acr_ed -create -ctype acmdb.Devos -subset1 acmdb.Device -subset2 acmdb.Os -separator /
// Example 2:
// acr_ed -create -ctype atf_tmsg.FOrder -pooltype Tpool -indexed
void acr_ed::Main_CreateCtype() {
    acr_ed::FNs *ns = ind_ns_Find(dmmeta::Ctype_ns_Get(_db.cmdline.ctype));

    // Determine name of pkey for the newly created ctype
    // - if ssimfile name is known, pick ssimfile name;
    // - otherwise, construct the pkey name from ctype name
    tempstr pkey_name =  acr_ed::_db.cmdline.ssimfile != ""
        ? tempstr(dmmeta::Ssimfile_name_Get(acr_ed::_db.cmdline.ssimfile))
        : tempstr(PkeyName(acr_ed::_db.cmdline.ctype));

    // Guess ctype name from ssimfile name
    // (If ctype name was not specified on the command line, but
    // ssimfile name was specified)
    if (!ch_N(acr_ed::_db.cmdline.ctype)) {
        tempstr ctype_name(acr_ed::_db.cmdline.ctype);
        ctype_name << GetNs(acr_ed::_db.cmdline.ssimfile);
        ctype_name << ".";
        strptr_PrintCamel(pkey_name, ctype_name);
        acr_ed::_db.cmdline.ctype = ctype_name;
    }

    dmmeta::Ctype ctype;
    ctype.ctype = acr_ed::_db.cmdline.ctype;
    ctype.comment.value = acr_ed::_db.cmdline.comment;
    acr_ed::_db.out_ssim << ctype << eol;

    dmmeta::Field pkey;
    bool relational = ns && ns->nstype == dmmeta_Nstype_nstype_ssimdb;

    // when creating a relational type, pick default subset
    // if -indexed was specified with -create -ctype, we also need a field,
    // so pick a type for it (can always be edited later)
    if (acr_ed::_db.cmdline.subset == "" && (relational || _db.cmdline.indexed)) {
        acr_ed::_db.cmdline.subset = "algo.Smallstr50";
    }

    // if new ctype is a subset of one other relational ctype, use that type's name.
    if (ch_N(acr_ed::_db.cmdline.subset2) == 0 && relational) {
        acr_ed::FCtype &subset = acr_ed::ind_ctype_FindX(acr_ed::_db.cmdline.subset);
        if (c_field_N(subset) > 0) {
            pkey_name = name_Get(*c_field_Find(subset,0));
        }
        if (subset.p_ns->nstype == dmmeta_Nstype_nstype_ssimdb) {
            pkey.reftype = dmmeta_Reftype_reftype_Pkey;
        } else {
            pkey.reftype = dmmeta_Reftype_reftype_Val;
        }
    }

    // print pkey -- if -subset was specified
    if (ch_N(acr_ed::_db.cmdline.subset) > 0) {
        pkey.field = tempstr() << ctype.ctype << "." << pkey_name;
        if (acr_ed::_db.cmdline.reftype != "") {
            pkey.reftype = acr_ed::_db.cmdline.reftype;
        }
        pkey.arg = ch_N(acr_ed::_db.cmdline.subset2) == 0
            ? strptr(acr_ed::_db.cmdline.subset)
            : strptr("algo.Smallstr50");
        acr_ed::_db.out_ssim << pkey << eol;
        acr_ed::_db.cmdline.field = pkey.field;//save it
        InsertFieldExtras(pkey.field, pkey.arg, acr_ed::_db.cmdline.reftype);
    }

    // subset of a 2 other ctypes: substrings using a separator
    if (acr_ed::_db.cmdline.subset2 != "") {
        CreateCrossProduct(ctype,pkey);
    }

    // Create a pool for the ctype
    if (ch_N(acr_ed::_db.cmdline.pooltype)) {
        dmmeta::Field pool;
        pool.field = tempstr() << ns_Get(ctype) <<".FDb." << pkey_name;
        pool.arg = ctype.ctype;
        pool.reftype = acr_ed::_db.cmdline.pooltype;
        acr_ed::_db.out_ssim << pool << eol;
    }

    if (acr_ed::_db.cmdline.indexed && name_Get(pkey) != "") {
        CreateHashIndex(pkey);
    }
}

// -----------------------------------------------------------------------------

// acr_ed -ctype:X -del -write
void acr_ed::Main_DeleteCtype() {
    prlog("acr_ed.delete_ctype"
          <<Keyval("ctype",acr_ed::_db.cmdline.ctype));
    acr_ed::FCtype &ctype = acr_ed::ind_ctype_FindX(acr_ed::_db.cmdline.ctype);
    algo_lib::Replscope R;
    Set(R, "$ctype", ctype.ctype);

    Ins(&R, acr_ed::_db.script, "bin/acr ctype:$ctype -del -write");
}

// -----------------------------------------------------------------------------

// acr_ed -ctype:X -rename:Y -write
void acr_ed::Main_RenameCtype() {
    prlog("acr_ed.rename_ctype"
          <<Keyval("ctype",acr_ed::_db.cmdline.target)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    algo_lib::Replscope R;
    Set(R, "$oldctype", acr_ed::_db.cmdline.ctype);
    Set(R, "$newctype", acr_ed::_db.cmdline.rename);
    // nothing more to do...
    Ins(&R, acr_ed::_db.script, "bin/acr ctype:$oldctype -rename $newctype  -write");
}
