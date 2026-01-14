// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/src_func/main.cpp -- Main file
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
        ret = Trimmed(FirstN(line,r.beg));
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
// - has parentheses
// - last nonblank character is {
// - it's not namespace, enum or struct
bool src_func::FuncstartQ(strptr line, strptr trimmedline) {
    bool funcstart = line.n_elems>0
        && !algo_lib::WhiteCharQ(line.elems[0])
        && !Regx_Match(src_func::_db.ignore_funcstart,line)
        && FindStr(line,"(") !=-1
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

// Extract function namespace name
// void *ns::blah(arg1, arg2) -> ns
tempstr src_func::GetFuncNs(src_func::FFunc &func) {
    strptr funcname = func.func;
    strptr s = Pathcomp(funcname,"(LL RR"); // void *ns::blah
    algo::i32_Range r = substr_FindLast(s,"::"); // 8..10
    s = FirstN(s,r.beg);// void *ns
    int idx =s.n_elems;
    while (idx>0 && algo_lib::IdentCharQ(s[idx-1])) {
        idx--;
    }
    tempstr ret(RestFrom(s,idx));// ns
    return ret;
}

static strptr GetAcrkey(src_func::FFunc &func) {
    algo::strptr ret;
    if (func.p_userfunc) {
        // for keys like X/Y, leave only the Y
        ret = Pathcomp(func.p_userfunc->acrkey,"/RR");
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Filter functions based on parameters provided on command line.
bool src_func::MatchFuncQ(src_func::FFunc &func) {
    bool show=true
        && !(func.isstatic && !src_func::_db.cmdline.showstatic)
        // match raw function name (no namespace)
        && Regx_Match(src_func::_db.cmdline.func,func.name)
        && Regx_Match(src_func::_db.cmdline.matchbody,func.body)
        && Regx_Match(src_func::_db.cmdline.matchproto,func.func)
        && Regx_Match(src_func::_db.cmdline.acrkey,GetAcrkey(func))
        && Regx_Match(src_func::_db.cmdline.matchcomment, func.precomment);
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
    ind_beg(src_func::_db_targsrc_curs,targsrc,src_func::_db) {
        targsrc.select=Regx_Match(src_func::_db.cmdline.targsrc, targsrc.targsrc);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_Report() {
    prlog(src_func::_db.report);
}

// -----------------------------------------------------------------------------

void src_func::RewriteOpts() {
    bool nextfile = ch_N(src_func::_db.cmdline.nextfile);
    bool updateproto = src_func::_db.cmdline.updateproto;

    if (!nextfile && !updateproto && !_db.cmdline.baddecl) {
        src_func::_db.cmdline.list = true;
    }
    if (_db.cmdline.f) {
        _db.cmdline.showbody=true;
        _db.cmdline.showcomment=true;
        _db.cmdline.sortname=true;
    }
    if (_db.cmdline.createmissing) {
        _db.cmdline.list=false;
    }
}

// -----------------------------------------------------------------------------

// Calculate a set of prefixes & suffixes (together:affixes)
// which "look like generated code"
// We will not generate prototypes for functions that look like generated code
//   (this is a heuristic, not a hard rule, but it saves hours of debugging when it works)
// The reason is that if something changes in the underlying table where the userfunc
// no longer gets generated (and thus expected) by amc, the user function should trigger
// a compile error from lack of prototype.
// Also, for each prototype that we refused to generate because a function matched
// a known gen affix, we add a comment to the include file.
void src_func::CalcGenaffix() {
    ind_beg(_db_userfunc_curs,userfunc,_db) {
        tempstr affix;
        strptr ns = Pathcomp(userfunc.userfunc,".LL");
        if (StartsWithQ(userfunc.acrkey,"gstatic/")) {
            // e.g. userfunc:ns...tuneparam_hugepages
            //      acrkey:gstatic/db.tuneparam:hugepages
            //   -> affix ns_tuneparam_
            // this is the most important class
            affix << ns << "."<<Pathcomp(userfunc.acrkey,":LL.RR") << "_";
        } else if (StartsWithQ(userfunc.acrkey,"dispatch_msg:")) {
            // acrkey:dispatch_msg:someproc.In/somens.SomeMsg
            //   -> affix someproc.In_
            affix << Pathcomp(userfunc.acrkey,":LR/LL") << "_";
        } else if (StartsWithQ(userfunc.acrkey,"fstep:") || StartsWithQ(userfunc.acrkey,"fbuf:")) {
            // acrkey:fstep:%
            // userfunc:ns.FDb.cd_intfmc_read.Step
            //   -> affix ns._Step
            // or
            // acrkey:fbuf:ns.Msgbuf.in_custom
            // userfunc:ns.Msgbuf.in_custom.ScanMsg
            //   -> affix ns._ScanMsg
            affix << ns << "._" << Pathcomp(userfunc.userfunc,".RR");
        }
        if (affix != "") {
            src_func::ind_genaffix_GetOrCreate(affix);
        }
    }ind_end;
    ind_beg(_db_genaffix_curs,genaffix,_db) {
        verblog("src_func.genaffix"
                <<Keyval("genaffix",genaffix.genaffix));
    }ind_end;
}

// -----------------------------------------------------------------------------

src_func::FGenaffix *src_func::FindAffix(strptr cppname) {
    src_func::FGenaffix *ret=NULL;
    int idx = 0;
    strptr ns=Pathcomp(cppname,".LL");
    strptr name=Pathcomp(cppname,".LR");
    while ((idx = FindFrom(name, "_", idx, true))!=-1) {
        ret=ind_genaffix_Find(tempstr()<<ns<<"."<<FirstN(strptr(name), idx+1)); // try prefix
        if (ret) {
            break;
        }
        ret=ind_genaffix_Find(tempstr()<<ns<<"."<<RestFrom(strptr(name), idx)); // try suffix
        if (ret) {
            break;
        }
        idx++;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Main
void src_func::Main() {
    (void)Regx_ReadStrptrMaybe(src_func::_db.ignore_funcstart, "(namespace|enum|struct|#|//|/*|*/)%");
    bool report=_db.cmdline.report;

    SelectTarget();

    RewriteOpts();

    CalcGenaffix();

    bool action = false;
    if (_db.cmdline.baddecl) {
        action=true;
    }

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
        if (src_func::_db.cmdline.createmissing) {
            action=true;
            Main_CreateMissing();
        }
        Main_SelectFunc();
        if (src_func::_db.cmdline.e) {
            action=true;
            Main_EditFunc();
        }
        if (src_func::_db.cmdline.list) {
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
