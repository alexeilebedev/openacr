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
// Source: cpp/amc/disp/print.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Generate Dispatch_Print function
void amc::Disp_Print(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..Print"));
    func.glob = true;
    func.ret = "bool";
    Ins(&R, func.comment, "Print message to STR. If message is too short for MSG_LEN, print nothing.");
    Ins(&R, func.comment, "MSG.LENGTH must have already been validated against msg_len.");
    Ins(&R, func.comment, "This function will additionally validate that sizeof(Msg) <= msg_len");
    Ins(&R, func.proto, "$Dname_Print(algo::cstring &str, $Hdrtype &msg, u32 msg_len)", false);
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        vrfy(msg.p_ctype->c_msgtype, tempstr()<<"amc.Disp_Print  ctype:"<<ctype_Get(msg)<<"  error:'No msgtype defined for ctype'");
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body, "case $Msgtype: {");
        Ins(&R, func.body, "    if (sizeof($Ctype) > msg_len) { return false; }");
        Ins(&R, func.body, "    $Msgname_Print(($Ctype&)(msg), str);");
        Ins(&R, func.body, "    return true;");
        Ins(&R, func.body, "}");
    }ind_end;
    Ins(&R, func.body, "default:\n");
    Ins(&R, func.body, "    return false;");
    Ins(&R, func.body, "}");
}
