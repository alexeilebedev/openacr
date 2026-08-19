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

// Is TEXT nothing but a run of NMIN or more copies of CH?
static bool CharRunQ(strptr text, char ch, int nmin) {
    bool ret = ch_N(text) >= nmin;
    for (int i = 0; i < ch_N(text); i++) {
        ret = ret && text[i] == ch;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Is TEXT a banner -- a line drawn out of slashes and nothing else?
// A banner is written two ways in this tree. One is a bare run of slashes, which
// is a comment by its first two bytes and a drawing by the rest of them. The
// other puts the run behind a comment of its own, `// ` and then the slashes,
// which is how the two banners in the tree are written and what a titled banner
// needs so its title line can be a comment too. Whitespace may stand anywhere on
// the line, and the seventeen-slash bound counts every slash on it, the comment's
// own two among them.
// A word among the slashes makes the line comment text rather than a divider,
// which is the rule a rule line of dashes broken by a word follows: a banner
// carrying a section title says what the section is, and dropping the lines above
// it drops the paragraph the title belongs to.
static bool BannerQ(strptr text) {
    int nslash = 0;
    bool only = true;
    for (int i = 0; i < ch_N(text); i++) {
        char c = text[i];
        only = only && (c == '/' || algo_lib::WhiteCharQ(c));
        nslash += c == '/';
    }
    return only && nslash >= 17;
}

// -----------------------------------------------------------------------------

// Is LINE a divider -- the line of dashes, or the banner of slashes, that
// stands between one function and the next?
// A divider belongs to no function: it is not comment text, and it ends the
// comment accumulated above it.
// The test reads the line's whole form, not a substring of it. A leading
// comment that draws an ascii table has a rule line made of dashes, and reading
// that rule as a divider drops everything above the table -- the paragraph
// saying what the function does, and the table's own header row -- from the
// function's comment and from the header mirroring it. A rule line therefore
// stays comment text as long as its dashes are broken by something, a space or
// a pipe, which every table in the tree does; a rule line of nothing but dashes
// cannot be told apart from a divider and reads as one.
// The banner of slashes reads the same way, in either of the forms it is written
// in, and BannerQ carries that half of the rule.
static bool DividerQ(strptr line) {
    strptr text = Trimmed(line);
    bool banner = BannerQ(text);
    bool dashes = StartsWithQ(text,"//") && CharRunQ(Trimmed(RestFrom(text,2)),'-',20);
    return banner || dashes;
}

// -----------------------------------------------------------------------------

// Detect comments just prior to a function
// PRECOMMENT is the comment being accumulated.
// TARGSRC is the sourcfile being scanned, LINE is the current line.
// Either append line to comment (after stripping decorations) or clear PRECOMMENT.
// After the function finishes, PRECOMMENT contains current precomment so far.
static void AccumPrecomment(src_func::FTargsrc &targsrc, strptr line, cstring &precomment) {
    bool separator = DividerQ(line);
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

static tempstr Sortkey(src_func::FTargsrc &targsrc, strptr funcline, strptr stripped, i32 lineno) {
    tempstr ret;
    if (src_func::_db.cmdline.sortname) {
        ret << Pathcomp(stripped,"(LL RR:RR")<<"."<<funcline;
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
        // one stripped view serves every extraction below: the sortkey, args,
        // userfunc key, and name must all key the same function identically.
        // func->func gains a " //<file>" suffix for statics, but the suffix
        // follows the "(", past everything the extractions read, so the view
        // bound from funcline stands in for func->func as well.
        strptr stripped = src_func::StripTemplate(funcline);
        func->args=Pathcomp(stripped,"(LR");
        // the keyword is matched as a whole token: a raw substring search
        // would claim any function whose name carries the word
        // (gen_load_gstatic, comptest_amc_GstaticLoadFail), and a function
        // misread as static is silently dropped from the header prototypes
        func->isstatic = algo::ContainsIdentQ(funcline,"static");
        if (func->isstatic) {// make pkey of static function unique by including file name
            func->func <<" //"<<src_Get(targsrc);
        }
        func->isinline = algo::ContainsIdentQ(funcline,"inline");
        func->sortkey=Sortkey(targsrc,funcline,stripped,src_func::_db.cur_line);
        tempstr key(Pathcomp(stripped, "(LL RR*RR&RR"));
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
        tempstr ns = src_func::GetFuncNs(stripped);
        if (ns == "") {
            ns = func->p_targsrc->p_target->target;
        }
        func->name << ns << "." << Pathcomp(stripped,"(LL RR:RR");

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
// A targsrc file that cannot be read fails the run: scanning it as empty
// would silently drop its functions from the index and the header protos.
// A file that ends inside a function body fails it the same way: the scan
// is incomplete, and the index must not drive a header rewrite.
static void ScanFile(src_func::FTargsrc &targsrc) {
    algo_lib::MmapFile file;
    prcat(verbose2,"src_func.scanfile"
          <<Keyval("src",src_Get(targsrc)));
    if (!MmapFile_Load(file,src_Get(targsrc))) {
        algo::PrerrFileFail("src_func.file_read",src_Get(targsrc),"source file could not be read");
        algo_lib::_db.exit_code++;
    } else {
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
                if (func) {
                    func->endline = src_func::_db.cur_line;
                }
                func=NULL;
                precomment="";
            }
        }ind_end;
        if (func) {
            prerr("src_func.unfinished_func"
                  <<Keyval("file",src_Get(targsrc))
                  <<Keyval("func",func->func)
                  <<Keyval("comment","file ends inside a function body; scan is incomplete"));
            algo_lib::_db.exit_code++;
        }
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
