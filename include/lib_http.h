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
// Header: include/lib_http.h
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

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

    // Get single line of HTTP ptotocol
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
