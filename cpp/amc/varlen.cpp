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
// Exceptions: NO
// Source: cpp/amc/varlen.cpp -- Varlen reftype
//

#include "include/amc.h"

// True if FIELD adds a member of its own to the emitted struct, and so
// occupies bytes of the ctype's fixed portion. A Bitfld re-slices the word of
// its source field and a fldfunc computes its value from other fields, so
// neither declares storage. Neither do the reftypes below: Global and
// Cppstack name an instance that lives outside the struct, Malloc keeps its
// state elsewhere entirely, an Opt draws a comment line where its optional
// trailing element would sit and no member at all, and Count and Exec declare
// what storage they need through generated child fields, which the caller's
// walk visits in their own right. Every other reftype is answered true,
// including the ones that reach the struct by a route other than InsVar --
// Base through inheritance, Pkey and Hook and RegxSql through their own
// emitters -- and including ZSListMT, whose two list heads are members of
// the parent like any other. test/amc/bad_varlen_last.ssim carries a control
// for each case the predicate answers false and a reported row for the cases
// it answers true.
static bool StructMemberQ(amc::FField &field) {
    bool member = field.reftype != dmmeta_Reftype_reftype_Bitfld;
    member = member && field.reftype != dmmeta_Reftype_reftype_Global;
    member = member && field.reftype != dmmeta_Reftype_reftype_Cppstack;
    member = member && field.reftype != dmmeta_Reftype_reftype_Malloc;
    member = member && field.reftype != dmmeta_Reftype_reftype_Opt;
    member = member && field.reftype != dmmeta_Reftype_reftype_Count;
    member = member && field.reftype != dmmeta_Reftype_reftype_Exec;
    member = member && !amc::FldfuncQ(field);
    return member;
}

amc::FField *amc::LengthField(amc::FCtype &ctype) {
    return ctype.c_lenfld ? ctype.c_lenfld->p_field : NULL;
}

tempstr amc::LengthType(amc::FCtype &ctype) {
    tempstr ret;
    if (amc::FField *lenfld = LengthField(ctype)) {
        ret << lenfld->cpp_type;
    } else {
        ret << "u32";
    }
    return ret;
}

tempstr amc::VarlenEndName(amc::FField &field) {
    return tempstr() << name_Get(field) << "_end";
}

tempstr amc::VarlenEndExpr(strptr parname, amc::FField &field) {
    tempstr ret;
    if (amc::FField *lenfld = LengthField(*field.p_ctype)) {
        ret = FieldvalExpr(NULL,*lenfld,parname,VarlenEndName(field));
    } else {
        ret << VarlenEndName(field);
    }
    return ret;
}

tempstr amc::VarlenEndAssign(strptr parname, amc::FField &field, strptr value) {
    tempstr ret;
    if (amc::FField *lenfld = LengthField(*field.p_ctype)) {
        ret = AssignExpr(*lenfld, parname, value, true, VarlenEndName(field));
    } else {
        ret << VarlenEndName(field) << " = " << value;
    }
    return ret;
}

tempstr amc::VarlenEndIncr(strptr parname, amc::FField &field, strptr incr) {
    tempstr value = tempstr() << VarlenEndExpr(parname,field) << " + " << incr;
    return amc::VarlenEndAssign(parname, field, value);
}

