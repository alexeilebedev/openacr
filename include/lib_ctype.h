// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 Astra
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
// Target: lib_ctype (lib) -- Helpful library for reading/writing ctypes as text tuples
// Exceptions: yes
// Header: include/lib_ctype.h
//

#include "include/gen/lib_ctype_gen.h"
#include "include/gen/lib_ctype_gen.inl.h"

namespace lib_ctype { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_ctype.cpp
    //

    // Find ctype from tuple type tag.
    // Supports ctype and ssimfile lookups.
    lib_ctype::FCtype *TagToCtype(algo::Tuple &tuple);

    // Check if specified attribute matches the default value for the field.
    bool EqualDefaultQ(algo::Attr *attr, lib_ctype::FField &field);

    // Print attributes from TUPLE,
    // which is known to be of ctype CTYPE, to cstring TEXT.
    // Output fields in normalized order, respecting Base.
    // If SKIP_DFLT is true, do not print fields which happen to match their default.
    // Suports Varlen fields.
    // Attributes that are themselves tuples are recursively normalized as well.
    void PrintTupleAttrs(cstring& text, algo::Tuple &tuple, lib_ctype::FCtype &ctype, bool skip_dflt);

    // Normalize a string that's supposed to correspond CTYPE
    // If CTYPE is NULL, it is guessed from the type tag
    // The following actions are done:
    // 1. Print correct type tag back
    // 2. Remove any attributes from tuple that don't correspond to fields of CTYPE
    // 3. Print back any fields in ctype that don't appear in the string
    // 4. Optionally skip printing any field that's already equal to the default value (if SKIP_DFLT is specified)
    // 5. Recursively call NormalizeTuple on any field that has Tuple print format
    // NOTE:
    // Ctype from parent field has been removed, as it is wrong for derived types!
    tempstr NormalizeSsimTuple(strptr str, bool skip_dflt);

    // Retrieve base type for the given ctype
    lib_ctype::FCtype *Basetype(lib_ctype::FCtype &ctype);

    // Find field by name, possibly scanning base fields
    // Skips dot-suffix first, i.e. attr "abc.4" is looked up as "abc" (used for Varlen fields)
    lib_ctype::FField *FindField(lib_ctype::FCtype &ctype, strptr name);

    // Check if field is printed as tuple -- i.e. requires recursive parsing
    bool TupleFieldQ(lib_ctype::FField &field);

    // Check if attribute is a tuple.
    // This is basically the same as TupleFieldQ, but returns false for
    // empty Opt fields.
    bool TupleAttrQ(lib_ctype::FField &field, strptr val);

    // Prepare main schema
    void Init();

    // Find fconst from field and ssim attr value.
    lib_ctype::FFconst *FindFconst(lib_ctype::FField *field, cstring& attr_value);

    // Match Tuple EXPECT against tuple RESULT.
    // EXPECT must be a subset of RESULT for the match to pass.
    // Results of comparison, mostly represented as a match distance, are saved in MATCH.
    // Search exits early if match.distance exceeds match.maxdist.
    void Match_Tuple(lib_ctype::Match &match, Tuple &expect, Tuple &result);

    // Remove unstable fields from a string that's supposed to correspond CTYPE
    tempstr StabilizeSsimTuple(strptr str);

    // Fill Replscope with attribute values, including substr
    void FillReplscope(algo_lib::Replscope &R, strptr str);
}
