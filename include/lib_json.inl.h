// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2014-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: lib_json (lib) -- Full json support library
// Exceptions: NO
// Header: include/lib_json.inl.h
//
// JSON library

#pragma once

// Check for parse error
//
// PARSER    parser handle
// RETURN    true for OK, false for parse error
inline bool lib_json::JsonParseOkQ(lib_json::FParser &parser) {
    return parser.state != lib_json_FParser_state_err;
}
