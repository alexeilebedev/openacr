// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/acr_ed/finput.cpp
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

static tempstr Pooltype() {
    tempstr ret;
    ret = dmmeta_Reftype_reftype_Lary;
    if (acr_ed::_db.cmdline.pooltype != "") {
        ret = acr_ed::_db.cmdline.pooltype;
    }
    if (acr_ed::_db.cmdline.reftype != "") {
        ret = acr_ed::_db.cmdline.reftype;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// #AL# todo: merge this with -create -ctype
void acr_ed::edaction_Create_Finput() {
    prlog("acr_ed.create_finput"
          <<Keyval("target",acr_ed::_db.cmdline.target)
          <<Keyval("ssimfile",acr_ed::_db.cmdline.ssimfile));
    acr_ed::FSsimfile *ssimfile = acr_ed::ind_ssimfile_Find(acr_ed::_db.cmdline.ssimfile);
    vrfy(ssimfile, "acr_ed.ssimfile_not_found");
    acr_ed::FCtype *fbase = ssimfile->p_ctype;
    vrfy(c_field_N(*fbase), "acr_ed.finput_nofields  comment:'base has no fields'");
    tempstr name(strptr(name_Get(*ssimfile)));

    // derive an F-type from the ctype
    dmmeta::Ctype ftype;
    ftype.ctype = tempstr() << acr_ed::_db.cmdline.target << ".F" << name_Get(*fbase);
    ftype.comment.value = acr_ed::_db.cmdline.comment;
    ctype_InsertMaybe(ftype);
    acr_ed::_db.out_ssim << ftype   << eol;

    dmmeta::Field base;
    base.field   = tempstr() << ftype.ctype << ".base";
    base.arg     = fbase->ctype;
    base.reftype = dmmeta_Reftype_reftype_Base;
    acr_ed::_db.out_ssim << base   << eol;

    // allocation
    dmmeta::Field pool;
    pool.field   = tempstr() << acr_ed::_db.cmdline.target << ".FDb." << name;
    pool.arg     = ftype.ctype;
    pool.reftype = Pooltype();
    acr_ed::_db.out_ssim << pool    << eol;
    InsertFieldExtras(pool.field, pool.arg, pool.reftype);

    if (acr_ed::_db.cmdline.gstatic) {
        dmmeta::Gstatic gstatic(pool.field,algo::Comment());
        acr_ed::_db.out_ssim << gstatic  << eol;
        // add a hook field
        dmmeta::Field hook;
        hook.field = tempstr() << ftype.ctype << ".step";
        hook.arg = "";
        hook.reftype = dmmeta_Reftype_reftype_Hook;
        acr_ed::_db.out_ssim << hook << eol;
        InsertFieldExtras(hook.field, hook.arg, hook.reftype);
    } else {
        dmmeta::Finput finput(pool.field,false/*extrn*/,false/*update*/,true/*strict*/,algo::Comment());
        acr_ed::_db.out_ssim << finput  << eol;
    }

    if (acr_ed::_db.cmdline.foutput) {
        dmmeta::Foutput foutput;
        foutput.field = pool.field;
        Foutput_Print(foutput, acr_ed::_db.out_ssim);
        acr_ed::_db.out_ssim << eol;
    }

    acr_ed::vis_Alloc() << ftype.ctype;
    acr_ed::vis_Alloc() << acr_ed::_db.cmdline.target<<".FDb";

    // #AL# TODO: rewrite this using
    // CreateHashIndex(hashfld);
    if (acr_ed::_db.cmdline.indexed) {
        // pick a hash based on the first field
        dmmeta::Field hash;
        hash.field   = tempstr() << acr_ed::_db.cmdline.target << ".FDb.ind_" << name;
        hash.arg     = ftype.ctype;
        hash.reftype = dmmeta_Reftype_reftype_Thash;

        dmmeta::Thash thash;
        thash.field   = hash.field;
        thash.hashfld = c_field_Find(*fbase, 0)->field;// first field of ctype

        dmmeta::Xref xref;
        xref.field     = hash.field;
        xref.inscond.value = "true";

        acr_ed::_db.out_ssim << eol;
        acr_ed::_db.out_ssim << hash    << eol;
        acr_ed::_db.out_ssim << thash   << eol;
        acr_ed::_db.out_ssim << xref    << eol;
    }
}
