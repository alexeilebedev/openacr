// Copyright (C) 2024,2026 AlgoRND
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
// Target: lib_http (lib) -- Library for HTTP support
// Exceptions: yes
// Source: cpp/lib_http/lib_http.cpp
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/algo.h"
#include "include/lib_http.h"

void lib_http::List_curs_Next(List_curs &curs) {
    do {
        int pos = FindChar(curs.rest,',');
        curs.elem = Trimmed(pos < 0 ? curs.rest : FirstN(curs.rest,pos));
        curs.rest = pos < 0 ? strptr() : RestFrom(curs.rest,pos+1);
    } while(!ch_N(curs.elem) && ch_N(curs.rest));
}

void lib_http::List_curs_Reset(List_curs &curs, strptr list) {
    curs.rest = list;
    List_curs_Next(curs);
}

bool lib_http::List_curs_ValidQ(List_curs &curs) {
    return ch_N(curs.elem);
}

strptr &lib_http::List_curs_Access(List_curs &curs) {
    return curs.elem;
}

// Whether http list contains token
bool lib_http::ListContainsQ(strptr list, strptr token) {
    bool ret(false);
    ind_beg(lib_http::List_curs, elem, list) {
        ret = elem == token;
        if (ret) {
            break;
        }
    }ind_end;
    return ret;
}

// Get single line of HTTP ptotocol
bool lib_http::DecodeLine(strptr &buf, strptr &result) {
    i32 ind = FindChar(buf,'\n');
    bool ok = ind >= 0;
    if (ok) {
        result = TrimmedRight(FirstN(buf,ind));
        buf = RestFrom(buf,ind+1);
    }
    return ok;
}

// decode HTTP request
bool lib_http::DecodeRequest(strptr &buf, http::Request &request) {
    strptr line;
    bool ok = lib_http::DecodeLine(buf,line);
    ok = ok && ch_N(line);
    if (ok) {
        algo::StringIter it(line);
        strptr method = GetWordCharf(it);
        if (!http::Method_ReadStrptrMaybe(request.method,method)) {
            request.method = http_Method_extension_method;
            request.extension_method = method;
        }
        if (ok) {
            request.uri = GetWordCharf(it);
            request.version = GetWordCharf(it);
        }
    }
    ok = ok && (request.version == "HTTP/1.0" || request.version == "HTTP/1.1");
    while ((ok = ok && DecodeLine(buf,line)) && ch_N(line)) {
        i32 ind = FindChar(line,':');
        ok = ind > 0;
        if (ok) {
            strptr name = Trimmed(FirstN(line,ind));
            strptr value = Trimmed(RestFrom(line,ind+1));
            ok = SetRequestHeader(request,name,value);
        }
    }
    ok = ok && !ch_N(buf);
    return ok;
}

// Add request header
bool lib_http::SetRequestHeader(http::Request &request, strptr name, strptr value) {
    MakeLower(name);
    http::HeaderType h;
    bool ok = true;
    bool known = HeaderType_ReadStrptrMaybe(h,name);
    if (known) {
        switch (value_GetEnum(h)) {
            // general
        case http_HeaderType_cache_control: request.cache_control = value; break;
        case http_HeaderType_connection: request.connection = value; break;
        case http_HeaderType_date: request.date = value; break;
        case http_HeaderType_pragma: request.pragma = value; break;
        case http_HeaderType_trailer: request.trailer = value; break;
        case http_HeaderType_transfer_encoding: request.transfer_encoding = value; break;
        case http_HeaderType_upgrade: request.upgrade = value; break;
        case http_HeaderType_via: request.via = value; break;
        case http_HeaderType_warning: request.warning = value; break;
            // request
        case http_HeaderType_accept: request.accept = value; break;
        case http_HeaderType_accept_charset: request.accept_charset = value; break;
        case http_HeaderType_accept_encoding: request.accept_encoding = value; break;
        case http_HeaderType_accept_language: request.accept_language = value; break;
        case http_HeaderType_authorization: request.authorization = value; break;
        case http_HeaderType_expect: request.expect = value; break;
        case http_HeaderType_from: request.from = value; break;
        case http_HeaderType_host: request.host = value; break;
        case http_HeaderType_if_match: request.if_match = value; break;
        case http_HeaderType_if_modified_since: request.if_modified_since = value; break;
        case http_HeaderType_if_none_match: request.if_none_match = value; break;
        case http_HeaderType_if_range: request.if_range = value; break;
        case http_HeaderType_if_unmodified_since: request.if_unmodified_since = value; break;
        case http_HeaderType_max_forwards: request.max_forwards = value; break;
        case http_HeaderType_proxy_authorization: request.proxy_authorization = value; break;
        case http_HeaderType_range: request.range = value; break;
        case http_HeaderType_referer: request.referer = value; break;
        case http_HeaderType_te: request.te = value; break;
        case http_HeaderType_user_agent: request.user_agent = value; break;
            // entity
        case http_HeaderType_allow: request.allow = value; break;
        case http_HeaderType_content_encoding: request.content_encoding = value; break;
        case http_HeaderType_content_language: request.content_language = value; break;
        case http_HeaderType_content_length: ok = i32_ReadStrptrMaybe(request.content_length,value); break;
        case http_HeaderType_content_location: request.content_location = value; break;
        case http_HeaderType_content_md5: request.content_md5 = value; break;
        case http_HeaderType_content_range: request.content_range = value; break;
        case http_HeaderType_content_type: request.content_type = value; break;
        case http_HeaderType_expires: request.expires = value; break;
        case http_HeaderType_last_modified: request.last_modified = value; break;
            // websocket
        case http_HeaderType_sec_websocket_key: request.sec_websocket_key = value; break;
        case http_HeaderType_sec_websocket_extensions: request.sec_websocket_extensions = value; break;
        case http_HeaderType_sec_websocket_protocol: request.sec_websocket_protocol = value; break;
        case http_HeaderType_sec_websocket_version: request.sec_websocket_version = value; break;
        default: ok = false; break;
        }
    }
    if (!known) {
        http::ExtensionHeader &eh = extension_header_Alloc(request);
        eh.name = name;
        eh.value = value;
    }
    return ok;
}

