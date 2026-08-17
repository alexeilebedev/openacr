// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/gsymbol.cpp -- Generate strings from tables
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static tempstr ResolveGsymboltype(amc::FGsymbol &gsymbol) {
    tempstr ret;
    if (ch_N(gsymbol.symboltype)) {
        ret << amc::NsToCpp(gsymbol.p_symboltype->ctype);
    } else {
        ret << amc::NsToCpp(gsymbol.p_ssimfile->p_ctype->ctype)<<"Pkey";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Generate regular c++ symbols from tables
void amc::gen_ns_gsymbol() {
    amc::FNs &ns =*amc::_db.c_ns;
    amc::_db.genctx.p_field = NULL;
    ind_beg(amc::ns_c_gsymbol_curs, gsymbol,ns) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx, gsymbol.inc, true);
        algo_lib::MmapFile file;
        tempstr fname(SsimFname(DataRoot(),ssimfile_Get(gsymbol)));
        // The table's rows are the symbols this namespace exports, so a table
        // that cannot be read would emit an empty symbol block and exit 0,
        // leaving every reference to those symbols unresolved at link time.
        // Report the file, fail the run, and skip this gsymbol, so one run
        // still reaches every other one.
        if (!SideloadFile(file,fname)) {
            algo::PrerrFileFail("amc.load", fname, "gsymbol table could not be read");
            algo_lib::_db.exit_code++;
        } else {
            amc::BeginNsBlock(*ns.hdr, ns, "");
            amc::BeginNsBlock(*ns.cpp, ns, "");
            cstring symboltype = ResolveGsymboltype(gsymbol);
            int nline = 0;
            ind_beg(Line_curs,line,file.text) {
                Tuple tuple;
                nline++;
                bool readq = Tuple_ReadStrptrMaybe(tuple, line);
                bool empty = (attrs_N(tuple) == 0);
                tempstr value = empty ? tempstr() : tempstr(attrs_qFind(tuple, 0).value);
                // A line that does not parse as a tuple is an input error the
                // reader itself cannot describe: an unterminated quoted value
                // keeps whatever was read before the quote ran out, so the
                // truncated text becomes a symbol name and value the table
                // never wrote, and every reference to the intended symbol
                // fails to link. The tuple reader leaves no error text of its
                // own, so the diagnostic states its own subject: this file,
                // this line, this text. The line is skipped and the scan
                // continues, so every bad line is reported once.
                if (!readq) {
                    prerr("amc.bad_gsymbol"
                          <<Keyval("gsymbol",gsymbol.gsymbol)
                          <<Keyval("file",fname)
                          <<Keyval("line",nline)
                          <<Keyval("text",line)
                          <<Keyval("comment","gsymbol table line is not a tuple"));
                    algo_lib::_db.exit_code++;
                } else if (!empty && Regx_Match(regx, value)) {
                    tempstr name = strptr_ToCppIdent(tempstr()<<ssimfile_Get(gsymbol)<<"_"<<value,true);
                    *ns.hdr << "    extern const "<< symboltype << " " << name;
                    *ns.hdr << "; // ";
                    strptr_PrintCppQuoted(value, *ns.hdr, '"');
                    *ns.hdr << eol;

                    *ns.cpp << "    const " << symboltype << " " << name << "(";
                    strptr_PrintCppQuoted(value, *ns.cpp, '"');
                    *ns.cpp << ");" << eol;
                }
            }ind_end;
            amc::EndNsBlock(*ns.hdr, ns, "");
            amc::EndNsBlock(*ns.cpp, ns, "");
        }
    }ind_end;
}
