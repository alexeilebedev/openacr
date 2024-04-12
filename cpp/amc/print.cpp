// Copyright (C) 2023-2024 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/print.cpp -- Print ctype to string
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

tempstr amc::CheckDfltExpr(algo_lib::Replscope &R, amc::FField &field, strptr text, bool canskip) {
    tempstr out;
    tempstr dflt(amc::DfltExprVal(field));
    tempstr checkexpr;
    bool has_compare = field.p_arg->c_ccmp;
    bool cheap_copy = field.p_arg->c_cpptype && field.p_arg->c_cpptype->cheap_copy;

    if (canskip) {
        if (field.reftype==dmmeta_Reftype_reftype_Regx && ch_N(dflt) > 0) {
            checkexpr << FieldvalExpr(field.p_ctype,field,Subst(R,"$parname"))<<".expr == "<<dflt;// known to be a cstring
        } else if (field.reftype==dmmeta_Reftype_reftype_Val && ch_N(dflt) > 0) {
            checkexpr << EqExpr(*field.p_arg, FieldvalExpr(field.p_ctype,field,Subst(R,"$parname")), dflt);// compare with default expression
        } else if (field.reftype==dmmeta_Reftype_reftype_Val && cheap_copy && has_compare) {
            checkexpr << EqExpr(*field.p_arg, FieldvalExpr(field.p_ctype,field,Subst(R,"$parname")), Tempexpr(*field.p_arg,""));
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

// Determine if the field should be printed
static bool GoodForPrintingQ(amc::FField& field, amc::FCfmt &cfmt) {
    bool good = false;
    bool israw = cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw;
    bool source = !bh_bitfld_EmptyQ(field);
    good = good || field.reftype == dmmeta_Reftype_reftype_Varlen;
    good = good || field.reftype == dmmeta_Reftype_reftype_Val;
    good = good || field.reftype == dmmeta_Reftype_reftype_Inlary;
    good = good || field.reftype == dmmeta_Reftype_reftype_Tary;
    good = good || field.reftype == dmmeta_Reftype_reftype_Smallstr;
    good = good || field.reftype == dmmeta_Reftype_reftype_Ptr;
    good = good || field.reftype == dmmeta_Reftype_reftype_Opt;
    good = good || field.reftype == dmmeta_Reftype_reftype_Bitfld;
    good = good || field.reftype == dmmeta_Reftype_reftype_Regx;
    good = good && field.arg != "pad_byte";
    good = good && field.reftype != dmmeta_Reftype_reftype_Base;
    good = good && (!field.c_substr);// do not print these
    good = good && (!field.c_cppfunc || field.c_cppfunc->print);// do not print these
    good = good && (!field.c_bitfld || !israw);// OK to print bitflds in tuple or sep
    good = good && (!source || israw);// do not print srcfield in tuple form
    good = good && !field.c_pmaskfld;// do not print presence mask
    good = good && !field.c_typefld; // skip type field (not applicable)
    good = good && !GetLenfld(field);// skip length field (not applicable to ssim encoding)
    return good;
}

// -----------------------------------------------------------------------------

// Some tuples contain an id gconst field which is based
// on the pkey of the tuple.
// Printing this field as symbolic makes the resulting string unusable, as we
// need the numeric id in this specific case
// Works in tuple and raw contexts
static bool GenPrintField_Id(algo_lib::Replscope &R, amc::FField &field,  cstring &text) {
    bool ret = false;
    amc::FCtype *arg = field.p_arg;
    amc::FField *pkey = c_field_Find(*field.p_ctype,0);
    amc::FField *idfield = c_field_Find(*arg,0);
    if (c_field_N(*arg)==1 && idfield->c_gconst && &field != pkey && idfield->c_gconst->namefld == pkey->field) {
        Set(R, "$Ftype", idfield->cpp_type);
        Set(R, "$accessid", FieldvalExpr(field.p_ctype, *idfield, "$pararg"));
        Ins(&R, text, tempstr()<<"// printing funique, gconst id for pkey "<<pkey->field);
        Ins(&R, text, "$Ftype_Print($accessid, $attrstr);");
        ret=true;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Generate code for printing an individual field
// Output goes into $ATTRSTR
// In tuple context, $ATTRSTR is a temporary variable which is then quoted and inserted
// as a key-value pair into the tuple (using field name as the key)
// In non-tuple context, $ATTRSTR refers to the output string itself, and the output is "raw"
// Function returns TRUE if it emitted code that modified $ATTRSTR
// Works in tuple and raw contexts
static bool GenPrintField(algo_lib::Replscope &R, amc::FField &field,  amc::FFunc &print, bool istuple) {
    amc::FCtype& valtype = *(field).p_arg;
    amc::FCtype &ctype = *field.p_ctype;
    bool ret=false;
    if (field.reftype == dmmeta_Reftype_reftype_Ptr) {
        if (!FldfuncQ(field)) {
            Ins(&R, print.body, "u64_PrintHex(u64($parname.$name), $attrstr, 8, true);");
            if (istuple) {
                Ins(&R, print.body, "PrintAttrSpaceReset(str,\"$name\", $attrstr);");
            }
            ret=true;
        }
    } else if (amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
        vrfy(!func->ismacro, "invalid macro print function");
        Set(R, "$fns", ns_Get(ctype));
        Ins(&R, print.body, "$fns::$name_Print($pararg, $attrstr);");// const cast
        if (istuple) {
            Ins(&R, print.body, "PrintAttrSpaceReset(str,\"$name\", $attrstr);");
        }
        ret=true;
    } else if (field.reftype == dmmeta_Reftype_reftype_Varlen || field.reftype == dmmeta_Reftype_reftype_Inlary || field.reftype == dmmeta_Reftype_reftype_Tary) {
        if (istuple && amc::HasStringPrintQ(valtype)) {
            Set(R, "$Ftype", valtype.cpp_type);
            Ins(&R, print.body, "ind_beg($Parname_$name_curs,$name,$pararg) {");
            if (field.p_arg->c_typefld && !field.p_arg->c_msgtype) {
                Set(R, "$Fldhdrtype", field.p_arg->c_typefld->p_ctype->cpp_type);
                Ins(&R, print.body, "    $FldhdrtypeMsgs_Print($attrstr, $name, INT_MAX);");
            } else {
                Ins(&R, print.body, "    $Ftype_Print($name, $attrstr);");
            }
            Ins(&R, print.body, "    tempstr name;");
            Ins(&R, print.body, "    name << \"$name.\";");
            Ins(&R, print.body, "    name << ind_curs($name).index;");
            Ins(&R, print.body, "    PrintAttrSpaceReset(str, name, $attrstr);");
            Ins(&R, print.body, "}ind_end;");
            ret=true;
        }
    } else if (HasStringPrintQ(valtype)) {
        ret=true;
        if (GenPrintField_Id(R,field,print.body)) {
            // handled
        } else {
            Set(R, "$Ftype", valtype.cpp_type);
            Ins(&R, print.body, tempstr()<<"$Ftype_Print("<<FieldvalExpr(&ctype, field, "$parname")<<", $attrstr);");
        }
        if (istuple) {
            Ins(&R, print.body, "PrintAttrSpaceReset(str,\"$name\", $attrstr);");
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Print an ssim tuple with values of all fields in CTYPE
// First, print the type tag. It's either the ctype name, or the ssimfile name, if one exists.
// Then, print each field as an ssim key-value pair ("Attr").
// Special handling:
// - For fields that are members of a presence mask with "filter_member:Y" set,
//   the field is only printed if the correponding bit is set in the presence mask
// - For fields that are of type "bool" and part of a bitfield, the field is only printed if its value is "true".
// - Opt fields are only printed if present.
static void GenPrintTuple(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    Set(R, "$attrstr", "temp");
    Set(R, "$rel", ctype.c_ssimfile ? algo::strptr(ctype.c_ssimfile->ssimfile) : algo::strptr(ctype.ctype));
    Ins(&R, print.body, "algo::tempstr temp;");
    Ins(&R, print.body, "str << \"$rel\";");
    amc::FPmaskfld *filter = GetPrintFilter(ctype);
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (GoodForPrintingQ(field,cfmt)) {
        amc::FPmaskfldMember *filter_member = filter ? FindMember(field,filter) : NULL;
        Set(R, "$name", name_Get(field));
        bool bitfldbool = field.c_bitfld && field.arg=="bool" && field.c_bitfld->p_srcfield->c_anonfld;
        bool optfield = field.p_reftype->reftype == amcdb_Tclass_tclass_Opt;
        int conditional=0;
        Ins(&R, print.body, "");
        int n0=ch_N(print.body);
        if (filter_member){ // do not print pmasked field if there is pmask and corresponding bit set
            Set(R, "$Present", filter_member->p_pmaskfld->funcname);
            Ins(&R, print.body,"if ($name_$PresentQ($parname)) {");
            conditional++;
        }
        if (bitfldbool || optfield){
            Ins(&R, print.body,"if ($name_Get($pararg)) {");
            conditional++;
        }
        if (!GenPrintField(R,field,print,true)) {
            if (!field.p_arg->c_cextern) {
                prlog("amc.need_print"
                      <<Keyval("ctype",field.arg)
                      <<Keyval("required_by",cfmt.cfmt));
                int nfield=c_field_N(*field.p_arg);
                algo::strptr printfmt=(nfield==1 ? "Raw": "Tuple");
                prlog("dmmeta.cfmt cfmt:"<<field.arg<<".String  printfmt:"<<printfmt<<" read:Y print:Y genop:Y");
                algo_lib::_db.exit_code=1;
            }
            print.body.ch_n=n0;
            conditional=0;
        }
        while (conditional) {
            Ins(&R, print.body, "}");
            conditional--;
        }
    }ind_end;
    amc::MaybeUnused(print, "temp");
}


// -----------------------------------------------------------------------------

// Print values of all fields in CTYPE, using Sep format.
// Fields are separated by the characters from the separator string (cfmt.sep).
// The characters are taken from the separator string one by one, until it
// it is exhausted, at which point '.' is used as the separator.
// The value of the field is printed after the separator.
static void GenPrintSep(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    int counter = 0;
    Set(R, "$attrstr", "str");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (GoodForPrintingQ(field,cfmt)) {
        Set(R, "$name", name_Get(field));
        int n0=ch_N(print.body);
        if (counter > 0) {// separator
            algo::Smallstr20 sep = cfmt.sep;
            int i = counter-1;
            char c = i < ch_N(sep) ? sep.ch[i] : '.';
            Ins(&R, print.body, tempstr()<<"str << "<<amc::char_ToCppSingleQuote(c)<<";");
        }
        if (GenPrintField(R,field,print,false)) {
            counter++;
        } else {
            // field not printed, roll back
            print.body.ch_n=n0;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print a separated list of fields which are "true" in the parent struct
// The name of each field is printed.
static void GenPrintBitset(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &print) {
    cstring sep;
    strptr_PrintCppQuoted(cfmt.sep,sep,'"');
    Set(R, "$sep", sep);
    Ins(&R, print.body        , "algo::ListSep ls($sep);");
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.arg == "bool") {
        Set(R, "$name", name_Get(field));
        Ins(&R, print.body, tempstr()<<"if ("<<FieldvalExpr(&ctype,field,"$parname")<<") {");
        Ins(&R, print.body,            "    str << ls << \"$name\";");
        Ins(&R, print.body,            "}");
    }ind_end;
    amc::MaybeUnused(print, "ls");
}

// -----------------------------------------------------------------------------

static void GenPrintArgv(algo_lib::Replscope &R, amc::FCtype &parent, amc::FCfmt &cfmt, amc::FFunc &print) {
    bool gnu = strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_ArgvGnu;
    Ins(&R, print.body   , "algo::tempstr temp;");
    Ins(&R, print.body   , "(void)temp;");
    Ins(&R, print.body   , "(void)str;");// necessary evil
    ind_beg(amc::ctype_c_field_curs, field,parent) if (!FldfuncQ(field)) {
        amc::FCtype& valtype = *(field).p_arg;
        algo::strptr dash = gnu && ch_N(name_Get(field))>1 ? "--" : "-";
        algo::strptr space = gnu ? " " : ":";
        Set(R, "$Attr", tempstr()<<dash<<name_Get(field)<<space);
        Set(R, "$name", name_Get(field));
        if (field.reftype == dmmeta_Reftype_reftype_Tary || field.reftype == dmmeta_Reftype_reftype_Inlary) {
            Set(R, "$Ftype", name_Get(valtype));
            Ins(&R, print.body, "ind_beg($Name_$name_curs,value,$parname) {");
            Ins(&R, print.body, "    ch_RemoveAll(temp);");
            Ins(&R, print.body, "    $Ftype_Print(value, temp);");
            Ins(&R, print.body, "    str << \" $Attr\";");
            Ins(&R, print.body, "    strptr_PrintBash(temp,str);");
            Ins(&R, print.body, "}ind_end;");
        } else {
            tempstr text;
            Ins(&R, text,"ch_RemoveAll(temp);");
            if (amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"))) {
                Set(R, "$fns", ns_Get(*field.p_ctype));
                Ins(&R, text, "$fns::$name_Print(const_cast<$Cpptype&>($parname), temp);");
            } else {
                Set(R, "$Ftype", name_Get(valtype));
                Ins(&R, text,"$Ftype_Print($parname.$name, temp);");
            }
            Ins(&R, text,"str << \" $Attr\";");
            Ins(&R, text,"strptr_PrintBash(temp,str);");
            bool canskip = !field.c_anonfld; // anonymous fields must be printed or meaning of command can change
            print.body << CheckDfltExpr(R,field,text,canskip);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::GenPrintJson(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &, amc::FFunc &fmtjson) {
    if (c_datafld_N(ctype)!=1) { // zero or many -- always return some object even empty, to form valud field on upper object
        Ins(&R, fmtjson.body, "lib_json::FNode *object_node = &lib_json::node_Alloc();");
        Ins(&R, fmtjson.body, "object_node->p_parent = parent?parent:object_node;");
        Ins(&R, fmtjson.body, "object_node->type   = lib_json_FNode_type_object;");
        Ins(&R, fmtjson.body, "node_XrefMaybe(*object_node);");
        Set(R,"$return_json_node","object_node");
    }
    ind_beg(amc::ctype_c_field_curs, field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Val && !FldfuncQ(field)) {
            amc::FCtype& valtype = *(field).p_arg;
            Set(R, "$Ftype", valtype.cpp_type);
            Set(R, "$fldref", FieldvalExpr(&ctype, field, "const_cast<$Cpptype&>(row)"));
            if (c_datafld_N(ctype)!=1) {
                Set(R, "$Fldtype", valtype.ctype);
                Set(R, "$name", name_Get(field));
                amc::FCfmt *print_json = amc::ind_cfmt_Find(Subst(R, "$Fldtype.Json"));
                if (print_json) {
                    Ins(&R, fmtjson.body, "");
                    Ins(&R, fmtjson.body, "lib_json::FNode *$name_field_node = &lib_json::node_Alloc();");
                    Ins(&R, fmtjson.body, "$name_field_node->p_parent = object_node;");
                    Ins(&R, fmtjson.body, "$name_field_node->type   = lib_json_FNode_type_field;");
                    Ins(&R, fmtjson.body, "$name_field_node->value  = \"$name\";");
                    Ins(&R, fmtjson.body, "node_XrefMaybe(*$name_field_node);");
                    Ins(&R, fmtjson.body, "$Ftype_FmtJson($fldref,$name_field_node);");
                } else {// try the print function
                    Ins(&R, fmtjson.body, "$Ftype_Print($fldref, lib_json::NewStringNode(object_node, \"$name\").value);");
                }
            } else {
                Set(R,"$return_json_node", "$Ftype_FmtJson($fldref,parent);");
            }
        } else if (field.reftype == dmmeta_Reftype_reftype_Tary || field.reftype == dmmeta_Reftype_reftype_Inlary) {
            amc::FCtype& valtype = *(field).p_arg;
            Set(R, "$Ftype", valtype.cpp_type);
            Set(R, "$fldref", FieldvalExpr(&ctype, field, "const_cast<$Cpptype&>(row)"));
            if (c_datafld_N(ctype)!=1) {
                Set(R, "$Fldtype", valtype.ctype);
                Set(R, "$name", name_Get(field));
                if (amc::ind_cfmt_Find(Subst(R, "$Fldtype.Json"))) {
                    Ins(&R, fmtjson.body, "");
                    Ins(&R, fmtjson.body, "algo::aryptr<$Ftype> $name_ary = $name_Getary(row);");
                    Ins(&R, fmtjson.body, "lib_json::FNode& $name_node = lib_json::NewArrayNode(object_node, \"$name\");");
                    Ins(&R, fmtjson.body, "for (int i = 0; i < $name_ary.n_elems; ++i) {");
                    Ins(&R, fmtjson.body, "    $Ftype_FmtJson($name_ary[i], &$name_node);");
                    Ins(&R, fmtjson.body, "}");
                }
            } else {
                Set(R,"$return_json_node", "$Ftype_FmtJson($fldref,parent);");
            }
        }
    }ind_end;
    Ins(&R, fmtjson.body, "return $return_json_node;");
}

// -----------------------------------------------------------------------------

void amc::GenPrint(amc::FCtype &parent, amc::FCfmt &cfmt) {
    algo_lib::Replscope R;
    tempstr parname("row");//Refname(parent);
    bool glob = GlobalQ(parent);
    amc::FField *pool=FirstInst(parent);
    Set(R, "$parname"  , parname);
    Set(R, "$pararg"   , glob ? strptr("")    : parname);
    Set(R, "$Parname"  , (pool ? strptr(name_Get(*pool)) : strptr(name_Get(parent))));
    Set(R, "$Name", name_Get(parent));
    Set(R, "$Cpptype",    parent.cpp_type);
    Set(R, "$Ctype", parent.ctype);
    // compute function name
    dmmeta::StrfmtPkey strfmt = strfmt_Get(cfmt);
    bool json = strfmt == dmmeta_Strfmt_strfmt_Json;
    Set(R, "$Func", strfmt == dmmeta_Strfmt_strfmt_String ? "Print"
        : (strfmt == dmmeta_Strfmt_strfmt_Argv || strfmt == dmmeta_Strfmt_strfmt_ArgvGnu) ? "PrintArgv"
        : algo::strptr(tempstr() << "Fmt"<<strfmt));// FmtJson

    amc::FFunc& print = amc::CreateCurFunc(false,Subst(R,"$Func"));
    Ins(&R, print.proto, "$Name_$Func()",false);
    AddProtoArg(print,ByvalArgtype(parent),parname);
    // create arguments
    if (!json) {// json has different args
        AddRetval(print,"void","","");
        AddProtoArg(print,"algo::cstring&", "str");
        Ins(&R, print.comment, "print string representation of ROW to string STR");
    } else {
        AddRetval(print, "lib_json::FNode *", "", "");
        AddProtoArg(print, "lib_json::FNode *", "parent");
        Ins(&R, print.comment, "Create JSON representation of $Cpptype under PARENT node");
    }
    Ins(&R, print.comment, tempstr()<<"cfmt:"<<cfmt.cfmt<<"  printfmt:"<<cfmt.printfmt);

    if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Extern) {
        print.extrn = true;
    } else if (strfmt == dmmeta_Strfmt_strfmt_String) {
        if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Tuple || cfmt.printfmt == dmmeta_Printfmt_printfmt_Auto) {
            GenPrintTuple(R, parent, cfmt, print);
        } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Bitset) {
            GenPrintBitset(R, parent, cfmt, print);
        } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Sep || cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw) {
            GenPrintSep(R, parent, cfmt, print);
        }
    } else if (strfmt == dmmeta_Strfmt_strfmt_ArgvGnu || strfmt == dmmeta_Strfmt_strfmt_Argv) {
        GenPrintArgv(R, parent, cfmt, print);
    } else if (strfmt == dmmeta_Strfmt_strfmt_Json) {
        GenPrintJson(R, parent, cfmt, print);
    } else if (strfmt == dmmeta_Strfmt_strfmt_Tuple) {
        vrfy("amc.badcfmt",tempstr()
             <<Keyval("cfmt",cfmt.cfmt)
             <<Keyval("printfmt",cfmt.printfmt)
             <<Keyval("comment","unsupported strfmt for printing"));
    }
}
