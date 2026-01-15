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
    // void Main(); // main:jkv
}
