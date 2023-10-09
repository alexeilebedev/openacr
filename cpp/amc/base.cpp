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
// Source: cpp/amc/base.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool StripCommentQ(amc::FField &field) {
    return field.c_fbase && field.c_fbase->stripcomment;
}

// -----------------------------------------------------------------------------

static bool AllowCastbaseQ(amc::FField &field) {
    amc::FCtype &ctype = *field.p_ctype;
    amc::FCtype *base = GetBaseType(ctype,NULL);
    bool retval =
        ctype.n_xref==0
        && zd_inst_EmptyQ(ctype)
        && !StripCommentQ(field)
        && !(base && (base->c_optfld || base->c_varlenfld));
    return retval;
}

// -----------------------------------------------------------------------------

void amc::tclass_Base() {
}

// -----------------------------------------------------------------------------

// Generate a function to copy fields from a ctype to its base type
void amc::tfunc_Base_CopyOut() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &fldbase = *amc::_db.genfield.p_field;
    amc::FFunc& copyout = amc::CreateCurFunc();
    Ins(&R, copyout.ret  , "void", false);
    Ins(&R, copyout.proto, "$parname_CopyOut($Partype &row, $Cpptype &out)", false);
    ind_beg(amc::ctype_c_field_curs, field, *fldbase.p_arg) {
        Set(R, "$cppname", name_Get(field));
        Set(R, "$reftype", field.reftype);
        if (!FindFieldByName(*fldbase.p_ctype, name_Get(field))) {
            Ins(&R, copyout.body, "// $cppname: field stripped (see dmmeta.fbase:$field)");
        } else if (field.c_fregx) {
            Set(R, "$Regxtype", field.c_fregx->regxtype);
            Ins(&R, copyout.body, "(void)Regx_Read$Regxtype(out.$cppname, row.$cppname.expr, true);");
        } else if (FldfuncQ(field)) {// cppfunc, substr, alias
            //Ins(&R, copyout.body, "// $cppname: fldfunc, skipped");
        } else if (ComputedFieldQ(field)) {// lenfld, typefld
            Ins(&R, copyout.body, "// $cppname: field value is computed");
        } else if (field.c_tary) {// tary
            Ins(&R, copyout.body, "$cppname_Setary(out, $cppname_Getary(row));");
        } else if (ValQ(field)) {
            Ins(&R, copyout.body, "out.$cppname = row.$cppname;");
        } else {
            Ins(&R, copyout.body, "// $cppname: unknown field type ($reftype), skipped");
        }
    }ind_end;
    MaybeUnused(copyout,"row");
    MaybeUnused(copyout,"out");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Base_CopyIn() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &fldbase = *amc::_db.genfield.p_field;
    if (amc::CanCopyQ(*fldbase.p_ctype) && !zd_inst_EmptyQ(*fldbase.p_ctype)) {
        amc::FFunc& copyin = amc::CreateCurFunc();
        Ins(&R, copyin.ret  , "void", false);
        Ins(&R, copyin.proto, "$parname_CopyIn($Partype &row, $Cpptype &in)", false);
        ind_beg(amc::ctype_c_field_curs, field, *fldbase.p_arg) {
            Set(R, "$reftype", field.reftype);
            Set(R, "$cppname", name_Get(field));
            if (!FindFieldByName(*fldbase.p_ctype, name_Get(field))) {
                Ins(&R, copyin.body, "// $cppname: field stripped (see dmmeta.fbase:$field)");
            } else if (field.c_fregx) {
                Set(R, "$Regxtype", field.c_fregx->regxtype);
                Ins(&R, copyin.body, "(void)Regx_Read$Regxtype(row.$cppname, in.$cppname.expr, true);");
            } else if (FldfuncQ(field)) {// cppfunc, substr
                //Ins(&R, copyin.body, "// $cppname: fldfunc, skipped");
            } else if (ComputedFieldQ(field)) {// lenfld, typefld
                Ins(&R, copyin.body, "// $cppname: field value is computed");
            } else if (field.c_tary) {// tary
                Ins(&R, copyin.body, "$cppname_Setary(row, $cppname_Getary(in));");
            } else if (ValQ(field)) {
                Ins(&R, copyin.body, "row.$cppname = in.$cppname;");
            } else {
                Ins(&R, copyin.body, "// $cppname: unknown field reftype ($reftype), skipped");
            }
        }ind_end;
        MaybeUnused(copyin,"row");
        MaybeUnused(copyin,"in");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Base_Castbase() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (AllowCastbaseQ(field)) {
        amc::FFunc& castbase = amc::CreateCurFunc();
        Ins(&R, castbase.ret  , "$Cpptype&", false);
        Ins(&R, castbase.proto, "Castbase($Parent)", false);
        Ins(&R, castbase.body, "return reinterpret_cast<$Cpptype&>($pararg);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Base_Castdown() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCtype &ctype = *field.p_ctype;
    if (AllowCastbaseQ(field) && ctype.c_msgtype && ctype.c_typefld) {
        Set(R, "$Msgtype", ctype.c_msgtype->type.value);
        Set(R, "$Hdrtype", UltimateBaseType(&ctype,&ctype)->cpp_type);
        Set(R, "$typefld", FieldvalExpr(&ctype, *ctype.c_typefld->p_field, "hdr"));
        amc::FFunc& castdown = amc::CreateCurFunc();
        Ins(&R, castdown.comment, "Check if $Cpptype is an instance of $Parname by checking the type field");
        Ins(&R, castdown.comment, "If it is, return the pointer of target type.");
        if (ctype.c_lenfld) {
            Ins(&R, castdown.comment, "Additionally, check if the length field permits valid instance of $Parname.");
        }
        Ins(&R, castdown.comment , "If not successful, quietly return NULL.");
        Ins(&R, castdown.ret  , "$Partype*", false);
        Ins(&R, castdown.proto, "$Parname_Castdown($Hdrtype &hdr)", false);
        Ins(&R, castdown.body    , "bool cond = $typefld == ($Msgtype);");
        if (ctype.c_lenfld) {
            Set(R, "$lenexpr", amc::LengthExpr(*field.p_arg, "hdr"));
            Ins(&R, castdown.body, "cond &= $lenexpr >= ssizeof($Partype);");
        }
        Ins(&R, castdown.body    , "return cond ? reinterpret_cast<$Partype*>(&hdr) : NULL;");
    }
}

// -----------------------------------------------------------------------------

double amc::ChildRowid(double rowid) {
    return rowid + 0.001;
}

// -----------------------------------------------------------------------------

static bool ClonableQ(amc::FField &field, bool stripcomment) {
    return
        field.reftype != dmmeta_Reftype_reftype_Base
        && (!stripcomment || name_Get(field) != "comment");
}

// -----------------------------------------------------------------------------

void amc::CloneFields(amc::FCtype &from, amc::FCtype &to, double next_rowid, amc::FField &basefield) {
    bool stripcomment = StripCommentQ(basefield);
    ind_beg(amc::ctype_c_field_curs, field,from) if (ClonableQ(field,stripcomment)) {
        dmmeta::Field newfield;
        amc::field_CopyOut(field,newfield);

        // change name of this field. everything else stays.
        newfield.field = tempstr() << to.ctype << "." << name_Get(field);
        int count=2;
        while (amc::ind_field_Find(newfield.field)) {
            prerr("amc.newfield_cbase"
                  <<Keyval("base_field",field.field)
                  <<Keyval("child_field",newfield.field)
                  <<Keyval("comment","Field name collision with base; rename child field"));
            newfield.field = tempstr() << to.ctype << "." << name_Get(field) << count;
            algo_lib::_db.exit_code++;
            count++;
        }

        amc::FField& fnewfield = *amc::InsField(newfield);
        fnewfield.rowid = next_rowid;
        next_rowid = ChildRowid(next_rowid);
        c_field_QuickSort(to);

        if (field.c_bitfld) {
            dmmeta::Bitfld newbitfld;
            newbitfld.field = newfield.field;
            newbitfld.offset = field.c_bitfld->offset;
            newbitfld.width = field.c_bitfld->width;
            newbitfld.srcfield = tempstr() << to.ctype << "." << dmmeta::Field_name_Get(field.c_bitfld->srcfield);
            vrfy(amc::bitfld_InsertMaybe(newbitfld),"amc::bitfld_InsertMaybe");
        }
        if (amc::FLenfld *lenfld = GetLenfld(field)) {
            dmmeta::Lenfld newlenfld;
            amc::lenfld_CopyOut(*lenfld, newlenfld);
            newlenfld.field = newfield.field;
            amc::lenfld_InsertMaybe(newlenfld);
        }

        if (field.c_typefld) {
            dmmeta::Typefld newtypefld;
            amc::typefld_CopyOut(*field.c_typefld, newtypefld);
            newtypefld.field = newfield.field;
            amc::typefld_InsertMaybe(newtypefld);
        }

        if (field.c_rowid) {
            dmmeta::Rowid newrowid;
            amc::rowid_CopyOut(*field.c_rowid, newrowid);
            newrowid.field = newfield.field;
            amc::rowid_InsertMaybe(newrowid);
        }

        if (field.c_inlary) {
            dmmeta::Inlary newinlary;
            amc::inlary_CopyOut(*field.c_inlary, newinlary);
            newinlary.field = newfield.field;
            amc::inlary_InsertMaybe(newinlary);
        }

        if (field.c_fcmp) {
            dmmeta::Fcmp newfcmp;
            amc::fcmp_CopyOut(*field.c_fcmp, newfcmp);
            newfcmp.field = newfield.field;
            amc::fcmp_InsertMaybe(newfcmp);
        }
        if (field.c_substr) {
            dmmeta::Substr newsubstr;
            amc::substr_CopyOut(*field.c_substr, newsubstr);
            newsubstr.field = newfield.field;
            amc::substr_InsertMaybe(newsubstr);
        }
        if (field.c_cppfunc) {
            dmmeta::Cppfunc newcppfunc;
            amc::cppfunc_CopyOut(*field.c_cppfunc, newcppfunc);
            newcppfunc.field = newfield.field;
            amc::cppfunc_InsertMaybe(newcppfunc);
        }
        if (field.c_falias) {
            dmmeta::Falias newfalias;
            amc::falias_CopyOut(*field.c_falias, newfalias);
            newfalias.field = newfield.field;
            newfalias.basefield = dmmeta::Field_Concat_ctype_name(ctype_Get(newfield),name_Get(*field.c_falias->p_basefield));
            amc::falias_InsertMaybe(newfalias);
        }
        if (field.c_fbigend) {
            dmmeta::Fbigend newfbigend;
            amc::fbigend_CopyOut(*field.c_fbigend, newfbigend);
            newfbigend.field = newfield.field;
            amc::fbigend_InsertMaybe(newfbigend);
        }
        if (field.c_fregx) {
            dmmeta::Fregx newfregx;
            newfregx.partial=field.c_fregx->partial;
            newfregx.regxtype=field.c_fregx->regxtype;
            newfregx.field = newfield.field;
            amc::fregx_InsertMaybe(newfregx);
        }
        if (field.c_pmaskfld) {
            dmmeta::Pmaskfld newpmaskfld;
            amc::pmaskfld_CopyOut(*field.c_pmaskfld, newpmaskfld);
            newpmaskfld.field = newfield.field;
            amc::pmaskfld_InsertMaybe(newpmaskfld);
        }
        // throw away c_fldfoffset
        vrfy(!fnewfield.c_xref, "cannot copy field with xref");
        vrfy(!fnewfield.c_basepool, "cannot copy field with basepool");
        vrfy(!fnewfield.c_llist, "cannot copy field with llist");
        vrfy(!fnewfield.c_ptrary, "cannot copy field with gsort");
        vrfy(!fnewfield.c_sortfld, "cannot copy field with sortfld");
        vrfy(!fnewfield.c_thash, "cannot copy field with thash");
        vrfy(!fnewfield.c_tary, "cannot copy field with tary");
        vrfy(!fnewfield.c_fbuf, "cannot copy field with fbuf");
    }ind_end;
}