// Set up the varlen field: declare its end-offset variable when a later
// varlen field needs one, set the length expressions, ensure the parent
// pool can hold trailing data, and fail the run when a non-varlen field
// follows.  gen_check_varlen rejects an element type with a destructor.
void amc::tclass_Varlen() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (ctype_zd_varlenfld_Next(field)) {
        Set(R, "$lentype", LengthType(*field.p_ctype));
        InsVar(R, field.p_ctype, "$lentype", VarlenEndName(field), "", "end of $name field");
    }
    InsStruct(R, field.p_ctype, "// var-length field $field starts here. access it with $name_Addr");
    Set(R, "$lenexpr", LengthExpr(*field.p_ctype, Subst(R,"$parname")));
    Set(R, "$curslenexpr", LengthExpr(*field.p_ctype, Subst(R,"parent")));
    Set(R, "$rettype",field.p_arg->c_lenfld ? "u8" : "$Cpptype");

    // Per-ctype checks run once, from the ctype's first varlen field --
    // running them per varlen field would report each offender once per
    // varlen field of the ctype.
    if (zd_varlenfld_First(*field.p_ctype) == &field) {
        // Every instance of the ctype must be a varlen-capable pool.
        EnsureVarlenPool(*field.p_ctype);

        // Ensure varlen fields are last: report each fixed field that follows
        // a varlen field, naming the nearest preceding varlen field. Only a
        // field that adds a member of its own can be clobbered by the varlen
        // data, so a zero-storage field (StructMemberQ) is legal after it --
        // an Opt among them, whose own claim on the end of the fixed portion
        // is rejected as amc.trail_overlay instead.
        amc::FField *varlen = NULL;
        ind_beg(amc::ctype_c_field_curs, _field,*field.p_ctype) {
            if (_field.reftype == dmmeta_Reftype_reftype_Varlen) {
                varlen = &_field;
            } else if (varlen && StructMemberQ(_field)) {
                prerr("amc.varlen_last"
                      <<Keyval("field",_field.field)
                      <<Keyval("varlen",varlen->field)
                      <<Keyval("comment","fixed field follows a varlen field; varlen fields must be last"));
                algo_lib::_db.exit_code++;
            }
        }ind_end;
    }
}

void amc::tfunc_Varlen_Addr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& addr = amc::CreateCurFunc();
    Ins(&R, addr.ret  , "$rettype*", false);
    Ins(&R, addr.proto, "$name_Addr($Parent)", false);
    if (ctype_zd_varlenfld_Prev(field)) {
        Set(R, "$prevend", VarlenEndExpr(Subst(R,"$parname"),*ctype_zd_varlenfld_Prev(field)));
        Ins(&R, addr.body, "return ($rettype*)((u8*)&$parname + sizeof($Partype) + $prevend); // address of varlen portion");
    } else {
        Ins(&R, addr.body, "return ($rettype*)((u8*)&$parname + sizeof($Partype)); // address of varlen portion");
    }
}

void amc::tfunc_Varlen_Getary() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& get = amc::CreateCurFunc(true);
    AddRetval(get, Subst(R,"algo::aryptr<$rettype>"), "", "");
    Ins(&R, get.body, "return algo::aryptr<$rettype>($name_Addr($pararg), $name_N($pararg));");
}

void amc::tfunc_Varlen_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    // N: number of elements in varlen portion
    amc::FFunc& get_n = amc::CreateCurFunc();
    Ins(&R, get_n.ret  , "u32",false);
    Ins(&R, get_n.proto, "$name_N($Cparent)",false);
    if (ctype_zd_varlenfld_Prev(field)) {
        Set(R, "$prevend", VarlenEndExpr(Subst(R,"$parname"),*ctype_zd_varlenfld_Prev(field)));
        Set(R, "$minusprevend", " - $prevend");
    } else {
        Set(R, "$minusprevend", "");
    }
    if (ctype_zd_varlenfld_Next(field)) {
        Set(R, "$endexpr", VarlenEndExpr(Subst(R,"$parname"),field));
        Ins(&R, get_n.body, "return u32(($endexpr$minusprevend) / sizeof($rettype));");
    } else {
        if (field.p_ctype->c_lenfld) {
            Set(R, "$lenexpr", LengthExpr(*field.p_ctype, "(($Partype&)$parname)"));
            Ins(&R, get_n.body, "u32 length = $lenexpr;");
            Ins(&R, get_n.body, "u32 extra_bytes = u32_Max(length,sizeof($Partype)) - sizeof($Partype)$minusprevend; // avoid unsigned subtraction underflow");
            Ins(&R, get_n.body, "return u32(extra_bytes / sizeof($rettype));");
        } else {
            get_n.acrkey << "field:"<<field.field;
            get_n.extrn=true;
        }
    }
}

