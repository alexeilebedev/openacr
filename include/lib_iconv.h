// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: lib_iconv (lib) -- Iconv wrapper
// Exceptions: NO
// Header: include/lib_iconv.h
//

#pragma once
#include "include/algo.h"
#include "include/gen/lib_iconv_gen.h"
#include "include/gen/lib_iconv_gen.inl.h"

namespace lib_iconv {
    // convert multibyte sequence
    tempstr Convert(lib_iconv::Icd &icd, strptr in, bool no_throw = false);
    void Close(lib_iconv::Icd &icd);
    void Open(lib_iconv::Icd& icd, strptr tocode, strptr fromcode);
}
