// Copyright (C) 2023-2026 AlgoRND
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
// Exceptions: yes
// Source: cpp/amc/pmask.cpp -- Presence masks
//

#include "include/amc.h"

// True if field is suitable for inclusion in a pmask
static bool InPmaskQ(amc::FField &field) {
    bool need = field.reftype != dmmeta_Reftype_reftype_Base;
    need &= field.reftype != dmmeta_Reftype_reftype_Global;// the global instance is an anchor, not data
    need &= !field.c_pmaskfld;// not the pmaskfld itself
    need &= !GetLenfld(field);// no pmask bit for length fields
    need &= !field.c_typefld;// no pmask bit for type fields
    return need;
}

// Prepare each pmask for generation: derive the accessor name, create the
// backing bitset, populate the member set (every suitable field, unless an
// explicit member list was provided), validate the members, and assign
// each member its bit
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
        // explicit and auto-created members converge here; check every row
        // against the same membership rule the auto-populate loop selects
        // by, so an explicit row cannot smuggle in a field the rule
        // excludes -- a bit for a field of another ctype, for the Global
        // anchor, or for the pmask itself would consume a bit position and
        // generate accessors the pmask's parent struct cannot back
        ind_beg(pmaskfld_c_pmaskfld_member_curs,pmaskfld_member,pmaskfld) {
            if (pmaskfld_member.p_field->p_ctype != pmaskfld.p_field->p_ctype) {
                prerr("amc.pmask_member"
                      <<Keyval("pmaskfld_member",pmaskfld_member.pmaskfld_member)
                      <<Keyval("member_ctype",pmaskfld_member.p_field->p_ctype->ctype)
                      <<Keyval("pmask_ctype",pmaskfld.p_field->p_ctype->ctype)
                      <<Keyval("comment","member field belongs to a different ctype than the pmask field"));
                algo_lib::_db.exit_code++;
            } else if (!InPmaskQ(*pmaskfld_member.p_field)) {
                prerr("amc.pmask_member"
                      <<Keyval("pmaskfld_member",pmaskfld_member.pmaskfld_member)
                      <<Keyval("reftype",pmaskfld_member.p_field->reftype)
                      <<Keyval("comment","this field cannot carry a presence bit (Base, Global, the pmask itself, a length field, and a type field are excluded)"));
                algo_lib::_db.exit_code++;
            }
        }ind_end;
        // compute FPmaskfldMember.bit  for each member
        // (the check that all bits fit the pmask field's width runs in
        // gen_check_pmask, after struct sizes are computed)
        ind_beg(pmaskfld_c_pmaskfld_member_curs,pmaskfld_member,pmaskfld) {
            pmaskfld_member.bit = pmaskfld.nextbit;
            pmaskfld.nextbit++;
        }ind_end;
    }ind_end;
}

// Check that every pmask has no more member fields than its pmask field has bits.
// Runs after gen_compute_size: a pmask field of non-builtin type has no csize,
// so its byte size is only known once struct sizes are computed.
// A pmask field whose size is unknown even then (nbit 0) is reported by
// tclass_Bitset, not here.
void amc::gen_check_pmask() {
    ind_beg(amc::_db_pmaskfld_curs, pmaskfld, amc::_db) {
        i64 nbit = amc::Field_Sizeof(*pmaskfld.p_field)*8;
        if (nbit > 0 && pmaskfld.nextbit > nbit) {
            prerr("amc.pmask_width"
                  <<Keyval("pmask",pmaskfld.field)
                  <<Keyval("nmember",pmaskfld.nextbit)
                  <<Keyval("nbit",nbit)
                  <<Keyval("comment","Out of bits: pmask has more member fields than the pmask field has bits"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

void amc::tclass_Pmask() {
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_PresentQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        Set(R,"$pmask",name_Get(*pmaskfld_member.p_pmaskfld->p_field));
        Set(R,"$Present",pmaskfld_member.p_pmaskfld->funcname);
        Set(R,"$bit",tempstr()<<pmaskfld_member.bit);
        amc::FFunc& presentq = amc::CreateCurFunc(true,Subst(R,"$PresentQ"));
        Ins(&R, presentq.ret  , "bool",false);
        Ins(&R, presentq.body , "return $pmask_qGetBit($pararg, $bit);");
    }ind_end;
}

// Emit C++ that sets FIELD's bit on the pmaskfld carrying MEMBER and,
// when FIELD is a oneof variant, clears the bits of its sibling
// variants within the same oneof on that same pmaskfld.
tempstr amc::SetPresentForMember(amc::FField &field, amc::FPmaskfldMember &member, strptr parent) {
    tempstr out;
    amc::FPmaskfld &pmaskfld = *member.p_pmaskfld;
    tempstr pmask_name(name_Get(*pmaskfld.p_field));
    tempstr parent_arg(ParentArgExpr(*field.p_ctype,parent,true));
    out << pmask_name << "_qSetBit(" << parent_arg << member.bit << "); // mark presence in pmask" << eol;
    if (field.c_fpbufcase) {
        tempstr my_oneof(oneof_Get(*field.c_fpbufcase));
        ind_beg(amc::ctype_c_fpbufcase_curs, sib_case, *field.p_ctype) {
            bool clear_bit = (field.c_fpbufcase != &sib_case) && (oneof_Get(*field.c_fpbufcase) == oneof_Get(sib_case));
            amc::FPmaskfldMember *sib_member = clear_bit ? FindMember(*sib_case.p_field, &pmaskfld) : NULL;
            if (sib_member) {
                out << pmask_name << "_qClearBit(" << parent_arg << sib_member->bit << ");" << eol;
            }
        }ind_end;
    }
    return out;
}

// Return C++ expression testing the present bit of FIELD, via the
// generated <field>_<Funcname>Q accessor of each pmask of which the field
// is a member, AND-joined over the memberships; PARENT names the parent
// record (collapsed by ParentArgExpr when the parent is a global).
// Returns empty string if the field has no pmask membership.
tempstr amc::PresentQExpr(amc::FField &field, strptr parent) {
    tempstr out;
    tempstr arg(ParentArgExpr(*field.p_ctype,parent,false));
    algo::ListSep ls(" && ");
    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        out << ls << name_Get(field) << "_" << pmaskfld_member.p_pmaskfld->funcname << "Q(" << arg << ")";
    }ind_end;
    return out;
}

// Return C++ expression(s) setting the present bit for the field
// in all presence masks of which the field is a member.
// If the field is a oneof variant, sibling variants are also cleared.
// Returns empty string if the field has no pmask membership.
tempstr amc::SetPresentExpr(amc::FField &field, strptr parent) {
    tempstr out;
    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        out << SetPresentForMember(field, pmaskfld_member, parent);
    }ind_end;
    return out;
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_SetPresent() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    ind_beg(field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        Set(R,"$Present",pmaskfld_member.p_pmaskfld->funcname);
        amc::FFunc& setpresent = amc::CreateCurFunc(true,Subst(R,"Set$Present"));
        Ins(&R, setpresent.ret, "void", false);
        setpresent.body << SetPresentForMember(field, pmaskfld_member, Subst(R,"$pararg"));
    }ind_end;
}

// Create multiple functions, one for each pmask of which this field is a member
void amc::tfunc_Pmask_GetBit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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
