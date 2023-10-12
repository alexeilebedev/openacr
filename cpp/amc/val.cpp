// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/val.cpp
//

#include "include/amc.h"

void amc::tclass_Val() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // hack
    // what is this crap???
    tempstr dflt;
    dflt << field.dflt;
    Replace(dflt, "*this", Subst(R,"$pararg"));

    // pick a default
    if (field.p_arg->c_cextern && field.p_arg->c_cextern->initmemset && !ch_N(dflt)) {
        dflt = "0";
    }
    Set(R, "$dflt"   , dflt);

    Set(R, "$Fldtype", field.cpp_type);

    // endian suffix.
    Set(R, "$suffix", "");
    Set(R, "$comment", field.comment);

    if (field.c_fbigend) {
        Set(R, "$suffix", "_be");
    }

    if (field.c_finput && !field.c_finput->extrn) {
        prerr("amc.val_finput  "
              <<Keyval("field",field.field)
              <<Keyval("comment","Non-extern finput + val do not mix"));
        algo_lib::_db.exit_code++;
    }
    if (field.c_fbigend && FldfuncQ(field)) {
        prerr("amc.bigend_fldfunc"
              <<Keyval("field",field.field)
              <<Keyval("comment","bad combination of fbigend and fldfunc"));
        algo_lib::_db.exit_code++;
    }

    // data field
    if (!FldfuncQ(field)) {
        InsVar(R, field.p_ctype, "$Fldtype", "$name$suffix", dflt, "$comment");
    }
}

void amc::tfunc_Val_Get() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // retrieve big-endian value.
    if (field.c_fbigend) {
        Set(R, "$WID",LastN(strptr(field.arg),2)); // two last digits
        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , "$Fldtype", false);
        Ins(&R, get.proto, "$name_Get($Cparent)", false);
        get.inl = true;
        Ins(&R, get.body, "return be$WIDtoh($parname.$name_be); // read big-endian value from memory");
    }
}

void amc::tfunc_Val_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    bool pmaskfld = field.p_ctype->c_pmaskfld && field.p_ctype->c_pmaskfld->p_field != &field;
    bool need_set = (field.c_fbigend || (pmaskfld && field.arg != "algo_lib.Regx")) && !PadQ(field);
    if (need_set) {
        amc::FFunc& set = amc::CreateCurFunc();
        Set(R, "$Fldargtype", Argtype(field));
        Ins(&R, set.ret  , "void", false);
        Ins(&R, set.proto, "$name_Set($Parent, $Fldargtype rhs)", false);
        set.inl = true;
        if (field.c_fbigend) {
            Ins(&R, set.body, "$parname.$name_be = htobe$WID(rhs); // write big-endian value to memory");
        } else {
            Ins(&R, set.body, "$parname.$name = rhs;");
        }
        if (pmaskfld) {
            Ins(&R, set.body, "$pmask_qSetBit($pararg, $bit); // mark presence in pmask");
        }
    }
}

void amc::tfunc_Val_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FldfuncQ(field)) {// default initializer for field
        amc::FFunc& init = amc::CreateCurFunc();
        init.inl = true;
        tempstr initcast = Initcast(field);
        Set(R, "$initcast", initcast);
        // Initialize field, but don't use the Set function because
        // it will update presence mask.
        if (ch_N(Subst(R,"$dflt")) > 0) {
            if (field.p_arg->c_cextern && field.p_arg->c_cextern->initmemset) {
                Ins(&R, init.body, "memset(&$parname.$name, $dflt, sizeof($parname.$name));");
            } else if (field.c_fbigend) {
                Ins(&R, init.body, "$parname.$name_be = htobe$WID($dflt); // write big-endian value to memory");
            } else {
                Ins(&R, init.body, "$parname.$name = $initcast($dflt);"); // amc_val.cpp:158");
            }
        }
    }
}

void amc::tfunc_Val_RowidFind() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.reflect) {
        amc::FFunc& rowid_findx = amc::CreateCurFunc();
        rowid_findx.priv=true;
        Ins(&R, rowid_findx.comment, "find $Ctype by row id (used to implement reflection)");
        Ins(&R, rowid_findx.ret, "algo::ImrowPtr", false);
        Ins(&R, rowid_findx.proto, "$name_RowidFind(int t)", false);
        Ins(&R, rowid_findx.body, "return algo::ImrowPtr(t==0 ? u64(&$parname.$name) : u64(0));");
    }
}

void amc::tfunc_Val_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.reflect) {
        amc::FFunc& nitems = amc::CreateCurFunc();
        nitems.priv=true;
        Ins(&R, nitems.comment, "Function return 1");
        Ins(&R, nitems.ret, "i32", false);
        Ins(&R, nitems.proto, "$name_N()", false);
        Ins(&R, nitems.body, "return 1;");
    }
}
