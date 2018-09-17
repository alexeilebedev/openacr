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
// Source: cpp/amc/tableid.cpp -- Per-namespace enum of tables
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

bool amc::HasFinputQ(amc::FCtype &ctype) {
    amc::FField *inst = FirstInst(ctype);
    return inst && inst->c_finput;
}

// -----------------------------------------------------------------------------

void amc::GenTableId(amc::FNs &ns) {
    amc::FCtype& table_ctype = amc::ind_ctype_GetOrCreate(tempstr() << ns.ns << ".TableId");
    table_ctype.comment.value = "Index of table in this namespace";

    amc::FCpptype& cpptype = amc::ind_cpptype_GetOrCreate(table_ctype.ctype);
    cpptype.ctor  = true;

    dmmeta::Field field;
    field.field         = tempstr() << table_ctype.ctype << "." << "value";
    field.arg           = "i32";
    field.dflt.value    = "-1";
    field.comment.value = "index of table";
    amc::InsField(field);

    dmmeta::Anonfld anon;
    anon.field = field.field;
    amc::anonfld_InsertMaybe(anon);

    // print function for table enum,
    amc::cfmt_InsertMaybe(dmmeta::Cfmt(tempstr() << table_ctype.ctype << "." << dmmeta_Strfmt_strfmt_String
                                       , dmmeta_Printfmt_printfmt_Raw, true, true, "", true, algo::Comment()));
    amc::fcast_InsertMaybe(dmmeta::Fcast(field.field, "", algo::Comment()));

    int imrowidx = 0;
    ind_beg(amc::ns_c_ctype_curs, ctype,ns) if (HasFinputQ(ctype)) {
        dmmeta::Fconst fconst;
        fconst.value         = dmmeta::CppExpr(tempstr() << imrowidx);

        amc::FCtype *base = GetBaseType(ctype, &ctype); // recognize base, or type itself
        fconst.fconst = tempstr() << table_ctype.ctype << ".value/" << base->ctype;
        fconst.comment.value = tempstr()<<base->ctype<<" -> "<<ctype.ctype;
        amc::fconst_InsertMaybe(fconst);

        if (base->c_ssimfile && base->c_ssimfile->ssimfile != name_Get(fconst)) {
            fconst.fconst = tempstr() << table_ctype.ctype << ".value/" << base->c_ssimfile->ssimfile;
            fconst.comment.value = tempstr()<<base->c_ssimfile->ssimfile<<" -> "<<ctype.ctype;
            amc::fconst_InsertMaybe(fconst);
        }
        imrowidx++;
    }ind_end;
}

// -----------------------------------------------------------------------------

// create TableId type.
// generate an enum representing tables in the in-memory database
void amc::gen_tableenum() {
    ind_beg(amc::_db_ns_curs, ns,amc::_db) {
        if (HasFinputsQ(ns)) {
            GenTableId(ns);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::GenFieldId(amc::FNs &ns) {
    amc::FCtype& field_ctype = amc::ind_ctype_GetOrCreate(tempstr() << ns.ns << ".FieldId");
    field_ctype.comment.value = "Field read helper";

    amc::FCpptype& cpptype = amc::ind_cpptype_GetOrCreate(field_ctype.ctype);
    cpptype.ctor  = true;

    dmmeta::Field newfield;
    newfield.field         = tempstr() << field_ctype.ctype << "." << "value";
    newfield.arg           = "i32";
    newfield.dflt.value    = "-1";
    amc::InsField(newfield);

    dmmeta::Anonfld anon;
    anon.field = newfield.field;
    amc::anonfld_InsertMaybe(anon);

    amc::cfmt_InsertMaybe(dmmeta::Cfmt(tempstr() << field_ctype.ctype << "." << dmmeta_Strfmt_strfmt_String
                                       , dmmeta_Printfmt_printfmt_Raw, true, true, "", true, algo::Comment()));
    amc::fcast_InsertMaybe(dmmeta::Fcast(newfield.field, "", algo::Comment()));

    dmmeta::Pack pack;
    pack.ctype = field_ctype.ctype;
    amc::pack_InsertMaybe(pack);

    int nextidx = 0;
    dmmeta::Fconst fconst;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (HasReadQ(ctype)) {
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            fconst.fconst = tempstr() << field_ctype.ctype << ".value/" << name_Get(field);
            if (!ind_fconst_Find(fconst.fconst)) {
                fconst.value.value = tempstr() << nextidx++;
                (void)amc::fconst_InsertMaybe(fconst);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_fieldid() {
    ind_beg(amc::_db_ns_curs, ns,amc::_db) if (ns.ns != "") {
        GenFieldId(ns);
    }ind_end;
}
