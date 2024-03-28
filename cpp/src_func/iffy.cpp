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
// Source: cpp/src_func/iffy.cpp -- Check iffy function prototypes
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

static void MarkIffy(src_func::FFunc &func, int errline, strptr where, strptr error, strptr suggestion) {
    // allow disabling errors with 'ignore:XYZ' tag
    if (FindStr(func.precomment, tempstr()<<"ignore:"<<error)==-1) {
        tempstr comment;
        comment<<suggestion<<"; to ignore, add ignore:"<<error<<" anywhere in function comment";
        func.iffy = true;
        prlog(Location(func, errline)
              << Keyval("where",where)
              << Keyval("error",error)
              << Keyval("comment",comment));
    }
}

// -----------------------------------------------------------------------------

static bool ByValQ(strptr s) {
    return FindStr(s,"&")==-1 && FindStr(s,"*")==-1;
}

// -----------------------------------------------------------------------------

static void CheckArg(src_func::FFunc &func, strptr arg) {
    if (ByValQ(arg)) {
        if (FindStr(arg,"cstring ")!=-1) {
            MarkIffy(func, 0, arg, "bigarg", "pass strptr instead");
        }
        if (FindStr(arg,"tempstr ")!=-1) {
            MarkIffy(func, 0, arg, "temparg", "pass strptr instead");
        }
        tempstr ctype(Trimmed(Pathcomp(arg," LL")));
        Replace(ctype,"::",".");
        src_func::FCtypelen *ctypelen = src_func::ind_ctypelen_Find(ctype);
        if (!ctypelen) {
            ctypelen = src_func::ind_ctypelen_Find(tempstr()<<"algo."<<ctype);
        }
        if (ctypelen) {
            if (ctypelen->len >= 32) {
                MarkIffy(func, 0, arg, "bigarg"
                         ,tempstr()<<"arg is "<<ctypelen->len<<" bytes; pass by const reference");
            }
        } else if (ctype != "") {
            verblog("src_func.badtype"
                    <<Keyval("location",Location(func,0))
                    <<Keyval("ctype",ctype)
                    <<Keyval("comment","unknown type"));
        }
    }
    if (FindStr(arg, "*&") != -1 || FindStr(arg, "* &") != -1) {
        MarkIffy(func, 0, arg, "ptr_byref", "don't pass pointers by reference, leads to dangling pointer bugs");
    }
}

// -----------------------------------------------------------------------------

static int LeadingWhitespace(strptr line) {
    return ch_N(line) - ch_N(TrimmedLeft(line));
}

// -----------------------------------------------------------------------------

static void CheckIndBegIndentation(src_func::FFunc &func) {
    // scan function body for badly indented ind_beg/ind_end;
    // stack of whitespace indentations for ind_beg.
    // Matching ind_ends must occur at the same level of indentation
    algo::U64Ary beg_stack;
    algo::U64Ary loc_stack;
    static algo_lib::Regx beg;
    static algo_lib::Regx end;
    static bool firsttime;
    // non-sql regx
    if (bool_Update(firsttime,true)) {
        (void)Regx_ReadDflt(beg,"\\s*ind_beg.*\\(.*"); // full
        (void)Regx_ReadDflt(end,"\\s*}\\s*ind_end.*"); // full
    }
    ind_beg(Line_curs,line,func.body) {
        if (Regx_Match(beg,line)) {
            ary_Alloc(beg_stack) = LeadingWhitespace(line);
            ary_Alloc(loc_stack) = ind_curs(line).i;
        } else if (Regx_Match(end,line)) {
            u32 offset = LeadingWhitespace(line);
            if (ary_N(beg_stack)==0) {
                MarkIffy(func, 0, "ind_end", "extra_ind_end", "No matching ind_beg");
                break;
            }
            u32 shouldbe = ary_qLast(beg_stack);
            u32 errline = ary_qLast(loc_stack);
            if (shouldbe != offset) {
                prlog(func.body);
                MarkIffy(func, errline, "ind_end", "unbalanced_ind_beg"
                         , tempstr()
                         <<"Indentation level of ind_end ("<<offset<<")"
                         <<" doesn't match that of preceding ind_beg ("<<shouldbe<<") "<<Keyval("func",func.func));
                break;
            }
            ary_RemoveLast(beg_stack);
            ary_RemoveLast(loc_stack);
        }
    }ind_end;
    // don't care if the beg_stack is non-empty here -- C++ compiler
    // will catch syntax errors.
}

// -----------------------------------------------------------------------------

// Check if the function is iffy and set FUNC.IFFY flag if so.
// Checks can be disabled with ignore: annotations in function pre-comment
void src_func::ComputeIffy(src_func::FFunc &func) {
    strptr retval = Pathcomp(func.func,"(LL");
    if (FindStr(retval, "cstring ") != -1 && ByValQ(retval)) {
        MarkIffy(func, 0, "retval", "bigret", "return tempstr instead");
    }
    strptr proto = Pathcomp(func.args,")RL");
    for (; proto != ""; proto=Pathcomp(proto,",LR")) {
        strptr arg=Pathcomp(proto,",LL");
        CheckArg(func,arg);
    }
    strptr ns = GetFuncNs(func.func);
    tempstr need_ns(target_Get(*func.p_targsrc));
    if (ns != "" && ns != need_ns && ind_target_Find(ns)) {
        MarkIffy(func, 0, func.func, "funcns"
                 , (tempstr()<<"Expected namespace "<<need_ns));
    }
    CheckIndBegIndentation(func);
}
