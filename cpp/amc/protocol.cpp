// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/amc/protocol.cpp -- Protocol functions
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Ns() {
}

// -----------------------------------------------------------------------------

static void SizeCheck(algo_lib::Replscope &R, amc::FNs &ns, amc::FFunc &chk) {
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
        if ((ns.ns=="" ? ctype.c_bltin!=NULL : true) && ctype.c_csize) {// check csize
            Set(R,"$size",tempstr()<<ctype.c_csize->size);
            Set(R, "$Type", ctype.cpp_type);
            Set(R, "$Ctype", ctype.ctype);
            Ins(&R, chk.body, "algo_assert(sizeof($Type) == $size); // csize:$Ctype");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void OffsetCheck(algo_lib::Replscope &R, amc::FField &field, int field_idx, cstring &text) {
    amc::FCtype &ctype=*field.p_ctype;
    int reqd_bit_width = 0;
    ind_beg(amc::field_bh_bitfld_curs, sf, field) {
        vrfy(sf.width,tempstr()<< "Subfield '"
             <<  sf.srcfield << "." << name_Get(sf) << "' is zero-length");
        reqd_bit_width = i32_Max(reqd_bit_width, sf.offset + sf.width);
    }ind_end;
    bool fixwidth = WidthMax(field) == WidthMin(field);
    if (ValQ(field) && fixwidth && !field.c_smallstr && !field.c_tary) {
        if (reqd_bit_width) {
            Set(R, "$Type", amc::NsToCpp(ctype.ctype));
            Set(R, "$fld", name_Get(field));
            Set(R, "$suffix",field.c_fbigend?"_be":"");
            Set(R, "$bitwidth", tempstr() << reqd_bit_width);
            Ins(&R, text, "// check that bitfield fits width");
            Ins(&R, text, "algo_assert(sizeof((($Type*)0)->$fld$suffix)*8 >= $bitwidth);");
        }
        bool is_val = !field.c_inlary && !ctype_zd_varlenfld_InLlistQ(field);
        bool needcheck = field_idx == c_datafld_N(ctype)-1
            && ctype.c_pack
            && name_Get(field).n_ch > 0
            && !FirstInst(ctype)
            && is_val;
        if (needcheck) {
            Set(R, "$Type", amc::NsToCpp(ctype.ctype));
            Set(R, "$fld", name_Get(field));
            Set(R, "$suffix",field.c_fbigend?"_be":"");
            Ins(&R, text, "algo_assert(_offset_of($Type, $fld$suffix) "
                "+ sizeof((($Type*)0)->$fld$suffix) == sizeof($Type));");
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Ns_StaticCheck() {
    amc::FNs &ns = *amc::_db.genctx.p_ns;
    if (ns.nstype == dmmeta_Nstype_nstype_protocol || ns.c_globfld) {
        algo_lib::Replscope R;
        Set(R, "$ns", ns.ns);
        amc::FFunc& chk = amc::CreateCurFunc();
        Ins(&R, chk.ret, "void",false);
        Ins(&R, chk.proto, "StaticCheck()",false);
        SizeCheck(R,ns,chk);
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            int field_idx = 0;
            ind_beg(amc::ctype_c_field_curs, field,ctype) if (!FldfuncQ(field)) {
                OffsetCheck(R,field,field_idx,chk.body);
                if (!field.c_bitfld) {
                    field_idx++;
                }
            }ind_end;
            ind_beg(amc::ctype_c_field_curs, field,ctype) if (field.c_fldoffset) {
                if (name_Get(field).n_ch > 0 && !FixaryQ(field) && !ctype_zd_varlenfld_InLlistQ(field)) {
                    Set(R, "$Type", amc::NsToCpp(ctype_Get(field)));
                    Set(R, "$offset", tempstr() << field.c_fldoffset->offset);
                    Set(R, "$fld", name_Get(field));
                    Set(R, "$suffix",field.c_fbigend?"_be":"");
                    Ins(&R, chk.body, "algo_assert(_offset_of($Type, $fld$suffix) == $offset);");
                }
            }ind_end;
        }ind_end;
    }
}
