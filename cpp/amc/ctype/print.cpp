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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/ctype/print.cpp -- Print ctype to string
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static tempstr EqExpr(amc::FCtype &ctype, strptr lhs, strptr rhs) {
    tempstr ret;
    // strings have an EqStrptr function which I'm not sure how to access here
    // they should be compared using ==
    // builtin types should be compared with ==
    // other types that have Eq can be compared with _Eq
    if (ctype.c_cstr || ctype.c_bltin || !ctype.c_ccmp || !ctype.c_ccmp->genop) {
        ret << lhs << " == "<<rhs;
    } else {
        ret << name_Get(ctype)<<"_Eq("<<lhs<<", "<<rhs<<")";
    }
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr Tempexpr(amc::FCtype &ctype, strptr expr) {
    return tempstr()<<ctype.cpp_type<<"("<<expr<<")";
}

// -----------------------------------------------------------------------------

tempstr amc::CheckDfltExpr(amc::FField &field, strptr text, bool canskip) {
    tempstr out;
    tempstr dflt(amc::DfltExprVal(field));
    tempstr checkexpr;
    bool has_compare = field.p_arg->c_ccmp;
    bool cheap_copy = field.p_arg->c_cpptype && field.p_arg->c_cpptype->cheap_copy;

    if (canskip) {
        if (field.reftype==dmmeta_Reftype_reftype_RegxSql && ch_N(dflt) > 0) {
            checkexpr << FieldvalExpr(field.p_ctype,field,"row")<<".expr == "<<dflt;// known to be a cstring
        } else if (field.reftype==dmmeta_Reftype_reftype_Val && ch_N(dflt) > 0) {
            checkexpr << EqExpr(*field.p_arg, FieldvalExpr(field.p_ctype,field,"row"), dflt);// compare with default expression
        } else if (field.reftype==dmmeta_Reftype_reftype_Val && cheap_copy && has_compare) {
            checkexpr << EqExpr(*field.p_arg, FieldvalExpr(field.p_ctype,field,"row"), Tempexpr(*field.p_arg,""));
        }
    }
    if (ch_N(checkexpr)) {
        out << "if (!("<<checkexpr<<")) {"<<eol;
        out << text;
        out << "}"<<eol;
    } else {
        out<<text;
    }
    return out;
}

// -----------------------------------------------------------------------------

static bool GoodForPrintingQ(amc::FField& field, amc::FCfmt &cfmt) {
    bool good = false;
    bool israw = cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw;
    bool source = !bh_bitfld_EmptyQ(field);
    good = good || field.reftype == dmmeta_Reftype_reftype_Varlen;
    good = good || field.reftype == dmmeta_Reftype_reftype_Val;
    good = good || field.reftype == dmmeta_Reftype_reftype_Inlary;
    good = good || field.reftype == dmmeta_Reftype_reftype_Smallstr;
    good = good || field.reftype == dmmeta_Reftype_reftype_Ptr;
    good = good || field.reftype == dmmeta_Reftype_reftype_Opt;
    good = good || field.reftype == dmmeta_Reftype_reftype_Bitfld;
    good = good || field.reftype == dmmeta_Reftype_reftype_RegxSql;
    good = good && (!field.c_substr);// do not read these
    good = good && (!field.c_cppfunc);// do not read these
    good = good && (!field.c_bitfld || !israw);// OK to print bitflds in tuple or sep
    good = good && (!source || israw);// do not print srcfield in tuple form
    good = good && (!field.p_ctype->c_pmaskfld || &field != field.p_ctype->c_pmaskfld->p_field);// do not print presence mask
    good = good && !GetLenfld(field);// skip length field (not applicable to ssim encoding)
    good = good && (!(field.c_typefld)); // skip type field (not applicable)
    return good;
}

// -----------------------------------------------------------------------------

// Generate code for printing an array to string.
// We insert code directly into print.body, since we output multiple attributes
// and not a single value.
static void GenPrintVarlen(algo_lib::Replscope &R, amc::FField &field, amc::FFunc &print) {
    amc::FCtype& valtype = *(field).p_arg;
    if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Getary"))) {// Must have Getary to proceed.
        Set(R, "$Ftype", valtype.cpp_type);
        Set(R, "$field", field.field);
        Ins(&R, print.body, "");// !! add code directly to print
        Ins(&R, print.body, "algo::aryptr<$Ftype> $name_ary = $name_Getary($pararg);");// note row vs. parent because of inlining
        if (field.arg == "char") {
            Ins(&R, print.body, "PrintAttrSpace(str, \"$name\", $name_ary); // print field $field");
        } else if (field.arg == "char" || field.arg == "u8") {
            Ins(&R, print.body, "PrintAttrSpace(str, \"$name\", algo::strptr((char*)$name_ary.elems, $name_ary.n_elems)); // print field $field");
        } else {
            Ins(&R, print.body, "for (int i = 0; i < $name_ary.n_elems; i++) { // print field $field as repeating group");
            print.body << "    "<<name_Get(*field.p_arg)<<"_Print("<<name_Get(field)<<"_ary.elems[i], "<<Subst(R,"$outstr")<<");\n";
            Ins(&R, print.body, "    tempstr name;");
            Ins(&R, print.body, "    name << \"$name.\";");
            Ins(&R, print.body, "    name << i;");
            Ins(&R, print.body, "    PrintAttrSpaceReset(str, name, $outstr);");
            Ins(&R, print.body, "};");
        }
    }
}

