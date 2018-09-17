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
// Source: cpp/amc/disp/read.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

amc::FCfmt *amc::FindStringRead(amc::FCtype &ctype) {
    amc::FCfmt *ret = NULL;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) {
        if (cfmt.read && strfmt_Get(cfmt) == dmmeta_Strfmt_strfmt_String) {
            ret = &cfmt;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Generate Dispatch_Read function
void amc::Disp_Read(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$Disp", disp.dispatch);
    Set(R, "$ns", ns_Get(disp));
    amc::FCtype &casetype = *disp.p_casetype;
    amc::FField &typefld = *c_field_Find(casetype,0);
    Set(R, "$Casetype", disp.p_casetype->cpp_type);
    Set(R, "$casetypefld", strptr(name_Get(typefld)));
    Set(R, "$caseenumprefix", (tempstr() << ns_Get(casetype) << "_"<< name_Get(casetype)));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$Disp..ReadStrptr"));
    func.ret = Subst(R,"$Casetype");
    func.glob = true;
    Ins(&R, func.comment, "Parse ascii representation of message into binary, appending new data to BUF.");
    Ins(&R, func.proto, "$Dname_ReadStrptr(algo::strptr str, algo::ByteAry &buf)", false);
    Ins(&R, func.body            , "bool ok = false;");
    Ins(&R, func.body            , "tempstr msgtype_str;");
    Ins(&R, func.body            , "StringIter iter(str);");
    Ins(&R, func.body            , "cstring_ReadCmdarg(msgtype_str, iter, false); // read first word");
    Ins(&R, func.body            , "$Casetype msgtype;");
    Ins(&R, func.body            , "$casetypefld_SetStrptrMaybe(msgtype, msgtype_str); // map string -> enum");
    Ins(&R, func.body            , "switch (value_GetEnum(msgtype)) { // what message is it?");

    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        bool is_varlen = VarlenQ(*msg.p_ctype);
        amc::FLenfld *lenfld = msg.p_ctype->c_lenfld;
        Set(R, "$Msgname", name_Get(*msg.p_ctype));
        Set(R, "$msgns", ns_Get(*msg.p_ctype));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body        , "case $caseenumprefix_$msgns_$Msgname: {");
        amc::FCfmt *cfmt = FindStringRead(*msg.p_ctype);
        if (!cfmt) {
            Ins(&R, func.body        , "    // no cfmt read:Y found -- cannot read");
        } else if (cfmt->printfmt == dmmeta_Printfmt_printfmt_Tuple || cfmt->printfmt == dmmeta_Printfmt_printfmt_Extern) {
            Ins(&R, func.body        , "    int len = sizeof($Ctype);");
            Ins(&R, func.body        , "    $Ctype *ctype = new(ary_AllocN(buf, len).elems) $Ctype; // default values");
            Ins(&R, func.body        , "    ok = $Msgname_ReadStrptrMaybe(*ctype, str); // now read attributes");
            if (is_varlen) {
                Ins(&R, func.body    , "    len += ary_N(algo_lib::_db.varlenbuf);");
            }
            if (is_varlen && lenfld) {// for non-varlen, length is already valid
                Set(R, "$assignlen", AssignExpr(*lenfld->p_field, "*ctype", "len", true));
                Ins(&R, func.body    , "    $assignlen;");
            }
            if (is_varlen) {
                Ins(&R, func.body    , "    ary_Addary(buf, ary_Getary(algo_lib::_db.varlenbuf));");
            }
        } else {
            prerr("amc.bad_dispatch_read"
                  <<Keyval("dispatch_msg",msg.dispatch_msg)
                  <<Keyval("comment","Underlying ctype is not printed as a Tuple, so it cannot be read using dispatch"));
            algo_lib::_db.exit_code=1;
        }
        Ins(&R, func.body        , "} break; // $Ctype case");
        Ins(&R, func.body        , "");
    }ind_end;
    Ins(&R, func.body            , "default: break;");
    Ins(&R, func.body            , "}");
    MaybeUnused(func,"buf");
    Ins(&R, func.body            , "return ok ? msgtype : $Casetype();");

    //---
    amc::FFunc &read = amc::ind_func_GetOrCreate(Subst(R, "$Disp..ReadStrptrMaybe"));
    read.ret = Subst(R,"bool");
    read.glob = true;
    Ins(&R, read.comment, "Parse ascii representation of message into binary, appending new data to BUF.");
    Ins(&R, read.proto, "$Dname_ReadStrptrMaybe(algo::strptr str, algo::ByteAry &buf)", false);
    Ins(&R, read.body            , "$Casetype msgtype = $Dname_ReadStrptr(str,buf);");
    Ins(&R, read.body            , "return !(msgtype == $Casetype());");
}
