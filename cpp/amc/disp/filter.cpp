// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/disp/filter.cpp -- Dispatch filter
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool FilterAcceptQ(amc::FField &field) {
    bool skip = field.reftype == dmmeta_Reftype_reftype_Varlen;
    skip |= field.reftype == dmmeta_Reftype_reftype_Base;
    skip |= GetLenfld(field) != NULL;
    skip |= field.c_typefld != NULL;
    skip |= field.c_pmaskfld != NULL;
    return !skip;
}

// -----------------------------------------------------------------------------

static void Filter_Partial(algo_lib::Replscope &R, amc::FDispatchmsg &dispatch_msg, amc::FFunc &filter) {
    ind_beg(amc::ctype_c_field_curs,field,*dispatch_msg.p_ctype) if (!PadQ(field)) {
        Set(R,"$Fieldname",name_Get(field));
        Set(R,"$Getfield",amc::FieldvalExpr(field.p_ctype,field,"msg"));
        if (amc::FField *parfield = amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.$Fieldname"))) {
            if (parfield->c_pmaskfld) {// skip pmask in parent
                parfield = NULL;
            }
            if (parfield && parfield->arg != field.arg) {// make sure types match
                verblog("amc.gen_dispatch"
                        <<Keyval("dispatch_msg",dispatch_msg.dispatch_msg)
                        <<Keyval("field",field.field)
                        <<Keyval("field2",parfield->field)
                        <<Keyval("comment","With with same name but different type, filtering skipped"));
                parfield = NULL;
            }
            if (parfield) {
                Ins(&R, filter.body, "if ($Fieldname_PresentQ(parent) && !($Getfield == parent.$Fieldname)) {");
                Ins(&R, filter.body, "    return false;");
                Ins(&R, filter.body, "}");
            }
        }
        if (amc::FField *filterfield = amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.$Fieldname_regx"))) {
            Ins(&R, filter.body, "if ($Fieldname_regx_PresentQ(parent)) {");
            Ins(&R, filter.body, "    ch_RemoveAll(fieldstr);");
            filter.body << "    ";
            (void)filterfield;
            GenPrintStmt(filter.body, *field.p_ctype, field, "fieldstr", "msg");
            Ins(&R, filter.body, "    if (!algo_lib::Regx_Match(parent.$Fieldname_regx, fieldstr)) {");
            Ins(&R, filter.body, "        return false;");
            Ins(&R, filter.body, "    }");
            Ins(&R, filter.body, "}");
        }
        if (amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.start_$Fieldname"))) {
            Ins(&R, filter.body, "if (start_$Fieldname_PresentQ(parent) && $Getfield < parent.start_$Fieldname) {");
            Ins(&R, filter.body, "    return false;");
            Ins(&R, filter.body, "}");
        }
        if (amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.end_$Fieldname"))) {
            Ins(&R, filter.body, "if (end_$Fieldname_PresentQ(parent) && parent.end_$Fieldname < $Getfield) {");
            Ins(&R, filter.body, "    return false;");
            Ins(&R, filter.body, "}");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Filter_Full(algo_lib::Replscope &R, amc::FDispatchmsg &dispatch_msg, amc::FFunc &filter, amc::FCtype &ctype) {
    ind_beg(amc::ctype_c_field_curs,field,ctype) if (!PadQ(field)) {
        bool do_match = FilterAcceptQ(field);
        if (!do_match) {
            continue;
        }
        Set(R,"$Fieldname",name_Get(field));
        Set(R,"$Getfield",amc::FieldvalExpr(field.p_ctype,field,"msg"));
        if (amc::FField *childfield = amc::ind_field_Find(Subst(R,"$Msgns.$Msgname.$Fieldname"))) {
            if (field.arg != childfield->arg) {// make sure types match
                verblog("amc.gen_dispatch"
                        <<Keyval("dispatch_msg",dispatch_msg.dispatch_msg)
                        <<Keyval("field",field.field)
                        <<Keyval("field2",childfield->field)
                        <<Keyval("comment","With with same name but different type, filtering skipped"));
            } else {
                Ins(&R, filter.body, "if ($Fieldname_PresentQ(parent) && !($Getfield == parent.$Fieldname)) {");
                Ins(&R, filter.body, "    return false;");
                Ins(&R, filter.body, "}");
                if (amc::FField *filterfield = amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.$Fieldname_regx"))) {
                    Ins(&R, filter.body, "if ($Fieldname_regx_PresentQ(parent)) {");
                    Ins(&R, filter.body, "    ch_RemoveAll(fieldstr);");
                    filter.body << "    ";
                    GenPrintStmt(filter.body, *filterfield->p_ctype, *filterfield, "fieldstr", "parent");
                    Ins(&R, filter.body, "    if (!algo_lib::Regx_Match(parent.$Fieldname_regx, fieldstr)) {");
                    Ins(&R, filter.body, "        return false;");
                    Ins(&R, filter.body, "    }");
                    Ins(&R, filter.body, "}");
                }
                if (amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.start_$Fieldname"))) {
                    Ins(&R, filter.body, "if (start_$Fieldname_PresentQ(parent) && $Getfield < parent.start_$Fieldname) {");
                    Ins(&R, filter.body, "    return false;");
                    Ins(&R, filter.body, "}");
                }
                if (amc::ind_field_Find(Subst(R,"$ns.$DispnameFilter.end_$Fieldname"))) {
                    Ins(&R, filter.body, "if (end_$Fieldname_PresentQ(parent) && parent.end_$Fieldname < $Getfield) {");
                    Ins(&R, filter.body, "    return false;");
                    Ins(&R, filter.body, "}");
                }
            }
        } else {
            bool check = true;
            amc::FField *ffield = NULL;
            if (StartsWithQ(name_Get(field), "start_")) {
                strptr fieldname = name_Get(field);
                ffield = amc::ind_field_Find(Subst(R, "$Msgns.$Msgname.")<<RestFrom(fieldname,6));
            } else if (StartsWithQ(name_Get(field), "end_")) {
                strptr fieldname = name_Get(field);
                ffield = amc::ind_field_Find(Subst(R, "$Msgns.$Msgname.")<<RestFrom(fieldname,4));
            } else if(EndsWithQ(name_Get(field), "_regx")) {
                strptr fieldname = name_Get(field);
                ffield = amc::ind_field_Find(Subst(R, "$Msgns.$Msgname.")<<FirstN(fieldname, elems_N(fieldname)-5));
            }
            check &= !ffield;
            if (check) {
                Ins(&R, filter.body, "// if field present in filter but not a member of ctype, return not filtered");
                Ins(&R, filter.body, "if ($Fieldname_PresentQ(parent)) {");
                Ins(&R, filter.body, "    return false;");
                Ins(&R, filter.body, "}");
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Filter_Match(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    amc::FCtype *hdr=dispatch.p_ctype_hdr;
    Set(R, "$Hdrtype" , amc::NsToCpp(hdr->ctype));
    Set(R, "$ns"  , ns_Get(dispatch));
    Set(R, "$typefld", FieldvalExpr(hdr, *hdr->c_typefld->p_field, "msg"));
    Set(R, "$lenexpr" , !dispatch.haslen ? strptr(amc::LengthExpr(*hdr, "msg")) : strptr("msg_len"));
    Set(R, "$lenarg", dispatch.haslen ? ", msg_len" : "");
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$DispnameFilter..Match"));
    func.ret = "bool";
    func.glob = true;
    Ins(&R, func.proto, "$DispnameFilter_Match($ns::$DispnameFilter &parent, $Hdrtype &msg)", false); {
        amc::AddProtoArg(func, "u32", "msg_len", dispatch.haslen);
    }
    Ins(&R, func.body        , "u32 len = $lenexpr;");
    Ins(&R, func.body        , "bool ret = false;");
    Ins(&R, func.body        , "switch($typefld) {" );
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,dispatch) {
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body    , "case $Msgtype: if (sizeof($Ctype) <= len) {");
        Ins(&R, func.body    , "    ret = $ns::$DispnameFilter_Match$Msgname(parent, ($Ctype&)msg$lenarg);");
        Ins(&R, func.body    , "} break;");
    }ind_end;
    Ins(&R, func.body        , "default:");
    Ins(&R, func.body        , "    break;");
    Ins(&R, func.body        , "}");
    Ins(&R, func.body        , "return ret;" );
}

// -----------------------------------------------------------------------------

static void Disp_Filter(amc::FDispatch &dispatch) {
    algo_lib::Replscope R;
    Set(R, "$ns", ns_Get(dispatch));
    Set(R, "$Dispname", name_Get(dispatch));

    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg,dispatch) {
        Set(R,"$Msgns",ns_Get(*dispatch_msg.p_ctype));
        Set(R,"$Msgname",name_Get(*dispatch_msg.p_ctype));
        amc::FFunc &filter = amc::ind_func_GetOrCreate(Subst(R,"$ns.$DispnameFilter..Match$Msgname"));
        filter.glob=true;
        filter.ret ="bool";
        Ins(&R, filter.proto, "$DispnameFilter_Match$Msgname($ns::$DispnameFilter &parent, $Msgns::$Msgname &msg)",false);
        Ins(&R, filter.body, "bool ret = true;");
        Ins(&R, filter.body, "tempstr fieldstr;");
        // loop over fields of message.
        // for each field, if same field present in filter -> compare
        // if _regx present -> regx match
        // if start_ present, end_ present, compare
        if (dispatch.c_dispfilter && !dispatch.c_dispfilter->match_all) {
            Filter_Partial(R, dispatch_msg, filter);
        } else {
            amc::FCtype *ctype = amc::ind_ctype_Find(Subst(R, "$ns.$DispnameFilter"));
            if (ctype) {
                Filter_Full(R, dispatch_msg, filter, *ctype);
            }
        }
        // try other fields
        MaybeUnused(filter,"parent");
        MaybeUnused(filter,"msg");
        Ins(&R, filter.body, "return ret;");
    }ind_end;

    bool all_msgs = true;
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,dispatch) {
        all_msgs &= msg.p_ctype->c_msgtype != NULL;
    }ind_end;

    if (all_msgs) {
        Filter_Match(R, dispatch);
    }
}

// -----------------------------------------------------------------------------

static void AddFilterField(amc::FCtype &ctype, amc::FField &field) {
    amc::FCtype *base = StripWrappers(*field.p_arg,NULL);
    bool string = !base || (base->c_cstr != NULL || base->ctype == "char");

    tempstr key = tempstr()<<ctype.ctype<<"."<<name_Get(field);
    tempstr start_key = tempstr()<<ctype.ctype<<".start_"<<name_Get(field);
    bool key_exists = amc::ind_field_Find(key);
    bool start_key_exists = amc::ind_field_Find(start_key);

    if (!string && !start_key_exists) {//range -- numeric fields only
        amc::InsField(dmmeta::Field(start_key
                                    , field.arg
                                    , field.reftype
                                    , algo::CppExpr()
                                    , algo::Comment()));
        amc::InsField(dmmeta::Field(tempstr()<<ctype.ctype<<".end_"<<name_Get(field)
                                    , field.arg
                                    , field.reftype
                                    , algo::CppExpr()
                                    , algo::Comment()));
    }
    if (!key_exists) {// always match verbatim field
        amc::InsField(dmmeta::Field(key
                                    , field.arg
                                    , field.reftype
                                    , algo::CppExpr()
                                    , algo::Comment()));
    }
    if (!key_exists && string) {// regx
        amc::InsField(dmmeta::Field(tempstr()<<key<<"_regx"
                                    , "algo_lib.Regx"
                                    , "Val"
                                    , algo::CppExpr()
                                    , algo::Comment()));
    }
}

// -----------------------------------------------------------------------------

static void NewField_Filter(amc::FDispatch &dispatch) {
    amc::FCtype &ctype = amc::ind_ctype_GetOrCreate(tempstr() << dispatch.dispatch<<"Filter");
    dmmeta::FieldPkey pmaskname(tempstr()<<ctype.ctype<<".pmask");
    amc::FField &pmask = *amc::InsField(dmmeta::Field(pmaskname, "u64", "Inlary", algo::CppExpr(), algo::Comment()));
    amc::FInlary *inlary = amc::inlary_InsertMaybe(dmmeta::Inlary(pmaskname, 1,1, algo::Comment()));
    amc::pmaskfld_InsertMaybe(dmmeta::Pmaskfld(pmask.field, algo::Comment()));
    amc::cfmt_InsertMaybe(dmmeta::Cfmt(tempstr()<<ctype.ctype<<"."<<dmmeta_Strfmt_strfmt_Tuple
                                       ,dmmeta_Printfmt_printfmt_Auto
                                       , true
                                       ,false
                                       , ""
                                       , true // genop
                                       , algo::Comment()));
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, dispatch) {
        ind_beg(amc::ctype_c_field_curs, field, *dispatch_msg.p_ctype) {
            if (FilterAcceptQ(field)) {
                AddFilterField(ctype,field);
            }
        }ind_end;
    }ind_end;
    if (inlary) {
        int width = (c_field_N(ctype)+63)/64;
        inlary->min = width;
        inlary->max = width;
    }
}

// -----------------------------------------------------------------------------

// Create new fields for dispatch filters.
void amc::Filter_NewField() {
    // Create new ctype for dispfilter.
    ind_beg(amc::_db_dispatch_curs, dispatch, amc::_db) {
        if (dispatch.c_dispfilter) {
            NewField_Filter(dispatch);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::Filter_Gen(amc::FNs &ns) {
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.c_dispfilter) {
            Disp_Filter(disp);
        }
    }ind_end;
}
