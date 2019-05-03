// (C) 2019 AlgoR&D
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
// Target: atf_norm (exe) -- Run normalization tests (see normcheck table)
// Exceptions: yes
// Source: cpp/atf/norm/readme.cpp
//

#include "include/atf_norm.h"

// --------------------------------------------------------------------------------

// History of SKNF -> [History of SKNF](#history-of-sknf)
static tempstr TocLink(strptr str) {
    tempstr ret;
    ret << "[" << str << "](#";
    tempstr lc;
    lc << str;
    MakeLower(lc);
    for (u32 i=0; i < lc.ch_n; i++) {
        if (!algo_lib::IdentCharQ(lc.ch_elems[i])) {
            lc.ch_elems[i] = '-';
        }
    }
    Replace(lc,"--","-");
    ret << lc << ")";
    return ret;
}

// Scan string FROM for markdown header indicators
// (==, ===, ==== etc)
// And add them as sections to the table of contents, with 3 spaces per level
static void AppendToc(strptr from, cstring &to) {
    ind_beg(Line_curs,line,from) {
        int i=0;
        while (i<line.n_elems && line[i]=='#') {
            i++;
        }
        if (i>1 && i<line.n_elems && line[i]==' ') {
            char_PrintNTimes(' ',to,(i-1)*3);
            to << "* " << TocLink(RestFrom(line,i+1)) << eol;
        }
    }ind_end;
}

// Generate README.md by scanning the readme table
// for instructions
void atf_norm::normcheck_readme() {
    cstring text;
    cstring out;
    out << "This file was created with 'atf_norm readme' from files in [txt/] -- *do not edit*\n\n";
    out << "## Table Of Contents\n";
    ind_beg(_db_readme_curs,readme,_db) {
        text << eol;
        text << FileToString(readme.gitfile);
    }ind_end;
    AppendToc(text,out);
    out << eol;
    out << text;
    StringToFile(out, "README.md");
}
