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

// True if field is suitable for inclusion in a pmask
static bool InPmaskQ(amc::FField &field) {
    bool need = field.reftype != dmmeta_Reftype_reftype_Base;
    need &= !field.c_pmaskfld;// not the pmaskfld itself
    need &= !GetLenfld(field);// no pmask bit for length fields
    need &= !field.c_typefld;// no pmask bit for type fields
    return need;
}

void amc::gen_pmask() {
    ind_beg(amc::_db_pmaskfld_curs, pmaskfld, amc::_db) {
        // initialize name
        // for compatibility with old code, the word "pmask" maps to "Present"
        // and other words map to camelcase versions, i.e. nullable -> Nullable
        if (name_Get(*pmaskfld.p_field) == "pmask") {
            pmaskfld.funcname="Present";
        } else {
            tempstr camel;
            algo::strptr_PrintCamel(name_Get(*pmaskfld.p_field),camel);
            pmaskfld.funcname=camel;
        }
        // check that only one pmaskfld is labeled print_filter:Y
        if (pmaskfld.filter_print) {
            ind_beg(ctype_c_pmaskfld_curs,other,*pmaskfld.p_field->p_ctype) {
                vrfy(&other==&pmaskfld || !other.filter_print
                     ,tempstr()<<"amc.multiple_print_filters"
                     <<Keyval("pmaskfld1",pmaskfld.field)
                     <<Keyval("pmaskfld2",other.field)
                     <<Keyval("comment","Only one pmaskfld with filter_print:Y allowed on a ctype"));
            }ind_end;
        }
        // create bitsets for pmask fields
        if (!pmaskfld.p_field->c_fbitset) {
            amc::fbitset_InsertMaybe(dmmeta::Fbitset(pmaskfld.field, algo::Comment()));
        }
        // create pmaskfld members for all suitable fields, unless
        // the list of members was explicitly provided
        if (!c_pmaskfld_member_N(pmaskfld)) {
            ind_beg(ctype_c_field_curs,field,*pmaskfld.p_field->p_ctype) {
                if (InPmaskQ(field)) {
                    dmmeta::PmaskfldMember member;
                    member.pmaskfld_member = dmmeta::PmaskfldMember_Concat_pmaskfld_field(pmaskfld.field,field.field);
                    vrfy(pmaskfld_member_InsertMaybe(member)
                         ,tempstr()<<"error inserting new pmaskfld member "<<member);
                }
            }ind_end;
        }
        // compute FPmaskfldMember.bit  for each member
        // Check that it fits parent pmaskfld's width
        ind_beg(pmaskfld_c_pmaskfld_member_curs,pmaskfld_member,pmaskfld) {
            int bit = pmaskfld.nextbit;
            int psizeof = amc::Field_Sizeof(*pmaskfld.p_field);
            vrfy(bit < psizeof*8, tempstr() << "amc.val"
                 <<Keyval("field",pmaskfld_member.p_field->field)
                 <<Keyval("pmask",pmaskfld.field)
                 <<Keyval("error","Out of bits. Too many fields"));
            pmaskfld_member.bit = bit;
            pmaskfld.nextbit++;
        }ind_end;
    }ind_end;
}

void amc::tclass_Pmask() {
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_PresentQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        Set(R,"$pmask",name_Get(*pmaskfld_member.p_pmaskfld->p_field));
        Set(R,"$Present",pmaskfld_member.p_pmaskfld->funcname);
        Set(R,"$bit",tempstr()<<pmaskfld_member.bit);
        amc::FFunc& presentq = amc::CreateCurFunc(true,Subst(R,"$PresentQ"));
        Ins(&R, presentq.ret  , "bool",false);
        Ins(&R, presentq.body , "return $pmask_qGetBit($parname, $bit);");
    }ind_end;
}

// Return C++ expression(s) setting the present bit for the field
// in all presence masks of which the field is a member
// If no presence masks are defined, return empty string;
tempstr amc::SetPresentExpr(amc::FField &field, strptr parent) {
    tempstr out;
    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        out << name_Get(*pmaskfld_member.p_pmaskfld->p_field) << "_qSetBit("<<parent<<", "<<pmaskfld_member.bit<<");"<<eol;
    }ind_end;
    return out;
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_SetPresent() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        Set(R,"$pmask",name_Get(*pmaskfld_member.p_pmaskfld->p_field));
        Set(R,"$Present",pmaskfld_member.p_pmaskfld->funcname);
        Set(R,"$bit",tempstr()<<pmaskfld_member.bit);
        amc::FFunc& setpresent = amc::CreateCurFunc(true,Subst(R,"Set$Present"));
        Ins(&R, setpresent.ret  , "void", false);
        Ins(&R, setpresent.body, "$pmask_qSetBit($pararg, $bit); // mark presence in pmask");
    }ind_end;
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_GetBit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        Set(R,"$Present",pmaskfld_member.p_pmaskfld->funcname);
        Set(R,"$bit",tempstr()<<pmaskfld_member.bit);
        amc::FFunc& func = amc::CreateCurFunc(true,Subst(R,"$Present_GetBit"));
        AddRetval(func,"int","retval",Subst(R,"$bit"));
    }ind_end;
}

// Return FPmaskfld which filters printing for ctype CTYPE
// NULL if none
amc::FPmaskfld *amc::GetPrintFilter(amc::FCtype &ctype) {
    amc::FPmaskfld *filter = NULL;
    ind_beg(amc::ctype_c_pmaskfld_curs,pmaskfld,ctype) {
        if (pmaskfld.filter_print) {
            filter=&pmaskfld;
            break;
        }
    }ind_end;
    return filter;
}

// Find PMASKFLD_MEMBER record for field FIELD and pmask PMASKFLD
// NULL if none
amc::FPmaskfldMember *amc::FindMember(amc::FField &field, amc::FPmaskfld *pmaskfld) {
    amc::FPmaskfldMember *ret=NULL;
    ind_beg(amc::field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        if (pmaskfld_member.p_pmaskfld == pmaskfld) {
            ret=&pmaskfld_member;
            break;
        }
    }ind_end;
    return ret;
}
