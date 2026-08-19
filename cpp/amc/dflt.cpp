// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/dflt.cpp -- Field default
//

#include "include/amc.h"

static bool UseCdfltQ(dmmeta::ReftypePkey &reftype) {
    return !(reftype == dmmeta_Reftype_reftype_Ptr
             || reftype == dmmeta_Reftype_reftype_Upptr
             || reftype == dmmeta_Reftype_reftype_Varlen
             || reftype == dmmeta_Reftype_reftype_Opt);
}

// -----------------------------------------------------------------------------

// Compute the C++ expression for FIELD's default value: the field's own
// dflt, or, for a reftype that stores the value inline, the value type's
// cdflt row. A cextern type initialized by memset gets 0 in place of an
// empty default. An empty result means the field has no default; print
// emitters compare against the expression to skip default-valued fields.
tempstr amc::DfltExprVal(amc::FField &field) {
    tempstr dflt;
    dflt << field.dflt;
    if (UseCdfltQ(field.reftype)) {
        if (ch_N(dflt) == 0 && field.p_arg->c_cdflt) {
            dflt = field.p_arg->c_cdflt->dflt.value;
        }
    }
    // pick a default for memset
    if (field.p_arg->c_cextern && field.p_arg->c_cextern->initmemset && !ch_N(dflt)) {
        dflt = "0";
    }
    return dflt;
}

// -----------------------------------------------------------------------------

// Compute the initialization expression for a bitfield: the field's dflt,
// squashed to empty when it is a no-op (bool false, or 0 when the value
// type's cdflt is already 0). The source field's initializer zeroes every
// bit, so an empty result tells the bitfield Init generator to emit no
// initialization step of its own.
tempstr amc::DfltExprBitfld(amc::FField &field) {
    tempstr dflt;
    dflt << field.dflt;
    if (field.arg == "bool" && dflt == "false") {
        dflt="";// squash no-op default
    }
    if (field.p_arg->c_cdflt && field.p_arg->c_cdflt->dflt.value=="0" && dflt == "0") {
        dflt="";// squash no-op default
    }
    return dflt;
}

// -----------------------------------------------------------------------------

// Retarget the instance reference in DFLT, a field-default C++ expression:
// a default may name the containing instance as *this (Field_UpdateDflt's
// message-length default ssizeof(*this)), and an emitter that places the
// expression where the instance is named differently -- the Init function's
// parent parameter, a pool row, _db -- substitutes PAREXPR for the
// reference. Each reference is substituted as a whole identifier, so a
// longer identifier that merely starts with the same characters
// (*thisvalue) is not an instance reference and survives verbatim, even
// when the same expression also carries a real reference.
// C++ spells the implicit object two ways, and a default may use either:
// *this is the object, this is a pointer to it. The pointer spelling
// substitutes the address of PAREXPR, parenthesized, so that the member
// access in this->len binds to the whole address expression rather than to
// the member (&parent->len would take the address of the member). Both
// passes run: a reference left in either spelling would reach a generated
// non-member function, where neither name is declared.
void amc::DfltRetarget(cstring &dflt, strptr parexpr) {
    tempstr ptrexpr;
    ptrexpr << "(&" << parexpr << ")";
    algo::ReplaceIdent(dflt, "*this", parexpr);
    algo::ReplaceIdent(dflt, "this", ptrexpr);
}

// -----------------------------------------------------------------------------

// Compute the field's effective default (field.dflt) from the schema:
// a field with no explicit default inherits its value type's cdflt;
// a message's type field defaults to the msgtype constant, and its length
// field to the message-length expression. This keeps generated initializers
// consistent with the schema: a freshly constructed message identifies its
// own type and length without caller involvement.
void amc::Field_UpdateDflt(amc::FField &field) {
    amc::FCtype& valtype = *(field).p_arg;
    if (UseCdfltQ(field.reftype)) {
        if (valtype.c_cdflt && !FldfuncQ(field) && !ch_N(field.dflt.value)) {
            field.dflt = valtype.c_cdflt->dflt;
        }
    }
    if (field.c_typefld && field.p_ctype->c_msgtype) {
        field.dflt.value = field.p_ctype->c_msgtype->type.value;
    }
    if (amc::FLenfld *lenfld = GetLenfld(field)) {
        if (field.p_ctype->c_msgtype) {
            // the default names the instance as *this; emitters that place it
            // in a non-member context retarget the reference textually (val.cpp)
            field.dflt.value = LenfldStoreExpr(*lenfld, "ssizeof(*this)");
        }
    }
}
