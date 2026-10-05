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
// Target: jkv (exe) -- JSON <-> key-value mapping tool
// Exceptions: yes
// Header: include/jkv.h
//

#include "include/gen/jkv_gen.h"
#include "include/gen/jkv_gen.inl.h"

namespace jkv { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/jkv/jkv.cpp
    //
    void PrintKv(lib_json::FNode &node, algo::strptr prefix, cstring &out);
    void SetPath(lib_json::FNode *node, algo::strptr key, lib_json::FNode *val);
    void ApplyKv(lib_json::FNode &node, algo::strptr kv);
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:jkv
}
