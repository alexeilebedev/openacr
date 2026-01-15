// Copyright (C) 2023-2026 AlgoRND
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
// Target: gcli (exe) -- Gcli - gitlab/github command-line client
// Exceptions: yes
// Source: cpp/gcli/rest.cpp
//

#include "include/algo.h"
#include "include/gcli.h"
#include <curl/curl.h>

// -----------------------------------------------------------------------------
tempstr gcli::Urlencode(strptr str_in){
    return str_in.n_elems == 0 ? tempstr() : tempstr()<<curl_easy_escape(NULL, str_in.elems, str_in.n_elems);
}
// -----------------------------------------------------------------------------
static void FormTuple(gcli::FGclicmd &gclicmd){
    // form a tuple for all matched jkeys
    ind_beg(gcli::gclicmd_c_gclicmdc_curs,gclicmdc,gclicmd) {
        gcli::FTuples &tuple=gcli::tuples_Alloc();
        tuple.tuples=ctype_Get(gclicmdc);
        c_tuples_Insert(gclicmd,tuple);

        ind_beg(gcli::gclicmdc_c_gclicmdf_curs,gclicmdf,gclicmdc) {
            tuple.tuples<<Keyval(name_Get(gclicmdf),gclicmdf.uval);
            gclicmdf.uval=""; //clear all values
        }ind_end;
    }ind_end;

    gclicmd.select=false;
    ind_beg(gcli::_db_gclicmdj2f_curs,gclicmdj2f,gcli::_db) {
        gclicmdj2f.select=false;
    }ind_end;
}
// -----------------------------------------------------------------------------
static void JsonJkeyVal(gcli::FGclicmd &gclicmd, cstring &jkey, strptr val){
    // insert value if the key is specified
    prcat(debug,Keyval(jkey,val));
    if (val!=""){
        tempstr gclicmdj2f_key;
        gclicmdj2f_key=gcli::FGclicmdj2f_Concat_gclicmd_jkey(gclicmd.gclicmd,jkey);
        if (gcli::FGclicmdj2f *gclicmdj2f=gcli::ind_gclicmdj2f_Find(gclicmdj2f_key)){
            // trigger stanza processing if the value repeats
            if (gclicmdj2f->select) {
                prcat(debug,"TUPLE");
                FormTuple(gclicmd);
            }
            // Fill all fields matching jkey with its value
            gclicmdj2f->select=!gclicmdj2f->dup;
            ind_beg(gcli::gclicmdj2f_c_gclicmdf_curs,gclicmdf,*gclicmdj2f){
                if (gclicmdj2f->select){
                    gclicmdf.uval=Trimmed(val);
                }else {
                    if (gclicmdf.uval!=""){
                        gclicmdf.uval<<",";
                    }
                    gclicmdf.uval<<Trimmed(val);
                }
                gclicmd.select=true;
                prcat(debug,Keyval(gclicmdf.gclicmdf,val));
            }ind_end;
        }
    }
}
// -----------------------------------------------------------------------------
// Serialize to (jkey,val) pairs
static void JsonTuple(gcli::FGclicmd &gclicmd, lib_json::FNode* node, cstring &jkey) {
    if (node) {
        switch (node->type) {
            //        case lib_json_FNode_type_null  :    JsonJkeyVal(gclicmd, jkey,"null","STATE1");   break;
        case lib_json_FNode_type_null  :    JsonJkeyVal(gclicmd, jkey,"");   break;
        case lib_json_FNode_type_true  :    JsonJkeyVal(gclicmd, jkey,"true");    break;
        case lib_json_FNode_type_false :    JsonJkeyVal(gclicmd, jkey,"false");     break;
        case lib_json_FNode_type_string:    JsonJkeyVal(gclicmd, jkey,node->value);  break;
        case lib_json_FNode_type_number:    JsonJkeyVal(gclicmd, jkey,node->value);  break;

        case lib_json_FNode_type_field : {
            // silently omit malformed fields
            if (c_child_N(*node)==1) {
                JsonTuple(gclicmd,c_child_Find(*node,0),jkey);
            }
        } break;

        case lib_json_FNode_type_array:
        case lib_json_FNode_type_object: {
            bool obj=node->type==lib_json_FNode_type_object;
            ind_beg(lib_json::node_c_child_curs, child, *node) {
                if (obj){ // extend the key
                    if (jkey!=""){
                        jkey<<".";
                    }
                    jkey<<child.value;
                }
                JsonTuple(gclicmd,&child,jkey);
                // Shift the key
                if (obj){ // reduce the key
                    jkey=tempstr()<<Pathcomp(jkey,".RL");
                };
            }ind_end;
            JsonJkeyVal(gclicmd,jkey,node->value);
        } break;
        default:
            vrfy_(0);
        }
    }

}
// -----------------------------------------------------------------------------

