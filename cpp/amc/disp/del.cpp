// Copyright (C) 2025-2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/disp/del.cpp
//

#include "include/amc.h"

// Generate Dispatch_Delete function
void amc::Disp_Delete(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..Delete"));
    func.glob = true;
    func.ret = "void";
    Ins(&R, func.comment, "Delete message allocated with new.");
    Ins(&R, func.proto, "$Dname_Delete($Hdrtype &msg)", false);
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        vrfy(msg.p_ctype->c_msgtype, tempstr()<<"amc.Disp_Delete  ctype:"<<ctype_Get(msg)<<"  error:'No msgtype defined for ctype'");
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body, "case $Msgtype: delete $Ctype_Castdown(msg); break;");
    }ind_end;
    Ins(&R, func.body, "}");
}
