// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/tableid.cpp -- Per-namespace enum of tables
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
    table_ctype.comment = "Index of table in this namespace";

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
        fconst.value         = algo::CppExpr(tempstr() << imrowidx);

        amc::FCtype *base = GetBaseType(ctype, &ctype); // recognize base, or type itself
        fconst.fconst = tempstr() << table_ctype.ctype << ".value/" << base->ctype;
        fconst.comment.value = tempstr()<<base->ctype<<" -> "<<ctype.ctype;
        amc::fconst_InsertMaybe(fconst);

        if (base->c_ssimfile && !algo::strptr_Eq(base->c_ssimfile->ssimfile, name_Get(fconst))) {
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
    field_ctype.comment = "Field read helper";

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
