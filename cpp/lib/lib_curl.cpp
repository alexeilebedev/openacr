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
// Target: lib_curl (lib) -- covers curl_easy
// Exceptions: yes
// Source: cpp/lib/lib_curl.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/lib_curl.h"
#include <curl/curl.h>

// -----------------------------------------------------------------------------
#define curl_vrfy_(res)                                                 \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
    } while (0)

#define curl_vrfy_rc(res, opt, val, strname)                            \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
        strname<<(opt) << " = \"" << (val)<<"\""<<eol;                  \
    } while (0)


// -----------------------------------------------------------------------------
// ---- write callback (accumulate body) ----
static size_t WriteCb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    // auto* out = static_cast<std::string*>(userdata);
    // out->append(ptr, size * nmemb);
    // return size * nmemb;
    vrfy_(userdata);
    lib_curl::FResponse &resp = *(lib_curl::FResponse *)userdata;
    strptr chunk(ptr,size*nmemb);
    resp.body << chunk;
    return size*nmemb;
}
// -----------------------------------------------------------------------------
// Curl header calback function, also called for status line
static size_t HeaderCb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    lib_curl::FResponse &resp = *(lib_curl::FResponse *)userdata;
    strptr chunk(ptr,size*nmemb);
    tempstr header(chunk);
    header = tempstr()<<Trimmed(header);
    if (header!=""){
        // Parse header for status etc
        // Parse reason phrase (HTTP/<v> <code> <reason>)
        headers_Alloc(resp)=header;

        if (algo::StartsWithQ(header, "HTTP/")){
            resp.status_line=header;
            u16_ReadStrptrMaybe(resp.code,Trimmed(Pathcomp(header," LR LL")));
            resp.reason = algo::Trimmed(Pathcomp(header," LR LR"));
        } else if (algo::StartsWithQ(header, "Content-Type")){ // Parse Content-Type: <type>; <??>
            resp.content_type=Trimmed(Pathcomp(header,":LR;LL"));
        }
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------
// Internal: non-throwing attempt that returns CURLcode in out param.
// We keep this internal so HttpJson can still throw on transport errors,
// while retry wrapper can inspect and retry on transport failures.
static CURLcode HttpJsonAttempt(lib_curl::FRequest &req, lib_curl::FResponse &out_resp){
    CURLcode out_cc(CURLE_OK);

    CURL *curl;
    vrfy(curl = curl_easy_init(),"curl_easy_init failed");

    // Set Headers
    struct curl_slist* hdrs=NULL;
    tempstr data_brkt("\"");
    ind_beg(lib_curl::request_headers_curs,header,req){
        if (header=="Content-Type: application/json"){
            data_brkt="";
        }
        hdrs = curl_slist_append(hdrs, Zeroterm(header));
        out_resp.curlrc<<"header = \""<<header<<"\""<<eol;
    }ind_end;

    // Core options
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_URL,Zeroterm(req.url)),"url",req.url,out_resp.curlrc);
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HTTPHEADER,hdrs));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCb));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_resp));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &HeaderCb));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &out_resp));
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_USERAGENT, "lib_curl/1.0"),"user-agent","lib_curl/1.0",out_resp.curlrc);

    // TLS (secure by default)
    i64 host_tls=req.insecure_tls ? 0L : 2L;
    i64 peer_tls=req.insecure_tls ? 0L : 1L;
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, host_tls));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, peer_tls));
    if (req.insecure_tls){
        out_resp.curlrc<<"insecure"<<eol;
    }

    // Timeouts
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, req.connect_timeout_sec),"connect-timeout",req.connect_timeout_sec,out_resp.curlrc);
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_TIMEOUT, req.total_timeout_sec),"max-time",req.total_timeout_sec,out_resp.curlrc);
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L),"keepalive-time", 1, out_resp.curlrc);

    // Method + body
    out_resp.curlrc<<"request = "<<req.method<<eol;
    if (req.method == "GET") {
        // default
    } else if (req.method == "POST") {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        if (req.body.ch_n) {
            out_resp.curlrc<<"data = "<<data_brkt<<req.body<<data_brkt<<eol;
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.ch_elems));
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.body.ch_n));
        }
    } else if (req.method == "PUT" || req.method == "PATCH" || req.method == "DELETE") {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.method));
        if (req.body.ch_n) {
            out_resp.curlrc<<"data = "<<data_brkt<<req.body<<data_brkt<<eol;
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.ch_elems));
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.body.ch_n));
        }
    } else {
        curl_slist_free_all(hdrs);
        curl_easy_cleanup(curl);
        out_cc = CURLE_UNSUPPORTED_PROTOCOL; // sentinel for "bad method" here
        return out_cc;
    }

    // Perform
    if (req.curlrc_only){
        out_cc = CURLE_OK;
    } else {
        out_cc = curl_easy_perform(curl);
        vrfy(out_cc == CURLE_OK,tempstr()
             <<Keyval("error",out_cc)
             <<Keyval("comment",curl_easy_strerror(out_cc))
             );
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &out_resp.code);
    }
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);
    return out_cc;
}
// ----------------- Public API -----------------
bool lib_curl::Curl(lib_curl::FRequest &req, lib_curl::FResponse &out_resp){
    CURLcode out_cc(HttpJsonAttempt(req, out_resp));
    if (out_cc) {
        algo::Throw(curl_easy_strerror(out_cc),(Errcode)out_cc);
    }
    return out_cc;
}

// -----------------------------------------------------------------------------
tempstr lib_curl::PrintCurlResp(lib_curl::FResponse &resp, bool nodate DFLTVAL(false)){
    tempstr out;
    out<<"# "<<Keyval("code",resp.code)<<eol;
    out<<"# "<<Keyval("reason",resp.reason)<<eol;
    out<<"# "<<Keyval("content",resp.content_type)<<eol;
    out<<"# "<<Keyval("body",resp.body)<<eol;
    out<<"# "<<Keyval("status_line",resp.status_line)<<eol;
    ind_beg(lib_curl::response_headers_curs,header,resp) if (!(StartsWithQ(header,"Date:") && nodate)) {
        out<<"# "<<Keyval("header",header)<<eol;
    }ind_end;
    out<<"#---curlrc---"<<eol;
    out<<resp.curlrc<<eol;
    return out;
}
