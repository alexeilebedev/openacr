// Copyright (C) 2025-2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/disp/kafka_encode.cpp
//

#include "include/amc.h"

void amc::Disp_KafkaEncode(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..KafkaEncode"));
    func.glob = true;
    func.ret = "void";
    Ins(&R, func.comment, "Encode kafka message to BUF.");
    Ins(&R, func.proto, "$Dname_KafkaEncode(algo::ByteAry &buf, $Hdrtype &msg)", false);
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        vrfy(msg.p_ctype->c_msgtype, tempstr()<<"amc.Disp_KafkaEncode  ctype:"<<ctype_Get(msg)<<"  error:'No msgtype defined for ctype'");
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body, "case $Msgtype: {");
        Ins(&R, func.body, "    $Msgname_KafkaEncode(buf,*$Ctype_Castdown(msg),msg.request_api_version);");
        Ins(&R, func.body, "} break;");
        Ins(&R, func.body, "");
    }ind_end;
    Ins(&R, func.body, "}");
}
