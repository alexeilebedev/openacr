// Copyright (C) 2023-2026 AlgoRND
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

static tempstr CheckDfltExpr(algo_lib::Replscope &R, amc::FField &field, strptr text, bool canskip) {
    tempstr out;
    tempstr dflt(amc::DfltExprVal(field));
    tempstr checkexpr;
    bool has_compare = field.p_arg->c_ccmp;
    bool cheap_copy = amc::CheapCopyQ(field);

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
    good = good && !field.c_falias;// do not print alias field
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
        Set(R, "$accessid", FieldvalExpr(field.p_ctype, *idfield, "$parname"));
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
            Ins(&R, print.body, "ind_beg($Parname_$name_curs,$name,$parname) {");
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
    ind_beg(amc::ctype_c_field_curs, field,ctype) {
        if (GoodForPrintingQ(field,cfmt)) {
            amc::FPmaskfldMember *filter_member = filter ? FindMember(field,filter) : NULL;
            Set(R, "$name", name_Get(field));
            bool bitfldbool = field.c_bitfld && field.arg=="bool" && field.c_bitfld->p_srcfield->c_anonfld;
            bool optfield = field.p_reftype->reftype == amcdb_Tclass_tclass_Opt;
            int conditional=0;
            Ins(&R, print.body, "");
            int n0=ch_N(print.body);
            if (filter_member){ // do not print pmasked field if there is pmask and corresponding bit set
                Set(R, "$Present", filter_member->p_pmaskfld->funcname);
                Ins(&R, print.body,"if ($name_$PresentQ($pararg)) {");
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

// Append to TEXT the statement(s) emitting one command-line token whose
// value has been printed into TEMP: for ToArgv a token appended to the args
// array (the ArgvGnu form uses a separate value token), for PrintArgv
// bash-quoted text appended to the output string. INDENT prefixes each line
// (the array walk emits inside a cursor block).
static void GenArgvEmit(algo_lib::Replscope &R, cstring &text, bool toargv, bool gnu, strptr indent) {
    if (toargv && gnu) {
        Ins(&R, text, tempstr()<<indent<<"ary_Alloc(args) << \"$Attr\";");
        Ins(&R, text, tempstr()<<indent<<"ary_Alloc(args) << temp;");
    } else if (toargv) {
        Ins(&R, text, tempstr()<<indent<<"ary_Alloc(args) << \"$Attr\" << temp;");
    } else {
        Ins(&R, text, tempstr()<<indent<<"str << \" $Attr\";");
        Ins(&R, text, tempstr()<<indent<<"strptr_PrintBash(temp,str);");
    }
}

// -----------------------------------------------------------------------------

// Emit the per-field token walk shared by PrintArgv and ToArgv: each field
// the cfmt prints becomes one command-line token (-name:value; --name with a
// separate value token for ArgvGnu), array fields walk via a cursor, and a
// defaulted value is skipped unless the field is anonymous (skipping an
// anonymous field would change the meaning of the command).
// TOARGV picks the output form (see GenArgvEmit).
// The selector is GoodForPrintingQ, the same allowlist the tuple and sep
// walks use: a field with no member of its own to name -- the Global anchor,
// the Base anchor, a length or type field, the presence mask -- carries no
// command-line value and is not admitted. A Ptr is admitted for tuple print,
// where it shows as a hex address for debugging, but an address has no
// command-line spelling that ReadArgv could take back, so it draws no token
// here. The value prints through GenPrintStmt, which calls the field's own
// Print function when one exists (parent argument collapsed by ParentArgExpr
// when the parent is a global) and otherwise applies the value type's Print to
// the field value expression, so a bitfield or a big-endian field reaches its
// value through its Get accessor.
// test/amc/argv_field.ssim pins which field shapes draw a token and which
// do not.
static void GenArgvField(algo_lib::Replscope &R, amc::FCtype &parent, amc::FCfmt &cfmt, amc::FFunc &func, bool toargv) {
    bool gnu = strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_ArgvGnu;
    ind_beg(amc::ctype_c_field_curs, field,parent) {
        bool token = GoodForPrintingQ(field,cfmt) && field.reftype != dmmeta_Reftype_reftype_Ptr;
        bool array = field.reftype == dmmeta_Reftype_reftype_Tary
            || field.reftype == dmmeta_Reftype_reftype_Inlary
            || field.reftype == dmmeta_Reftype_reftype_Varlen;
        if (token) {
            amc::FCtype& valtype = *(field).p_arg;
            algo::strptr dash = gnu && ch_N(name_Get(field))>1 ? "--" : "-";
            algo::strptr suffix = gnu ? (toargv ? strptr("") : strptr(" ")) : strptr(":");
            Set(R, "$Attr", tempstr()<<dash<<name_Get(field)<<suffix);
            Set(R, "$name", name_Get(field));
            Set(R, "$Ftype", name_Get(valtype));
            if (array) {
                Ins(&R, func.body, "ind_beg($Name_$name_curs,value,$parname) {");
                Ins(&R, func.body, "    ch_RemoveAll(temp);");
                Ins(&R, func.body, "    $Ftype_Print(value, temp);");
                GenArgvEmit(R, func.body, toargv, gnu, "    ");
                Ins(&R, func.body, "}ind_end;");
            } else {
                tempstr text;
                Ins(&R, text,"ch_RemoveAll(temp);");
                amc::GenPrintStmt(text, parent, field, "temp", Subst(R,"$parname"));
                GenArgvEmit(R, text, toargv, gnu, "");
                bool canskip = !field.c_anonfld; // anonymous fields must be printed or meaning of command can change
                func.body << CheckDfltExpr(R,field,text,canskip);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void GenPrintArgv(algo_lib::Replscope &R, amc::FCtype &parent, amc::FCfmt &cfmt, amc::FFunc &print) {
    Ins(&R, print.body   , "algo::tempstr temp;");
    Ins(&R, print.body   , "(void)temp;");
    Ins(&R, print.body   , "(void)str;");// necessary evil
    GenArgvField(R, parent, cfmt, print, false);
}

// -----------------------------------------------------------------------------

// Build an argv (algo::StringAry) from a command ctype, the array analog of
// GenPrintArgv. args[0] is the command path; each field becomes one token
// (-name:value, Argv) or two (--name value, ArgvGnu). Selects the same fields
// and skips the same defaulted values as GenPrintArgv -- both walk
// GenArgvField.
static void GenToArgv(algo_lib::Replscope &R, amc::FCtype &parent, amc::FCfmt &cfmt, amc::FFunc &func) {
    bool gnu = strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_ArgvGnu;
    Ins(&R, func.body, "algo::tempstr temp;");
    Ins(&R, func.body, "(void)temp;");
    Ins(&R, func.body, "ary_RemoveAll(args);");
    if (gnu) {
        Ins(&R, func.body, "ary_Alloc(args) << \"$Name\"; // command path");
    } else {
        Ins(&R, func.body, "ary_Alloc(args) << \"bin/$Name\"; // command path");
    }
    GenArgvField(R, parent, cfmt, func, true);
}

// -----------------------------------------------------------------------------

// Return the single data field of CTYPE whose own JSON node stands in for an
// object wrapper -- a one-field ctype prints as that field's value, so that
// x.y reads y's value directly instead of the object {"y":...}.
// NULL when CTYPE has zero or several data fields, and also when its one data
// field carries nothing of its own: the presence mask, a Base anchor, and a
// Global anchor each produce no node, and a ctype standing on one of those
// prints as an object like any other, empty when the walk emits no member.
// NULL as well when the field carries a bit on the ctype's print filter,
// because such a field prints only when its bit is set while the inlined node
// is the function's return value and has to exist on every path. Such a ctype
// prints as an object, empty when the bit is clear. Membership decides this,
// not the presence of a filter: a filter that gives the field no bit leaves
// the field printing unconditionally, and it stays inlined.
static amc::FField *GetJsonInlfld(amc::FCtype &ctype) {
    amc::FField *ret = NULL;
    amc::FPmaskfld *filter = GetPrintFilter(ctype);
    if (c_datafld_N(ctype) == 1) {
        amc::FField &field = *c_datafld_Find(ctype, 0);
        bool node = !field.c_pmaskfld;
        node = node && (field.reftype == dmmeta_Reftype_reftype_Val
                        || field.reftype == dmmeta_Reftype_reftype_Tary
                        || field.reftype == dmmeta_Reftype_reftype_Inlary);
        node = node && !(filter && FindMember(field,filter));
        if (node) {
            ret = &field;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Emit the FmtJson body for CTYPE under CFMT.
// The walk visits the ctype's data fields -- what c_datafld holds, less the
// presence mask -- which is the same set GetJsonInlfld chooses the inlined
// field from, so the decision to declare the object node and the decision to
// emit a member into it cannot disagree. A field carrying no data of its own is
// therefore skipped and not reported: a fldfunc computes its value rather than
// storing one, a bitfield's bits travel inside the printed source word, a
// base's fields print as members of this walk, and a global field is the
// singleton instance anchor.
// When the ctype has a print filter, a field carrying a bit on it emits its
// member inside a test of that bit, so the member appears only when the bit is
// set. The test is opened only around a member the walk emits: a field that
// carries no data of its own emits nothing, and opening a test around nothing
// would spell out an empty conditional.
// A repeated field's element span is fetched through that field's own Getary,
// which takes the parent record for an ordinary ctype and no argument at all
// for a global -- there is one instance, reached through the namespace global
// -- so the call is written with the same collapsing parent argument the rest
// of the walk uses.
// The locals the walk declares are named after the fields they serve, and a
// field name is any identifier the schema accepts. A ctype whose array field
// is named "object" therefore reaches for the name of the record's own node,
// and a value field named "x" reaches for the name an array field named
// "x_field" would take, each collision spelling a redeclaration the compiler
// refuses. So a local takes a field's name through one role suffix only --
// $name_node for the node standing for the field, $name_ary for its element
// span -- which makes two such locals equal only when the field and the role
// are, and the record's own node is named objnode, a name no suffix produces.
// The comptest amc.JsonAry pins a ctype whose field names reach for each of
// those collisions.
static void GenPrintJson(algo_lib::Replscope &R, amc::FCtype &ctype, amc::FCfmt &cfmt, amc::FFunc &fmtjson) {
    amc::FPmaskfld *filter = GetPrintFilter(ctype);
    amc::FField *inlfld = GetJsonInlfld(ctype);
    if (inlfld) {
        Set(R,"$return_json_node","NULL");// overwritten by the inlined field's own node below
    } else {
        // zero, several, or one field that prints nothing of its own -- always
        // return an object, empty if the walk emits no member, so the record
        // still forms a valid field value on the upper object
        Ins(&R, fmtjson.body, "lib_json::FNode *objnode = &lib_json::node_Alloc();");
        Ins(&R, fmtjson.body, "objnode->p_parent = parent?parent:objnode;");
        Ins(&R, fmtjson.body, "objnode->type   = lib_json_FNode_type_object;");
        Ins(&R, fmtjson.body, "node_XrefMaybe(*objnode);");
        Set(R,"$return_json_node","objnode");
    }
    ind_beg(amc::ctype_c_field_curs, field,ctype) if (!field.c_pmaskfld) {
        Set(R, "$name", name_Get(field));
        int conditional=0;
        tempstr reason;// non-empty when the field produces no node and must be reported
        bool datafld = !FldfuncQ(field) && !field.c_bitfld;// the c_datafld rule: the fields this walk can emit
        amc::FPmaskfldMember *filter_member = filter && datafld ? FindMember(field,filter) : NULL;
        if (filter_member){ // do not print pmasked field if there is pmask and corresponding bit set
            Set(R, "$Present", filter_member->p_pmaskfld->funcname);
            Ins(&R, fmtjson.body,"if ($name_$PresentQ($pararg)) {");
            conditional++;
        }
        if (!datafld) {
            // not a data field: outside c_datafld, so outside this walk
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            amc::FCtype& valtype = *(field).p_arg;
            Set(R, "$Ftype", valtype.cpp_type);
            Set(R, "$fldref", FieldvalExpr(&ctype, field, "row"));
            Set(R, "$Fldtype", valtype.ctype);
            bool val_json = amc::ind_cfmt_Find(Subst(R, "$Fldtype.Json")) != NULL;
            bool val_print = amc::HasStringPrintQ(valtype);
            bool inl = &field == inlfld;
            if (!inl && val_json) {
                Ins(&R, fmtjson.body, "");
                Ins(&R, fmtjson.body, "lib_json::FNode *$name_node = &lib_json::node_Alloc();");
                Ins(&R, fmtjson.body, "$name_node->p_parent = objnode;");
                Ins(&R, fmtjson.body, "$name_node->type   = lib_json_FNode_type_field;");
                Ins(&R, fmtjson.body, "$name_node->value  = \"$name\";");
                Ins(&R, fmtjson.body, "node_XrefMaybe(*$name_node);");
                Ins(&R, fmtjson.body, "$Ftype_FmtJson($fldref,$name_node);");
            } else if (!inl && val_print) {
                Ins(&R, fmtjson.body, "$Ftype_Print($fldref, lib_json::NewStringNode(objnode, \"$name\").value);");
            } else if (inl && val_json) {
                Set(R,"$return_json_node", "$Ftype_FmtJson($fldref,parent)");
            } else if (inl && val_print) {
                Ins(&R, fmtjson.body, "lib_json::FNode& $name_node = lib_json::NewStringNode(parent);");
                Ins(&R, fmtjson.body, "$Ftype_Print($fldref, $name_node.value);");
                Set(R,"$return_json_node", "&$name_node");
            } else {
                reason << "field type has neither a Json cfmt nor a String print";
            }
        } else if (field.reftype == dmmeta_Reftype_reftype_Tary || field.reftype == dmmeta_Reftype_reftype_Inlary) {
            amc::FCtype& valtype = *(field).p_arg;
            Set(R, "$Ftype", valtype.cpp_type);
            Set(R, "$Fldtype", valtype.ctype);
            bool inl = &field == inlfld;
            bool elem_json = amc::ind_cfmt_Find(Subst(R, "$Fldtype.Json")) != NULL;
            bool elem_print = amc::HasStringPrintQ(valtype);
            if (elem_json || elem_print) {
                Ins(&R, fmtjson.body, "");
                Ins(&R, fmtjson.body, "algo::aryptr<$Ftype> $name_ary = $name_Getary($pararg);");
                if (inl) {
                    Ins(&R, fmtjson.body, "lib_json::FNode& $name_node = lib_json::NewArrayNode(parent);");
                    Set(R,"$return_json_node", "&$name_node");
                } else {
                    Ins(&R, fmtjson.body, "lib_json::FNode& $name_node = lib_json::NewArrayNode(objnode, \"$name\");");
                }
                Ins(&R, fmtjson.body, "for (int i = 0; i < $name_ary.n_elems; ++i) {");
                if (elem_json) {
                    Ins(&R, fmtjson.body, "    $Ftype_FmtJson($name_ary[i], &$name_node);");
                } else {
                    Ins(&R, fmtjson.body, "    $Ftype_Print($name_ary[i], lib_json::NewStringNode(&$name_node).value);");
                }
                Ins(&R, fmtjson.body, "}");
            } else {
                reason << "element type has neither a Json cfmt nor a String print";
            }
        } else if (field.reftype != dmmeta_Reftype_reftype_Base && field.reftype != dmmeta_Reftype_reftype_Global) {
            reason << "unsupported reftype for JSON print";
        }
        if (ch_N(reason)) {
            prerr("amc.badjsonfld"
                  <<Keyval("cfmt",cfmt.cfmt)
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype)
                  <<Keyval("comment",reason));
            algo_lib::_db.exit_code++;
        }
        while (conditional) {
            Ins(&R, fmtjson.body, "}");
            conditional--;
        }
    }ind_end;
    Ins(&R, fmtjson.body, "return $return_json_node;");
}

// -----------------------------------------------------------------------------

// Create print function for ctype PARENT based on CFMT.
// The pair (strfmt, printfmt) selects the body. The Extern printfmt hands the
// body to the user for every strfmt; otherwise String takes a printfmt of
// Tuple, Auto, Bitset, Sep or Raw, Argv and ArgvGnu take any printfmt (the
// command line spells its own separators), and Json takes any printfmt as
// well. Every other pair -- the Tuple strfmt, which has no print path at all,
// and the CompactSep printfmt, which has no print path under String -- would
// leave the function with an empty body, so it is rejected naming both
// attributes. test/amc/bad_cfmt_print.ssim pins the pairs on both sides of
// that line.
void amc::GenPrint(amc::FCtype &parent, amc::FCfmt &cfmt) {
    algo_lib::Replscope R;
    bool good = true;
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
        print.acrkey << "cfmt:"<<cfmt.cfmt;
        print.extrn = true;
    } else if (strfmt == dmmeta_Strfmt_strfmt_String) {
        if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Tuple || cfmt.printfmt == dmmeta_Printfmt_printfmt_Auto) {
            GenPrintTuple(R, parent, cfmt, print);
        } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Bitset) {
            GenPrintBitset(R, parent, cfmt, print);
        } else if (cfmt.printfmt == dmmeta_Printfmt_printfmt_Sep || cfmt.printfmt == dmmeta_Printfmt_printfmt_Raw) {
            GenPrintSep(R, parent, cfmt, print);
        } else {
            good = false;
        }
    } else if (strfmt == dmmeta_Strfmt_strfmt_ArgvGnu || strfmt == dmmeta_Strfmt_strfmt_Argv) {
        GenPrintArgv(R, parent, cfmt, print);
        // Also emit ToArgv: build an algo::StringAry (real argv) from the command,
        // ready for algo_lib::ProcStart. Same cfmt drives both.
        amc::FFunc& toargv = amc::CreateCurFunc(false, "ToArgv");
        Ins(&R, toargv.proto, "$Name_ToArgv()", false);
        AddProtoArg(toargv, ByvalArgtype(parent), parname);
        AddRetval(toargv, "void", "", "");
        AddProtoArg(toargv, "algo::StringAry&", "args");
        Ins(&R, toargv.comment, "Build argv from ROW into ARGS; args[0] is the command name");
        Ins(&R, toargv.comment, tempstr()<<"cfmt:"<<cfmt.cfmt<<"  printfmt:"<<cfmt.printfmt);
        GenToArgv(R, parent, cfmt, toargv);
    } else if (strfmt == dmmeta_Strfmt_strfmt_Json) {
        GenPrintJson(R, parent, cfmt, print);
    } else {
        good = false;
    }
    if (!good) {
        prerr("amc.badcfmt"
              <<Keyval("cfmt",cfmt.cfmt)
              <<Keyval("strfmt",strfmt)
              <<Keyval("printfmt",cfmt.printfmt)
              <<Keyval("comment","no print path for this strfmt and printfmt"));
        algo_lib::_db.exit_code++;
    }
}
