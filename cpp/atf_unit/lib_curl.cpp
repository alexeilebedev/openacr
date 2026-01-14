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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_curl.cpp
//

#include "include/atf_unit.h"
#include "include/lib_curl.h"

// -----------------------------------------------------------------------------
static void PrintCurlResp(lib_curl::FResponse &resp,strptr test){
    tempstr out(lib_curl::PrintCurlResp(resp,/*nodate*/false));
    tempstr fname("temp/lib_curl_");
    fname<<test;
    StringToFile(out,fname);
    atf_unit::CompareOutput(fname);
}
// -----------------------------------------------------------------------------
void atf_unit::unittest_lib_curl_GET_Echo(){
    lib_curl::FRequest &req=lib_curl::request_Alloc();
    lib_curl::FResponse &resp=lib_curl::response_Alloc();
    req.curlrc_only=true;
    req.url="http://127.0.0.1:8080/echo";
    lib_curl::Curl(req,resp);
    PrintCurlResp(resp,testname_Get(*atf_unit::_db.c_curtest));
}
// -----------------------------------------------------------------------------
void atf_unit::unittest_lib_curl_POST_JSON(){
    lib_curl::FRequest &req=lib_curl::request_Alloc();
    lib_curl::FResponse &resp=lib_curl::response_Alloc();
    req.curlrc_only=true;
    req.url="http://127.0.0.1:8080/echo";
    headers_Alloc(req)<<"Content-Type: application/json";
    headers_Alloc(req)<<"X-Demo: test";
    req.method="POST";
    lib_json::FNode &obj =  lib_json::NewObjectNode(NULL,"");
    lib_json::NewNumberNode(&obj,"x","1");
    lib_json::NewStringNode(&obj,"y","z");
    lib_json::JsonSerialize(&obj,req.body);
    //    req.body="{\"x\":1,\"y\":\"z\"}";
    lib_curl::Curl(req,resp);
    PrintCurlResp(resp,testname_Get(*atf_unit::_db.c_curtest));
}

// -----------------------------------------------------------------------------
void atf_unit::unittest_lib_curl_PUT_PLAINTEXT(){
    lib_curl::FRequest &req=lib_curl::request_Alloc();
    lib_curl::FResponse &resp=lib_curl::response_Alloc();
    req.curlrc_only=true;
    req.url="http://127.0.0.1:8080/echo";
    req.method="PUT";
    req.body="plain text payload";

    lib_curl::Curl(req,resp);
    PrintCurlResp(resp,testname_Get(*atf_unit::_db.c_curtest));
}
// -----------------------------------------------------------------------------
void atf_unit::unittest_lib_curl_STATUS_200(){
    lib_curl::FRequest &req=lib_curl::request_Alloc();
    lib_curl::FResponse &resp=lib_curl::response_Alloc();
    req.curlrc_only=true;
    req.url="http://127.0.0.1:8080/status/200";

    lib_curl::Curl(req,resp);
    PrintCurlResp(resp,testname_Get(*atf_unit::_db.c_curtest));
}
