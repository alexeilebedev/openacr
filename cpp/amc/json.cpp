// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/json.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"


// -----------------------------------------------------------------------------

static void _GenFmtJson(amc::FCtype &ctype, amc::FCfmt *fcfmt) {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    Set(R, "$Cpptype",    ctype.cpp_type);
    Set(R, "$Ctype", ctype.ctype);
    Set(R, "$Name", name_Get(ctype));
    Set(R, "$ByvalArgtype", ByvalArgtype(ctype));

    amc::FFunc& fmtjson = amc::CreateCurFunc();
    fmtjson.extrn = fcfmt->printfmt == dmmeta_Printfmt_printfmt_Extern;
    Ins(&R, fmtjson.ret  , "lib_json::FNode *",false);
    Ins(&R, fmtjson.proto, "$Name_FmtJson($ByvalArgtype row, lib_json::FNode *parent)",false);

    if (fmtjson.extrn) {
        Ins(&R, fmtjson.comment, "Create JSON representation of $Cpptype under PARENT node -- implemented externally");
    } else {
        Ins(&R, fmtjson.comment, "Create JSON representation of $Cpptype under PARENT node -- cprint:$Ctype.Json");
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
            } else if (field.reftype == dmmeta_Reftype_reftype_Tary) {
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
}

// -----------------------------------------------------------------------------

// Generate a function to convert ctype to Json
// Called from amc::tfunc_Ctype_FmtJson
void amc::GenFmtJson(amc::FCtype &ctype) {
    algo_lib::Replscope R;
    amc::FCfmt *fcfmt = NULL;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.print && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_Json) {
            fcfmt = &cfmt;
        }
    }ind_end;
    if (NULL != fcfmt) {
        _GenFmtJson(ctype,fcfmt);
    }
}
