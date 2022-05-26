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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src/func/scan.cpp -- Scan sources to collect functions
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

// Try to figure out if function has an amc-defined prototype by looking up its prefix
// in dispatch table for its namespace.
// This only works if dispatch name does not contain an underscore
static bool AmcProtQ(strptr funcname) {
    bool ret=false;
    strptr prefix = Pathcomp(funcname, "(LL RR*RR&RR");
    int start =0, next=0;
    for (; start < prefix.n_elems; start = next+1) {
        next=FindFrom(prefix,'_',start);
        if (next==-1) {
            break;
        }
        tempstr key(tempstr() << FirstN(prefix,next));
        if (src_func::ind_genprefix_Find(key)) {
            ret=true;
            break;
        }
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
        func->amcprot = AmcProtQ(func->func);
        func->p_targsrc = &targsrc;
        func->precomment = precomment;
        func->mystery = ch_N(func->precomment)<20 && !func->isstatic && !func->isinline;
        func->line = src_func::_db.cur_line;
        // function may fail to xref (and that's ok)
        bool xrefok=func_XrefMaybe(*func);
        if (!xrefok) {
            verblog(Location(*func,0)<<": src_func can't parse declaration: "<<func->func<<" (failed to xref)");
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
    MmapFile file;
    verblog2("src_func.scanfile"
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
        if (ch_First(line)== '}') {
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
    bool ret = (issrc || inlhdr);
    if (src_func::_db.cmdline.updateproto) {
        ret &= targsrc.p_target->select;
    } else {
        ret &=
            Regx_Match(src_func::_db.cmdline.targsrc, targsrc.targsrc)
            || targsrc.p_target->select;
    }
    ret &= (src_func::_db.cmdline.gen || !GeneratedQ(targsrc));
    return ret;
}

// -----------------------------------------------------------------------------

static void CreatePrefix(strptr ns, strptr name) {
    src_func::ind_genprefix_GetOrCreate(tempstr() << ns << "::" << name);
}

// -----------------------------------------------------------------------------

// Compute a table of function name prefixes that correspond to generated functions.
// We should not emit prototypes for these since this will allow dead code
// to exist.
static void PrepGenprefix() {
    ind_beg(src_func::_db_gstatic_curs,gstatic,src_func::_db) {
        CreatePrefix(ns_Get(gstatic), dmmeta::Field_name_Get(gstatic.field));
    }ind_end;
    ind_beg(src_func::_db_dispatch_curs,dispatch,src_func::_db) {
        CreatePrefix(ns_Get(dispatch), name_Get(dispatch));
    }ind_end;

    ind_beg(src_func::_db_fstep_curs,fstep,src_func::_db) {
        CreatePrefix(ns_Get(fstep), name_Get(fstep));
    }ind_end;
    ind_beg(src_func::_db_genprefix_curs,genprefix,src_func::_db) {
        verblog2("src_func.prefix"
                 <<Keyval("prefix",genprefix.genprefix));
    }ind_end;
}

// -----------------------------------------------------------------------------

// Read functions from all sources
void src_func::Main_ScanFiles() {
    // Create genprefix
    PrepGenprefix();
    // load functions
    ind_beg(src_func::_db_targsrc_curs,targsrc,src_func::_db) {
        if (VisitfileQ(targsrc)) {
            ScanFile(targsrc);
        }
    }ind_end;
}
