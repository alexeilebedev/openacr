// (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Target: src_lim (exe) -- Enforce line length, function length, and indentation limits
// Exceptions: yes
// Source: cpp/src/lim.cpp
//
// Created By: alexei.lebedev
//

#include "include/algo.h"
#include "include/gen/src_lim_gen.h"
#include "include/gen/src_lim_gen.inl.h"

// -----------------------------------------------------------------------------

static dev::Linelim GetCustomlim(src_lim::FTargsrc &targsrc, dev::Linelim &ideal) {
    dev::Linelim ret(ideal);
    ret.gitfile=src_Get(targsrc);
    if (targsrc.p_gitfile->c_linelim) {
        linelim_CopyOut(*targsrc.p_gitfile->c_linelim,ret);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// suggest a record that will remove the observed error.
// this is used by bin/update-linelim to periodically tighten the line limits.
static void Capture(dev::Linelim &observed, dev::Linelim &ideallim) {
    tempstr badstr;
    int badness = algo::u32_SubClip(observed.nbadws,ideallim.nbadws)
        + algo::u32_SubClip(observed.nlongline,ideallim.nlongline)/5
        + observed.nmysteryfunc*5
        + algo::u32_SubClip(observed.nlongfunc,ideallim.nlongfunc)*2;
    char badchar='.';
    if (badness>30) {
        badness/=2;
        badness=i32_Min(badness,30);
        badchar='*';
    }
    char_PrintNTimes(badchar,badstr,badness);// some measure of badness
    observed.badness=badstr;
    src_lim::_db.outtext << observed << eol;
}

// -----------------------------------------------------------------------------

// INFUNC     inside-function state; updated in this function
// FUNCLEN    current function length; updated in this function as well
// COMMENT    current function comment; updated as well
// LINE       current line
// Keep track of whether we're inside a function or not,
// and accumulate running comment and function length.
static void UpdateInfunc(bool &infunc, u32 &funclen, cstring &comment, strptr line) {
    if (!infunc) {
        funclen=0;
    }
    if (infunc) {
        if (ch_N(line)>0 && !algo_lib::WhiteCharQ(line[0])) {
            infunc=false;
            comment="";
        }
    } else {
        // accumulate running comment
        if (StartsWithQ(line,"//")) {
            comment << Trimmed(RestFrom(line,2)) << eol;
        } else if (line=="") {
            comment = ""; // empty line -> reset comment
        } else if (FindChar(line,'{')!=-1) {
            infunc=true;
            funclen=0;
        }
    }
    if (infunc) {
        funclen++;
    }
}

// -----------------------------------------------------------------------------

// IDEALLIM       a set of preferred limits
// WS             currently observed leading line whitespace
// update OBSERVED, the observed descriptor of a source file.
static bool CheckBadws(dev::Linelim &ideallim, dev::Linelim &observed, u32 ws) {
    bool bad = ws>ideallim.maxws;
    if (bad) {
        observed.nbadws++;
    }
    u32_UpdateMax(observed.maxws,ws);
    return bad;
}

// -----------------------------------------------------------------------------

// IDEALLIM       a set of preferred limits
// LINELEN        currently observed line length
// update OBSERVED, the observed descriptor of a source file.
static bool CheckBadlinelen(dev::Linelim &ideallim, dev::Linelim &observed, u32 linelen) {
    bool bad = linelen>ideallim.longestline;
    if (bad) {
        observed.nlongline++;
    }
    u32_UpdateMax(observed.longestline,linelen);
    return bad;
}

// -----------------------------------------------------------------------------

// IDEALLIM       a set of preferred limits
// FUNCLEN        currently observed function length
// update OBSERVED, the observed descriptor of a source file.
static bool CheckBadfunclen(dev::Linelim &ideallim, dev::Linelim &observed, u32 funclen) {
    bool bad = funclen>ideallim.longestfunc;
    if (bad) {
        observed.nlongfunc++;
    }
    u32_UpdateMax(observed.longestfunc,funclen);
    return bad;
}

// -----------------------------------------------------------------------------

// IDEALLIM       a set of preferred limits
// NMYSTERYFUNC   currently observed # of mystery (undocumented) global functions
// update OBSERVED, the observed descriptor of a source file.
static bool CheckMysteryFunc(dev::Linelim &observed, strptr comment) {
    bool bad = comment=="";
    if (bad) {
        observed.nmysteryfunc++;
    }
    return bad;
}

// -----------------------------------------------------------------------------

static bool GlobalFuncQ(strptr line) {
    return FindStr(Pathcomp(line,"(LL RR"),"::")!=-1;
}

// -----------------------------------------------------------------------------

static int CheckLim(dev::Linelim &ideallim, dev::Linelim &customlim, dev::Linelim &observed) {
    int nerr=0;
    bool infunc=false;
    u32 funclen=0;
    MmapFile file;
    MmapFile_Load(file,ideallim.gitfile);
    cstring func_comment;
    bool show_workaround=false;
    ind_beg(algo::Line_curs, line, file.text) {
        u32 linelen=ch_N(line);
        bool hascurly=FindChar(line,'}')!=-1;
        u32 ws=hascurly ? ch_N(line) - ch_N(TrimmedLeft(line)) : 0;
        bool infunc_before = infunc;
        UpdateInfunc(infunc,funclen,func_comment,line);
        bool badmystery = infunc && !infunc_before && GlobalFuncQ(line) ? CheckMysteryFunc(observed,func_comment) : false;
        bool badws=CheckBadws(ideallim,observed,ws);
        bool badlinelen=CheckBadlinelen(ideallim,observed,linelen);
        bool badfunclen = infunc ? false : CheckBadfunclen(ideallim,observed,funclen);
        bool badline=false;
        if (badfunclen && (observed.nlongfunc > customlim.nlongfunc || observed.longestfunc > customlim.longestfunc)) {
            badline=true;
            show_workaround=true;
            prerr("src_lim.funclen_crazy"
                  <<Keyval("funclength",funclen)
                  <<Keyval("limit",ideallim.longestfunc)
                  <<Keyval("comment","Maximum function length exceeded."));
        }
        if (badlinelen && (observed.longestline > customlim.longestline || observed.nlongline > customlim.nlongline)) {
            badline=true;
            show_workaround=true;
            prerr("src_lim.not_uk"
                  <<Keyval("linelength",linelen)
                  <<Keyval("limit",ideallim.longestline)
                  <<Keyval("comment","Maximum line length exceeded."));
        }
        if (badmystery && (observed.nmysteryfunc > customlim.nmysteryfunc)) {
            badline=true;
            show_workaround=true;
            prerr("src_lim.voynich_manuscript"
                  <<Keyval("nmysteryfunc",observed.nmysteryfunc)
                  <<Keyval("limit",ideallim.nmysteryfunc)
                  <<Keyval("comment","Maximum # of global, undocumented functions exceeded."));
        }
        if (badws && (observed.nbadws > customlim.nbadws || observed.maxws > customlim.maxws)) {
            badline=true;
            show_workaround=true;
            prerr("src_lim.indentured_service"
                  <<Keyval("ws",ws)
                  <<Keyval("limit",ideallim.maxws)
                  <<Keyval("comment","Maximum indentation level exceeded."));
        }
        if (badline) {
            prerr(ideallim.gitfile<<":"<<ind_curs(line).i+1<<": "<<line);
            prerr("");
            nerr++;
        }
    }ind_end;
    if (show_workaround) {
        prerr("# NOTE: to bypass the error message, re-run update-linelim to re-baseline the change");
    }
    return nerr;
}

// -----------------------------------------------------------------------------

static void Main_Lim() {
    algo_lib::Regx include;
    Regx_ReadSql(include, "%.cpp", true);
    int nerr=0;
    ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) if (targsrc.select) {
        dev::Linelim ideallim;
        ideallim.gitfile=src_Get(targsrc);
        ideallim.maxws=16;
        ideallim.longestline=120;
        ideallim.longestfunc=60;
        // start observation with max. allowed values for maxws, longestline, longestfunc.
        // this is to avoid frequent and unnecessary changes to the linelim table when
        // longestfunc goes from 38 to 36 lines. any function under 60 lines is OK
        // so the value 60 is recorded.
        dev::Linelim observed(ideallim);// compute limit
        ideallim.nmysteryfunc=2;
        dev::Linelim customlim=GetCustomlim(targsrc,ideallim);// look up limit on this file
        bool bad=false;
        if (algo_lib::Regx_Match(include, ideallim.gitfile)) {
            bad = CheckLim(ideallim,customlim,observed);
        }
        nerr += bad;
        if (bad) {
            prerr("src_lim.badfile"
                  <<Keyval("gitfile",ideallim.gitfile));
            algo_lib::_db.exit_code=1;
        }
        // If any observation exceeds the ideal, emit a linelim entry to
        // save to file.
        if (observed.nlongline > 0
            || observed.nbadws > 0
            || observed.nlongfunc > 0
            || observed.nmysteryfunc > ideallim.nmysteryfunc) {
            Capture(observed,ideallim);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool SrcfileQ(src_lim::FGitfile &gitfile) {
    return ext_Get(gitfile) == "cpp";
}

static void StraySrc() {
    int nerr=0;
    ind_beg(src_lim::_db_gitfile_curs,gitfile,src_lim::_db) {
        verblog("src_lim.normcheck_srcfile"
                <<Keyval("gitfile",gitfile.gitfile)
                <<Keyval("targsrc", Bool(gitfile.c_targsrc!=0))
                <<Keyval("srcfileq", SrcfileQ(gitfile)));
        if (!gitfile.c_targsrc && SrcfileQ(gitfile)) {
            prlog("src_lim.stray_srcfile"
                  <<Keyval("fname",gitfile.gitfile)
                  <<Keyval("comment","Source file not part of any target (not in targsrc table)"));
            nerr++;
        }
    }ind_end;
    vrfy(nerr==0,"src_lim.stray_files  comment:'Please delete these files or add them to targsrc table'");
}

// -----------------------------------------------------------------------------

static void CheckPerms() {
    int nbad=0;
    ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) {
        struct stat st;
        algo::ZeroBytes(st);
        (void)stat(Zeroterm(tempstr()<<src_Get(targsrc)),&st);
        if (st.st_mode & 0111) {
            nbad++;
            prerr("src_lim.badperms"
                  <<Keyval("gitfile",src_Get(targsrc))
                  <<Keyval("comment","source files should not be executable"));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void StrayInclude() {
    command::abt abt;
    abt.target.expr = "%";
    abt.listincl =true;
    cstring retval(SysEval(abt_ToCmdline(abt),FailokQ(true),1024*1024*10,EchoQ(false)));
    ind_beg(Line_curs,line,retval) {
        src_lim::InsertStrptrMaybe(line);
    }ind_end;
    prlog("src_lim.stray_include_1"
          <<Keyval("n_include", src_lim::include_N())
          <<Keyval("n_gitfile", src_lim::gitfile_N()));
    int nerr=0;
    ind_beg(src_lim::_db_include_curs,include,src_lim::_db) if (!include.sys) {    // check that every include site refers to a versioned header.
        src_lim::FGitfile *hdrfile=src_lim::ind_gitfile_Find(filename_Get(include));
        if (!hdrfile) {
            prlog("src_lim.unversioned_include"
                  <<Keyval("srcfile",srcfile_Get(include))
                  <<Keyval("hdrfile",filename_Get(include))
                  <<Keyval("comment","Header file not versioned"));
            nerr++;
        } else {
            zd_include_Insert(*hdrfile,include);
        }
    }ind_end;
    // Check that every versioned include has at least one include site
    ind_beg(src_lim::_db_gitfile_curs,gitfile,src_lim::_db) {
        bool is_header = ext_Get(gitfile)=="h";
        bool no_sites = zd_include_EmptyQ(gitfile);
        bool not_extern = !StartsWithQ(gitfile.gitfile,"extern/");
        if (is_header && no_sites && not_extern) {
            prlog("src_lim.stray_include"
                  <<Keyval("fname",gitfile.gitfile)
                  <<Keyval("comment","Include file is not included anywhere."));
            nerr++;
        }
    }ind_end;
    vrfy(nerr==0,"src_lim.stray_files  comment:'Please delete these files or add them to targsrc table'");
}

// -----------------------------------------------------------------------------

static void WriteLineLim() {
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile, "src_lim");
    StringToFile(src_lim::_db.outtext, tempfile.filename);
    command::acr acr;
    acr.replace = true;
    acr.check = true;
    acr.trunc = true;
    acr.write = true;
    SysCmd(acr_ToCmdline(acr) << " <"<<tempfile.filename,FailokQ(false),DryrunQ(false));
}

static void Main_FinishCapture() {
    if (src_lim::_db.cmdline.capture) {
        if (src_lim::_db.cmdline.write) {
            WriteLineLim();
        } else {
            prlog(src_lim::_db.outtext);
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckBadline(src_lim::FTargsrc &targsrc) {
    MmapFile file;
    tempstr fname(src_Get(targsrc));
    MmapFile_Load(file,fname);
    ind_beg(algo::Line_curs, line, file.text) {
        ind_beg(src_lim::_db_badline_curs,badline,src_lim::_db) if (badline.select) {
            if (Regx_Match(badline.regx, line)
                && Regx_Match(badline._targsrc_regx,fname)// todo: move this out of the loop
                && FindStr(line, tempstr()<<"ignore:"<<badline.badline)==-1) {
                tempstr loc = tempstr() << fname<<":"<<ind_curs(line).i+1<<": ";
                prlog(loc << line);
                prlog("  "<<loc<<": src_lim.badline"
                      <<Keyval("badline",badline.badline)
                      <<Keyval("comment",tempstr()<<badline.comment));
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;
}

static void CheckBadline() {
    int nselect=0;
    ind_beg(src_lim::_db_badline_curs,badline,src_lim::_db) {
        badline.select = Regx_Match(src_lim::_db.cmdline.badline,badline.badline);
        nselect += badline.select;
        (void)Regx_ReadDflt(badline.regx, badline.expr);// full regx
        (void)Regx_ReadSql(badline._targsrc_regx, badline.targsrc_regx, true);// full regx
    }ind_end;
    if (nselect) {
        ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) if (targsrc.select) {
            CheckBadline(targsrc);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void src_lim::Main() {
    // select targsrc for processing
    algo_lib::Regx exclude;
    Regx_ReadSql(exclude, "(%/gen/%|extern/%)", true);
    ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) {
        targsrc.select = algo_lib::Regx_Match(src_lim::_db.cmdline.srcfile, src_Get(targsrc))
            && !algo_lib::Regx_Match(exclude, src_Get(targsrc));
    }ind_end;

    if (src_lim::_db.cmdline.linelim) {
        CheckPerms();
        Main_Lim();
    }
    if (src_lim::_db.cmdline.strayfile) {
        StraySrc();
        StrayInclude();
    }
    if (src_lim::_db.cmdline.badline.expr != "") {
        CheckBadline();
    }
    if (src_lim::_db.cmdline.capture) {
        Main_FinishCapture();
    }
}
