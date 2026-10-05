// Copyright (C) 2024,2026 AlgoX2 Corp
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
// Target: lib_http (lib) -- Library for HTTP support
// Exceptions: yes
// Header: include/lib_http.h
//

#include "include/gen/lib_http_gen.h"
#include "include/gen/lib_http_gen.inl.h"
#include "include/gen/http_gen.h"
#include "include/gen/http_gen.inl.h"

namespace lib_http {
    // comma-separated token list (with optional whitespace around commas)
    struct List_curs {
        typedef strptr ChildType ;
        strptr elem;
        strptr rest;
    };
}

namespace lib_http { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_http/lib_http.cpp
    //
    void List_curs_Next(List_curs &curs);
    void List_curs_Reset(List_curs &curs, strptr list);
    bool List_curs_ValidQ(List_curs &curs);
    strptr &List_curs_Access(List_curs &curs);

    // Whether http list contains token
    bool ListContainsQ(strptr list, strptr token);

    // Get a single line of the HTTP head, ending at the next LF.  A bare LF is
    // accepted as a line terminator with any preceding CR ignored, which RFC 7230
    // 3.5 permits a recipient to do; TrimmedRight strips the CR so a CRLF and a bare
    // LF decode to the same line.
    bool DecodeLine(strptr &buf, strptr &result);

    // decode HTTP request
    bool DecodeRequest(strptr &buf, http::Request &request);

    // Add request header
    bool SetRequestHeader(http::Request &request, strptr name, strptr value);

    // Encode HTTP response
    void EncodeResponse(cstring &buf, http::Response &response);

    // Get HTTP message length
    // TODO content-length, chunked
    i32 GetMsgLen(strptr buf);
}
