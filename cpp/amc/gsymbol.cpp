// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/gsymbol.cpp -- Generate strings from tables
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static tempstr ToIdent(strptr s) {
    tempstr ret;
    amc::strptr_PrintCppIdent(s,ret);
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr ResolveGsymboltype(amc::FGsymbol &gsymbol) {
    tempstr ret;
    if (ch_N(gsymbol.symboltype)) {
        ret << amc::NsToCpp(gsymbol.p_symboltype->ctype);
    } else {
        ret << amc::NsToCpp(gsymbol.p_ssimfile->p_ctype->ctype)<<"Pkey";
    }
    //prlog(ret);
    return ret;
}

// -----------------------------------------------------------------------------

// Generate regular c++ symbols from tables
void amc::gen_ns_gsymbol() {
    amc::FNs &ns =*amc::_db.c_ns;
    amc::_db.genfield.p_field = NULL;
    ind_beg(amc::ns_c_gsymbol_curs, gsymbol,ns) {
        algo_lib::Regx regx;
        (void)Regx_ReadSql(regx, tempstr() <<"%("<<gsymbol.inc<<")%", true);
        MmapFile file;
        tempstr fname(SsimFname(amc::_db.cmdline.in_dir,ssimfile_Get(gsymbol)));
        vrfy(MmapFile_Load(file,fname),tempstr()<<"amc.load"<<Keyval("filename",fname));
        amc::BeginNsBlock(*ns.hdr, ns, "");
        amc::BeginNsBlock(*ns.cpp, ns, "");
        cstring symboltype = ResolveGsymboltype(gsymbol);
        ind_beg(Line_curs,line,file.text) {
            if (Regx_Match(regx,line)) {
                Tuple tuple;
                Tuple_ReadStrptr(tuple, line, false);
                if (attrs_N(tuple) > 0) {
                    tempstr value(attrs_qFind(tuple,0).value);
                    tempstr name = tempstr() << ToIdent(ssimfile_Get(gsymbol))
                                             << "_"
                                             << ToIdent(value);
                    *ns.hdr << "    extern const "<< symboltype << " " << ToIdent(name);
                    *ns.hdr << "; // ";
                    strptr_PrintCppQuoted(value, *ns.hdr, '"');
                    *ns.hdr << eol;

                    *ns.cpp << "    const " << symboltype << " " << ToIdent(name) << "(";
                    strptr_PrintCppQuoted(value, *ns.cpp, '"');
                    *ns.cpp << ");" << eol;
                }
            }
        }ind_end;
        amc::EndNsBlock(*ns.hdr, ns, "");
        amc::EndNsBlock(*ns.cpp, ns, "");
    }ind_end;
}
