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
// Target: lib_curl (lib) -- covers curl_easy
// Exceptions: yes
// Header: include/lib_curl.h
//

#include "include/gen/lib_curl_gen.h"
#include "include/gen/lib_curl_gen.inl.h"

namespace lib_curl { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_curl.cpp
    //

    // ----------------- Public API -----------------
    // Perform the request synchronously.  Throws on transport failure (refused
    // connection, timeout); returns true once a response arrived, whatever its
    // HTTP status — the caller reads out_resp.code to judge the outcome.
    bool Curl(lib_curl::FRequest &req, lib_curl::FResponse &out_resp);
    tempstr PrintCurlResp(lib_curl::FResponse &resp, bool nodate = false);

    // Start CALL's exchange and return without waiting.  The done hook fires when
    // the response is complete or the transport gave up; that hook reads resp and
    // err and disposes of the row.
    //
    // Until it fires, CALL must stay where it is: curl holds the body by address
    // rather than copying it.
    void CurlBegin(lib_curl::FCall &call);
}
