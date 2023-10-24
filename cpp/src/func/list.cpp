// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src/func/list.cpp -- Print list of functions
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

static void PrintPrecomment(src_func::FFunc &func, cstring &out) {
    ind_beg(Line_curs,line,func.precomment) {
        out << "// "<< line << eol;
    }ind_end;
    if (src_func::_db.cmdline.showsortkey) {
        out << "// "<<func.sortkey << eol;
    }
}

// -----------------------------------------------------------------------------

// Return the first line of function definition, without namespace.
// Convert DFLTARG(xxx) to =xxx
static tempstr FirstLineWithoutNs(src_func::FFunc &func, strptr ns) {
    tempstr proto(GetProto(func)); // void ns::funcname(....)
    // strip namespace by looking for "ns::"
    // to the left of the first paren
    // and excluding it from the prototype;
    tempstr nsstr = tempstr() << ns << "::";
    int paren=Find(strptr(proto),'(');
    strptr left=FirstN(strptr(proto),paren);
    algo::i32_Range r=substr_FindLast(left,nsstr);
    return tempstr() << FirstN(strptr(proto),r.beg)
                     << algo::Trimmed(RestFrom(strptr(proto),r.end));
}

// -----------------------------------------------------------------------------

// Print function declaration for use in include
static void PrintGlobalProtos_Decl(src_func::FFunc &func, cstring &out, strptr ns) {
    tempstr result;
    // space before previous function
    if (ch_N(func.precomment)) {
        result << eol;
    }
    PrintPrecomment(func,result);
    if (func.amcprot) {
        if (bool_Update(src_func::_db.printed_user_impl_notice, true)) {
            // avoid several of these notices in a row -- looks bad
            result << "//     (user-implemented function, prototype is in amc-generated header)\n";
        }
        result << "// "; // comment out subsequent prototype
    } else {
        src_func::_db.printed_user_impl_notice = false;
    }
    result << FirstLineWithoutNs(func,ns)<< ";"<< eol;
    // see define.h for meaning of this
    Replace(result,"NOTHROW","__attribute__((nothrow))");
    Replace(result,"NORETURN","__attribute__((noreturn))");
    out << result;
}

// -----------------------------------------------------------------------------

static tempstr DemarcatedComment(strptr comment) {
    tempstr ret;
    if (ch_N(comment)) {
        ret << " -- " << comment;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Collect list of global function prototypes
// in target TARGET with namespace NS
void src_func::PrintGlobalProtos(src_func::FTarget &target, strptr ns, strptr srcfilter, cstring &out) {
    algo_lib::Regx regx;
    verblog("src_func.print_global_protos"
            <<Keyval("target",target.target)
            <<Keyval("ns",ns)
            <<Keyval("srcfilter",srcfilter));
    (void)Regx_ReadStrptrMaybe(regx,srcfilter);
    out <<
        "    // Dear human:\n"
        "    //     Text from here to the closing curly brace was produced by scanning\n"
        "    //     source files. Editing this text is futile.\n"
        "    //     To refresh the contents of this section, run 'update-hdr'.\n"
        "    //     To convert this section to a hand-written section, remove the word"
        " 'update-hdr' from namespace line.\n";
    ind_beg(src_func::target_cd_targsrc_curs,targsrc,target) {
        src_func::_db.printed_user_impl_notice = false;// reset warning indicator
        if (Regx_Match(regx,src_Get(targsrc))) {
            tempstr funcs;
            ind_beg(src_func::targsrc_zd_func_curs,func,targsrc) {
                if (!func.isstatic && GetFuncNs(func.func) == ns) {
                    func.p_written_to = &targsrc; // save it
                    PrintGlobalProtos_Decl(func,funcs,ns);
                }
            }ind_end;
            if (ch_N(funcs)) {
                out << "" << eol;
                out << "    // -------------------------------------------------------------------" << eol
                    << "    // "<<src_Get(targsrc) << DemarcatedComment(targsrc.comment) << eol
                    << "    //"<<eol;
                algo::InsertIndent(out,funcs,1);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void src_func::Main_ListFunc() {
    ind_beg(src_func::_db_bh_func_curs,func,src_func::_db) {
        if (func.select) {
            tempstr out;
            verblog("# src_func.func"
                    <<Keyval("func",func.func)
                    <<Keyval("line",func.line)
                    <<Keyval("isstatic",func.isstatic)
                    <<Keyval("isinline",func.isinline)
                    <<Keyval("amcprot",func.amcprot)
                    <<Keyval("iffy",func.iffy)
                    <<Keyval("mystery",func.mystery)
                    <<Keyval("sortkey",func.sortkey)
                    );
            PrintPrecomment(func,out);
            if (src_func::_db.cmdline.showloc) {
                out << Location(func,0);
                if (!src_func::_db.cmdline.proto) {
                    out << eol;
                }
            }
            if (src_func::_db.cmdline.proto) {
                out << GetProto(func);
            } else {
                out << func.body;
            }
            prlog(out);
        }
    }ind_end;
}
