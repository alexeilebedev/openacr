// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Source: cpp/amc/enum.cpp -- Enumerated types
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// emit constants
// - c++ enums (fconst for integers)
// - extern strings (fconst for non-integers)
void amc::Main_GenEnum(amc::FNs& ns, amc::FCtype &ctype) {
    tempstr text;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (c_fconst_N(field)) {
        // strings do not become enums
        bool is_string = field.p_arg->c_cstr && field.arg != "char";
        if (!is_string) {
            tempstr enum_type(Enumtype(field));
            CppSection(text, enum_type, false);
            text << " \n";
            text << "enum " << enum_type << " {\t\t// " << field.field << "\n";
            strptr sep(" ");
            // If the whole enum is the sequence 0,1,2,..., omit every
            // "= N" suffix and let the C++ compiler auto-number. This
            // keeps adding/removing one entry from renumbering every
            // later constant in the diff. Mix-and-match enums (gaps,
            // duplicates, non-numeric values) print all values
            // explicitly.
            bool all_seq = true;
            {
                i64 expected = 0;
                ind_beg(amc::field_c_fconst_curs, fconst, field) {
                    i64 v = 0;
                    all_seq &= i64_ReadStrptrMaybe(v, fconst.cpp_value) && v == expected;
                    expected++;
                }ind_end;
            }

            ind_beg(amc::field_c_fconst_curs, fconst,field) {
                text << "    " << sep << fconst.cpp_name;
                if (!all_seq) {
                    text << " \t= " << fconst.cpp_value;
                }
                if (ch_N(fconst.comment)) {
                    text << " \t// " << fconst.comment;
                }
                text << "\n";
                sep = ",";
            }ind_end;
            text << "};\n";
            text << "\n";
            // print this if the values are non-consecutive?
            text << "enum { "<<enum_type<<"_N = "<<c_fconst_N(field) << " };\n";
            text << "\n";
        } else {
            ind_beg(amc::field_c_fconst_curs, fconst, field) {
                text << "extern const char *\t";
                text << fconst.cpp_name;
                text << "; \t// "<<fconst.value<<" \t fconst:"<<fconst.fconst << "\n";
            }ind_end;
        }
    }ind_end;
    if (ch_N(text) > 0) {
        *ns.hdr << Tabulated(text, "\t", "lll", 2);
    }
}

// -----------------------------------------------------------------------------

tempstr amc::Enumtype(amc::FField &field) {
    tempstr ret;
    if (field.c_anonfld) {
        ret << NsTo_(ctype_Get(field)) << "Enum";
    } else {
        ret << NsTo_(field.field) << "_Enum";
    }
    return ret;
}