// Generate $name_ReadStrptrMaybe, appending one element parsed from the
// string to the varlen buffer under construction: char/u8 bytes are copied
// raw, a typefld element parses through the message dispatcher, and any
// other element parses in place -- a nested varlen element collects its
// own tail in a fresh buffer, stores its length through its own lenfld,
// and splices in at the field's end offset
void amc::tfunc_Varlen_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr in_str)", false);
        Ins(&R, rd.body, "bool retval = true;");
        Ins(&R, rd.body, "if (algo_lib::_db.varlenbuf) {");
        if (ctype_zd_varlenfld_Next(field)) {
            Set(R, "$endexpr", VarlenEndExpr(Subst(R,"$parname"),field));
        }
        if ((field.arg == "char" || field.arg == "u8")) {
            if (ctype_zd_varlenfld_Next(field)) {
                Ins(&R, rd.body , "    ary_Insary(*algo_lib::_db.varlenbuf,strptr_ToMemptr(in_str),$endexpr);");
                Ins(&R, rd.body , "    u32 incr = ch_N(in_str);");
            } else {
                Ins(&R, rd.body , "    ary_Addary(*algo_lib::_db.varlenbuf, strptr_ToMemptr(in_str));");
            }
        } else if (field.p_arg->c_typefld) {
            Set(R, "$Fldhdrtype", field.p_arg->c_typefld->p_ctype->cpp_type);
            Ins(&R, rd.body, "    algo::ByteAry temp;");
            Ins(&R, rd.body, "    retval = $FldhdrtypeMsgs_ReadStrptrMaybe(in_str, temp); // read any of several message types here");
            if (ctype_zd_varlenfld_Next(field)) {
                Ins(&R, rd.body , "    ary_Insary(*algo_lib::_db.varlenbuf,ary_Getary(temp),$endexpr);");
                Ins(&R, rd.body , "    u32 incr = ary_N(temp);");
            } else {
                Ins(&R, rd.body, "    ary_Addary(*algo_lib::_db.varlenbuf, ary_Getary(temp)); // return it");
            }
        } else {
            Set(R, "$Fldcpptype", field.cpp_type);
            if (ctype_zd_varlenfld_Next(field)) {
                Ins(&R, rd.body , "    $Fldcpptype *$name_tmp = new(ary_AllocNAt(*algo_lib::_db.varlenbuf, sizeof($Fldcpptype), $endexpr).elems) $Fldcpptype;");
                Ins(&R, rd.body , "    u32 incr = sizeof($Fldcpptype);");
            } else {
                Ins(&R, rd.body , "    $Fldcpptype *$name_tmp = new(ary_AllocN(*algo_lib::_db.varlenbuf, sizeof($Fldcpptype)).elems) $Fldcpptype;");
            }
            if (amc::RuntimeFrameLenQ(*field.p_arg)) {
                Ins(&R, rd.body , "    algo::ByteAry varlenbuf;");
                Ins(&R, rd.body , "    algo::ByteAry *varlenbuf_save = algo_lib::_db.varlenbuf;");
                Ins(&R, rd.body , "    algo_lib::_db.varlenbuf = &varlenbuf;");
            }
            Ins(&R, rd.body , "    retval = $Cpptype_ReadStrptrMaybe(*$name_tmp, in_str);");
            if (amc::RuntimeFrameLenQ(*field.p_arg)) {
                Ins(&R, rd.body , "    algo_lib::_db.varlenbuf = varlenbuf_save;");
                if (field.p_arg->c_lenfld) {
                    // the length is stored through the element's own lenfld,
                    // not the containing ctype's
                    amc::FLenfld &lenfld = *field.p_arg->c_lenfld;
                    if (amc::LenfldGuardNeededQ(lenfld)) {
                        Set(R,"$lenchk",Subst(R,LenfldCheckExpr(lenfld,"sizeof($Fldcpptype)+ary_N(varlenbuf)")));
                        Ins(&R, rd.body , "    retval = retval && ($lenchk); // only a total the element's length field can store round-trips");
                    }
                    Set(R,"$lenincr",amc::AssignExpr(*lenfld.p_field,Subst(R,"(*$name_tmp)"),Subst(R,LenfldStoreExpr(lenfld,"sizeof($Fldcpptype)+ary_N(varlenbuf)")),true));
                    Ins(&R, rd.body , "    $lenincr;");
                }
                if (ctype_zd_varlenfld_Next(field)) {
                    Ins(&R, rd.body , "    ary_Insary(*algo_lib::_db.varlenbuf,varlenbuf,$endexpr+sizeof($Fldcpptype));");
                    Ins(&R, rd.body , "    incr += ary_N(varlenbuf);");
                } else {
                    Ins(&R, rd.body , "    ary_Addary(*algo_lib::_db.varlenbuf,varlenbuf);");
                }
            }
        }
        if (ctype_zd_varlenfld_Next(field)) {
            for (FField *ptr = &field; ctype_zd_varlenfld_Next(*ptr); ptr = ctype_zd_varlenfld_Next(*ptr)) {
                Set(R, "$endincr", VarlenEndIncr(Subst(R,"$parname"),*ptr,"incr"));
                Ins(&R, rd.body , "    $endincr;");
            }
        }
        Ins(&R, rd.body, "}");
        SetPresent(rd,Subst(R,"$parname"),field);
        MaybeUnused(rd,Subst(R,"$parname"));
        MaybeUnused(rd,"in_str");
        Ins(&R, rd.body, "return retval;");
    }
}