// Encode HTTP response
void lib_http::EncodeResponse(cstring &buf, http::Response &response) {
    buf << response.version << " " << response.status;
    buf << "\r\n";

    bool no_body = StartsWithQ(response.status,"1")
        || StartsWithQ(response.status,"204")
        || StartsWithQ(response.status,"304");

    // GENERAL HEADERS

    if (ch_N(response.cache_control)) {
        buf << "Cache-Control: " << response.cache_control << "\r\n";
    }
    if (ch_N(response.connection)) {
        buf << "Connection: " << response.connection << "\r\n";
    }
    if (ch_N(response.date)) {
        buf << "Date: " << response.date << "\r\n";
    }
    if (ch_N(response.pragma)) {
        buf << "Pragma: " << response.pragma << "\r\n";
    }
    if (ch_N(response.trailer)) {
        buf << "Trailer: " << response.trailer << "\r\n";
    }
    if (ch_N(response.transfer_encoding)) {
        buf << "Transfer-Encoding: " << response.transfer_encoding << "\r\n";
    }
    if (ch_N(response.upgrade)) {
        buf << "Upgrade: " << response.upgrade << "\r\n";
    }
    if (ch_N(response.via)) {
        buf << "Via: " << response.via << "\r\n";
    }
    if (ch_N(response.warning)) {
        buf << "Warning: " << response.warning << "\r\n";
    }

    // RESPONSE HEADERS

    if (ch_N(response.accept_ranges)) {
        buf << "Accept-Ranges: " << response.accept_ranges << "\r\n";
    }
    if (ch_N(response.age)) {
        buf << "Age: " << response.age << "\r\n";
    }
    if (ch_N(response.etag)) {
        buf << "ETag: " << response.etag << "\r\n";
    }
    if (ch_N(response.location)) {
        buf << "Location: " << response.location << "\r\n";
    }
    if (ch_N(response.proxy_authenticate)) {
        buf << "Proxy-Authenticate: " << response.proxy_authenticate << "\r\n";
    }
    if (ch_N(response.retry_after)) {
        buf << "Retry-After: " << response.retry_after << "\r\n";
    }
    if (ch_N(response.server)) {
        buf << "Server: " << response.server << "\r\n";
    }
    if (ch_N(response.vary)) {
        buf << "Vary: " << response.vary << "\r\n";
    }
    if (ch_N(response.www_authenticate)) {
        buf << "WWW-Authenticate: " << response.www_authenticate << "\r\n";
    }

    // ENTITY HEADERS

    if (ch_N(response.allow)) {
        buf << "Allow: " << response.allow << "\r\n";
    }
    if (ch_N(response.content_encoding)) {
        buf << "Content-Encoding: " << response.content_encoding << "\r\n";
    }
    if (ch_N(response.content_language)) {
        buf << "Content-Language: " << response.content_language << "\r\n";
    }
    if (!no_body) {
        if (response.content_length) {
            buf << "Content-Length: " << response.content_length << "\r\n";
        } else {
            buf << "Content-Length: " << ch_N(response.body) << "\r\n";
        }
    }
    if (ch_N(response.content_location)) {
        buf << "Content-Location: " << response.content_location << "\r\n";
    }
    if (ch_N(response.content_md5)) {
        buf << "Content-MD5: " << response.content_md5 << "\r\n";
    }
    if (ch_N(response.content_range)) {
        buf << "Content-Range: " << response.content_range << "\r\n";
    }
    if (ch_N(response.content_type)) {
        buf << "Content-Type: " << response.content_type << "\r\n";
    }
    if (ch_N(response.expires)) {
        buf << "Expires: " << response.expires << "\r\n";
    }
    if (ch_N(response.last_modified)) {
        buf << "Last-Modified: " << response.last_modified << "\r\n";
    }

    // WEBSOCKET HEADERS

    if (ch_N(response.sec_websocket_accept)) {
        buf << "Sec-WebSocket-Accept: " << response.sec_websocket_accept << "\r\n";
    }
    if (ch_N(response.sec_websocket_extensions)) {
        buf << "Sec-WebSocket-Extensions: " << response.sec_websocket_extensions << "\r\n";
    }
    if (ch_N(response.sec_websocket_protocol)) {
        buf << "Sec-WebSocket-Protocol: " << response.sec_websocket_protocol << "\r\n";
    }
    if (ch_N(response.sec_websocket_version)) {
        buf << "Sec-WebSocket-Version: " << response.sec_websocket_version << "\r\n";
    }

    // OTHER EXTENSION HEADERS

    ind_beg(http::Response_extension_header_curs,h,response) {
        buf << h.name << ": " << h.value << "\r\n";
    }ind_end;
    buf << "\r\n";

    // BODY

    if (!no_body) {
        buf << response.body;
    }
}

// Get HTTP message length
// TODO content-length, chunked
i32 lib_http::GetMsgLen(strptr buf) {
    i32 len(0);
    i32 prev(0);
    frep_(i,ch_N(buf)) {
        if (buf[i] == '\n') {
            strptr line(buf.elems+prev, i-prev);
            prev = i + 1;
            if (!ch_N(line) || line == "\r") {
                len = prev;
                break;
            }
        }
    }
    return len;
}
