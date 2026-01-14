// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/scan.cpp -- Scan sources to collect functions
//

#include "include/src_func.h"


// -----------------------------------------------------------------------------

// Detect comments just prior to a function
// PRECOMMENT is the comment being accumulated.
// TARGSRC is the sourcfile being scanned, LINE is the current line.
// Either append line to comment (after stripping decorations) or clear PRECOMMENT.
// After the function finishes, PRECOMMENT contains current precomment so far.
static void AccumPrecomment(src_func::FTargsrc &targsrc, strptr line, cstring &precomment) {
    bool separator = FindStr(line,"--------------------")!=-1
        || FindStr(line,"/////////////////")!=-1;
    bool comment = StartsWithQ(line,"//");
    bool clear = separator
        || (zd_func_N(targsrc)==0 && line=="")
        || (ch_N(line)>20 && !comment);

    if (comment && !separator) {
        precomment << TrimmedLeft(RestFrom(line,2)) << eol;
    }
    if (clear) {
        if (precomment != "" && zd_func_N(targsrc)>0) {
            verblog(src_func::GetFileloc()<<"src_func.drop_precomment"
                    <<Keyval("text",algo::LimitLengthEllipsis(precomment,200)));
        }
        precomment = "";// clear it
    }
}

// -----------------------------------------------------------------------------

static tempstr Sortkey(src_func::FTargsrc &targsrc, strptr funcline, i32 lineno) {
    tempstr ret;
    if (src_func::_db.cmdline.sortname) {
        ret << Pathcomp(funcline,"(LL RR:RR")<<"."<<funcline;
    } else {
        ret << src_Get(targsrc)<<":"<<lineno;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Create function record associated with FNAME,LINENO
// Where first line is FUNCLINE
static src_func::FFunc *CreateFunc(src_func::FTargsrc &targsrc, strptr funcline, strptr precomment) {
    src_func::FFunc *func=src_func::ind_func_Find(funcline);
    if (!func) {
        func=&src_func::func_Alloc();
        func->func=funcline;
        func->args=Pathcomp(funcline,"(LR");
        func->isstatic = FindStr(funcline,"static")!=-1;
        if (func->isstatic) {// make pkey of static function unique by including file name
            func->func <<" //"<<src_Get(targsrc);
        }
        func->isinline = FindStr(funcline,"inline")!=-1;
        func->sortkey=Sortkey(targsrc,funcline,src_func::_db.cur_line);
        tempstr key(Pathcomp(func->func, "(LL RR*RR&RR"));
        Replace(key,"::",".");
        func->p_userfunc = src_func::ind_userfunc_cppname_Find(key);
        func->p_targsrc = &targsrc;
        func->precomment = precomment;
        func->mystery = ch_N(func->precomment)<20 && !func->isstatic && !func->isinline;
        func->line = src_func::_db.cur_line;
        // Compute a key-looking string, e.g. ns.blah
        // for a function "void *ns::blah(arg1, arg2)"
        // For a static function where ns is not part of the definition,
        // grab ns name from the target
        tempstr ns = src_func::GetFuncNs(*func);
        if (ns == "") {
            ns = func->p_targsrc->p_target->target;
        }
        func->name << ns << "." << Pathcomp(func->func,"(LL RR:RR");

        // function may fail to xref (and that's ok)
        bool xrefok=func_XrefMaybe(*func);
        if (!xrefok) {
            verblog(Location(*func,0)<<": src_func can't parse declaration: "<<func->func<<" (failed to xref)");
        }
        if (func->p_userfunc) {
            zd_func_Insert(*func->p_userfunc,*func);
        }
        src_func::_db.report.n_func++;
        src_func::_db.report.n_static += func->isstatic;
        src_func::_db.report.n_inline += func->isinline;
        src_func::_db.report.n_mysteryfunc += func->mystery;
    }
    return func;
}

// -----------------------------------------------------------------------------

// Scan contents of FNAME and create function records
static void ScanFile(src_func::FTargsrc &targsrc) {
    algo_lib::MmapFile file;
    prcat(verbose2,"src_func.scanfile"
          <<Keyval("src",src_Get(targsrc)));
    MmapFile_Load(file,src_Get(targsrc));
    cstring precomment;
    src_func::FFunc *func=NULL;
    ind_beg(Line_curs,line,file.text) {
        strptr trimmedline=src_func::StripComment(Trimmed(line));
        SaveFileloc(targsrc,ind_curs(line).i+1);
        bool funcstart = src_func::FuncstartQ(line,trimmedline);
        if (funcstart) {
            func=CreateFunc(targsrc,trimmedline, precomment);
        } else if (!func) {
            AccumPrecomment(targsrc,line,precomment);
        }
        if (func) {
            src_func::_db.report.n_line++;
            func->body << line << eol;
        }
        if (ch_First(line,0)== '}') {
            func=NULL;
            precomment="";
        }
    }ind_end;
    if (func) {
        prerr("src_func.unfinished_func"
              <<Keyval("file",src_Get(targsrc))
              <<Keyval("func",func->func)
              );
    }
}

// -----------------------------------------------------------------------------

static bool GeneratedQ(src_func::FTargsrc &targsrc) {
    return FindStr(src_Get(targsrc),"/gen/")!=-1
        || FindStr(src_Get(targsrc),"extern/")!=-1;
}

// -----------------------------------------------------------------------------

// Check if source file should be scanned.
// And is a C++ file
// And is not generated.
static bool VisitfileQ(src_func::FTargsrc &targsrc) {
    strptr ext = Pathcomp(targsrc.targsrc,"/RR.LR");
    bool issrc = ext == "cpp";
    bool inlhdr = ext == "inl.h";
    bool ret = (issrc || inlhdr) && targsrc.select;
    ret = ret && (src_func::_db.cmdline.gen || !GeneratedQ(targsrc));
    return ret;
}

// -----------------------------------------------------------------------------

// Read functions from all sources
void src_func::Main_ScanFiles() {
    // load functions
    ind_beg(src_func::_db_targsrc_curs,targsrc,src_func::_db) {
        if (VisitfileQ(targsrc)) {
            ScanFile(targsrc);
        }
    }ind_end;
}
