// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Source: cpp/acr_ed/ctype.cpp -- Create, delete, rename ctype
//

#include "include/acr_ed.h"
#include "include/algo.h"
// -----------------------------------------------------------------------------

// Determine name of pkey for the newly created ctype
// By converting the ctype's name to lower_under form.
static tempstr PkeyName(strptr ctype) {
    algo::tempstr _name(dmmeta::Ctype_name_Get(ctype));
    algo::strptr name(_name);
    // if name starts with two caps, drop the first one
    if (ch_N(name)>2
        && algo_lib::UpperCharQ(name.elems[0])
        && algo_lib::UpperCharQ(name.elems[1])
        && !algo_lib::UpperCharQ(name.elems[2])
        ) {
        name=ch_RestFrom(name,1);
    }
    tempstr ret;
    strptr_PrintLowerUnder(name,ret);
    return ret;
}

// -----------------------------------------------------------------------------

// Pick a default reftype when creating a subset
// If the target type is relational, use Pkey. Otherwise use Val
dmmeta::ReftypePkey acr_ed::SubsetPickReftype(algo::strptr ctype_key) {
    dmmeta::ReftypePkey ret(dmmeta_Reftype_reftype_Pkey);
    acr_ed::FCtype *ctype=ind_ctype_Find(ctype_key);
    if (ctype && !ctype->c_ssimfile) {
        ret=dmmeta_Reftype_reftype_Val;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Structured pkey creation: triggered with -subset X -subset2 Y -separator Z
// Two fields are created under CTYPE:
// one referring to ctype cmdline.subset, the other to cmdline.subset2
// The fields are substrings of FIELD_PKEY
void acr_ed::CreateCrossProduct(dmmeta::Ctype &ctype, dmmeta::Field &field_pkey) {
    acr_ed::_db.out_ssim<<eol;
    // left half
    dmmeta::Field field_substr1;
    field_substr1.field   = tempstr()<<ctype.ctype << "." << PkeyName(acr_ed::_db.cmdline.subset);
    field_substr1.arg     = acr_ed::_db.cmdline.subset;
    field_substr1.reftype = SubsetPickReftype(acr_ed::_db.cmdline.subset);
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
    field_substr2.reftype = SubsetPickReftype(acr_ed::_db.cmdline.subset2);
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

// Looking for max(msgtypes value) where msgtypes first field is Base and arg is the target subset
// and return max+1 assuming that msgtypes value is integer.
// ignore:bigret
cstring acr_ed::getNextMsgTypeValue(strptr target_subset) {
    u32 next_msg_type_value = 0;
    ind_beg(acr_ed::_db_msgtype_curs, msgtype,_db) {
        auto *base_type = ind_ctype_Find(msgtype.ctype);
        if (base_type && base_type->c_field_n > 0) {
            auto msgtype_ctype_w_base = dmmeta::Field_Concat_ctype_name(msgtype.ctype, "base");
            // If the first field of this msgtype is Base and the arg is the target subset
            if (msgtype_ctype_w_base == base_type->c_field_elems[0]->field
                && target_subset == base_type->c_field_elems[0]->arg) {
                algo::StringIter iter(msgtype.type.value);
                u32 msgtype_value;
                if (algo::TryParseU32(iter, msgtype_value)
                    && msgtype_value > next_msg_type_value) {
                    next_msg_type_value = msgtype_value;
                }
            }
        }
    }ind_end;
    tempstr ret;
    ret << next_msg_type_value+1;
    return ret;
}

// -----------------------------------------------------------------------------

// Create a new ctype
// The new type can be relational, i.e. a subset of a cross product of 2 other types,
// or an in-memory type.
// Example 1:
// acr_ed -create -ctype acmdb.Devos -subset1 acmdb.Device -subset2 acmdb.Os -separator /
// Example 2:
// acr_ed -create -ctype atf_tmsg.FOrder -pooltype Tpool -indexed
void acr_ed::edaction_Create_Ctype() {
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

    // now look up namespace
    // from ctype
    acr_ed::FNs *ns = ind_ns_Find(dmmeta::Ctype_ns_Get(_db.cmdline.ctype));

    dmmeta::Ctype ctype;
    ctype.ctype = acr_ed::_db.cmdline.ctype;
    ctype.comment.value = acr_ed::_db.cmdline.comment;
    acr_ed::_db.out_ssim << ctype << eol;

    dmmeta::Field pkey;
    if (!ns) {
        prerr("acr_ed.invalid_namespace"
              <<Keyval("ns",dmmeta::Ctype_ns_Get(_db.cmdline.ctype))
              <<Keyval("comment","namespace doesn't exist"));
        algo_lib::_db.exit_code=1;
    }
    bool relational = ns && ns->nstype == dmmeta_Nstype_nstype_ssimdb;

    // when creating a relational type, pick default subset
    // if -indexed was specified with -create -ctype, we also need a field,
    // so pick a type for it (can always be edited later)
    if (acr_ed::_db.cmdline.subset == "" && (relational || _db.cmdline.indexed)) {
        acr_ed::_db.cmdline.subset = "algo.Smallstr50";
    }

    // if new ctype is a subset of one other relational ctype, use that type's name.
    if (ch_N(acr_ed::_db.cmdline.subset2) == 0) {
        acr_ed::FCtype &subset = acr_ed::ind_ctype_FindX(acr_ed::_db.cmdline.subset);
        // if creating a subset of a relational type, borrow the name of the first field
        if (relational) {
            if (c_field_N(subset) > 0 && subset.c_ssimfile) {
                pkey_name = name_Get(*c_field_Find(subset,0));
            }
            if (subset.p_ns->nstype == dmmeta_Nstype_nstype_ssimdb) {
                pkey.reftype = dmmeta_Reftype_reftype_Pkey;
            } else {
                pkey.reftype = dmmeta_Reftype_reftype_Val;
            }
        } else if (subset.c_typefld && algo::ch_N(subset.c_typefld->field)) {
            // If subset has typefld entry this is proto Msg type thus using Base reftype
            pkey.reftype = dmmeta_Reftype_reftype_Base;
        }
    }

    // print pkey -- if -subset was specified
    if (ch_N(acr_ed::_db.cmdline.subset) > 0) {
        if (acr_ed::_db.cmdline.reftype != "") {
            pkey.reftype = acr_ed::_db.cmdline.reftype;
        }
        if (pkey.reftype == dmmeta_Reftype_reftype_Base) {
            pkey.field = dmmeta::Field_Concat_ctype_name(ctype.ctype, "base");
        } else {
            pkey.field = dmmeta::Field_Concat_ctype_name(ctype.ctype, pkey_name);
        }
        pkey.arg = ch_N(acr_ed::_db.cmdline.subset2) == 0
            ? strptr(acr_ed::_db.cmdline.subset)
            : strptr("algo.Smallstr50");
        acr_ed::_db.out_ssim << pkey << eol;
        acr_ed::_db.cmdline.field = pkey.field;//save it
        InsertFieldExtras(pkey.field, pkey.arg, acr_ed::_db.cmdline.reftype);
        // create extra fields
        auto *base_type = ind_ctype_Find(_db.cmdline.subset);
        if (base_type && pkey.reftype == dmmeta_Reftype_reftype_Base) {
            if (base_type->c_pack) {
                dmmeta::Pack pack;
                pack.ctype = ctype.ctype;
                acr_ed::_db.out_ssim << pack << eol;
            }
            if (base_type->c_typefld) {
                dmmeta::Msgtype msgtype;
                msgtype.ctype = ctype.ctype;
                cstring msgtype_str_arg = acr_ed::_db.cmdline.msgtype.ch_n
                    ? cstring(acr_ed::_db.cmdline.msgtype)
                    : getNextMsgTypeValue(acr_ed::_db.cmdline.subset);
                algo::CppExpr_ReadStrptrMaybe(msgtype.type,msgtype_str_arg);
                acr_ed::_db.out_ssim << msgtype << eol;
            }
            if (base_type->c_cpptype) {
                dmmeta::Cpptype cpptype;
                cpptype.ctype = ctype.ctype;
                cpptype.ctor = false;
                cpptype.dtor = false;
                cpptype.cheap_copy = false;
                acr_ed::_db.out_ssim << cpptype << eol;
            }
            ind_beg(acr_ed::ctype_c_cfmt_curs,c_cfmt,*base_type){
                dmmeta::Cfmt cfmt;
                cfmt.cfmt = dmmeta::Cfmt_Concat_ctype_strfmt(ctype.ctype, acr_ed::strfmt_Get(c_cfmt));
                cfmt.printfmt = c_cfmt.printfmt;
                cfmt.print = c_cfmt.print;
                cfmt.read = c_cfmt.read;
                cfmt.genop = c_cfmt.genop;
                cfmt.sep = c_cfmt.sep;
                acr_ed::_db.out_ssim << cfmt << eol;
            }ind_end;
        }
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

        // output required records for this pool type
        if (ch_N(acr_ed::_db.cmdline.pooltype) && acr_ed::_db.cmdline.pooltype == dmmeta_Reftype_reftype_Tary) {
            dmmeta::Tary tary;
            tary.field = pool.field;
            acr_ed::_db.out_ssim << tary << eol;
        }
        if (ch_N(acr_ed::_db.cmdline.pooltype) && acr_ed::_db.cmdline.pooltype == dmmeta_Reftype_reftype_Inlary) {
            dmmeta::Inlary inlary;
            inlary.field = pool.field;
            inlary.min=0;
            inlary.max=1;
            acr_ed::_db.out_ssim << inlary << eol;
        }
    }

    if (acr_ed::_db.cmdline.indexed && name_Get(pkey) != "") {
        CreateHashIndex(pkey);
    }
}

// -----------------------------------------------------------------------------

// acr_ed -ctype:X -del -write
void acr_ed::edaction_Delete_Ctype() {
    prlog("acr_ed.delete_ctype"
          <<Keyval("ctype",acr_ed::_db.cmdline.ctype));
    acr_ed::FCtype &ctype = acr_ed::ind_ctype_FindX(acr_ed::_db.cmdline.ctype);
    algo_lib::Replscope R;
    Set(R, "$ctype", ctype.ctype);

    Ins(&R, acr_ed::_db.script, "bin/acr ctype:$ctype -del -write");
}

// -----------------------------------------------------------------------------

// acr_ed -ctype:X -rename:Y -write
void acr_ed::edaction_Rename_Ctype() {
    prlog("acr_ed.rename_ctype"
          <<Keyval("ctype",acr_ed::_db.cmdline.target)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    algo_lib::Replscope R;
    Set(R, "$oldctype", acr_ed::_db.cmdline.ctype);
    Set(R, "$newctype", acr_ed::_db.cmdline.rename);
    // nothing more to do...
    Ins(&R, acr_ed::_db.script, "bin/acr ctype:$oldctype -rename $newctype  -write");
}
