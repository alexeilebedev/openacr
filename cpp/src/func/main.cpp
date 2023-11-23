// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/src/func/main.cpp -- Main file
//

#include "include/algo.h"
#include "include/src_func.h"

// -----------------------------------------------------------------------------

// Remove single-line C++ comment from file
// and return result
strptr src_func::StripComment(strptr line) {
    algo::i32_Range r=substr_FindLast(line,"//");
    strptr ret = line;
    if (r.end > r.beg) {
        ret = FirstN(line,r.beg);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// S   : input string
// CALL: string that looks like "XYZ("
// Result: find all instances of "XYZ(...)", handling nested parentheses,
//    in string and replace them with
//    PREFIX ... (whatever was inside the parentheses)
// This expands macros DFLTVAL and FUNCATTR in headers.
static tempstr RemoveAnnotation(strptr s, strptr call, strptr prefix) {
    tempstr ret(s);
    int i=0;
    while ((i=FindStr(ret,call))!=-1) {
        int parens=1;
        int start=i+call.n_elems, end;
        for (end =i+call.n_elems; end <ch_N(ret) && parens>0; end++) {
            if (ret.ch_elems[end]=='(') {
                parens++;
            } else if (ret.ch_elems[end]==')') {
                parens--;
            }
        }
        ret=tempstr()<<ch_FirstN(ret,i)<<prefix<<ch_GetRegion(ret,start,end-1-start)<<ch_RestFrom(ret,end);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Get first line of function definition
// Remove open curly
// Replace DFLTVAL(x) with =x (for headers)
// Replace FUNCATTR(x) with x (for headers)
tempstr src_func::GetProto(src_func::FFunc &func) {
    tempstr ret(Pathcomp(func.body,"\nLL{LL"));
    ret=RemoveAnnotation(ret,"DFLTVAL(","= ");
    ret=RemoveAnnotation(ret,"FUNCATTR(","");
    return ret;
}

// -----------------------------------------------------------------------------

// Check if line contains function start
// Criteria are:
// - first character nonblank
// - last nonblank character is {
// - it's not namespace, enum or struct
bool src_func::FuncstartQ(strptr line, strptr trimmedline) {
    bool funcstart = line.n_elems>0
        && !algo_lib::WhiteCharQ(line.elems[0])
        && !Regx_Match(src_func::_db.ignore_funcstart,line)
        && trimmedline.n_elems>0
        && trimmedline.elems[0] != '}'
        && qLast(trimmedline)!= ';';
    bool curly
        = trimmedline.n_elems>0
        && qLast(trimmedline) == '{';
    if (funcstart && !curly) {
        tempstr msg;
        msg << GetFileloc() << "suspicious multi-line function declaration "<<trimmedline;
        src_func::_db.report.n_baddecl++;
        if (src_func::_db.cmdline.baddecl) {
            prlog(msg);
            algo_lib::_db.exit_code++;
        } else {
            verblog(msg);
        }
    }
    return funcstart && curly;
}

// -----------------------------------------------------------------------------

// Extract function name for a global function, stripping arguments and namespace
static strptr GetFuncname(strptr funcname) {
    return Pathcomp(funcname,"(LL RR:RR");
}

// -----------------------------------------------------------------------------

// Extract function namespace name
// void *ns::blah(arg1, arg2) -> ns
strptr src_func::GetFuncNs(strptr funcname) {
    strptr s = Pathcomp(funcname,"(LL RR"); // void *ns::blah
    algo::i32_Range r = substr_FindLast(s,"::"); // 8..10
    s = FirstN(s,r.beg);// void *ns
    int idx =s.n_elems;
    while (idx>0 && algo_lib::IdentCharQ(s[idx-1])) {
        idx--;
    }
    return RestFrom(s,idx);// ns
}

// -----------------------------------------------------------------------------

// Filter functions based on parameters provided on command line.
static bool MatchFuncQ(src_func::FFunc &func) {
    bool show=true
        && !(func.isstatic && !src_func::_db.cmdline.showstatic)
        // match raw function name (no namespace)
        && Regx_Match(src_func::_db.cmdline.name,GetFuncname(func.func))
        && Regx_Match(src_func::_db.cmdline.body,func.body)
        && Regx_Match(src_func::_db.cmdline.func,func.func)
        && Regx_Match(src_func::_db.cmdline.comment, func.precomment);
    return show;
}

// -----------------------------------------------------------------------------

// List functions from sources that match command line arg
static void Main_SelectFunc() {
    // show loaded functions in sorted order
    ind_beg(src_func::_db_bh_func_curs,func,src_func::_db) {
        func.select = MatchFuncQ(func);
        if (src_func::_db.cmdline.iffy) {
            ComputeIffy(func);
            func.select = func.select && func.iffy;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Get srcfile filter from update-hdr line.
// If none specified, use %
tempstr src_func::Nsline_GetSrcfile(strptr line) {
    tempstr ret("%");
    int idx = FindStr(line,"// update-hdr");
    if (idx != -1) {
        Tuple tuple;
        (void)Tuple_ReadStrptrMaybe(tuple,RestFrom(line,idx+3));
        ret=attr_GetString(tuple,"srcfile","%");
    }
    return ret;
}
// -----------------------------------------------------------------------------

// extract namespace name from a line like 'namespace xyz {'
strptr src_func::Nsline_GetNamespace(strptr str) {
    algo::StringIter iter(str);
    GetWordCharf(iter);
    return GetWordCharf(iter);
}

// -----------------------------------------------------------------------------

static void SelectTarget() {
    // select target, and any targsrc under it
    ind_beg(src_func::_db_target_curs,target,src_func::_db) {
        target.select = Regx_Match(src_func::_db.cmdline.target, target.target);
        if (target.select) {
            ind_beg(src_func::target_cd_targsrc_curs,targsrc,target) {
                targsrc.select=true;
            }ind_end;
        }
    }ind_end;
    // select targsrc, and any target above it
    ind_beg(src_func::_db_targsrc_curs,targsrc,src_func::_db) {
        targsrc.select = targsrc.select || Regx_Match(src_func::_db.cmdline.targsrc, targsrc.targsrc);
        if (targsrc.select) {
            targsrc.p_target->select=true;
        }
    }ind_end;
    ind_beg(src_func::_db_target_curs,target,src_func::_db) if (target.select) {
        verblog("# select "<<target.target);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_Report() {
    prlog(src_func::_db.report);
}

// -----------------------------------------------------------------------------

static void RewriteOpts() {
    bool nextfile = ch_N(src_func::_db.cmdline.nextfile);
    bool updateproto = src_func::_db.cmdline.updateproto;

    if (!nextfile && !updateproto) {
        src_func::_db.cmdline.listfunc = true;
    }
}

// -----------------------------------------------------------------------------

// Main
void src_func::Main() {
    (void)Regx_ReadStrptrMaybe(src_func::_db.ignore_funcstart
                               , "(namespace|enum|struct|#|//|/*|*/)%");
    bool report=_db.cmdline.report;

    SelectTarget();

    RewriteOpts();
    bool action = false;

    if (ch_N(src_func::_db.cmdline.nextfile)) {
        Main_Nextfile();
        action=true;
        report=false;
    } else {
        Main_ScanFiles();
        if (src_func::_db.cmdline.updateproto) {
            action=true;
            Main_UpdateHeader();
        }
        Main_SelectFunc();
        if (src_func::_db.cmdline.e) {
            action=true;
            Main_EditFunc();
        }
        if (src_func::_db.cmdline.listfunc) {
            action=true;
            Main_ListFunc();
        }
    }
    vrfy(action,tempstr()<<"src_func.unknown_option"
         <<Keyval("comment","please select something to do"));
    if (report) {
        Main_Report();
    }
}