// -----------------------------------------------------------------------------

// Some tuples contain an id gconst field which is based
// on the pkey of the tuple.
// Printing this field as symbolic makes the resulting string unusable, as we
// need the numeric id in this specific case
static bool GenPrintId(algo_lib::Replscope &R, amc::FField &field,  cstring &text) {
    bool ret = false;
    amc::FCtype *arg = field.p_arg;
    amc::FField *pkey = c_field_Find(*field.p_ctype,0);
    amc::FField *idfield = c_field_Find(*arg,0);
    if (c_field_N(*arg)==1 && idfield->c_gconst && &field != pkey && idfield->c_gconst->namefld == pkey->field) {
        Set(R, "$Ftype", idfield->cpp_type);
        Set(R, "$accessid", FieldvalExpr(field.p_ctype, *idfield, "$pararg"));
        Ins(&R, text, tempstr()<<"// printing funique, gconst id for pkey "<<pkey->field);
        Ins(&R, text, "$Ftype_Print($accessid, $outstr);");
        ret=true;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Generate code for printing an individual field
// Output goes into TEXT.
// TODO: use
// if (amc::FTfunc* tfunc = amc::ind_tfunc_Find(amcdb::Tfunc_Concat_tclass_name(field.reftype,"Print")))...
static void GenPrintField(algo_lib::Replscope &R, amc::FField &field,  amc::FFunc &print, cstring &text) {
    amc::FCtype& valtype = *(field).p_arg;
    amc::FCtype &ctype = *field.p_ctype;
    if (amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
        (void)func;
        if (func->ismacro) {// inline the function!
            Ins(&R, text, func->body, false);
        } else {
            Set(R, "$fns", ns_Get(ctype));
            Ins(&R, text, "$fns::$name_Print($pararg, $outstr);");// const cast
        }
    } else if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
        // inserts directly into "print"
        GenPrintVarlen(R,field,print);
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
    } else if (HasStringPrintQ(valtype)) {
        if (GenPrintId(R,field,text)) {
            // handled
        } else {
            Set(R, "$Ftype", valtype.cpp_type);
            Ins(&R, text, tempstr()<<"$Ftype_Print("<<FieldvalExpr(&ctype, field, "row")<<", $outstr);");
        }
    }
}

// -----------------------------------------------------------------------------

static void GenPrintTuple(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    Set(R, "$outstr", "temp");
    Set(R, "$rel", ctype.c_ssimfile ? algo::strptr(ctype.c_ssimfile->ssimfile) : algo::strptr(ctype.ctype));
    Ins(&R, print.body, "algo::tempstr temp;");
    Ins(&R, print.body, "str << \"$rel\";");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (GoodForPrintingQ(field,cfmt)) {
        Set(R, "$name", name_Get(field));
        Set(R, "$pararg", GlobalQ(ctype) ? "" : "row");
        tempstr text;
        GenPrintField(R,field,print,text);
        if (ch_N(text)) {
            bool bitfldbool = field.c_bitfld && field.arg=="bool" && field.c_bitfld->p_srcfield->c_anonfld;
            bool optfield = field.p_reftype->reftype == amcdb_Tclass_tclass_Opt;
            bool use_conditional = field.p_ctype->c_pmaskfld || bitfldbool || optfield;
            Ins(&R, print.body, "");
            if (use_conditional) {
                if (field.p_ctype->c_pmaskfld){ //do not print pmasked field if there is pmask and corresponding bit set
                    Ins(&R, print.body,"if ($name_PresentQ(row)) {");
                } else if (bitfldbool){
                    Ins(&R, print.body,"if ($name_Get($pararg)) {");
                } else if (optfield){
                    Ins(&R, print.body,"if ($name_Get($pararg) != NULL) {");
                }
            }
            Set(R, "$Attr", (field.c_anonfld ? strptr() : strptr(name_Get(field))));
            tempstr expr;
            Ins(&R, expr, "PrintAttrSpaceReset(str,\"$Attr\", temp);");
            text << CheckDfltExpr(field,expr,false);
            print.body << text;
            if (use_conditional) {
                Ins(&R, print.body, "}");
            }
        }
    }ind_end;
    amc::MaybeUnused(print, "temp");
}


// -----------------------------------------------------------------------------

static void GenPrintNontuple(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    int  counter = 0;
    Set(R, "$outstr", "str");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (GoodForPrintingQ(field,cfmt)) {
        Set(R, "$name", name_Get(field));
        Set(R, "$pararg", GlobalQ(ctype) ? "" : "row");
        tempstr text;
        GenPrintField(R,field,print,text);
        if (ch_N(text)) {
            if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Sep) {
                if (counter > 0) {// separator
                    algo::Smallstr20 sep = cfmt.sep;
                    int i = counter-1;
                    char c = i < ch_N(sep) ? sep.ch[i] : '.';
                    Ins(&R, print.body, tempstr()<<"str << '"<<c<<"';");
                }
            } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_CompactSep) {
                if( counter ) {
                    Ins(&R, print.body, tempstr()<<"str << '"<<cfmt.sep<<"';");
                }
                tempstr expr;
                expr << text;
                text = "";
                text << CheckDfltExpr(field,expr,true);
            }
            print.body << text;
            // counter always gets incremented -- even when field is not printed.
            counter++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void GenPrintBitset(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    cstring sep;
    strptr_PrintCppQuoted(cfmt.sep,sep,'"');
    Set(R, "$sep", sep);
    Set(R, "$outstr", "str");
    Ins(&R, print.body        , "algo::ListSep ls($sep);");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.arg == "bool") {
        Set(R, "$name", name_Get(field));
        if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Get"))) {
            Set(R, "$pararg", GlobalQ(ctype) ? "" : "row");
            Ins(&R, print.body, "if ($name_Get($pararg)) {");
        } else {
            Ins(&R, print.body, "if (row.$name) {");
        }
        Ins(&R, print.body    , "    str << ls << \"$name\";");
        Ins(&R, print.body    , "}");
    }ind_end;
    amc::MaybeUnused(print, "ls");
    amc::MaybeUnused(print, "row");
    amc::MaybeUnused(print, "str");
}

