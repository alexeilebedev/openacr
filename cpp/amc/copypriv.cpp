// (C) 2017-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/copypriv.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void SetCopyPriv(amc::FCtype &ctype, strptr reason) {
    ctype.copy_priv_reason << reason << eol;
    ctype.copy_priv = true;
}

// -----------------------------------------------------------------------------

// Calculate value of copy_priv and return it.
// This function can only be called once; addition of new fields after calling
// this function is not supported.
bool amc::CopyPrivQ(amc::FCtype &ctype) {
    if (bool_Update(ctype.copy_priv_valid,true)) {
        if (ctype.ctype == "algo.cstring") {
            ctype.copy_priv=false;
        } else {
            ind_beg(amc::ctype_c_field_curs, field, ctype) {
                if (field.p_reftype->isval && CopyPrivQ(*field.p_arg)) {
                    SetCopyPriv(ctype, tempstr()<<"value field "<<field.field<<" is not copiable");
                }
                if (!field.p_reftype->cancopy && ctype_Get(field) != "algo.cstring") {
                    SetCopyPriv(ctype, tempstr()<<"reftype of "<<field.field<<" prohibits copy");
                }
                if (field.c_xref != NULL) {
                    SetCopyPriv(ctype, tempstr()<<"x-reference on "<<field.field<<" prevents copy");
                }
                if (field.c_cascdel != NULL) {
                    SetCopyPriv(ctype, tempstr()<<"cascdel on "<<field.field<<" prevents copy");
                }
                if (field.c_fcleanup != NULL) {
                    SetCopyPriv(ctype, tempstr()<<"user-defined fcleanup on "<<field.field<<" prevents copy");
                }
            }ind_end;
        }
    }
    return ctype.copy_priv;
}
