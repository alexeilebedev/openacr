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
    bool Curl(lib_curl::FRequest &req, lib_curl::FResponse &out_resp);
    tempstr PrintCurlResp(lib_curl::FResponse &resp, bool nodate = false);
}