// -----------------------------------------------------------------------------

void amc::GenPrint(amc::FCtype &ctype, amc::FCfmt &cfmt) {
    algo_lib::Replscope R;
    Set(R, "$Cpptype",    ctype.cpp_type);
    Set(R, "$Ctype", ctype.ctype);
    Set(R, "$Name", name_Get(ctype));
    Set(R, "$ByvalArgtype", ByvalArgtype(ctype));

    amc::FFunc& print = amc::CreateCurFunc();
    print.extrn = cfmt.printfmt == dmmeta_Printfmt_printfmt_Extern;
    Ins(&R, print.ret  , "void",false);
    Ins(&R, print.proto, "$Name_Print($ByvalArgtype row, algo::cstring &str)",false);

    if (print.extrn) {
        Ins(&R, print.comment, "Convert $Cpptype to a string (user-implemented function)");
    } else {
        // TODO: fix grammar
        Ins(&R, print.comment, tempstr()<<"print string representation of $Cpptype to string LHS, no header -- cprint:"<<cfmt.cfmt);
    }
    if (!print.extrn) {
        if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Tuple) {
            GenPrintTuple(R, ctype, cfmt, print);
        } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Bitset) {
            GenPrintBitset(R, ctype, cfmt, print);
        } else {
            GenPrintNontuple(R, ctype, cfmt, print);
        }
    }
    amc::MaybeUnused(print, "row");
    amc::MaybeUnused(print, "str");
}
