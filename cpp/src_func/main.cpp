// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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

// Return LINE with a leading 'template<...>' parameter list removed.
// Name extraction locates a function's parameter list as the first paren on
// the line; a template parameter list can carry parens of its own (e.g. the
// function-pointer parameter in 'template<typename T, void (*F)(T)>'), which
// that search would stop at, degenerating the extracted name. Skipping the
// template list first -- by angle-bracket depth, with angle brackets inside
// parens ignored so a comparison in a parenthesized default argument (e.g.
// 'template<bool B = (N > 0)>') does not end the scan early -- leaves a line
// the paren search handles like any non-template function.
strptr src_func::StripTemplate(strptr line) {
    strptr ret = line;
    if (StartsWithQ(line,"template") && (line.n_elems==8 || !algo_lib::IdentCharQ(line.elems[8]))) {
        int depth = 0;
        int parens = 0;
        int end = 0;
        for (int i = 0; i < line.n_elems && end == 0; i++) {
            if (line.elems[i] == '(') {
                parens++;
            } else if (line.elems[i] == ')') {
                parens--;
            } else if (parens == 0) {
                if (line.elems[i] == '<') {
                    depth++;
                } else if (line.elems[i] == '>') {
                    depth--;
                    if (depth == 0) {
                        end = i+1;
                    }
                }
            }
        }
        ret = TrimmedLeft(RestFrom(line,end));
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

// Extract function namespace name from FUNCLINE, a function's first line
// void *ns::blah(arg1, arg2) -> ns
// A leading template parameter list is skipped, so a caller may pass the raw
// line or an already-stripped view (the skip is then a no-op).
tempstr src_func::GetFuncNs(strptr funcline) {
    strptr funcname = src_func::StripTemplate(funcline);
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
    return func.p_cppsym ? acrkey_Get(*func.p_cppsym) : algo::strptr();
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

// The srcfile filter the update-hdr tag in NSCOMMENT names, and `%` when it names
// none. NSCOMMENT is the comment a marker line ends in, which is where its tag and
// the tag's attributes stand.
// A marker line can carry the tag twice: `namespace foo { /* was: // update-hdr
// srcfile:src/z.cpp */ // update-hdr srcfile:src/y.cpp` quotes an earlier marker in
// a remark and writes the live one after it, and the two occurrences name different
// sources. What makes such a line a marker is the tag in the comment it ends in, so
// the attributes are read from that same occurrence: a search over the whole line
// reaches the quoted tag first and fills the section from a source the live marker
// does not name, at exit 0.
// Pinned by src_func.SectionOpen, whose requoted marker names one source inside the
// remark and another in the tag that opens its section.
tempstr src_func::Nsline_GetSrcfile(strptr nscomment) {
    tempstr ret("%");
    int idx = FindStr(nscomment,"// update-hdr");
    if (idx != -1) {
        Tuple tuple;
        (void)Tuple_ReadStrptrMaybe(tuple,RestFrom(nscomment,idx+3));
        ret=attr_GetString(tuple,"srcfile","%");
    }
    return ret;
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
// The reason is that if something changes in the underlying table where the user function
// no longer gets generated (and thus expected) by amc, the user function should trigger
// a compile error from lack of prototype.
// Also, for each prototype that we refused to generate because a function matched
// a known gen affix, we add a comment to the include file.
void src_func::CalcGenaffix() {
    ind_beg(_db_cppsym_curs,cppsym,_db) if (cppsym.extrn) {
        tempstr affix;
        strptr cppname = cppname_Get(cppsym);
        strptr acrkey = acrkey_Get(cppsym);
        strptr ns = Pathcomp(cppname,".LL");
        strptr table = Pathcomp(acrkey,":LL");
        tempstr table_affix = tempstr() << ns << "." << Pathcomp(table,".RR") << "_";
        if (table == "dmmeta.dispatch_msg") {
            // acrkey:dmmeta.dispatch_msg:<ns>.In/<proto>.AuthMsg
            //   -> affix <ns>.In_
            affix << Pathcomp(acrkey,":LR/LL") << "_";
        } else if (table == "dmmeta.fstep" || table == "dmmeta.fbuf") {
            // acrkey:dmmeta.fstep:%
            // cppname:ns.cd_intfmc_read_Step
            //   -> affix ns._Step
            // or
            // acrkey:dmmeta.fbuf:ns.Msgbuf.in_custom
            // cppname:ns.in_custom_ScanMsg
            //   -> affix ns._ScanMsg
            affix << ns << "._" << Pathcomp(cppname,"_RR");
        } else if (StartsWithQ(cppname, table_affix)) {
            // a function named after the table that binds it, as a gstatic
            // table binds one function per row:
            // cppname:ns.tuneparam_hugepages
            // acrkey:<ns>db.tuneparam:hugepages
            //   -> affix ns.tuneparam_
            // this is the most important class
            affix << table_affix;
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
        // a scan that failed to read a targsrc file leaves the function
        // index incomplete: rewriting headers from it would strip the unread
        // file's prototypes, and re-creating "missing" functions from it
        // would duplicate definitions the scan never saw, so the failed run
        // reports the unreadable file and mutates nothing
        bool scanok = algo_lib::_db.exit_code==0;
        if (src_func::_db.cmdline.updateproto) {
            action=true;
            if (scanok) {
                Main_UpdateHeader();
            }
        }
        if (src_func::_db.cmdline.createmissing) {
            action=true;
            if (scanok) {
                Main_CreateMissing();
            }
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
