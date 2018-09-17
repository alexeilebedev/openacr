// (C) 2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/dflt.cpp -- Field default
//

#include "include/amc.h"

static bool UseCdfltQ(dmmeta::ReftypePkey &reftype) {
    return !(reftype == dmmeta_Reftype_reftype_Ptr
             || reftype == dmmeta_Reftype_reftype_Upptr);
}

// -----------------------------------------------------------------------------

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
            field.dflt.value = tempstr() << "sizeof(*this) + (" << lenfld->extra << ")";
        }
    }
}
