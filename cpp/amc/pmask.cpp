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
// Source: cpp/amc/pmask.cpp
//

#include "include/amc.h"

static bool NeedPmaskQ(amc::FField &field) {
    amc::FPmaskfld *pmaskfld = field.p_ctype->c_pmaskfld;// optional presence mask
    bool need = field.reftype != dmmeta_Reftype_reftype_Base;
    need &= pmaskfld && pmaskfld->p_field != &field;
    return need;
}

void amc::tclass_Pmask() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FPmaskfld *pmaskfld = field.p_ctype->c_pmaskfld;// optional presence mask
    vrfy(pmaskfld, tempstr()<<"amc.missing_pmask"
         <<Keyval("field",field.field)
         <<Keyval("ctype",ctype_Get(field)));

    if (NeedPmaskQ(field)) {
        int bit = pmaskfld->nextbit;
        int psizeof = amc::Field_Sizeof(*pmaskfld->p_field);
        vrfy(bit < psizeof*8, tempstr() << "amc.val"
             <<Keyval("field",field.field)
             <<Keyval("pmask",pmaskfld->field)
             <<Keyval("error","Out of bits. Too many fields"));
        pmaskfld->nextbit++;
        Set(R, "$pmask", name_Get(*pmaskfld->p_field));
        Set(R, "$bit", tempstr() << bit);
    }
}

void amc::tfunc_Pmask_PresentQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (NeedPmaskQ(field)) {
        amc::FFunc& presentq = amc::CreateCurFunc();
        presentq.inl = true;
        Ins(&R, presentq.comment, "Return true if the field is marked in the presence mask");
        Ins(&R, presentq.ret  , "bool",false);
        Ins(&R, presentq.proto, "$name_PresentQ($Parent)",false);
        Ins(&R, presentq.body , "return $pmask_qGetBit($parname, $bit);");
    }
}

void amc::tfunc_Pmask_SetPresent() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (NeedPmaskQ(field)) {
        amc::FFunc& setpresent = amc::CreateCurFunc();
        Ins(&R, setpresent.ret  , "void", false);
        Ins(&R, setpresent.proto, "$name_SetPresent($Parent)", false);
        setpresent.inl = true;
        Ins(&R, setpresent.body, "$pmask_qSetBit($pararg, $bit); // mark presence in pmask");
    }
}
