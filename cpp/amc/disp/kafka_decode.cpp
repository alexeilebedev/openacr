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
// Source: cpp/amc/disp/kafka_decode.cpp
//

#include "include/amc.h"

void amc::Disp_KafkaDecode(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..KafkaDecode"));
    func.glob = true;
    func.ret = "void";
    Ins(&R, func.comment, "Decode kafka message to BUF.");
    Ins(&R, func.proto, "$Dname_KafkaDecode()", false);
    AddRetval(func, Subst(R,"$Hdrtype *"), "ret", "NULL");
    AddProtoArg(func, Refto("algo::memptr"), "buf");
    bool req = name_Get(*disp.p_ctype_hdr) == "RequestHeader";
    if (req) {
        Ins(&R, func.body , "algo::memptr peek(buf);");
        Ins(&R, func.body , "i16 request_api_key, request_api_version;");
        Ins(&R, func.body , "bool ok = lib_kafka::DecodeI16(peek,request_api_key) && lib_kafka::DecodeI16(peek,request_api_version);");
        Ins(&R, func.body , "if (ok) {");
    } else {
        AddProtoArg(func, "i16", "request_api_key");
        AddProtoArg(func, "i16", "request_api_version");
        Ins(&R, func.body , "bool ok(true);");
    }
    AddProtoArg(func, Refto("algo::cstring"), "err");
    Ins(&R, func.body, "switch(request_api_key) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        vrfy(msg.p_ctype->c_msgtype, tempstr()<<"amc.Disp_KafkaDecode  ctype:"<<ctype_Get(msg)<<"  error:'No msgtype defined for ctype'");
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body, "case $Msgtype: {");
        Ins(&R, func.body , "$Ctype &msg = *new $Ctype;");
        Ins(&R, func.body , "ok = $Msgname_KafkaDecode(buf,msg,request_api_version);");
        Ins(&R, func.body , "if (ok) {");
        Ins(&R, func.body , "ret = &Castbase(msg);");
        Ins(&R, func.body , "} else {");
        Ins(&R, func.body , "err << \"$Dname_KafkaDecode(): Failed to decode $Msgname, decoded portion and rest bytes follow\" << eol;");
        Ins(&R, func.body , "$Msgname_Print(msg,err);");
        Ins(&R, func.body , "err << eol << \"rest bytes: \" << buf << eol;");
        Ins(&R, func.body , "delete &msg;");
        Ins(&R, func.body , "}");
        Ins(&R, func.body , "} break;");
        Ins(&R, func.body , "");
    }ind_end;
    //Ins(&R, func.body, "}");
    Ins(&R, func.body , "default: {");
    Ins(&R, func.body , "ok = false;");
    Ins(&R, func.body , "err << \"$Dname_KafkaDecode(): bad API key \" << request_api_key << eol;");
    Ins(&R, func.body , "} break;");
    Ins(&R, func.body , "}");
    if (req) {
        Ins(&R, func.body , "}");
    }
}