void amc::tfunc_Varlen_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    if (field.p_ctype->c_lenfld) {

        Ins(&R, ns.curstext, "");
        Ins(&R, ns.curstext, "struct $Parname_$name_curs {// cursor");
        Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext, "    u8 *ptr;");
        Ins(&R, ns.curstext, "    int length;");
        Ins(&R, ns.curstext, "    int index;"); // index for printing
        Ins(&R, ns.curstext, "    $Parname_$name_curs() { ptr=NULL; length=0; index=0; }");
        Ins(&R, ns.curstext, "};");
        Ins(&R, ns.curstext, "");

        {
            amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
            Ins(&R, curs_reset.ret  , "void", false);
            Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
            Ins(&R, curs_reset.body, "curs.ptr = (u8*)$name_Addr(parent);");
            if (ctype_zd_varlenfld_Prev(field)) {
                Set(R, "$prevend", VarlenEndExpr(Subst(R,"$parname"),*ctype_zd_varlenfld_Prev(field)));
                Set(R, "$minusprevend", "- $prevend");
            } else {
                Set(R, "$minusprevend", "");
            }
            if (ctype_zd_varlenfld_Next(field)) {
                Set(R, "$endexpr", VarlenEndExpr(Subst(R,"$parname"),field));
                Ins(&R, curs_reset.body, "curs.length = $endexpr$minusprevend;");
            } else {
                Ins(&R, curs_reset.body, "curs.length = $curslenexpr $minusprevend - sizeof($Partype);");
            }
            Ins(&R, curs_reset.body, "curs.index = 0;");
        }

        {
            amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
            Ins(&R, curs_validq.comment, "cursor points to valid item");
            Ins(&R, curs_validq.ret  , "bool", false);
            Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
            Ins(&R, curs_validq.body, "bool valid = ssizeof($Cpptype) <= curs.length;");
            if (field.p_arg->c_lenfld) {
                Set(R, "$childlenexpr", LengthExpr(*field.p_arg, "(*($Cpptype*)curs.ptr)"));
                // check that len is >= ssizeof($Cppexpr) && <= curs.length (remaining length)
                // using unsigned comparison trick
                Ins(&R, curs_validq.body, "valid = valid && unsigned($childlenexpr-ssizeof($Cpptype)) <= curs.length-ssizeof($Cpptype);");
            }
            Ins(&R, curs_validq.body, "return valid;");
        }

        {
            amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
            Ins(&R, curs_next.comment, "proceed to next item");
            Ins(&R, curs_next.ret  , "void", false);
            Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
            Set(R, "$childlenexpr", LengthExpr(*field.p_arg, "(*($Cpptype*)curs.ptr)"));
            Ins(&R, curs_next.body, "i32 len = $childlenexpr;");
            Ins(&R, curs_next.body, "curs.ptr += len;");
            Ins(&R, curs_next.body, "curs.length -= len;");
            Ins(&R, curs_next.body, "++curs.index;");
        }

        {
            amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
            Ins(&R, curs_access.comment, "item access");
            Ins(&R, curs_access.ret  , "$Cpptype&", false);
            Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
            Ins(&R, curs_access.body, "return *($Cpptype*)curs.ptr;");
        }
    }
}

void amc::tfunc_Varlen_Print() {
    tfunc_Inlary_Print();
}

void amc::tfunc_Varlen_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    if (ctype_zd_varlenfld_Next(field)) {
        Set(R, "$endassign", VarlenEndAssign(Subst(R,"$parname"),field,"0"));
        Ins(&R, init.body, "$endassign; // $name: initialize");
    }
}