#define curl_vrfy_(res)                                                 \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
    } while (0)
// -----------------------------------------------------------------------------

// Curl write callback function
static size_t CurlWrite(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gcli::FHttp &http = *(gcli::FHttp *)userdata;
    strptr chunk(ptr,size*nmemb);
    if (http.response_content_type_json) {
        JsonParse(http.response_json_parser,chunk);
    } else {
        http.response_body << chunk;
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------

// Curl header calback function, also called for status line
static size_t CurlHeader(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gcli::FHttp &http = *(gcli::FHttp *)userdata;
    strptr header(ptr,size*nmemb);
    if (!http.response_status_line.ch_n || http.response_status_code == 100) {
        // store status line
        http.response_status_line = Trimmed(header);
        // parse status line
        algo::StringIter it(http.response_status_line);
        GetWordCharf(it); //skip HTTP-Version
        TryParseU32(it,http.response_status_code); // Status-Code
        http.response_reason_phrase = Trimmed(it.Rest());// Reason-Phrase
    } else {
        // add header
        response_header_Alloc(http) = Trimmed(header);
        // check content-type
        strptr name = Trimmed(Pathcomp(header,":LL"));
        strptr lr = Trimmed(Pathcomp(header,":LR"));
        strptr value = Trimmed(Pathcomp(lr,";LL"));
        if (!CompareNoCase(name,"Content-Type")) {
            http.response_content_type = value;
            http.response_content_type_json = !CompareNoCase(value,"application/json");
        }
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------

// Curl header calback function, also called for status line
static size_t CurlRead(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gcli::FHttp &http = *(gcli::FHttp *)userdata;
    u32 requested_bytes = size*nmemb;
    u32 available_bytes = http.request_body.ch_n-http.request_body_sent_bytes;
    u32 returned_bytes  = u32_Min(available_bytes,requested_bytes);
    memcpy(ptr,http.request_body.ch_elems+http.request_body_sent_bytes,returned_bytes);
    http.request_body_sent_bytes+=returned_bytes;
    return returned_bytes;
}
// -----------------------------------------------------------------------------
static bool GetResponseHeaderNext(gcli::FHttp &http){
    bool ret(false);
    algo_lib::Regx more_records;
    Regx_ReadSql(more_records,"rel=\"next\"", false);
    ind_beg(gcli::FHttp_response_header_curs,hdr,http) {
        ret=algo_lib::Regx_Match(more_records,hdr);
        if (ret){
            break;
        }
    }ind_end;
    return ret;
}
// -----------------------------------------------------------------------------
static bool CurlExecPage(gcli::FGclicmd &gclicmd, strptr per_page, strptr page) {
    gcli::FHttp http;
    http.request_uri=tempstr()
        <<gclicmd.url
        <<gclicmd.sep
        <<"per_page="<<per_page
        <<"&"
        <<"page="<<page;
    verblog(Keyval("URL",http.request_uri));
    http.request_method=gclicmd.method;
    verblog(Keyval("X",gcli::request_method_ToCstr(http)));
    http.request_body=gclicmd.body;
    verblog(Keyval("Body",http.request_body));
    // Collect headers
    curl_slist *hdr(NULL);
    ind_beg(gcli::gtype_c_gtypeh_curs,gtypeh,*gcli::_db.p_gtype){
        vrfy_(hdr=curl_slist_append(hdr,Zeroterm(gtypeh.header)));
        verblog(Keyval("H",gtypeh.header));
    }ind_end;

    CURL *curl;
    vrfy_(curl = curl_easy_init());

    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HTTPHEADER,hdr));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_URL, Zeroterm(http.request_uri)));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L));
    if (algo_lib::_db.cmdline.debug) {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L));
    }
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &http));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CurlHeader));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlRead));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_READDATA, &http));
    switch (http.request_method) {
    case gcli_FHttp_request_method_GET:
        // this is the default
        break;
    case gcli_FHttp_request_method_POST:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        break;
    case gcli_FHttp_request_method_PUT:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L));
        break;
    case gcli_FHttp_request_method_PATCH:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"));
        break;
    case gcli_FHttp_request_method_DELETE:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"));
        break;
    default:
        vrfy(0,tempstr()<<"Unsupported HTTP method: " << http.request_method);
    }
    bool ret(false);
    if (!gcli::_db.cmdline.dry_run){
        curl_vrfy_(curl_easy_perform(curl));

        curl_slist_free_all(hdr);
        curl_easy_cleanup(curl);

        // prechecks on output
        // print debug output
        if (algo_lib::_db.cmdline.debug) {
            // params
            prcat(debug,http);
            // headers
            ind_beg(gcli::FHttp_response_header_curs,rh,http){
                prcat(debug,rh);
            }ind_end;
            // json
            if (http.response_json_parser.root_node) {
                cstring out;
                JsonSerialize(http.response_json_parser.root_node,out,true);
                prcat(debug,out);
            }
        }
        bool ok=http.response_status_code == 200 || http.response_status_code == 201;
        cstring out;
        cstring reason("NA");
        if (!ok){
            if (http.response_json_parser.root_node){
                out=strptr_Get(http.response_json_parser.root_node,"message");
                if (lib_json::FNode *error_ary=lib_json::node_GetArray(http.response_json_parser.root_node, "errors")){
                    ind_beg(lib_json::node_c_child_curs, child, *error_ary){
                        reason=strptr_Get(&child,"message");
                    }ind_end;
                }
            }
            if (out==""){
                out="request failed, repeat with -debug for diagnostics";
            }
        }
        vrfy(ok,tempstr()
             <<Keyval("status",http.response_status_line)
             <<Keyval("message",out)
             <<Keyval("reason",reason)
             );
        // process collected json
        if (http.response_json_parser.root_node) {
            cstring key;
            JsonTuple(gclicmd,http.response_json_parser.root_node,key);
            // process last stanza
            if (gclicmd.select){
                FormTuple(gclicmd);
            }
        } else {
            prlog(http.response_body);
        }
        ret=GetResponseHeaderNext(http);
    }
    return ret;
}
// -----------------------------------------------------------------------------
static void SetGclicmd(gcli::FGclicmd &gclicmd){
    gclicmd.gclicmdt_key=
        gclidb::Gclicmdt_Concat_gclicmd_gtype(gclicmd.gclicmd,gcli::_db.p_gtype->gtype);

    tempstr gmethod_key;
    if (gcli::FGclicmdt *gclicmdt=gcli::ind_gclicmdt_Find(gclicmd.gclicmdt_key)){
        // Set url
        //        gclicmd.url<<gcli::_db.p_gtype->default_url<<"/"<<gclicmdt->uri;
        // Check if local ip is called for

        gclicmd.host=host_Get(gcli::_db.grepo_sel);

        gclicmd.token=gcli::_db.grepo_sel.token;
        gclicmd.url=tempstr()<<gclicmd.host<<"/"<<gclicmdt->uri;
        gmethod_key=gclicmdt->gmethod;
    }
    // Set name/proj
    gclicmd.proj=gcli::_db.grepo_sel.name;
    // Set separator
    gclicmd.sep="?";
    if (Replace(gclicmd.url,gclicmd.sep,gclicmd.sep)){
        gclicmd.sep="&";
    }

    //    tempstr gmethod_key(gclicmd.gmethod);
    if (gcli::FGmethod *gmethod=gcli::ind_gmethod_Find(gmethod_key)){
        // Set url
        gclicmd.method=gmethod->val;
    }
    Replace(gclicmd.url,"$KEYID",gcli::_db.grepo_sel.keyid);
    Replace(gclicmd.url,"$ISSUE",gclicmd.issue_id);
    Replace(gclicmd.url,"$NOTE",gclicmd.note_id);
    Replace(gclicmd.url,"$COND",gclicmd.cond);
    // Replace(gclicmd.url,"$STATE",gcli::_db.cmdline.showclosed ?
    //         gcli::Gstate(gclidb_Gstate_gstate_state_closed) : gcli::Gstate(gclidb_Gstate_gstate_state_opened));
}
// -----------------------------------------------------------------------------
static bool CurlExecPageTry(gcli::FGclicmd &gclicmd, strptr per_page, strptr page){
    bool ret(false);
    u32 try_cnt(3);
    do {
        try {
            ret=CurlExecPage(gclicmd,per_page,page);
            break;
        } catch (algo_lib::ErrorX &x) {
            prlog("gcli.error  "<<x.str);
            sleep(2);
        }
    }while (--try_cnt!=0);
    return ret;
}
// -----------------------------------------------------------------------------
void gcli::CurlExec(gcli::FGclicmd &gclicmd) {
    ind_beg(gcli::gclicmd_c_gclicmdarg_curs,gclicmdarg,gclicmd){
        // collect tuples from json produced by rest API
        gclicmd.issue_id=arg_Get(gclicmdarg);
        SetGclicmd(gclicmd);

        u32 page(1);
        u32 per_page(100);
        bool more(true);
        while (more){
            more=CurlExecPageTry(gclicmd,tempstr()<<per_page,tempstr()<<page);
            page++;
        }
        // process collected tuples
        gclicmd.step(gclicmd);
        // Clear the tuples
        c_tuples_RemoveAll(gclicmd);
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::Main_CurlExec(){
    // Execute gclicmd
    ind_beg(gcli::_db_c_gclicmd_curs,gclicmd,gcli::_db){
        gcli::CurlExec(gclicmd);
    }ind_end;
    c_gclicmd_RemoveAll();
}
